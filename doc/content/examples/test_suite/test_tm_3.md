# Test TM 3

## Description

An elastic cube deforms due to an instant temperature change

A cube with an edge size of $L = 1\,m$ is investigated.

Gravity is neglected. The solid material has a Poisson's ratio $v = 0.25$ a Young's modulus $E = 25000\,Pa$, zero heat capacity and a thermal expansion $\alpha = 5 \cdot 10^{-5} \, K^{-1}$.
The faces of the coordinate planes are sliding planes. Simulation starts with $T_0 = 0 \degree C$  and uses one time step to instantly change the temperature  to $T_1 = -40 \degree C$ at top of the domain.

Due to the setup the entire system is free of shear.

\begin{equation}
\begin{aligned}
\sigma_{11} = \sigma_{22} = \sigma_{33} = 0
\end{aligned}
\end{equation}

Due to the change of temperature from T_0 to T_1 the remaining strains are:

\begin{equation}
\begin{aligned}
\epsilon_{11} = \epsilon_{22} = \epsilon_{33}\, \alpha \,(T_1 - T_0)
\end{aligned}
\end{equation}


Integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$

\begin{equation}
\begin{aligned}
u_x(x) &=\, \alpha \,(T_1 - T_0)x\\
u_y(y) &=\, \alpha \,(T_1 - T_0)y\\
u_z(z) &=\, \alpha \,(T_1 - T_0)z
\end{aligned}
\end{equation}

## Input file

With the type **`[DirichletBC]`** the temperature on the front or top of the domain is given with -40°C.

```
    [BCs]
      [./T1_front]
        type = DirichletBC
        variable = temperature
        boundary = front
        value = -40
      [../]
      [./no_x_left]
        type = PresetBC
        variable = disp_x
        boundary = left
        value = 0.0
      [../]
      [./no_y_bottom]
        type = PresetBC
        variable = disp_y
        boundary = bottom
        value = 0.0
      [../]
      [./no_z_back]
        type = PresetBC
        variable = disp_z
        boundary = back
        value = 0.0
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/TM3_plot.png
       caption=Thermomechanic deformation
       style=width:50%;
