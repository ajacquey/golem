# Test THM 1
## Description
A permeable elastic cuboid deforms due to gravity, internal fluid pressure and instant temperature change

A cuboid with a height of $H = 30\,m$ is investigated.

The solid material has density $\rho = 2038.736\, kg/m^3$, a Poisson's ratio $v = 0.25$, a Young's modulus $E = 10000\,MPa$, zero heat capacity and a thermal expansion $\alpha = 3 \cdot 10^{-6} \, 1/K$. Gravity is applied in negative z-direction. fluid density is $\rho_f = 1019.368\, kg/m^3$.
There is zero pressure at the top face $z = H$ and all the other faces are sliding planes.
Simulation start with $T_0 = 0 \degree C$  and uses one time step to instantly change the temperature  to $T_1 = 2.5 \degree C$ to the entire domain.

The pressure distribution is hydrostatic and only depends on the z coordinate:

\begin{equation}
p(z) = p_f \,g (H - z)
\end{equation}

let $\sigma$ denotes the stress tensor and $I$ the unit tensor:

\begin{equation}
0 = \nabla \cdot (\sigma - p \,I) - (0,0,p_s\,g)
\end{equation}

the vertical stress $\sigma_{33}$ is (the face $H = z$ is free):

\begin{equation}
\sigma_{33} = \alpha \frac{E}{1-v}(T_1 - T_0) + (p_s - p_f)(-g)(H-z)
\end{equation}

Due to the simulation setup there is no horizontal displacement anywhere, hence the horizontal train is:

\begin{equation}
\epsilon_{11} = \epsilon_{22} = 0
\end{equation}

the remaining strain and stress are:

\begin{equation}
\begin{aligned}
\sigma_{11} &= \sigma_{22} = -\alpha \frac{E}{1-v}(T_1 - T_0) + \frac{v}{1-v} (p_s - p_f)(-g)(H-z)\\
\epsilon_{11} &= \frac{1+v}{1-v}\alpha(T_1 - T_0) + \bigg( 1- \frac{2v^2}{1-v} \bigg) \frac{1}{E}(p_s - p_f) (-g) (H-z)
\end{aligned}
\end{equation}

integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$:

\begin{equation}
\begin{aligned}
u_x &= u_y = 0\\
u_z(z) &= \frac{1+v}{1-v}\alpha (T_1 - T_0)z + \bigg( 1- \frac{2v^2}{1-v}\bigg) \frac{1}{E}(p_s - p_f) (-g) \bigg( zH - \frac{1}{2}z^2\bigg)
\end{aligned}
\end{equation}

Since only the strain and stress in the vertical direction are required only those are defined in the input file, here in the **`[AuxKernels]`** for the z-direction with the **index_i** and **index_y** = 2

```
    [AuxKernels]
      [./strain_zz]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_zz
        index_i = 2
        index_j = 2
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

The **`[BCs]`** defines the immediate temperature change in the beginning of the simulation as a **type = DirichletBC** which is located at the whole domain (**boundary = 'top bottom left right front back'**)

```
    [BCs]
      [./pf_top]
        type = DirichletBC
        variable = pore_pressure
        boundary = front
        value = 0.0
      [../]
      [./T_domain]
        type = DirichletBC
        variable = temperature
        boundary = 'top bottom left right front back'
        value = 2.5
      [../]
      [./no_x]
        type = PresetBC
        variable = disp_x
        boundary = 'left right'
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

!media media/examples/test_suite/THM1_plot.png
       caption=Displacement
       style=width:50%;

