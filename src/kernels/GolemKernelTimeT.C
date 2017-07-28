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

template <>
InputParameters
validParams<GolemKernelTimeT>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("displacements", "The displacement variables vector.");
  params.addParam<bool>("has_lumped_mass_matrix", false, "Has mass lumped?");
  return params;
}

GolemKernelTimeT::GolemKernelTimeT(const InputParameters & parameters)
  : DerivativeMaterialInterface<TimeDerivative>(parameters),
    _has_lumped_mass_matrix(getParam<bool>("has_lumped_mass_matrix")),
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
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
void
GolemKernelTimeT::computeResidual()
{
  DenseVector<Number> & re = _assembly.residualBlock(_var.number(), Moose::KT_TIME);
  _local_re.resize(re.size());
  _local_re.zero();
  Real inv_dt = 1.0 / _dt;
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
      Real weight =
          (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      _local_re(_i) += _scaling_factor[_qp_nodal] * weight * inv_dt * _T_kernel_time[_qp_nodal] *
                       ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]);
    }
  }
  else
  {
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      for (_i = 0; _i < _test.size(); ++_i)
        _local_re(_i) += _scaling_factor[_qp] * _JxW[_qp] * _coord[_qp] * inv_dt *
                         _T_kernel_time[_qp] * (_u[_qp] - _u_old[_qp]) * _test[_i][_qp];
  }
  re += _local_re;
  if (_has_save_in)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _save_in.size(); ++i)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
  }
}

Real
GolemKernelTimeT::computeQpResidual()
{
  mooseError("GolemKernelTimeT : computeQpResidual should not be called!!");
  return 0.0;
}

/******************************************************************************/
/*                          JACOBIAN                                          */
/******************************************************************************/
void
GolemKernelTimeT::computeJacobian()
{
  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), _var.number());
  _local_ke.resize(ke.m(), ke.n());
  _local_ke.zero();
  Real inv_dt = 1.0 / _dt;
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
      Real weight =
          (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      _local_ke(_i, _i) += _scaling_factor[_qp_nodal] * weight * inv_dt * _T_kernel_time[_qp_nodal];
      if (_has_pf)
        for (_j = 0; _j < _phi.size(); ++_j)
          _local_ke(_i, _j) +=
              _scaling_factor[_qp_nodal] * weight * inv_dt * _dT_kernel_time_dT[_qp_nodal] *
              ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) * _phi[_j][_qp_nodal];
    }
  }
  else
  {
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      for (_i = 0; _i < _test.size(); ++_i)
        for (_j = 0; _j < _phi.size(); ++_j)
        {
          _local_ke(_i, _j) += _scaling_factor[_qp] * _JxW[_qp] * _coord[_qp] * inv_dt *
                               _T_kernel_time[_qp] * _test[_i][_qp] * _phi[_j][_qp];
          if (_has_pf)
            _local_ke(_i, _j) += _scaling_factor[_qp] * _JxW[_qp] * _coord[_qp] *
                                 (_u[_qp] - _u_old[_qp]) * inv_dt * _dT_kernel_time_dT[_qp] *
                                 _test[_i][_qp] * _phi[_j][_qp];
        }
  }
  ke += _local_ke;
  if (_has_diag_save_in)
  {
    unsigned int rows = ke.m();
    DenseVector<Number> diag(rows);
    for (unsigned int i = 0; i < rows; ++i)
      diag(i) = _local_ke(i, i);
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
/*                     OFF DIAGONAL JACOBIAN                                  */
/******************************************************************************/
void
GolemKernelTimeT::computeOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _var.number())
    computeJacobian();

  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar);
  Real inv_dt = 1.0 / _dt;
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
      Real weight =
          (_current_elem->volume() * _scaling_factor[_qp_nodal]) / _current_elem->n_nodes();
      for (_j = 0; _j < _phi.size(); ++_j)
      {
        if (_has_pf && (jvar == _pf_var))
          ke(_i, _j) +=
              _scaling_factor[_qp_nodal] * weight * inv_dt * _dT_kernel_time_dpf[_qp_nodal] *
              ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) * _phi[_j][_qp_nodal];

        for (unsigned i = 0; i < _ndisp; ++i)
          if ((_has_disp && _has_pf) && (jvar == _disp_var[i]))
            ke(_i, _j) += _scaling_factor[_qp_nodal] * weight * inv_dt *
                          _dT_kernel_time_dev[_qp_nodal] *
                          ((*_nodal_temp)[_qp_nodal] - (*_nodal_temp_old)[_qp_nodal]) *
                          _grad_phi[_j][_qp_nodal](i);
      }
    }
  }
  else
  {
    for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      for (_i = 0; _i < _test.size(); ++_i)
        for (_j = 0; _j < _phi.size(); ++_j)
        {
          if (_has_pf && (jvar == _pf_var))
            ke(_i, _j) += _scaling_factor[_qp] * _JxW[_qp] * _coord[_qp] * inv_dt *
                          _dT_kernel_time_dpf[_qp] * (_u[_qp] - _u_old[_qp]) * _test[_i][_qp] *
                          _phi[_j][_qp];

          for (unsigned i = 0; i < _ndisp; ++i)
            if ((_has_disp && _has_pf) && (jvar == _disp_var[i]))
              ke(_i, _j) += _scaling_factor[_qp] * _JxW[_qp] * _coord[_qp] * inv_dt *
                            _dT_kernel_time_dev[_qp] * (_u[_qp] - _u_old[_qp]) * _test[_i][_qp] *
                            _grad_phi[_j][_qp](i);
        }
  }
}

Real
GolemKernelTimeT::computeQpOffDiagJacobian(unsigned int)
{
  mooseError("GolemKernelTimeT : computeQpOffDiagJacobian should not be called!!");
  return 0.0;
}
