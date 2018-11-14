# Test T 3

## Description

2D steady-state temperature distribution, boundary conditions of 1st kind

The temperature distribution of a plate 1 by 1 meter ($L = 1\,m$) is investigated. The thermal conductivity of the material is $1\,W/(m*K)$.
The Laplace equation describes the steady-state temperature distribution.

\begin{equation}
 \frac{d^2T}{dx^2} + \frac{d^2T}{dy^2} = 0
\end{equation}

With a $T_0$ temperature of $1\, \degree C$ the boundary conditions are:

\begin{equation}
\begin{aligned}
T(x,0)& = 0 &\qquad for \quad 0 \le x \le L, \\
T(0,y)& = 0 &\qquad for \quad 0 \le y \le L, \\
T(x,L)& = T_0 \frac{x}{L} &\qquad for \quad 0 \le x \le L, \\
T(L,y)& = T_0 \frac{y}{L} &\qquad for \quad 0 \le y \le L,
\end{aligned}
\end{equation}
The resulting temperature distribution is:

\begin{equation}
T(x, y) = T_0 \frac{x}{L} \frac{y}{L}
\end{equation}

## Input file

**`[Mesh]`** generates a three-dimensional mesh with 50 elements in x and y direction over a length of 1 meter. **`[zmax = 0.02]`** is a thin layer regarding the z-direction since it is not a relevant direction for the test.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 50
      ny = 50
      nz = 1
      xmin = 0
      xmax = 1
      ymin = 0
      ymax = 1
      zmin = 0
      zmax = 0.02
    []
```

**`[Functions]`** generates two functions describing the temperature along the x and the y-axis. With **`[./T_right_func]`** a new function is defined. The type **`[ParsedFunction]`** only takes the variables x,y and z as well as time as an input. **`[value]`** defines the end value of the function. **`[vars]`** defines the fixed variables of the function, in this case **`[T0]`** and **`[L]`**, and **`[vals]`** then defines the actual values of the fixed variables.

```
    [Functions]
      [./T_right_func]
        type = ParsedFunction
        value = 'T0*y/L'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
      [./T_top_func]
        type = ParsedFunction
        value = 'T0*x/L'
        vars = 'T0 L'
        vals = '1.0 1.0'
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/T3_plot.png
       caption=Temperature distribution
       style=width:50%;


!media media/examples/test_suite/T3_plot_2D.png
       caption=2D Temperature distribution
       style=width:50%;
