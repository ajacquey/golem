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

#include "MooseTypes.h"
#include "ColumnMajorMatrix.h"

class GolemSetBCFromFile
{
public:
  GolemSetBCFromFile(const int n_points,
                     const std::vector<Real> & time_frames,
                     const std::vector<std::string> & file_names,
                     const ColumnMajorMatrix & px,
                     const ColumnMajorMatrix & py,
                     const ColumnMajorMatrix & pv);
  GolemSetBCFromFile()
    : _n_points(0), _time_frames(std::vector<Real>()), _file_names(std::vector<std::string>())
  {
  }
  Real sample(Real t, Real xcoord, Real ycoord);
  Real sampleTime(Real t, Real xcoord, Real ycoord);

protected:
  const unsigned int _n_points;
  const std::vector<Real> _time_frames;
  const std::vector<std::string> _file_names;
  ColumnMajorMatrix _px;
  ColumnMajorMatrix _py;
  ColumnMajorMatrix _pv;

private:
  void errorCheck();
  Real find_value(int posi, Real xcoord, Real ycoord);
};