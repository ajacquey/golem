# Test HM 2

##Description

A permeable elastic beam deforms under transient internal fluid pressure. Specified boundary conditions are time-dependent and of 1st and 2nd kind

A rectangle beam with a length of $L = 1\,m$ is investigated. The beam consists of a permeable material with a permeability $k = 10^{-10}\,m^2 $ and zero porosity. The fluid has a viscosity of $\mu = 1\,mPa\cdot s $. Gravity is neglected. The solid material has a Poisson's ratio $v = 0.2$ a Young's modulus $E = 27000\,Pa$ and a Biot number one.
The face $x = 0$ is free, while all other faces are sliding planes. Zero pressure is specified at the face $x = 0$. The specific discharge is $q_1 \cdot t$ ($q_1 = 7.6 * 10^{-5}\, m/s$) and is applied at the face $x = L$ for $t>0$.
The simulation evaluates the transient pressure, stress, strain and displacement after 5 and 10 seconds.

Using Biot's simplified theory, with $\sigma$ as the stress tensor and $I$ as the unit tensor, the equation of the mechanical equilibrium is:

\begin{equation}
0 = \nabla \cdot (\sigma - p I)
\end{equation}

and with the stress $\sigma_{22}$ and $\sigma_{33}$ being only functions of $x$ the horizontal stress $\sigma_{11}$ satisfies

\begin{equation}
\frac{\partial}{\partial x} (\sigma_{11} - p) = 0
\end{equation}

Introducing:

\begin{equation}
\chi = \frac{kE}{\mu} \bigg/ \bigg( 1 - \frac{2v^2}{1-v} \bigg)
\end{equation}

the formal problem is to determine the solution $p(x,t)$ of the parabolic equation:

\begin{equation}
\frac{\partial p}{\partial t} = \chi \frac{\partial^2 p}{\partial x^2}
\end{equation}

Applying the Laplace transform with respect to $t$ yields the ordinary differential equation:

\begin{equation}
\chi \, \overline{p}'' - s \, \overline{p} = 0
\end{equation}
with $\overline{p}$ as the transform of $p$, $s$ is the transformation parameter and the prime denotes the derivative with respect to $x$.

the solution for $\overline{p}(x,s)$ is:

\begin{equation}
\overline{p}(x,s) = q_1 \frac{\mu}{k} \frac{\sinh(\sqrt{s/ \chi}x)}{s^2 \sqrt{s/ \chi} \cosh (\sqrt{s/ \chi})L}
\end{equation}

the solution for the displacement reads:

\begin{equation}
\overline{u}_x(x,s) = q_1 \frac{\cosh (\sqrt{s / \chi}x) - \cosh (\sqrt{s/ \chi}L)} {s^3 \cosh (\sqrt{s /\chi}L)}
\end{equation}

## Input file

**`[Functions]`** defines the specific discharge which increases linear with time (boundary condition 2nd kind).

```
    [Functions]
      [./func_q1_right]
        type = ParsedFunction
        value = 'q_rate*t'
        vars = 'q_rate'
        vals = '7.6e-05'
      [../]
    []
```

Since there is a combination of 1st and 2nd kind boundary conditions, both kinds are defined in the **`[BCs]`** part of the input file. So instead of just type **PresetBC** and **FunctionPresetBC** for boundary conditions 1st kind, there is also type **FunctionNeumannBC** for the linear increasing inlet as a boundary condition of 2nk kind for the pore pressure.

```
    [BCs]
      [./p0_left]
        type = PresetBC
        variable = pore_pressure
        boundary = left
        value = 0.0
      [../]
      [./q1_right]
        type = FunctionNeumannBC
        variable = pore_pressure
        boundary = right
        function = func_q1_right
      [../]
      [./no_x_right]
        type = PresetBC
        variable = disp_x
        boundary = right
        value = 0.0
      [../]
      [./no_y]
        type = PresetBC
        variable = disp_y
        boundary = 'front back'
        value = 0.0
      [../]
      [./no_z]
        type = PresetBC
        variable = disp_z
        boundary = 'bottom top'
        value = 0.0
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/HM2_disp_x_plot.png
       caption= pore pressure distribution
       style=width:50%;

!media media/examples/test_suite/HM2_pore_pressure_plot.png
       caption= pore pressure distribution
       style=width:50%;
