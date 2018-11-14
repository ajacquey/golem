# Test T 5

## Description

3D steady-state temperature distribution

The temperature distribution of a cube with a side length of $L = 1\,m$ is investigated. The thermal conductivity of the material is $1\,W/(m*K)$.

The Laplace Equation describes the steady-state temperature distribution.

\begin{equation}
 \frac{d^2T}{dx^2} + \frac{d^2T}{dy^2} + \frac{d^2T}{dz^2} = 0
\end{equation}

With a temperature of $T_0 = 1\, \degree C$ the boundary conditions are:

\begin{equation}
\begin{aligned}
T(y,z,0)& = T_0(0+\frac{y}{L}+\frac{z}{L}) &\qquad \text{on the face} \quad x=0, \\
T(x,0,z)& = T_0(\frac{x}{L}+0+\frac{z}{L}) &\qquad \text{on the face} \quad y=0, \\
T(x,y,0)& = T_0(\frac{x}{L}+\frac{y}{L}+0) &\qquad \text{on the face} \quad z=0, \\
T(L,y,z)& = T_0(1+\frac{y}{L}+\frac{z}{L}) &\qquad \text{on the face} \quad x=L, \\
T(x,L,z)& = T_0(\frac{x}{L}+1+\frac{z}{L}) &\qquad \text{on the face} \quad y=L, \\
T(x,y,L)& = T_0(\frac{x}{L}+\frac{y}{L}+1) &\qquad \text{on the face} \quad z=L. \\
\end{aligned}
\end{equation}

The resulting temperature distribution is:

\begin{equation}
T(x,y,z) = T_0(\frac{x}{L}+\frac{y}{L}+\frac{z}{L})
\end{equation}

## Input file

**`[Mesh]`** generates a mesh with 5 by 5 by 6 hexahedral elements with a size of 1 by 1 by 1 meters.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 5
      ny = 5
      nz = 6
      xmin = 0
      xmax = 1
      ymin = 0
      ymax = 1
      zmin = 0
      zmax = 1
    []
```

**`[Functions]`** defines the functions which are interlinked with the boundary conditions. The functions are just defined as stated above.

```
    [Functions]
      [./T_left_func]
        type = ParsedFunction
        value = 'T0*(0+y/L+z/L)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_right_func]
        type = ParsedFunction
        value = 'T0*(1+y/L+z/L)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_bottom_func]
        type = ParsedFunction
        value = 'T0*(x/L+0+z/L)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_top_func]
        type = ParsedFunction
        value = 'T0*(x/L+1+z/L)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_back_func]
        type = ParsedFunction
        value = 'T0*(x/L+y/L+0)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_front_func]
        type = ParsedFunction
        value = 'T0*(x/L+y/L+1)'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
    []
```

**`[BCs]`** defines the boundary conditions using the functions defined above.

```
    [BCs]
      [./T_left]
        type = FunctionPresetBC
        variable = temperature
        boundary = left
        function = T_left_func
      [../]
      [./T_right]
        type = FunctionPresetBC
        variable = temperature
        boundary = right
        function = T_right_func
      [../]
      [./T_bottom]
        type = FunctionPresetBC
        variable = temperature
        boundary = bottom
        function = T_bottom_func
      [../]
      [./T_top]
        type = FunctionPresetBC
        variable = temperature
        boundary = top
        function = T_top_func
      [../]
      [./T_front]
        type = FunctionPresetBC
        variable = temperature
        boundary = front
        function = T_front_func
      [../]
      [./T_back]
        type = FunctionPresetBC
        variable = temperature
        boundary = back
        function = T_back_func
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/T5_plot.png
       caption=Temperature distribution
       style=width:50%;

!media media/examples/test_suite/T5_plot_3D.png
       caption=3D Temperature distribution
       style=width:50%;
