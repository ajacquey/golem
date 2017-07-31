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

#ifndef GOLEMPROPERTYREADFILE_H
#define GOLEMPROPERTYREADFILE_H

#include "GeneralUserObject.h"

class GolemPropertyReadFile;

template <>
InputParameters validParams<GolemPropertyReadFile>();

class GolemPropertyReadFile : public GeneralUserObject
{
public:
  GolemPropertyReadFile(const InputParameters & parameters);
  virtual ~GolemPropertyReadFile() {}
  virtual void initialize() {}
  virtual void execute() {}
  virtual void finalize() {}
  void readData();
  Real getData(const Elem *, unsigned int) const;

protected:
  std::string _prop_file_name;
  std::vector<Real> _Eledata;
  unsigned int _nprop;
  unsigned int _nelem;

private:
};

#endif // GOLEMPROPERTYREADFILE_H