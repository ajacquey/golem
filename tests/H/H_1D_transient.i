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
  [./pore_pressure]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
[]

[Kernels]
  [./H_time]
    type = GolemKernelTimeH
    variable = pore_pressure
  [../]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
[]

[Functions]
  [./p_func]
    type = ParsedFunction
    value = 'p1*t'
    vars = 'p1'
    vals = '23.14814815'
  [../]
[]

[BCs]
  [./p_left]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = left
    function = p_func
  [../]
  [./p_right]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = right
    function = p_func
  [../]
[]

[Materials]
  [./hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-14
    fluid_viscosity_initial = 1.728e-03
    porosity_initial = 1.0
    fluid_modulus = 4.0e+09
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
    petsc_options_value = 'gmres asm 1E-15 1E-10 200 500 lu NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  start_time = 0.0
  end_time = 43200
  dt = 4320
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
