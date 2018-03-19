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

#include "GolemPQPlasticity.h"
#include "libmesh/utility.h" // for Utility::pow
#include <petscblaslapack.h> // LAPACKgesv_

template <>
InputParameters
validParams<GolemPQPlasticity>()
{
  InputParameters params = validParams<GolemInelasticBase>();
  params.addClassDescription(
      "Base class for plasticity return-map algorithm for (P, Q) plastic models.");
  params.addRangeCheckedParam<unsigned int>(
      "max_NR_iterations",
      20,
      "max_NR_iterations>0",
      "Maximum number of Newton-Raphson iterations allowed during the return-map algorithm");
  params.addRequiredParam<Real>("yield_function_tol",
                                "The return-map process will be deemed to have converged if all "
                                "yield functions are within yield_function_tol of zero.  If this "
                                "is set very low then precision-loss might be encountered: if the "
                                "code detects precision loss then it also deems the return-map "
                                "process has converged.");
  params.addParam<Real>("min_step_size",
                        1.0,
                        "In order to help the Newton-Raphson procedure, the applied strain "
                        "increment may be applied in sub-increments of size greater than this "
                        "value.  Usually it is better for Moose's nonlinear convergence to "
                        "increase max_NR_iterations rather than decrease this parameter.");
  return params;
}

GolemPQPlasticity::GolemPQPlasticity(const InputParameters & parameters)
  : GolemInelasticBase(parameters),
    _max_nr_its(getParam<unsigned>("max_NR_iterations")),
    _f_tol(getParam<Real>("yield_function_tol")),
    _f_tol2(Utility::pow<2>(getParam<Real>("yield_function_tol"))),
    _min_step_size(getParam<Real>("min_step_size")),
    _plastic_strain(declareProperty<RankTwoTensor>(_base_name + "plastic_strain")),
    _plastic_strain_old(getMaterialPropertyOld<RankTwoTensor>(_base_name + "plastic_strain")),
    _intnl(declareProperty<Real>(_base_name + "plastic_internal_parameter")),
    _intnl_old(getMaterialPropertyOld<Real>(_base_name + "plastic_internal_parameter")),
    _yf(declareProperty<Real>(_base_name + "plastic_yield_function")),
    _p_trial(0.0),
    _q_trial(0.0),
    _intnl_ok(0.0),
    _dintnl(_num_pq),
    _Epp(0.0),
    _Eqq(0.0),
    _dgaE_dpt(0.0),
    _dp_dpt(0.0),
    _dq_dpt(0.0),
    _dgaE_dqt(0.0),
    _dp_dqt(0.0),
    _dq_dqt(0.0)
{
}

void
GolemPQPlasticity::initQpStatefulProperties()
{
  _plastic_strain[_qp].zero();
  _intnl[_qp] = 0.0;
}

