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

#include "GolemHardeningModel.h"

template <>
InputParameters
validParams<GolemHardeningModel>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription(
      "Hardening based virtual class. Override the virtual functions in your class.");
  params.addParam<bool>("convert_to_radians",
                        false,
                        "If true, the value you entered will be "
                        "multiplied by Pi/180 before passing to the "
                        "Plasticity algorithms.");
  return params;
}

GolemHardeningModel::GolemHardeningModel(const InputParameters & parameters)
  : GeneralUserObject(parameters), _is_radians(getParam<bool>("convert_to_radians"))
{
}