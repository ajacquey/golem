[Mesh]
  file = GMS.e
[]

[Variables]
  [./temperature]
  [../]
[]

[GlobalParams]
  temperature = temperature
[]

[Functions]
  [./func_bc_0]
    type = GolemFunctionBCFromFile
    data_file = bc_file_0.dat
    interpolate_data_in_time = false
    interpolate_data_in_space = true
  [../]
[]

[BCs]
  [./bc_0]
    type = FunctionDirichletBC
    function = func_bc_0
    boundary = 1
    variable = temperature
  [../]
  [./bc_1]
    type = DirichletBC
    value = 250
    boundary = 2
    variable = temperature
  [../]
[]

[Kernels]
  [./temp_conduction]
    type = GMSEnergyResidual
    variable = temperature
  [../]
[]

[UserObjects]
  [./prop]
    type = GolemPropertyReadFile
    prop_file_name = GMS_prop.dat
    nprop = 4
    nele = 1071
  [../]
[]

[Materials]
 [./layer1]
   type = GMSMaterial
   block = 0
   read_prop_user_object = prop
   outputs = out
 [../]
 [./layer2]
   type = GMSMaterial
   block = 1
   read_prop_user_object = prop
   outputs = out
 [../]
 [./layer3]
   type = GMSMaterial
   block = 2
   read_prop_user_object = prop
   outputs = out
 [../]
[]

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
  type = Steady
  solve_type = 'NEWTON'
  automatic_scaling = true
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  [./out]
    type = Exodus
  [../]
[]
