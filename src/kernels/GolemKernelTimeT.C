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

#include "GolemKernelTimeT.h"
#include "Assembly.h"
#include "libmesh/quadrature.h"

registerMooseObject("GolemApp", GolemKernelTimeT);

InputParameters
GolemKernelTimeT::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  params.addParam<bool>("has_lumped_mass_matrix", false, "Has mass lumped?");
  params.addParam<bool>("has_boussinesq", false, "Has Boussinesq terms?");
  return params;
}

GolemKernelTimeT::GolemKernelTimeT(const InputParameters & parameters)
  : DerivativeMaterialInterface<TimeDerivative>(parameters),
    _has_lumped_mass_matrix(getParam<bool>("has_lumped_mass_matrix")),
    _has_boussinesq(getParam<bool>("has_boussinesq")),
    _u_old(valueOld()),
    _has_pf(isCoupled("pore_pressure")),
    _has_disp(isCoupled("displacements")),
    _scaling_factor(getMaterialProperty<Real>("scaling_factor")),
    _T_kernel_time(getMaterialProperty<Real>("T_kernel_time")),
    _dT_kernel_time_dT(getDefaultMaterialProperty<Real>("dT_kernel_time_dT")),
    _dT_kernel_time_dpf(getDefaultMaterialProperty<Real>("dT_kernel_time_dpf")),
    _dT_kernel_time_dev(getDefaultMaterialProperty<Real>("dT_kernel_time_dev")),
    _node_number(_has_lumped_mass_matrix ? &getMaterialProperty<unsigned int>("node_number")
                                         : NULL),
    _nodal_temp(_has_lumped_mass_matrix ? &getMaterialProperty<Real>("nodal_temp") : NULL),
    _nodal_temp_old(_has_lumped_mass_matrix ? &getMaterialProperty<Real>("nodal_temp_old") : NULL),
    _nodal_pf((_has_pf && (_has_boussinesq && _has_lumped_mass_matrix))
                  ? &getMaterialProperty<Real>("nodal_pf")
                  : NULL),
    _nodal_pf_old((_has_pf && (_has_boussinesq && _has_lumped_mass_matrix))
                      ? &getMaterialProperty<Real>("nodal_pf_old")
                      : NULL),
    _pf((_has_pf && _has_boussinesq) ? &coupledValue("pore_pressure") : NULL),
    _pf_old((_has_pf && _has_boussinesq) ? &coupledValueOld("pore_pressure") : NULL),
    _pf_var(_has_pf ? coupled("pore_pressure") : zero),
    _ndisp(_has_disp ? coupledComponents("displacements") : 0),
    _disp_var(_ndisp)
{
  if (_has_disp)
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = coupled("displacements", i);
  else
    for (unsigned i = 0; i < _ndisp; ++i)
      _disp_var[i] = zero;

  if (_has_boussinesq && !_has_pf)
  {
    _has_boussinesq = false;
    mooseWarning("GolemKernelTimeT: using Boussinesq terms but you did not provide the pore "
                 "pressure coupled variable. Ignoring Boussinesq terms.\n");
  }
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
void
GolemKernelTimeT::computeResidual()
{
  prepareVectorTag(_assembly, _var.number());

  precalculateResidual();

  Real inv_dt = 1.0 / _dt;
  Real weight = 0.0;
  Real dT_dt = 0.0;
  Real dp_dt = 0.0;
  Real res = 0.0;
  if (_has_lumped_mass_matrix)
  {
    _qp_map.assign(_test.size(), -1);
    for (unsigned _qp = 0; _qp < _qrule->n_points(); ++_qp)
      _qp_map[(*_node_number)[_qp]] = _qp;
    for (_i = 0; _i < _test.size(); ++_i)
    {
      const int _qp_nodal = _qp_map[_i];
      if (_qp_nodal == -1)
        return;
      weight = (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      dT_dt = ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) * inv_dt;
      res = 0.0;
      res += _T_kernel_time[_qp_nodal] * dT_dt;
      if (_has_boussinesq)
      {
        dp_dt = ((*_nodal_pf)[_qp_nodal] - (*_nodal_pf_old)[_qp_nodal]) * inv_dt;
        // Add pressure term
        res += _dT_kernel_time_dpf[_qp_nodal] * (*_nodal_temp)[_qp_nodal] * dp_dt;
        // Add temperature term
        res += _dT_kernel_time_dT[_qp_nodal] * (*_nodal_temp)[_qp_nodal] * dT_dt;
      }
      _local_re(_i) += _scaling_factor[_qp_nodal] * weight * res;
    }
  }
  else
  {
    for (_i = 0; _i < _test.size(); _i++)
      for (_qp = 0; _qp < _qrule->n_points(); _qp++)
      {
        weight = _JxW[_qp] * _coord[_qp] * _test[_i][_qp];
        dT_dt = (_u[_qp] - _u_old[_qp]) * inv_dt;
        res = 0.0;
        res += _T_kernel_time[_qp] * dT_dt;
        if (_has_boussinesq)
        {
          dp_dt = ((*_pf)[_qp] - (*_pf_old)[_qp]) * inv_dt;
          // Add pressure term
          res += _dT_kernel_time_dpf[_qp] * _u[_qp] * dp_dt;
          // Add temperature term
          res += _dT_kernel_time_dT[_qp] * _u[_qp] * dT_dt;
        }
        _local_re(_i) += _scaling_factor[_qp] * weight * res;
      }
  }

  accumulateTaggedLocalResidual();

  if (_has_save_in)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (const auto & var : _save_in)
      var->sys().solution().add_vector(_local_re, var->dofIndices());
  }
}

