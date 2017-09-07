#ifndef GOLEMFLUIDDENSITYLINEAR_H
#define GOLEMFLUIDDENSITYLINEAR_H

#include "GolemFluidDensity.h"

class GolemFluidDensityLinear;

template <>
InputParameters validParams<GolemFluidDensityLinear>();

class GolemFluidDensityLinear : public GolemFluidDensity
{
public:
  GolemFluidDensityLinear(const InputParameters & parameters);
  Real computeDensity(Real, Real temperature, Real rho0) const;
  Real computedDensitydT(Real, Real, Real rho0) const;
  Real computedDensitydp(Real, Real) const;

private:
  Real _alpha;
  Real _Tc;
};

#endif // GOLEMFLUIDDENSITYLINEAR_H
