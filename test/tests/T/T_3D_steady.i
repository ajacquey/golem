[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 5
  ny = 5
  nz = 6
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
[]

[Variables]
  [./temperature]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./TKernel]
    type = GolemKernelT
    variable = temperature
  [../]
[]

[Functions]
  [./T_left_func]
    type = ParsedFunction
    value = 'T0*(0+y/L+z/L)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_right_func]
    type = ParsedFunction
    value = 'T0*(1+y/L+z/L)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_bottom_func]
    type = ParsedFunction
    value = 'T0*(x/L+0+z/L)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_top_func]
    type = ParsedFunction
    value = 'T0*(x/L+1+z/L)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_back_func]
    type = ParsedFunction
    value = 'T0*(x/L+y/L+0)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_front_func]
    type = ParsedFunction
    value = 'T0*(x/L+y/L+1)'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
[]

[BCs]
  [./T_left]
    type = FunctionDirichletBC
    variable = temperature
    boundary = left
    function = T_left_func
    preset = true
  [../]
  [./T_right]
    type = FunctionDirichletBC
    variable = temperature
    boundary = right
    function = T_right_func
    preset = true
  [../]
  [./T_bottom]
    type = FunctionDirichletBC
    variable = temperature
    boundary = bottom
    function = T_bottom_func
    preset = true
  [../]
  [./T_top]
    type = FunctionDirichletBC
    variable = temperature
    boundary = top
    function = T_top_func
    preset = true
  [../]
  [./T_front]
    type = FunctionDirichletBC
    variable = temperature
    boundary = front
    function = T_front_func
    preset = true
  [../]
  [./T_back]
    type = FunctionDirichletBC
    variable = temperature
    boundary = back
    function = T_back_func
    preset = true
  [../]
[]

[Materials]
  [./thermal]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 1.0
    porosity_uo = porosity
  [../]
[]

[UserObjects]
  [./porosity]
    type = GolemPorosityConstant
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
  type = Steady
  solve_type = 'NEWTON'
  automatic_scaling = true
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
