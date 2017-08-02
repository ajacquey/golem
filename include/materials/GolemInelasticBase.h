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

#ifndef GOLEMINELASTICBASE_H
#define GOLEMINELASTICBASE_H

#include "Conversion.h"
#include "GolemMaterialMElastic.h"

class GolemInelasticBase;

template <>
InputParameters validParams<GolemInelasticBase>();

class GolemInelasticBase : public Material
{
public:
  GolemInelasticBase(const InputParameters & parameters);
  virtual void updateStress(RankTwoTensor & strain_increment,
                            RankTwoTensor & inelastic_strain_increment,
                            RankTwoTensor & stress_new,
                            const RankTwoTensor & stress_old,
                            const RankFourTensor & elasticity_tensor,
                            bool compute_full_tangent_operator,
                            RankFourTensor & tangent_operator) = 0;
  void setQp(unsigned int qp);
  void resetQpProperties() final {}
  void resetProperties() final {}
protected:
  const std::string _base_name;
};

#endif // GOLEMINELASTICBASE_H