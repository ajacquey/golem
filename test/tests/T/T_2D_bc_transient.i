[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 100
  ny = 100
  nz = 1
  xmin = 0
  xmax = 100
  ymin = 0
  ymax = 100
[]

[Variables]
  [./temperature]
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
[]

[Functions]
  [./IC_x_func]
    type = PiecewiseMultilinear
    data_file = IC_x.txt
  [../]
  [./IC_y_func]
    type = PiecewiseMultilinear
    data_file = IC_y.txt
  [../]
  [./IC_func]
    type = CompositeFunction
    functions = 'IC_x_func IC_y_func'
  [../]
[]

[ICs]
  [./IC_temp]
    type = FunctionIC
    variable = temperature
    function = IC_func
  [../]
[]

[BCs]
  [./T0_left1]
    type = DirichletBC
    variable = temperature
    boundary = left
    value = 0.0
    preset = true
  [../]
  [./T0_right1]
    type = DirichletBC
    variable = temperature
    boundary = right
    value = 0.0
    preset = true
  [../]
[]

[Materials]
  [./thermal]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 0.5787037
    solid_heat_capacity_initial = 0.01
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
  end_time = 3456
  dt = 345.6
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
