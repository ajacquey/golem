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

#include "GolemFluidDensityLinear.h"

registerMooseObject("GolemApp", GolemFluidDensityLinear);

template <>
InputParameters
validParams<GolemFluidDensityLinear>()
{
  InputParameters params = validParams<GolemFluidDensity>();
  params.addClassDescription("Linear polynomial fitting for fluid density");
  params.addRequiredParam<Real>("alpha", "the thermal expansion coefficient");
  params.addRequiredParam<Real>("Tc", "The initial temperature");
  return params;
}

GolemFluidDensityLinear::GolemFluidDensityLinear(const InputParameters & parameters)
  : GolemFluidDensity(parameters), _alpha(getParam<Real>("alpha")), _Tc(getParam<Real>("Tc"))
{
}

Real
GolemFluidDensityLinear::computeDensity(Real, Real temperature, Real rho0) const
{
  Real alpha = _alpha;
  Real Tc = _Tc;
  if (_has_scaled_properties)
  {
    Tc /= _scaling_uo->_s_temperature;
    alpha /= _scaling_uo->_s_expansivity;
  }
  return rho0 * (1.0 - alpha * (temperature - _Tc));
}

Real
GolemFluidDensityLinear::computedDensitydT(Real, Real, Real rho0) const
{
  Real alpha = _alpha;
  if (_has_scaled_properties)
    alpha /= _scaling_uo->_s_expansivity;
  return -1.0 * rho0 * alpha;
}

Real GolemFluidDensityLinear::computedDensitydp(Real, Real) const { return 0.0; }
