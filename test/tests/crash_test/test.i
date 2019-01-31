[Mesh]
  type = FileMesh
  file = mesh_sd.e
  #nemesis = true
  boundary_id = '1 2 3 4 5 6 7 8'
  boundary_name = 'front right back left bottom sd1 sd2 top'
  block_id = '0 1 2 3'
  block_name = 'u1 u2 f1 f2'
[]

[GlobalParams]
  pore_pressure = pore_pressure
  temperature = temperature
  strain_model = incr_small_strain
  has_lumped_mass_matrix = true
  has_gravity = true
  gravity_acceleration = 9.8065
  fluid_density_initial = 1050
  fluid_thermal_conductivity_initial = 0.71
  fluid_heat_capacity_initial = 4190
  fluid_viscosity_initial = 1.93e-04
  young_modulus = 33.5e+09
  #solid_bulk_modulus = 7.525e+10
  solid_density_initial = 2700.0
  # fluid_thermal_expansion = 0.0
  # solid_thermal_expansion = 0.0
  supg_uo = supg
  porosity_uo = porosity
  fluid_density_uo = fluid_density
  fluid_viscosity_uo = fluid_viscosity
  permeability_uo = permeability
  scaling_uo = scaling
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
  [../]
  [./temperature]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./KernelH]
    type = GolemKernelH
    variable = pore_pressure
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./KernelT]
    type = GolemKernelT
    variable = temperature
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./KernelTH]
    type = GolemKernelTH
    variable = temperature
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./KernelM_x]
    type = GolemKernelM
    variable = disp_x
    component = 0
    displacements = 'disp_x disp_y disp_z'
    block = 'u1 u2'
  [../]
  [./KernelM_y]
    type = GolemKernelM
    variable = disp_y
    component = 1
    displacements = 'disp_x disp_y disp_z'
    block = 'u1 u2'
  [../]
  [./KernelM_z]
    type = GolemKernelM
    variable = disp_z
    component = 2
    displacements = 'disp_x disp_y disp_z'
    block = 'u1 u2'
  [../]
[]

[AuxVariables]
  [./stress_xx]
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
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./stress_xx]
    type = GolemStress
    variable = stress_xx
    index_i = 0
    index_j = 0
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
  [./stress_yy]
    type = GolemStress
    variable = stress_yy
    index_i = 1
    index_j = 1
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
  [./stress_zz]
    type = GolemStress
    variable = stress_zz
    index_i = 2
    index_j = 2
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
  [./stress_xy]
    type = GolemStress
    variable = stress_xy
    index_i = 0
    index_j = 1
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
  [./stress_xz]
    type = GolemStress
    variable = stress_xz
    index_i = 0
    index_j = 2
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
  [./stress_yz]
    type = GolemStress
    variable = stress_yz
    index_i = 1
    index_j = 2
    # block = 'u1 u2'
    execute_on = timestep_end
  [../]
[]

[Functions]
  [./temperature_gradient]
    type = ParsedFunction
    value = a*(z-b)+c
    vars = 'a b c'
    vals = '-0.0319 -4200 150'
  [../]
  [./pressure_gradient]
    type = ParsedFunction
    value = a*b*(z-c)+d
    vars = 'a b c d'
    vals = '1.050e-3 -9.8065 0 0.1'
  [../]
  [./load_stress_zz]
    type = ParsedFunction
    value = a*b*(z-c)
    vars = 'a b c'
    vals = '2.670e-03 9.8065 26'
  [../]
  [./load_stress_xx]
    type = ParsedFunction
    value = b/a*(z-a)+b
    vars = 'a b'
    vals = '-4200 -139'
  [../]
  [./load_stress_yy]
    type = ParsedFunction
    value = b/a*(z-a)+b
    vars = 'a b'
    vals = '-4200 -82'
  [../]
[]

[ICs]
  [./temperature]
    type = FunctionIC
    variable = temperature
    function = temperature_gradient
  [../]
  [./pore_pressure_ic]
    type = FunctionIC
    variable = pore_pressure
    function = pressure_gradient
  [../]
[]

[BCs]
  [./T_top]
    type = FunctionDirichletBC
    variable = temperature
    boundary = 'left right back front'
    function = temperature_gradient
  [../]
  [./T_bottom]
    type = GolemHeatFlowBC
    variable = temperature
    boundary = bottom
    value = 0.072
  [../]
  [./pore_pressure_sides]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = 'left right back front'
    function = pressure_gradient
  [../]
  [./no_z]
    type = PresetBC
    variable = disp_z
    boundary = bottom
    value = 0.0
  [../]
  [./disp_x_east]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0
  [../]
  [./disp_x_west]
    type = PresetBC
    variable = disp_x
    boundary = right
    value = 0
  [../]
  [./disp_y_north]
    type = PresetBC
    variable = disp_y
    boundary = front
    value = 0.0
  [../]
  [./disp_y_south]
    type = PresetBC
    variable = disp_y
    boundary = back
    value = 0.0
  [../]
  [./push_z]
    type = PresetBC
    variable = disp_z
    boundary = top
    value = 1e-1
  [../]
[]

