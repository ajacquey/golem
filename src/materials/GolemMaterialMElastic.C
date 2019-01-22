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

#include "GolemMaterialMElastic.h"
#include "Assembly.h"
#include "MooseMesh.h"
#include "libmesh/quadrature.h"
#include "Function.h"
#include "GolemH.h"

registerMooseObject("GolemApp", GolemMaterialMElastic);

template <>
InputParameters
validParams<GolemMaterialMElastic>()
{
  InputParameters params = validParams<GolemMaterialBase>();
  params.addRequiredCoupledVar("displacements", "The displacement variables vector.");
  params.addCoupledVar("pore_pressure", "The pore pressure variable.");
  params.addCoupledVar("temperature", "The temperature variable.");
  params.addParam<MooseEnum>("strain_model",
                             GolemMaterialMElastic::strainModel() = "small_strain",
                             "The strain model to be used.");
  params.addParam<bool>(
      "volumetric_locking_correction", false, "Flag to correct volumetric locking");
  params.addParam<Real>("bulk_modulus", "The bulk modulus [Pa].");
  params.addParam<Real>("lame_modulus", "The first lame constant [Pa].");
  params.addParam<Real>("poisson_ratio", "The Poisson's ratio [-].");
  params.addParam<Real>("shear_modulus", "The shear modulus [Pa].");
  params.addParam<Real>("young_modulus", "The young modulus [Pa].");
  params.addParam<std::vector<FunctionName>>(
      "background_stress",
      "A list of functions describing the background stress. If provided, "
      "there must be 3 of these, corresponding to the xx, yy, zz components.");
  params.addParam<Real>("solid_bulk_modulus", 1e+99, "The solid bulk modulus [Pa].");
  params.addParam<Real>("end_bulk_modulus", "The end bulk modulus for crack closure model [Pa].");
  params.addParam<Real>("closure_pressure", "The closure pressure for crack closure model [Pa].");
  // GolemMaterialT
  params.addParam<bool>(
      "has_heat_source_sink", false, "Has source/sink of temperature considered?");
  params.addParam<Real>("fluid_thermal_conductivity_initial",
                        "The fluid thermal conductivity [W/m/K].");
  params.addParam<Real>("solid_thermal_conductivity_initial",
                        "The solid thermal conductivity [W/m/K].");
  params.addParam<Real>("fluid_heat_capacity_initial", 0.0, "The fluid heat capacity [J/m^3/K].");
  params.addParam<Real>("solid_heat_capacity_initial", 0.0, "The solid heat capacity [J/m^3/K].");
  params.addParam<Real>("heat_source_sink", 0.0, "The heat source or sink [W/m^3].");
  // GolemMaterialH
  params.addParam<MooseEnum>(
      "permeability_type",
      GolemMaterialMElastic::permeabilityType() = "isotropic",
      "The permeability distribution type [isotropic, orthotropic, anisotropic].");
  params.addParam<std::vector<Real>>("permeability_initial", "The initial permeability [m2]");
  params.addParam<Real>("fluid_viscosity_initial", 1e-3, "The fluid viscosity [Pa s].");
  params.addParam<Real>("fluid_modulus", 1.0e+99, "The fluid modulus [Pa].");
  params.addRequiredParam<UserObjectName>("fluid_density_uo",
                                          "The name of the fluid density user object");
  params.addParam<UserObjectName>("fluid_viscosity_uo",
                                  "The name of the fluid viscosity user object");
  params.addParam<UserObjectName>("permeability_uo", "The name of the permeability user object");
  // GolemMaterialTH
  params.addParam<bool>("has_lumped_mass_matrix", false, "Has lumped mass matrix?");
  params.addParam<UserObjectName>("supg_uo", "The name of the SUPG user object.");
  return params;
}

GolemMaterialMElastic::GolemMaterialMElastic(const InputParameters & parameters)
  : GolemMaterialBase(parameters),
    _ndisp(coupledComponents("displacements")),
    _disp(3),
    _grad_disp(3),
    _strain_model(getParam<MooseEnum>("strain_model")),
    _volumetric_locking_correction(getParam<bool>("volumetric_locking_correction")),
    _mechanical_strain(declareProperty<RankTwoTensor>("mechanical_strain")),
    _total_strain(_fe_problem.getMaxQps()),
    _Cijkl(_fe_problem.getMaxQps()),
    _bulk_modulus_set(isParamValid("bulk_modulus")),
    _lame_modulus_set(isParamValid("lame_modulus")),
    _poisson_ratio_set(isParamValid("poisson_ratio")),
    _shear_modulus_set(isParamValid("shear_modulus")),
    _young_modulus_set(isParamValid("young_modulus")),
    _stress(declareProperty<RankTwoTensor>("stress")),
    _M_jacobian(declareProperty<RankFourTensor>("M_jacobian")),
    _M_kernel_grav(declareProperty<RealVectorValue>("M_kernel_grav")),
    _porosity_old(getMaterialPropertyOld<Real>("porosity")),
    _crack_closure_set(isParamValid("end_bulk_modulus") && isParamValid("closure_pressure")),
    _has_pf(isCoupled("pore_pressure")),
    _permeability_type(getParam<MooseEnum>("permeability_type")),
    _has_T(isCoupled("temperature")),
    _has_T_source_sink(getParam<bool>("has_heat_source_sink")),
    // SUPG parameters
    _grad_pf(coupledGradient("pore_pressure")),
    _SUPG_N(declareProperty<RealVectorValue>("SUPG_N")),
    _SUPG_dtau_dgradpf(declareProperty<RankTwoTensor>("SUPG_dtau_dgradpf")),
    _SUPG_dtau_dpf(declareProperty<RealVectorValue>("SUPG_dtau_dpf")),
    _SUPG_dtau_dT(declareProperty<RealVectorValue>("SUPG_dtau_dT")),
    _SUPG_dtau_dev(declareProperty<RealVectorValue>("SUPG_dtau_dev"))
{
  if (_ndisp != _mesh.dimension())
    mooseError(
        "The number of variables supplied for displacements does not match the mesh dimension!\n");
  setPropertiesM();
  _porosity_uo = &getUserObject<GolemPorosity>("porosity_uo");
  _fluid_density_uo = &getUserObject<GolemFluidDensity>("fluid_density_uo");
  if (_has_pf)
  {
    if (_strain_model < 2)
      mooseError("Use an incremental strain model for hydro-mechanical coupling.\n");
    else
      setPropertiesHM();
  }
  if (_has_T)
  {
    if (_strain_model < 2)
      mooseError("Use an incremental strain model for thermo-mechanical coupling.\n");
    else
      setPropertiesTM();
  }
  // these two lines avoid having to take care about user errors in setting parameters
  _has_lumped_mass_matrix = false;
  _has_SUPG_upwind = false;
  if (_has_pf && _has_T)
    setPropertiesTHM();
}

