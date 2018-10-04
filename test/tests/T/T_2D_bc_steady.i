[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 50
  ny = 25
  nz = 1
  xmin = 0
  xmax = 2
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 0.01
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
  [./T_right_func]
    type = ParsedFunction
    value = 'T0*(2*y+2*L)/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_bottom_func]
    type = ParsedFunction
    value = 'T0*x/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_top_func]
    type = ParsedFunction
    value = 'T0*(x+2*L)/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
[]

[BCs]
  [./qf_left]
    type = NeumannBC
    variable = temperature
    boundary = left
    value = -1.0
  [../]
  [./T_bottom]
    type = FunctionPresetBC
    variable = temperature
    boundary = bottom
    function = T_bottom_func
  [../]
  [./T_right]
    type = FunctionPresetBC
    variable = temperature
    boundary = right
    function = T_right_func
  [../]
  [./T_top]
    type = FunctionPresetBC
    variable = temperature
    boundary = top
    function = T_top_func
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
  perf_graph = true
  exodus = true
[]
