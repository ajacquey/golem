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

#include "GolemMaterialMInelastic.h"
#include "GolemInelasticBase.h"
#include "MooseException.h"

registerMooseObject("GolemApp", GolemMaterialMInelastic);

template <>
InputParameters
validParams<GolemMaterialMInelastic>()
{
  InputParameters params = validParams<GolemMaterialMElastic>();
  params.addClassDescription(
      "Base class for inelastic computation based on a classical radial return mapping algorithm.");
  params.addRangeCheckedParam<unsigned int>(
      "max_iterations", 30, "max_iterations>0", "Maximum number of stress update iterations.");
  params.addParam<Real>(
      "relative_tolerance", 1e-5, "Relative convergence criterion for stress update iterations.");
  params.addParam<Real>(
      "absolute_tolerance", 1e-5, "Absolute convergence criterion for stress update iterations.");
  params.addRequiredParam<std::vector<MaterialName>>(
      "inelastic_models",
      "The material objects used to calculate the inelastic correction to the stress.");
  MooseEnum tangent_operator("elastic nonlinear", "nonlinear");
  params.addParam<MooseEnum>("tangent_operator",
                             tangent_operator,
                             "Type of tangent operator to return. 'elastic': return "
                             "the elasticity tensor. 'nonlinear': return the full, "
                             "general consistent tangentoperator.");
  return params;
}

GolemMaterialMInelastic::GolemMaterialMInelastic(const InputParameters & parameters)
  : GolemMaterialMElastic(parameters),
    _max_its(getParam<unsigned int>("max_iterations")),
    _relative_tolerance(getParam<Real>("relative_tolerance")),
    _absolute_tolerance(getParam<Real>("absolute_tolerance")),
    _inelastic_strain(declareProperty<RankTwoTensor>("inelastic_strain")),
    _inelastic_strain_old(getMaterialPropertyOld<RankTwoTensor>("inelastic_strain")),
    _tangent_operator_type(getParam<MooseEnum>("tangent_operator").getEnum<TangentOperatorEnum>()),
    _num_models(getParam<std::vector<MaterialName>>("inelastic_models").size())
{
  if (_strain_model < 2)
    mooseError("GolemMaterialMInelastic: you need to use an incremental strain model!");

  std::vector<MaterialName> models = getParam<std::vector<MaterialName>>("inelastic_models");
  for (unsigned int i = 0; i < models.size(); ++i)
  {
    GolemInelasticBase * base = dynamic_cast<GolemInelasticBase *>(&getMaterialByName(models[i]));
    if (base)
      _models.push_back(base);
    else
      mooseError("GolemMaterialMInelastic: the inelastic model " + models[i] +
                 " is not compatible with the class.");
  }
}

void
GolemMaterialMInelastic::initQpStatefulProperties()
{
  GolemMaterialMElastic::initQpStatefulProperties();
  _inelastic_strain[_qp].zero();
}

void
GolemMaterialMInelastic::GolemStress()
{
  RankTwoTensor inelastic_strain_increment = RankTwoTensor();

  if (_num_models == 0)
  {
    _stress[_qp] = (*_stress_old)[_qp] + _Cijkl[_qp] * (*_strain_increment)[_qp];
    _M_jacobian[_qp] = _Cijkl[_qp];
  }
  else if (_num_models == 1)
    updateQpStressSingleModel(inelastic_strain_increment);
  else
    updateQpStress(inelastic_strain_increment);

  _inelastic_strain[_qp] = _inelastic_strain_old[_qp] + inelastic_strain_increment;

  if (_strain_model > 2)
  {
    _stress[_qp] =
        (*_rotation_increment)[_qp] * _stress[_qp] * (*_rotation_increment)[_qp].transpose();
    _inelastic_strain[_qp] = (*_rotation_increment)[_qp] * _inelastic_strain[_qp] *
                             (*_rotation_increment)[_qp].transpose();
  }

  if (_has_pf)
  {
    _porosity[_qp] += (1.0 - (*_biot)[_qp]) *
                      (_inelastic_strain[_qp].trace() - _inelastic_strain_old[_qp].trace());
    (*_vol_strain_rate)[_qp] +=
        (1.0 - (*_biot)[_qp]) *
        (_inelastic_strain[_qp].trace() - _inelastic_strain_old[_qp].trace()) / _dt;
  }
}

