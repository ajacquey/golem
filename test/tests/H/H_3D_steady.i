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
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
[]

[Functions]
  [./p_left_func]
    type = ParsedFunction
    value = 'p0*(0+y/L+z/L)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_right_func]
    type = ParsedFunction
    value = 'p0*(1+y/L+z/L)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_bottom_func]
    type = ParsedFunction
    value = 'p0*(x/L+0+z/L)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_top_func]
    type = ParsedFunction
    value = 'p0*(x/L+1+z/L)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_back_func]
    type = ParsedFunction
    value = 'p0*(x/L+y/L+0)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_front_func]
    type = ParsedFunction
    value = 'p0*(x/L+y/L+1)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
[]

[BCs]
  [./p_left]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = left
    function = p_left_func
  [../]
  [./p_right]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = right
    function = p_right_func
  [../]
  [./p_bottom]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = bottom
    function = p_bottom_func
  [../]
  [./p_top]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = top
    function = p_top_func
  [../]
  [./p_back]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = back
    function = p_back_func
  [../]
  [./p_front]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = front
    function = p_front_func
  [../]
[]

[Materials]
  [./hydro]
    type = GolemMaterialH
    block = 0
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
  type = Steady
  solve_type = Newton
[]

[Outputs]
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
