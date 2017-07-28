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

#ifndef GOLEMSCALING_H
#define GOLEMSCALING_H

#include "GeneralUserObject.h"

class GolemScaling;

template <>
InputParameters validParams<GolemScaling>();

class GolemScaling : public GeneralUserObject
{
public:
  GolemScaling(const InputParameters & parameters);
  virtual void initialize() {}
  virtual void execute() {}
  virtual void finalize() {}
  // Characteristic values
  Real _s_time;
  Real _s_length;
  Real _s_temperature;
  Real _s_stress;
  // Secondary variables
  Real _s_force;        // stress * area
  Real _s_energy;       // force * length
  Real _s_power;        // energy / time
  Real _s_velocity;     // length / time
  Real _s_acceleration; // lenght / time / time
  Real _s_mass;         // force / acceleration
  // Material parameters - T
  Real _s_density;         // mass / volume
  Real _s_specific_heat;   // energy / mass / temperature
  Real _s_conductivity;    // power * length * temperature
  Real _s_heat_production; // power / mass
  Real _s_heat_flow;       // power / area
  // Material parameters - H
  Real _s_permeability;    //
  Real _s_viscosity;       // stress * time
  Real _s_compressibility; // stress
  // Material parameters - M
  Real _s_expansivity; // 1 / temperature
};

#endif // GOLEMSCALING_H