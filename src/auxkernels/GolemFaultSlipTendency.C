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

#include "GolemFaultSlipTendency.h"
#include "Assembly.h"

registerMooseObject("GolemApp", GolemFaultSlipTendency);

InputParameters
GolemFaultSlipTendency::validParams()
{
  InputParameters params = AuxKernel::validParams();
  return params;
}

GolemFaultSlipTendency::GolemFaultSlipTendency(const InputParameters & parameters)
  : AuxKernel(parameters),
    _normals(_assembly.normals()),
    _stress(getMaterialProperty<RankTwoTensor>("stress"))
{
  if (!_bnd)
    paramError("boundary", "You need to provide a boundary for this AuxKernel!");
}

RealVectorValue
GolemFaultSlipTendency::computeFaultTraction()
{
  return _stress[_qp] * _normals[_qp];
}

Real
GolemFaultSlipTendency::computeValue()
{
  RealVectorValue fault_traction = computeFaultTraction();
  RealVectorValue shear_traction =
      fault_traction - (fault_traction * _normals[_qp]) * _normals[_qp];
  Real shear_stress = shear_traction.norm();
  Real normal_stress = -fault_traction * _normals[_qp];

  return shear_stress / normal_stress;
}