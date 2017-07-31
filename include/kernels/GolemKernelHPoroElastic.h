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

#ifndef GOLEMKERNELHPOROELASTIC_H
#define GOLEMKERNELHPOROELASTIC_H

#include "Kernel.h"

class GolemKernelHPoroElastic;

template <>
InputParameters validParams<GolemKernelHPoroElastic>();

class GolemKernelHPoroElastic : public Kernel
{
public:
  GolemKernelHPoroElastic(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;
  const MaterialProperty<Real> & _biot;
  const MaterialProperty<Real> & _vol_strain_rate;
};

#endif // GOLEMKERNELHPOROELASTIC_H