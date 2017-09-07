#include "GolemFluidDensityLinear.h"

template <>
InputParameters
validParams<GolemFluidDensityLinear>()
{
  InputParameters params = validParams<GolemFluidDensity>();
  params.addClassDescription("Linear polynomial fitting for fluid density");
  params.addRequiredParam<Real>("alpha", "the thermal expansion coefficient");
  params.addRequiredParam<Real>("Tc", "The initial temperature");
  return params;
}

GolemFluidDensityLinear::GolemFluidDensityLinear(const InputParameters & parameters)
  : GolemFluidDensity(parameters), _alpha(getParam<Real>("alpha")), _Tc(getParam<Real>("Tc"))
{
}

Real
GolemFluidDensityLinear::computeDensity(Real, Real temperature, Real rho0) const
{
  Real alpha = _alpha;
  Real Tc = _Tc;
  if (_has_scaled_properties)
  {
    Tc /= _scaling_uo->_s_temperature;
    alpha /= _scaling_uo->_s_expansivity;
  }
  return rho0 * (1.0 - alpha * (temperature - _Tc));
}

Real
GolemFluidDensityLinear::computedDensitydT(Real, Real, Real rho0) const
{
  Real alpha = _alpha;
  if (_has_scaled_properties)
    alpha /= _scaling_uo->_s_expansivity;
  return -1.0 * rho0 * alpha;
}

Real GolemFluidDensityLinear::computedDensitydp(Real, Real) const { return 0.0; }
