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

#include "GolemHardeningCubic.h"
#include "libmesh/utility.h"

registerMooseObject("GolemApp", GolemHardeningCubic);

InputParameters
GolemHardeningCubic::validParams()
{
  InputParameters params = GolemHardeningModel::validParams();
  params.addClassDescription("Cubic hardening class.");
  params.addRequiredParam<Real>(
      "value_initial", "The value of the parameter for all internal_parameter <= internal_0.");
  params.addRequiredParam<Real>(
      "value_residual", "The value of the parameter for internal_parameter >= internal_limit.");
  params.addParam<Real>(
      "internal_0", 0.0, "The value of the internal_parameter when hardening begins.");
  params.addParam<Real>(
      "internal_limit", 1.0, "The value of the internal_parameter when hardening ends.");
  return params;
}

GolemHardeningCubic::GolemHardeningCubic(const InputParameters & parameters)
  : GolemHardeningModel(parameters),
    _val_ini(_is_radians ? getParam<Real>("value_initial") * libMesh::pi / 180.
                         : getParam<Real>("value_initial")),
    _val_res(_is_radians ? getParam<Real>("value_residual") * libMesh::pi / 180.
                         : getParam<Real>("value_residual")),
    _intnl_0(getParam<Real>("internal_0")),
    _intnl_lim(getParam<Real>("internal_limit")),
    _alpha(2 * (_val_ini - _val_res) / Utility::pow<3>(_intnl_lim - _intnl_0)),
    _beta(-3. / 2. * (_val_ini - _val_res) / (_intnl_lim - _intnl_0))
{
  if (_intnl_lim <= _intnl_0)
    mooseError("internal_limit must be greater than internal_0 in GolemHardeningCubic model!");
}

Real
GolemHardeningCubic::value(Real intnl) const
{
  Real x = intnl - _intnl_0 - 0.5 * (_intnl_lim - _intnl_0);
  if (intnl <= _intnl_0)
    return _val_ini;
  else if (intnl >= _intnl_lim)
    return _val_res;
  else
    return _alpha * Utility::pow<3>(x) + _beta * x + 0.5 * (_val_ini + _val_res);
}

Real
GolemHardeningCubic::dvalue(Real intnl) const
{
  Real x = intnl - _intnl_0 - 0.5 * (_intnl_lim - _intnl_0);
  if (intnl <= _intnl_0)
    return 0.0;
  else if (intnl >= _intnl_lim)
    return 0.0;
  else
    return 3 * _alpha * Utility::pow<2>(x) + _beta;
}