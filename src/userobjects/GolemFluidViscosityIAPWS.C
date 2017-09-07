/******************************************************************************/
/*           GOLEM - Multiphysics of faulted geothermal reservoirs            */
/*                                                                            */
/*          Copyright (C) 2017 by Antoine B. Jacquey and Mauro Cacace         */
/*             GFZ Potsdam, German Research Centre for Geosciences            */
/*                                                                            */
/*    This program is free software: you can redistribute it and/or modify    */
/*    it under the terms of the GNU General Public License as published by    */
/*      the Free Software Foundation, either version 3 of the License, or     */
/*                     (at your option) any later version.                    */
/*                                                                            */
/*       This program is distributed in the hope that it will be useful,      */
/*       but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       */
/*                GNU General Public License for more details.                */
/*                                                                            */
/*      You should have received a copy of the GNU General Public License     */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>    */
/******************************************************************************/

#include "GolemFluidViscosityIAPWS.h"
#include "GolemGlobals.h"

int I_visc[21] = {0, 1, 2, 3, 0, 1, 2, 3, 5, 0, 1, 2, 3, 4, 0, 1, 0, 3, 4, 3, 5};
int J_visc[21] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 6, 6};
Real H0_visc[4] = {1.67752e0, 2.20462e0, 0.6366564e0, -0.241605e0};
Real H1_visc[21] = {
    5.20094e-01, 8.50895e-02, -1.08374e+00, -2.89555e-01, 2.22531e-01,  9.99115e-01,  1.88797e+00,
    1.26613e+00, 1.20573e-01, -2.81378e-01, -9.06851e-01, -7.72479e-01, -4.89837e-01, -2.57040e-01,
    1.61913e-01, 2.57399e-01, -3.25372e-02, 6.98452e-02,  8.72102e-03,  -4.35673e-03, -5.93264e-04};

template <>
InputParameters
validParams<GolemFluidViscosityIAPWS>()
{
  InputParameters params = validParams<GolemFluidViscosity>();
  params.addClassDescription("IAPWS fluid viscosity formulation for region 1.");
  params.addParam<bool>("has_kelvin", false, "Is the temperature in Kelvin?");
  return params;
}

GolemFluidViscosityIAPWS::GolemFluidViscosityIAPWS(const InputParameters & parameters)
  : GolemFluidViscosity(parameters), _has_kelvin(getParam<bool>("has_kelvin"))
{
}

Real
GolemFluidViscosityIAPWS::computeViscosity(Real temperature, Real rho, Real) const
{
  if (_has_scaled_properties)
  {
    rho *= _scaling_uo->_s_density;
    temperature *= _scaling_uo->_s_temperature;
  }
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real mu0 = GolemFluidViscosityIAPWS::mu0Region1(temp_k);
  Real mu1 = GolemFluidViscosityIAPWS::mu1Region1(temp_k, rho);
  Real enh = GolemFluidViscosityIAPWS::critical_enhancement();
  if (_has_scaled_properties)
    return (MUSTAR * mu0 * mu1 * enh) / _scaling_uo->_s_viscosity;
  else
    return MUSTAR * mu0 * mu1 * enh;
}

Real
GolemFluidViscosityIAPWS::computedViscositydT(Real temperature, Real rho, Real drho_dT, Real) const
{
  if (_has_scaled_properties)
  {
    rho *= _scaling_uo->_s_density;
    temperature *= _scaling_uo->_s_temperature;
    drho_dT *= _scaling_uo->_s_density / _scaling_uo->_s_temperature;
  }
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real mu0 = GolemFluidViscosityIAPWS::mu1Region1(temp_k, rho);
  Real dmu0 = GolemFluidViscosityIAPWS::dmu0dTRegion1(temp_k);
  Real mu1 = GolemFluidViscosityIAPWS::mu0Region1(temp_k);
  Real dmu1 = GolemFluidViscosityIAPWS::dmu1dTRegion1(temp_k, rho, drho_dT);
  if (_has_scaled_properties)
    return _scaling_uo->_s_temperature * (MUSTAR * (dmu0 * mu1 + dmu1 * mu0)) /
           _scaling_uo->_s_viscosity;
  else
    return MUSTAR * (dmu0 * mu1 + dmu1 * mu0);
}

