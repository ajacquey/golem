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

#include "GolemKernelTimeH.h"

template <>
InputParameters
validParams<GolemKernelTimeH>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addCoupledVar("temperature", "The temperature variable.");
  // params.addCoupledVar("brine_mass", "The brine_mass variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  params.addParam<bool>("has_density_coupling", false, "Has fluid density coupling?");
  return params;
}

GolemKernelTimeH::GolemKernelTimeH(const InputParameters & parameters)
  : DerivativeMaterialInterface<TimeDerivative>(parameters),
    _has_T(isCoupled("temperature")),
    _has_disp(isCoupled("displacements")),
    //  _has_brine(isCoupled("brine_mass")),
    _temp(_has_T ? coupledValue("temperature") : _zero),
    _temp_old(_has_T ? coupledValueOld("temperature") : _zero),
    //  _brine(_has_brine ? coupledValue("brine_mass") : _zero),
    //  _brine_old(_has_brine ? coupledValueOld("brine_mass") : _zero),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _H_kernel_time(getMaterialProperty<Real>("H_kernel_time")),
    _dH_kernel_time_dpf(getDefaultMaterialProperty<Real>("dH_kernel_time_dpf")),
    _dH_kernel_time_dT(getDefaultMaterialProperty<Real>("dH_kernel_time_dT")),
    _dH_kernel_time_dev(getDefaultMaterialProperty<Real>("dH_kernel_time_dev")),
    _temp_var(_has_T ? coupled("temperature") : zero),
    _ndisp(_has_disp ? coupledComponents("displacements") : 0),
    _disp_var(_ndisp)
//  _brine_var(_has_brine ? coupled("brine_mass") : zero)
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
GolemKernelTimeH::computeQpResidual()
{
  return _scaling_factor[_qp] * _H_kernel_time[_qp] * _u_dot[_qp] * _test[_i][_qp];
}

/******************************************************************************/
/*                          JACOBIAN                                          */
/******************************************************************************/
Real
GolemKernelTimeH::computeQpJacobian()
{
  Real jac =
      _scaling_factor[_qp] * _H_kernel_time[_qp] * _du_dot_du[_qp] * _phi[_j][_qp] * _test[_i][_qp];
  jac += _scaling_factor[_qp] * _dH_kernel_time_dpf[_qp] * _u_dot[_qp] * _phi[_j][_qp] *
         _test[_i][_qp];
  return jac;
}

/******************************************************************************/
/*                     OFF DIAGONAL JACOBIAN                                  */
/******************************************************************************/
Real
GolemKernelTimeH::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned i = 0; i < _ndisp; ++i)
    if (_has_disp && (jvar == _disp_var[i]))
      return _scaling_factor[_qp] * _dH_kernel_time_dev[_qp] * _u_dot[_qp] * _grad_phi[_j][_qp](i) *
             _test[_i][_qp];
  if ((_has_T && _has_disp) && (jvar == _temp_var))
    return _scaling_factor[_qp] * _dH_kernel_time_dT[_qp] * _u_dot[_qp] * _phi[_j][_qp] *
           _test[_i][_qp];
  else
    return 0.0;
}