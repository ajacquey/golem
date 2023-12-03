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
 [pore_pressure]
   order = FIRST
   family = LAGRANGE
 []
[]

[Kernels]
  [HKernel]
    type = GolemKernelH
    variable = pore_pressure
  []
[]

[DiracKernels]
  [H_injection]
    type = GolemDiracKernelTH
    variable = pore_pressure
    source_point = '0.0 0.0 0.0'
    in_out_rate = 1.0
    start_time = 0.0
    end_time = 10.0
  []
[]

[BCs]
  [p0_outer]
    type = DirichletBC
    variable = pore_pressure
    boundary = 'left right bottom top front back'
    value = 0.0
    preset = true
  []
[]

[Materials]
  [hydro]
    type = GolemMaterialH
    block = 0
    permeability_initial = 1.0e-15
    fluid_viscosity_initial = 1.0e-03
    fluid_density_initial = 1.0e+03
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

#[VectorPostprocessors]
#  [line_pf]
#    type = LineValueSampler
#    variable = pore_pressure
#    start_point = '0.0 0.0 0.0'
#    end_point = '5000 5000 5000'
#    num_points = 10
#    sort_by = x
#    outputs = csv
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
  end_time = 1.0
  dt = 1.0
[]

[Outputs]
  execute_on = 'timestep_end'
  print_linear_residuals = true
  perf_graph = true
  exodus = true
  #csv = true
[]