[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 20
  ny = 2
  nz = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 0.1
  zmin = 0
  zmax = 0.1
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

#[Functions]
#  [./disp_x_analytical]
#    type = ParsedFunction
#    value = 'S0/E*x'
#    vars = 'S0 E'
#    vals = '2.0e+06 10.0e+09'
#  [../]
#  [./disp_y_analytical]
#    type = ParsedFunction
#    value = '-nu*S0/E*y'
#    vars = 'S0 E nu'
#    vals = '2.0e+06 10.0e+09 0.25'
#  [../]
#  [./disp_z_analytical]
#    type = ParsedFunction
#    value = '-nu*S0/E*z'
#    vars = 'S0 E nu'
#    vals = '2.0e+06 10.0e+09 0.25'
#  [../]
#[]

[BCs]
  [./no_x_left]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./load_x_right]
    type = NeumannBC
    variable = disp_x
    boundary = right
    value = 2.0e+06
  [../]
  [./no_y_bottom]
    type = PresetBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./no_z_back]
    type = PresetBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
[]

[Materials]
  [./MMaterial]
    type = GolemMaterialMElastic
    block = 0
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
#  [./error_ux]
#    type = NodalL2Error
#    variable = disp_x
#    function = disp_x_analytical
#  [../]
#  [./error_uy]
#    type = NodalL2Error
#    variable = disp_y
#    function = disp_y_analytical
#  [../]
#  [./error_uz]
#    type = NodalL2Error
#    variable = disp_z
#    function = disp_z_analytical
#  [../]
#[]

[Preconditioning]
  [./precond]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
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