MooseEnum
GolemMaterialMElastic::strainModel()
{
  return MooseEnum("small_strain=1 incr_small_strain=2 finite_strain=3");
}

MooseEnum
GolemMaterialMElastic::permeabilityType()
{
  return MooseEnum("isotropic=1 orthotropic=2 anisotropic=3");
}

/*******************************************************************************
# Set initial properties
*******************************************************************************/
void
GolemMaterialMElastic::setPropertiesM()
{
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    _disp[i] = &coupledValue("displacements", i);
    _grad_disp[i] = &coupledGradient("displacements", i);
  }
  for (unsigned int i = _ndisp; i < 3; ++i)
  {
    _disp[i] = &_zero;
    _grad_disp[i] = &_grad_zero;
  }
  setStrainModel();
  setElasticModuli();
  setBackgroundStress();
}

void
GolemMaterialMElastic::setStrainModel()
{
  if (_strain_model > 1) // incremental strain model
  {
    _grad_disp_old = new std::vector<const VariableGradient *>;
    (*_grad_disp_old).resize(3);
    for (unsigned int i = _ndisp; i < 3; ++i)
      (*_grad_disp_old)[i] = &_grad_zero;
    for (unsigned int i = 0; i < _ndisp; ++i)
      if (_fe_problem.isTransient())
        (*_grad_disp_old)[i] = &coupledGradientOld("displacements", i);
      else
        (*_grad_disp_old)[i] = &_grad_zero;
    _mechanical_strain_old = &getMaterialPropertyOld<RankTwoTensor>("mechanical_strain");
    _strain_increment = new std::vector<RankTwoTensor>;
    (*_strain_increment).resize(_fe_problem.getMaxQps());
    _total_strain_increment = new std::vector<RankTwoTensor>;
    (*_total_strain_increment).resize(_fe_problem.getMaxQps());
    _stress_old = &getMaterialPropertyOld<RankTwoTensor>("stress");
    if (_strain_model > 2) // finite strain model
    {
      _current_elem_volume = &_assembly.elemVolume();
      _Fhat = new std::vector<RankTwoTensor>;
      (*_Fhat).resize(_fe_problem.getMaxQps());
      _deformation_gradient = &declareProperty<RankTwoTensor>("deformation_gradient");
      _deformation_gradient_old = &getMaterialPropertyOld<RankTwoTensor>("deformation_gradient");
      _rotation_increment = &declareProperty<RankTwoTensor>("rotation_increment");
    }
  }
}

void
GolemMaterialMElastic::setElasticModuli()
{
  std::vector<Real> iso_const(2);
  if (_lame_modulus_set && _shear_modulus_set)
  {
    iso_const[0] = getParam<Real>("lame_modulus");
    iso_const[1] = getParam<Real>("shear_modulus");
  }
  else if (_young_modulus_set && _poisson_ratio_set)
  {
    iso_const[0] =
        getParam<Real>("young_modulus") * getParam<Real>("poisson_ratio") /
        ((1.0 + getParam<Real>("poisson_ratio")) * (1.0 - 2.0 * getParam<Real>("poisson_ratio")));
    iso_const[1] =
        getParam<Real>("young_modulus") / (2.0 * (1.0 + getParam<Real>("poisson_ratio")));
  }
  else if (_shear_modulus_set && _bulk_modulus_set)
  {
    iso_const[0] = getParam<Real>("bulk_modulus") - 2.0 / 3.0 * getParam<Real>("shear_modulus");
    iso_const[1] = getParam<Real>("shear_modulus");
  }
  else
    mooseError("Incorrect combination of elastic properties in GolemMaterialMElasticech.");
  if (_has_scaled_properties)
  {
    iso_const[0] *= _scaling_uo->_s_compressibility;
    iso_const[1] *= _scaling_uo->_s_compressibility;
  }
  for (_qp = 0; _qp < _fe_problem.getMaxQps(); ++_qp)
    _Cijkl[_qp].fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);

  if (_crack_closure_set)
  {
    if (_strain_model < 2)
      mooseError("Use an incremental strain model for crack closure model.\n");
    _K_i = new Real;
    (*_K_i) = iso_const[0] + 2. / 3. * iso_const[1];
    _K_end = new Real;
    (*_K_end) = getParam<Real>("end_bulk_modulus");
    _G = new Real;
    (*_G) = iso_const[1];
    _p_hat = new Real;
    (*_p_hat) = getParam<Real>("closure_pressure");
    if (_has_scaled_properties)
    {
      (*_K_end) *= _scaling_uo->_s_compressibility;
      (*_p_hat) /= _scaling_uo->_s_stress;
    }
  }
}

