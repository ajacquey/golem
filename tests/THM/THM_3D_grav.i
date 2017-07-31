[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 10
  xmin = 0
  xmax = 6
  ymin = 0
  ymax = 6
  zmin = 0
  zmax = 30
[]

[GlobalParams]
  pore_pressure = pore_pressure
  temperature = temperature
  displacements = 'disp_x disp_y disp_z'
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
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
  [./TKernel]
    type = GolemKernelT
    variable = temperature
  [../]
  [./MKernel_x]
    type = GolemKernelM
    variable = disp_x
    component = 0
  [../]
  [./MKernel_y]
    type = GolemKernelM
    variable = disp_y
    component = 1
  [../]
  [./MKernel_z]
    type = GolemKernelM
    variable = disp_z
    component = 2
  [../]
[]

[AuxVariables]
  [./strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./strain_zz]
    type = GolemStrain
    variable = strain_zz
    index_i = 2
    index_j = 2
  [../]
  [./stress_zz]
    type = GolemStress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
[]

[BCs]
  [./pf_top]
    type = DirichletBC
    variable = pore_pressure
    boundary = front
    value = 0.0
  [../]
  [./T_domain]
    type = DirichletBC
    variable = temperature
    boundary = 'top bottom left right front back'
    value = 2.5
  [../]
  [./no_x]
    type = PresetBC
    variable = disp_x
    boundary = 'left right'
    value = 0.0
  [../]
  [./no_y]
    type = PresetBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
  [../]
  [./no_z]
    type = PresetBC
    variable = disp_z
    boundary = 'back front'
    value = 0.0
  [../]
[]

[Materials]
  [./MMaterial]
    type = GolemMaterialMElastic
    block = 0
    has_gravity = true
    solid_density_initial = 2038.736
    fluid_density_initial = 1019.368
    gravity_acceleration = 9.81
    strain_model = incr_small_strain
    young_modulus = 10.0e+09
    poisson_ratio = 0.25
    porosity_initial = 0.0
    permeability_initial = 1.0e-12
    fluid_viscosity_initial = 1.0e-03
    solid_thermal_expansion = 3.0e-06
    fluid_thermal_expansion = 0.0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.0
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
  [./precond]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  start_time = 0.0
  end_time = 1.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]