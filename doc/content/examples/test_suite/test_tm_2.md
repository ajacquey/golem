# Test TM 2
## Description
An Elastic Square Deforms Due to an Instant Temperature Change

A square with a side length of $L = 1\,m$ orientated in the x-y-plane is investigated.

Gravity is neglected. The solid material has a Poisson's ratio $v = 0.25$ a Young's modulus $E = 25000\,Pa$, zero heat capacity and a thermal expansion $\alpha = 4 \cdot 10^{-5} \, 1/K$.
The top and bottom and the lateral faces on the coordinate planes are sliding planes. Simulation start with $T_0 = 0 \degree C$  and uses one time step to instantly change the temperature  to $T_1 = 1 \degree C$ at the bottom.

Due to the setup the x- and y-direction is free of stress, while the z-direction has no strain:

\begin{equation}
\begin{aligned}
\sigma_{11} &= \sigma_{22} = 0\\
\epsilon_{33} & = 0
\end{aligned}
\end{equation}

Due to the change of temperature from T_0 to T_1 the remaining stresses and strains are:

\begin{equation}
\begin{aligned}
\sigma_{33} &= -\alpha\, (T_1 - T_0)E\\
\epsilon_{11} & = \epsilon_{22} = (1+v)\,\alpha\, (T_1 - T_0)
\end{aligned}
\end{equation}

Integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$

\begin{equation}
\begin{aligned}
u_x(x) &= (1+v)\, \alpha \, (T_1 - T_0)x\\
u_y(y) &= (1+v)\, \alpha \,(T_1 - T_0)y\\
u_z &= 0
\end{aligned}
\end{equation}

## Input file

Besides the dimension of the mesh the strain and stress of the individual directions have to be adjusted. As a result only the stress in the z-direction and the strain in x- and y direction has to be calculated.

```
    [AuxKernels]
      [./strain_xx]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_xx
        index_i = 0
        index_j = 0
      [../]
      [./strain_yy]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_yy
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
    []
```

## Analytical Plot

!media media/examples/test_suite/TM2_plot.png
       caption=Thermomechanic deformation
       style=width:50%;

