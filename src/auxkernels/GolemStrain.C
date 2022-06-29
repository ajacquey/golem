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

#include "GolemStrain.h"

registerMooseObject("GolemApp", GolemStrain);

InputParameters
GolemStrain::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription(
      "Access a component of the strain (total, inelastic or plastic) tensor.");
  params.addParam<MooseEnum>("strain_type",
                             GolemStrain::strainType() = "total",
                             "The component of the strain tensor to output.");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "index_i",
      "index_i >= 0 & index_i <= 2",
      "The index i of ij for the stress tensor (0, 1, 2)");
  params.addRequiredRangeCheckedParam<unsigned int>(
      "index_j",
      "index_j >= 0 & index_j <= 2",
      "The index j of ij for the stress tensor (0, 1, 2)");
  return params;
}

GolemStrain::GolemStrain(const InputParameters & parameters)
  : AuxKernel(parameters),
    _strain_type(getParam<MooseEnum>("strain_type")),
    _i(getParam<unsigned int>("index_i")),
    _j(getParam<unsigned int>("index_j"))
{
  switch (_strain_type)
  {
    case 1:
      _strain = &getMaterialProperty<RankTwoTensor>("mechanical_strain");
      break;
    case 2:
      _strain = &getMaterialProperty<RankTwoTensor>("inelastic_strain");
      break;
    case 3:
      _strain = &getMaterialProperty<RankTwoTensor>("plastic_strain");
      break;
  }
}

MooseEnum
GolemStrain::strainType()
{
  return MooseEnum("total=1 inelastic=2 plastic=3");
}

Real
GolemStrain::computeValue()
{
  return (*_strain)[_qp](_i, _j);
}