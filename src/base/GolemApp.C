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

InputParameters
GolemApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

GolemApp::GolemApp(InputParameters parameters) : MooseApp(parameters)
{
  GolemApp::registerAll(_factory, _action_factory, _syntax);
}

GolemApp::~GolemApp() {}

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
  registerSyntax("EmptyAction", "BCs/GolemPressure");
  registerSyntax("GolemPressureAction", "BCs/GolemPressure/*");
}

void
GolemApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)	
{
  Registry::registerObjectsTo(f, {"GolemApp"});
  Registry::registerActionsTo(af, {"GolemApp"});
  associateSyntaxInner(s, af);
}

void
GolemApp::registerApps()
{
  registerApp(GolemApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
GolemApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  GolemApp::registerAll(f, af, s);
}
extern "C" void
GolemApp__registerApps()
{
  GolemApp::registerApps();
}