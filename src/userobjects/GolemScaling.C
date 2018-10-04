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

#include "GolemScaling.h"
#include "MooseError.h"

registerMooseObject("GolemApp", GolemScaling);

template <>
InputParameters
validParams<GolemScaling>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription(
      "Scales the variables and material properties by the characteristic values.");
  params.addRequiredParam<Real>("characteristic_time", "The characteristic time.");
  params.addRequiredParam<Real>("characteristic_length", "The characteristic length.");
  params.addRequiredParam<Real>("characteristic_temperature", "The characteristic temperature.");
  params.addRequiredParam<Real>("characteristic_stress", "The characteristic stress.");
  return params;
}

GolemScaling::GolemScaling(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _s_time(getParam<Real>("characteristic_time")),
    _s_length(getParam<Real>("characteristic_length")),
    _s_temperature(getParam<Real>("characteristic_temperature")),
    _s_stress(getParam<Real>("characteristic_stress"))
{
  Real area = _s_length * _s_length;
  Real volume = area * _s_length;
  Real squared_time = _s_time * _s_time;
  // Secondary variables
  _s_force = _s_stress * area;
  _s_energy = _s_force * _s_length;
  _s_power = _s_energy / _s_time;
  _s_velocity = _s_length / _s_time;
  _s_acceleration = _s_length / squared_time;
  _s_mass = _s_force / _s_acceleration;
  // Material properties - T
  _s_density = _s_mass / volume;
  _s_specific_heat = _s_energy / _s_mass / _s_temperature;
  _s_conductivity = _s_power / _s_length / _s_temperature;
  _s_heat_production = _s_power / volume;
  _s_heat_flow = _s_power / area;
  // Material properties - H
  _s_permeability = area;
  _s_viscosity = _s_stress * _s_time;
  _s_compressibility = 1.0 / _s_stress;
  // Material properties - M (plus compressibility)
  _s_expansivity = 1.0 / _s_temperature;
}