void
GolemMaterialMElastic::setBackgroundStress()
{
  const std::vector<FunctionName> & fcn_names(
      getParam<std::vector<FunctionName>>("background_stress"));
  const unsigned num = fcn_names.size();
  if (!(num == 0 || num == 3))
    mooseError("Either zero or 3 background stress functions must be provided to GolemMaterialM*. "
               "You supplied ",
               num,
               ".\n");
  _background_stress.resize(num);
  for (unsigned i = 0; i < num; ++i)
    _background_stress[i] = &getFunctionByName(fcn_names[i]);
}

void
GolemMaterialMElastic::setPropertiesHM()
{
  if (!isParamValid("permeability_initial") || !isParamValid("fluid_viscosity_initial"))
    mooseError("GolemMaterialMElastic: You need to provide values for permeability and fluid "
               "viscosity for hydro-mechanical coupling!");
  // Parameters
  _k0 = getParam<std::vector<Real>>("permeability_initial");
  _mu0 = getParam<Real>("fluid_viscosity_initial");
  _Kf = getParam<Real>("fluid_modulus");
  _Ks = getParam<Real>("solid_bulk_modulus");
  if (_has_scaled_properties)
  {
    for (unsigned i = 0; i < _k0.size(); ++i)
      _k0[i] /= _scaling_uo->_s_permeability;
    _mu0 /= _scaling_uo->_s_viscosity;
    _Kf *= _scaling_uo->_s_compressibility;
    _Ks *= _scaling_uo->_s_compressibility;
  }
  // UserObject
  if (!isParamValid("fluid_viscosity_uo") || !isParamValid("permeability_uo"))
    mooseError("You must provide a FluidViscosity and Permeability UserObject for hydro-mechanical "
               "coupling!\n");
  else
  {
    _fluid_viscosity_uo = &getUserObject<GolemFluidViscosity>("fluid_viscosity_uo");
    _permeability_uo = &getUserObject<GolemPermeability>("permeability_uo");
  }
  // Properties
  _permeability = &declareProperty<std::vector<Real>>("permeability");
  if (_fe_problem.isTransient())
    _H_kernel_time = &declareProperty<Real>("H_kernel_time");
  _H_kernel_grav = &declareProperty<RealVectorValue>("H_kernel_grav");
  _H_kernel = &declareProperty<RankTwoTensor>("H_kernel");
  _biot = &declareProperty<Real>("biot_coefficient");
  _vol_strain_rate = &declareProperty<Real>("volumetric_strain_rate");
  // Coupled Var
  _pf = &coupledValue("pore_pressure");
  if (_fe_problem.isTransient())
    _pf_old = &coupledValueOld("pore_pressure");
  // Properties derivatives
  _dphi_dev = &declareProperty<Real>("dphi_dev");
  _dphi_dpf = &declareProperty<Real>("dphi_dpf");
  _dk_dev = &declareProperty<std::vector<Real>>("dk_dev");
  _dk_dpf = &declareProperty<std::vector<Real>>("dk_dpf");
  _dH_kernel_dev = &declareProperty<RankTwoTensor>("dH_kernel_dev");
  _dH_kernel_dpf = &declareProperty<RankTwoTensor>("dH_kernel_dpf");
  if (_fe_problem.isTransient())
  {
    _dH_kernel_time_dev = &declareProperty<Real>("dH_kernel_time_dev");
    _dH_kernel_time_dpf = &declareProperty<Real>("dH_kernel_time_dpf");
  }
  _dM_kernel_grav_dev = &declareProperty<RealVectorValue>("dM_kernel_grav_dev");
  _dM_kernel_grav_dpf = &declareProperty<RealVectorValue>("dM_kernel_grav_dpf");
}

void
GolemMaterialMElastic::setPropertiesTM()
{
  if (!isParamValid("fluid_thermal_conductivity_initial") ||
      !isParamValid("solid_thermal_conductivity_initial"))
    mooseError("GolemMaterialMElastic: You need to provide values for thermal conductivities for "
               "thermo-mechanical coupling!");
  // Parameters
  _lambda_f = getParam<Real>("fluid_thermal_conductivity_initial");
  _lambda_s = getParam<Real>("solid_thermal_conductivity_initial");
  _c_f = getParam<Real>("fluid_heat_capacity_initial");
  _c_s = getParam<Real>("solid_heat_capacity_initial");
  _T_source_sink = getParam<Real>("heat_source_sink");
  if (_has_scaled_properties)
  {
    _lambda_f /= _scaling_uo->_s_conductivity;
    _lambda_s /= _scaling_uo->_s_conductivity;
    _c_f /= _scaling_uo->_s_specific_heat;
    _c_s /= _scaling_uo->_s_specific_heat;
    _T_source_sink /= _scaling_uo->_s_heat_production;
  }
  // Properties
  if (_fe_problem.isTransient())
    _T_kernel_time = &declareProperty<Real>("T_kernel_time");
  _T_kernel_diff = &declareProperty<Real>("T_kernel_diff");
  if (_has_T_source_sink)
    _T_kernel_source = &declareProperty<Real>("T_kernel_source");
  // Coupled Var
  _temp = &coupledValue("temperature");
  if (_fe_problem.isTransient())
    _temp_old = &coupledValueOld("temperature");
  // Properties derivatices
  _TM_jacobian = &declareProperty<RankTwoTensor>("TM_jacobian");
}

