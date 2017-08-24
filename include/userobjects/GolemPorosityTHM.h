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

#ifndef GOLEMPOROSITYTHM_H
#define GOLEMPOROSITYTHM_H

#include "GolemPorosity.h"

class GolemPorosityTHM;

template <>
InputParameters validParams<GolemPorosityTHM>();

class GolemPorosityTHM : public GolemPorosity
{
public:
  GolemPorosityTHM(const InputParameters & parameters);
  Real computePorosity(
      Real phi_old, Real dphi_dev, Real dphi_dpf, Real dphi_dT, Real dev, Real dpf, Real dT) const;
  Real computedPorositydev(Real phi_old, Real biot) const;
  Real computedPorositydpf(Real phi_old, Real biot, Real Ks) const;
  Real computedPorositydT(Real phi_old, Real biot, Real beta_f, Real beta_s) const;
};

#endif // GOLEMPOROSITYTHM_H