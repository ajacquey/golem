[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 200
  ny = 1
  nz = 1
  xmin = -50
  xmax = 50
  ymin = 0
  ymax = 0.5
  zmin = 0
  zmax = 0.5
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
  [./T_time_func]
    type = ParsedFunction
    value = 'T1*t'
    vars = 'T1'
    vals = '0.000023148'
  [../]
[]

[BCs]
  [./T_left]
    type = FunctionPresetBC
    variable = temperature
    boundary = left
    function = T_time_func
  [../]
  [./T_right]
    type = FunctionPresetBC
    variable = temperature
    boundary = right
    function = T_time_func
  [../]
[]

[Materials]
  [./thermal]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 0.5787037
    solid_heat_capacity_initial = 0.01
    solid_density_initial = 2500
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
  end_time = 43200
  dt = 4320
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
