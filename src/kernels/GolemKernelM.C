/******************************************************************************/
/*           GOLEM - Multiphysics of faulted geothermal reservoirs            */
/*                                                                            */
/*          Copyright (C) 2017 by Antoine B. Jacquey and Mauro Cacace         */
/*             GFZ Potsdam, German Research Centre for Geosciences            */
/*                                                                            */
/*    This program is free software: you can redistribute it and/or modify    */
/*    it under the terms of the GNU General Public License as published by    */
/*      the Free Software Foundation, either version 3 of the License, or     */
/*                     (at your option) any later version.                    */
/*                                                                            */
/*       This program is distributed in the hope that it will be useful,      */
/*       but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       */
/*                GNU General Public License for more details.                */
/*                                                                            */
/*      You should have received a copy of the GNU General Public License     */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>    */
/******************************************************************************/

#include "GolemKernelM.h"
#include "MooseVariable.h"
#include "Assembly.h"
#include "MooseMesh.h"
#include "GolemM.h"
#include "libmesh/quadrature.h"

registerMooseObject("GolemApp", GolemKernelM);

template <>
InputParameters
validParams<GolemKernelM>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Kernel for conservation of momentum in terms of effective stress.");
  params.addRequiredParam<unsigned int>("component",
                                        "The direction this kernel acts onto (0=x, 1=y, 2=z).");
  params.addRequiredCoupledVar("displacements", "The displacement variables vector.");
  params.addCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("temperature", "The temperature variable.");
  params.set<bool>("use_displaced_mesh") = false;
  params.addParam<bool>(
      "use_finite_deform_jacobian", false, "Jacobian for corotational finite strain");
  return params;
}

