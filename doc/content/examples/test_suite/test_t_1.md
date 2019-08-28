#  Test T 1

## Description

1D Steady-State Temperature distribution, boundary condition of 1st kind

The temperature distribution along a 100m beam is investigated. The Laplace equation describes the analytical solution: A steady-state temperature distribution.

$\frac{d^2T}{dx^2} = 0$

The temperature distribution:

\begin{equation}
T(x) = ax + b
\end{equation}

with the given initial temperature ($T_0 = 1 \degree C$) at one end of the beam

\begin{equation}
T(x) = T_0 \frac{x}{L}
\end{equation}

## Input file

The input file consists of a couple of individual blocks, which are opened with **`[block name]`** and closed with empty **`[]`**. Editable sub blocks are opened with **`[./name]`** and closed with **`[../]`** .

**`[Mesh]`** gives all the information for the generation of the mesh. **`[type = GeneratedMesh]`** will generate a mesh with the given dimension and proportions. **`[nx]`**, **`[ny]`** and **`[nz]`** are the given amount of mesh elements in each direction. **`[xmin]`** and **`[xmax]`** defines from where to where the beam extends along the x-axis. The same applies to **`[ymin]`**, **`[ymax]`** and **`[zmin]`** and **`[zmax]`** in their respective directions.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 10
      ny = 1
      nz = 1
      xmin = 0
      xmax = 100
      ymin = 0
      ymax = 10
      zmin = 0
      zmax = 10
    []
```

**`[Variables]`** creates with **`[./temperature]`** a new variable with the name temperature. The name can be changed, however in the next blocks there will be references to this variable, which needs to have the same name. With **`[../]`** the definition of the variable is closed.

```
    [Variables]
      [./temperature]
        order = FIRST
        family = LAGRANGE
      [../]
    []
```

**`[Kernels]`** defines a new Kernel with **`[./TKernel]`**. The type of Kernel is chosen to fit the variable.

```
    [Kernels]
      [./TKernel]
        type = GolemKernelT
        variable = temperature
      [../]
    []
```

**`[BSc]`** defines the boundary conditions. A new boundary condition is defined with **`[./T0_left]`**, again the name can be changed. The type defines the kind of boundary condition, in this case it is Dirichlet boundary condition or 1st kind. The variable refers to the defined variables from the **`[Variables]`** - Block. **`[boundary = left]`** will automatically find the left spot of the x-axis, so in this chase x = 0m, while **`[boundary = right]`** in the second boundary condition (**`[./T1_right]`**) will find the right end of the x-axis, here x = 100m. The value gives the value of the variable at the chosen boundary.

```
    [BCs]
      [./T0_left]
        type = PresetBC
        variable = temperature
        boundary = left
        value = 0.0
      [../]
      [./T1_right]
        type = PresetBC
        variable = temperature
        boundary = right
        value = 1.0
      [../]
    []
```

**`[Materials]`** defines the material of the beam. Even when no specific material properties are given some simple input properties have to be given in order for the input file to work. With **`[./thermal]`** a new material is created. With type a for a temperature simulation fitting code of Golum is chosen. **`[block = 0]`** is the first and only block of this simulation. The Initial conductivity of the fluid as well as the solid have to be given, here with 1.0, so that there will be no change of the simulation. **`[porosity_uo]`** refers to the porosity defined in the **`[UserObject]`** - Block. The **`[porosity_uo]`** has to be defined in the next block in order for the simulation to work.

```
    [Materials]
      [./thermal]
        type = GolemMaterialT
        block = 0
        thermal_conductivity_fluid_initial = 1.0
        thermal_conductivity_solid_initial = 1.0
        porosity_uo = porosity
      [../]
    []
```

**`[UserObjects]`** defines with **`[./porosity]`** a type of simulation for the porosity, in this case the **`[GolemPososityConstant]`**, which will in this simple example just return the input parameter given. However, it has to be defined in order to work.

```
    [UserObjects]
      [./porosity]
        type = GolemPorosityConstant
      [../]
    []
```

**`[Preconditioning]`** defines a couple of the conditions for the optimization of the calculation.

```
    [Preconditioning]
      [./precond]
        type = SMP
        full = true
        petsc_options = '-snes_ksp_ew'
            petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
        petsc_options_value = 'gmres asm 1E-10 1E-10 200 500 lu NONZERO'
      [../]
    []
```

**`[Executioner]`** not sure what to write here.

```
    [Executioner]
      type = Steady
      solve_type = Newton
    []
```

**`[Outputs]`** defines the kind of log and data output. With **`[print_linear_residuals = true]`**, ever linear equation will be printed. The **`[print_per_log]`** will return computing times and other information. **`[exodus = true]`** will create an exodus file with the suffix .e containing the result of the simulation, which can be opened in Paraview.

```
    [Outputs]
      print_linear_residuals = true
      print_perf_log = true
      exodus = true
    []
```

## Analytical Plot

!media media/examples/test_suite/T1_plot.png
       caption=Temperature distribution
       style=width:50%;
