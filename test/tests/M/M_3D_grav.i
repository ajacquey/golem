[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 10
  xmin = 0
  xmax = 6
  ymin = 0
  ymax = 6
  zmin = 0
  zmax = 30
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  [./MKernel_x]
    type = GolemKernelM
    variable = disp_x
    component = 0
  [../]
  [./MKernel_y]
    type = GolemKernelM
    variable = disp_y
    component = 1
  [../]
  [./MKernel_z]
    type = GolemKernelM
    variable = disp_z
    component = 2
  [../]
[]

[AuxVariables]
  [./strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  #[./strain_zz_analytical]
  #  order = CONSTANT
  #  family = MONOMIAL
  #[../]
  #[./stress_zz_analytical]
  #  order = CONSTANT
  #  family = MONOMIAL
  #[../]
[]

[AuxKernels]
  [./strain_zz]
    type = GolemStrain
    variable = strain_zz
    index_i = 2
    index_j = 2
  [../]
  [./stress_zz]
    type = GolemStress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
  #[./strain_zz_analytical]
  #  type = FunctionAux
  #  variable = strain_zz_analytical
  #  function = strain_zz_analytical
  #[../]
  #[./stress_zz_analytical]
  #  type = FunctionAux
  #  variable = stress_zz_analytical
  #  function = stress_zz_analytical
  #[../]
[]

#[Functions]
#  [./disp_z_analytical]
#    type = ParsedFunction
#    value = '1/E*(1-2*nu*nu/(1-nu))*rho*(-g)*(H*z-1/2*z*z)'
#    vars = 'E nu rho g H'
#    vals = '10.0e+09 0.25 3058.104 9.81 30'
#  [../]
#  [./strain_zz_analytical]
#    type = ParsedFunction
#    value = '1/E*(1-2*nu*nu/(1-nu))*rho*(-g)*(H-z)'
#    vars = 'E nu rho g H'
#    vals = '10.0e+09 0.25 3058.104 9.81 30'
#  [../]
#  [./stress_zz_analytical]
#    type = ParsedFunction
#    value = 'rho*(-g)*(H-z)'
#    vars = 'rho g H'
#    vals = '3058.104 9.81 30'
#  [../]
#[]

[BCs]
  [./no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0.0
    preset = true
  [../]
  [./no_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
    preset = true
  [../]
  [./no_z]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
    preset = true
  [../]
[]

[Materials]
  [./MMaterial]
    type = GolemMaterialMElastic
    block = 0
    has_gravity = true
    solid_density_initial = 3058.104
    gravity_acceleration = 9.81
    strain_model = small_strain
    young_modulus = 10.0e+09
    poisson_ratio = 0.25
    porosity_uo = porosity
    fluid_density_uo = fluid_density
  [../]
[]

[UserObjects]
  [./porosity]
    type = GolemPorosityConstant
  [../]
  [./fluid_density]
    type = GolemFluidDensityConstant
  [../]
[]

#[Postprocessors]
#  [./error_uz]
#    type = NodalL2Error
#    variable = disp_z
#    function = disp_z_analytical
#  [../]
#  [./error_ezz]
#    type = ElementL2Difference
#    variable = strain_zz
#    other_variable = strain_zz_analytical
#  [../]
#  [./error_szz]
#    type = ElementL2Difference
#    variable = stress_zz
#    other_variable = stress_zz_analytical
#  [../]
#[]

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
  end_time = 1.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]
