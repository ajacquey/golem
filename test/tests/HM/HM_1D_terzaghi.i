[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 10
  xmin = -1
  xmax = 1
  ymin = -1
  ymax = 1
  zmin = 0
  zmax = 10
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  pore_pressure = pore_pressure
  block = 0
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./pore_pressure]
  [../]
[]

[BCs]
  [./confinex]
    type = DirichletBC
    variable = disp_x
    value = 0
    boundary = 'left right'
    preset = true
  [../]
  [./confiney]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = 'bottom top'
    preset = true
  [../]
  [./basefixed]
    type = DirichletBC
    variable = disp_z
    value = 0
    boundary = back
    preset = true
  [../]
  [./topdrained]
    type = DirichletBC
    variable = pore_pressure
    value = 0
    boundary = front
    preset = false
  [../]
  [./topload]
    type = NeumannBC
    variable = disp_z
    value = -1
    boundary = front
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
  [./p_time]
    type = GolemKernelTimeH
    variable = pore_pressure
  [../]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
  [./HMKernel]
    type = GolemKernelHPoroElastic
    variable = pore_pressure
  [../]
[]

[Materials]
  [./HMMaterial]
    type = GolemMaterialMElastic
    block = 0
    strain_model = incr_small_strain
    lame_modulus = 2
    shear_modulus = 3
    permeability_initial = 1.5
    fluid_viscosity_initial = 1.0
    porosity_initial = 0.1
    solid_bulk_modulus = 10
    fluid_modulus = 8
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

[Postprocessors]
  [./p0]
    type = PointValue
    outputs = csv
    point = '0 0 0'
    variable = pore_pressure
  [../]
  [./p1]
    type = PointValue
    outputs = csv
    point = '0 0 1'
    variable = pore_pressure
  [../]
  [./p2]
    type = PointValue
    outputs = csv
    point = '0 0 2'
    variable = pore_pressure
  [../]
  [./p3]
    type = PointValue
    outputs = csv
    point = '0 0 3'
    variable = pore_pressure
  [../]
  [./p4]
    type = PointValue
    outputs = csv
    point = '0 0 4'
    variable = pore_pressure
  [../]
  [./p5]
    type = PointValue
    outputs = csv
    point = '0 0 5'
    variable = pore_pressure
  [../]
  [./p6]
    type = PointValue
    outputs = csv
    point = '0 0 6'
    variable = pore_pressure
  [../]
  [./p7]
    type = PointValue
    outputs = csv
    point = '0 0 7'
    variable = pore_pressure
  [../]
  [./p8]
    type = PointValue
    outputs = csv
    point = '0 0 8'
    variable = pore_pressure
  [../]
  [./p9]
    type = PointValue
    outputs = csv
    point = '0 0 9'
    variable = pore_pressure
  [../]
  [./p99]
    type = PointValue
    outputs = csv
    point = '0 0 10'
    variable = pore_pressure
  [../]
  [./zdisp]
    type = PointValue
    outputs = csv
    point = '0 0 10'
    variable = disp_z
  [../]
  [./dt]
    type = FunctionValuePostprocessor
    outputs = console
    function = if(0.5*t<0.1,0.5*t,0.1)
  [../]
[]


[Preconditioning]
  [./andy]
    type = SMP
    full = true
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  start_time = 0
  end_time = 10
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 0.0001
  [../]
[]

[Outputs]
  execute_on = 'timestep_end'
  [./csv]
    type = CSV
  [../]
[]
