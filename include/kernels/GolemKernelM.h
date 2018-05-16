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

#ifndef GOLEMKERNELM_H
#define GOLEMKERNELM_H

#include "Kernel.h"
#include "DerivativeMaterialInterface.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

class GolemKernelM;
class RankTwoTensor;
class RankFourTensor;

template <>
InputParameters validParams<GolemKernelM>();

class GolemKernelM : public DerivativeMaterialInterface<Kernel>
{
public:
  GolemKernelM(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual void computeJacobian();
  virtual Real computeQpJacobian();
  virtual void computeOffDiagJacobian(MooseVariableFEBase & jvar);
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual void computeFiniteDeformJacobian();

  const bool _has_pf;
  const bool _has_T;
  bool _use_finite_deform_jacobian;
  const MaterialProperty<RankTwoTensor> & _stress;
  const MaterialProperty<RealVectorValue> & _M_kernel_grav;
  const MaterialProperty<RankFourTensor> & _M_jacobian;
  Assembly * _assembly_undisplaced;
  const VariablePhiGradient * _grad_phi_undisplaced;
  std::vector<RankFourTensor> _finite_deform_jacobian;
  const MaterialProperty<RankTwoTensor> * _deformation_gradient;
  const MaterialProperty<RankTwoTensor> * _deformation_gradient_old;
  const MaterialProperty<RankTwoTensor> * _rotation_increment;
  const unsigned int _component;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;
  const VariableValue & _pf;
  const unsigned int _pf_var;
  const MaterialProperty<Real> & _biot;
  const unsigned int _T_var;
  const MaterialProperty<RankTwoTensor> & _TM_jacobian;
  const MaterialProperty<RealVectorValue> & _dM_kernel_grav_dev;
  const MaterialProperty<RealVectorValue> & _dM_kernel_grav_dpf;
  const MaterialProperty<RealVectorValue> & _dM_kernel_grav_dT;
};

#endif // GOLEMKERNELM_H