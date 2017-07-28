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

#include "GolemMaterialT.h"

template <>
InputParameters
validParams<GolemMaterialT>()
{
  InputParameters params = validParams<GolemMaterialBase>();
  params.addParam<bool>("has_heat_source_sink", false, "Has source/sink of temperature?");
  params.addRequiredParam<Real>("fluid_thermal_conductivity_initial",
                                "The fluid thermal conductivity [W/m/K].");
  params.addRequiredParam<Real>("solid_thermal_conductivity_initial",
                                "The solid thermal conductivity [W/m/K].");
  params.addParam<Real>("fluid_heat_capacity_initial", 0.0, "The fluid heat capacity [J/m^3/K].");
  params.addParam<Real>("solid_heat_capacity_initial", 0.0, "The solid heat capacity [J/m^3/K].");
  params.addParam<Real>("heat_source_sink", 0.0, "The heat source or sink [W/m^3].");
  params.addParam<UserObjectName>("fluid_density_uo", "The name of the fluid density user object.");
  params.addClassDescription(
      "This is the base class for thermal (conductive) Golem material properties.");
  return params;
}

GolemMaterialT::GolemMaterialT(const InputParameters & parameters)
  : GolemMaterialBase(parameters),
    _has_T_source_sink(getParam<bool>("has_heat_source_sink")),
    _lambda_f(getParam<Real>("fluid_thermal_conductivity_initial")),
    _lambda_s(getParam<Real>("solid_thermal_conductivity_initial")),
    _c_f(getParam<Real>("fluid_heat_capacity_initial")),
    _c_s(getParam<Real>("solid_heat_capacity_initial")),
    _T_source_sink(getParam<Real>("heat_source_sink")),
    _T_kernel_diff(declareProperty<Real>("T_kernel_diff"))
{
  // scale properties if required
  if (_has_scaled_properties)
  {
    _lambda_s /= _scaling_uo->_s_conductivity;
    _lambda_f /= _scaling_uo->_s_conductivity;
    _c_s /= _scaling_uo->_s_specific_heat;
    _c_f /= _scaling_uo->_s_specific_heat;
    _T_source_sink /= _scaling_uo->_s_heat_production;
  }
  if (_has_T_source_sink)
    _T_kernel_source = &declareProperty<Real>("T_kernel_source");
  if (_fe_problem.isTransient())
  {
    if (isParamValid("fluid_density_uo"))
      _fluid_density_uo = &getUserObject<GolemFluidDensity>("fluid_density_uo");
    else
      mooseError("Fluid Density User Object must be declared");
    _T_kernel_time = &declareProperty<Real>("T_kernel_time");
  }
}

void
GolemMaterialT::computeQpProperties()
{
  _scaling_factor[_qp] = computeQpScaling();
  _porosity[_qp] = _porosity_uo->computePorosity(_phi0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  _fluid_density[_qp] = _rho0_f;
  _T_kernel_diff[_qp] = (_porosity[_qp] * _lambda_f + (1 - _porosity[_qp]) * _lambda_s);
  if (_has_T_source_sink)
    (*_T_kernel_source)[_qp] = -1.0 * _T_source_sink;
  if (_fe_problem.isTransient())
  {
    _fluid_density[_qp] = _fluid_density_uo->computeDensity(0.0, 0.0, _rho0_f);
    (*_T_kernel_time)[_qp] =
        (_porosity[_qp] * _c_f * _fluid_density[_qp] + (1 - _porosity[_qp]) * _c_s * _rho0_s);
  }
}