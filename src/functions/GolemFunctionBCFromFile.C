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

#include "GolemFunctionBCFromFile.h"

registerMooseObject("GolemApp", GolemFunctionBCFromFile);

template <>
InputParameters
validParams<GolemFunctionBCFromFile>()
{
  InputParameters params = validParams<Function>();
  params.addParam<std::string>("data_file", "File holding csv/tab-separated bc data.");
  params.addParam<bool>(
      "interpolate_data_in_time", false, "Linearly interpolating among the data in time?");
  params.addParam<bool>(
      "interpolate_data_in_space", false, "Interpolating among the data in space?");
  return params;
}

GolemFunctionBCFromFile::GolemFunctionBCFromFile(const InputParameters & parameters)
  : Function(parameters),
    _set_bc(NULL),
    _interpolate_bc(NULL),
    _has_interpol_in_time(getParam<bool>("interpolate_data_in_time")),
    _has_interpol_in_space(getParam<bool>("interpolate_data_in_space")),
    _data_file_name(isParamValid("data_file") ? getParam<std::string>("data_file") : "")
{
  if (_data_file_name != "")
    parseFile();
  else
    mooseError("GolemFunctionBCFromFile: data_file_name is not specified.");
  ColumnMajorMatrix px(_time_frames.size(), 1);
  ColumnMajorMatrix py(_time_frames.size(), 1);
  ColumnMajorMatrix pz(_time_frames.size(), 1);
  fillMatrixBC(px, py, pz);
  if (!_has_interpol_in_space)
    _set_bc = new GolemSetBCFromFile(_n_points, _time_frames, _file_names, px, py, pz);
  else
    _interpolate_bc =
        new GolemInterpolateBCFromFile(_n_points, _time_frames, _file_names, px, py, pz);
}

GolemFunctionBCFromFile::~GolemFunctionBCFromFile()
{
  if (!_has_interpol_in_space)
    delete _set_bc;
  else
    delete _interpolate_bc;
}

Real
GolemFunctionBCFromFile::value(Real t, const Point & p) const
{
  if (!_has_interpol_in_space)
    return constant_value(t, p);
  return interpolated_value(t, p);
}

Real
GolemFunctionBCFromFile::interpolated_value(Real t, const Point & p) const
{
  if (_has_interpol_in_time)
    return _interpolate_bc->sampleTime(t, p(0), p(1));
  return _interpolate_bc->sample(t, p(0), p(1));
}

Real
GolemFunctionBCFromFile::constant_value(Real t, const Point & p) const
{
  if (_has_interpol_in_time)
    return _set_bc->sampleTime(t, p(0), p(1));
  return _set_bc->sample(t, p(0), p(1));
}

bool
GolemFunctionBCFromFile::parseNextLineReals(std::ifstream & ifs, std::vector<Real> & myvec)
{
  std::string line;
  myvec.clear();
  bool gotline(false);
  if (getline(ifs, line))
  {
    gotline = true;
    while (size_t pos = line.find(','))
    {
      if (pos == line.npos)
        break;
      line.replace(pos, 1, 1, ' ');
    }
    std::istringstream iss(line);
    Real f;
    while (iss >> f)
      myvec.push_back(f);
  }
  return gotline;
}

bool
GolemFunctionBCFromFile::parseNextLineStrings(std::ifstream & ifs, std::vector<std::string> & myvec)
{
  std::string line;
  myvec.clear();
  bool gotline(false);
  if (getline(ifs, line))
  {
    gotline = true;
    while (size_t pos = line.find(','))
    {
      if (pos == line.npos)
        break;
      line.replace(pos, 1, 1, ' ');
    }
    std::istringstream iss(line);
    std::string s;
    while (iss >> s)
      myvec.push_back(s);
  }
  return gotline;
}

void
GolemFunctionBCFromFile::parseFile()
{
  std::ifstream file(_data_file_name.c_str());
  if (!file.good())
    mooseError("Error opening file '", _data_file_name, "' from GolemFunctionBCFromFile function.");
  std::string line;
  while (parseNextLineReals(file, _time_frames))
  {
    if (_time_frames.size() > 0)
      break;
  }
  if (_time_frames.size() == 0)
    mooseError(
        "File '", _data_file_name, "' contains no data for GolemFunctionBCFromFile function.");
  while (parseNextLineStrings(file, _file_names))
  {
    if (_file_names.size() > 0)
      break;
  }
  if (_file_names.size() == 0)
    mooseError(
        "File '", _data_file_name, "' contains no y data for GolemFunctionBCFromFile function.");
  else if (_file_names.size() != _time_frames.size())
  {
    mooseError("Lengths of time_frames and file_names do not match in file '",
               _data_file_name,
               "' for GolemFunctionBCFromFile function.");
  }

  std::vector<Real> scratch;
  while (parseNextLineReals(file, scratch))
  {
    if (scratch.size() > 0)
      mooseError("Read more than two rows of data from file '",
                 _data_file_name,
                 "' for GolemFunctionBCFromFile function.");
  }
}

void
GolemFunctionBCFromFile::fillMatrixBC(ColumnMajorMatrix & px,
                                      ColumnMajorMatrix & py,
                                      ColumnMajorMatrix & pz)
{
  std::string line;
  std::vector<Real> myx;
  std::vector<Real> myy;
  std::vector<Real> myv;
  for (unsigned int i(0); i < _time_frames.size(); ++i)
  {
    std::ifstream file(_file_names[i].c_str());
    if (!file.good())
      mooseError(
          "Error opening file '", _file_names[i], "' from GolemFunctionBCFromFile function.");
    while (getline(file, line))
    {
      if (line[0] != '#')
      {
        std::istringstream iss(line);
        std::vector<Real> scratch;
        Real f;
        while (iss >> f)
          scratch.push_back(f);
        if (scratch.size() > 0)
        {
          if ((scratch.size() < 3) || (scratch.size() > 4))
            mooseError("Wrong number of columns from file '",
                       _file_names[i],
                       "' in GolemFunctionBCFromFile function.");
          if (scratch.size() == 3)
          {
            myx.push_back(scratch[0]);
            myy.push_back(scratch[1]);
            myv.push_back(scratch[2]);
          }
          else if (scratch.size() == 4)
          {
            myx.push_back(scratch[0]);
            myy.push_back(scratch[1]);
            myv.push_back(scratch[3]);
          }
        }
      }
    }
    _n_points = myx.size();
    px.reshape(_time_frames.size(), _n_points);
    py.reshape(_time_frames.size(), _n_points);
    pz.reshape(_time_frames.size(), _n_points);
    for (unsigned int j(0); j < _n_points; ++j)
    {
      px(i, j) = myx[j];
      py(i, j) = myy[j];
      pz(i, j) = myv[j];
    }
    myx.clear();
    myy.clear();
    myv.clear();
  }
}