[Materials]
  [./unit_granite]
    type = GolemMaterialMElastic
    displacements = 'disp_x disp_y disp_z'
    block = 'u1 u2'
    porosity_initial = 0.005
    poisson_ratio = 0.2
    permeability_type = 'orthotropic'
    permeability_initial = '5e-17 5e-17 5e-17'
    solid_thermal_conductivity_initial = 3.0
    solid_heat_capacity_initial = 800
    fluid_modulus = 1.00e+09
  [../]
  [./major_faults]
    type = GolemMaterialMElastic
    displacements = 'disp_x disp_y disp_z'
    block = 'f1 f2'
    material_type = frac
    poisson_ratio = 0.2
    scaling_factor_initial = 0.01
    porosity_initial = 0.25
    permeability_type = isotropic
    permeability_initial = 0.7e-11
    solid_thermal_conductivity_initial = 3.0
    fluid_modulus = 2.50e+09
  [../]
  # [./well]
  #   type = GolemMaterialTH
  #   block = 'px1 px2'
  #   material_type = 'well'
  #   scaling_factor_initial = 0.10795
  #   porosity_initial = 1.0
  #   permeability_type = 'isotropic'
  #   permeability_initial = 1.45e-3
  #   solid_thermal_conductivity_initial = 0.0
  # [../]
[]

[UserObjects]
  [./scaling]
    type = GolemScaling
    characteristic_time = 1.0
    characteristic_length = 1.0
    characteristic_temperature = 1.0
    characteristic_stress = 1e6
  [../]
  [./supg]
    type = GolemSUPG
    effective_length = 'min'
    method = 'full'
  [../]
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
  active = 'fsp'
  [./lu]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew -ksp_diagonal_scale -ksp_diagonal_scale_fix'
    petsc_options_iname = '-snes_type
                           -snes_linesearch_type -snes_linesearch_maxstep -sneslinesearch_minlambda
                           -snes_atol -snes_rtol -snes_max_it
                           -pc_type pc_factor_mat_solver_package -pc_factor_shift_type -pc_factor_shift_amount
                           -ksp_type -ksp_atol -ksp_rtol -ksp_stol -ksp_max_it'
    petsc_options_value = 'newtonls
                           basic 2e12 1e-3
                           1e-3 1e-10 5000
                           lu mumps NONZERO 1e-12
                           bcgs 1e-12 1e-15 1e-8 500'
  [../]
  [./mg_cheb]
    type = SMP
    full = true
    petsc_options = '-ksp_diagonal_scale -ksp_diagonal_scale_fix'
    petsc_options_iname = '-snes_linesearch_type -snes_linesearch_maxstep -sneslinesearch_minlambda
                           -snes_atol -snes_rtol -snes_max_it
                           -ksp_type -ksp_rtol -ksp_max_it
                           -pc_type
                           -pc_mg_type
                           -pc_mg_galerkin
                           -pc_mg_levels_ksp_type -pc_pc_mg_levels_ksp_max_it -pc_mg_levels_ksp_norm_type
                           -pc_mg_levels_ksp_chebyshev_esteig -pc_mg_levels_esteig_ksp_norm_type
                           -pc_mg_levels_pc_type
                           -pc_mg_coarse_ksp_type -pc_mg_coarse_ksp_max_it -pc_mg_coarse_ksp_rtol
                           -pc_mg_coarse_pc_type -pc_mg_coarse_pc_asm_overlap -pc_mg_coarse_sub_pc_type'
    petsc_options_value = 'basic 2e12 1.0e-3
                           1.0e-03 1.0e-12 500
                           bcgs 1e-3 200
                           mg
                           full
                           mat
                           chebyshev 10 NONE
                           0,0.2,0,1.1 NONE
                           jacobi
                           bcgs 25 1.0e-4
                           asm 2 ilu'
  [../]
  [./fsp]
    type = FSP
    topsplit = 'HTM'
    [./HTM]
      splitting = 'H T M'
      splitting_type = multiplicative
      petsc_options = '-snes_ksp_ew
                       -snes_monitor
                       -snes_linesearch_monitor
                       -snes_converged_reason'
      petsc_options_iname = '-ksp_type
                             -ksp_rtol -ksp_max_it
                             -snes_type -snes_linesearch_type
                             -snes_atol -snes_stol -snes_max_it'
      petsc_options_value = 'fgmres
                             1.0e-12 50
                             newtonls basic
                             1 0 100'
    [../]
    [./H]
      vars = 'pore_pressure'
      petsc_options_iname = '-ksp_type
                             -pc_type -pc_hypre_type
                             -ksp_rtol -ksp_max_it'
      petsc_options_value = 'preonly
                             hypre boomeramg
                             1e-04 500'
    [../]
    [./T]
      vars = 'temperature'
      petsc_options_iname = '-ksp_type
                             -pc_type
                             -sub_pc_type -sub_pc_factor_levels
                             -ksp_rtol -ksp_max_it'
      petsc_options_value = 'fgmres
                             asm
                             ilu 1
                             1e-04 500'
    [../]
    [./M]
      vars = 'disp_x disp_y disp_z'
      petsc_options_iname = '-ksp_type
                             -pc_type
                             -sub_pc_type -sub_pc_factor_levels
                             -ksp_rtol -ksp_max_it'
      petsc_options_value = 'fgmres
                             asm
                             ilu 1
                             1e-04 500'
    [../]
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  start_time = 0.0
  end_time = 1.0
  [./TimeStepper]
    type = TimeSequenceStepper
    time_sequence = '0 1'
  [../]
[]

# [Debug]
#   show_var_residual_norms = true
# []

[Outputs]
  print_linear_residuals = true
  # print_perf_log = true
  [./out]
    type = Exodus
  [../]
  # [./csv]
  #   type = CSV
  # [../]
[]