void
GolemMaterialMElastic::setPropertiesTHM()
{
  // Parameters
  _has_SUPG_upwind = isParamValid("supg_uo") ? true : false;
  _has_lumped_mass_matrix = getParam<bool>("has_lumped_mass_matrix");
  // UserObject
  _supg_uo = _has_SUPG_upwind ? &getUserObject<GolemSUPG>("supg_uo") : NULL;
  // Properties
  _TH_kernel = &declareProperty<RankTwoTensor>("TH_kernel");
  // Properties derivatives
  _dphi_dT = &declareProperty<Real>("dphi_dT");
  _dk_dT = &declareProperty<std::vector<Real>>("dk_dT");
  _drho_dpf = &declareProperty<Real>("drho_dpf");
  _drho_dT = &declareProperty<Real>("drho_dT");
  _dmu_dpf = &declareProperty<Real>("dmu_dpf");
  _dmu_dT = &declareProperty<Real>("dmu_dT");
  _dT_kernel_diff_dev = &declareProperty<Real>("dT_kernel_diff_dev");
  _dT_kernel_diff_dpf = &declareProperty<Real>("dT_kernel_diff_dpf");
  _dT_kernel_diff_dT = &declareProperty<Real>("dT_kernel_diff_dT");
  _dH_kernel_dT = &declareProperty<RankTwoTensor>("dH_kernel_dT");
  _dTH_kernel_dev = &declareProperty<RankTwoTensor>("dTH_kernel_dev");
  _dTH_kernel_dpf = &declareProperty<RankTwoTensor>("dTH_kernel_dpf");
  _dTH_kernel_dT = &declareProperty<RankTwoTensor>("dTH_kernel_dT");
  if (_fe_problem.isTransient())
  {
    _dT_kernel_time_dev = &declareProperty<Real>("dT_kernel_time_dev");
    _dT_kernel_time_dpf = &declareProperty<Real>("dT_kernel_time_dpf");
    _dT_kernel_time_dT = &declareProperty<Real>("dT_kernel_time_dT");
    _dH_kernel_time_dT = &declareProperty<Real>("dH_kernel_time_dT");
  }
  _dH_kernel_grav_dpf = &declareProperty<RealVectorValue>("dH_kernel_grav_dpf");
  _dH_kernel_grav_dT = &declareProperty<RealVectorValue>("dH_kernel_grav_dT");
  _dM_kernel_grav_dT = &declareProperty<RealVectorValue>("dM_kernel_grav_dT");

  if (_has_lumped_mass_matrix)
  {
    if (!_fe_problem.isTransient())
      mooseError("_has_lumped_mass_matrix is set to ",
                 _has_lumped_mass_matrix,
                 " but simulation is Steady State!");
    _nodal_pf_var = &coupledNodalValue("pore_pressure");
    _nodal_temp_var = &coupledNodalValue("temperature");
    _node_number = &declareProperty<unsigned int>("node_number");
    _nodal_pf = &declareProperty<Real>("nodal_pf");
    _nodal_temp = &declareProperty<Real>("nodal_temp");
    _nodal_temp_var_old = &coupledNodalValueOld("temperature");
    _nodal_temp_old = &declareProperty<Real>("nodal_temp_old");
    _nodal_pf_var_old = &coupledNodalValueOld("pore_pressure");
    _nodal_pf_old = &declareProperty<Real>("nodal_pf_old");
  }
}

void
GolemMaterialMElastic::initQpStatefulProperties()
{
  _mechanical_strain[_qp].zero();
  _stress[_qp].zero();
  if (_background_stress.size() == 3)
    for (unsigned i = 0; i < 3; ++i)
      _stress[_qp](i, i) = _background_stress[i]->value(_t, _q_point[_qp]);
  if (_strain_model > 2)
  {
    (*_deformation_gradient)[_qp].zero();
    (*_deformation_gradient)[_qp].addIa(1.0);
    (*_rotation_increment)[_qp].zero();
  }
  _porosity[_qp] = _phi0;
}

void
GolemMaterialMElastic::computeProperties()
{
  computeStrain();
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    computeQpProperties();
}

void
GolemMaterialMElastic::computeStrain()
{
  switch (_strain_model)
  {
    case 1:
      for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      {
        RankTwoTensor A((*_grad_disp[0])[_qp], (*_grad_disp[1])[_qp], (*_grad_disp[2])[_qp]);
        _total_strain[_qp] = 0.5 * (A + A.transpose());
        _mechanical_strain[_qp] = _total_strain[_qp];
      }
      break;
    case 2:
      for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      {
        RankTwoTensor A((*_grad_disp[0])[_qp], (*_grad_disp[1])[_qp], (*_grad_disp[2])[_qp]);
        RankTwoTensor Fbar((*(*_grad_disp_old)[0])[_qp],
                           (*(*_grad_disp_old)[1])[_qp],
                           (*(*_grad_disp_old)[2])[_qp]);
        A -= Fbar;
        (*_total_strain_increment)[_qp] = 0.5 * (A + A.transpose());
        //  (*_strain_increment)[_qp] = (*_total_strain_increment)[_qp];
        //  substractThermalEigenStrain((*_strain_increment)[_qp]);
        //  _mechanical_strain[_qp] = (*_mechanical_strain_old)[_qp] + (*_strain_increment)[_qp];
      }
      break;
    case 3: // Rashid (1993)
      RankTwoTensor ave_Fhat;
      Real ave_dfgrd_det = 0.0;
      for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      {
        RankTwoTensor A((*_grad_disp[0])[_qp], (*_grad_disp[1])[_qp], (*_grad_disp[2])[_qp]);
        RankTwoTensor Fbar((*(*_grad_disp_old)[0])[_qp],
                           (*(*_grad_disp_old)[1])[_qp],
                           (*(*_grad_disp_old)[2])[_qp]);
        (*_deformation_gradient)[_qp] = A;
        (*_deformation_gradient)[_qp].addIa(1.0);
        A -= Fbar;
        Fbar.addIa(1.0);
        (*_Fhat)[_qp] = A * Fbar.inverse();
        (*_Fhat)[_qp].addIa(1.0);
        if (_volumetric_locking_correction)
        {
          // Calculate average _Fhat for volumetric locking correction
          ave_Fhat += (*_Fhat)[_qp] * _JxW[_qp] * _coord[_qp];

          // Average deformation gradient
          ave_dfgrd_det += (*_deformation_gradient)[_qp].det() * _JxW[_qp] * _coord[_qp];
        }
      }
      if (_volumetric_locking_correction)
      {
        // needed for volumetric locking correction
        ave_Fhat /= (*_current_elem_volume);
        // average deformation gradient
        ave_dfgrd_det /= (*_current_elem_volume);
      }
      for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
      {
        if (_volumetric_locking_correction)
        {
          // Finalize volumetric locking correction
          (*_Fhat)[_qp] *= std::cbrt(ave_Fhat.det() / (*_Fhat)[_qp].det());
          (*_deformation_gradient)[_qp] *=
              std::cbrt(ave_dfgrd_det / (*_deformation_gradient)[_qp].det());
        }
        computeQpFiniteStrain();
      }
      break;
  }
}

