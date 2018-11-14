# Test TH 2

## Description

A transient 1D temperature distribution in a moving fluid

A rectangle plate with the measurements `[0,2L]` by `[-0.75L, 0.75L]` with $L = 1 \ m$ is investigated. The plate consists of a permeable material with a permeability $k = 10^{-11}\,m^2 $ and a porosity of $\Phi = 0.1$. The fluid has a viscosity of $\mu = 1\,mPa \, s$ and is incompressible. Gravity is neglected. Density, heat capacity and thermal conductivity for both the fluid and the material of the beam (solid grain) are given below.


|                        | Fluid                         | Solid grain                    |
|------------------------|--------------------------------|--------------------------------|
| Density                | $\rho_f = 1000\,kg/m^3$         | $\rho_s = 2000\,kg/m^3$           |
| Specific heat capacity | $c_f = 1100\,J/(kg\cdot K)$    | $c_s = 250\,J/(kg\cdot K)$     |
| Thermal conductivity   | $\lambda_f = 0.5\,W/(m\cdot K)$ | $\lambda_s = 2.0\,W/(m\cdot K)$ |

Fluid pressure at the fluid inlet ($x = 0\,m$) is set to $p_0 = 2 \cdot 10^4\,Pa$ and a zero pressure boundary is imposed at the outlet ($x = 2L$) generating a steady state flow along the x-axis. The width of the inlet is determined by the variable $a$ and a temperature gradient at the border of the inlet is applied, resulting a "buffer zone" with the width $b$. With the inlet temperature $T_0 = 10\, \degree C$ , $a = 0.15\,m$ and $b=0.25\,m$ the specific inlet temperature is:

\begin{equation}
g(y) =
\left\{\begin{matrix}
0 & \text{for} & y \le b \\
T_0\frac{b+y}{b-a}& \text{for} & -b \le y \le -a\\
T_0 & \text{for} & -a \le y \le a\\
T_0  \frac{b-y}{b-a} & \text{for} & a \le y \le b\\
0 & \text{for} & b \le y
\end{matrix}\right.
\end{equation}

The 2D analytical solution is outlined in the literature [cite:kolditz2016] - however, a 1D analytical solution may be easily obtained by setting $y = z = 0$. Thus the temperature distribution is only evaluated along the x-axis and the same method as in the TH1 test can be applied. For the 1D case, [cite:ogata1961] have also given a solution which reads:

\begin{equation}
T(x,t) = \frac{T_0}{2} \text{erfc} (\frac{x-v_x \, t}{\sqrt{4kt}}) + \frac{T_0}{2} \text{exp} (\frac{v_x \, x}{k}) \text{erfc} (\frac{x+v_x \, t}{\sqrt{4kt}})
\end{equation}



## Input file

`[Functions]` defines the used functions. The `PicewiseMultilinear` will open the data `BC_y.txt`, which contains the following:

```
    AXIS Y
    -0.75 -0.25 -0.15 0.15 0.25 0.75
    DATA
    0 0 1 1 0 0
```

and will build a continuous line from these points. This line will than be combined with a constant function here called `T0_func` which creates the function `T_bc_func` using the `type = CompositeFunction` to combine both functions. Using this method an individual inlet can be created.

```
    [Functions]
      [./T_bc_func_y]
        type = PiecewiseMultilinear
        data_file = BC_y.txt
      [../]
      [./T0_func]
        type = ConstantFunction
        value = 10.0
      [../]
      [./T_bc_func]
        type = CompositeFunction
        functions = 'T_bc_func_y T0_func'
      [../]
    []
```

The `[Executioner]` is given the start and end time of the simulation as well as the size of the time steps (dt) and the resulting number of time steps. Not all time steps are needed for the solution of the problem, however, if the gap between the time steps becomes to big the simulation becomes unstable.

```
    [Executioner]
      type = Transient
      solve_type = Newton
      start_time = 0.0
      end_time = 7000
      dt = 700
      num_steps = 10
    []
```


`[Outputs]` is set up to only give out every fifth result, because only these are required. The fifth result equals a simulation time of 3500 seconds and the 10th result of the `[Executioner]` equals the simulation time of 7000 seconds.

```
    [Outputs]
      interval = 5
      print_perf_loginear_residuals = true
      print_perf_log = true
      exodus = true
    []
```


## Result

The analytical solution can be obtained similarly to the 1D case in test TH1, if the temperature is plotted along the x-axis with $y \, = \, z \, = \, 0$.

!media media/examples/test_suite/TH1_result.png
       caption=Temperature distribution
       style=width:50%;

!bibtex bibliography
