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

#ifndef GOLEMFLUIDDENSITYIAPWS_H
#define GOLEMFLUIDDENSITYIAPWS_H

#include "GolemFluidDensity.h"

class GolemFluidDensityIAPWS;

template <>
InputParameters validParams<GolemFluidDensityIAPWS>();

class GolemFluidDensityIAPWS : public GolemFluidDensity
{
public:
  GolemFluidDensityIAPWS(const InputParameters & parameters);
  Real computeDensity(Real pressure, Real temperature, Real) const;
  Real computedDensitydT(Real pressure, Real temperature, Real) const;
  Real computedDensitydp(Real pressure, Real temperature) const;

  bool _has_kelvin;
};

#endif // GOLEMFLUIDDENSITYIAPWS_H