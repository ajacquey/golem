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

#include "GeneralUserObject.h"

class GolemSUPG : public GeneralUserObject
{
public:
  static InputParameters validParams();
  GolemSUPG(const InputParameters & parameters);
  virtual void initialize() {}
  virtual void execute() {}
  virtual void finalize() {}
  static MooseEnum eleType();
  static MooseEnum methodType();
  virtual Real tau(RealVectorValue vel, Real diff, Real dt, const Elem * ele) const;

protected:
  MooseEnum _effective_length;
  MooseEnum _method;

private:
  Real EEL(RealVectorValue vel, const Elem * ele) const;
  Real cosh_relation(Real) const;
  Real Full(Real, Real, Real) const;
  Real Temporal(Real, Real, Real, Real) const;
  Real DoublyAsymptotic(Real, Real, Real) const;
  Real Critical(Real, Real, Real) const;
};