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
  [temperature]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  []
[]

[Kernels]
  [T_time]
    type = GolemKernelTimeT
    variable = temperature
  []
  [TKernel]
    type = GolemKernelT
    variable = temperature
  []
[]

[Functions]
  [T_time_func]
    type = ParsedFunction
    expression = 'T1*t'
    symbol_names = 'T1'
    symbol_values = '0.000023148'
  []
[]

[BCs]
  [T_left]
    type = FunctionDirichletBC
    variable = temperature
    boundary = left
    function = T_time_func
    preset = true
  []
  [T_right]
    type = FunctionDirichletBC
    variable = temperature
    boundary = right
    function = T_time_func
    preset = true
  []
[]

[Materials]
  [thermal]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 0.5787037
    solid_heat_capacity_initial = 0.01
    solid_density_initial = 2500
    porosity_uo = porosity
    fluid_density_uo = fluid_density
  []
[]

[UserObjects]
  [porosity]
    type = GolemPorosityConstant
  []
  [fluid_density]
    type = GolemFluidDensityConstant
  []
[]

[Preconditioning]
  [hypre]
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
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  automatic_scaling = true
  start_time = 0.0
  end_time = 43200
  dt = 4320
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
