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

#include "GolemKernelT.h"

template <>
InputParameters
validParams<GolemKernelT>()
{
  InputParameters params = validParams<Kernel>();
  params.addCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  return params;
}

GolemKernelT::GolemKernelT(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _has_pf(isCoupled("pore_pressure")),
    _has_disp(isCoupled("displacements")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _T_kernel_diff(getMaterialProperty<Real>("T_kernel_diff")),
    _dT_kernel_diff_dT(getDefaultMaterialProperty<Real>("dT_kernel_diff_dT")),
    _dT_kernel_diff_dpf(getDefaultMaterialProperty<Real>("dT_kernel_diff_dpf")),
    _dT_kernel_diff_dev(getDefaultMaterialProperty<Real>("dT_kernel_diff_dev")),
    _T_kernel_source(getDefaultMaterialProperty<Real>("T_kernel_source")),
    _pf_var(_has_pf ? coupled("pore_pressure") : zero),
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
GolemKernelT::computeQpResidual()
{
  Real res = _T_kernel_diff[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
  res += _T_kernel_source[_qp] * _test[_i][_qp];
  return _scaling_factor[_qp] * res;
}

/******************************************************************************/
/*                          JACOBIAN                                          */
/******************************************************************************/
Real
GolemKernelT::computeQpJacobian()
{
  Real jac = _T_kernel_diff[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
  jac += _dT_kernel_diff_dT[_qp] * _grad_u[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
  return _scaling_factor[_qp] * jac;
}

/******************************************************************************/
/*                     OFF DIAGONAL JACOBIAN                                  */
/******************************************************************************/
Real
GolemKernelT::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0.0;
  if (_has_pf && _has_disp)
  {
    if (jvar == _pf_var)
      jac += _grad_test[_i][_qp] * _dT_kernel_diff_dT[_qp] * _grad_u[_qp] * _phi[_j][_qp];

    for (unsigned i = 0; i < _ndisp; ++i)
      if (jvar == _disp_var[i])
        jac +=
            _grad_test[_i][_qp] * _dT_kernel_diff_dev[_qp] * _grad_u[_qp] * _grad_phi[_j][_qp](i);
  }
  return jac * _scaling_factor[_qp];
}