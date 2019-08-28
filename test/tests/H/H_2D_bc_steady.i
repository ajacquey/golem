[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 50
  ny = 25
  nz = 1
  xmin = 0
  xmax = 2
  ymin = 0
  ymax = 1
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
  [./p_right_func]
    type = ParsedFunction
    value = 'p0/L*(2*L+2*y)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_bottom_func]
    type = ParsedFunction
    value = 'p0*x/L'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
  [./p_top_func]
    type = ParsedFunction
    value = 'p0/L*(x+2*L)'
    vars = 'p0 L'
    vals = '1.0e+06 1.0'
  [../]
[]

[BCs]
  [./q_left]
    type = NeumannBC
    variable = pore_pressure
    boundary = left
    value = -1.0e-05
  [../]
  [./p_bottom]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = bottom
    function = p_bottom_func
  [../]
  [./p_right]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = right
    function = p_right_func
  [../]
  [./p_top]
    type = FunctionPresetBC
    variable = pore_pressure
    boundary = top
    function = p_top_func
  [../]
[]

[Materials]
  [./hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-14
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
