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

#include "GolemMaterialH.h"
#include "GolemH.h"
#include "MooseMesh.h"
#include "libmesh/quadrature.h"

template <>
InputParameters
validParams<GolemMaterialH>()
{
  InputParameters params = validParams<GolemMaterialBase>();
  params.addCoupledVar("displacements", "The displacement vector");
  params.addParam<MooseEnum>(
      "permeability_type",
      GolemMaterialH::permeabilityType() = "isotropic",
      "The permeability distribution type [isotropic, orthotropic, anisotropic].");
  params.addParam<std::vector<Real>>("permeability_initial", "The initial permeability [m2]");
  params.addParam<Real>("fluid_viscosity_initial", 1e-3, "The fluid viscosity [Pa s].");
  params.addParam<Real>("fluid_modulus", 1.0e+99, "The fluid modulus [Pa].");
  params.addRequiredParam<UserObjectName>("fluid_density_uo",
                                          "The name of the fluid density user object.");
  params.addRequiredParam<UserObjectName>("fluid_viscosity_uo",
                                          "The name of the fluid viscosity user object.");
  params.addRequiredParam<UserObjectName>("permeability_uo",
                                          "The name of the permeability user object.");
  params.addClassDescription("This is the base class for hydraulic Golem material properties. NB: "
                             "The rotation of the permeability tensor is done here.");
  return params;
}

GolemMaterialH::GolemMaterialH(const InputParameters & parameters)
  : GolemMaterialBase(parameters),
    _has_disp(isCoupled("displacements")),
    _permeability_type(getParam<MooseEnum>("permeability_type")),
    _k0(getParam<std::vector<Real>>("permeability_initial")),
    _mu0(getParam<Real>("fluid_viscosity_initial")),
    _Kf(getParam<Real>("fluid_modulus")),
    _permeability(declareProperty<std::vector<Real>>("permeability")),
    _H_kernel_grav(declareProperty<RealVectorValue>("H_kernel_grav")),
    _H_kernel(declareProperty<RankTwoTensor>("H_kernel"))
{
  // Scaled properties if required
  if (_has_scaled_properties)
  {
    _mu0 /= _scaling_uo->_s_viscosity;
    _Kf *= _scaling_uo->_s_compressibility;
    for (unsigned int i = 0; i < _k0.size(); ++i)
      _k0[i] /= _scaling_uo->_s_permeability;
  }
  if (_fe_problem.isTransient())
    _H_kernel_time = &declareProperty<Real>("H_kernel_time");
  _fluid_density_uo = &getUserObject<GolemFluidDensity>("fluid_density_uo");
  _fluid_viscosity_uo = &getUserObject<GolemFluidViscosity>("fluid_viscosity_uo");
  _permeability_uo = &getUserObject<GolemPermeability>("permeability_uo");

  if (_has_disp)
  {
    // Declare some property when this material is used for fractures or faults in a HM simulation
    _dH_kernel_dev = &declareProperty<RankTwoTensor>("dH_kernel_dev");
    _dH_kernel_dpf = &declareProperty<RankTwoTensor>("dH_kernel_dpf");
    if (_fe_problem.isTransient())
    {
      _dH_kernel_time_dev = &declareProperty<Real>("dH_kernel_time_dev");
      _dH_kernel_time_dpf = &declareProperty<Real>("dH_kernel_time_dpf");
    }
  }
}

MooseEnum
GolemMaterialH::permeabilityType()
{
  return MooseEnum("isotropic=1 orthotropic=2 anisotropic=3");
}

void
GolemMaterialH::computeProperties()
{
  if (_current_elem->dim() < _mesh.dimension())
    computeRotationMatrix();
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    computeQpProperties();
}

void
GolemMaterialH::computeQpProperties()
{
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(0.0, 0.0, _rho0_f);
  _fluid_viscosity[_qp] = _fluid_viscosity_uo->computeViscosity(0.0, 0.0, _mu0);
  _porosity[_qp] = _porosity_uo->computePorosity(_phi0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  _permeability[_qp] = _permeability_uo->computePermeability(_k0, _phi0, _porosity[_qp]);
  GolemPropertiesH();
  if (_has_disp)
  {
    // Declare some property when this material is used for fractures or faults in a HM simulation
    (*_dH_kernel_dev)[_qp] = RankTwoTensor();
    (*_dH_kernel_dpf)[_qp] = RankTwoTensor();
    if (_fe_problem.isTransient())
    {
      (*_dH_kernel_time_dev)[_qp] = 0.0;
      (*_dH_kernel_time_dpf)[_qp] = 0.0;
    }
  }
}

void
GolemMaterialH::GolemPropertiesH()
{
  Real one_on_visc = 1.0 / _fluid_viscosity[_qp];
  _scaling_factor[_qp] = computeQpScaling();
  if (_fe_problem.isTransient())
    (*_H_kernel_time)[_qp] = _porosity[_qp] / _Kf;
  _H_kernel[_qp] =
      computeKernel(_permeability[_qp], _permeability_type, one_on_visc, _current_elem->dim());
  if (_current_elem->dim() < _mesh.dimension())
    _H_kernel[_qp].rotate(_rotation_matrix);
  _H_kernel_grav[_qp] = -_fluid_density[_qp] * _gravity;
}