void
GolemMaterialMElastic::computeQpFiniteStrain()
{
  RankTwoTensor invFhat((*_Fhat)[_qp].inverse());
  RankTwoTensor A(RankTwoTensor::initIdentity);
  A -= invFhat;
  RankTwoTensor Cinv_I = A * A.transpose() - A - A.transpose();
  (*_total_strain_increment)[_qp] = -Cinv_I * 0.5 + Cinv_I * Cinv_I * 0.25;
  const Real a[3] = {
      invFhat(1, 2) - invFhat(2, 1), invFhat(2, 0) - invFhat(0, 2), invFhat(0, 1) - invFhat(1, 0)};
  Real q = (a[0] * a[0] + a[1] * a[1] + a[2] * a[2]) / 4.0;
  Real trFhatinv_1 = invFhat.trace() - 1.0;
  const Real p = trFhatinv_1 * trFhatinv_1 / 4.0;
  const Real C1 = std::sqrt(p + 3.0 * p * p * (1.0 - (p + q)) / ((p + q) * (p + q)) -
                            2.0 * p * p * p * (1 - (p + q)) / ((p + q) * (p + q) * (p + q)));
  Real C2;
  if (q > 0.01)
    C2 = (1.0 - C1) / (4.0 * q);
  else
    C2 = 0.125 + q * 0.03125 * (p * p - 12 * (p - 1)) / (p * p) +
         q * q * (p - 2.0) * (p * p - 10.0 * p + 32.0) / (p * p * p) +
         q * q * q * (1104.0 - 992.0 * p + 376.0 * p * p - 72 * p * p * p + 5.0 * p * p * p * p) /
             (512.0 * p * p * p * p);
  const Real C3 =
      0.5 * std::sqrt((p * q * (3.0 - q) + p * p * p + q * q) / ((p + q) * (p + q) * (p + q)));
  RankTwoTensor R_incr;
  R_incr.addIa(C1);
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j < 3; ++j)
      R_incr(i, j) += C2 * a[i] * a[j];
  R_incr(0, 1) += C3 * a[2];
  R_incr(0, 2) -= C3 * a[1];
  R_incr(1, 0) -= C3 * a[2];
  R_incr(1, 2) += C3 * a[0];
  R_incr(2, 0) += C3 * a[1];
  R_incr(2, 1) -= C3 * a[0];
  (*_rotation_increment)[_qp] = R_incr.transpose();
}

void
GolemMaterialMElastic::computeQpProperties()
{
  // Update elasticity tensor
  GolemCrackClosure();
  // Check for coupling
  if ((_has_pf) && (!_has_T))
  {
    // HM coupling
    if (_current_elem->dim() < _mesh.dimension())
      computeRotationMatrix();
    GolemMatPropertiesHM();
    GolemKernelPropertiesHM();
    GolemKernelPropertiesDerivativesHM();
  }
  else if ((_has_T) && (!_has_pf))
  {
    // TM coupling
    GolemMatPropertiesTM();
    GolemKernelPropertiesTM();
    GolemKernelPropertiesDerivativesTM();
  }
  else if ((_has_T) && (_has_pf))
  {
    // THM coupling
    if (_current_elem->dim() < _mesh.dimension())
      computeRotationMatrix();
    GolemMatPropertiesTHM();
    GolemKernelPropertiesTHM();
    GolemKernelPropertiesDerivativesTHM();
  }
  else
    GolemMatPropertiesM();

  // Mechanical properties
  GolemKernelPropertiesM();
}

void
GolemMaterialMElastic::GolemCrackClosure()
{
  if (_crack_closure_set)
  {
    std::vector<Real> iso_const(2);
    // Bulk modulus
    Real K = 1. / (1. / (*_K_end) + (1. / (*_K_i) - 1. / (*_K_end)) *
                                        std::exp((*_stress_old)[_qp].trace() / (3.0 * (*_p_hat))));
    iso_const[0] = K - 2.0 / 3.0 * (*_G);
    iso_const[1] = (*_G);
    // Fill elasticity tensor
    _Cijkl[_qp].fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);
  }
}

void
GolemMaterialMElastic::GolemMatPropertiesHM()
{
  _scaling_factor[_qp] = computeQpScaling();
  // Fluid density
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(0.0, 0.0, _rho0_f);
  // Fluid viscosity
  _fluid_viscosity[_qp] = _fluid_viscosity_uo->computeViscosity(0.0, _fluid_density[_qp], _mu0);
  // Biot coefficient
  (*_biot)[_qp] = 1.0 - (_Cijkl[_qp](0, 0, 1, 1) + 2.0 / 3.0 * _Cijkl[_qp](0, 1, 0, 1)) / _Ks;
  // Porosity
  Real dev = (_fe_problem.isTransient()) * (*_total_strain_increment)[_qp].trace();
  Real dpf = 0.0;
  if (_fe_problem.isTransient())
    dpf = (*_pf)[_qp] - (*_pf_old)[_qp];
  (*_dphi_dev)[_qp] = _porosity_uo->computedPorositydev(_porosity_old[_qp], (*_biot)[_qp]);
  (*_dphi_dpf)[_qp] = _porosity_uo->computedPorositydpf(_porosity_old[_qp], (*_biot)[_qp], _Ks);
  _porosity[_qp] = _porosity_uo->computePorosity(
      _porosity_old[_qp], (*_dphi_dev)[_qp], (*_dphi_dpf)[_qp], 0.0, dev, dpf, 0.0);
  // Permeability
  (*_permeability)[_qp] =
      _permeability_uo->computePermeability(_k0, _phi0, _porosity[_qp], _scaling_factor[_qp]);
  (*_dk_dev)[_qp] =
      _permeability_uo->computedPermeabilitydev(_k0, _phi0, _porosity[_qp], (*_dphi_dev)[_qp]);
  (*_dk_dpf)[_qp] =
      _permeability_uo->computedPermeabilitydpf(_k0, _phi0, _porosity[_qp], (*_dphi_dpf)[_qp]);
}

