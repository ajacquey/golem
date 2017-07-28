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

#ifndef GOLEMMATERIALH_H
#define GOLEMMATERIALH_H

#include "GolemMaterialBase.h"
#include "RankTwoTensor.h"

class GolemMaterialH;

template <>
InputParameters validParams<GolemMaterialH>();

class GolemMaterialH : public GolemMaterialBase
{
public:
  GolemMaterialH(const InputParameters & parameters);
  static MooseEnum permeabilityType();

protected:
  virtual void computeProperties();
  virtual void computeQpProperties();
  virtual void GolemPropertiesH();

  MooseEnum _permeability_type;
  std::vector<Real> _k0;
  Real _mu0;
  Real _Kf;
  MaterialProperty<std::vector<Real>> & _permeability;
  MaterialProperty<RealVectorValue> & _H_kernel_grav;
  MaterialProperty<RankTwoTensor> & _H_kernel;
  MaterialProperty<Real> * _H_kernel_time;
};

#endif // GOLEMMATERIALH_H