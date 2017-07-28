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

template <>
InputParameters
validParams<GolemKernelH>()
{
  InputParameters params = validParams<Kernel>();
  params.addCoupledVar("temperature", "The temperature variable.");
  // params.addCoupledVar("brine_mass", "The brine_mass variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  return params;
}

GolemKernelH::GolemKernelH(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _has_T(isCoupled("temperature")),
    //  _has_brine(isCoupled("brine_mass")),
    _has_disp(isCoupled("displacements")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _H_kernel_grav(getMaterialProperty<RealVectorValue>("H_kernel_grav")),
    _H_kernel(getMaterialProperty<RankTwoTensor>("H_kernel")),
    _dH_kernel_grav_dpf(getDefaultMaterialProperty<RealVectorValue>("dH_kernel_grav_dpf")),
    _dH_kernel_grav_dT(getDefaultMaterialProperty<RealVectorValue>("dH_kernel_grav_dT")),
    _dH_kernel_dev(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dev")),
    _dH_kernel_dpf(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dpf")),
    _dH_kernel_dT(getDefaultMaterialProperty<RankTwoTensor>("dH_kernel_dT")),
    //  _drho_dbrine_gravity(getDefaultMaterialProperty<RealVectorValue>("drho_dbrine_kernel_gravity")),
    //  _dmu_dbrine_kernel(getDefaultMaterialProperty<Real>("dmu_dbrine_kernel_H")),
    _T_var(_has_T ? coupled("temperature") : zero),
    //  _brine_var(_has_brine ? coupled("brine_mass") : zero),
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
  return _scaling_factor[_qp] * vel * _grad_test[_i][_qp];
}

/******************************************************************************/
/*                          JACOBIAN                                          */
/******************************************************************************/
Real
GolemKernelH::computeQpJacobian()
{
  RealVectorValue coeff_1 = _dH_kernel_dpf[_qp] * (_grad_u[_qp] + _H_kernel_grav[_qp]);
  RealVectorValue coeff_2 = _H_kernel[_qp] * _dH_kernel_grav_dpf[_qp];
  RealVectorValue coeff_3 = _H_kernel[_qp] * _grad_phi[_j][_qp];
  return _scaling_factor[_qp] * ((coeff_1 + coeff_2) * _phi[_j][_qp] + coeff_3) *
         _grad_test[_i][_qp];
}

/******************************************************************************/
/*                     OFF DIAGONAL JACOBIAN                                  */
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
    jac += _scaling_factor[_qp] * (coeff_1 + coeff_2) * _phi[_j][_qp] * _grad_test[_i][_qp];
  }
  // else if ((jvar == _brine_var) && _has_brine)
  // {
  // coeff_1 += _H_kernel[_qp] * (*_drho_dbrine_gravity)[_qp] * _grad_test[_i][_qp] * _phi[_j][_qp];
  // coeff_2 += (*_dmu_dbrine_kernel)[_qp] * vel * _grad_test[_i][_qp] * _phi[_j][_qp];
  // }
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