void
GolemMaterialMElastic::GolemKernelPropertiesHM()
{
  Real one_on_visc = 1.0 / _fluid_viscosity[_qp];
  if (_fe_problem.isTransient())
    (*_H_kernel_time)[_qp] = _porosity[_qp] / _Kf + ((*_biot)[_qp] - _porosity[_qp]) / _Ks;
  (*_H_kernel)[_qp] =
      computeKernel((*_permeability)[_qp], _permeability_type, one_on_visc, _current_elem->dim());
  if (_current_elem->dim() < _mesh.dimension())
    (*_H_kernel)[_qp].rotate(_rotation_matrix);
  (*_H_kernel_grav)[_qp] = -_fluid_density[_qp] * _gravity;
  (*_vol_strain_rate)[_qp] = (*_total_strain_increment)[_qp].trace() / _dt;
}

void
GolemMaterialMElastic::GolemKernelPropertiesDerivativesHM()
{
  // H_kernel derivatives
  Real one_on_visc = 1.0 / _fluid_viscosity[_qp];
  (*_dH_kernel_dev)[_qp] =
      computeKernel((*_dk_dev)[_qp], _permeability_type, one_on_visc, _current_elem->dim());
  (*_dH_kernel_dpf)[_qp] =
      computeKernel((*_dk_dpf)[_qp], _permeability_type, one_on_visc, _current_elem->dim());
  // H_kernel_time derivatives
  if (_fe_problem.isTransient())
  {
    (*_dH_kernel_time_dpf)[_qp] = (*_dphi_dpf)[_qp] * (1.0 / _Kf - 1.0 / _Ks);
    (*_dH_kernel_time_dev)[_qp] = (*_dphi_dev)[_qp] * (1.0 / _Kf - 1.0 / _Ks);
  }
  // M_kernel_grav derivatives
  (*_dM_kernel_grav_dev)[_qp] = -(*_dphi_dev)[_qp] * (_fluid_density[_qp] - _rho0_s) * _gravity;
  (*_dM_kernel_grav_dpf)[_qp] = -(*_dphi_dpf)[_qp] * (_fluid_density[_qp] - _rho0_s) * _gravity;
}

