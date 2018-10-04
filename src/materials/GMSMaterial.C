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

#include "GMSMaterial.h"
#include "libmesh/quadrature.h"
#include "MooseMesh.h"

#define KELVIN 272.15
#define GIGAPA 1e9

registerMooseObject("GolemApp", GMSMaterial);

template <>
InputParameters
validParams<GMSMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("material class to run steady pressure load and steady/transient heat "
                             "with parameters from file.");
  params.addCoupledVar("temperature", "temperature");
  params.addCoupledVar("load", "load");
  params.addParam<bool>("has_lambda_pT", false, "Has T-dependent thermal conductivity?");
  params.addParam<Real>("density", "The bulk density.");
  params.addParam<Real>("thermal_conductivity", "The bulk thermal conductivity.");
  params.addParam<Real>("heat_capacity", "The bulk heat capacity.");
  params.addParam<Real>("heat_production", "The heat production.");
  params.addParam<Real>("scale_factor", "The scale factor.");
  params.addParam<UserObjectName>(
      "read_prop_user_object",
      "The ReadPropertyFile GeneralUserObject to read specific property values from file.");
  return params;
}

GMSMaterial::GMSMaterial(const InputParameters & parameters)
  : Material(parameters),
    // Coupled variables
    _is_temp_coupled(isCoupled("temperature")),
    _is_load_coupled(isCoupled("load")),
    _temp(_is_temp_coupled ? coupledValue("temperature") : _zero),
    _load(_is_load_coupled ? coupledValue("load") : _zero),
    // Parameters
    _has_lambda_pT(getParam<bool>("has_lambda_pT")),
    _has_read_prop_uo(isParamValid("read_prop_user_object")),
    _scale_factor(_is_temp_coupled && _fe_problem.isTransient() ? getParam<Real>("scale_factor")
                                                                : 1.),
    _read_prop_user_object(
        _has_read_prop_uo ? &getUserObject<GolemPropertyReadFile>("read_prop_user_object") : NULL),
    // Properties
    _bulk_density(declareProperty<Real>("bulk_density")),
    _bulk_thermal_conductivity(declareProperty<Real>("bulk_thermal_conductivity")),
    _bulk_specific_heat(declareProperty<Real>("bulk_specific_heat")),
    _heat_production(declareProperty<Real>("heat_production")),
    _scale(declareProperty<Real>("scale_factor")),
    _gravity(declareProperty<RealVectorValue>("gravity"))
{
  if (!_has_read_prop_uo)
  {
    _rho_b = (_is_load_coupled || _fe_problem.isTransient() ? getParam<Real>("density") : 0.);
    _lambda_b = (_is_temp_coupled ? getParam<Real>("thermal_conductivity") : 0.);
    _c_b = (_is_temp_coupled && _fe_problem.isTransient() ? getParam<Real>("heat_capacity") : 0.);
    _h_prod = (_is_temp_coupled ? getParam<Real>("heat_production") : 0.);
  }
}

void
GMSMaterial::computeQpProperties()
{
  if (_has_read_prop_uo)
  {
    _bulk_density[_qp] = _read_prop_user_object->getData(_current_elem, 0);
    if (!_has_lambda_pT)
      _bulk_thermal_conductivity[_qp] = _read_prop_user_object->getData(_current_elem, 1);
    else
    {
      if (_is_load_coupled)
        _bulk_thermal_conductivity[_qp] = computeEOSlambda(_temp[_qp], _load[_qp]);
      else
        _bulk_thermal_conductivity[_qp] = computeEOSlambda(_temp[_qp], 0.0);
    }
    _bulk_specific_heat[_qp] = _read_prop_user_object->getData(_current_elem, 2);
    _heat_production[_qp] = _read_prop_user_object->getData(_current_elem, 3);
  }
  else
  {
    _bulk_density[_qp] = _rho_b;
    if (!_has_lambda_pT)
      _bulk_thermal_conductivity[_qp] = _lambda_b;
    else
    {
      if (_is_load_coupled)
        _bulk_thermal_conductivity[_qp] = computeEOSlambda(_temp[_qp], _load[_qp]);
      else
        _bulk_thermal_conductivity[_qp] = computeEOSlambda(_temp[_qp], 0.0);
    }
    _bulk_specific_heat[_qp] = _c_b;
    _heat_production[_qp] = _h_prod;
  }
  _scale[_qp] = _scale_factor;
  if (_mesh.dimension() == 3)
    _gravity[_qp] = RealVectorValue(0., 0., -9.81);
  else if (_mesh.dimension() == 2)
    _gravity[_qp] = RealVectorValue(0., -9.81, 0.);
  else
    _gravity[_qp] = RealVectorValue(-9.81, 0., 0.);
}

Real
GMSMaterial::computeEOSlambda(Real temperature, Real pressure)
{
  Real lambda0, n, a, temp0 = 298.0;
  Real temp = temperature + KELVIN;
  Real pres = pressure / GIGAPA;
  if (pressure == 0.0)
  {
    lambda0 = 4.08;
    n = 0.406;
    a = 0.0;
  }
  else
  {
    lambda0 = 4.1;
    n = 0.493;
    a = 0.032;
  }
  return lambda0 * std::pow(temp0 / temp, n) * (1.0 + a * pres);
}