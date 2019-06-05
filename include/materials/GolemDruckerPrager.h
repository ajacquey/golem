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

#pragma once

#include "GolemPQPlasticity.h"
#include "GolemHardeningModel.h"

class GolemDruckerPrager;

template <>
InputParameters validParams<GolemDruckerPrager>();

class GolemDruckerPrager : public GolemPQPlasticity
{
public:
  GolemDruckerPrager(const InputParameters & parameters);
  static MooseEnum MC_interpolation_scheme();

protected:
  virtual void computePQStress(const RankTwoTensor & stress, Real & p, Real & q) const override;
  virtual void
  setEffectiveElasticity(const RankFourTensor & Eijkl, Real & Epp, Real & Eqq) const override;
  virtual RankTwoTensor dpdstress(const RankTwoTensor & stress) const override;
  virtual RankFourTensor d2pdstress2(const RankTwoTensor & stress) const override;
  virtual RankTwoTensor dqdstress(const RankTwoTensor & stress) const override;
  virtual RankFourTensor d2qdstress2(const RankTwoTensor & stress) const override;
  virtual Real yieldFunctionValue(Real p, Real q, const Real & intnl) const override;
  virtual Real dyieldFunction_dp(Real p, Real q, const Real & intnl) const override;
  virtual Real dyieldFunction_dq(Real p, Real q, const Real & intnl) const override;
  virtual Real dyieldFunction_dintnl(Real p, Real q, const Real & intnl) const override;
  virtual Real dflowPotential_dp(Real p, Real q, const Real & intnl) const override;
  virtual Real dflowPotential_dq(Real p, Real q, const Real & intnl) const override;
  virtual Real d2flowPotential_dp2(Real p, Real q, const Real & intnl) const override;
  virtual Real d2flowPotential_dp_dq(Real p, Real q, const Real & intnl) const override;
  virtual Real d2flowPotential_dq2(Real p, Real q, const Real & intnl) const override;
  virtual Real d2flowPotential_dp_dintnl(Real p, Real q, const Real & intnl) const override;
  virtual Real d2flowPotential_dq_dintnl(Real p, Real q, const Real & intnl) const override;
  virtual void setIntnlValues(Real p_trial,
                              Real q_trial,
                              Real p,
                              Real q,
                              const Real & intnl_old,
                              Real & intnl) const override;
  virtual void setIntnlDerivatives(Real p_trial,
                                   Real q_trial,
                                   Real p,
                                   Real q,
                                   const Real & intnl,
                                   std::vector<Real> & dintnl) const override;

  const GolemHardeningModel & _MC_cohesion;
  const GolemHardeningModel & _MC_friction;
  const GolemHardeningModel & _MC_dilation;
  const MooseEnum _MC_interpolation_scheme;
  const bool _no_hardening_cohesion;
  const bool _no_hardening_friction;
  const bool _no_hardening_dilation;
  enum FrictionOrDilation
  {
    friction = 0,
    dilation = 1
  };

private:
  void bothAB(Real intnl, Real & k, Real & alpha) const;
  void onlyB(Real intnl, int f_or_d, Real & alpha_beta) const;
  void dbothAB(Real intnl, Real & dk, Real & dalpha) const;
  void donlyB(Real intnl, int f_or_d, Real & dalpha_beta) const;
  void initializeAB(Real intnl, Real & k, Real & alpha) const;
  void initializeB(Real intnl, int f_or_d, Real & alpha_beta) const;

  Real _smoother2;
  Real _alpha; // for friction
  Real _beta;  // for dilation
  Real _k;
};