void
GolemPQPlasticity::updateStress(RankTwoTensor & strain_increment,
                                RankTwoTensor & inelastic_strain_increment,
                                RankTwoTensor & stress_new,
                                const RankTwoTensor & stress_old,
                                const RankFourTensor & elasticity_tensor,
                                bool compute_full_tangent_operator,
                                RankFourTensor & tangent_operator)
{
  // Initially assume an elastic deformation
  _intnl[_qp] = _intnl_old[_qp];

  computePQStress(stress_new, _p_trial, _q_trial);
  _yf[_qp] = yieldFunctionValue(_p_trial, _q_trial, _intnl[_qp]);

  if (_yf[_qp] <= _f_tol)
  {
    _plastic_strain[_qp] = _plastic_strain_old[_qp];
    inelastic_strain_increment.zero();
    tangent_operator = elasticity_tensor;
    return;
  }

  const RankTwoTensor stress_trial = stress_new;
  /* The trial stress must be inadmissible
   * so we need to return to the yield surface.  The following
   * equations must be satisfied.
   *
   * 0 = f(p, q, intnl)                  = rhs[0]
   * 0 = p - p^trial + Epp * ga * dg/dp  = rhs[1]
   * 0 = q - q^trial + Eqq * ga * dg/dq  = rhs[2]
   * Equations defining intnl parameters as functions of (p, q, p_trial, q_trial, intnl_old)
   *
   * The unknowns are p, q, ga, and intnl.
   * I find it convenient to solve the first three equations for p, q and ga*Epp=gaE,
   * while substituting the "intnl parameters" equations into these during the solve process
   */

  setEffectiveElasticity(elasticity_tensor, _Epp, _Eqq);

  // Values of p, q  and intnl we know to be admissible
  Real p_ok;
  Real q_ok;
  computePQStress(stress_old, p_ok, q_ok);
  _intnl_ok = _intnl_old[_qp];

  _dgaE_dpt = 0.0;
  _dp_dpt = 0.0;
  _dq_dpt = 0.0;
  _dgaE_dqt = 0.0;
  _dp_dqt = 0.0;
  _dq_dqt = 0.0;

  // Return-map problem: must apply the following changes in p and q, and find the returned p and q.
  const Real del_p = _p_trial - p_ok;
  const Real del_q = _q_trial - q_ok;

  // Can potentially split a time step into substeps
  Real step_taken = 0.0;
  Real step_size = 1.0;
  Real gaE_total = 0.0;

  // Current values of the yield function, derivatives, etc
  yieldAndFlow F_and_Q;

  // In the following sub-stepping procedure it is possible that
  // the last step is an elastic step, and therefore F_and_Q won't
  // be computed on the last step, so we have to compute it.
  bool F_and_Q_calculated = false;

  while (step_taken < 1.0 && step_size >= _min_step_size)
  {
    // Prevent over-shoots of substepping
    if (1.0 - step_taken < step_size)
      step_size = 1.0 - step_taken;

    // Trial variables in terms of admissible variables
    _p_trial = p_ok + step_size * del_p;
    _q_trial = q_ok + step_size * del_q;

    // Initialise (p, q, gaE)
    Real p = _p_trial;
    Real q = _q_trial;
    Real gaE = 0.0;

    // Flags indicating failure of Newton-Raphson and line-search
    int nr_failure = 0;
    int ls_failure = 0;

    // NR iterations taken in this substep
    unsigned step_iter = 0;

    // The residual-squared for the line-search
    Real res2 = 0.0;

    // This is an elastic step
    // The "step_size < 1.0" in above condition is for efficiency: we definitely
    // know that this is a plastic step if step_size = 1.0
    if (step_size < 1.0 && yieldFunctionValue(_p_trial, _q_trial, _intnl_ok) <= _f_tol)
      F_and_Q_calculated = false;
    else
    {
      // This is a plastic step

      // initialise p, q and gaE
      initialiseVars(_p_trial, _q_trial, _intnl_ok, p, q, gaE, _intnl[_qp]);

      // Calculate yield function, flow potential and derivatives
      F_and_Q = calcAllQuantities(p, q, _intnl[_qp]);
      F_and_Q_calculated = true;
      res2 = calculateRHS(_p_trial, _q_trial, p, q, gaE, F_and_Q);

      // Perform a Newton-Raphson with linesearch to get current_sp, gaE, and also smoothed_q
      while (res2 > _f_tol2 && step_iter < _max_nr_its && nr_failure == 0 && ls_failure == 0)
      {
        // Solve the linear system and store the answer (the "updates") in rhs
        nr_failure = nrStep(F_and_Q, _p_trial, _q_trial, p, q, gaE);
        if (nr_failure != 0)
          break;

        // apply (parts of) the updates, re-calculate smoothed_q, and res2
        ls_failure = lineSearch(res2, gaE, p, q, _p_trial, _q_trial, F_and_Q, _intnl_ok);
        step_iter++;
      }
    }

    if (res2 <= _f_tol2 && step_iter < _max_nr_its && nr_failure == 0 && ls_failure == 0 &&
        gaE >= 0.0)
    {
      // This Newton-Raphson worked fine, or this was an elastic step
      p_ok = p;
      q_ok = q;
      gaE_total += gaE;
      step_taken += step_size;
      setIntnlValues(_p_trial, _q_trial, p_ok, q_ok, _intnl_ok, _intnl[_qp]);
      _intnl_ok = _intnl[_qp];
      // calculate dp/dp_trial, dp/dq_trial, etc, for Jacobian
      dVardTrial(!F_and_Q_calculated,
                 _p_trial,
                 _q_trial,
                 p_ok,
                 q_ok,
                 gaE,
                 _intnl_ok,
                 F_and_Q,
                 step_size,
                 compute_full_tangent_operator);

      step_size *= 1.1;
    }
    else
    {
      // Newton-Raphson + line-search process failed
      _intnl[_qp] = _intnl_ok;
      step_size *= 0.5;
    }
  }

  if (step_size < _min_step_size)
    throw MooseException("GolemPQPlasticity: Minimum step-size violated");

  // Success!
  _yf[_qp] = yieldFunctionValue(p_ok, q_ok, _intnl[_qp]);

  if (!F_and_Q_calculated)
    F_and_Q = calcAllQuantities(p_ok, q_ok, _intnl[_qp]);

  setStressAfterReturn(stress_trial, gaE_total, F_and_Q, elasticity_tensor, stress_new);

  setInelasticStrainIncrementAfterReturn(
      gaE_total, F_and_Q, stress_new, inelastic_strain_increment);

  strain_increment = strain_increment - inelastic_strain_increment;
  _plastic_strain[_qp] = _plastic_strain_old[_qp] + inelastic_strain_increment;

  consistentTangentOperator(stress_trial,
                            stress_new,
                            gaE_total,
                            F_and_Q,
                            elasticity_tensor,
                            compute_full_tangent_operator,
                            tangent_operator);
}

