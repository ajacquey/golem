[Mesh]
  type = FileMesh
  file = H_1D_bc_steady.msh
  boundary_id = '0 1 2 3 4 5'
  boundary_name = 'left right bottom top front back'
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
  [./q_func]
    type = ParsedFunction
    value = 'q1*t'
    vars = 'q1'
    vals = '4.46530093e-11'
  [../]
[]

[BCs]
  [./q_right]
    type = FunctionNeumannBC
    variable = pore_pressure
    boundary = right
    function = q_func
  [../]
[]

[Materials]
  [./hydro_1]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-14
    fluid_viscosity_initial = 0.864e-03
    porosity_initial = 1.0
    fluid_modulus = 5.0e+09
    porosity_uo = porosity
    fluid_density_uo = fluid_density
    fluid_viscosity_uo = fluid_viscosity
    permeability_uo = permeability
  [../]
  [./hydro_2]
    type = GolemMaterialH
    block = 1
    permeability_initial = 1.0e-14
    fluid_viscosity_initial = 0.864e-03
    porosity_initial = 1.0
    fluid_modulus = 2.5e+09
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
  end_time = 7776
  dt = 777.6
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  print_perf_log = true
  exodus = true
[]
