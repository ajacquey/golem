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

#include "GolemDarcyVelocity.h"
#include "RankTwoTensor.h"

registerMooseObject("GolemApp", GolemDarcyVelocity);

template <>
InputParameters
validParams<GolemDarcyVelocity>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addRequiredParam<int>("component", "The darcy velocity component.");
  return params;
}

GolemDarcyVelocity::GolemDarcyVelocity(const InputParameters & parameters)
  : AuxKernel(parameters),
    _grad_pf(coupledGradient("pore_pressure")),
    _H_kernel(getMaterialProperty<RankTwoTensor>("H_kernel")),
    _H_kernel_grav(getMaterialProperty<RealVectorValue>("H_kernel_grav")),
    _component(getParam<int>("component"))
{
}

Real
GolemDarcyVelocity::computeValue()
{
  RealVectorValue dv = -_H_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  return dv(_component);
}