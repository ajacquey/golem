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

#include "GolemMaterialMElastic.h"

class GolemMaterialMInelastic : public GolemMaterialMElastic
{
public:
  static InputParameters validParams();
  GolemMaterialMInelastic(const InputParameters & parameters);
  virtual void initialSetup() override;

protected:
  virtual void initQpStatefulProperties();
  virtual void GolemStress();
  virtual void updateQpStress(RankTwoTensor & combined_inelastic_strain_increment);
  virtual void updateQpStressSingleModel(RankTwoTensor & combined_inelastic_strain_increment);
  virtual void computeAdmissibleState(unsigned model_number,
                                      RankTwoTensor & elastic_strain_increment,
                                      RankTwoTensor & inelastic_strain_increment,
                                      RankFourTensor & consistent_tangent_operator);
  virtual void computeQpJacobian(const std::vector<RankFourTensor> & consistent_tangent_operator);

  const unsigned int _max_its;
  const Real _relative_tolerance;
  const Real _absolute_tolerance;
  std::vector<GolemInelasticBase *> _models;
  MaterialProperty<RankTwoTensor> & _inelastic_strain;
  const MaterialProperty<RankTwoTensor> & _inelastic_strain_old;
  const enum class TangentOperatorEnum { elastic, nonlinear } _tangent_operator_type;
  const unsigned _num_models;
};