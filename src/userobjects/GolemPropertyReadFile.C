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

#include "GolemPropertyReadFile.h"

template <>
InputParameters
validParams<GolemPropertyReadFile>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription(
      "User Object to read property data from an external file and assign to elements.");
  params.addParam<std::string>("prop_file_name", "", "Name of the property file name.");
  params.addParam<unsigned int>("nprop", 0, "number of property to read.");
  params.addParam<unsigned int>("nele", 0, "number of elements to read.");
  return params;
}

GolemPropertyReadFile::GolemPropertyReadFile(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _prop_file_name(getParam<std::string>("prop_file_name")),
    _nprop(getParam<unsigned int>("nprop")),
    _nelem(getParam<unsigned int>("nele"))
{
  readData();
}

void
GolemPropertyReadFile::readData()
{
  _Eledata.resize(_nelem * _nprop);
  MooseUtils::checkFileReadable(_prop_file_name);
  std::ifstream file_prop;
  file_prop.open(_prop_file_name.c_str());
  for (unsigned int i = 0; i < _nelem; ++i)
  {
    for (unsigned int j = 0; j < _nprop; ++j)
    {
      if (!(file_prop >> _Eledata[i * _nprop + j]))
        mooseError("Error GolemPropertyReadFile : Premature end of file!");
    }
  }
  file_prop.close();
}

Real
GolemPropertyReadFile::getData(const Elem * elem, unsigned int prop_num) const
{
  unsigned int jelem = elem->id();
  mooseAssert(jelem < _nelem,
              "Error GolemPropertyReadFile: Element "
                  << jelem
                  << " greater than total number of block elements\n");
  mooseAssert(prop_num < _nprop,
              "Error GolemPropertyReadFile: Property number "
                  << prop_num
                  << " greater than total number of properties "
                  << 4);
  return _Eledata[jelem * _nprop + prop_num];
}