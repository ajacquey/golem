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

#include "GolemKernelH.h"
#include "libmesh/utility.h"

registerMooseObject("GolemApp", GolemKernelH);

template <>
InputParameters
validParams<GolemKernelH>()
{
  InputParameters params = validParams<Kernel>();
  params.addCoupledVar("temperature", "The temperature variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  params.addParam<bool>("has_boussinesq", false, "Has Boussinesq terms?");
  return params;
}

GolemKernelH::GolemKernelH(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _has_T(isCoupled("temperature")),
    _has_disp(isCoupled("displacements")),
    _has_boussinesq(getParam<bool>("has_boussinesq")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _H_kernel_grav(getMaterialProperty<RealVectorValue>("H_kernel_grav")),
    _H_kernel(getMaterialProperty<RankTwoTensor>("H_kernel")),
    _dH_kernel_grav_dpf(getDefaultMaterialProperty<RealVectorValue>("dH_kernel_grav_dpf")),
    _dH_kernel_grav_dT(getDefaultMaterialProperty<RealVectorValue>("dH_kernel_grav_dT")),
    _dH_kernel_dev(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dev")),
    _dH_kernel_dpf(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dpf")),
    _dH_kernel_dT(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dT")),
    _grad_temp(_has_boussinesq ? &coupledGradient("temperature") : NULL),
    _fluid_density(_has_boussinesq ? &getMaterialProperty<Real>("fluid_density") : NULL),
    _drho_dpf(_has_boussinesq ? &getMaterialProperty<Real>("drho_dp") : NULL),
    _drho_dT(_has_boussinesq ? &getMaterialProperty<Real>("drho_dT") : NULL),
    _T_var(_has_T ? coupled("temperature") : zero),
    _ndisp(_has_disp ? coupledComponents("displacements") : 0),
    _disp_var(_ndisp)
{
  if (_has_disp)
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = coupled("displacements", i);
  else
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = zero;
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
Real
GolemKernelH::computeQpResidual()
{
  RealVectorValue vel = _H_kernel[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);

  Real boussinesq = 0.0;
  if (_has_boussinesq)
    boussinesq += (vel / (*_fluid_density)[_qp]) *
                  ((*_drho_dpf)[_qp] * _grad_u[_qp] + (*_drho_dT)[_qp] * (*_grad_temp)[_qp]);
  return _scaling_factor[_qp] * (vel * _grad_test[_i][_qp] + boussinesq * _test[_i][_qp]);
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
Real
GolemKernelH::computeQpJacobian()
{
  Real jac = 0.0;

  RealVectorValue coeff_1 = _dH_kernel_dpf[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
  RealVectorValue coeff_2 = _H_kernel[_qp] * _dH_kernel_grav_dpf[_qp];
  RealVectorValue coeff_3 = _H_kernel[_qp] * _grad_phi[_j][_qp];
  RealVectorValue dvel_dpf = (coeff_1 + coeff_2) * _phi[_j][_qp] + coeff_3;

  jac += _scaling_factor[_qp] * dvel_dpf * _grad_test[_i][_qp];

  // jac +=
  //     _scaling_factor[_qp] * ((coeff_1 + coeff_2) * _phi[_j][_qp] + coeff_3) *
  //     _grad_test[_i][_qp];

  if (_has_boussinesq)
  {
    RealVectorValue vel = _H_kernel[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
    RealVectorValue boussinesq =
        (*_drho_dpf)[_qp] * _grad_u[_qp] + (*_drho_dT)[_qp] * (*_grad_temp)[_qp];
    Real dboussinesq_dpf = 0.0;
    // First term related to dvel_dpf
    dboussinesq_dpf += (dvel_dpf / (*_fluid_density)[_qp]) * boussinesq * _phi[_j][_qp];
    // Second term related to drho_dpf
    dboussinesq_dpf += -(vel * (*_drho_dpf)[_qp] / Utility::pow<2>((*_fluid_density)[_qp])) *
                       boussinesq * _phi[_j][_qp];
    // Third term related to dgrad_u_dpf
    dboussinesq_dpf += (vel / (*_fluid_density)[_qp]) * ((*_drho_dpf)[_qp] * _grad_phi[_j][_qp]);
    // // first term related to qd
    // RealVectorValue dqp_dp = (coeff_1 + coeff_2) * _phi[_j][_qp] + coeff_3;
    // RealVectorValue boussinesq =
    //     (*_drho_dpf)[_qp] * _grad_u[_qp] + (*_drho_dT)[_qp] * (*_grad_temp)[_qp];
    // jac += _scaling_factor[_qp] * (dqp_dp / (*_fluid_density)[_qp]) * boussinesq *
    // _test[_i][_qp];
    // // second term related to boussinesq
    // RealVectorValue qd = _H_kernel[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
    // RealVectorValue b_1 = ((*_drho_dpf)[_qp] / (*_fluid_density)[_qp]) * _grad_phi[_j][_qp];
    // RealVectorValue b_2 =
    //     (-1.0 * Utility::pow<2>((*_drho_dpf)[_qp] / (*_fluid_density)[_qp])) * _grad_u[_qp];
    // RealVectorValue b_3 = (-1.0 / Utility::pow<2>((*_fluid_density)[_qp])) * (*_drho_dpf)[_qp] *
    //                       (*_drho_dT)[_qp] * (*_grad_temp)[_qp];
    // jac += _scaling_factor[_qp] * qd * (b_1 + (b_2 + b_3) * _phi[_j][_qp]) * _test[_i][_qp];
    jac += _scaling_factor[_qp] * dboussinesq_dpf * _test[_i][_qp];
  }

  return jac;
}

/******************************************************************************/
/*                            OFF DIAGONAL JACOBIAN                           */
/******************************************************************************/
Real
GolemKernelH::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0.0;
  RealVectorValue coeff_1 = RealVectorValue();
  RealVectorValue coeff_2 = RealVectorValue();
  if ((jvar == _T_var) && _has_T)
  {
    coeff_1 = _dH_kernel_dT[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
    coeff_2 = _H_kernel[_qp] * _dH_kernel_grav_dT[_qp];
    RealVectorValue dvel_dT = (coeff_1 + coeff_2) * _phi[_j][_qp];

    jac += _scaling_factor[_qp] * dvel_dT * _grad_test[_i][_qp];

    if (_has_boussinesq)
    {
      RealVectorValue vel = _H_kernel[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
      RealVectorValue boussinesq =
          (*_drho_dpf)[_qp] * _grad_u[_qp] + (*_drho_dT)[_qp] * (*_grad_temp)[_qp];
      Real dboussinesq_dT = 0.0;
      // First term related to dvel_dT
      dboussinesq_dT += (dvel_dT / (*_fluid_density)[_qp]) * boussinesq * _phi[_j][_qp];
      // Second term related to drho_dT
      dboussinesq_dT += -(vel * (*_drho_dT)[_qp] / Utility::pow<2>((*_fluid_density)[_qp])) *
                        boussinesq * _phi[_j][_qp];
      // Third term related to dgrad_T_dpf
      dboussinesq_dT += (vel / (*_fluid_density)[_qp]) * ((*_drho_dT)[_qp] * _grad_phi[_j][_qp]);
      // // first term related to qd
      // RealVectorValue dqd_dT = coeff_1 + coeff_2;
      // RealVectorValue boussinesq =
      //     (*_drho_dpf)[_qp] * _grad_u[_qp] + (*_drho_dT)[_qp] * (*_grad_temp)[_qp];
      // jac += _scaling_factor[_qp] * (dqd_dT / (*_fluid_density)[_qp]) * boussinesq *
      // _phi[_j][_qp] *
      //        _test[_i][_qp];
      // // second term related to boussinesq
      // RealVectorValue qd = _H_kernel[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
      // RealVectorValue b_1 =
      //     (-1.0 * Utility::pow<2>((*_drho_dT)[_qp] / (*_fluid_density)[_qp])) *
      //     (*_grad_temp)[_qp];
      // RealVectorValue b_2 = (-1.0 / Utility::pow<2>((*_fluid_density)[_qp])) * (*_drho_dT)[_qp] *
      //                       (*_drho_dpf)[_qp] * _grad_u[_qp];
      // jac += _scaling_factor[_qp] * qd * (b_1 + b_2) * _phi[_j][_qp] * _test[_i][_qp];
      jac += _scaling_factor[_qp] * dboussinesq_dT * _test[_i][_qp];
    }
  }
  for (unsigned i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
    {
      coeff_1 = _dH_kernel_dev[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
      coeff_2 = RealVectorValue();
      jac +=
          _scaling_factor[_qp] * (coeff_1 + coeff_2) * _grad_phi[_j][_qp](i) * _grad_test[_i][_qp];
    }
  return jac;
}