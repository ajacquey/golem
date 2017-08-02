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

#ifndef GOLEMPQPLASTICITY_H
#define GOLEMPQPLASTICITY_H

#include "GolemInelasticBase.h"
#include <array>

class GolemPQPlasticity;

template <>
InputParameters validParams<GolemPQPlasticity>();

class GolemPQPlasticity : public GolemInelasticBase
{
public:
  GolemPQPlasticity(const InputParameters & parameters);
  virtual void updateStress(RankTwoTensor & strain_increment,
                            RankTwoTensor & inelastic_strain_increment,
                            RankTwoTensor & stress_new,
                            const RankTwoTensor & stress_old,
                            const RankFourTensor & elasticity_tensor,
                            bool compute_full_tangent_operator,
                            RankFourTensor & tangent_operator) override;

protected:
  constexpr static unsigned _tensor_dimensionality = 3;
  constexpr static int _num_pq = 2;
  constexpr static int _num_rhs = 3;
  struct yieldAndFlow
  {
    Real f;
    std::vector<Real> df;
    Real df_di;
    std::vector<Real> dg;
    std::vector<std::vector<Real>> d2g;
    std::vector<Real> d2g_di;

    yieldAndFlow() : yieldAndFlow(0) {}

    yieldAndFlow(unsigned num_var)
      : f(0.0), df(num_var), df_di(0.0), dg(num_var), d2g(num_var), d2g_di(num_var)
    {
      for (unsigned i = 0; i < num_var; ++i)
        d2g[i].assign(num_var, 0.0);
    }
  };

  virtual void initQpStatefulProperties() override;
  virtual Real yieldFunctionValue(Real p, Real q, const Real & intnl) const = 0;
  virtual Real dyieldFunction_dp(Real p, Real q, const Real & intnl) const = 0;
  virtual Real dyieldFunction_dq(Real p, Real q, const Real & intnl) const = 0;
  virtual Real dyieldFunction_dintnl(Real p, Real q, const Real & intnl) const = 0;
  virtual Real dflowPotential_dp(Real p, Real q, const Real & intnl) const = 0;
  virtual Real dflowPotential_dq(Real p, Real q, const Real & intnl) const = 0;
  virtual Real d2flowPotential_dp2(Real p, Real q, const Real & intnl) const = 0;
  virtual Real d2flowPotential_dp_dq(Real p, Real q, const Real & intnl) const = 0;
  virtual Real d2flowPotential_dq2(Real p, Real q, const Real & intnl) const = 0;
  virtual Real d2flowPotential_dp_dintnl(Real p, Real q, const Real & intnl) const = 0;
  virtual Real d2flowPotential_dq_dintnl(Real p, Real q, const Real & intnl) const = 0;
  yieldAndFlow calcAllQuantities(Real p, Real q, const Real & intnl);
  void dVardTrial(bool elastic_only,
                  Real p_trial,
                  Real q_trial,
                  Real p,
                  Real q,
                  Real gaE,
                  const Real & intnl,
                  const yieldAndFlow & F_and_Q,
                  Real step_size,
                  bool compute_full_tangent_operator);
  int lineSearch(Real & res2,
                 Real & gaE,
                 Real & p,
                 Real & q,
                 Real p_trial,
                 Real q_trial,
                 yieldAndFlow & F_and_Q,
                 const Real & intnl_ok);
  int nrStep(const yieldAndFlow & F_and_Q, Real p_trial, Real q_trial, Real p, Real q, Real gaE);
  Real
  calculateRHS(Real p_trial, Real q_trial, Real p, Real q, Real gaE, const yieldAndFlow & F_and_Q);
  void dnRHSdVar(const yieldAndFlow & F_and_Q,
                 const std::vector<Real> & dintnl,
                 Real gaE,
                 std::array<double, _num_rhs * _num_rhs> & jac) const;
  virtual void initialiseVars(Real p_trial,
                              Real q_trial,
                              const Real & intnl_old,
                              Real & p,
                              Real & q,
                              Real & gaE,
                              Real & intnl) const;
  virtual void setIntnlValues(
      Real p_trial, Real q_trial, Real p, Real q, const Real & intnl_old, Real & intnl) const = 0;
  virtual void setIntnlDerivatives(Real p_trial,
                                   Real q_trial,
                                   Real p,
                                   Real q,
                                   const Real & intnl,
                                   std::vector<Real> & dintnl) const = 0;
  virtual void computePQStress(const RankTwoTensor & stress, Real & p, Real & q) const = 0;
  virtual void
  setEffectiveElasticity(const RankFourTensor & Eijkl, Real & Epp, Real & Eqq) const = 0;
  virtual void setStressAfterReturn(const RankTwoTensor & stress_trial,
                                    Real gaE,
                                    const yieldAndFlow & F_and_Q,
                                    const RankFourTensor & Eijkl,
                                    RankTwoTensor & stress) const;
  virtual void
  setInelasticStrainIncrementAfterReturn(Real gaE,
                                         const yieldAndFlow & F_and_Q,
                                         const RankTwoTensor & returned_stress,
                                         RankTwoTensor & inelastic_strain_increment) const;
  virtual void consistentTangentOperator(const RankTwoTensor & stress_trial,
                                         const RankTwoTensor & stress,
                                         Real gaE,
                                         const yieldAndFlow & F_and_Q,
                                         const RankFourTensor & Eijkl,
                                         bool compute_full_tangent_operator,
                                         RankFourTensor & cto) const;
  virtual RankTwoTensor dpdstress(const RankTwoTensor & stress) const = 0;
  virtual RankFourTensor d2pdstress2(const RankTwoTensor & stress) const = 0;
  virtual RankTwoTensor dqdstress(const RankTwoTensor & stress) const = 0;
  virtual RankFourTensor d2qdstress2(const RankTwoTensor & stress) const = 0;

  const unsigned _max_nr_its;
  const Real _f_tol;
  const Real _f_tol2;
  const Real _min_step_size;
  MaterialProperty<RankTwoTensor> & _plastic_strain;
  const MaterialProperty<RankTwoTensor> & _plastic_strain_old;
  MaterialProperty<Real> & _intnl;
  const MaterialProperty<Real> & _intnl_old;
  MaterialProperty<Real> & _yf;

  Real _p_trial;
  Real _q_trial;
  Real _intnl_ok;
  std::vector<Real> _dintnl;
  Real _Epp;
  Real _Eqq;
  Real _dgaE_dpt;
  Real _dp_dpt;
  Real _dq_dpt;
  Real _dgaE_dqt;
  Real _dp_dqt;
  Real _dq_dqt;
  std::array<Real, _num_rhs> _rhs;
};

#endif // GOLEMPQPLASTICITY_H