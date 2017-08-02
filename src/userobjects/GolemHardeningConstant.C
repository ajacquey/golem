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

#include "GolemHardeningConstant.h"
#include "libmesh/utility.h"
#include <math.h>

template <>
InputParameters
validParams<GolemHardeningConstant>()
{
  InputParameters params = validParams<GolemHardeningModel>();
  params.addParam<Real>(
      "value", 1.0, "The value of the parameter (it is valid for all internal parameters).");
  params.addClassDescription("No hardening model.");
  return params;
}

GolemHardeningConstant::GolemHardeningConstant(const InputParameters & parameters)
  : GolemHardeningModel(parameters),
    _value(_is_radians ? getParam<Real>("value") * libMesh::pi / 180.0 : getParam<Real>("value"))
{
}

Real GolemHardeningConstant::value(Real) const { return _value; }

Real GolemHardeningConstant::dvalue(Real) const { return 0.0; }