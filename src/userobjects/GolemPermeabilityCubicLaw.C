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

#include "GolemPermeabilityCubicLaw.h"
#include "libmesh/utility.h"

registerMooseObject("GolemApp", GolemPermeabilityCubicLaw);

InputParameters
GolemPermeabilityCubicLaw::validParams()
{
  InputParameters params = GolemPermeability::validParams();
  params.addClassDescription("Cubic law permeability formulation.");
  return params;
}

GolemPermeabilityCubicLaw::GolemPermeabilityCubicLaw(const InputParameters & parameters)
  : GolemPermeability(parameters)
{
}

std::vector<Real>
GolemPermeabilityCubicLaw::computePermeability(std::vector<Real> k0,
                                               Real,
                                               Real,
                                               Real aperture) const
{
  Real cl = Utility::pow<2>(aperture) / 8.0;
  for (unsigned int i = 0; i < k0.size(); ++i)
    k0[i] = cl;
  return k0;
}

std::vector<Real>
GolemPermeabilityCubicLaw::computedPermeabilitydev(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dev(k0.size(), 0.0);
  return dk_dev;
}

std::vector<Real>
GolemPermeabilityCubicLaw::computedPermeabilitydpf(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dpf(k0.size(), 0.0);
  return dk_dpf;
}

std::vector<Real>
GolemPermeabilityCubicLaw::computedPermeabilitydT(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dT(k0.size(), 0.0);
  return dk_dT;
}