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
  displacements = 'disp_x disp_y disp_z'
  temperature = temperature
[]

[Variables]
  [./temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 10
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
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
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
  [./stress_xx]
    type = GolemStress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_yy]
    type = GolemStress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
[]

[BCs]
  [./T0_back]
    type = DirichletBC
    variable = temperature
    boundary = back
    value = 10.0
    preset = false
  [../]
  [./T1_front]
    type = DirichletBC
    variable = temperature
    boundary = front
    value = 4.0
    preset = false
  [../]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0.0
    preset = true
  [../]
  [./no_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
    preset = true
  [../]
  [./no_z_back]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
    preset = true
  [../]
[]

[Materials]
  [./TMMaterial]
    type = GolemMaterialMElastic
    block = 0
    strain_model = incr_small_strain
    has_gravity = true
    gravity_acceleration = 9.81
    solid_density_initial = 2038.736
    young_modulus = 5.0e+09
    poisson_ratio = 0.25
    solid_thermal_expansion = 1.5e-05
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.0
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
  [./hypre]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_hypre_type
                           -ksp_type -ksp_rtol -ksp_max_it
                           -snes_type -snes_atol -snes_rtol -snes_max_it
                           -ksp_gmres_restart'
    petsc_options_value = 'hypre boomeramg
                           fgmres 1e-10 100
                           newtonls 1e-05 1e-10 100
                           201'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  automatic_scaling = true
  start_time = 0.0
  end_time = 1.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
