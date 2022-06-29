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

#include "GolemMaterialH.h"
#include "GolemSUPG.h"
#include "GolemFluidDensity.h"
#include "GolemFluidViscosity.h"

class GolemMaterialTH : public GolemMaterialH
{
public:
  static InputParameters validParams();
  GolemMaterialTH(const InputParameters & parameters);

protected:
  virtual void computeProperties();
  virtual void computeQpProperties();
  virtual void computeDensity();
  virtual void computeViscosity();
  virtual void computeQpSUPG();
  virtual unsigned nearest();

  bool _has_T_source_sink;
  bool _has_SUPG_upwind;
  bool _has_lumped_mass_matrix;
  bool _has_boussinesq;
  const VariableValue & _temp;
  const VariableValue & _pf;
  const VariableGradient & _grad_pf;
  const VariableGradient & _grad_temp;
  Real _lambda_f;
  Real _lambda_s;
  Real _c_f;
  Real _c_s;
  Real _T_source_sink;
  // UserObjects
  const GolemSUPG * _supg_uo;
  // Kernels related material properties
  MaterialProperty<RankTwoTensor> & _TH_kernel;
  MaterialProperty<Real> & _T_kernel_diff;
  MaterialProperty<Real> * _T_kernel_source;
  MaterialProperty<Real> * _T_kernel_time;
  // Properties derivatives
  MaterialProperty<Real> & _drho_dpf;
  MaterialProperty<Real> & _drho_dT;
  MaterialProperty<Real> & _dmu_dpf;
  MaterialProperty<Real> & _dmu_dT;
  // H_kernel derivatives
  MaterialProperty<RankTwoTensor> & _dH_kernel_dT;
  // H_kernel_grav derivatives
  MaterialProperty<RealVectorValue> & _dH_kernel_grav_dpf;
  MaterialProperty<RealVectorValue> & _dH_kernel_grav_dT;
  // TH_kernel derivatives
  MaterialProperty<RankTwoTensor> & _dTH_kernel_dpf;
  MaterialProperty<RankTwoTensor> & _dTH_kernel_dT;
  // T_kernel_time derivatives
  MaterialProperty<Real> * _dT_kernel_time_dpf;
  MaterialProperty<Real> * _dT_kernel_time_dT;
  // SUPG related material properties
  MaterialProperty<RealVectorValue> & _SUPG_N;
  MaterialProperty<RankTwoTensor> & _SUPG_dtau_dgradpf;
  MaterialProperty<RealVectorValue> & _SUPG_dtau_dpf;
  MaterialProperty<RealVectorValue> & _SUPG_dtau_dT;
  // nodal values related material properties --> for lumping the mass matrix
  MaterialProperty<unsigned int> * _node_number;
  MaterialProperty<Real> * _nodal_pf;
  MaterialProperty<Real> * _nodal_pf_old;
  MaterialProperty<Real> * _nodal_temp;
  MaterialProperty<Real> * _nodal_temp_old;
  const VariableValue * _nodal_pf_var;
  const VariableValue * _nodal_pf_var_old;
  const VariableValue * _nodal_temp_var;
  const VariableValue * _nodal_temp_var_old;
  // Additional properties when using this material for frac and fault in THM simulations
  MaterialProperty<Real> * _dT_kernel_diff_dev;
  MaterialProperty<Real> * _dT_kernel_diff_dpf;
  MaterialProperty<Real> * _dT_kernel_diff_dT;
  MaterialProperty<RankTwoTensor> * _dTH_kernel_dev;
  MaterialProperty<Real> * _dT_kernel_time_dev;
  MaterialProperty<Real> * _dH_kernel_time_dT;
  MaterialProperty<RealVectorValue> * _SUPG_dtau_dev;
};