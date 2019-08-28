# Test T 8

## Description

Transient 2D temperature distribution, non-zero initial temperature, boundary conditions of 1st and 2nd kind

The temperature distribution of a square plate with a side length of ($L = 100\,m$) is investigated. The plate has a thermal conductivity of $\lambda = 0.5787037\, W/(m \cdot K)$ and further properties comprise a heat capacity $c = 0.01\, J/(kg\cdot K)$ and a density of $\rho = 2,000kg/m^3$.

The heat conduction equation is the equation describing the transient temperature distribution:

\begin{equation}
 pc\frac{\partial T}{\partial t} = \lambda \nabla \cdot \nabla T
\end{equation}

with:

\begin{equation}
\chi = \frac{\lambda}{\rho c}
\end{equation}


With a temperature of $T_0=1\, \degree C$ and:

\begin{equation}
f(x) =
\left\{\begin{matrix}
0 & \text{for} & 0 \le x \le \frac{L}{10}\\
\frac{10}{3L}x - \frac{1}{3} & \text{for} & \frac{L}{10} \le x \le \frac{4L}{10}\\
1 & \text{for} & \frac{4L}{10} \le x \le \frac{6L}{10}\\
3 - \frac{10}{3L}x & \text{for} & \frac{6L}{10} \le x \le \frac{9L}{10}\\
0 & \text{for} & \frac{9L}{10} \le x \le L
\end{matrix}\right.
\end{equation}

The simulation starts with the initial temperature distribution $T(x,y,0) = T_0 \cdot f(x)$ and delivers an output after 0.02 days and 0.04 days.

Given the initial conditions:

\begin{equation}
\begin{aligned}
T(x,y,0) = T_0 \cdot f(x) \cdot f(y) & \qquad \text{for} & 0 \le x,y\le L,
\end{aligned}
\end{equation}

The boundary conditions on the x-axis have a specified zero temperature at $x=0$ and $x=L$:

\begin{equation}
\begin{aligned}
T(0,y,t) = 0 &\qquad \text{for} & 0 \le y \le L,\\
T(L,y,t) = 0 &\qquad \text{for} & 0 \le y \le L,
\end{aligned}
\end{equation}

for $y = 0$ and $y = L$ non-flow boundary conditions are defined:

\begin{equation}
\begin{aligned}
\frac{\partial T}{\partial y}(x,0,t) = 0 &\qquad \text{for} 0 \le x \ L,\\
\frac{\partial T}{\partial y}(x,L,t) = 0 &\qquad \text{for}  0 \le x \ L.
\end{aligned}
\end{equation}

the solution for these two boundary conditions are given by the previous example:

\begin{equation}
T1(x,t) = \sum_{n=1}^\infty \text{sin} \frac{n\pi x}{L} \text{exp} \left(-\chi n^2 \pi^2 \frac{t}{L^2} \right) \cdot \frac{80}{3{(n\pi)}^2} \text{sin} \frac{n \pi}{2} \text{sin} \frac{n \pi}{4} \text{sin} \frac{3n \pi}{20}
\end{equation}


\begin{equation}
T2(x,t) = \frac{1}{2} + \sum_{n=1}^\infty \text{cos} \frac{n\pi x}{L} \text{exp} \left(-\chi n^2 \pi^2 \frac{t}{L^2} \right) \cdot \frac{80}{3{(n\pi)}^2} \text{cos} \frac{n \pi}{2} \text{sin} \frac{n \pi}{4} \text{sin} \frac{3n \pi}{20}
\end{equation}

The solution for this problem satisfying all boundary conditions is:

\begin{equation}
T(x,y,t) = T_0 \cdot T1(x,t) \cdot T2(y,t)
\end{equation}

## Input file

**`[Mesh]`** generates a mesh with three dimensions and with 100 Elements in the x and y direction resulting in a mesh with 10000 cubes with side lengths of 1 m.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 100
      ny = 100
      nz = 1
      xmin = 0
      xmax = 100
      ymin = 0
      ymax = 100
      zmin = 0
      zmax = 1
    []
```

**`[Functions]`** loads a **`[data_file]`** called **`[IC_x.txt]`** and **`[IC_y.txt]`** which contains the given boundary conditions above, with values of 1 and 0 for certain points in either the x or y direction of the plate.

IC_x.txt

```
    AXIS X
    0 10 40 60 90 100
    DATA
    0 0 1 1 0 0
```

IC_y.txt

```
    AXIS Y
    0 10 40 60 90 100
    DATA
    0 0 1 1 0 0
```

However, the interpolation of $f(x)$ is done with the **`[PiecewiseMultilinear]`** which will interpolate the values between 0 and 1.  The **`[CompositeFunction]`** then combines the functions.

```
    [Functions]
      [./IC_x_func]
        type = PiecewiseMultilinear
        data_file = IC_x.txt
      [../]
      [./IC_y_func]
        type = PiecewiseMultilinear
        data_file = IC_y.txt
      [../]
      [./IC_func]
        type = CompositeFunction
        functions = 'IC_x_func IC_y_func'
      [../]
    []
```

**`[ICs]`** are the initial boundary conditions. In this case it simply links the temperature to the function which was defined above instead of a fixed value.

```
    [ICs]
      [./IC_temp]
        type = FunctionIC
        variable = temperature
        function = IC_func
      [../]
    []
```

**`[Executioner]`** divides the given start and end time (3456 seconds = 0.04 days) in 10 pieces, so that the simulation is stable.

```
    [Executioner]
      type = Transient
      solve_type = Newton
      start_time = 0.0
      end_time = 3456
      dt = 345.6
      num_steps = 10
    []
```

**`[Outputs]`** now has an interval assigned. Since the **`[Executioner]`** will return 10 time steps and only the 5th and last time step are required the interval of 5 will make the Output only contain the required time steps.

```
    [Outputs]
      interval = 5
      print_linear_residuals = true
      print_perf_log = true
      exodus = true
    []
```

## Analytical Plot

!media media/examples/test_suite/T8_2D_plot_0_02_days.png
       caption=Temperature distribution
       style=width:50%;

!media media/examples/test_suite/T8_plot_0_02_days.png
       caption=2D Temperature distribution
       style=width:50%;
