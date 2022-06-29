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

#include "Material.h"
#include "GolemPropertyReadFile.h"

class GMSMaterial : public Material
{
public:
  static InputParameters validParams();
  GMSMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties();
  Real computeEOSlambda(Real, Real);

  // Coupled vars
  bool _is_temp_coupled;
  bool _is_load_coupled;
  const VariableValue & _temp;
  const VariableValue & _load;
  // Parameters
  bool _has_lambda_pT;
  bool _has_read_prop_uo;
  Real _rho_b;
  Real _lambda_b;
  Real _c_b;
  Real _h_prod;
  Real _scale_factor;
  // user object variable
  const GolemPropertyReadFile * _read_prop_user_object;
  // Properties
  MaterialProperty<Real> & _bulk_density;
  MaterialProperty<Real> & _bulk_thermal_conductivity;
  MaterialProperty<Real> & _bulk_specific_heat;
  MaterialProperty<Real> & _heat_production;
  MaterialProperty<Real> & _scale;
  MaterialProperty<RealVectorValue> & _gravity;
};