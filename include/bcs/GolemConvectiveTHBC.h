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

#include "IntegratedBC.h"
#include "DerivativeMaterialInterface.h"
#include "RankTwoTensor.h"

class GolemConvectiveTHBC : public DerivativeMaterialInterface<IntegratedBC>
{
public:
  static InputParameters validParams();
  GolemConvectiveTHBC(const InputParameters & params);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int) override;

  bool _has_disp;
  const VariableGradient & _grad_pf;
  const MaterialProperty<Real> & _scaling_factor;
  const MaterialProperty<RealVectorValue> & _H_kernel_grav;
  const MaterialProperty<RankTwoTensor> & _TH_kernel;
  const MaterialProperty<Real> & _T_kernel_diff;
  // Properties derivatives
  const MaterialProperty<RankTwoTensor> & _dTH_kernel_dT;
  const MaterialProperty<RankTwoTensor> & _dTH_kernel_dpf;
  const MaterialProperty<RealVectorValue> & _dH_kernel_grav_dT;
  const MaterialProperty<RealVectorValue> & _dH_kernel_grav_dpf;
  const MaterialProperty<Real> & _dT_kernel_diff_dT;
  const MaterialProperty<Real> & _dT_kernel_diff_dpf;
  const MaterialProperty<Real> & _dT_kernel_diff_dev;

private:
  unsigned int _p_var;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;
};