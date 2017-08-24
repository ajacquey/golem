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

#ifndef GOLEMHARDENINGEXPONENTIAL_H
#define GOLEMHARDENINGEXPONENTIAL_H

#include "GolemHardeningModel.h"

class GolemHardeningExponential;

template <>
InputParameters validParams<GolemHardeningExponential>();

class GolemHardeningExponential : public GolemHardeningModel
{
public:
  GolemHardeningExponential(const InputParameters & parameters);
  Real value(Real intnl) const;
  Real dvalue(Real intnl) const;
  virtual std::string modelName() const { return "Exponential"; }

private:
  Real _val_ini;
  Real _val_res;
  Real _rate;
};

#endif // GOLEMHARDENINGEXPONENTIAL_H