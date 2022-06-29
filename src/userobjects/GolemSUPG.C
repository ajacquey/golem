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

#include "GolemSUPG.h"

registerMooseObject("GolemApp", GolemSUPG);

InputParameters
GolemSUPG::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addParam<MooseEnum>("effective_length",
                             GolemSUPG::eleType() = "min",
                             "The characteristic element length for SUPG upwinding.");
  params.addParam<MooseEnum>("method", GolemSUPG::methodType() = "full", "The SUPG method.");
  params.addClassDescription(
      "User Object class to implement Streamline Petrov-Galerking Upwind technique.");
  return params;
}

GolemSUPG::GolemSUPG(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _effective_length(getParam<MooseEnum>("effective_length")),
    _method(getParam<MooseEnum>("method"))
{
}

MooseEnum
GolemSUPG::eleType()
{
  return MooseEnum("min=1 max=2 average=3 streamline=4");
}

MooseEnum
GolemSUPG::methodType()
{
  return MooseEnum("full=1 temporal=2 doubly_asymptotic=3 critical=4");
}

Real
GolemSUPG::tau(RealVectorValue vel, Real diff, Real dt, const Elem * ele) const
{
  Real norm_v = vel.norm();
  Real h_ele = EEL(vel, ele);
  Real tau = 0.0;
  switch (_method)
  {
    case 1: // full
      tau += Full(norm_v, h_ele, diff);
      break;
    case 2: // temporal
      tau += Temporal(norm_v, h_ele, diff, dt);
      break;
    case 3:
      tau += DoublyAsymptotic(norm_v, h_ele, diff);
      break;
    case 4:
      tau += Critical(norm_v, h_ele, diff);
      break;
  }
  return tau;
}

Real
GolemSUPG::Full(Real norm_v, Real h_ele, Real diff) const
{
  Real alpha = 0.5 * norm_v * h_ele / diff;
  Real tau = (norm_v > 0.0) ? cosh_relation(alpha) / norm_v : 0.0;
  return tau;
}

Real
GolemSUPG::Temporal(Real norm_v, Real h_ele, Real diff, Real dt) const
{
  return (1.0 / sqrt((2.0 / dt) * (2.0 / dt) + (2.0 * norm_v / h_ele) * (2.0 * norm_v / h_ele) +
                     (4.0 * diff / (h_ele * h_ele)) * (4.0 * diff / (h_ele * h_ele))));
}

Real
GolemSUPG::DoublyAsymptotic(Real norm_v, Real h_ele, Real diff) const
{
  Real alpha = 0.5 * norm_v * h_ele / diff;
  Real s = 0.0;
  if (-3.0 <= alpha && alpha <= 3.0)
    s = alpha / 3.0;
  else
    s = alpha * std::sqrt(alpha * alpha);
  return s;
}

Real
GolemSUPG::Critical(Real norm_v, Real h_ele, Real diff) const
{
  Real alpha = 0.5 * norm_v * h_ele / diff;
  Real s = 0.0;
  if (alpha < 1.0)
    s = -1.0 - 1.0 / alpha;
  else if (-1.0 <= alpha && alpha <= 1.0)
    s = 0.0;
  else if (1.0 < alpha)
    s = 1.0 - 1.0 / alpha;
  return s;
}

Real
GolemSUPG::cosh_relation(Real alpha) const
{
  Real s = 0.0;
  if (alpha < 0.01)
    s = alpha * (1.0 / 3.0 + alpha * alpha * (-1.0 / 45.0 + 18.0 / 8505.0 * alpha * alpha));
  else if (0.01 <= alpha && alpha < 20)
    s = (exp(alpha) + exp(-alpha)) / (exp(alpha) - exp(-alpha)) - 1 / alpha;
  else if (20 <= alpha)
    s = 1.0;
  return s;
}

Real
GolemSUPG::EEL(RealVectorValue vel, const Elem * ele) const
{
  Real L = 0.0;
  RealVectorValue my_vel = vel;
  if (ele->dim() == 1)
    L += ele->volume();
  else
  {
    switch (_effective_length)
    {
      case 1: // min
        L += ele->hmin();
        break;
      case 2: // max
        L += ele->hmax();
        break;
      case 3: // average
        L += 0.5 * (ele->hmin() + ele->hmax());
        break;
      case 4: // stream line length
        mooseError("GolemSUPG::EEL() --> stream line length needs some revision");
        break;
    }
  }
  return L;
}