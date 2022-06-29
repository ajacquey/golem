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

#include "GolemPressureAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

registerMooseAction("GolemApp",GolemPressureAction, "add_bc");

InputParameters
GolemPressureAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Set up pressure boundary conditions.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundary IDs from the mesh where the pressure will be applied.");
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addParam<FunctionName>("function", "The function that describes the pressure.");
  params.addParam<UserObjectName>("scaling_uo", "The name of the scaling user object.");
  return params;
}

GolemPressureAction::GolemPressureAction(const InputParameters & params) : Action(params) {}

void
GolemPressureAction::act()
{
  const std::string kernel_name = "GolemPressureBC";

  std::vector<NonlinearVariableName> displacements =
      getParam<std::vector<NonlinearVariableName>>("displacements");

  // Create pressure BCs
  for (unsigned int i = 0; i < displacements.size(); ++i)
  {
    // Create unique kernel name for each of the components
    std::string unique_kernel_name = kernel_name + "_" + _name + "_" + Moose::stringify(i);

    InputParameters params = _factory.getValidParams(kernel_name);
    params.applyParameters(parameters());
    params.set<bool>("use_displaced_mesh") = true;
    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = displacements[i];

    _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
  }
}