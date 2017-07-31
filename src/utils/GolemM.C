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

#include "MooseTypes.h"
#include "RankFourTensor.h"

namespace GolemM
{

Real
elasticJacobian(const RankFourTensor & r4t,
                unsigned int i,
                unsigned int k,
                const RealGradient & grad_test,
                const RealGradient & grad_phi)
{
  // d(stress_ij*d(test)/dx_j)/du_k = d(C_ijmn*du_m/dx_n dtest/dx_j)/du_k (which is nonzero for m ==
  // k)
  const Real gt0 = grad_test(0);
  const Real gt1 = grad_test(1);
  const Real gt2 = grad_test(2);
  const Real gp0 = grad_phi(0);
  const Real gp1 = grad_phi(1);
  const Real gp2 = grad_phi(2);

  // clang-format off
  // This is the algorithm that is unrolled below:
  //
  //    Real sum = 0.0;
  //    for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
  //      for (unsigned int l = 0; l < LIBMESH_DIM; ++l)
  //        sum += r4t(i, j, k, l) * grad_phi(l) * grad_test(j);
  //    return sum;

  return
     (
         r4t(i,0,k,0) * gp0
       + r4t(i,0,k,1) * gp1
       + r4t(i,0,k,2) * gp2
     ) * gt0
     +
     (
         r4t(i,1,k,0) * gp0
       + r4t(i,1,k,1) * gp1
       + r4t(i,1,k,2) * gp2
     ) * gt1
     +
     (
       r4t(i,2,k,0) * gp0
     + r4t(i,2,k,1) * gp1
     + r4t(i,2,k,2) * gp2
     ) * gt2;
  // clang-format on
}

Real
getIsotropicShearModulus(const RankFourTensor & elasticity_tensor)
{
  const Real shear_modulus = elasticity_tensor(0, 1, 0, 1);
  return shear_modulus;
}

Real
getIsotropicBulkModulus(const RankFourTensor & elasticity_tensor)
{
  const Real shear_modulus = getIsotropicShearModulus(elasticity_tensor);
  // dilatational modulus is defined as lambda plus two mu
  const Real dilatational_modulus = elasticity_tensor(0, 0, 0, 0);
  const Real lambda = dilatational_modulus - 2.0 * shear_modulus;
  const Real bulk_modulus = lambda + 2.0 * shear_modulus / 3.0;
  return bulk_modulus;
}

Real
getIsotropicYoungsModulus(const RankFourTensor & elasticity_tensor)
{
  const Real shear_modulus = getIsotropicShearModulus(elasticity_tensor);
  // dilatational modulus is defined as lambda plus two mu
  const Real dilatational_modulus = elasticity_tensor(0, 0, 0, 0);
  const Real lambda = dilatational_modulus - 2.0 * shear_modulus;
  const Real youngs_modulus =
      shear_modulus * (3.0 * lambda + 2.0 * shear_modulus) / (lambda + shear_modulus);
  return youngs_modulus;
}
}