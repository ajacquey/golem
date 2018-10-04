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

#include "GolemFluidDensityIAPWS.h"
#include "GolemGlobals.h"

Real n_gibbs[34] = {
    0.14632971213167e0,    -0.84548187169114e0,  -0.37563603672040e1,   0.33855169168385e1,
    -0.95791963387872e0,   0.15772038513228e0,   -0.16616417199501e-1,  0.81214629983568e-3,
    0.28319080123804e-3,   -0.60706301565874e-3, -0.18990068218419e-1,  -0.32529748770505e-1,
    -0.21841717175414e-1,  -0.52838357969930e-4, -0.47184321073267e-3,  -0.30001780793026e-3,
    0.47661393906987e-4,   -0.44141845330846e-5, -0.72694996297594e-15, -0.31679644845054e-4,
    -0.28270797985312e-5,  -0.85205128120103e-9, -0.22425281908000e-5,  -0.65171222895601e-6,
    -0.14341729937924e-12, -0.40516996860117e-6, -0.12734301741641e-8,  -0.17424871230634e-9,
    -0.68762131295531e-18, 0.14478307828521e-19, 0.26335781662795e-22,  -0.11947622640071e-22,
    0.18228094581404e-23,  -0.93537087292458e-25};
int I_gibbs[34] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,  1,  1,  2,  2,  2,
                   2, 2, 3, 3, 3, 4, 4, 4, 5, 8, 8, 21, 23, 29, 30, 31, 32};
int J_gibbs[34] = {-2, -1, 0,  1, 2, 3,  4,  5,  -9, -7,  -1, 0,   1,   3,   -3,  0,   1,
                   3,  17, -4, 0, 6, -5, -2, 10, -8, -11, -6, -29, -31, -38, -39, -40, -41};

registerMooseObject("GolemApp", GolemFluidDensityIAPWS);

template <>
InputParameters
validParams<GolemFluidDensityIAPWS>()
{
  InputParameters params = validParams<GolemFluidDensity>();
  params.addClassDescription("IAPWS fluid density formulation for region 1.");
  params.addParam<bool>("has_kelvin", false, "Is the temperature in Kelvin?");
  return params;
}

GolemFluidDensityIAPWS::GolemFluidDensityIAPWS(const InputParameters & parameters)
  : GolemFluidDensity(parameters), _has_kelvin(getParam<bool>("has_kelvin"))
{
}

Real
GolemFluidDensityIAPWS::computeDensity(Real pressure, Real temperature, Real) const
{
  if (_has_scaled_properties)
  {
    pressure *= _scaling_uo->_s_stress;
    temperature *= _scaling_uo->_s_temperature;
  }
  Real rw = R / MH20;
  Real pi = pressure / PSTAR;
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real tau = TSTAR / temp_k;
  Real arg_pi = 7.1 - pi;
  Real arg_tau = tau - 1.222;
  Real gamma = 0.0;
  for (int i = 0; i < 34; ++i)
    gamma -=
        n_gibbs[i] * I_gibbs[i] * std::pow(arg_pi, I_gibbs[i] - 1) * std::pow(arg_tau, J_gibbs[i]);
  if (_has_scaled_properties)
    return (PSTAR / (rw * temp_k * gamma)) / _scaling_uo->_s_density;
  else
    return PSTAR / (rw * temp_k * gamma);
}

Real
GolemFluidDensityIAPWS::computedDensitydT(Real pressure, Real temperature, Real) const
{
  if (_has_scaled_properties)
  {
    pressure *= _scaling_uo->_s_stress;
    temperature *= _scaling_uo->_s_temperature;
  }
  Real rw = R / MH20;
  Real pi = pressure / PSTAR;
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real tau = TSTAR / temp_k;
  Real arg_pi = 7.1 - pi;
  Real arg_tau = tau - 1.222;
  Real gamma_pi = 0.0;
  Real gamma_pi_tau = 0.0;
  for (int i = 0; i < 34; ++i)
  {
    gamma_pi -=
        n_gibbs[i] * I_gibbs[i] * std::pow(arg_pi, I_gibbs[i] - 1) * std::pow(arg_tau, J_gibbs[i]);
    gamma_pi_tau -= n_gibbs[i] * I_gibbs[i] * std::pow(arg_pi, I_gibbs[i] - 1) * J_gibbs[i] *
                    std::pow(arg_tau, J_gibbs[i] - 1);
  }
  Real a = PSTAR / (rw * temp_k * temp_k * gamma_pi * gamma_pi);
  Real b = (gamma_pi - tau * gamma_pi_tau);
  if (_has_scaled_properties)
    return _scaling_uo->_s_temperature * (-a * b) / _scaling_uo->_s_density;
  else
    return -a * b;
}

Real
GolemFluidDensityIAPWS::computedDensitydp(Real pressure, Real temperature) const
{
  if (_has_scaled_properties)
  {
    pressure *= _scaling_uo->_s_stress;
    temperature *= _scaling_uo->_s_temperature;
  }
  Real rw = R / MH20;
  Real pi = pressure / PSTAR;
  Real temp_k = _has_kelvin ? temperature : temperature + KELVIN;
  Real tau = TSTAR / temp_k;
  Real arg_pi = 7.1 - pi;
  Real arg_tau = tau - 1.222;
  Real gamma_pi = 0.0;
  Real gamma_pi_pi = 0.0;
  for (int i = 0; i < 34; ++i)
  {
    gamma_pi -=
        n_gibbs[i] * I_gibbs[i] * std::pow(arg_pi, I_gibbs[i] - 1) * std::pow(arg_tau, J_gibbs[i]);
    gamma_pi_pi += n_gibbs[i] * I_gibbs[i] * (I_gibbs[i] - 1) * std::pow(arg_pi, I_gibbs[i] - 2) *
                   std::pow(arg_tau, J_gibbs[i]);
  }
  if (_has_scaled_properties)
    return _scaling_uo->_s_stress * (-gamma_pi_pi / (rw * temp_k * gamma_pi * gamma_pi)) /
           _scaling_uo->_s_density;
  else
    return -gamma_pi_pi / (rw * temp_k * gamma_pi * gamma_pi);
}