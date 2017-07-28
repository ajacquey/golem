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

#include "GolemH.h"
#include "MooseError.h"
#include "MooseEnum.h"

RankTwoTensor
computeKernel(std::vector<Real> k0, MooseEnum dist, Real den, int dim)
{
  RealVectorValue kx;
  RealVectorValue ky;
  RealVectorValue kz;
  RankTwoTensor k;
  if (dim == 1)
  {
    switch (dist)
    {
      case 1:
        if (k0.size() != 1)
          mooseError(
              "One input value is needed for isotropic distribution of permeability! You provided ",
              k0.size(),
              " values.\n");
        kx = RealVectorValue(k0[0] * den, 0.0, 0.0);
        ky = RealVectorValue(0.0, 0.0, 0.0);
        kz = RealVectorValue(0.0, 0.0, 0.0);
        break;
      case 2:
      case 3:
        mooseError("One dimensional elements cannot have non-isotropic permeability values.\n");
        break;
    }
  }
  else if (dim == 2)
  {
    switch (dist)
    {
      case 1:
        if (k0.size() != 1)
          mooseError(
              "One input value is needed for isotropic distribution of permeability! You provided ",
              k0.size(),
              " values.\n");
        kx = RealVectorValue(k0[0] * den, 0.0, 0.0);
        ky = RealVectorValue(0.0, k0[0] * den, 0.0);
        kz = RealVectorValue(0.0, 0.0, 0.0);
        break;
      case 2:
        if (k0.size() != 2)
          mooseError("Two input values are needed for orthotropic distribution of permeability! "
                     "You provided ",
                     k0.size(),
                     " values.\n");
        kx = RealVectorValue(k0[0] * den, 0.0, 0.0);
        ky = RealVectorValue(0.0, k0[1] * den, 0.0);
        kz = RealVectorValue(0.0, 0.0, 0.0);
        break;
      case 3:
        mooseError("Two dimensional elements cannot have non-isotropic permeability values.\n");
        break;
    }
  }
  else if (dim == 3)
  {
    switch (dist)
    {
      case 1:
        if (k0.size() != 1)
          mooseError(
              "One input value is needed for isotropic distribution of permeability! You provided ",
              k0.size(),
              " values.\n");
        kx = RealVectorValue(k0[0] * den, 0.0, 0.0);
        ky = RealVectorValue(0.0, k0[0] * den, 0.0);
        kz = RealVectorValue(0.0, 0.0, k0[0] * den);
        break;
      case 2:
        if (k0.size() != 3)
          mooseError("Three input values are needed for orthotropic distribution of permeability! "
                     "You provided ",
                     k0.size(),
                     " values.\n");
        kx = RealVectorValue(k0[0] * den, 0.0, 0.0);
        ky = RealVectorValue(0.0, k0[1] * den, 0.0);
        kz = RealVectorValue(0.0, 0.0, k0[2] * den);
        break;
      case 3:
        if (k0.size() != 9)
          mooseError("Nine input values are needed for anisotropic distribution of permeability! "
                     "You provided ",
                     k0.size(),
                     " values.\n");
        kx = RealVectorValue(k0[0] * den, k0[1] * den, k0[2] * den);
        ky = RealVectorValue(k0[3] * den, k0[4] * den, k0[5] * den);
        kz = RealVectorValue(k0[6] * den, k0[7] * den, k0[8] * den);
        break;
    }
  }
  k = RankTwoTensor(kx, ky, kz);
  return k;
}