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

#include "GolemHardeningExponential.h"

template <>
InputParameters
validParams<GolemHardeningExponential>()
{
  InputParameters params = validParams<GolemHardeningModel>();
  params.addRequiredParam<Real>("value_initial",
                                "The value of the parameter at internal_parameter = 0.");
  params.addRequiredParam<Real>("value_residual",
                                "The value of the parameter at internal_parameter = infinity.");
  params.addParam<Real>("rate", 0, "The rate parameter for the exponential function.");
  params.addClassDescription("Exponential hardening model.");
  return params;
}

GolemHardeningExponential::GolemHardeningExponential(const InputParameters & parameters)
  : GolemHardeningModel(parameters),
    _val_ini(_is_radians ? getParam<Real>("value_initial") * libMesh::pi / 180.
                         : getParam<Real>("value_initial")),
    _val_res(_is_radians ? getParam<Real>("value_residual") * libMesh::pi / 180.
                         : getParam<Real>("value_residual")),
    _rate(getParam<Real>("rate"))
{
}

Real
GolemHardeningExponential::value(Real intnl) const
{
  return _val_res + (_val_ini - _val_res) * std::exp(-_rate * intnl);
}

Real
GolemHardeningExponential::dvalue(Real intnl) const
{
  return -_rate * (_val_ini - _val_res) * std::exp(-_rate * intnl);
}