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
    type = FunctionPresetBC
    variable = temperature
    boundary = left
    function = T_left_func
  [../]
  [./T_right]
    type = FunctionPresetBC
    variable = temperature
    boundary = right
    function = T_right_func
  [../]
  [./T_bottom]
    type = FunctionPresetBC
    variable = temperature
    boundary = bottom
    function = T_bottom_func
  [../]
  [./T_top]
    type = FunctionPresetBC
    variable = temperature
    boundary = top
    function = T_top_func
  [../]
  [./T_front]
    type = FunctionPresetBC
    variable = temperature
    boundary = front
    function = T_front_func
  [../]
  [./T_back]
    type = FunctionPresetBC
    variable = temperature
    boundary = back
    function = T_back_func
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
  [./precond]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
  [../]
[]

[Executioner]
  type = Steady
  solve_type = Newton
[]

[Outputs]
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
