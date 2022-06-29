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

#include "AuxKernel.h"
#include "RankTwoTensor.h"

class GolemThermalStress : public AuxKernel
{
public:
  static InputParameters validParams();
  GolemThermalStress(const InputParameters & parameters);

protected:
  virtual Real computeValue();
  const VariableValue & _u_old;
  const VariableValue & _temp;
  const VariableValue * _temp_old;
  const unsigned int _i;
  const MaterialProperty<RankTwoTensor> & _TM_jacobian;
};