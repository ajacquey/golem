[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
  xmin = -5000
  xmax = 5000
  ymin = -5000
  ymax = 5000
  zmin = -5000
  zmax = 5000
[]

[Variables]
 [./pore_pressure]
   order = FIRST
   family = LAGRANGE
 [../]
[]

[Kernels]
  [./HKernel]
    type = GolemKernelH
    variable = pore_pressure
  [../]
[]

[DiracKernels]
  [./H_injection]
    type = GolemDiracKernelTH
    variable = pore_pressure
    source_point = '0.0 0.0 0.0'
    in_out_rate = 1.0
    start_time = 0.0
    end_time = 10.0
  [../]
[]

[BCs]
  [./p0_outer]
    type = PresetBC
    variable = pore_pressure
    boundary = 'left right bottom top front back'
    value = 0.0
  [../]
[]

[Materials]
  [./hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-15
    fluid_viscosity_initial = 1.0e-03
    fluid_density_initial = 1.0e+03
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

#[VectorPostprocessors]
#  [./line_pf]
#    type = LineValueSampler
#    variable = pore_pressure
#    start_point = '0.0 0.0 0.0'
#    end_point = '5000 5000 5000'
#    num_points = 10
#    sort_by = x
#    outputs = csv
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
  print_perf_log = true
  exodus = true
  #csv = true
[]