GolemPQPlasticity::yieldAndFlow
GolemPQPlasticity::calcAllQuantities(Real p, Real q, const Real & intnl)
{
  yieldAndFlow F_and_Q(_num_rhs);

  Real y = yieldFunctionValue(p, q, intnl);
  Real dy_dp = dyieldFunction_dp(p, q, intnl);
  Real dy_dq = dyieldFunction_dq(p, q, intnl);
  Real dy_dintnl = dyieldFunction_dintnl(p, q, intnl);
  Real dg_dp = dflowPotential_dp(p, q, intnl);
  Real dg_dq = dflowPotential_dq(p, q, intnl);
  Real d2g_dp2 = d2flowPotential_dp2(p, q, intnl);
  Real d2g_dp_dq = d2flowPotential_dp_dq(p, q, intnl);
  Real d2g_dq2 = d2flowPotential_dq2(p, q, intnl);
  Real d2g_dp_di = d2flowPotential_dp_dintnl(p, q, intnl);
  Real d2g_dq_di = d2flowPotential_dq_dintnl(p, q, intnl);

  // Yield function: f
  F_and_Q.f = y;

  // Derivatives of the yield function f
  // d(yf)/d(p)
  F_and_Q.df[0] = dy_dp;
  // d(yf)/d(q)
  F_and_Q.df[1] = dy_dq;
  // d(yf)/d(intnl)
  F_and_Q.df_di = dy_dintnl;

  // Derivatives of the flow potential: g
  // d(g)/d(p)
  F_and_Q.dg[0] = dg_dp;
  // d(g)/d(q)
  F_and_Q.dg[1] = dg_dq;

  // Second derivatives of the flow potential: g
  // d2(g)/d(p)2
  F_and_Q.d2g[0][0] = d2g_dp2;
  // d2(g)/d(p)d(q)
  F_and_Q.d2g[0][1] = d2g_dp_dq;
  // d2(g)/d(q)d(p)
  F_and_Q.d2g[1][0] = d2g_dp_dq;
  // d2(g)/d(q)2
  F_and_Q.d2g[1][1] = d2g_dq2;
  // d2(g)/d(p)d(intnl)
  F_and_Q.d2g_di[0] = d2g_dp_di;
  // d2(g)/d(q)d(intnl)
  F_and_Q.d2g_di[1] = d2g_dq_di;

  return F_and_Q;
}

