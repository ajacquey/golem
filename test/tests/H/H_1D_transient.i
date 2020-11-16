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
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = left
    function = p_func
    preset = true
  [../]
  [./p_right]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = right
    function = p_func
    preset = true
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
  end_time = 43200
  dt = 4320
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
