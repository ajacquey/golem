# Test TH 1

## Description

Transient 1D temperature distribution in moving fluid

A beam with a length of $L = 10\,m$ is investigated.
The beam is made of a permeable material with a permeability $k = 10^{-11}\,m^2 $ and a porosity of $\Phi = 0.1$. The fluid has a viscosity of $\mu = 1\,mPa\cdot s$ and is incompressible.
Gravity is neglected. Density, heat capacity and thermal conductivity for both the fluid and the material of the beam (solid grain) are given below.


|                        | Fluid                          | Solid grain                    |
|------------------------|--------------------------------|--------------------------------|
| Density                | $\rho_f = 1000\,kg/m^3$           | $\rho_s = 2000\,kg/m^3$           |
| Specific heat capacity | $c_f = 1100\,J/(kg\cdot K)$    | $c_s = 250\,J/(kg\cdot K)$     |
| Thermal conductivity   | $\lambda_f = 10\,W/(m\cdot K)$ | $\lambda_s = 50\,W/(m\cdot K)$ |

There is $p_0 = 10^5\,Pa$ pressure at the fluid inlet ($x = 0\,m$) and zero pressure at the outlet ($x = L$) generating a steady state flow along the x-axis. A constant temperature $T_0 = 10\, \degree C$ is applied at the inlet for $t > 0$ starting from zero initial temperature. Evaluation is implemented after 10000 seconds (or 2.5 h) and 20000 seconds (or 5h).

Darcy's law for incompressible fluids and the continuity equation yields the Laplace equation as the governing equation:

\begin{equation}
\frac{\partial^2 p}{\partial x^2} = 0
\end{equation}

The specific discharge is obtained by Darcy's law

\begin{equation}
q = \frac{k}{\mu} \frac{p_0}{L} = 0
\end{equation}

With the notation:

\begin{equation}
\begin{aligned}
w &= \frac{\Phi p_f c_f}{\Phi p_f c_f + (1-\Phi) p_s c_s \Phi} \frac{q}{\Phi}\\
\chi &= \frac{\Phi \lambda_f +(1-\Phi)\lambda_s}{\Phi p_f c_f + (1- \Phi) p_s c_s}
\end{aligned}
\end{equation}

the heat transport equation is:

\begin{equation}
\frac{\partial T}{\partial t} + w \frac{\partial T}{\partial x} = \chi \frac{\partial^2 T}{\partial x^2}
\end{equation}

with the initial condition

\begin{equation}
T(x,0) = 0 \quad \text{for}\quad x > 0
\end{equation}

and the boundary conditions

\begin{equation}
\begin{aligned}
&T(0,t) = T_0 &\quad \text{for} \quad t > 0\\
\lim\limits_{x \to \infty} & T(x,t) = 0 &\quad \text{for} \quad t > 0
\end{aligned}
\end{equation}

Applying the Laplace transform with respect to t yields the differential equation

\begin{equation}
\chi \overline{T}'' - w\overline{T} -s\overline{T} = 0, \overline{t}
\end{equation}

$\overline{T}$ is the transform of $T$, $s$ is the transformation parameter and the prime denotes the derivative to x. Solving the equation with respect to the transformed boundary conditions yields:

\begin{equation}
\overline{T}(x,s) = \frac{T_0}{s} \text{exp}\left[x\left(\frac{w}{2\chi} - \sqrt{(\frac{w}{2\chi})^2 + \frac{s}{\chi}}\right)\right]
\end{equation}

## Input file

**`[GlobalParams]`** gives the temperature and pore pressure for the kernels. In this case the variables are named the same as the demanded variable by the kernels. However, these **`[GlobalParms]`** still have to be defined in order for the kernels to find the variables.

```
    [GlobalParams]
      pore_pressure = pore_pressure
      temperature = temperature
    []
```

**`[AuxKernels]`** takes the defined variables from **`[AuxVariables]`** and assigns them to the correct component of the **`[GolemDarcyVelocity]`** kernel, so that the kernel knows which variable equals which direction.

```
    [AuxKernels]
      [./darcyx]
        type = GolemDarcyVelocity
        variable = vx
        component = 0
      [../]
      [./darcyy]
        type = GolemDarcyVelocity
        variable = vy
        component = 1
      [../]
      [./darcyz]
        type = GolemDarcyVelocity
        variable = vz
        component = 2
      [../]
    []
```

**`[Kernels]`** assigns to correct kernel to the calculation to each variable. Additionally, for the **`[GolemKernelTH]`** you can choose the type of calculation. Standard procedure should be **`[is_conservative = true]`** in which case no assumptions are made. However, if your simulation is (almost) steady state and convection is irrelevant or the simulation tends to be unstable you can switch the calculation to **`[is_conservative = false]`**.

```
    [Kernels]
      [./HKernel]
        type = GolemKernelH
        variable = pore_pressure
      [../]
      [./temp_time]
        type = GolemKernelTimeT
        variable = temperature
      [../]
      [./THKernel]
        type = GolemKernelTH
        variable = temperature
        is_conservative = true
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/TH1_plot.png
       caption=Temperature distribution
       style=width:50%;
