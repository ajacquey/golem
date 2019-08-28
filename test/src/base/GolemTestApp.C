#include "GolemTestApp.h"
#include "GolemApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

template <>
InputParameters
validParams<GolemTestApp>()
{
  InputParameters params = validParams<GolemApp>();
  return params;
}

GolemTestApp::GolemTestApp(InputParameters parameters) : MooseApp(parameters)
{
  GolemTestApp::registerAll(_factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

GolemTestApp::~GolemTestApp() {}

void
GolemTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  GolemApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"GolemTestApp"});
    Registry::registerActionsTo(af, {"GolemTestApp"});
  }
}

void
GolemTestApp::registerApps()
{
  registerApp(GolemApp);
  registerApp(GolemTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
GolemTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  GolemTestApp::registerAll(f, af, s);
}
extern "C" void
GolemTestApp__registerApps()
{
  GolemTestApp::registerApps();
}