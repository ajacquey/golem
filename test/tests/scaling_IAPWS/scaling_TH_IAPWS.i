[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 100
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  zmin = -10
  zmax = 0
[]

[GlobalParams]
  pore_pressure = pore_pressure
  temperature = temperature
  has_lumped_mass_matrix = true
  scaling_uo = scaling
[]

[Variables]
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
  [../]
  [./temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 50.0
  [../]
[]

[Kernels]
  [./Htime]
    type = GolemKernelTimeH
    variable = pore_pressure
  [../]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
  [./Ttime]
    type = GolemKernelTimeT
    variable = temperature
  [../]
  [./TKernel]
    type = GolemKernelT
    variable = temperature
  [../]
  [./THKernel]
    type = GolemKernelTH
    variable = temperature
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
  [./fluid_density]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./fluid_viscosity]
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
  [./fluid_density_aux]
    type = MaterialRealAux
    variable = fluid_density
    property = fluid_density
  [../]
  [./fluid_viscosity_aux]
    type = MaterialRealAux
    variable = fluid_viscosity
    property = fluid_viscosity
  [../]
[]

[Functions]
  [./hydrostat]
    type = ParsedFunction
    value = 'p0-rho_f*g*z'
    vars = 'p0 rho_f g'
    vals = '1.0 1000e-06 9.81'
  [../]
[]

[ICs]
  [./pf_ic]
    type = FunctionIC
    variable = pore_pressure
    function = hydrostat
  [../]
[]

[BCs]
  [./p_front]
    type = DirichletBC
    variable = pore_pressure
    boundary = front
    value = 1.0
  [../]
  [./T_front]
    type = DirichletBC
    variable = temperature
    boundary = front
    value = 10
  [../]
[]

[Materials]
  [./THMaterial]
    type = GolemMaterialTH
    block = 0
    has_gravity = true
    porosity_initial = 0.1
    permeability_initial = 1.0e-11
    fluid_viscosity_initial = 1.0e-03
    fluid_density_initial = 1000
    solid_density_initial = 2000
    fluid_thermal_conductivity_initial = 10
    solid_thermal_conductivity_initial = 50
    fluid_heat_capacity_initial = 1100
    solid_heat_capacity_initial = 250
    fluid_modulus = 1.0e+06
    porosity_uo = porosity
    fluid_density_uo = fluid_density
    fluid_viscosity_uo = fluid_viscosity
    permeability_uo = permeability
    supg_uo = supg
  [../]
[]

[UserObjects]
  [./scaling]
    type = GolemScaling
    characteristic_length = 1.0
    characteristic_stress = 1.0e+06
    characteristic_time = 1.0
    characteristic_temperature = 1.0
  [../]
  [./porosity]
    type = GolemPorosityConstant
  [../]
  [./fluid_density]
    type = GolemFluidDensityIAPWS
  [../]
  [./fluid_viscosity]
    type = GolemFluidViscosityIAPWS
  [../]
  [./permeability]
    type = GolemPermeabilityConstant
  [../]
  [./supg]
    type = GolemSUPG
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
  solve_type = 'NEWTON'
  automatic_scaling = true
  start_time = 0.0
  end_time = 100000
  dt = 10000
[]

[Outputs]
  #interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
