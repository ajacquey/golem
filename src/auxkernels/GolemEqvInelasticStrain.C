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

#include "GolemEqvInelasticStrain.h"

registerMooseObject("GolemApp", GolemEqvInelasticStrain);

template <>
InputParameters
validParams<GolemEqvInelasticStrain>()
{
  InputParameters params = validParams<AuxKernel>();
  return params;
}

GolemEqvInelasticStrain::GolemEqvInelasticStrain(const InputParameters & parameters)
  : AuxKernel(parameters),
    _inelastic_strain(getMaterialProperty<RankTwoTensor>("inelastic_strain")),
    _inelastic_strain_old(getMaterialPropertyOld<RankTwoTensor>("inelastic_strain"))
{
}

Real
GolemEqvInelasticStrain::computeValue()
{
  RankTwoTensor inelastic_strain_increment = _inelastic_strain[_qp] - _inelastic_strain_old[_qp];
  return _u_old[_qp] + std::sqrt(2.0 / 3.0) * inelastic_strain_increment.L2norm();
}