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

#include "GolemThermalStress.h"

registerMooseObject("GolemApp", GolemThermalStress);

template <>
InputParameters
validParams<GolemThermalStress>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("temperature", "The temperature");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "index", "index >= 0 & index <= 2", "The index i of ij for the tensor to output (0, 1, 2).");
  return params;
}

GolemThermalStress::GolemThermalStress(const InputParameters & parameters)
  : AuxKernel(parameters),
    _u_old(uOld()),
    _temp(coupledValue("temperature")),
    _i(getParam<unsigned int>("index")),
    _TM_jacobian(getMaterialProperty<RankTwoTensor>("TM_jacobian"))
{
  if (!_c_fe_problem.isTransient())
    mooseError("You cannot use GolemThermalStress in Steady simulations!");
  else
    _temp_old = &coupledValueOld("temperature");
}

Real
GolemThermalStress::computeValue()
{
  RankTwoTensor dstressT = _TM_jacobian[_qp] * (_temp[_qp] - (*_temp_old)[_qp]);
  return _u_old[_qp] + dstressT(_i, _i);
}