void
GolemPQPlasticity::dVardTrial(bool elastic_only,
                              Real p_trial,
                              Real q_trial,
                              Real p,
                              Real q,
                              Real gaE,
                              const Real & intnl,
                              const yieldAndFlow & F_and_Q,
                              Real step_size,
                              bool compute_full_tangent_operator)
{
  if (!_fe_problem.currentlyComputingJacobian())
    return;

  if (!compute_full_tangent_operator)
    return;

  if (elastic_only)
  {
    // no change to gaE, and dp_dqt and dq_dpt are unchanged from previous step
    _dp_dpt = step_size + _dp_dpt;
    _dq_dqt = step_size + _dq_dqt;
    return;
  }

  setIntnlDerivatives(p_trial, q_trial, p, q, intnl, _dintnl);

  // _rhs is defined above, the following are changes in rhs wrt the trial p and q values
  // In the following we use d(intnl)/d(trial variable) = - d(intnl)/d(variable)
  std::array<Real, _num_pq * _num_rhs> rhs_cto{{0.0}};

  // change in p_trial
  rhs_cto[0] -= F_and_Q.df_di * _dintnl[0];
  rhs_cto[1] = -1.0;
  rhs_cto[1] -= gaE * F_and_Q.d2g_di[0] * _dintnl[0];
  rhs_cto[2] -= _Eqq * gaE / _Epp * F_and_Q.d2g_di[1] * _dintnl[0];

  // change in q_trial
  rhs_cto[3] -= F_and_Q.df_di * _dintnl[1];
  rhs_cto[4] -= gaE * F_and_Q.d2g_di[0] * _dintnl[1];
  rhs_cto[5] = -1.0;
  rhs_cto[5] -= _Eqq * gaE / _Epp * F_and_Q.d2g_di[1] * _dintnl[1];

  // jac = d(-rhs)/d(var), where var[0] = gaE, var[1] = p, var[2] = q.
  std::array<double, _num_rhs * _num_rhs> jac;
  dnRHSdVar(F_and_Q, _dintnl, gaE, jac);

  std::array<int, _num_rhs> ipiv;
  int info;
  const int gesv_num_rhs = _num_rhs;
  const int gesv_num_pq = _num_pq;
  LAPACKgesv_(&gesv_num_rhs,
              &gesv_num_pq,
              &jac[0],
              &gesv_num_rhs,
              &ipiv[0],
              &rhs_cto[0],
              &gesv_num_rhs,
              &info);
  if (info != 0)
    throw MooseException("GolemPQPlasticity: PETSC LAPACK gsev routine returned with error code " +
                         Moose::stringify(info));

  const Real dgaEn_dptn = rhs_cto[0];
  const Real dpn_dptn = rhs_cto[1];
  const Real dqn_dptn = rhs_cto[2];
  const Real dgaEn_dqtn = rhs_cto[3];
  const Real dpn_dqtn = rhs_cto[4];
  const Real dqn_dqtn = rhs_cto[5];

  const Real dp_dpt_old = _dp_dpt;
  const Real dq_dpt_old = _dq_dpt;
  const Real dp_dqt_old = _dp_dqt;
  const Real dq_dqt_old = _dq_dqt;

  _dgaE_dpt += dgaEn_dptn * (step_size + dp_dpt_old) + dgaEn_dqtn * dq_dpt_old;
  _dp_dpt = dpn_dptn * (step_size + dp_dpt_old) + dpn_dqtn * dq_dpt_old;
  _dq_dpt = dqn_dptn * (step_size + dp_dpt_old) + dqn_dqtn * dq_dpt_old;
  _dgaE_dqt += dgaEn_dqtn * (step_size + dq_dqt_old) + dgaEn_dptn * dp_dqt_old;
  _dp_dqt = dpn_dqtn * (step_size + dq_dqt_old) + dpn_dptn * dp_dqt_old;
  _dq_dqt = dqn_dqtn * (step_size + dq_dqt_old) + dqn_dptn * dp_dqt_old;
}

int
GolemPQPlasticity::lineSearch(Real & res2,
                              Real & gaE,
                              Real & p,
                              Real & q,
                              Real p_trial,
                              Real q_trial,
                              yieldAndFlow & F_and_Q,
                              const Real & intnl_ok)
{
  const Real res2_old = res2;
  const Real gaE_old = gaE;
  const Real p_old = p;
  const Real q_old = q;
  const Real de_gaE = _rhs[0];
  const Real de_p = _rhs[1];
  const Real de_q = _rhs[2];

  Real lam = 1.0;                     // line-search parameter
  const Real lam_min = 1E-10;         // minimum value of lam allowed
  const Real slope = -2.0 * res2_old; // "Numerical Recipes" uses -b*A*x, in order to check for
                                      // roundoff, but i hope the nrStep would warn if there were
                                      // problems
  Real tmp_lam;                       // cached value of lam used in quadratic & cubic line search
  Real f2 = res2_old; // cached value of f = residual2 used in the cubic in the line search
  Real lam2 = lam;    // cached value of lam used in the cubic in the line search

  while (true)
  {
    // update variables using the current line-search parameter
    gaE = gaE_old + lam * de_gaE;
    p = p_old + lam * de_p;
    q = q_old + lam * de_q;

    // and internal parameters
    setIntnlValues(_p_trial, _q_trial, p, q, intnl_ok, _intnl[_qp]);

    F_and_Q = calcAllQuantities(p, q, _intnl[_qp]);

    // update rhs for next-time through
    res2 = calculateRHS(p_trial, q_trial, p, q, gaE, F_and_Q);

    // do the line-search
    if (res2 < res2_old + 1E-4 * lam * slope)
      break;
    else if (lam < lam_min)
      return 1;
    else if (lam == 1.0)
    {
      // model as a quadratic
      tmp_lam = -0.5 * slope / (res2 - res2_old - slope);
    }
    else
    {
      // model as a cubic
      const Real rhs1 = res2 - res2_old - lam * slope;
      const Real rhs2 = f2 - res2_old - lam2 * slope;
      const Real a = (rhs1 / Utility::pow<2>(lam) - rhs2 / Utility::pow<2>(lam2)) / (lam - lam2);
      const Real b =
          (-lam2 * rhs1 / Utility::pow<2>(lam) + lam * rhs2 / Utility::pow<2>(lam2)) / (lam - lam2);
      if (a == 0.0)
        tmp_lam = -slope / (2.0 * b);
      else
      {
        const Real disc = Utility::pow<2>(b) - 3.0 * a * slope;
        if (disc < 0)
          tmp_lam = 0.5 * lam;
        else if (b <= 0)
          tmp_lam = (-b + std::sqrt(disc)) / (3.0 * a);
        else
          tmp_lam = -slope / (b + std::sqrt(disc));
      }
      if (tmp_lam > 0.5 * lam)
        tmp_lam = 0.5 * lam;
    }
    lam2 = lam;
    f2 = res2;
    lam = std::max(tmp_lam, 0.1 * lam);
  }

  return 0;
}