Real
GolemKernelTimeT::computeQpResidual()
{
  mooseError("GolemKernelTimeT : computeQpResidual should not be called!!");
  return 0.0;
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
void
GolemKernelTimeT::computeJacobian()
{
  prepareMatrixTag(_assembly, _var.number(), _var.number());

  precalculateJacobian();

  Real inv_dt = 1.0 / _dt;
  Real weight = 0.0;
  Real dT_dt = 0.0;
  Real dp_dt = 0.0;
  Real jac = 0.0;
  if (_has_lumped_mass_matrix)
  {
    _qp_map.assign(_test.size(), -1);
    for (unsigned _qp = 0; _qp < _qrule->n_points(); ++_qp)
      _qp_map[(*_node_number)[_qp]] = _qp;
    for (_i = 0; _i < _test.size(); ++_i)
    {
      const int _qp_nodal = _qp_map[_i];
      if (_qp_nodal == -1)
        return;
      weight = (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      jac = 0.0;
      jac += inv_dt * _T_kernel_time[_qp_nodal];
      if (_has_pf)
      {
        dT_dt = ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) * inv_dt;
        jac += _dT_kernel_time_dT[_qp_nodal] * dT_dt;
        if (_has_boussinesq)
        {
          dp_dt = ((*_nodal_pf)[_qp_nodal] - (*_nodal_pf_old)[_qp_nodal]) * inv_dt;
          // Add pressure term --> we assume that second order terms are negligible
          jac += _dT_kernel_time_dpf[_qp_nodal] * dp_dt;
          // Add temperature terms --> we assume that second order terms are negligible
          jac += _dT_kernel_time_dT[_qp_nodal] * dT_dt;
          jac += _dT_kernel_time_dT[_qp_nodal] * (*_nodal_temp)[_qp_nodal] * inv_dt;
        }
      }
      _local_ke(_i, _i) += _scaling_factor[_qp_nodal] * weight * jac;
    }
  }
  else
  {
    for (_i = 0; _i < _test.size(); _i++)
      for (_j = 0; _j < _phi.size(); _j++)
        for (_qp = 0; _qp < _qrule->n_points(); _qp++)
        {
          weight = _JxW[_qp] * _coord[_qp] * _test[_i][_qp] * _phi[_j][_qp];
          jac = 0.0;
          jac += _T_kernel_time[_qp] * inv_dt;
          if (_has_pf)
          {
            dT_dt = (_u[_qp] - _u_old[_qp]) * inv_dt;
            jac += _dT_kernel_time_dT[_qp] * dT_dt;
            if (_has_boussinesq)
            {
              dp_dt = ((*_pf)[_qp] - (*_pf_old)[_qp]) * inv_dt;
              // Add pressure term --> we assume that second order terms are negligible
              jac += _dT_kernel_time_dpf[_qp] * dp_dt;
              // Add temperature terms --> we assume that second order terms are negligible
              jac += _dT_kernel_time_dT[_qp] * dT_dt;
              jac += _dT_kernel_time_dT[_qp] * _u[_qp] * inv_dt;
            }
          }
          _local_ke(_i, _j) += _scaling_factor[_qp] * weight * jac;
        }
  }
  
  accumulateTaggedLocalMatrix();

  if (_has_diag_save_in && !_sys.computingScalingJacobian())
  {
    DenseVector<Number> diag = _assembly.getJacobianDiagonal(_local_ke);
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (const auto & var : _diag_save_in)
      var->sys().solution().add_vector(diag, var->dofIndices());
  }
}

Real
GolemKernelTimeT::computeQpJacobian()
{
  mooseError("GolemKernelTimeT : computeQpJacobian should not be called!!");
  return 0.0;
}

/******************************************************************************/
/*                            OFF DIAGONAL JACOBIAN                           */
/******************************************************************************/
void
GolemKernelTimeT::computeOffDiagJacobian(const unsigned int jvar_num)
{
  if (jvar_num == _var.number())
    computeJacobian();

  // DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar_num);
  prepareMatrixTag(_assembly, _var.number(), jvar_num);

  Real inv_dt = 1.0 / _dt;
  Real weight = 0.0;
  Real dT_dt = 0.0;
  Real jac = 0.0;
  if (_has_lumped_mass_matrix)
  {
    _qp_map.assign(_test.size(), -1);
    for (unsigned _qp = 0; _qp < _qrule->n_points(); ++_qp)
      _qp_map[(*_node_number)[_qp]] = _qp;
    for (_i = 0; _i < _test.size(); ++_i)
    {
      const int _qp_nodal = _qp_map[_i];
      if (_qp_nodal == -1)
        return;
      weight = (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      dT_dt = ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) * inv_dt;
      jac = 0.0;
      if (_has_pf && (jvar_num == _pf_var))
      {
        jac += _dT_kernel_time_dpf[_qp_nodal] * dT_dt;
        if (_has_boussinesq)
          jac += _dT_kernel_time_dpf[_qp_nodal] * inv_dt * (*_nodal_temp)[_qp_nodal];
      }
      for (unsigned i = 0; i < _ndisp; ++i)
        if ((_has_disp && _has_pf) && (jvar_num == _disp_var[i]))
          jac += _dT_kernel_time_dev[_qp_nodal] * dT_dt;

      _local_ke(_i, _i) += _scaling_factor[_qp_nodal] * weight * jac;
    }
  }
  else
  {
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      for (_i = 0; _i < _test.size(); ++_i)
        for (_j = 0; _j < _phi.size(); ++_j)
        {
          weight = _JxW[_qp] * _coord[_qp] * _test[_i][_qp];
          dT_dt = (_u[_qp] - _u_old[_qp]) * inv_dt;
          jac = 0.0;
          if (_has_pf && (jvar_num == _pf_var))
          {
            jac += _dT_kernel_time_dpf[_qp] * dT_dt * _phi[_j][_qp];
            if (_has_boussinesq)
            {
              jac += _dT_kernel_time_dpf[_qp] * inv_dt * _u[_qp] * _phi[_j][_qp];
            }
          }
          for (unsigned i = 0; i < _ndisp; ++i)
            if ((_has_disp && _has_pf) && (jvar_num == _disp_var[i]))
              jac += _dT_kernel_time_dev[_qp] * dT_dt * _grad_phi[_j][_qp](i);

          _local_ke(_i, _j) += _scaling_factor[_qp] * weight * jac;
        }
  }
}

Real
GolemKernelTimeT::computeQpOffDiagJacobian(unsigned int)
{
  mooseError("GolemKernelTimeT : computeQpOffDiagJacobian should not be called!!");
  return 0.0;
}
