#include "GolemApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

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
