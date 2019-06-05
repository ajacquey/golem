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

namespace GolemM
{

/**
 * This is used for the standard kernel stress_ij*d(test)/dx_j, when varied wrt u_k
 * Jacobian entry: d(stress_ij*d(test)/dx_j)/du_k = d(C_ijmn*du_m/dx_n*dtest/dx_j)/du_k
 */
Real elasticJacobian(const RankFourTensor & r4t,
                     unsigned int i,
                     unsigned int k,
                     const RealGradient & grad_test,
                     const RealGradient & grad_phi);

/**
 * Get the shear modulus for an isotropic elasticity tensor
 * param elasticity_tensor the tensor (must be isotropic, but not checked for efficiency)
 */
Real getIsotropicShearModulus(const RankFourTensor & elasticity_tensor);

/**
 * Get the bulk modulus for an isotropic elasticity tensor
 * param elasticity_tensor the tensor (must be isotropic, but not checked for efficiency)
 */
Real getIsotropicBulkModulus(const RankFourTensor & elasticity_tensor);

/**
 * Get the Young's modulus for an isotropic elasticity tensor
 * param elasticity_tensor the tensor (must be isotropic, but not checked for efficiency)
 */
Real getIsotropicYoungsModulus(const RankFourTensor & elasticity_tensor);
}