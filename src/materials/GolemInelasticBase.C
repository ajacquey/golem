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

#include "GolemInelasticBase.h"
#include "MooseMesh.h"

InputParameters
GolemInelasticBase::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription("Base class for radial return mapping. It should be inherited by the "
                             "differnet constitutive models.");
  params.addParam<std::string>(
      "base_name",
      "Optional parameter that enables to define multiple mechanics material on the same block.");
  params.set<bool>("compute") = false;
  params.suppressParameter<bool>("compute");
  return params;
}

GolemInelasticBase::GolemInelasticBase(const InputParameters & parameters)
  : Material(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : "")
{
}

void
GolemInelasticBase::setQp(unsigned int qp)
{
  _qp = qp;
}