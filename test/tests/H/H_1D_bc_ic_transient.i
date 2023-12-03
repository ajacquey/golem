[Mesh]
  type = FileMesh
  file = H_1D_bc_ic_transient.msh
  boundary_id = '0 1 2 3 4 5 6 7'
  boundary_name = 'left1 left2 right1 right2 bottom top front back'
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
  [p0]
    type = ConstantFunction
    value = 1.0e+06
  []
  [IC_func]
    type = CompositeFunction
    functions = 'p0 IC_x_func'
  []
[]

[ICs]
  [p_IC]
    type = FunctionIC
    variable = pore_pressure
    function = 'IC_func'
  []
[]

[BCs]
  [p0_left]
    type = DirichletBC
    variable = pore_pressure
    boundary = left1
    value = 0.0
    preset = true
  []
  [p0_right]
    type = DirichletBC
    variable = pore_pressure
    boundary = right1
    value = 0.0
    preset = true
  []
[]

[Materials]
  [hydro_1]
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
  [hydro_2]
    type = GolemMaterialH
    block = 1
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
  end_time = 8640
  dt = 864
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
