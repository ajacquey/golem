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

#include "GolemDruckerPrager.h"
#include "libmesh/utility.h"

registerMooseObject("GolemApp", GolemDruckerPrager);

template <>
InputParameters
validParams<GolemDruckerPrager>()
{
  InputParameters params = validParams<GolemPQPlasticity>();
  params.addClassDescription("(Non)associated Drucker-Prager plasticity with smoothing of the cone "
                             "tip and with+/-out hardening");
  params.addParam<MooseEnum>("MC_interpolation_scheme",
                             GolemDruckerPrager::MC_interpolation_scheme() = "DP2",
                             "The Drucker-Prager approximation to Mohr-Coulomb.");
  params.addRangeCheckedParam<Real>(
      "smoother",
      0.0,
      "smoother>=0",
      "The cone vertex at J2=0 is smoothed. This is a non-hardening parameter");
  params.addRequiredParam<UserObjectName>(
      "MC_cohesion", "A UserObject that defines hardening of the Mohr-Coulomb cohesion");
  params.addRequiredParam<UserObjectName>(
      "MC_friction", "A UserObject that defines hardening of the Mohr-Coulomb friction angle");
  params.addRequiredParam<UserObjectName>(
      "MC_dilation", "A UserObject that defines hardening of the Mohr-Coulomb dilation angle");
  return params;
}

GolemDruckerPrager::GolemDruckerPrager(const InputParameters & parameters)
  : GolemPQPlasticity(parameters),
    _MC_cohesion(getUserObject<GolemHardeningModel>("MC_cohesion")),
    _MC_friction(getUserObject<GolemHardeningModel>("MC_friction")),
    _MC_dilation(getUserObject<GolemHardeningModel>("MC_dilation")),
    _MC_interpolation_scheme(getParam<MooseEnum>("MC_interpolation_scheme")),
    _no_hardening_cohesion(_MC_cohesion.modelName().compare("Constant") == 0),
    _no_hardening_friction(_MC_friction.modelName().compare("Constant") == 0),
    _no_hardening_dilation(_MC_dilation.modelName().compare("Constant") == 0),
    _smoother2(std::pow(getParam<Real>("smoother"), 2))
{
  if (_MC_friction.value(0.0) < 0.0 || _MC_dilation.value(0.0) < 0.0 ||
      _MC_friction.value(0.0) > libMesh::pi / 2.0 || _MC_dilation.value(0.0) > libMesh::pi / 2.0)
    mooseError("GolemDruckerPrager: both friction and dilation angles must lie in [0, Pi/2]!");
  if (_MC_friction.value(0.0) < _MC_dilation.value(0.0))
    mooseError("GolemDruckerPrager: friction angle should not be smaller than dilation angle");
  if (_MC_cohesion.value(0.0) < 0.0)
    mooseError("GolemDruckerPrager: cohesion should not be negative");
  initializeAB(0.0, _k, _alpha);
  initializeB(0.0, dilation, _beta);
}

MooseEnum
GolemDruckerPrager::MC_interpolation_scheme()
{
  return MooseEnum("DP1=1 DP2=2 DP3=3 DP4=4 DP5=5");
}

void
GolemDruckerPrager::computePQStress(const RankTwoTensor & stress, Real & p, Real & q) const
{
  p = stress.trace();
  q = std::sqrt(stress.secondInvariant());
}

void
GolemDruckerPrager::setEffectiveElasticity(const RankFourTensor & Eijkl,
                                           Real & Epp,
                                           Real & Eqq) const
{
  Epp = Eijkl.sum3x3();
  Eqq = Eijkl(0, 1, 0, 1);
}

RankTwoTensor
GolemDruckerPrager::dpdstress(const RankTwoTensor & stress) const
{
  return stress.dtrace();
}

RankFourTensor
GolemDruckerPrager::d2pdstress2(const RankTwoTensor & /*stress*/) const
{
  return RankFourTensor();
}

