[Mesh]
  type = FileMesh
  file = TM_1D_bc_transient.msh
  boundary_id = '0 1 2 3 4 5'
  boundary_name = 'left right bottom top front back'
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  temperature = 'temperature'
[]

[Variables]
  [./temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
    scaling = 1.0e+04
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
  [./T_time]
    type = GolemKernelTimeT
    variable = temperature
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
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./strain_xx]
    type = GolemStrain
    variable = strain_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_yy]
    type = GolemStress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./stress_zz]
    type = GolemStress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
[]

[Functions]
  [./qf_right_func]
    type = ParsedFunction
    value = 'q1*t'
    vars = 'q1'
    vals = '4.465e-06'
[]

[BCs]
  [./qf_right]
    type = FunctionNeumannBC
    variable = temperature
    boundary = right
    function = qf_right_func
  [../]
  [./no_x_left]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./no_y]
    type = PresetBC
    variable = disp_y
    boundary = 'back front'
    value = 0.0
  [../]
  [./no_z]
    type = PresetBC
    variable = disp_z
    boundary = 'bottom top'
    value = 0.0
  [../]
[]

[Materials]
  [./TM_left]
    type = GolemMaterialMElastic
    block = 0
    strain_model = incr_small_strain
    young_modulus = 25.0e+09
    poisson_ratio = 0.25
    solid_thermal_expansion = 9.0e-05
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.1574074
    solid_density_initial = 2000
    solid_heat_capacity_initial = 0.01
    porosity_uo = porosity
    fluid_density_uo = fluid_density
  [../]
  [./TM_right]
    type = GolemMaterialMElastic
    block = 1
    strain_model = incr_small_strain
    young_modulus = 25.0e+09
    poisson_ratio = 0.25
    solid_thermal_expansion = 9.0e-05
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.1574074
    solid_density_initial = 2000
    solid_heat_capacity_initial = 0.02
    porosity_uo = porosity
    fluid_density_uo = fluid_density
  [../]
[]

[UserObjects]
  [./porosity]
    type = GolemPorosityConstant
  [../]
  [./fluid_density]
    type = GolemFluidDensityConstant
  [../]
[]

[Preconditioning]
  [./precond]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1E-08 1E-10 20 50 lu NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  #solve_type = Newton
  start_time = 0.0
  end_time = 7776
  dt = 777.6
  num_steps = 10
[]

[Outputs]
  interval = 5
  execute_on = 'timestep_end'
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
