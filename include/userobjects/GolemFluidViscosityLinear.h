#ifndef GOLEMFLUIDVISCOSITYLINEAR_H
#define GOLEMFLUIDVISCOSITYLINEAR_H

#include "GolemFluidViscosity.h"

class GolemFluidViscosityLinear;

template <>
InputParameters validParams<GolemFluidViscosityLinear>();

class GolemFluidViscosityLinear : public GolemFluidViscosity
{
public:
  GolemFluidViscosityLinear(const InputParameters & parameters);
  Real computeViscosity(Real temperature, Real, Real mu0) const;
  Real computedViscositydT(Real temperature, Real, Real, Real mu0) const;
  Real computedViscositydp(Real, Real, Real) const;

private:
  Real _Tc;
  Real _Tv;
};

#endif // GOLEMFLUIDVISCOSITYLINEAR_H
