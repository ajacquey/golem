[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 100
  ny = 100
  nz = 1
  xmin = 0
  xmax = 100
  ymin = 0
  ymax = 100
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
    type = PresetBC
    variable = temperature
    boundary = left
    value = 0.0
  [../]
  [./T0_right1]
    type = PresetBC
    variable = temperature
    boundary = right
    value = 0.0
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
