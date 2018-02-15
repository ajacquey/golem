#ifndef GOLEMTESTAPP_H
#define GOLEMTESTAPP_H

#include "MooseApp.h"

class GolemTestApp;

template <>
InputParameters validParams<GolemTestApp>();

class GolemTestApp : public MooseApp
{
public:
  GolemTestApp(InputParameters parameters);
  virtual ~GolemTestApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* GOLEMTESTAPP_H */
