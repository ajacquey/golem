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

#ifndef GOLEMDIRACKERNELTH_H
#define GOLEMDIRACKERNELTH_H

#include "DiracKernel.h"

class GolemDiracKernelTH;
class Function;
class GolemScaling;

template <>
InputParameters validParams<GolemDiracKernelTH>();

class GolemDiracKernelTH : public DiracKernel
{
public:
  GolemDiracKernelTH(const InputParameters & parameters);
  static MooseEnum Type();
  virtual void addPoints();

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  bool _has_scaled_properties;
  Point _source_point;
  MooseEnum _source_type;
  Real _in_out_rate;
  Function * _function;
  Real _start_time;
  Real _end_time;
  const MaterialProperty<Real> & _scaling_factor;
  const MaterialProperty<Real> & _fluid_density;

private:
  const GolemScaling * _scaling_uo;
  Real _scale;
};

#endif // GOLEMDIRACKERNELTH_H