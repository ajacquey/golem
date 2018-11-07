# Test TM 5

## Description

An elastic beam deforms due to a transient temperature change. Temperature boundary conditions are time-dependent and of 1st kind

A beam with a length of $L = 1\,m$ is investigated.

The solid material has a density $\rho = 2000\, kg/m^3$, a Poisson's ratio $v = 0.25$, a Young's modulus $E = 25000\,Pa$, $c =0.45\, J/(kg\cdot K)$ heat capacity, a thermal conductivity of $\lambda = 2.7\, W/(m\cdot K)$ and a thermal expansion $\alpha = 3 \cdot 10^{-4} \, 1/K$. Gravity is neglected.

$x = 0$ is free, while all the other faces are sliding planes. Initial temperature is zeros at $x = 0$ with a linear increase over time $T_1 \cdot t $ ($T_1 = 1\, \degree C/s$) at $x = L$ for $t>0$.
The temperature is evaluated after 5 and 10 seconds

The transient temperature distribution is described by the governing heat conduction equation:

\begin{equation}
p\,c\frac{\partial T}{\partial t} =\lambda \nabla \cdot \nabla T
\end{equation}

with the notation:

\begin{equation}
\chi = \frac{\lambda}{p\,c}
\end{equation}

the given 1D problem is described with the parabolic equation:

\begin{equation}
\frac{1}{\chi} \frac{\partial T}{\partial t} = \frac{\partial^2 T}{\partial^2 x}
\end{equation}

Due to the setup the x-direction is free of stress, while y- and z-direction are free of strain, as a result the change in temperature lead to a change of stress and strain in the remaining directions:

\begin{equation}
\begin{aligned}
\sigma_{22} &= \sigma_{33} -\alpha \frac{E}{1-v} T(x,t)\\
\epsilon_{11}&= \alpha \frac{1+v}{1-v} T(x,t)
\end{aligned}
\end{equation}

Integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$

\begin{equation}
\begin{aligned}
u_x(x) &= \alpha \frac{1+v}{1-v}\, \int^x_L T(x',t)dx'\\
u_y &= u_z = 0
\end{aligned}
\end{equation}

Applying the Laplace transform with respect to $t$ yields the ordinary differential equation:

\begin{equation}
\chi \, \overline{T}'' - s \, \overline{T} = 0
\end{equation}
with $\overline{T}$ as the transform of $T(x,t)$, $s$ is the transformation parameter. The prime denotes the derivative with respect to $x$. This yields the transform of the only non zeros displacement u_x(x,t) becomes:

\begin{equation}
\begin{aligned}
\overline{T}(x,s) &= T_1 \frac{\sinh (\sqrt{s/ \chi}x)}{s^2 \sinh (\sqrt{s/ \chi}L)}\\
\overline{u}_x(x,s) &= \alpha \frac{1+v}{1-v} T_1 \frac{\cosh (\sqrt{s/ \chi}x) - \cosh (\sqrt{s/ \chi}L) }{s^2 \sqrt{s/ \chi} \sinh (\sqrt{s / \chi}L)}
\end{aligned}
\end{equation}

## Input file

Because the problem now is time dependent, the appropriate Kernel type **GolemKernelTimeT** has to be used for the time dependent temperature.

```
    [Kernels]
      [./T_time]
        type = GolemKernelTimeT
        variable = temperature
      [../]
      [./TKernel]
        type = GolemKernelT
        variable = temperature
      [../]
      [./MKernel_x]
        type = GolemKernelM
        variable = disp_x
        component = 0
      [../]
      [./MKernel_y]
        type = GolemKernelM
        variable = disp_y
        component = 1
      [../]
      [./MKernel_z]
        type = GolemKernelM
        variable = disp_z
        component = 2
      [../]
    []
```

the linear increase of the temperature is defined in the **`[Functions]`** part of the input file

```
    [Functions]
      [./T_right_func]
        type = ParsedFunction
        value = 'T1*t'
        vars = 'T1'
        vals = '1.0'
    []
```

and applied to the $x = L$ face in the **`[BCs]`** part of the input file. The right boundary represents the $x = L$ face.

```
    [BCs]
      [./T0_left]
        type = PresetBC
        variable = temperature
        boundary = left
        value = 0.0
      [../]
      [./T1_right]
        type = FunctionPresetBC
        variable = temperature
        boundary = right
        function = T_right_func
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
        boundary = 'bottom top'
        value = 0.0
      [../]
      [./no_z]
        type = PresetBC
        variable = disp_z
        boundary = 'back front'
        value = 0.0
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/TM5_displacement_plot.png
       caption=Horizontal displacement
       style=width:50%;

!media media/examples/test_suite/TM5_temperature_plot.png
       caption=Temperature distribution
       style=width:50%;
