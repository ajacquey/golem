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

#include "GolemMaterialTH.h"
#include "MooseMesh.h"
#include "libmesh/quadrature.h"

registerMooseObject("GolemApp", GolemMaterialTH);

template <>
InputParameters
validParams<GolemMaterialTH>()
{
  InputParameters params = validParams<GolemMaterialH>();
  params.addRequiredCoupledVar("temperature", "The temperature");
  params.addRequiredCoupledVar("pore_pressure", "The pore pressure");
  params.addParam<bool>(
      "has_heat_source_sink", false, "Has source/sink of temperature considered?");
  params.addParam<bool>("has_lumped_mass_matrix", false, "Has lumped mass matrix?");
  params.addParam<bool>("has_boussinesq", false, "Has Boussinesq terms?");
  params.addRequiredParam<Real>("fluid_thermal_conductivity_initial",
                                "The fluid thermal conductivity [W/m/K].");
  params.addRequiredParam<Real>("solid_thermal_conductivity_initial",
                                "The solid thermal conductivity [W/m/K].");
  params.addParam<Real>("fluid_heat_capacity_initial", 0.0, "The fluid heat capacity [J/m^3/K].");
  params.addParam<Real>("solid_heat_capacity_initial", 0.0, "The solid heat capacity [J/m^3/K].");
  params.addParam<Real>("heat_source_sink", 0.0, "The heat source or sink [W/m^3].");
  params.addParam<UserObjectName>("supg_uo", "The name of the SUPG user object.");
  params.addRequiredParam<UserObjectName>("fluid_density_uo",
                                          "The name of the fluid density user object.");
  params.addRequiredParam<UserObjectName>("fluid_viscosity_uo",
                                          "The name of the fluid viscosity user object.");
  params.addClassDescription("This is the base class for Thermo-Hydraulic (TH) Golem material "
                             "properties. It inherits from GolmMaterialH all related hydraulic "
                             "properties.");
  return params;
}

GolemMaterialTH::GolemMaterialTH(const InputParameters & parameters)
  : GolemMaterialH(parameters),
    _has_T_source_sink(getParam<bool>("has_heat_source_sink")),
    _has_SUPG_upwind(isParamValid("supg_uo") ? true : false),
    _has_lumped_mass_matrix(getParam<bool>("has_lumped_mass_matrix")),
    _has_boussinesq(getParam<bool>("has_boussinesq")),
    _temp(coupledValue("temperature")),
    _pf(coupledValue("pore_pressure")),
    _grad_pf(coupledGradient("pore_pressure")),
    _grad_temp(coupledGradient("temperature")),
    _lambda_f(getParam<Real>("fluid_thermal_conductivity_initial")),
    _lambda_s(getParam<Real>("solid_thermal_conductivity_initial")),
    _c_f(getParam<Real>("fluid_heat_capacity_initial")),
    _c_s(getParam<Real>("solid_heat_capacity_initial")),
    _T_source_sink(getParam<Real>("heat_source_sink")),
    _supg_uo(_has_SUPG_upwind ? &getUserObject<GolemSUPG>("supg_uo") : NULL),
    _TH_kernel(declareProperty<RankTwoTensor>("TH_kernel")),
    _T_kernel_diff(declareProperty<Real>("T_kernel_diff")),
    _drho_dpf(declareProperty<Real>("drho_dp")),
    _drho_dT(declareProperty<Real>("drho_dT")),
    _dmu_dpf(declareProperty<Real>("dmu_dp")),
    _dmu_dT(declareProperty<Real>("dmu_dT")),
    _dH_kernel_dT(declareProperty<RankTwoTensor>("dH_kernel_dT")),
    _dH_kernel_grav_dpf(declareProperty<RealVectorValue>("dH_kernel_grav_dpf")),
    _dH_kernel_grav_dT(declareProperty<RealVectorValue>("dH_kernel_grav_dT")),
    _dTH_kernel_dpf(declareProperty<RankTwoTensor>("dTH_kernel_dpf")),
    _dTH_kernel_dT(declareProperty<RankTwoTensor>("dTH_kernel_dT")),
    _SUPG_N(declareProperty<RealVectorValue>("SUPG_N")),
    _SUPG_dtau_dgradpf(declareProperty<RankTwoTensor>("SUPG_dtau_dgradpf")),
    _SUPG_dtau_dpf(declareProperty<RealVectorValue>("SUPG_dtau_dpf")),
    _SUPG_dtau_dT(declareProperty<RealVectorValue>("SUPG_dtau_dT"))
{
  // scale properties if required
  if (_has_scaled_properties)
  {
    _lambda_s /= _scaling_uo->_s_conductivity;
    _lambda_f /= _scaling_uo->_s_conductivity;
    _c_s /= _scaling_uo->_s_specific_heat;
    _c_f /= _scaling_uo->_s_specific_heat;
    _T_source_sink /= _scaling_uo->_s_heat_production;
  }
  if (_fe_problem.isTransient())
  {
    _T_kernel_time = &declareProperty<Real>("T_kernel_time");
    _dT_kernel_time_dpf = &declareProperty<Real>("dT_kernel_time_dpf");
    _dT_kernel_time_dT = &declareProperty<Real>("dT_kernel_time_dT");
  }
  if (_has_T_source_sink)
    _T_kernel_source = &declareProperty<Real>("T_kernel_source");
  if (_has_lumped_mass_matrix)
  {
    if (!_fe_problem.isTransient())
      mooseError("_has_lumped_mass_matrix is set to ",
                 _has_lumped_mass_matrix,
                 " but simulation is Steady State");

    // Set the mapping from qps to nodes
    _node_number = &declareProperty<unsigned int>("node_number");
    // Pressure nodal
    _nodal_pf_var = &coupledNodalValue("pore_pressure");
    _nodal_pf = &declareProperty<Real>("nodal_pf");
    if (_has_boussinesq)
    {
      _nodal_pf_var_old = &coupledNodalValueOld("pore_pressure");
      _nodal_pf_old = &declareProperty<Real>("nodal_pf_old");
    }
    // Temperature nodal
    _nodal_temp_var = &coupledNodalValue("temperature");
    _nodal_temp_var_old = &coupledNodalValueOld("temperature");
    _nodal_temp = &declareProperty<Real>("nodal_temp");
    _nodal_temp_old = &declareProperty<Real>("nodal_temp_old");
  }
  if (_has_disp)
  {
    // Declare some property when this material is used for fractures or faults in a THM simulation
    _dT_kernel_diff_dev = &declareProperty<Real>("dT_kernel_diff_dev");
    _dT_kernel_diff_dpf = &declareProperty<Real>("dT_kernel_diff_dpf");
    _dT_kernel_diff_dT = &declareProperty<Real>("dT_kernel_diff_dT");
    _dTH_kernel_dev = &declareProperty<RankTwoTensor>("dTH_kernel_dev");
    if (_fe_problem.isTransient())
    {
      _dT_kernel_time_dev = &declareProperty<Real>("dT_kernel_time_dev");
      _dH_kernel_time_dT = &declareProperty<Real>("dH_kernel_time_dT");
    }
    _SUPG_dtau_dev = &declareProperty<RealVectorValue>("SUPG_dtau_dev");
  }
  else
    _dH_kernel_dpf = &declareProperty<RankTwoTensor>("dH_kernel_dpf");
}

