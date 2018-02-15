[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 1
  nz = 1
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 0.1
  zmin = 0
  zmax = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  pore_pressure = pore_pressure
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
  [./roller_xmin]
    type = PresetBC
    variable = disp_x
    value = 0
    boundary = 'left'
  [../]
  [./roller_ymin]
    type = PresetBC
    variable = disp_y
    value = 0
    boundary = 'bottom'
  [../]
  [./plane_strain]
    type = PresetBC
    variable = disp_z
    value = 0
    boundary = 'back front'
  [../]
  [./xmax_drained]
    type = DirichletBC
    variable = pore_pressure
    value = 0
    boundary = right
  [../]
  [./top_velocity]
    type = FunctionPresetBC
    variable = disp_y
    function = top_velocity
    boundary = top
  [../]
[]

[Functions]
  [./top_velocity]
    type = PiecewiseLinear
    x = '0 0.002 0.006   0.014   0.03    0.046   0.062   0.078   0.094   0.11    0.126   0.142   0.158   0.174   0.19 0.206 0.222 0.238 0.254 0.27 0.286 0.302 0.318 0.334 0.35 0.366 0.382 0.398 0.414 0.43 0.446 0.462 0.478 0.494 0.51 0.526 0.542 0.558 0.574 0.59 0.606 0.622 0.638 0.654 0.67 0.686 0.702'
    y = '-0.041824842    -0.042730269    -0.043412712    -0.04428867     -0.045509181    -0.04645965     -0.047268246 -0.047974749      -0.048597109     -0.0491467  -0.049632388     -0.050061697      -0.050441198     -0.050776675     -0.051073238      -0.0513354 -0.051567152      -0.051772022     -0.051953128 -0.052113227 -0.052254754 -0.052379865 -0.052490464 -0.052588233 -0.052674662 -0.052751065 -0.052818606 -0.052878312 -0.052931093 -0.052977751 -0.053018997 -0.053055459 -0.053087691 -0.053116185 -0.053141373 -0.05316364 -0.053183324 -0.053200724 -0.053216106 -0.053229704 -0.053241725 -0.053252351 -0.053261745 -0.053270049 -0.053277389 -0.053283879 -0.053289615'
  [../]
[]

[AuxVariables]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./tot_force]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./stress_yy]
    type = GolemStress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./tot_force]
    type = ParsedAux
    args = 'stress_yy pore_pressure'
    execute_on = timestep_end
    variable = tot_force
    function = '-stress_yy+0.6*pore_pressure'
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
    lame_modulus = 0.5
    shear_modulus = 0.75
    permeability_initial = 1.5e-03
    fluid_viscosity_initial = 1.0e-03
    porosity_initial = 0.1
    solid_bulk_modulus = 2.5
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
    point = '0.0 0 0'
    variable = pore_pressure
  [../]
  [./p1]
    type = PointValue
    outputs = csv
    point = '0.1 0 0'
    variable = pore_pressure
  [../]
  [./p2]
    type = PointValue
    outputs = csv
    point = '0.2 0 0'
    variable = pore_pressure
  [../]
  [./p3]
    type = PointValue
    outputs = csv
    point = '0.3 0 0'
    variable = pore_pressure
  [../]
  [./p4]
    type = PointValue
    outputs = csv
    point = '0.4 0 0'
    variable = pore_pressure
  [../]
  [./p5]
    type = PointValue
    outputs = csv
    point = '0.5 0 0'
    variable = pore_pressure
  [../]
  [./p6]
    type = PointValue
    outputs = csv
    point = '0.6 0 0'
    variable = pore_pressure
  [../]
  [./p7]
    type = PointValue
    outputs = csv
    point = '0.7 0 0'
    variable = pore_pressure
  [../]
  [./p8]
    type = PointValue
    outputs = csv
    point = '0.8 0 0'
    variable = pore_pressure
  [../]
  [./p9]
    type = PointValue
    outputs = csv
    point = '0.9 0 0'
    variable = pore_pressure
  [../]
  [./p99]
    type = PointValue
    outputs = csv
    point = '1 0 0'
    variable = pore_pressure
  [../]
  [./xdisp]
    type = PointValue
    outputs = csv
    point = '1 0.1 0'
    variable = disp_x
  [../]
  [./ydisp]
    type = PointValue
    outputs = csv
    point = '1 0.1 0'
    variable = disp_y
  [../]
  [./total_downwards_force]
     type = ElementAverageValue
     outputs = csv
     variable = tot_force
  [../]
  [./dt]
    type = FunctionValuePostprocessor
    outputs = console
    function = if(0.15*t<0.01,0.15*t,0.01)
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
  end_time = 0.7
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
    dt = 0.001
  [../]
[]

[Outputs]
  execute_on = 'timestep_end'
  [./csv]
    interval = 3
    type = CSV
  [../]
[]
