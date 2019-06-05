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

#include "GolemSetBCFromFile.h"

GolemSetBCFromFile::GolemSetBCFromFile(const int n_points,
                                       const std::vector<Real> & time_frames,
                                       const std::vector<std::string> & file_names,
                                       const ColumnMajorMatrix & px,
                                       const ColumnMajorMatrix & py,
                                       const ColumnMajorMatrix & pv)
  : _n_points(n_points),
    _time_frames(time_frames),
    _file_names(file_names),
    _px(px),
    _py(py),
    _pv(pv)
{
  errorCheck();
}

void
GolemSetBCFromFile::errorCheck()
{
  if (_time_frames.size() != _file_names.size())
    mooseError(
        "In GolemSetBCFromFile: Vectors _time_frames & _file_names are not of the same length");
  bool error = false;
  for (unsigned i = 0; !error && i + 1 < _time_frames.size(); ++i)
  {
    if (_time_frames[i] >= _time_frames[i + 1])
      error = true;
  }
  if (error)
    mooseError("In GolemSetBCFromFile: time values are not strictly increasing");
}

Real
GolemSetBCFromFile::sample(Real t, Real xcoord, Real ycoord)
{
  mooseAssert(_time_frames.size() > 0, "Sampling an empty GolemSetBCFromFile.");
  if (_file_names.size() > 1)
  {
    for (unsigned int i(0); i < _file_names.size() - 1; ++i)
    {
      if (t <= _time_frames[0])
        return find_value(0, xcoord, ycoord);
      else if (t >= _time_frames.back())
        return find_value(_time_frames.size() - 1, xcoord, ycoord);
      else if (t >= _time_frames[i] && t < _time_frames[i + 1])
        return find_value(i, xcoord, ycoord);
    }
  }
  else if (t != _time_frames[0])
    mooseError(
        "In GolemSetBCFromFile::sample : t= ", t, " but _time_frames= ", _time_frames[0], ".");
  return find_value(0, xcoord, ycoord);
}

Real
GolemSetBCFromFile::sampleTime(Real t, Real xcoord, Real ycoord)
{
  mooseAssert(_time_frames.size() > 0, "Sampling an empty GolemSetBCFromFile.");
  int posi;

  bool found = false;
  if (t <= _time_frames[0])
    return find_value(0, xcoord, ycoord);
  else if (t >= _time_frames.back())
    return find_value(_time_frames.size() - 1, xcoord, ycoord);
  else
  {
    found = false;
    for (unsigned int i(0); i + 1 < _time_frames.size(); ++i)
    {
      if (t >= _time_frames[i] && t < _time_frames[i + 1])
      {
        posi = i;
        found = true;
        break;
      }
    }
    if (!found)
      mooseError("GolemSetBCFromFile : sampleTime Unreachable? @ x=",
                 xcoord,
                 " , y=",
                 ycoord,
                 " , posi=",
                 posi,
                 " !!");
    Real val_i = find_value(posi, xcoord, ycoord);
    Real val_ii = find_value(posi + 1, xcoord, ycoord);
    return val_i +
           (val_ii - val_i) * (t - _time_frames[posi]) /
               (_time_frames[posi + 1] - _time_frames[posi]);
  }
  return 0;
}

Real
GolemSetBCFromFile::find_value(int posi, Real xcoord, Real ycoord)
{
  for (unsigned int i(0); i < _n_points; ++i)
  {
    if ((std::fabs(_px(posi, i) - xcoord) < 0.1) && (std::fabs(_py(posi, i) - ycoord) < 0.1))
      return _pv(posi, i);
  }
  mooseError("GolemSetBCFromFile : BC Unreachable?: x=",
             xcoord,
             " , y=",
             ycoord,
             " , in file= ",
             _file_names[posi],
             " !!");
  return 0;
}