Real
GolemFluidViscosityIAPWS::computedViscositydp(Real temperature, Real rho, Real drho_dp) const
{
  if (_has_scaled_properties)
  {
    rho *= _scaling_uo->_s_density;
    temperature *= _scaling_uo->_s_temperature;
    drho_dp *= _scaling_uo->_s_density / _scaling_uo->_s_stress;
  }
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real mu0 = GolemFluidViscosityIAPWS::mu0Region1(temp_k);
  Real dmu1 = GolemFluidViscosityIAPWS::dmu1dpRegion1(temp_k, rho, drho_dp);
  if (_has_scaled_properties)
    return _scaling_uo->_s_stress * (MUSTAR * dmu1 * mu0) / _scaling_uo->_s_viscosity;
  else
    return MUSTAR * dmu1 * mu0;
}

Real
GolemFluidViscosityIAPWS::mu0Region1(Real temp) const
{
  Real tau = temp / TCRIT;
  Real t0[4];
  Real sum = 0.0;
  t0[0] = 1.0;
  t0[1] = t0[0] / tau;
  t0[2] = t0[0] * t0[1];
  t0[3] = t0[2] * t0[1];
  for (unsigned int i = 0; i < 4; ++i)
    sum += H0_visc[i] * t0[i];
  return 1.0e2 * std::sqrt(tau) / sum;
}

Real
GolemFluidViscosityIAPWS::mu1Region1(Real temp, Real rho) const
{
  Real del = rho / DCRIT;
  Real one_on_tau = TCRIT / temp;
  Real t1[6];
  Real d1[7];
  Real sum = 0.0;
  t1[0] = 1.0;
  t1[1] = t1[0] * one_on_tau - 1.0;
  t1[2] = t1[1] * t1[1];
  t1[3] = t1[2] * t1[1];
  t1[4] = t1[3] * t1[1];
  t1[5] = t1[4] * t1[1];
  d1[0] = 1.0;
  d1[1] = del - d1[0];
  d1[2] = d1[1] * d1[1];
  d1[3] = d1[2] * d1[1];
  d1[4] = d1[3] * d1[1];
  d1[5] = d1[4] * d1[1];
  d1[6] = d1[5] * d1[1];
  for (unsigned int i = 0; i < 21; ++i)
    sum += t1[I_visc[i]] * H1_visc[i] * d1[J_visc[i]];
  return std::exp(del * sum);
}

Real
GolemFluidViscosityIAPWS::critical_enhancement() const
{
  return 1.0; // Still to be calculated
}

Real
GolemFluidViscosityIAPWS::dmu0dTRegion1(Real temp) const
{
  Real tau = temp / TCRIT;
  Real done_on_tau_dT = -1.0 * TCRIT / (temp * temp);
  Real t0[4];
  Real dt0[4];
  Real sum = 0.0;
  Real dsum = 0.0;
  t0[0] = 1.0;
  t0[1] = t0[0] / tau;
  t0[2] = t0[0] * t0[1];
  t0[3] = t0[2] * t0[1];
  dt0[0] = 0.0;
  dt0[1] = t0[0] * done_on_tau_dT;
  dt0[2] = dt0[0] * t0[1] + t0[0] * dt0[1];
  dt0[3] = dt0[2] * t0[1] + t0[2] * dt0[1];
  for (unsigned int i = 0; i < 4; ++i)
  {
    sum += H0_visc[i] * t0[i];
    dsum += H0_visc[i] * dt0[i];
  }
  Real a = 1.0e2 * std::sqrt(tau);
  Real da = 0.5e2 * std::sqrt(1 / (TCRIT * temp));
  return (da * sum + dsum * a) / (sum * sum);
}

