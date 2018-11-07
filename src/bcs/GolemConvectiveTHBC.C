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

#include "GolemConvectiveTHBC.h"

registerMooseObject("GolemApp", GolemConvectiveTHBC);

template <>
InputParameters
validParams<GolemConvectiveTHBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addClassDescription(
      "Boundary condition for a zero-flux temperature when using conservative advection.");
  params.addRequiredCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  return params;
}

GolemConvectiveTHBC::GolemConvectiveTHBC(const InputParameters & parameters)
  : DerivativeMaterialInterface<IntegratedBC>(parameters),
    _has_disp(isCoupled("displacements")),
    _grad_pf(coupledGradient("pore_pressure")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _H_kernel_grav(getMaterialProperty<RealVectorValue>("H_kernel_grav")),
    _TH_kernel(getMaterialProperty<RankTwoTensor>("TH_kernel")),
    _T_kernel_diff(getMaterialProperty<Real>("T_kernel_diff")),
    _dTH_kernel_dT(getMaterialProperty<RankTwoTensor>("dTH_kernel_dT")),
    _dTH_kernel_dpf(getMaterialProperty<RankTwoTensor>("dTH_kernel_dpf")),
    _dH_kernel_grav_dT(getMaterialProperty<RealVectorValue>("dH_kernel_grav_dT")),
    _dH_kernel_grav_dpf(getMaterialProperty<RealVectorValue>("dH_kernel_grav_dpf")),
    _dT_kernel_diff_dT(getDefaultMaterialProperty<Real>("dT_kernel_diff_dT")),
    _dT_kernel_diff_dpf(getDefaultMaterialProperty<Real>("dT_kernel_diff_dpf")),
    _dT_kernel_diff_dev(getDefaultMaterialProperty<Real>("dT_kernel_diff_dev")),
    _p_var(coupled("pore_pressure")),
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

Real
GolemConvectiveTHBC::computeQpResidual()
{
  Real res = 0.0;
  RealVectorValue vel = _TH_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  res -= _T_kernel_diff[_qp] * (_grad_u[_qp] * _normals[_qp]) * _test[_i][_qp];
  res += _u[_qp] * (vel * _normals[_qp]) * _test[_i][_qp];
  return res * _scaling_factor[_qp];
}

Real
GolemConvectiveTHBC::computeQpJacobian()
{
  Real jac = 0.0;
  RealVectorValue vel = _TH_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  jac -= _T_kernel_diff[_qp] * (_grad_phi[_j][_qp] * _normals[_qp]) * _test[_i][_qp];
  jac -= _dT_kernel_diff_dT[_qp] * (_grad_u[_qp] * _normals[_qp]) * _phi[_j][_qp] * _test[_i][_qp];
  jac += _phi[_j][_qp] * (vel * _normals[_qp]) * _test[_i][_qp];
  jac += _u[_qp] * ((_dTH_kernel_dT[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp])) * _normals[_qp]) *
         _phi[_j][_qp] * _test[_i][_qp];
  jac += _u[_qp] * ((_TH_kernel[_qp] * (_grad_pf[_qp] + _dH_kernel_grav_dT[_qp])) * _normals[_qp]) *
         _phi[_j][_qp] * _test[_i][_qp];
  return jac * _scaling_factor[_qp];
}

Real
GolemConvectiveTHBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0.0;
  if (jvar == _p_var)
  {
    jac -=
        _dT_kernel_diff_dpf[_qp] * (_grad_u[_qp] * _normals[_qp]) * _phi[_j][_qp] * _test[_i][_qp];
    jac += _u[_qp] *
           ((_dTH_kernel_dpf[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp])) * _normals[_qp]) *
           _phi[_j][_qp] * _test[_i][_qp];
    jac += _u[_qp] * ((_TH_kernel[_qp] * _grad_phi[_j][_qp]) * _normals[_qp]) * _test[_i][_qp];
    jac += _u[_qp] *
           ((_TH_kernel[_qp] * (_grad_pf[_qp] + _dH_kernel_grav_dpf[_qp])) * _normals[_qp]) *
           _phi[_j][_qp] * _test[_i][_qp];
  }
  for (unsigned i = 0; i < _ndisp; ++i)
    if (_has_disp && (jvar == _disp_var[i]))
    {
      jac -= _dT_kernel_diff_dev[_qp] * (_grad_u[_qp] * _normals[_qp]) * _grad_phi[_j][_qp](i) *
             _test[_i][_qp];
    }
  return jac * _scaling_factor[_qp];
}
