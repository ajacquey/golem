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

#include "GolemTimeControl.h"

registerMooseObject("GolemApp", GolemTimeControl);

InputParameters
GolemTimeControl::validParams()
{
  InputParameters params = Control::validParams();
  params.addRequiredParam<FunctionName>("function", "The function to use.");
  params.addParam<std::vector<std::string>>(
      "enable_objects", std::vector<std::string>(), "Objects to enable.");
  params.addParam<std::vector<std::string>>(
      "disable_objects", std::vector<std::string>(), "Objects to disable.");
  return params;
}

GolemTimeControl::GolemTimeControl(const InputParameters & parameters)
  : Control(parameters),
    _function(getFunction("function")),
    _enable(getParam<std::vector<std::string>>("enable_objects")),
    _disable(getParam<std::vector<std::string>>("disable_objects"))
{
  if (!_fe_problem.isTransient())
    mooseError("GolemTimeControl: objects operate only on transient problems!");
  if (_enable.empty() && _disable.empty())
    mooseError("GolemTimeControl: both object lists are empty!");
  if (_function.value(_t, Point()) != -1 && _function.value(_t, Point()) != 1)
    mooseError("GolemTimeControl: wrong value in the input file found. Only +1 (enable) or -1 "
               "(disable) are possible!");
}

void
GolemTimeControl::initialSetup()
{
}

void
GolemTimeControl::execute()
{
  for (auto i = beginIndex(_enable); i < _enable.size(); ++i)
  {
    if (_function.value(_t, Point()) == 1)
      setControllableValueByName<bool>(_enable[i], std::string("enable"), true);
    else
      setControllableValueByName<bool>(_enable[i], std::string("enable"), false);
  }
  for (auto i = beginIndex(_disable); i < _disable.size(); ++i)
  {
    if (_function.value(_t, Point()) == -1)
      setControllableValueByName<bool>(_disable[i], std::string("enable"), false);
    else
      setControllableValueByName<bool>(_disable[i], std::string("enable"), true);
  }
}