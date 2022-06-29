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

#include "GolemPressureBC.h"
#include "Function.h"
#include "GolemScaling.h"

registerMooseObject("GolemApp", GolemPressureBC);

InputParameters
GolemPressureBC::validParams()
{
  InputParameters params = NeumannBC::validParams();
  params.addClassDescription("Applies a pressure on a given boundary in a given direction.");
  params.addRequiredParam<unsigned int>("component", "The component for the pressure.");
  params.addParam<FunctionName>("function", "The function that describes the pressure.");
  params.addParam<UserObjectName>("scaling_uo", "The name of the scaling user object.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

GolemPressureBC::GolemPressureBC(const InputParameters & parameters)
  : NeumannBC(parameters),
    _has_scaled_properties(isParamValid("scaling_uo") ? true : false),
    _component(getParam<unsigned int>("component")),
    _function(isParamValid("function") ? &getFunction("function") : NULL),
    _scaling_uo(_has_scaled_properties ? &getUserObject<GolemScaling>("scaling_uo") : NULL)
{
  if (_component > 2)
    mooseError("Invalid component given GolemPressureBC: ", _component, ".\n");
}

Real
GolemPressureBC::computeQpResidual()
{
  if (_has_scaled_properties)
  {
    if (isParamValid("function"))
      _scaled_value = _function->value(_t, Point()) / _scaling_uo->_s_stress;
    else
      _scaled_value = _value / _scaling_uo->_s_stress;
  }
  else
  {
    if (isParamValid("function"))
      _scaled_value = _function->value(_t, Point());
    else
      _scaled_value = _value;
  }
  return _scaled_value * (_normals[_qp](_component) * _test[_i][_qp]);
}

Real
GolemPressureBC::computeQpJacobian()
{
  return 0.0;
}