void
GolemPQPlasticity::dnRHSdVar(const yieldAndFlow & F_and_Q,
                             const std::vector<Real> & dintnl,
                             Real gaE,
                             std::array<double, _num_rhs * _num_rhs> & jac) const
{
  // LAPACK gsev stores the matrix in the following way:

  // d(-yieldF)/d(gaE)
  jac[0] = 0;

  // d(-rhs[1])/d(gaE)
  jac[1] = -F_and_Q.dg[0];

  // d(-rhs[2])/d(gaE))
  jac[2] = -_Eqq * F_and_Q.dg[1] / _Epp;

  // d(-yieldF)/d(p)
  jac[3] = -F_and_Q.df[0];
  jac[3] -= F_and_Q.df_di * dintnl[0];

  // d(-rhs[1])/d(p)
  jac[4] = -1.0 - gaE * F_and_Q.d2g[0][0];
  jac[4] -= gaE * F_and_Q.d2g_di[0] * dintnl[0];

  // d(-rhs[2])/d(p)
  jac[5] = -_Eqq * gaE / _Epp * F_and_Q.d2g[1][0];
  jac[5] -= _Eqq * gaE / _Epp * F_and_Q.d2g_di[1] * dintnl[0];

  // d(-yieldF)/d(q)
  jac[6] = -F_and_Q.df[1];
  jac[6] -= F_and_Q.df_di * dintnl[1];

  // d(-rhs[1])/d(q)
  jac[7] = -gaE * F_and_Q.d2g[0][1];
  jac[7] -= gaE * F_and_Q.d2g_di[0] * dintnl[1];

  // d(-rhs[2])/d(q)
  jac[8] = -1.0 - _Eqq * gaE / _Epp * F_and_Q.d2g[1][1];
  jac[8] -= _Eqq * gaE / _Epp * F_and_Q.d2g_di[1] * dintnl[1];
}

int
GolemPQPlasticity::nrStep(
    const yieldAndFlow & F_and_Q, Real p_trial, Real q_trial, Real p, Real q, Real gaE)
{
  setIntnlDerivatives(p_trial, q_trial, p, q, _intnl[_qp], _dintnl);

  std::array<double, _num_rhs * _num_rhs> jac;
  dnRHSdVar(F_and_Q, _dintnl, gaE, jac);

  // Use LAPACK to solve the linear system
  const int nrhs = 1;
  std::array<int, _num_rhs> ipiv;
  int info;
  const int gesv_num_rhs = _num_rhs;
  LAPACKgesv_(
      &gesv_num_rhs, &nrhs, &jac[0], &gesv_num_rhs, &ipiv[0], &_rhs[0], &gesv_num_rhs, &info);
  return info;
}

Real
GolemPQPlasticity::calculateRHS(
    Real p_trial, Real q_trial, Real p, Real q, Real gaE, const yieldAndFlow & F_and_Q)
{
  _rhs[0] = F_and_Q.f;
  _rhs[1] = p - p_trial + gaE * F_and_Q.dg[0];
  _rhs[2] = q - q_trial + _Eqq * gaE / _Epp * F_and_Q.dg[1];
  return _rhs[0] * _rhs[0] + _rhs[1] * _rhs[1] + _rhs[2] * _rhs[2];
}

