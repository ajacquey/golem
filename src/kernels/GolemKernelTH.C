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

#include "GolemKernelTH.h"
#include "Assembly.h"
#include "MooseVariable.h"
#include "Problem.h"
#include "SubProblem.h"
#include "SystemBase.h"
#include "libmesh/quadrature.h"
#include "MooseMesh.h"
#include "MooseError.h"

registerMooseObject("GolemApp", GolemKernelTH);

template <>
InputParameters
validParams<GolemKernelTH>()
{
  InputParameters params = validParams<TimeKernel>();
  params.addClassDescription("Advective kernel for temperature based on either a conservative or "
                             "non-conservative scheme.");
  params.addRequiredCoupledVar("pore_pressure", "The pore pressure");
  params.addCoupledVar("displacements", "The displacement vector");
  params.addParam<bool>("is_conservative", false, "Is conservative?");
  params.addParam<bool>("has_lumped_mass_matrix", false, "Has lumped mass matrix?");
  params.addParam<UserObjectName>("supg_uo", "The name of the SUPG user object");
  return params;
}

GolemKernelTH::GolemKernelTH(const InputParameters & parameters)
  : DerivativeMaterialInterface<TimeKernel>(parameters),
    _is_conservative(getParam<bool>("is_conservative")),
    _has_lumped_mass_matrix(getParam<bool>("has_lumped_mass_matrix")),
    _has_SUPG_upwind(isParamValid("supg_uo") ? true : false),
    _has_disp(isCoupled("displacements")),
    _u_old(_fe_problem.isTransient() ? valueOld() : _zero),
    _grad_pf(coupledGradient("pore_pressure")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _H_kernel_grav(getMaterialProperty<RealVectorValue>("H_kernel_grav")),
    _TH_kernel(getMaterialProperty<RankTwoTensor>("TH_kernel")),
    _T_kernel_diff(getMaterialProperty<Real>("T_kernel_diff")),
    _T_kernel_source(getDefaultMaterialProperty<Real>("T_kernel_source")),
    _T_kernel_time(getDefaultMaterialProperty<Real>("T_kernel_time")),
    _dTH_kernel_dev(getDefaultMaterialProperty<RankTwoTensor>("dTH_kernel_dev")),
    _dTH_kernel_dT(getMaterialProperty<RankTwoTensor>("dTH_kernel_dT")),
    _dTH_kernel_dpf(getMaterialProperty<RankTwoTensor>("dTH_kernel_dpf")),
    _dH_kernel_grav_dT(getMaterialProperty<RealVectorValue>("dH_kernel_grav_dT")),
    _dH_kernel_grav_dpf(getMaterialProperty<RealVectorValue>("dH_kernel_grav_dpf")),
    _dT_kernel_diff_dT(getDefaultMaterialProperty<Real>("dT_kernel_diff_dT")),
    _dT_kernel_diff_dpf(getDefaultMaterialProperty<Real>("dT_kernel_diff_dpf")),
    _dT_kernel_diff_dev(getDefaultMaterialProperty<Real>("dT_kernel_diff_dev")),
    _dT_kernel_time_dT(getDefaultMaterialProperty<Real>("dT_kernel_time_dT")),
    _dT_kernel_time_dpf(getDefaultMaterialProperty<Real>("dT_kernel_time_dpf")),
    _dT_kernel_time_dev(getDefaultMaterialProperty<Real>("dT_kernel_time_dev")),
    _SUPG_N(getDefaultMaterialProperty<RealVectorValue>("SUPG_N")),
    _SUPG_dtau_dgradpf(getDefaultMaterialProperty<RankTwoTensor>("SUPG_dtau_dgradpf")),
    _SUPG_dtau_dpf(getDefaultMaterialProperty<RealVectorValue>("SUPG_dtau_dpf")),
    _SUPG_dtau_dT(getDefaultMaterialProperty<RealVectorValue>("SUPG_dtau_dT")),
    _SUPG_dtau_dev(getDefaultMaterialProperty<RealVectorValue>("SUPG_dtau_dev")),
    _pf_var(coupled("pore_pressure")),
    _ndisp(_has_disp ? coupledComponents("displacements") : 0),
    _disp_var(_ndisp)
{
  if (_has_disp)
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = coupled("displacements", i);
  else
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = zero;
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
Real
GolemKernelTH::computeQpResidual()
{
  Real res = 0.0;
  RealVectorValue vel = _TH_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  if (_is_conservative)
  {
    res += _T_kernel_diff[_qp] * _grad_u[_qp] * _grad_test[_i][_qp];
    res += _T_kernel_source[_qp] * _test[_i][_qp];
    res -= vel * _u[_qp] * _grad_test[_i][_qp];
  }
  else
  {
    res += vel * _grad_u[_qp] * _test[_i][_qp];
    if (_has_SUPG_upwind)
    {
      if (!_has_lumped_mass_matrix && _fe_problem.isTransient())
        res += _T_kernel_time[_qp] * _u_dot[_qp] * _SUPG_N[_qp] *
               _grad_test[_i][_qp]; // strong form residual : time
      res += vel * _grad_u[_qp] * _SUPG_N[_qp] *
             _grad_test[_i][_qp]; // strong form residual : advection
    }
  }

  return _scaling_factor[_qp] * res;
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
Real
GolemKernelTH::computeQpJacobian()
{
  Real jac = 0.0;
  RealVectorValue vel = _TH_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  RealVectorValue dvel_dT = _dTH_kernel_dT[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]) +
                            _TH_kernel[_qp] * _dH_kernel_grav_dT[_qp];
  if (_is_conservative)
  {
    jac += _T_kernel_diff[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
    jac += _dT_kernel_diff_dT[_qp] * _grad_u[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
    jac -= vel * _phi[_j][_qp] * _grad_test[_i][_qp];
    jac -= dvel_dT * _u[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
  }
  else
  {
    jac += vel * _grad_phi[_j][_qp] * _test[_i][_qp];
    jac += dvel_dT * _grad_u[_qp] * _phi[_j][_qp] * _test[_i][_qp];
    if (_has_SUPG_upwind)
    {
      if (!_has_lumped_mass_matrix && _fe_problem.isTransient())
      {
        jac += _T_kernel_time[_qp] * _du_dot_du[_qp] * _SUPG_N[_qp] * _phi[_j][_qp] *
               _grad_test[_i][_qp];
        jac += _dT_kernel_time_dT[_qp] * _u_dot[_qp] * _SUPG_N[_qp] * _phi[_j][_qp] *
               _grad_test[_i][_qp];
        jac += _T_kernel_time[_qp] * _u_dot[_qp] * _SUPG_dtau_dT[_qp] * _phi[_j][_qp] *
               _grad_test[_i][_qp];
      }
      jac += vel * _grad_phi[_j][_qp] * _SUPG_N[_qp] * _grad_test[_i][_qp];
      jac += dvel_dT * _grad_u[_qp] * _SUPG_N[_qp] * _grad_test[_i][_qp];
      jac += vel * _grad_u[_qp] * _SUPG_dtau_dT[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
    }
  }

  return _scaling_factor[_qp] * jac;
}

/******************************************************************************/
/*                            OFF DIAGONAL JACOBIAN                           */
/******************************************************************************/
Real
GolemKernelTH::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real jac = 0.0;
  RealVectorValue vel = _TH_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  if (jvar == _pf_var)
  {
    RealVectorValue dvel_dpf =
        _TH_kernel[_qp] * (_grad_phi[_j][_qp] + _dH_kernel_grav_dpf[_qp] * _phi[_j][_qp]) +
        _dTH_kernel_dpf[_qp] * _phi[_j][_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
    if (_is_conservative)
    {
      jac += _dT_kernel_diff_dpf[_qp] * _grad_u[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
      jac -= dvel_dpf * _u[_qp] * _grad_test[_i][_qp];
    }
    else
    {
      jac += dvel_dpf * _grad_u[_qp] * _test[_i][_qp];
      if (_has_SUPG_upwind)
      {
        if (!_has_lumped_mass_matrix && _fe_problem.isTransient())
        {
          jac += _dT_kernel_time_dpf[_qp] * _u_dot[_qp] * _SUPG_N[_qp] * _phi[_j][_qp] *
                 _grad_test[_i][_qp];
          jac += _T_kernel_time[_qp] * _u_dot[_qp] * _SUPG_dtau_dpf[_qp] * _phi[_j][_qp] *
                 _grad_test[_i][_qp];
          jac += _T_kernel_time[_qp] * _u_dot[_qp] * _SUPG_dtau_dgradpf[_qp] * _grad_phi[_j][_qp] *
                 _grad_test[_i][_qp];
        }
        jac += dvel_dpf * _grad_u[_qp] * _SUPG_N[_qp] * _grad_test[_i][_qp];
        jac += vel * _grad_u[_qp] * _SUPG_dtau_dpf[_qp] * _phi[_j][_qp] * _grad_test[_i][_qp];
        jac +=
            vel * _grad_u[_qp] * _SUPG_dtau_dgradpf[_qp] * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
      }
    }
  }
  for (unsigned i = 0; i < _ndisp; ++i)
    if (_has_disp && (jvar == _disp_var[i]))
    {
      RealVectorValue dvel_dev = _dTH_kernel_dev[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
      if (_is_conservative)
      {
        jac +=
            _dT_kernel_diff_dev[_qp] * _grad_u[_qp] * _grad_phi[_j][_qp](i) * _grad_test[_i][_qp];
        jac -= dvel_dev * _u[_qp] * _grad_phi[_j][_qp](i) * _grad_test[_i][_qp];
      }
      else
      {
        jac += dvel_dev * _grad_u[_qp] * _grad_phi[_j][_qp](i) * _test[_i][_qp];
        if (_has_SUPG_upwind)
        {
          if (!_has_lumped_mass_matrix && _fe_problem.isTransient())
          {
            jac += _dT_kernel_time_dev[_qp] * _u_dot[_qp] * _SUPG_N[_qp] * _grad_phi[_j][_qp](i) *
                   _grad_test[_i][_qp];
            jac += _T_kernel_time[_qp] * _u_dot[_qp] * _SUPG_dtau_dev[_qp] * _grad_phi[_j][_qp](i) *
                   _grad_test[_i][_qp];
          }
          jac +=
              dvel_dev * _grad_u[_qp] * _SUPG_N[_qp] * _grad_phi[_j][_qp](i) * _grad_test[_i][_qp];
          jac += vel * _grad_u[_qp] * _SUPG_dtau_dev[_qp] * _grad_phi[_j][_qp](i) *
                 _grad_test[_i][_qp];
        }
      }
    }
  return _scaling_factor[_qp] * jac;
}
