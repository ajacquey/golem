# Test T 7
# Description
Transient 1D temperature distribution, non-zero initial temperature, boundary conditions of 1st and 2nd kind

The temperature distribution of two long beams ($L = 100\,m$)is investigated. Both beams have a thermal conductivity of $\lambda = 0.5787037\, W/(m \cdot K)$, their heat capacity is $c = 0.01\, J/(kg\cdot K)$ and their density is $\rho = 2,000kg/m^3$.

The heat conduction equation is the equation describing the transient temperature distribution:

\begin{equation}
 pc\frac{\partial T}{\partial t} = \lambda \nabla \cdot \nabla T
\end{equation}

with:

\begin{equation}
\chi = \frac{\lambda}{\rho c}
\end{equation}


With a temperature of $T_0 = 1\,\degree C$ and:

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

The simulation starts with the initial temperature distribution $T(x,0) = T_0 \cdot f(x)$ and delivers an output after 0.05 days and 0.1 days.

Given the initial conditions:

\begin{equation}
\begin{aligned}
T(x,0) = T_0 \cdot f(x) & \qquad \text{for} & 0 \le x\le L,
\end{aligned}
\end{equation}

The boundary conditions for the first beam T1 are:

\begin{equation}
\begin{aligned}
T(0,t) = 0 &\qquad \text{for} & t > 0,\\
T(L,t) = 0 &\qquad \text{for} & t > 0,
\end{aligned}
\end{equation}

while the second beam T2 has a set of non-flow boundary conditions:

\begin{equation}
\begin{aligned}
\frac{\partial T}{\partial x}(0,t) = 0 &\qquad \text{for} & t > 0,\\
\frac{\partial T}{\partial x}(L,t) = 0 &\qquad \text{for} & t > 0.
\end{aligned}
\end{equation}

the solution for the first beam T1 is:

\begin{equation}
T1(x,t) = \sum_{n=1}^\infty \text{sin} \frac{n\pi x}{L} \text{exp} \left(-\chi n^2 \pi^2 \frac{t}{L^2} \right) \cdot \frac{80}{3{(n\pi)}^2} \text{sin} \frac{n \pi}{2} \text{sin} \frac{n \pi}{4} \text{sin} \frac{3n \pi}{20}
\end{equation}

the solution for the beam T2 is:

\begin{equation}
T1(x,t) = \frac{1}{2} + \sum_{n=1}^\infty \text{cos} \frac{n\pi x}{L} \text{exp} \left(-\chi n^2 \pi^2 \frac{t}{L^2} \right) \cdot \frac{80}{3{(n\pi)}^2} \text{cos} \frac{n \pi}{2} \text{sin} \frac{n \pi}{4} \text{sin} \frac{3n \pi}{20}
\end{equation}

## Input file

## (Check the time in the input file, end_time should be 8640 (0.1 days) instead of 7776 (0.09 days) seconds)

**`[Mesh]`** loads a pregenerated mesh since it is not possible to generate a mesh with two individual beams with golem.
The mesh has some predefined boundary IDs which are given new names with **boundary_name** so that instead of the boundary ID 0 or 1, left1 and left2 right can be used in the rest of the input file.

```
    [Mesh]
      type = FileMesh
      file = T_1D_bc_ic_transient.msh
      boundary_id = '0 1 2 3 4 5 6 7'
      boundary_name = 'left1 left2 right1 right2 bottom top front back'
    []
```

**`[Functions]`** loads a **data_file** called **IC_x.txt**, which contains the given boundary conditions above, with values of 1 and 0 for certain points of the beam.
IC_x.text :

```
    AXIS X
    0 10 40 60 90 100
    DATA
    0 0 1 1 0 0
```

However, the interpolation of $f(x)$ is done with the **PiecewiseMultilinear**, which will interpolate the values between 0 and 1.

```
    [Functions]
      [./IC_x_func]
        type = PiecewiseMultilinear
        data_file = IC_x.txt
      [../]
    []
```

**`[ICs]`** are the initial boundary conditions. In this case it simply links the temperature to the function which was defined above instead of a fixed value.

```
    [ICs]
      [./T_IC]
        type = FunctionIC
        variable = temperature
        function = 'IC_x_func'
      [../]
    []
```

**`[Executioner]`** divides the given start and end time (8640 seconds = 0.1 days) again in 10 pieces, so that the simulation is stable.

```
    [Executioner]
      type = Transient
      solve_type = Newton
      start_time = 0.0
      end_time = 8640
      dt = 864
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

!media media/examples/test_suite/T7_plot_beam1_0_090_days.png
       caption=Temperature distribution in beam 1 after 0.090 days
       style=width:50%;
