[Mesh]
  type = FileMesh
  file = T_1D_bc_steady.msh
  boundary_id = '0 1 2 3 4 5'
  boundary_name = 'left right bottom top front back'
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [TKernel]
    type = GolemKernelT
    variable = temperature
  []
[]

[BCs]
  [T0_left]
    type = DirichletBC
    variable = temperature
    boundary = left
    value = 1.0
    preset = true
  []
  [qf_right]
    type = NeumannBC
    variable = temperature
    boundary = right
    value = 1.5
  []
[]

[Materials]
  [thermal_left]
    type = GolemMaterialT
    block = 0
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 100.0
    porosity_uo = porosity
  []
  [thermal_right]
    type = GolemMaterialT
    block = 1
    fluid_thermal_conductivity_initial = 1.0
    solid_thermal_conductivity_initial = 300.0
    porosity_uo = porosity
  []
[]

[UserObjects]
  [porosity]
    type = GolemPorosityConstant
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
  type = Steady
  solve_type = 'NEWTON'
  automatic_scaling = true
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
