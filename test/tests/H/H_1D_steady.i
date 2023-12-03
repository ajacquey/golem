[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 1
  nz = 1
  xmin = 0
  xmax = 100
  ymin = 0
  ymax = 10
  zmin = 0
  zmax = 10
[]

[Variables]
  [pore_pressure]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [HKernel]
    type = GolemKernelH
    variable = pore_pressure
  []
[]

[BCs]
  [p0_left]
    type = DirichletBC
    variable = pore_pressure
    boundary = left
    value = 0.0
    preset = true
  []
  [p1_right]
    type = DirichletBC
    variable = pore_pressure
    boundary = right
    value = 1.0e+06
    preset = true
  []
[]

[Materials]
  [hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-15
    fluid_viscosity_initial = 1.0e-03
    porosity_uo = porosity
    fluid_density_uo = fluid_density
    fluid_viscosity_uo = fluid_viscosity
    permeability_uo = permeability
  []
[]

[UserObjects]
  [porosity]
    type = GolemPorosityConstant
  []
  [fluid_density]
    type = GolemFluidDensityConstant
  []
  [fluid_viscosity]
    type = GolemFluidViscosityConstant
  []
  [permeability]
    type = GolemPermeabilityConstant
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
