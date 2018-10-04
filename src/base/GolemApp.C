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

#include "GolemApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

// Actions
#include "GolemPressureAction.h"

// Materials
#include "GMSMaterial.h"
#include "GolemMaterialBase.h"
#include "GolemMaterialH.h"
#include "GolemMaterialT.h"
#include "GolemMaterialTH.h"
#include "GolemMaterialMElastic.h"
#include "GolemMaterialMInelastic.h"
#include "GolemDruckerPrager.h"

// Kernels
#include "GMSEnergyTimeDerivative.h"
#include "GMSEnergyResidual.h"
#include "GMSMassResidual.h"
#include "GolemKernelTimeH.h"
#include "GolemKernelTimeT.h"
#include "GolemKernelH.h"
#include "GolemKernelT.h"
#include "GolemKernelTH.h"
#include "GolemKernelM.h"
#include "GolemKernelHPoroElastic.h"

// AuxKernels
#include "GolemDarcyVelocity.h"
#include "GolemFluidVelocity.h"
#include "GolemStress.h"
#include "GolemStrain.h"
#include "GolemThermalStress.h"
#include "GolemEqvInelasticStrain.h"

// DiracKernels
#include "GolemDiracKernelTH.h"

// BCs
#include "GolemConvectiveTHBC.h"
#include "GolemVelocityBC.h"
#include "GolemHeatFlowBC.h"
#include "GolemPressureBC.h"

// Controls
#include "GolemTimeControl.h"

// Functions
#include "GolemFunctionBCFromFile.h"
#include "GolemFunctionReadFile.h"

// UserObjects
#include "GolemScaling.h"
#include "GolemSUPG.h"
#include "GolemPropertyReadFile.h"
#include "GolemFluidDensityConstant.h"
#include "GolemFluidDensityLinear.h"
#include "GolemFluidDensityIAPWS.h"
#include "GolemFluidViscosityConstant.h"
#include "GolemFluidViscosityLinear.h"
#include "GolemFluidViscosityIAPWS.h"
#include "GolemPorosityConstant.h"
#include "GolemPorosityTHM.h"
#include "GolemPermeabilityConstant.h"
#include "GolemPermeabilityKC.h"
#include "GolemPermeabilityCubicLaw.h"
#include "GolemHardeningConstant.h"
#include "GolemHardeningCubic.h"
#include "GolemHardeningExponential.h"
#include "GolemHardeningPlasticSaturation.h"

template <>
InputParameters
validParams<GolemApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

GolemApp::GolemApp(InputParameters parameters) : MooseApp(parameters)
{
  GolemApp::registerAll(_factory, _action_factory, _syntax);
}

GolemApp::~GolemApp() {}

void
GolemApp::registerApps()
{
  registerApp(GolemApp);
}

void
GolemApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)	
{
  Registry::registerObjectsTo(f, {"GolemApp"});
  Registry::registerActionsTo(af, {"GolemApp"});
}

void
GolemApp::registerObjects(Factory & factory)
{
  mooseDeprecated("use registerAll instead of registerObjects");
  // Materials
  registerMaterial(GMSMaterial);
  registerMaterial(GolemMaterialBase);
  registerMaterial(GolemMaterialH);
  registerMaterial(GolemMaterialT);
  registerMaterial(GolemMaterialTH);
  registerMaterial(GolemMaterialMElastic);
  registerMaterial(GolemMaterialMInelastic);
  registerMaterial(GolemDruckerPrager);

  // Kernels
  registerKernel(GMSEnergyTimeDerivative);
  registerKernel(GMSEnergyResidual);
  registerKernel(GMSMassResidual);
  registerKernel(GolemKernelTimeH);
  registerKernel(GolemKernelTimeT);
  registerKernel(GolemKernelH);
  registerKernel(GolemKernelT);
  registerKernel(GolemKernelTH);
  registerKernel(GolemKernelM);
  registerKernel(GolemKernelHPoroElastic);

  // AuxKernels
  registerAux(GolemDarcyVelocity);
  registerAux(GolemFluidVelocity);
  registerAux(GolemStress);
  registerAux(GolemStrain);
  registerAux(GolemThermalStress);
  registerAux(GolemEqvInelasticStrain);

  // DiracKernels
  registerDiracKernel(GolemDiracKernelTH);

  // BCs
  registerBoundaryCondition(GolemConvectiveTHBC);
  registerBoundaryCondition(GolemVelocityBC);
  registerBoundaryCondition(GolemHeatFlowBC);
  registerBoundaryCondition(GolemPressureBC);

  // Controls
  registerControl(GolemTimeControl);

  // Functions
  registerFunction(GolemFunctionBCFromFile);
  registerFunction(GolemFunctionReadFile);

  // UserObjects
  registerUserObject(GolemScaling);
  registerUserObject(GolemSUPG);
  registerUserObject(GolemPropertyReadFile);
  registerUserObject(GolemFluidDensityConstant);
  registerUserObject(GolemFluidDensityLinear);
  registerUserObject(GolemFluidDensityIAPWS);
  registerUserObject(GolemFluidViscosityConstant);
  registerUserObject(GolemFluidViscosityLinear);
  registerUserObject(GolemFluidViscosityIAPWS);
  registerUserObject(GolemPorosityConstant);
  registerUserObject(GolemPorosityTHM);
  registerUserObject(GolemPermeabilityConstant);
  registerUserObject(GolemPermeabilityKC);
  registerUserObject(GolemPermeabilityCubicLaw);
  registerUserObject(GolemHardeningConstant);
  registerUserObject(GolemHardeningCubic);
  registerUserObject(GolemHardeningExponential);
  registerUserObject(GolemHardeningPlasticSaturation);
}

void
GolemApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  mooseDeprecated("use registerAll instead of associateSyntax");
  registerSyntax("EmptyAction", "BCs/GolemPressure");
  registerSyntax("GolemPressureAction", "BCs/GolemPressure/*");

  registerAction(GolemPressureAction, "add_bc");
}

extern "C" void
GolemApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  GolemApp::registerAll(f, af, s);
}