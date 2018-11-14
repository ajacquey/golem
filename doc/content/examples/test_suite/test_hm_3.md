# Test HM 3

## Description

Biot's 1D consolidation problem: Squeezing of a pressurized column causes the fluid to discharge from the domain

A rectangle beam with the length of $L = 1\,m$ is investigated. The beam is made of a permeable material with a permeability $k = 10^{-10}\,m^2 $ and zero porosity. The fluid has a viscosity of $\mu = 1\,mPa\cdot s $. Gravity is neglected. The solid material has a Poisson's ratio $v = 0.2$ a Young's modulus $E = 30000\,Pa$ and a Biot number one.
All the faces are sliding planes, except $x = L$. At the face $x = L$ pressure and mechanical boundary conditions are given. A compressive stress of $10000\,Pa$ acht in negative x-direction, while pressure is assigned 0 for $t> 0$. Starting from equilibrium (pressure $p_i = 1000\,Pa$) and zero mechanical stress, the simulation evaluates the transient pressure distribution $p(x,t)$, stress, strain and displacements with output after 5 and 10 seconds.


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


Applying the Laplace transform with respect to $t$ yields the ordinary differential equation:

\begin{equation}
\chi \, \overline{\sigma}_{11}'' - s \, \overline{\sigma}_{11} = 0
\end{equation}
with $\overline{\sigma}$ as the transform of $\sigma$, $s$ is the transformation parameter and the prime denotes the derivative with respect to $t$.

the solution for $\overline{\sigma}_{11}(x,s)$ is:

\begin{equation}
\overline{\sigma}_{11}(x,s) = -p_i \frac{\cosh(\sqrt{s/ \chi}x)}{s \cosh (\sqrt{s/ \chi}L)}
\end{equation}

the solution for the displacement reads:

\begin{equation}
\overline{u}_x(x,s) = - \frac{p_i}{E} \bigg( 1- \frac{2v^2}{1-v} \bigg) \frac{\sinh (\sqrt{s / \chi}x)} {s \sqrt{s / \chi} \cosh (\sqrt{s/ \chi}L)}
\end{equation}

## Input file


The scaling option in the **`[Variables]`** is only a numerical option which helps to solve the problem faster. However, it does not have any physical meaning.

```
    [Variables]
      [./pore_pressure]
        order = FIRST
        family = LAGRANGE
        initial_condition = 1.0e+03
        scaling = 1.0e+06
      [../]
      [./disp_x]
        order = FIRST
        family = LAGRANGE
      [../]
      [./disp_y]
        order = FIRST
        family = LAGRANGE
      [../]
      [./disp_z]
        order = FIRST
        family = LAGRANGE
      [../]
    []
```

The fixed boundary condition or Dirichlet boundary condition has to be defined in the **`[BCs]`** part of the input file. Here the pore pressure is defined with type **`[DirichletBC]`** and assigned the value 0.

```
    [BCs]
      [./p0_right]
        type = DirichletBC
        variable = pore_pressure
        boundary = right
        value = 0.0
      [../]
      [./no_x_left]
        type = PresetBC
        variable = disp_x
        boundary = left
        value = 0.0
      [../]
      [./load_x_right]
        type = NeumannBC
        variable = disp_x
        boundary = right
        value = -1.0e+03
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

## Analytical plot

!media media/examples/test_suite/HM3_disp_x_plot.png
       caption=x-Displacement
       style=width:50%;

!media media/examples/test_suite/HM3_stress_xx_plot.png
       caption=Stress xx
       style=width:50%;
