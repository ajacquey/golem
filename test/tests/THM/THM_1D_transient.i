[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 100
  ny = 1
  nz = 1
  xmin = 0
  xmax = 10
  ymin = 0
  ymax = 1
  zmin = 0
  zmax = 1
[]

[GlobalParams]
  pore_pressure = pore_pressure
  temperature = temperature
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [pore_pressure]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  []
  [temperature]
    order = FIRST
    family = LAGRANGE
   initial_condition = 0.0
  []
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
  [HKernel]
    type = GolemKernelH
    variable = pore_pressure
  []
  [TKernelTime]
    type = GolemKernelTimeT
    variable = temperature
  []
  [TKernel]
    type = GolemKernelTH
    variable = temperature
    is_conservative = true
  []
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
  [strain_zz]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_zz]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [strain_zz]
    type = GolemStrain
    variable = strain_zz
    index_i = 2
    index_j = 2
  []
  [stress_zz]
    type = GolemStress
    variable = stress_zz
    index_i = 2
    index_j = 2
  []
[]

[BCs]
  [pf_left]
    type = DirichletBC
    variable = pore_pressure
    boundary = 'left'
    value = 1.0e+05
    preset = false
  []
  [pf_right]
    type = DirichletBC
    variable = pore_pressure
    boundary = 'right'
    value = 0.0
    preset = false
  []
  [T_left]
    type = DirichletBC
    variable = temperature
    boundary = 'left'
    value = -10
    preset = false
  []
  [T_right]
    type = GolemConvectiveTHBC
    variable = temperature
    boundary = 'right'
  []
  [no_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left'
    value = 0.0
    preset = true
  []
  [no_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom top'
    value = 0.0
    preset = true
  []
  [no_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'back front'
    value = 0.0
    preset = true
  []
[]

[Materials]
  [MMaterial]
    type = GolemMaterialMElastic
    block = 0
    solid_density_initial = 2000
    fluid_density_initial = 1000
    strain_model = incr_small_strain
    young_modulus = 5.0e+09
    poisson_ratio = 0.25
    porosity_initial = 0.1
    permeability_initial = 1.0e-11
    fluid_viscosity_initial = 1.0e-03
    solid_thermal_expansion = 1.0e-06
    fluid_thermal_expansion = 1.0e-06
    fluid_thermal_conductivity_initial = 10
    solid_thermal_conductivity_initial = 50
    fluid_heat_capacity_initial = 1100
    solid_heat_capacity_initial = 250
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
  end_time = 20000
  dt = 2000
  num_steps = 10
[]

[Outputs]
  interval = 5
  print_linear_residuals = true
  perf_graph = true
  exodus = true
[]