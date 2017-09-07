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

#ifndef GOLEMFLUIDVISCOSITYIAPWS_H
#define GOLEMFLUIDVISCOSITYIAPWS_H

#include "GolemFluidViscosity.h"

class GolemFluidViscosityIAPWS;

template <>
InputParameters validParams<GolemFluidViscosityIAPWS>();

class GolemFluidViscosityIAPWS : public GolemFluidViscosity
{
public:
  GolemFluidViscosityIAPWS(const InputParameters & parameters);
  Real computeViscosity(Real temperature, Real rho, Real) const;
  Real computedViscositydT(Real temperature, Real rho, Real drho_dT, Real) const;
  Real computedViscositydp(Real temperature, Real rho, Real drho_dT) const;

private:
  Real mu0Region1(Real temp) const;
  Real mu1Region1(Real temp, Real rho) const;
  Real critical_enhancement() const;
  Real dmu0dTRegion1(Real temp) const;
  Real dmu1dTRegion1(Real temp, Real rho, Real drho_dT) const;
  Real dmu1dpRegion1(Real temp, Real rho, Real drho_dp) const;

  bool _has_kelvin;
};

#endif // GOLEMFLUIDVISCOSITYIAPWS_H