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

#include "Function.h"
#include "GolemSetBCFromFile.h"
#include "GolemInterpolateBCFromFile.h"

class GolemFunctionBCFromFile : public Function
{
public:
  static InputParameters validParams();
  GolemFunctionBCFromFile(const InputParameters & parameters);
  virtual ~GolemFunctionBCFromFile();
  virtual Real value(Real t, const Point & p) const override;

protected:
  GolemSetBCFromFile * _set_bc;
  GolemInterpolateBCFromFile * _interpolate_bc;
  unsigned int _n_points;
  bool _has_interpol_in_time;
  bool _has_interpol_in_space;
  const std::string _data_file_name;
  std::vector<Real> _time_frames;
  std::vector<std::string> _file_names;

private:
  void parseFile();
  bool parseNextLineReals(std::ifstream & ifs, std::vector<Real> & myvec);
  bool parseNextLineStrings(std::ifstream & ifs, std::vector<std::string> & myvec);
  void fillMatrixBC(ColumnMajorMatrix & px, ColumnMajorMatrix & py, ColumnMajorMatrix & pz);
  Real constant_value(Real t, const Point & pt) const;
  Real interpolated_value(Real t, const Point & pt) const;
};