RankTwoTensor
GolemDruckerPrager::dqdstress(const RankTwoTensor & stress) const
{
  const Real j2 = stress.secondInvariant();
  if (j2 == 0.0)
    return RankTwoTensor();
  return 0.5 * stress.dsecondInvariant() / std::sqrt(j2);
}

RankFourTensor
GolemDruckerPrager::d2qdstress2(const RankTwoTensor & stress) const
{
  const Real j2 = stress.secondInvariant();
  if (j2 == 0.0)
    return RankFourTensor();

  const RankTwoTensor dj2 = stress.dsecondInvariant();
  return -0.25 * dj2.outerProduct(dj2) / std::pow(j2, 1.5) +
         0.5 * stress.d2secondInvariant() / std::sqrt(j2);
}

Real
GolemDruckerPrager::yieldFunctionValue(Real p, Real q, const Real & intnl) const
{
  Real k;
  Real alpha;
  bothAB(intnl, k, alpha);
  return std::sqrt(Utility::pow<2>(q) + _smoother2) + p * alpha - k;
}

Real
GolemDruckerPrager::dyieldFunction_dp(Real /*p*/, Real /*q*/, const Real & intnl) const
{
  Real k;
  Real alpha;
  bothAB(intnl, k, alpha);
  return alpha;
}

Real
GolemDruckerPrager::dyieldFunction_dq(Real /*p*/, Real q, const Real & /*intnl*/) const
{
  if (q == 0.0)
    return 0.0;
  else
    return q / std::sqrt(Utility::pow<2>(q) + _smoother2);
}

Real
GolemDruckerPrager::dyieldFunction_dintnl(Real p, Real /*q*/, const Real & intnl) const
{
  Real dk;
  Real dalpha;
  dbothAB(intnl, dk, dalpha);
  return p * dalpha - dk;
}

Real
GolemDruckerPrager::dflowPotential_dp(Real /*p*/, Real /*q*/, const Real & intnl) const
{
  Real beta;
  onlyB(intnl, dilation, beta);
  return beta;
}

Real
GolemDruckerPrager::dflowPotential_dq(Real p, Real q, const Real & intnl) const
{
  return dyieldFunction_dq(p, q, intnl);
}

Real
GolemDruckerPrager::d2flowPotential_dp2(Real /*p*/, Real /*q*/, const Real & /*intnl*/) const
{
  return 0.0;
}

Real
GolemDruckerPrager::d2flowPotential_dp_dq(Real /*p*/, Real /*q*/, const Real & /*intnl*/) const
{
  return 0.0;
}

Real
GolemDruckerPrager::d2flowPotential_dq2(Real /*p*/, Real q, const Real & /*intnl*/) const
{
  Real sqrt_q = std::sqrt(Utility::pow<2>(q) + _smoother2);
  if (q == 0.0)
    return 0.0;
  else
    return (1.0 - Utility::pow<2>(q) / (Utility::pow<2>(q) + _smoother2)) / sqrt_q;
}

Real
GolemDruckerPrager::d2flowPotential_dp_dintnl(Real /*p*/, Real /*q*/, const Real & intnl) const
{
  Real dbeta;
  donlyB(intnl, dilation, dbeta);
  return dbeta;
}

Real
GolemDruckerPrager::d2flowPotential_dq_dintnl(Real /*p*/, Real /*q*/, const Real & /*intnl*/) const
{
  return 0.0;
}

void
GolemDruckerPrager::setIntnlValues(
    Real /*p_trial*/, Real q_trial, Real /*p*/, Real q, const Real & intnl_old, Real & intnl) const
{
  intnl = intnl_old + (q_trial - q) / _Eqq;
}

void
GolemDruckerPrager::setIntnlDerivatives(Real /*p_trial*/,
                                        Real /*q_trial*/,
                                        Real /*p*/,
                                        Real /*q*/,
                                        const Real & /*intnl*/,
                                        std::vector<Real> & dintnl) const
{
  dintnl[0] = 0.0;
  dintnl[1] = -1.0 / _Eqq;
}

void
GolemDruckerPrager::bothAB(Real intnl, Real & k, Real & alpha) const
{
  if (_no_hardening_cohesion && _no_hardening_friction)
  {
    k = _k;
    alpha = _alpha;
    return;
  }
  initializeAB(intnl, k, alpha);
}

