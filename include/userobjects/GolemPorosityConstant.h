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

#ifndef GOLEMPOROSITYCONSTANT_H
#define GOLEMPOROSITYCONSTANT_H

#include "GolemPorosity.h"

class GolemPorosityConstant;

template <>
InputParameters validParams<GolemPorosityConstant>();

class GolemPorosityConstant : public GolemPorosity
{
public:
  GolemPorosityConstant(const InputParameters & parameters);
  Real computePorosity(Real phi_old, Real, Real, Real, Real, Real, Real) const;
  Real computedPorositydev(Real, Real) const;
  Real computedPorositydpf(Real, Real, Real) const;
  Real computedPorositydT(Real, Real, Real, Real) const;
};

#endif // GOLEMPOROSITYCONSTANT_H