GolemKernelM::GolemKernelM(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _has_pf(isCoupled("pore_pressure")),
    _has_T(isCoupled("temperature")),
    _use_finite_deform_jacobian(getParam<bool>("use_finite_deform_jacobian")),
    _stress(getMaterialProperty<RankTwoTensor>("stress")),
    _M_kernel_grav(getMaterialProperty<RealVectorValue>("M_kernel_grav")),
    _M_jacobian(getMaterialProperty<RankFourTensor>("M_jacobian")),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(3),
    _pf(_has_pf ? coupledValue("pore_pressure") : _zero),
    _pf_var(_has_pf ? coupled("pore_pressure") : 0),
    _biot(getDefaultMaterialProperty<Real>("biot_coefficient")),
    _T_var(_has_T ? coupled("temperature") : 0),
    _TM_jacobian(getDefaultMaterialProperty<RankTwoTensor>("TM_jacobian")),
    _dM_kernel_grav_dev(getDefaultMaterialProperty<RealVectorValue>("dM_kernel_grav_dev")),
    _dM_kernel_grav_dpf(getDefaultMaterialProperty<RealVectorValue>("dM_kernel_grav_dpf")),
    _dM_kernel_grav_dT(getDefaultMaterialProperty<RealVectorValue>("dM_kernel_grav_dT"))
{
  if (_ndisp != _mesh.dimension())
    mooseError("The number of displacement variables supplied must match the mesh dimension.");
  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);
  if (_use_finite_deform_jacobian)
  {
    _assembly_undisplaced = &_fe_problem.assembly(_tid);
    _grad_phi_undisplaced = &(*_assembly_undisplaced).gradPhi();
    _deformation_gradient = &getMaterialProperty<RankTwoTensor>("deformation_gradient");
    _deformation_gradient_old = &getMaterialPropertyOld<RankTwoTensor>("deformation_gradient");
    _rotation_increment = &getMaterialProperty<RankTwoTensor>("rotation_increment");
  }
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
Real
GolemKernelM::computeQpResidual()
{
  RealVectorValue stress_row = _stress[_qp].row(_component);
  stress_row(_component) -= _biot[_qp] * _pf[_qp];
  return stress_row * _grad_test[_i][_qp] + _M_kernel_grav[_qp](_component) * _test[_i][_qp];
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
void
GolemKernelM::computeJacobian()
{
  if (_use_finite_deform_jacobian)
  {
    _finite_deform_jacobian.resize(_qrule->n_points());
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      computeFiniteDeformJacobian();
    _fe_problem.prepareShapes(_var.number(), _tid);
  }
  Kernel::computeJacobian();
}

Real
GolemKernelM::computeQpJacobian()
{
  Real jac = 0.0;
  if (_use_finite_deform_jacobian)
    jac += GolemM::elasticJacobian(_finite_deform_jacobian[_qp],
                                   _component,
                                   _component,
                                   _grad_test[_i][_qp],
                                   (*_grad_phi_undisplaced)[_j][_qp]);
  else
    jac += GolemM::elasticJacobian(
        _M_jacobian[_qp], _component, _component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);

  jac += _dM_kernel_grav_dev[_qp](_component) * _grad_phi[_j][_qp](_component) * _test[_i][_qp];

  return jac;
}

/******************************************************************************/
/*                            OFF DIAGONAL JACOBIAN                           */
/******************************************************************************/
void
GolemKernelM::computeOffDiagJacobian(MooseVariableFEBase & jvar)
{
  size_t jvar_num = jvar.number();
  if (_use_finite_deform_jacobian)
  {
    _finite_deform_jacobian.resize(_qrule->n_points());
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      computeFiniteDeformJacobian();
    _fe_problem.prepareShapes(jvar_num, _tid);
  }
  Kernel::computeOffDiagJacobian(jvar);
}

Real
GolemKernelM::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 0;
  bool active(false);
  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
    {
      coupled_component = i;
      active = true;
      break;
    }
  if (active)
  {
    Real jac = 0.0;
    if (_use_finite_deform_jacobian)
      jac += GolemM::elasticJacobian(_finite_deform_jacobian[_qp],
                                     _component,
                                     coupled_component,
                                     _grad_test[_i][_qp],
                                     (*_grad_phi_undisplaced)[_j][_qp]);
    else
      jac += GolemM::elasticJacobian(
          _M_jacobian[_qp], _component, coupled_component, _grad_test[_i][_qp], _grad_phi[_j][_qp]);

    jac += _dM_kernel_grav_dev[_qp](_component) * _grad_phi[_j][_qp](coupled_component) *
           _test[_i][_qp];
    return jac;
  }

  if (_has_pf && jvar == _pf_var)
  {
    Real jac = 0.0;
    jac += -_biot[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp](_component);
    jac += _dM_kernel_grav_dpf[_qp](_component) * _phi[_j][_qp] * _test[_i][_qp];
    return jac;
  }
  else if (_has_T && jvar == _T_var)
  {
    Real jac = 0.0;
    jac += (_TM_jacobian[_qp].row(_component) * _grad_test[_i][_qp]) * _phi[_j][_qp];
    jac += _dM_kernel_grav_dT[_qp](_component) * _phi[_j][_qp] * _test[_i][_qp];
    return jac;
  }

  return 0.0;
}

void
GolemKernelM::computeFiniteDeformJacobian()
{
  const RankTwoTensor I(RankTwoTensor::initIdentity);
  const RankFourTensor II_ijkl = I.mixedProductIkJl(I);

  // Bring back to unrotated config
  const RankTwoTensor unrotated_stress =
      (*_rotation_increment)[_qp].transpose() * _stress[_qp] * (*_rotation_increment)[_qp];

  // Incremental deformation gradient Fhat
  const RankTwoTensor Fhat =
      (*_deformation_gradient)[_qp] * (*_deformation_gradient_old)[_qp].inverse();
  const RankTwoTensor Fhatinv = Fhat.inverse();

  const RankTwoTensor rot_times_stress = (*_rotation_increment)[_qp] * unrotated_stress;
  const RankFourTensor dstress_drot =
      I.mixedProductIkJl(rot_times_stress) + I.mixedProductJkIl(rot_times_stress);
  const RankFourTensor rot_rank_four =
      (*_rotation_increment)[_qp].mixedProductIkJl((*_rotation_increment)[_qp]);
  const RankFourTensor drot_dUhatinv = Fhat.mixedProductIkJl(I);

  const RankTwoTensor A = I - Fhatinv;

  // Ctilde = Chat^-1 - I
  const RankTwoTensor Ctilde = A * A.transpose() - A - A.transpose();
  const RankFourTensor dCtilde_dFhatinv =
      -I.mixedProductIkJl(A) - I.mixedProductJkIl(A) + II_ijkl + I.mixedProductJkIl(I);

  // Second order approximation of Uhat - consistent with strain increment definition
  // const RankTwoTensor Uhat = I - 0.5 * Ctilde - 3.0/8.0 * Ctilde * Ctilde;

  RankFourTensor dUhatinv_dCtilde =
      0.5 * II_ijkl - 1.0 / 8.0 * (I.mixedProductIkJl(Ctilde) + Ctilde.mixedProductIkJl(I));
  RankFourTensor drot_dFhatinv = drot_dUhatinv * dUhatinv_dCtilde * dCtilde_dFhatinv;

  drot_dFhatinv -= Fhat.mixedProductIkJl((*_rotation_increment)[_qp].transpose());
  _finite_deform_jacobian[_qp] = dstress_drot * drot_dFhatinv;

  const RankFourTensor dstrain_increment_dCtilde =
      -0.5 * II_ijkl + 0.25 * (I.mixedProductIkJl(Ctilde) + Ctilde.mixedProductIkJl(I));
  _finite_deform_jacobian[_qp] +=
      rot_rank_four * _M_jacobian[_qp] * dstrain_increment_dCtilde * dCtilde_dFhatinv;
  _finite_deform_jacobian[_qp] += Fhat.mixedProductJkIl(_stress[_qp]);

  const RankFourTensor dFhat_dFhatinv = -Fhat.mixedProductIkJl(Fhat.transpose());
  const RankTwoTensor dJ_dFhatinv = dFhat_dFhatinv.innerProductTranspose(Fhat.ddet());

  // Component from Jacobian derivative
  _finite_deform_jacobian[_qp] += _stress[_qp].outerProduct(dJ_dFhatinv);

  // Derivative of Fhatinv w.r.t. undisplaced coordinates
  const RankTwoTensor Finv = (*_deformation_gradient)[_qp].inverse();
  const RankFourTensor dFhatinv_dGradu = -Fhatinv.mixedProductIkJl(Finv.transpose());
  _finite_deform_jacobian[_qp] = _finite_deform_jacobian[_qp] * dFhatinv_dGradu;
}