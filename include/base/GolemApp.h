#ifndef GOLEMAPP_H
#define GOLEMAPP_H

#include "MooseApp.h"

class GolemApp;

template <>
InputParameters validParams<GolemApp>();

class GolemApp : public MooseApp
{
public:
  GolemApp(InputParameters parameters);
  virtual ~GolemApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* GOLEMAPP_H */
