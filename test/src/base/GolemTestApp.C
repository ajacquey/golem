#include "GolemTestApp.h"
#include "GolemApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<GolemTestApp>()
{
  InputParameters params = validParams<GolemApp>();
  return params;
}

GolemTestApp::GolemTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  GolemApp::registerObjectDepends(_factory);
  GolemApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  GolemApp::associateSyntaxDepends(_syntax, _action_factory);
  GolemApp::associateSyntax(_syntax, _action_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    GolemTestApp::registerObjects(_factory);
    GolemTestApp::associateSyntax(_syntax, _action_factory);
  }
}

GolemTestApp::~GolemTestApp() {}

// External entry point for dynamic application loading
extern "C" void
GolemTestApp__registerApps()
{
  GolemTestApp::registerApps();
}
void
GolemTestApp::registerApps()
{
  registerApp(GolemApp);
  registerApp(GolemTestApp);
}

// External entry point for dynamic object registration
extern "C" void
GolemTestApp__registerObjects(Factory & factory)
{
  GolemTestApp::registerObjects(factory);
}
void
GolemTestApp::registerObjects(Factory & /*factory*/)
{
}

// External entry point for dynamic syntax association
extern "C" void
GolemTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  GolemTestApp::associateSyntax(syntax, action_factory);
}
void
GolemTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