void
GolemDruckerPrager::onlyB(Real intnl, int f_or_d, Real & alpha_beta) const
{
  if (_no_hardening_friction && (f_or_d == friction))
  {
    alpha_beta = _alpha;
    return;
  }
  if (_no_hardening_dilation && (f_or_d == dilation))
  {
    alpha_beta = _beta;
    return;
  }
  initializeB(intnl, f_or_d, alpha_beta);
}

void
GolemDruckerPrager::initializeAB(Real intnl, Real & k, Real & alpha) const
{
  const Real C = _MC_cohesion.value(intnl);
  const Real cosphi = std::cos(_MC_friction.value(intnl));
  const Real sinphi = std::sin(_MC_friction.value(intnl));
  switch (_MC_interpolation_scheme)
  {
    case 1: // DP1 --> outer tip
      k = 2.0 * std::sqrt(3.0) * C * cosphi / (3.0 - sinphi);
      alpha = 2.0 * sinphi / std::sqrt(3.0) / (3.0 - sinphi);
      break;
    case 2: // DP2 --> lode_zero
      k = C * cosphi;
      alpha = sinphi / 3.0;
      break;
    case 3: // DP3 --> inner tip
      k = 2.0 * std::sqrt(3.0) * C * cosphi / (3.0 + sinphi);
      alpha = 2.0 * sinphi / std::sqrt(3.0) / (3.0 + sinphi);
      break;
    case 4: // DP4 --> inner edge
      k = 3.0 * C * cosphi / std::sqrt(9.0 + 3.0 * Utility::pow<2>(sinphi));
      alpha = sinphi / std::sqrt(9.0 + 3.0 * Utility::pow<2>(sinphi));
      break;
    case 5: // DP5 --> native
      k = C;
      alpha = sinphi / cosphi;
      break;
  }
}

void
GolemDruckerPrager::initializeB(Real intnl, int f_or_d, Real & alpha_beta) const
{
  const Real s = (f_or_d == friction) ? std::sin(_MC_friction.value(intnl))
                                      : std::sin(_MC_dilation.value(intnl));
  switch (_MC_interpolation_scheme)
  {
    case 1: // DP1 --> outer tip
      alpha_beta = 2.0 * s / std::sqrt(3.0) / (3.0 - s);
      break;
    case 2: // DP2 --> lode_zero
      alpha_beta = s / 3.0;
      break;
    case 3: // DP3 --> inner tip
      alpha_beta = 2.0 * s / std::sqrt(3.0) / (3.0 + s);
      break;
    case 4: // DP4 --> inner edge
      alpha_beta = s / std::sqrt(9.0 + 3.0 * Utility::pow<2>(s));
      break;
    case 5: // DP5 --> native
      const Real c = (f_or_d == friction) ? std::cos(_MC_friction.value(intnl))
                                          : std::cos(_MC_dilation.value(intnl));
      alpha_beta = s / c;
      break;
  }
}

