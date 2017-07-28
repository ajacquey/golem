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

#include "GolemApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

// Materials
#include "GolemMaterialBase.h"
#include "GolemMaterialH.h"

// Kernels
#include "GolemKernelTimeH.h"
#include "GolemKernelH.h"

// UserObjects
#include "GolemFluidDensityConstant.h"
#include "GolemFluidViscosityConstant.h"
#include "GolemPorosityConstant.h"
#include "GolemPermeabilityConstant.h"

template <>
InputParameters
validParams<GolemApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

GolemApp::GolemApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  GolemApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  GolemApp::associateSyntax(_syntax, _action_factory);
}

GolemApp::~GolemApp() {}

// External entry point for dynamic application loading
extern "C" void
GolemApp__registerApps()
{
  GolemApp::registerApps();
}
void
GolemApp::registerApps()
{
  registerApp(GolemApp);
}

// External entry point for dynamic object registration
extern "C" void
GolemApp__registerObjects(Factory & factory)
{
  GolemApp::registerObjects(factory);
}
void
GolemApp::registerObjects(Factory & factory)
{
  // Materials
  registerMaterial(GolemMaterialBase);
  registerMaterial(GolemMaterialH);

  // Kernels
  registerKernel(GolemKernelTimeH);
  registerKernel(GolemKernelH);

  // UserObjects
  registerUserObject(GolemFluidDensityConstant);
  registerUserObject(GolemFluidViscosityConstant);
  registerUserObject(GolemPorosityConstant);
  registerUserObject(GolemPermeabilityConstant);
}

// External entry point for dynamic syntax association
extern "C" void
GolemApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  GolemApp::associateSyntax(syntax, action_factory);
}
void
GolemApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
