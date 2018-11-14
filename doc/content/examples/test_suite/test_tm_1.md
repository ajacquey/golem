# Test TM 1

## Description

An elastic beam deforms due to an instant temperature change

A rectangle beam with the length of $L = 1\,m$ is investigated.

Gravity is neglected. The solid material has a Poisson's ratio $v = 0.25$ a Young's modulus $E = 25000\,Pa$, zero heat capacity and a thermal expansion $\alpha = 3 \cdot 10^{-5} \, 1/K$.
The face $x = 0$ is free, all other faces are sliding planes. Simulation start with $T_0 = 0 \degree C$ and uses one time step to instantly change the temperature to $T_1 = 1 \degree C$ at $x = L$ and $T_0$ at $x = 0$.

The steady state is described by the governing Laplace equation:

\begin{equation}
\frac{d^2 T}{dx^2} =0
\end{equation}

Temperature distribution along the x-axis is given by:

\begin{equation}
T(x) = (T_1 - T_0) \frac{x}{L} + T_0
\end{equation}

Due to the setup the x-direction is free of stress, while the y and z-direction have no strain:

\begin{equation}
\begin{aligned}
\sigma_{11} &= 0\\
\epsilon_{22} & = \epsilon_{33} = 0
\end{aligned}
\end{equation}

Integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$

\begin{equation}
\begin{aligned}
u_x(x) &= \alpha \frac{1 + v}{1-v} \frac{T_1 - T_0}{2L}(x^2 - L^2)\\
u_y &= 0\\
u_z &= 0
\end{aligned}
\end{equation}

## Input file

Due to the setup the x-direction is free of stress and y and z-direction are free of strain. As a result only the strain **`[mechanical_strain]`**). and stress of the remaining directions have to be calculated.

```
    [AuxKernels]
      [./strain_xx]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_xx
        index_i = 0
        index_j = 0
      [../]
      [./stress_yy]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_yy
        index_i = 1
        index_j = 1
      [../]
      [./stress_zz]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_zz
        index_i = 2
        index_j = 2
      [../]
```

Since the temperature is 'fixed' this is set up in the **`[BCs]`** part of the input file with type **`[DirichletBC]`**). for the left and the right boundary.

```
    [BCs]
      [./T0_left]
        type = DirichletBC
        variable = temperature
        boundary = left
        value = 0.0
      [../]
      [./T1_right]
        type = DirichletBC
        variable = temperature
        boundary = right
        value = 1.0
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

The **`[Executioner]`** is set up so that only one time step is calculated, since this problem does not look at data over an interval of time.

```
    [Executioner]
      type = Transient
      #solve_type = Newton
      start_time = 0.0
      end_time = 1.0
      dt = 1.0
    []
```

## Analytical Plot

!media media/examples/test_suite/TM1_plot.png
       caption=Thermomechanic deformation
       style=width:50%;