Real
GolemFluidViscosityIAPWS::dmu1dTRegion1(Real temp, Real rho, Real drho_dT) const
{
  Real del = rho / DCRIT;
  Real ddel = drho_dT / DCRIT;
  Real one_on_tau = TCRIT / temp;
  Real done_on_1_tau_dT = -1.0 * TCRIT / (temp * temp);
  Real t1[6];
  Real dt1[6];
  Real d1[7];
  Real dd1[7];
  Real sum = 0.0;
  Real dsum = 0.0;
  t1[0] = 1.0;
  t1[1] = t1[0] * one_on_tau - 1.0;
  t1[2] = t1[1] * t1[1];
  t1[3] = t1[2] * t1[1];
  t1[4] = t1[3] * t1[1];
  t1[5] = t1[4] * t1[1];
  dt1[0] = 0.0;
  dt1[1] = t1[0] * done_on_1_tau_dT;
  dt1[2] = dt1[1] * t1[1] + t1[1] * dt1[1];
  dt1[3] = dt1[2] * t1[1] + t1[2] * dt1[1];
  dt1[4] = dt1[3] * t1[1] + t1[3] * dt1[1];
  dt1[5] = dt1[4] * t1[1] + t1[4] * dt1[1];
  d1[0] = 1.0;
  d1[1] = del - d1[0];
  d1[2] = d1[1] * d1[1];
  d1[3] = d1[2] * d1[1];
  d1[4] = d1[3] * d1[1];
  d1[5] = d1[4] * d1[1];
  d1[6] = d1[5] * d1[1];
  dd1[0] = 0.0;
  dd1[1] = ddel;
  dd1[2] = dd1[1] * d1[1] + d1[1] * dd1[1];
  dd1[3] = dd1[2] * d1[1] + d1[2] * dd1[1];
  dd1[4] = dd1[3] * d1[1] + d1[3] * dd1[1];
  dd1[5] = dd1[4] * d1[1] + d1[4] * dd1[1];
  dd1[6] = dd1[5] * d1[1] + d1[5] * dd1[1];
  for (unsigned int i = 0; i < 21; ++i)
  {
    sum += t1[I_visc[i]] * H1_visc[i] * d1[J_visc[i]];
    dsum += H1_visc[i] * (dt1[I_visc[i]] * d1[J_visc[i]] + t1[I_visc[i]] * dd1[J_visc[i]]);
  }
  Real arg = del * sum;
  Real darg = ddel * sum + del * dsum;
  return std::exp(arg) * darg;
}

Real
GolemFluidViscosityIAPWS::dmu1dpRegion1(Real temp, Real rho, Real drho_dp) const
{
  Real del = rho / DCRIT;
  Real ddel = drho_dp / DCRIT;
  Real one_on_tau = TCRIT / temp;
  Real t1[6];
  Real d1[7];
  Real dd1[7];
  Real sum = 0.0;
  Real dsum = 0.0;
  t1[0] = 1.0;
  t1[1] = t1[0] * one_on_tau - 1.0;
  t1[2] = t1[1] * t1[1];
  t1[3] = t1[2] * t1[1];
  t1[4] = t1[3] * t1[1];
  t1[5] = t1[4] * t1[1];
  d1[0] = 1.0;
  d1[1] = del - d1[0];
  d1[2] = d1[1] * d1[1];
  d1[3] = d1[2] * d1[1];
  d1[4] = d1[3] * d1[1];
  d1[5] = d1[4] * d1[1];
  d1[6] = d1[5] * d1[1];
  dd1[0] = 0.0;
  dd1[1] = ddel;
  dd1[2] = dd1[1] * d1[1] + d1[1] * dd1[1];
  dd1[3] = dd1[2] * d1[1] + d1[2] * dd1[1];
  dd1[4] = dd1[3] * d1[1] + d1[3] * dd1[1];
  dd1[5] = dd1[4] * d1[1] + d1[4] * dd1[1];
  dd1[6] = dd1[5] * d1[1] + d1[5] * dd1[1];
  for (unsigned int i = 0; i < 21; ++i)
  {
    sum += t1[I_visc[i]] * H1_visc[i] * d1[J_visc[i]];
    dsum += H1_visc[i] * t1[I_visc[i]] * dd1[J_visc[i]];
  }
  Real arg = del * sum;
  Real darg = ddel * sum + del * dsum;
  return std::exp(arg) * darg;
}