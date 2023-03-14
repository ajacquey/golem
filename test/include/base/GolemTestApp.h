#pragma once

#include "MooseApp.h"

class GolemTestApp : public MooseApp
{
public:
  static InputParameters validParams();
  GolemTestApp(InputParameters parameters);
  virtual ~GolemTestApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs = false);
};
