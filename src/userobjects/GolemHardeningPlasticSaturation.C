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

#include "GolemHardeningPlasticSaturation.h"
#include "libmesh/utility.h"

template <>
InputParameters
validParams<GolemHardeningPlasticSaturation>()
{
  InputParameters params = validParams<GolemHardeningModel>();
  params.addClassDescription("Plastic saturation hardening class.");
  params.addRequiredParam<Real>(
      "value_initial", "The value of the parameter for all internal_parameter <= internal_0.");
  params.addRequiredParam<Real>(
      "value_residual", "The value of the parameter for internal_parameter >= internal_limit.");
  params.addParam<Real>(
      "internal_limit", 1.0, "The value of the internal_parameter when hardening saturates.");
  return params;
}

GolemHardeningPlasticSaturation::GolemHardeningPlasticSaturation(const InputParameters & parameters)
  : GolemHardeningModel(parameters),
    _val_ini(_is_radians ? getParam<Real>("value_initial") * libMesh::pi / 180.
                         : getParam<Real>("value_initial")),
    _val_res(_is_radians ? getParam<Real>("value_residual") * libMesh::pi / 180.
                         : getParam<Real>("value_residual")),
    _intnl_lim(getParam<Real>("internal_limit"))
{
  if (_val_ini < 0.0)
    mooseError("GolemHardeningPlasticSaturation: _val_ini must be greater than 0.0!");
  if (_intnl_lim <= 0.0)
    mooseError("GolemHardeningPlasticSaturation: internal_limit must be greater than 0.0!");
}

Real
GolemHardeningPlasticSaturation::value(Real intnl) const
{
  if (intnl <= _intnl_lim)
    return _val_ini +
           (_val_res - _val_ini) * intnl / _intnl_lim *
               (Utility::pow<2>(intnl / _intnl_lim) - 3.0 * intnl / _intnl_lim + 3);
  else
    return _val_res;
}

Real
GolemHardeningPlasticSaturation::dvalue(Real intnl) const
{
  if (intnl <= _intnl_lim)
    return 3.0 * (_val_res - _val_ini) / _intnl_lim *
           (Utility::pow<2>(intnl / _intnl_lim) - 2.0 * intnl / _intnl_lim + 1);
  else
    return 0.0;
}