void
GolemDruckerPrager::dbothAB(Real intnl, Real & dk, Real & dalpha) const
{
  if (_no_hardening_cohesion && _no_hardening_friction)
  {
    dk = 0.0;
    dalpha = 0.0;
    return;
  }
  const Real C = _MC_cohesion.value(intnl);
  const Real dC = _MC_cohesion.dvalue(intnl);
  const Real cosphi = std::cos(_MC_friction.value(intnl));
  const Real dcosphi = -std::sin(_MC_friction.value(intnl)) * _MC_friction.dvalue(intnl);
  const Real sinphi = std::sin(_MC_friction.value(intnl));
  const Real dsinphi = std::cos(_MC_friction.value(intnl)) * _MC_friction.dvalue(intnl);
  switch (_MC_interpolation_scheme)
  {
    case 1: // DP1 --> outer tip
      dk = 2.0 * std::sqrt(3.0) * (dC * cosphi / (3.0 - sinphi) + C * dcosphi / (3.0 - sinphi) +
                                   C * cosphi * dsinphi / Utility::pow<2>(3.0 - sinphi));
      dalpha = 2.0 / std::sqrt(3.0) *
               (dsinphi / (3.0 - sinphi) + sinphi * dsinphi / Utility::pow<2>(3.0 - sinphi));
      break;
    case 2: // DP2 --> lode_zero
      dk = dC * cosphi + C * dcosphi;
      dalpha = dsinphi / 3.0;
      break;
    case 3: // DP3 --> inner tip
      dk = 2.0 * std::sqrt(3.0) * (dC * cosphi / (3.0 + sinphi) + C * dcosphi / (3.0 + sinphi) -
                                   C * cosphi * dsinphi / Utility::pow<2>(3.0 + sinphi));
      dalpha = 2.0 / std::sqrt(3.0) *
               (dsinphi / (3.0 + sinphi) - sinphi * dsinphi / Utility::pow<2>(3.0 + sinphi));
      break;
    case 4: // DP4 --> inner edge
      dk = 3.0 * dC * cosphi / std::sqrt(9.0 + 3.0 * Utility::pow<2>(sinphi)) +
           3.0 * C * dcosphi / std::sqrt(9.0 + 3.0 * Utility::pow<2>(sinphi)) -
           3.0 * C * cosphi * 3.0 * sinphi * dsinphi /
               std::pow(9.0 + 3.0 * Utility::pow<2>(sinphi), 1.5);
      dalpha = dsinphi / std::sqrt(9.0 + 3.0 * Utility::pow<2>(sinphi)) -
               3.0 * sinphi * sinphi * dsinphi / std::pow(9.0 + 3.0 * Utility::pow<2>(sinphi), 1.5);
      break;
    case 5: // DP5 --> native
      dk = dC;
      dalpha = dsinphi / cosphi - sinphi * dcosphi / Utility::pow<2>(cosphi);
      break;
  }
}

void
GolemDruckerPrager::donlyB(Real intnl, int f_or_d, Real & dalpha_beta) const
{
  if (_no_hardening_friction && (f_or_d == friction))
  {
    dalpha_beta = 0.0;
    return;
  }
  if (_no_hardening_dilation && (f_or_d == dilation))
  {
    dalpha_beta = 0.0;
    return;
  }
  const Real s = (f_or_d == friction) ? std::sin(_MC_friction.value(intnl))
                                      : std::sin(_MC_dilation.value(intnl));
  const Real ds = (f_or_d == friction)
                      ? std::cos(_MC_friction.value(intnl)) * _MC_friction.dvalue(intnl)
                      : std::cos(_MC_dilation.value(intnl)) * _MC_dilation.dvalue(intnl);
  switch (_MC_interpolation_scheme)
  {
    case 1: // DP1 --> outer tip
      dalpha_beta = 2.0 / std::sqrt(3.0) * (ds / (3.0 - s) + s * ds / Utility::pow<2>(3.0 - s));
      break;
    case 2: // DP2 --> lode_zero
      dalpha_beta = ds / 3.0;
      break;
    case 3: // DP3 --> inner tip
      dalpha_beta = 2.0 / std::sqrt(3.0) * (ds / (3.0 + s) - s * ds / Utility::pow<2>(3.0 + s));
      break;
    case 4: // DP4 --> inner edge
      dalpha_beta = ds / std::sqrt(9.0 + 3.0 * Utility::pow<2>(s)) -
                    3 * s * s * ds / std::pow(9.0 + 3.0 * Utility::pow<2>(s), 1.5);
      break;
    case 5: // DP5 --> native
      const Real c = (f_or_d == friction) ? std::cos(_MC_friction.value(intnl))
                                          : std::cos(_MC_dilation.value(intnl));
      const Real dc = (f_or_d == friction)
                          ? -std::sin(_MC_friction.value(intnl)) * _MC_friction.dvalue(intnl)
                          : -std::sin(_MC_dilation.value(intnl)) * _MC_dilation.dvalue(intnl);
      dalpha_beta = ds / c - s * dc / Utility::pow<2>(c);
      break;
  }
}