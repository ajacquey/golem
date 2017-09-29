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

#ifndef GOLEMPERMEABILITYCONSTANT_H
#define GOLEMPERMEABILITYCONSTANT_H

#include "GolemPermeability.h"

class GolemPermeabilityConstant;

template <>
InputParameters validParams<GolemPermeabilityConstant>();

class GolemPermeabilityConstant : public GolemPermeability
{
public:
  GolemPermeabilityConstant(const InputParameters & parameters);
  std::vector<Real>
  computePermeability(std::vector<Real> k0, Real phi0, Real porosity, Real aperture) const;
  std::vector<Real>
  computedPermeabilitydev(std::vector<Real> k0, Real phi0, Real porosity, Real dphi_dev) const;
  std::vector<Real>
  computedPermeabilitydpf(std::vector<Real> k0, Real phi0, Real porosity, Real dphi_dpf) const;
  std::vector<Real>
  computedPermeabilitydT(std::vector<Real> k0, Real phi0, Real porosity, Real dphi_dT) const;
};

#endif // GOLEMPERMEABILITYCONSTANT_H