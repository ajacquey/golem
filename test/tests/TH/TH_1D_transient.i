[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 100
  ny = 1
  nz = 1
  xmin = 0
  xmax = 10
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
[]

[GlobalParams]
  pore_pressure = pore_pressure
  temperature = temperature
[]

[Variables]
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./temperature]
    order = FIRST
    family = LAGRANGE
   initial_condition = 0.0
  [../]
[]

[Kernels]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
  [./temp_time]
    type = GolemKernelTimeT
    variable = temperature
  [../]
  [./THKernel]
    type = GolemKernelTH
    variable = temperature
    is_conservative = true
  [../]
[]

[AuxVariables]
  [./vx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./vy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./vz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./darcyx]
    type = GolemDarcyVelocity
    variable = vx
    component = 0
  [../]
  [./darcyy]
    type = GolemDarcyVelocity
    variable = vy
    component = 1
  [../]
  [./darcyz]
    type = GolemDarcyVelocity
    variable = vz
    component = 2
  [../]
[]

[BCs]
  [./p_left]
    type = PresetBC
    variable = pore_pressure
    boundary = left
    value = 1.0e+05
  [../]
  [./p0_right]
    type = PresetBC
    variable = pore_pressure
    boundary = right
    value = 0.0
  [../]
  [./T0_left]
    type = PresetBC
    variable = temperature
    boundary = left
    value = 10
  [../]
  [./T_right]
    type = GolemConvectiveTHBC
    variable = temperature
    boundary = right
  [../]
[]

[Materials]
  [./THMaterial]
    type = GolemMaterialTH
    block = 0
    porosity_initial = 0.1
    permeability_initial = 1.0e-11
    fluid_viscosity_initial = 1.0e-03
    fluid_density_initial = 1000
    solid_density_initial = 2000
    fluid_thermal_conductivity_initial = 10
    solid_thermal_conductivity_initial = 50
    fluid_heat_capacity_initial = 1100
    solid_heat_capacity_initial = 250
    porosity_uo = porosity
    fluid_density_uo = fluid_density
    fluid_viscosity_uo = fluid_viscosity
    permeability_uo = permeability
  [../]
[]

[UserObjects]
  [./porosity]
    type = GolemPorosityConstant
  [../]
  [./fluid_density]
    type = GolemFluidDensityConstant
  [../]
  [./fluid_viscosity]
    type = GolemFluidViscosityConstant
  [../]
  [./permeability]
    type = GolemPermeabilityConstant
  [../]
[]

[Preconditioning]
  [./fieldsplit]
    type = FSP
    topsplit = pT
    [./pT]
      splitting = 'p T'
      splitting_type = multiplicative
      petsc_options_iname = '-ksp_type
                             -ksp_rtol -ksp_max_it
                             -snes_type -snes_linesearch_type
                             -snes_atol -snes_rtol -snes_max_it'
      petsc_options_value = 'fgmres
                             1.0e-12 50
                             newtonls cp
                             1.0e-05 1.0e-12 25'
    [../]
    [./p]
     vars = 'pore_pressure'
     petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -sub_pc_factor_levels -ksp_rtol -ksp_max_it'
     petsc_options_value = 'fgmres asm ilu 1 1e-12 500'
    [../]
    [./T]
     vars = 'temperature'
     petsc_options_iname = '-ksp_type -pc_type -pc_hypre_type -ksp_rtol -ksp_max_it'
     petsc_options_value = 'preonly hypre boomeramg 1e-12 500'
    [../]
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  start_time = 0.0
  end_time = 20000
  dt = 2000
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
