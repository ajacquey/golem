[Mesh]
  type = FileMesh
  file = T_1D_bc_transient.msh
  boundary_id = '0 1 2 3 4 5'
  boundary_name = 'left right bottom top front back'
[]

[Variables]
  [./temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
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
[]

[Functions]
  [./qf_right_func]
    type = ParsedFunction
    value = 'q1*t'
    vars = 'q1'
    vals = '4.465e-06'
  [../]
[]

[BCs]
  [./qf_right]
    type = FunctionNeumannBC
    variable = temperature
    boundary = right
    function = qf_right_func
  [../]
[]

[Materials]
  [./thermal_left]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.1574074
    solid_heat_capacity_initial = 0.01
    solid_density_initial = 2000
    porosity_uo = porosity
    fluid_density_uo = fluid_density
  [../]
  [./thermal_right]
    type = GolemMaterialT
    block = 1
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.1574074
    solid_heat_capacity_initial = 0.02
    solid_density_initial = 2000
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
  end_time = 7776
  dt = 777.6
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
