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

#include "GolemPermeabilityConstant.h"

template <>
InputParameters
validParams<GolemPermeabilityConstant>()
{
  InputParameters params = validParams<GolemPermeability>();
  params.addClassDescription("Constant Permeability formulation.");
  return params;
}

GolemPermeabilityConstant::GolemPermeabilityConstant(const InputParameters & parameters)
  : GolemPermeability(parameters)
{
}

std::vector<Real>
GolemPermeabilityConstant::computePermeability(std::vector<Real> k0, Real, Real, Real) const
{
  return k0;
}

std::vector<Real>
GolemPermeabilityConstant::computedPermeabilitydev(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dev(k0.size(), 0.0);
  return dk_dev;
}

std::vector<Real>
GolemPermeabilityConstant::computedPermeabilitydpf(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dpf(k0.size(), 0.0);
  return dk_dpf;
}

std::vector<Real>
GolemPermeabilityConstant::computedPermeabilitydT(std::vector<Real> k0, Real, Real, Real) const
{
  std::vector<Real> dk_dT(k0.size(), 0.0);
  return dk_dT;
}