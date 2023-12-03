[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 8
  ny = 8
  nz = 2
  xmin = 0
  xmax = 10
  ymin = 0
  ymax = 10
  zmin = 0
  zmax = 2.5
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [disp_x]
    order = FIRST
    family = LAGRANGE
  []
  [disp_y]
    order = FIRST
    family = LAGRANGE
  []
  [disp_z]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [MKernel_x]
    type = GolemKernelM
    variable = disp_x
    component = 0
  []
  [MKernel_y]
    type = GolemKernelM
    variable = disp_y
    component = 1
  []
  [MKernel_z]
    type = GolemKernelM
    variable = disp_z
    component = 2
  []
[]

[AuxVariables]
  [strain_xy]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_xy]
    order = CONSTANT
    family = MONOMIAL
  []
  #[strain_xy_analytical]
  #  order = CONSTANT
  #  family = MONOMIAL
  #[]
  #[stress_xy_analytical]
  #  order = CONSTANT
  #  family = MONOMIAL
  #[]
[]

[AuxKernels]
  [strain_xy]
    type = GolemStrain
    variable = strain_xy
    index_i = 0
    index_j = 1
  []
  [stress_xy]
    type = GolemStress
    variable = stress_xy
    index_i = 0
    index_j = 1
  []
  #[strain_xy_analytical]
  #  type = FunctionAux
  #  variable = strain_xy_analytical
  #  function = strain_xy_analytical
  #[]
  #[stress_xy_analytical]
  #  type = FunctionAux
  #  variable = stress_xy_analytical
  #  function = stress_xy_analytical
  #[]
[]

[Functions]
  [disp_y_func]
    type = ParsedFunction
    expression = 'm*t*x'
    symbol_names = 'm'
    symbol_values = '-0.1'
  []
  #[strain_xy_analytical]
  #  type = ParsedFunction
  #  value = 'm*t/2'
  #  vars = 'm'
  #  vals = '-0.1'
  #[]
  #[stress_xy_analytical]
  #  type = ParsedFunction
  #  value = 'E/(2*(1+nu))*m*t'
  #  vars = 'm E nu'
  #  vals = '-0.1 10.0e+09 0.25'
  #[]
[]

[BCs]
  [no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right bottom top front back'
    value = 0.0
    preset = true
  []
  [no_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'left right bottom top front back'
    value = 0.0
    preset = true
  []
  [disp_y_plate]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'left right bottom top front back'
    function = disp_y_func
    preset = true
  []
[]

[Materials]
  [MMaterial]
    type = GolemMaterialMElastic
    block = 0
    strain_model = small_strain
    young_modulus = 10.0e+09
    poisson_ratio = 0.25
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

#[Postprocessors]
#  [error_exy]
#    type = ElementL2Difference
#    variable = strain_xy
#    other_variable = strain_xy_analytical
#  []
#  [error_sxy]
#    type = ElementL2Difference
#    variable = stress_xy
#    other_variable = stress_xy_analytical
#  []
#[]

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
  end_time = 2.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
