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

#include "GMSEnergyResidual.h"

registerMooseObject("GolemApp", GMSEnergyResidual);

template <>
InputParameters
validParams<GMSEnergyResidual>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

GMSEnergyResidual::GMSEnergyResidual(const InputParameters & parameters)
  : Kernel(parameters),
    _bulk_thermal_conductivity(getMaterialProperty<Real>("bulk_thermal_conductivity")),
    _heat_production(getMaterialProperty<Real>("heat_production")),
    _bulk_specific_heat(getMaterialProperty<Real>("bulk_specific_heat")),
    _bulk_density(getMaterialProperty<Real>("bulk_density")),
    _scale_factor(getMaterialProperty<Real>("scale_factor"))
{
}

Real
GMSEnergyResidual::computeQpResidual()
{
  Real diff;
  Real e_source;
  if (_fe_problem.isTransient())
  {
    diff = _scale_factor[_qp] * _bulk_thermal_conductivity[_qp] /
           (_bulk_density[_qp] * _bulk_specific_heat[_qp]);
    e_source = _scale_factor[_qp] * _heat_production[_qp] /
               (_bulk_density[_qp] * _bulk_specific_heat[_qp]);
  }
  else
  {
    diff = _bulk_thermal_conductivity[_qp];
    e_source = _heat_production[_qp];
  }
  return diff * _grad_u[_qp] * _grad_test[_i][_qp] - e_source * _test[_i][_qp];
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
Real
GMSEnergyResidual::computeQpJacobian()
{
  Real diff;
  if (_fe_problem.isTransient())
    diff = _scale_factor[_qp] * _bulk_thermal_conductivity[_qp] /
           (_bulk_density[_qp] * _bulk_specific_heat[_qp]);
  else
    diff = _bulk_thermal_conductivity[_qp];
  return diff * _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}