void
GolemMaterialMInelastic::updateQpStress(RankTwoTensor & combined_inelastic_strain_increment)
{
  RankTwoTensor elastic_strain_increment;
  Real l2norm_delta_stress, first_l2norm_delta_stress;

  unsigned int counter = 0;

  std::vector<RankFourTensor> consistent_tangent_operator;
  consistent_tangent_operator.resize(_num_models);

  std::vector<RankTwoTensor> inelastic_strain_increment;
  inelastic_strain_increment.resize(_num_models);
  for (unsigned i_mod = 0; i_mod < _num_models; ++i_mod)
    inelastic_strain_increment[i_mod].zero();

  RankTwoTensor stress_min, stress_max;

  do
  {
    for (unsigned i_mod = 0; i_mod < _num_models; ++i_mod)
    {
      _models[i_mod]->setQp(_qp);

      elastic_strain_increment = (*_strain_increment)[_qp];

      for (unsigned j_mod = 0; j_mod < _num_models; ++j_mod)
        if (i_mod != j_mod)
          elastic_strain_increment -= inelastic_strain_increment[j_mod];

      _stress[_qp] = (*_stress_old)[_qp] + _Cijkl[_qp] * elastic_strain_increment;

      computeAdmissibleState(i_mod,
                             elastic_strain_increment,
                             inelastic_strain_increment[i_mod],
                             consistent_tangent_operator[i_mod]);

      if (i_mod == 0)
      {
        stress_max = _stress[_qp];
        stress_min = _stress[_qp];
      }
      else
      {
        for (unsigned int i = 0; i < LIBMESH_DIM; ++i)
          for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
            if (_stress[_qp](i, j) > stress_max(i, j))
              stress_max(i, j) = _stress[_qp](i, j);
            else if (stress_min(i, j) > _stress[_qp](i, j))
              stress_min(i, j) = _stress[_qp](i, j);
      }
    }

    RankTwoTensor delta_stress(stress_max - stress_min);
    l2norm_delta_stress = delta_stress.L2norm();
    if (counter == 0)
      first_l2norm_delta_stress = l2norm_delta_stress;

    ++counter;
  } while (_num_models != 1 && counter < _max_its && l2norm_delta_stress > _absolute_tolerance &&
           (l2norm_delta_stress / first_l2norm_delta_stress) > _relative_tolerance);

  if (counter == _max_its && l2norm_delta_stress > _absolute_tolerance &&
      (l2norm_delta_stress / first_l2norm_delta_stress) > _relative_tolerance)
    throw MooseException("GolemMaterialMInelastic: Max stress iteration hit!");

  combined_inelastic_strain_increment.zero();
  for (unsigned i_mod = 0; i_mod < _num_models; ++i_mod)
    combined_inelastic_strain_increment += inelastic_strain_increment[i_mod];

  computeQpJacobian(consistent_tangent_operator);
}

void
GolemMaterialMInelastic::updateQpStressSingleModel(
    RankTwoTensor & combined_inelastic_strain_increment)
{
  RankTwoTensor elastic_strain_increment = (*_strain_increment)[_qp];
  _models[0]->setQp(_qp);

  _stress[_qp] = (*_stress_old)[_qp] + _Cijkl[_qp] * elastic_strain_increment;

  computeAdmissibleState(
      0, elastic_strain_increment, combined_inelastic_strain_increment, _M_jacobian[_qp]);
}

void
GolemMaterialMInelastic::computeAdmissibleState(unsigned model_number,
                                                RankTwoTensor & elastic_strain_increment,
                                                RankTwoTensor & inelastic_strain_increment,
                                                RankFourTensor & consistent_tangent_operator)
{
  _models[model_number]->updateStress(elastic_strain_increment,
                                      inelastic_strain_increment,
                                      _stress[_qp],
                                      (*_stress_old)[_qp],
                                      _Cijkl[_qp],
                                      _tangent_operator_type == TangentOperatorEnum::nonlinear,
                                      consistent_tangent_operator);
}

void
GolemMaterialMInelastic::computeQpJacobian(
    const std::vector<RankFourTensor> & consistent_tangent_operator)
{
  if (_tangent_operator_type == TangentOperatorEnum::elastic)
    _M_jacobian[_qp] = _Cijkl[_qp];
  else
  {
    const RankFourTensor E_inv = _Cijkl[_qp].invSymm();
    _M_jacobian[_qp] = consistent_tangent_operator[0];
    for (unsigned i_mod = 1; i_mod < _num_models; ++i_mod)
      _M_jacobian[_qp] = consistent_tangent_operator[i_mod] * E_inv * _M_jacobian[_qp];
  }
}