# Test T 2
## Description

1D Steady-State Temperature Distribution, Boundary Conditions of 1st and 2nd Kind

The temperature distribution along a 100m beam is investigated.

The beam is composed of to two types of materials with two different thermal conductivities. The first 40m has a thermal conductivity of $\lambda_1 = 100 \, W/(m*K)$ and the rest of the beam has a thermal conductivity of $\lambda_2 = 300 \, W/(m*K)$ ).

A specific temperature of $T_0 = 1 \degree C$ prevails at $x = 0m$, while a specific heat flow of $q_{th} = -1.5 \, W/m^2$ is present at $x = L = 100m$.

The Laplace equation describes the steady-state temperature distribution.

\begin{equation}
 \frac{d^2T}{dx^2} = 0
\end{equation}

The temperature distribution is described by:

\begin{equation}
 T(x) = \qquad
 \left\{
 \begin{aligned}
    -\frac{q_{th}}{\lambda_1} x +T_0 & \qquad \text{for} \quad x \le 2L/5 \\
    -\frac{q_{th}}{\lambda_2} x +T_0 +q_{th} \frac{2L}{5} (\frac{1}{\lambda_2} - \frac{1}{\lambda_1}) & \qquad \text{for} \quad x > 2L/5
  \end{aligned}
	\right.
\end{equation}

## Input file

**`[Mesh]`** loads a preexisting mesh with the suffix .msh

```
    [Mesh]
      type = FileMesh
      file = T_1D_bc_steady.msh
      boundary_id = '0 1 2 3 4 5'
      boundary_name = 'left right bottom top front back'
    []
```

**`[BCs]`** contains two boundary conditions. The first boundary condition is a Dirichlet boundary condition or 1st kind. Thus, the type of boundary conditions is **PresetBC**. The second boundary condition, the heat flow, is a Neumann boundary condition or 2nd kind. As a result of this the right kind of boundary type is chosen with **NeumannBC**.

```
    [BCs]
      [./T0_left]
        type = PresetBC
        variable = temperature
        boundary = left
        value = 1.0
      [../]
      [./qf_right]
        type = NeumannBC
        variable = temperature
        boundary = right
        value = 1.5
      [../]
    []
```

**`[Materials]`** defines two different materials with their individual thermal conductivity. First a new material is defined with **`[./thermal_left]`**, with a corresponding block number and an **inital_thermal_conductivity_solid** of a $100 W/(m*K)$. For each material there has to be a user defined porosity **porosity_uo**. The same applies to the second material **`[./thermal_right]`** with a conductivity of $300 W/(m*K)$.

```
    [Materials]
      [./thermal_left]
        type = GolemMaterialT
        block = 0
        thermal_conductivity_fluid_initial = 1.0
        thermal_conductivity_solid_initial = 100.0
        porosity_uo = porosity
      [../]
      [./thermal_right]
        type = GolemMaterialT
        block = 1
        thermal_conductivity_fluid_initial = 1.0
        thermal_conductivity_solid_initial = 300.0
        porosity_uo = porosity
      [../]
    []
```

**`[UserObjects]`** defines the needed porosity for the materials.

```
    [UserObjects]
      [./porosity]
        type = GolemPorosityConstant
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/T2_plot.png 
  caption=Temperature distribution
  style=width:50%;

