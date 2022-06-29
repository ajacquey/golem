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

registerMooseObject("GolemApp", GolemFunctionReadFile);

InputParameters
GolemFunctionReadFile::validParams()
{
  InputParameters params = Function::validParams();
  params.addRequiredParam<std::string>(
      "file", "File holding csv/tab/space-separated data. It does not consider ; as a delimiter");
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
GolemFunctionReadFile::value(Real t, const Point & /*pt*/) const
{
  Real value = 0.0;   // default value to return
  bool found = false; // flag for console outprinting
  Real time_step;
  Real value_step;
  for (unsigned int i = 0; i < _time.size(); i++)
  {
    /*check time bounds - if trepassed break the loop*/
    /*smaller than firt time entry --> set default value*/
    if (t < _time[0])
      break;
    /*bigger than last entry --> set last value*/
    if (t > _time[_time.size() - 1])
    {
      value = _value[_time.size() - 1];
      break;
    }
    /*internal entries --> either hit or inside*/
    if ((t >= _time[i] && t < _time[i + 1]))
    {
      time_step = _time[i + 1] - _time[i];
      if (time_step == 0.0)
        break;
      value_step = _value[i + 1] - _value[i];
      value = _value[i + 1] - value_step * (_time[i + 1] - t) / time_step;
      found = true;
      break;
    }
  }
  /*value not found -- should one print an error or disregard (return 0)? I am not sure here*/
  if (!found)
    Moose::out << "GolemFunctionReadFile::value() - function value @ time = " << t
               << " not found. Set to default value = " << value << "\n";
  return value;
}

void
GolemFunctionReadFile::readFile()
{
  std::string line;
  MooseUtils::checkFileReadable(_file);
  std::ifstream stream;
  stream.open(_file.c_str());
  if (!stream.good())
    mooseError("Error opening file '", _file, "' from GolemFunctionReadFile function.");
  while (std::getline(stream, line))
  {
    if (line.empty())
      continue;
    if (line[0] != '#')
    {
      std::istringstream iss(line);
      std::vector<Real> vec_tmp;
      Real f;
      while (iss >> f)
        vec_tmp.push_back(f);
      if (vec_tmp.size() > 0)
      {
        if (vec_tmp.size() > 2)
          mooseError("Wrong number of columns [",
                     vec_tmp.size(),
                     "] from file '",
                     _file,
                     "' in GolemFunctionReadFile function.");
        _time.push_back(vec_tmp[0]);
        _value.push_back(vec_tmp[1]);
      }
      vec_tmp.clear();
    }
  }
  stream.close();
}