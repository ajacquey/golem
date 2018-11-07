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

#include "GolemKernelHPoroElastic.h"
#include "MooseMesh.h"

registerMooseObject("GolemApp", GolemKernelHPoroElastic);

template <>
InputParameters
validParams<GolemKernelHPoroElastic>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription(
      "Kernel for poroelastic effect (solid to fluid) in the pore pressure equation.");
  params.addRequiredCoupledVar("displacements", "The displacement variables vector.");
  return params;
}

GolemKernelHPoroElastic::GolemKernelHPoroElastic(const InputParameters & parameters)
  : Kernel(parameters),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _biot(getMaterialProperty<Real>("biot_coefficient")),
    _vol_strain_rate(getMaterialProperty<Real>("volumetric_strain_rate"))
{
  if (_ndisp != _mesh.dimension())
    mooseError("The number of displacement variables supplied must match the mesh dimension.");
  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);
}

/******************************************************************************/
/*                                RESIDUAL                                    */
/******************************************************************************/
Real
GolemKernelHPoroElastic::computeQpResidual()
{
  return _biot[_qp] * _vol_strain_rate[_qp] * _test[_i][_qp];
}

/******************************************************************************/
/*                                  JACOBIAN                                  */
/******************************************************************************/
Real
GolemKernelHPoroElastic::computeQpJacobian()
{
  return 0.0;
}

/******************************************************************************/
/*                            OFF DIAGONAL JACOBIAN                           */
/******************************************************************************/
Real
GolemKernelHPoroElastic::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
      return _biot[_qp] * _grad_phi[_j][_qp](i) * _test[_i][_qp] / _dt;
  return 0.0;
}