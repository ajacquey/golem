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

#pragma once

#include "GolemMaterialBase.h"

class GolemMaterialT;

template <>
InputParameters validParams<GolemMaterialT>();

class GolemMaterialT : public GolemMaterialBase
{
public:
  GolemMaterialT(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();

  bool _has_T_source_sink;
  Real _lambda_f;
  Real _lambda_s;
  Real _c_f;
  Real _c_s;
  Real _T_source_sink;
  MaterialProperty<Real> & _T_kernel_diff;
  MaterialProperty<Real> * _T_kernel_source;
  MaterialProperty<Real> * _T_kernel_time;
};