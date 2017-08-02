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

#ifndef GOLEMHARDENINGMODEL_H
#define GOLEMHARDENINGMODEL_H

#include "GeneralUserObject.h"

class GolemHardeningModel;

template <>
InputParameters validParams<GolemHardeningModel>();

class GolemHardeningModel : public GeneralUserObject
{
public:
  GolemHardeningModel(const InputParameters & parameters);
  void initialize() {}
  void execute() {}
  void finalize() {}
  virtual Real value(Real intnl) const = 0;
  virtual Real dvalue(Real intnl) const = 0;
  virtual std::string modelName() const = 0;

protected:
  bool _is_radians;
};

#endif // GOLEMHARDENINGMODEL_H