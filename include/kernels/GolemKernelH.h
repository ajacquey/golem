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

#include "Kernel.h"
#include "DerivativeMaterialInterface.h"
#include "RankTwoTensor.h"

class GolemKernelH : public DerivativeMaterialInterface<Kernel>
{
public:
  static InputParameters validParams();
  GolemKernelH(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  bool _has_T;
  bool _has_disp;
  bool _has_boussinesq;
  const MaterialProperty<Real> & _scaling_factor;
  const MaterialProperty<RealVectorValue> & _H_kernel_grav;
  const MaterialProperty<RankTwoTensor> & _H_kernel;
  // Properties derivatives
  const MaterialProperty<RealVectorValue> & _dH_kernel_grav_dpf;
  const MaterialProperty<RealVectorValue> & _dH_kernel_grav_dT;
  const MaterialProperty<RankTwoTensor> & _dH_kernel_dev;
  const MaterialProperty<RankTwoTensor> & _dH_kernel_dpf;
  const MaterialProperty<RankTwoTensor> & _dH_kernel_dT;
  const VariableGradient * _grad_temp;
  const MaterialProperty<Real> * _fluid_density;
  const MaterialProperty<Real> * _drho_dpf;
  const MaterialProperty<Real> * _drho_dT;

private:
  unsigned int _T_var;
  // unsigned int _brine_var;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;
};