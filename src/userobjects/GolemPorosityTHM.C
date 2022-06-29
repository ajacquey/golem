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

#include "GolemPorosityTHM.h"

registerMooseObject("GolemApp", GolemPorosityTHM);

InputParameters
GolemPorosityTHM::validParams()
{
  InputParameters params = GolemPorosity::validParams();
  params.addClassDescription("Porosity formulation for thermo-hydro-mechanical coupling.");
  return params;
}

GolemPorosityTHM::GolemPorosityTHM(const InputParameters & parameters) : GolemPorosity(parameters)
{
}

Real
GolemPorosityTHM::computePorosity(
    Real phi_old, Real dphi_dev, Real dphi_dpf, Real dphi_dT, Real dev, Real dpf, Real dT) const
{
  return phi_old + dphi_dev * dev + dphi_dpf * dpf + dphi_dT * dT;
}

Real
GolemPorosityTHM::computedPorositydev(Real phi_old, Real biot) const
{
  return (biot - phi_old);
}

Real
GolemPorosityTHM::computedPorositydpf(Real phi_old, Real biot, Real Ks) const
{
  return (biot - phi_old) / Ks;
}

Real
GolemPorosityTHM::computedPorositydT(Real phi_old, Real biot, Real beta_f, Real beta_s) const
{
  return phi_old * (1.0 - biot) * beta_f - biot * (1.0 - phi_old) * beta_s;
}