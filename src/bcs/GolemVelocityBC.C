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

#include "GolemVelocityBC.h"

registerMooseObject("GolemApp", GolemVelocityBC);

InputParameters
GolemVelocityBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();
  params.addRequiredParam<Real>("velocity", "Value of the velocity applied.");
  params.suppressParameter<bool>("preset");
  return params;
}

GolemVelocityBC::GolemVelocityBC(const InputParameters & parameters)
  : DirichletBCBase(parameters), _u_old(valueOld()), _velocity(getParam<Real>("velocity"))
{
}

Real
GolemVelocityBC::computeQpValue()
{
  return _u_old[_qp] + _velocity * _dt;
}