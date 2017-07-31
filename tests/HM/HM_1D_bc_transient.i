[Mesh]
  type = FileMesh
  file = HM_1D_transient.msh
  boundary_id = '0 1 2 3 4 5'
  boundary_name = 'left right bottom top front back'
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  pore_pressure = pore_pressure
[]

[Variables]
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0
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
  [./HMKernel]
    type = GolemKernelHPoroElastic
    variable = pore_pressure
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
  [./stress_xx]
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
  [./stress_xx]
    type = GolemStress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
[]

[Functions]
  [./func_q1_right]
    type = ParsedFunction
    value = 'q_rate*t'
    vars = 'q_rate'
    vals = '7.6e-05'
  [../]
[]

[BCs]
  [./p0_left]
    type = PresetBC
    variable = pore_pressure
    boundary = left
    value = 0.0
  [../]
  [./q1_right]
    type = FunctionNeumannBC
    variable = pore_pressure
    boundary = right
    function = func_q1_right
  [../]
  [./no_x_right]
    type = PresetBC
    variable = disp_x
    boundary = right
    value = 0.0
  [../]
  [./no_y]
    type = PresetBC
    variable = disp_y
    boundary = 'front back'
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
  [./HMMaterial]
    type = GolemMaterialMElastic
    block = 0
    strain_model = incr_small_strain
    young_modulus = 27.0e+03
    poisson_ratio = 0.2
    permeability_initial = 1.0e-10
    fluid_viscosity_initial = 1.0e-03
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
  end_time = 10.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
  #interval = 5
[]