void
GolemMaterialMElastic::GolemMatPropertiesTM()
{
  _scaling_factor[_qp] = computeQpScaling();
  // Fluid density
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(0.0, 0.0, _rho0_f);
  // Fluid viscosity
  _fluid_viscosity[_qp] = _mu0;
  // Porosity
  _porosity[_qp] = _porosity_uo->computePorosity(_porosity_old[_qp], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

void
GolemMaterialMElastic::GolemKernelPropertiesTM()
{
  (*_T_kernel_diff)[_qp] = _porosity[_qp] * _lambda_f + (1.0 - _porosity[_qp]) * _lambda_s;
  if (_fe_problem.isTransient())
    (*_T_kernel_time)[_qp] =
        _porosity[_qp] * _fluid_density[_qp] * _c_f + (1.0 - _porosity[_qp]) * _rho0_s * _c_s;
  if (_has_T_source_sink)
    (*_T_kernel_source)[_qp] = -1.0 * _T_source_sink;
}

void
GolemMaterialMElastic::GolemKernelPropertiesDerivativesTM()
{
  // TM stress derivative
  Real bulk_thermal_expansion_coeff =
      _porosity_old[_qp] * _alpha_T_f + (1.0 - _porosity_old[_qp]) * _alpha_T_s;
  (*_TM_jacobian)[_qp] = -bulk_thermal_expansion_coeff * _Cijkl[_qp] *
                         RankTwoTensor(RankTwoTensor::initIdentity) / 3.0;
}

void
GolemMaterialMElastic::GolemMatPropertiesTHM()
{
  // Fluid density
  Real pres = (*_pf)[_qp];
  Real temp = (*_temp)[_qp];
  Real dpf = 0.0;
  Real dT = 0.0;
  if (_fe_problem.isTransient())
  {
    dpf = (*_pf)[_qp] - (*_pf_old)[_qp];
    dT = (*_temp)[_qp] - (*_temp_old)[_qp];
  }
  if (_has_lumped_mass_matrix)
  {
    (*_node_number)[_qp] = nearest();
    (*_nodal_temp)[_qp] = (*_nodal_temp_var)[(*_node_number)[_qp]];
    (*_nodal_temp_old)[_qp] = (*_nodal_temp_var_old)[(*_node_number)[_qp]];
    (*_nodal_pf)[_qp] = (*_nodal_pf_var)[(*_node_number)[_qp]];
    (*_nodal_pf_old)[_qp] = (*_nodal_pf_var_old)[(*_node_number)[_qp]];

    pres = (*_nodal_pf)[_qp];
    temp = (*_nodal_temp)[_qp];
    dpf = (*_nodal_pf)[_qp] - (*_nodal_pf_old)[_qp];
    dT = (*_nodal_temp)[_qp] - (*_nodal_temp_old)[_qp];
  }
  _scaling_factor[_qp] = computeQpScaling();
  (*_drho_dpf)[_qp] = _fluid_density_uo->computedDensitydp(pres, temp);
  (*_drho_dT)[_qp] = _fluid_density_uo->computedDensitydT(pres, temp, _rho0_f);
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(pres, temp, _rho0_f);
  // Fluid viscosity
  (*_dmu_dpf)[_qp] =
      _fluid_viscosity_uo->computedViscositydp(temp, _fluid_density[_qp], (*_drho_dpf)[_qp]);
  (*_dmu_dT)[_qp] =
      _fluid_viscosity_uo->computedViscositydT(temp, _fluid_density[_qp], (*_drho_dT)[_qp], _mu0);
  _fluid_viscosity[_qp] = _fluid_viscosity_uo->computeViscosity(temp, _fluid_density[_qp], _mu0);
  // Biot coefficient
  (*_biot)[_qp] = 1.0 - (_Cijkl[_qp](0, 0, 1, 1) + 2.0 / 3.0 * _Cijkl[_qp](0, 1, 0, 1)) / _Ks;
  // Porosity
  Real dev = (_fe_problem.isTransient()) * (*_total_strain_increment)[_qp].trace();
  (*_dphi_dev)[_qp] = _porosity_uo->computedPorositydev(_porosity_old[_qp], (*_biot)[_qp]);
  (*_dphi_dpf)[_qp] = _porosity_uo->computedPorositydpf(_porosity_old[_qp], (*_biot)[_qp], _Ks);
  (*_dphi_dT)[_qp] =
      _porosity_uo->computedPorositydT(_porosity_old[_qp], (*_biot)[_qp], _alpha_T_f, _alpha_T_s);
  _porosity[_qp] = _porosity_uo->computePorosity(
      _porosity_old[_qp], (*_dphi_dev)[_qp], (*_dphi_dpf)[_qp], (*_dphi_dT)[_qp], dev, dpf, dT);
  // Permeability
  (*_permeability)[_qp] =
      _permeability_uo->computePermeability(_k0, _phi0, _porosity[_qp], _scaling_factor[_qp]);
  (*_dk_dev)[_qp] =
      _permeability_uo->computedPermeabilitydev(_k0, _phi0, _porosity[_qp], (*_dphi_dev)[_qp]);
  (*_dk_dpf)[_qp] =
      _permeability_uo->computedPermeabilitydpf(_k0, _phi0, _porosity[_qp], (*_dphi_dpf)[_qp]);
  (*_dk_dT)[_qp] =
      _permeability_uo->computedPermeabilitydT(_k0, _phi0, _porosity[_qp], (*_dphi_dT)[_qp]);
}

void
GolemMaterialMElastic::GolemKernelPropertiesTHM()
{
  GolemKernelPropertiesHM();
  GolemKernelPropertiesTM();
  (*_TH_kernel)[_qp] = -(*_H_kernel)[_qp] * _fluid_density[_qp] * _c_f;
}

void
GolemMaterialMElastic::GolemKernelPropertiesDerivativesTHM()
{
  // T_kernel_diff derivatives
  (*_dT_kernel_diff_dev)[_qp] = (*_dphi_dev)[_qp] * (_lambda_f - _lambda_s);
  (*_dT_kernel_diff_dpf)[_qp] = (*_dphi_dpf)[_qp] * (_lambda_f - _lambda_s);
  (*_dT_kernel_diff_dT)[_qp] = (*_dphi_dT)[_qp] * (_lambda_f - _lambda_s);
  // T_kernel_time  and H_kernel_time derivatives
  if (_fe_problem.isTransient())
  {
    // T_kernel_time
    (*_dT_kernel_time_dev)[_qp] = (*_dphi_dev)[_qp] * (_fluid_density[_qp] * _c_f - _rho0_s * _c_s);
    (*_dT_kernel_time_dpf)[_qp] =
        (*_dphi_dpf)[_qp] * (_fluid_density[_qp] * _c_f - _rho0_s * _c_s) +
        (*_drho_dpf)[_qp] * _porosity[_qp] * _c_f;
    (*_dT_kernel_time_dT)[_qp] = (*_dphi_dT)[_qp] * (_fluid_density[_qp] * _c_f - _rho0_s * _c_s) +
                                 (*_drho_dT)[_qp] * _porosity[_qp] * _c_f;
    // H_kernel_time
    (*_dH_kernel_time_dev)[_qp] = (*_dphi_dev)[_qp] * (1.0 / _Kf - 1.0 / _Ks);
    (*_dH_kernel_time_dpf)[_qp] = (*_dphi_dpf)[_qp] * (1.0 / _Kf - 1.0 / _Ks);
    (*_dH_kernel_time_dT)[_qp] = (*_dphi_dT)[_qp] * (1.0 / _Kf - 1.0 / _Ks);
  }
  // H_kernel derivatives
  Real one_on_visc = 1.0 / _fluid_viscosity[_qp];
  (*_dH_kernel_dev)[_qp] =
      computeKernel((*_dk_dev)[_qp], _permeability_type, one_on_visc, _current_elem->dim());
  (*_dH_kernel_dpf)[_qp] =
      computeKernel((*_dk_dpf)[_qp], _permeability_type, one_on_visc, _current_elem->dim()) -
      (*_H_kernel)[_qp] * (*_dmu_dpf)[_qp] / _fluid_viscosity[_qp];
  (*_dH_kernel_dT)[_qp] =
      computeKernel((*_dk_dT)[_qp], _permeability_type, one_on_visc, _current_elem->dim()) -
      (*_H_kernel)[_qp] * (*_dmu_dT)[_qp] / _fluid_viscosity[_qp];
  // H_kernel_grav derivatives
  (*_dH_kernel_grav_dpf)[_qp] = -(*_drho_dpf)[_qp] * _gravity;
  (*_dH_kernel_grav_dT)[_qp] = -(*_drho_dT)[_qp] * _gravity;
  // TH_kernel derivatives
  (*_dTH_kernel_dev)[_qp] = -_fluid_density[_qp] * _c_f * (*_dH_kernel_dpf)[_qp];
  (*_dTH_kernel_dpf)[_qp] = -(_fluid_density[_qp] * _c_f * (*_dH_kernel_dpf)[_qp] +
                              (*_H_kernel)[_qp] * _c_f * (*_drho_dpf)[_qp]);
  (*_dTH_kernel_dT)[_qp] = -(_fluid_density[_qp] * _c_f * (*_dH_kernel_dT)[_qp] +
                             (*_H_kernel)[_qp] * _c_f * (*_drho_dT)[_qp]);
  // TM stress derivative
  Real bulk_thermal_expansion_coeff =
      _porosity_old[_qp] * _alpha_T_f + (1.0 - _porosity_old[_qp]) * _alpha_T_s;
  (*_TM_jacobian)[_qp] = -bulk_thermal_expansion_coeff * _Cijkl[_qp] *
                         RankTwoTensor(RankTwoTensor::initIdentity) / 3.0;
  // M_kernel_grav derivatives
  (*_dM_kernel_grav_dev)[_qp] = -(*_dphi_dev)[_qp] * (_fluid_density[_qp] - _rho0_s) * _gravity;
  (*_dM_kernel_grav_dpf)[_qp] -= _porosity[_qp] * (*_drho_dpf)[_qp] * _gravity;
  (*_dM_kernel_grav_dT)[_qp] -= _porosity[_qp] * (*_drho_dT)[_qp] * _gravity;
  // SUPG
  if (_has_SUPG_upwind)
  {
    RealVectorValue vel = -(*_H_kernel)[_qp] * (_grad_pf[_qp] + (*_H_kernel_grav)[_qp]);
    RankTwoTensor dvel_dgradp = -(*_H_kernel)[_qp];
    RealVectorValue dvel_dp = -(*_dH_kernel_dpf)[_qp] * (_grad_pf[_qp] + (*_H_kernel_grav)[_qp]) -
                              (*_H_kernel)[_qp] * (*_dH_kernel_grav_dpf)[_qp];
    RealVectorValue dvel_dT = -(*_dH_kernel_dT)[_qp] * (_grad_pf[_qp] + (*_H_kernel_grav)[_qp]) -
                              (*_H_kernel)[_qp] * (*_dH_kernel_grav_dT)[_qp];
    RealVectorValue dvel_dev = -(*_dH_kernel_dev)[_qp] * (_grad_pf[_qp] + (*_H_kernel_grav)[_qp]);
    Real diff = (*_T_kernel_diff)[_qp] / (*_T_kernel_time)[_qp];
    Real tau = _supg_uo->tau(vel, diff, _dt, _current_elem);
    _SUPG_N[_qp] = tau * vel;
    _SUPG_dtau_dgradpf[_qp] = tau * dvel_dgradp;
    _SUPG_dtau_dpf[_qp] = tau * dvel_dp;
    _SUPG_dtau_dT[_qp] = tau * dvel_dT;
    _SUPG_dtau_dev[_qp] = tau * dvel_dev;
  }
}

unsigned
GolemMaterialMElastic::nearest()
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

void
GolemMaterialMElastic::GolemMatPropertiesM()
{
  // FLuid density
  _fluid_density[_qp] = _fluid_density_uo->computeDensity(0.0, 0.0, _rho0_f);
  // Porosity
  _porosity[_qp] = _porosity_uo->computePorosity(_porosity_old[_qp], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

void
GolemMaterialMElastic::GolemKernelPropertiesM()
{
  GolemSubstractEigenStrain();
  GolemStress();
  _M_kernel_grav[_qp] =
      -(_porosity[_qp] * _fluid_density[_qp] + (1.0 - _porosity[_qp]) * _rho0_s) * _gravity;
}

void
GolemMaterialMElastic::GolemSubstractEigenStrain()
{
  if (_strain_model > 1)
  {
    (*_strain_increment)[_qp] = (*_total_strain_increment)[_qp];
    substractThermalEigenStrain((*_strain_increment)[_qp]);
    _mechanical_strain[_qp] = (*_mechanical_strain_old)[_qp] + (*_strain_increment)[_qp];
    if (_strain_model > 2)
      _mechanical_strain[_qp] = (*_rotation_increment)[_qp] * _mechanical_strain[_qp] *
                                (*_rotation_increment)[_qp].transpose();
  }
}

void
GolemMaterialMElastic::substractThermalEigenStrain(RankTwoTensor & strain_incr)
{
  RankTwoTensor thermal_es_incr;
  if (_has_T)
  {
    Real bulk_thermal_expansion_coeff =
        _porosity_old[_qp] * _alpha_T_f + (1.0 - _porosity_old[_qp]) * _alpha_T_s;
    thermal_es_incr.addIa(bulk_thermal_expansion_coeff / 3. * ((*_temp)[_qp] - (*_temp_old)[_qp]));
  }
  strain_incr -= thermal_es_incr;
}

void
GolemMaterialMElastic::GolemStress()
{
  switch (_strain_model)
  {
    case 1:
      _stress[_qp] = _Cijkl[_qp] * _mechanical_strain[_qp];
      break;
    case 2:
      _stress[_qp] = (*_stress_old)[_qp] + _Cijkl[_qp] * (*_strain_increment)[_qp];
      break;
    case 3:
      RankTwoTensor intermediate_stress =
          (*_stress_old)[_qp] +
          _Cijkl[_qp] * (*_strain_increment)[_qp]; // Calculate stress in intermediate configruation
      _stress[_qp] = (*_rotation_increment)[_qp] * intermediate_stress *
                     (*_rotation_increment)[_qp].transpose();
      break;
  }
  _M_jacobian[_qp] = _Cijkl[_qp];
}