void
GolemPQPlasticity::initialiseVars(Real p_trial,
                                  Real q_trial,
                                  const Real & intnl_old,
                                  Real & p,
                                  Real & q,
                                  Real & gaE,
                                  Real & intnl) const
{
  p = p_trial;
  q = q_trial;
  gaE = 0.0;
  intnl = intnl_old;
}

void
GolemPQPlasticity::consistentTangentOperator(const RankTwoTensor & stress_trial,
                                             const RankTwoTensor & stress,
                                             Real gaE,
                                             const yieldAndFlow & F_and_Q,
                                             const RankFourTensor & elasticity_tensor,
                                             bool compute_full_tangent_operator,
                                             RankFourTensor & cto) const
{
  if (!_fe_problem.currentlyComputingJacobian())
    return;

  cto = elasticity_tensor;
  if (!compute_full_tangent_operator)
    return;

  const RankTwoTensor dpdsig = dpdstress(stress);
  const RankTwoTensor dpdsig_trial = dpdstress(stress_trial);
  const RankTwoTensor dqdsig = dqdstress(stress);
  const RankTwoTensor dqdsig_trial = dqdstress(stress_trial);

  const RankTwoTensor s1 = elasticity_tensor * ((1.0 / _Epp) * (1.0 - _dp_dpt) * dpdsig +
                                                (1.0 / _Eqq) * (-_dq_dpt) * dqdsig);
  const RankTwoTensor s2 = elasticity_tensor * ((1.0 / _Epp) * (-_dp_dqt) * dpdsig +
                                                (1.0 / _Eqq) * (1.0 - _dq_dqt) * dqdsig);
  const RankTwoTensor t1 = elasticity_tensor * dpdsig_trial;
  const RankTwoTensor t2 = elasticity_tensor * dqdsig_trial;

  for (unsigned i = 0; i < _tensor_dimensionality; ++i)
    for (unsigned j = 0; j < _tensor_dimensionality; ++j)
      for (unsigned k = 0; k < _tensor_dimensionality; ++k)
        for (unsigned l = 0; l < _tensor_dimensionality; ++l)
          cto(i, j, k, l) -= s1(i, j) * t1(k, l) + s2(i, j) * t2(k, l);

  const RankFourTensor d2pdsig2 = d2pdstress2(stress);
  const RankFourTensor d2qdsig2 = d2qdstress2(stress);

  const RankFourTensor Tijab =
      elasticity_tensor * (gaE / _Epp) * (F_and_Q.dg[0] * d2pdsig2 + F_and_Q.dg[1] * d2qdsig2);

  RankFourTensor inv = RankFourTensor(RankFourTensor::initIdentityFour) + Tijab;
  try
  {
    inv = inv.transposeMajor().invSymm();
  }
  catch (const MooseException & e)
  {
    // Cannot form the inverse, so probably at some degenerate place in stress space.
    // Just return with the "best estimate" of the cto.
    mooseWarning("GolemPQPlasticity: Cannot invert 1+T in consistent tangent "
                 "operator computation at quadpoint ",
                 _qp,
                 " of element ",
                 _current_elem->id());
    return;
  }

  cto = (cto.transposeMajor() * inv).transposeMajor();
}

void
GolemPQPlasticity::setStressAfterReturn(const RankTwoTensor & stress_trial,
                                        Real gaE,
                                        const yieldAndFlow & F_and_Q,
                                        const RankFourTensor & elasticity_tensor,
                                        RankTwoTensor & stress) const
{
  const RankTwoTensor correction =
      elasticity_tensor * (F_and_Q.dg[0] * dpdstress(stress) + F_and_Q.dg[1] * dqdstress(stress));
  stress = stress_trial - gaE / _Epp * correction;
}

void
GolemPQPlasticity::setInelasticStrainIncrementAfterReturn(
    Real gaE,
    const yieldAndFlow & F_and_Q,
    const RankTwoTensor & returned_stress,
    RankTwoTensor & inelastic_strain_increment) const
{
  inelastic_strain_increment = (gaE / _Epp) * (F_and_Q.dg[0] * dpdstress(returned_stress) +
                                               F_and_Q.dg[1] * dqdstress(returned_stress));
}