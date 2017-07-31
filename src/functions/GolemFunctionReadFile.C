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

#include "GolemFunctionReadFile.h"

template <>
InputParameters
validParams<GolemFunctionReadFile>()
{
  InputParameters params = validParams<Function>();
  params.addRequiredParam<std::string>("file", "File holding csv/tab/space-separated data.");
  return params;
}

GolemFunctionReadFile::GolemFunctionReadFile(const InputParameters & parameters)
  : Function(parameters), _file(getParam<std::string>("file"))
{
  _time.clear();
  _value.clear();
  readFile();
}

Real
GolemFunctionReadFile::value(Real t, const Point &)
{
  Real value = 0.0;
  for (unsigned int i = 0; i < _time.size(); ++i)
    if (_time[i] == t || (t > _time[i] && t < _time[i + 1]))
    {
      value += _value[i];
      break;
    }
  return value;
}

void
GolemFunctionReadFile::readFile()
{
  std::string line;
  std::ifstream file(_file.c_str());
  if (!file.good())
    mooseError("Error opening file '", _file, "' from GolemFunctionReadFile function.");
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
        if (scratch.size() > 2)
          mooseError("Wrong number of columns [",
                     scratch.size(),
                     "] from file '",
                     _file,
                     "' in GolemFunctionReadFile function.");
        _time.push_back(scratch[0]);
        _value.push_back(scratch[1]);
      }
    }
  }
}