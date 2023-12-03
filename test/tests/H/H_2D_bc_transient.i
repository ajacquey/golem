[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 100
  ny = 100
  nz = 1
  xmin = 0
  xmax = 100
  ymin = 0
  ymax = 100
[]

[Variables]
  [pore_pressure]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [H_time]
    type = GolemKernelTimeH
    variable = pore_pressure
  []
  [HKernel]
    type = GolemKernelH
    variable = pore_pressure
  []
[]

[Functions]
  [IC_x_func]
    type = PiecewiseMultilinear
    data_file = IC_x.txt
  []
  [IC_y_func]
    type = PiecewiseMultilinear
    data_file = IC_y.txt
  []
  [p0]
    type = ConstantFunction
    value = 1.0e+06
  []
  [IC_func]
    type = CompositeFunction
    functions = 'p0 IC_x_func IC_y_func'
  []
[]

[ICs]
  [IC_p]
    type = FunctionIC
    variable = pore_pressure
    function = IC_func
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
  [p0_right]
    type = DirichletBC
    variable = pore_pressure
    boundary = right
    value = 0.0
    preset = true
  []
[]

[Materials]
  [hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-14
    fluid_viscosity_initial = 1.728e-03
    porosity_initial = 1.0
    fluid_modulus = 5.0e+09
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
  type = Transient
  solve_type = 'NEWTON'
  automatic_scaling = true
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
