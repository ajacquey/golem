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
  active = 'mine'
  [./hypre]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew -snes_view -ksp_converged_reason'
    petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'hypre boomeramg cp 201 1E1 1E-06 500'
  [../]
  [./mine]
    type = SMP
    full = true
    petsc_options = '-snes_ksp_ew -snes_monitor -snes_linesearch_monitor -snes_converged_reason -ksp_converged_reason -ksp_monitor_short'
    petsc_options_iname = '-ksp_type
                           -pc_type -sub_pc_type -sub_pc_factor_levels
                           -ksp_rtol -ksp_max_it
                           -snes_type -snes_linesearch_type
                           -snes_atol -snes_max_it' #-snes_atol -snes_stol -snes_max_it'
    petsc_options_value = 'fgmres asm ilu 1 1.0e-12 500 newtonls cp 1.0e-02 1000'
  [../]
[]

[Executioner]
  type = Steady
  solve_type = Newton
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  [./out]
    type = Exodus
  [../]
[]
