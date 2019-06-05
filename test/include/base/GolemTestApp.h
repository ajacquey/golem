#pragma once

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
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs = false);
};
