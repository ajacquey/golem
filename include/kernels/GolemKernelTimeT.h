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

#ifndef GOLEMKERNELTIMET_H
#define GOLEMKERNELTIMET_H

#include "TimeDerivative.h"
#include "DerivativeMaterialInterface.h"

class GolemKernelTimeT;

template <>
InputParameters validParams<GolemKernelTimeT>();

class GolemKernelTimeT : public DerivativeMaterialInterface<TimeDerivative>
{
public:
  GolemKernelTimeT(const InputParameters & parameters);

protected:
  virtual void computeResidual();
  virtual Real computeQpResidual();
  virtual void computeJacobian();
  virtual Real computeQpJacobian();
  virtual void computeOffDiagJacobian(MooseVariableFEBase & jvar);
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  bool _has_lumped_mass_matrix;
  bool _has_boussinesq;
  const VariableValue & _u_old;
  bool _has_pf;
  bool _has_disp;
  const MaterialProperty<Real> & _scaling_factor;
  const MaterialProperty<Real> & _T_kernel_time;
  const MaterialProperty<Real> & _dT_kernel_time_dT;
  const MaterialProperty<Real> & _dT_kernel_time_dpf;
  const MaterialProperty<Real> & _dT_kernel_time_dev;
  // nodal value related material properties --> for lumping the mass matrix at nodes
  std::vector<int> _qp_map;
  const MaterialProperty<unsigned int> * _node_number;
  const MaterialProperty<Real> * _nodal_temp;
  const MaterialProperty<Real> * _nodal_temp_old;
  // boussinesq related material properties --> lumped
  const MaterialProperty<Real> * _nodal_pf;
  const MaterialProperty<Real> * _nodal_pf_old;
  // boussinesq related material properties --> not lumped
  const VariableValue * _pf;
  const VariableValue * _pf_old;

private:
  unsigned int _pf_var;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;
};

#endif // GOLEMKERNELTIMET_H