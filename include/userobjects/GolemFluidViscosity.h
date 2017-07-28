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

#ifndef GOLEMFLUIDVISCOSITY_H
#define GOLEMFLUIDVISCOSITY_H

#include "GeneralUserObject.h"
#include "GolemScaling.h"

class GolemFluidViscosity;

template <>
InputParameters validParams<GolemFluidViscosity>();

class GolemFluidViscosity : public GeneralUserObject
{
public:
  GolemFluidViscosity(const InputParameters & parameters);
  void initialize() {}
  void execute() {}
  void finalize() {}
  virtual Real computeViscosity(Real temperature, Real rho, Real mu0) const = 0;
  virtual Real computedViscositydT(Real temperature, Real rho, Real drho_dT, Real mu0) const = 0;
  virtual Real computedViscositydp(Real temperature, Real rho, Real drho_dp) const = 0;

protected:
  bool _has_scaled_properties;
  const GolemScaling * _scaling_uo;
};

#endif // GOLEMFLUIDVISCOSITY_H