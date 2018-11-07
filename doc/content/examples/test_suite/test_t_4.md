# Test T 4
## Description
2D steady-state temperature distribution, boundary conditions of 1st and 2nd kind

The temperature distribution of a plate with dimensions of 2 by 1 meter ($L = 1\,m$) is investigated. The thermal conductivity of the material is $1\,W/(m*K)$.

The Laplace Equation describes the steady-state temperature distribution.

\begin{equation}
 \frac{d^2T}{dx^2} + \frac{d^2T}{dy^2} = 0
\end{equation}

With a temperature of $T_0 = 1\, \degree C$ the boundary conditions are:

\begin{equation}
\begin{aligned}
T(x,0)& = \frac{T_0}{L}x &\qquad for \quad 0 \le x \le 2L, \\
T(x,L)& = \frac{T_0}{L}(x+2L) &\qquad for \quad 0 \le x \le 2L, \\
T(2L,y)& = \frac{T_0}{L}(2L + 2y) &\qquad for \quad 0 \le y \le L, \\
\frac{dT}{dx}(0,y)& =\frac{T_0}{L} &\qquad for \quad 0 \le y \le L.
\end{aligned}
\end{equation}
The resulting temperature distribution is:

\begin{equation}
T(x, y) = \frac{T_0}{L}(x+2y)
\end{equation}

## Input file

**`[Functions]`** defines the functions which are interlinked with the boundary conditions.

```
[Functions]
  [./T_right_func]
    type = ParsedFunction
    value = 'T0*(2*y+2*L)/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_bottom_func]
    type = ParsedFunction
    value = 'T0*x/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
  [./T_top_func]
    type = ParsedFunction
    value = 'T0*(x+2*L)/L'
    vars = 'T0 L'
    vals = '1.0 1.0'
  [../]
[]
```

**`[BCs]`** defines the boundary conditions and their corresponding types. **`[./qf_left]`** defines the heat flow from the left of the plate with a **NeumannBC** or 2nd kind. All the other boundary conditions are of 1st kind which results in a **type=FunctionPresetBC**.

```
    [BCs]
      [./qf_left]
        type = NeumannBC
        variable = temperature
        boundary = left
        value = -1.0
      [../]
      [./T_bottom]
        type = FunctionPresetBC
        variable = temperature
        boundary = bottom
        function = T_bottom_func
      [../]
      [./T_right]
        type = FunctionPresetBC
        variable = temperature
        boundary = right
        function = T_right_func
      [../]
      [./T_top]
        type = FunctionPresetBC
        variable = temperature
        boundary = top
        function = T_top_func
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/T4_plot.png 
       caption=Temperature distribution
       style=width:50%;


!media media/examples/test_suite/T4_plot_2D.png
       caption=2D Temperature distribution
       style=width:50%;


