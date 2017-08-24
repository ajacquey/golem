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

#include "GolemPermeabilityKC.h"
#include "libmesh/utility.h"

template <>
InputParameters
validParams<GolemPermeabilityKC>()
{
  InputParameters params = validParams<GolemPermeability>();
  params.addClassDescription("Kozeny-Carman Permeability formulation");
  return params;
}

GolemPermeabilityKC::GolemPermeabilityKC(const InputParameters & parameters)
  : GolemPermeability(parameters)
{
}

std::vector<Real>
GolemPermeabilityKC::computePermeability(std::vector<Real> k0, Real phi0, Real porosity) const
{
  std::vector<Real> k;
  if (phi0 != 1.0)
  {
    std::vector<Real> A = k0;
    for (unsigned int i = 0; i < A.size(); ++i)
      A[i] *= Utility::pow<2>(1.0 - phi0) / Utility::pow<3>(phi0);

    k = A;
    for (unsigned int i = 0; i < k.size(); ++i)
      k[i] *= Utility::pow<3>(porosity) / Utility::pow<2>(1.0 - porosity);
  }
  else
  {
    k = k0;
  }

  return k;
}

std::vector<Real>
GolemPermeabilityKC::computedPermeabilitydev(std::vector<Real> k0,
                                             Real phi0,
                                             Real porosity,
                                             Real dphi_dev) const
{
  std::vector<Real> A = k0;
  for (unsigned int i = 0; i < A.size(); ++i)
    A[i] *= Utility::pow<2>(1.0 - phi0) / Utility::pow<3>(phi0);

  std::vector<Real> dk_dev = A;
  for (unsigned int i = 0; i < dk_dev.size(); ++i)
    dk_dev[i] *=
        Utility::pow<2>(porosity) * (3.0 - porosity) / Utility::pow<3>(1.0 - porosity) * dphi_dev;
  return dk_dev;
}

std::vector<Real>
GolemPermeabilityKC::computedPermeabilitydpf(std::vector<Real> k0,
                                             Real phi0,
                                             Real porosity,
                                             Real dphi_dpf) const
{
  std::vector<Real> A = k0;
  for (unsigned int i = 0; i < A.size(); ++i)
  {
    A[i] *= Utility::pow<2>(1.0 - phi0) / Utility::pow<3>(phi0);
  }

  std::vector<Real> dk_dpf = A;
  for (unsigned int i = 0; i < dk_dpf.size(); ++i)
  {
    dk_dpf[i] *=
        Utility::pow<2>(porosity) * (3.0 - porosity) / Utility::pow<3>(1.0 - porosity) * dphi_dpf;
  }

  return dk_dpf;
}

std::vector<Real>
GolemPermeabilityKC::computedPermeabilitydT(std::vector<Real> k0,
                                            Real phi0,
                                            Real porosity,
                                            Real dphi_dT) const
{
  std::vector<Real> A = k0;
  for (unsigned int i = 0; i < A.size(); ++i)
  {
    A[i] *= Utility::pow<2>(1.0 - phi0) / Utility::pow<3>(phi0);
  }

  std::vector<Real> dk_dT = A;
  for (unsigned int i = 0; i < dk_dT.size(); ++i)
  {
    dk_dT[i] *=
        Utility::pow<2>(porosity) * (3.0 - porosity) / Utility::pow<3>(1.0 - porosity) * dphi_dT;
  }

  return dk_dT;
}