void
GolemMaterialTH::computeProperties()
{
  if (_current_elem->dim() < _mesh.dimension())
    computeRotationMatrix();
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    computeQpProperties();
}

void
GolemMaterialTH::computeQpProperties()
{
  if (_has_lumped_mass_matrix)
  {
    (*_node_number)[_qp] = nearest();
    (*_nodal_temp)[_qp] = (*_nodal_temp_var)[(*_node_number)[_qp]];
    (*_nodal_temp_old)[_qp] = (*_nodal_temp_var_old)[(*_node_number)[_qp]];
    (*_nodal_pf)[_qp] = (*_nodal_pf_var)[(*_node_number)[_qp]];
    if (_has_boussinesq)
      (*_nodal_pf_old)[_qp] = (*_nodal_pf_var_old)[(*_node_number)[_qp]];
  }
  _scaling_factor[_qp] = computeQpScaling();
  computeDensity();
  computeViscosity();
  _porosity[_qp] = _porosity_uo->computePorosity(_phi0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  _permeability[_qp] =
      _permeability_uo->computePermeability(_k0, _phi0, _porosity[_qp], _scaling_factor[_qp]);
  // GolemKernelT related properties
  _T_kernel_diff[_qp] = _porosity[_qp] * _lambda_f + (1.0 - _porosity[_qp]) * _lambda_s;
  if (_has_T_source_sink)
    (*_T_kernel_source)[_qp] = -1.0 * _T_source_sink;
  // GolemKernelH related properties
  GolemPropertiesH();
  // GolemkernelTH related poperties
  _TH_kernel[_qp] = -_H_kernel[_qp] * _fluid_density[_qp] * _c_f;
  if (_fe_problem.isTransient())
  {
    // Correct H_kernel_time
    if (_drho_dpf[_qp] != 0.0)
      (*_H_kernel_time)[_qp] = (_porosity[_qp] * _drho_dpf[_qp]) / _fluid_density[_qp];
    (*_T_kernel_time)[_qp] =
        _porosity[_qp] * _fluid_density[_qp] * _c_f + (1.0 - _porosity[_qp]) * _rho0_s * _c_s;
  }
  // Properties derivatives
  // H_kernel derivatives
  (*_dH_kernel_dpf)[_qp] = -_H_kernel[_qp] * _dmu_dpf[_qp] / _fluid_viscosity[_qp];
  _dH_kernel_dT[_qp] = -_H_kernel[_qp] * _dmu_dT[_qp] / _fluid_viscosity[_qp];
  // H_kernel_grav derivatives
  _dH_kernel_grav_dpf[_qp] = -_drho_dpf[_qp] * _gravity;
  _dH_kernel_grav_dT[_qp] = -_drho_dT[_qp] * _gravity;
  if (_fe_problem.isTransient())
  {
    // T_kernel_time
    (*_dT_kernel_time_dpf)[_qp] = _drho_dpf[_qp] * _porosity[_qp] * _c_f;
    (*_dT_kernel_time_dT)[_qp] = _drho_dT[_qp] * _porosity[_qp] * _c_f;
  }
  // TH_kernel derivatives
  _dTH_kernel_dpf[_qp] = -(_fluid_density[_qp] * _c_f * (*_dH_kernel_dpf)[_qp] +
                           _H_kernel[_qp] * _c_f * _drho_dpf[_qp]);
  _dTH_kernel_dT[_qp] =
      -(_fluid_density[_qp] * _c_f * _dH_kernel_dT[_qp] + _H_kernel[_qp] * _c_f * _drho_dT[_qp]);
  if (_has_SUPG_upwind)
    computeQpSUPG();
  if (_has_disp)
  {
    // Declare some property when this material is used for fractures or faults in a THM simulation
    (*_dH_kernel_dev)[_qp] = RankTwoTensor();
    (*_dT_kernel_diff_dev)[_qp] = 0.0;
    (*_dT_kernel_diff_dpf)[_qp] = 0.0;
    (*_dT_kernel_diff_dT)[_qp] = 0.0;
    if (_fe_problem.isTransient())
    {
      (*_dT_kernel_time_dev)[_qp] = 0.0;
      (*_dH_kernel_time_dev)[_qp] = 0.0;
      (*_dH_kernel_time_dpf)[_qp] = 0.0;
      (*_dH_kernel_time_dT)[_qp] = 0.0;
    }
  }
}

void
GolemMaterialTH::computeDensity()
{
  Real pres = 0.0;
  Real temp = 0.0;
  if (_has_lumped_mass_matrix)
  {
    pres = (*_nodal_pf)[_qp];
    temp = (*_nodal_temp)[_qp];
  }
  else
  {
    pres = _pf[_qp];
    temp = _temp[_qp];
  }
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(pres, temp, _rho0_f);
  _drho_dT[_qp] = _fluid_density_uo->computedDensitydT(pres, temp, _rho0_f);
  _drho_dpf[_qp] = _fluid_density_uo->computedDensitydp(pres, temp);
}

void
GolemMaterialTH::computeViscosity()
{
  Real temp = 0.0;
  if (_has_lumped_mass_matrix)
    temp = (*_nodal_temp)[_qp];
  else
    temp = _temp[_qp];
  _fluid_viscosity[_qp] = _fluid_viscosity_uo->computeViscosity(temp, _fluid_density[_qp], _mu0);
  _dmu_dT[_qp] =
      _fluid_viscosity_uo->computedViscositydT(temp, _fluid_density[_qp], _drho_dT[_qp], _mu0);
  _dmu_dpf[_qp] =
      _fluid_viscosity_uo->computedViscositydp(temp, _fluid_density[_qp], _drho_dpf[_qp]);
}

void
GolemMaterialTH::computeQpSUPG()
{
  RealVectorValue vel = -_H_kernel[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]);
  RankTwoTensor dvel_dgradp = -_H_kernel[_qp];
  RealVectorValue dvel_dp = -(*_dH_kernel_dpf)[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]) -
                            _H_kernel[_qp] * _dH_kernel_grav_dpf[_qp];
  RealVectorValue dvel_dT = -_dH_kernel_dT[_qp] * (_grad_pf[_qp] + _H_kernel_grav[_qp]) -
                            _H_kernel[_qp] * _dH_kernel_grav_dT[_qp];
  Real diff = _T_kernel_diff[_qp] / (*_T_kernel_time)[_qp];
  Real tau = _supg_uo->tau(vel, diff, _dt, _current_elem);
  _SUPG_N[_qp] = tau * vel;
  _SUPG_dtau_dgradpf[_qp] = tau * dvel_dgradp;
  _SUPG_dtau_dpf[_qp] = tau * dvel_dp;
  _SUPG_dtau_dT[_qp] = tau * dvel_dT;
  if (_has_disp)
    (*_SUPG_dtau_dev)[_qp] = 0.0;
}

unsigned
GolemMaterialTH::nearest()
{
  unsigned n = 0;
  Real smallest_dist = std::numeric_limits<Real>::max();
  for (unsigned int i = 0; i < _current_elem->n_nodes(); ++i)
  {
    const Real this_dist = (_current_elem->point(i) - _q_point[_qp]).size();
    if (this_dist < smallest_dist)
    {
      n = i;
      smallest_dist = this_dist;
    }
  }
  return n;
}