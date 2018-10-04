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

#include "GolemFluidViscosityLinear.h"

registerMooseObject("GolemApp", GolemFluidViscosityLinear);

template <>
InputParameters
validParams<GolemFluidViscosityLinear>()
{
  InputParameters params = validParams<GolemFluidViscosity>();
  params.addClassDescription("Linear polynomial fitting for fluid viscosity");
  params.addRequiredParam<Real>("Tc", "The first thermal coefficient");
  params.addRequiredParam<Real>("Tv", "The second thermal coefficient");
  return params;
}

GolemFluidViscosityLinear::GolemFluidViscosityLinear(const InputParameters & parameters)
  : GolemFluidViscosity(parameters), _Tc(getParam<Real>("Tc")), _Tv(getParam<Real>("Tv"))
{
}

Real
GolemFluidViscosityLinear::computeViscosity(Real temperature, Real, Real mu0) const
{
  Real Tc = _Tc;
  Real Tv = _Tv;
  if (_has_scaled_properties)
  {
    Tc /= _scaling_uo->_s_temperature;
    Tv /= _scaling_uo->_s_temperature;
  }
  return mu0 * std::exp(-(temperature - Tc) / Tv);
}

Real
GolemFluidViscosityLinear::computedViscositydT(Real temperature, Real, Real, Real mu0) const
{
  Real Tc = _Tc;
  Real Tv = _Tv;
  if (_has_scaled_properties)
  {
    Tc /= _scaling_uo->_s_temperature;
    Tv /= _scaling_uo->_s_temperature;
  }
  return (-1.0 / Tv) * GolemFluidViscosityLinear::computeViscosity(
                           temperature, 0.0, mu0); // mu0 * std::exp(-(temperature - Tc) / Tv);
}

Real GolemFluidViscosityLinear::computedViscositydp(Real, Real, Real) const { return 0.0; }
