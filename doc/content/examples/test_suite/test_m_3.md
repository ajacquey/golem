# Test M 3
##Description

An elastic cuboid undergoes load due to gravity

A cuboid with the height $H = 30\,m$ is investigated under the load of gravity. The material of the cuboid is made from four different elastic materials. They all share the same density $\rho = 3058.104\,kg/m^3$, Poisson's ratio $v = 0.25$ and a Young's modulus of $E = 10,000\,MPa$. The bottom and lateral faces are sliding planes while the top face is free. The only load applied is the gravity with $g = 9.81\,m/s^2$.

With $\sigma$ for the stress, the equation of mechanical equilibrium is satisfied:

\begin{equation}
0 =\nabla \cdot \sigma - (0,0,\rho g)
\end{equation}

if the horizontal stress $\sigma_{11}$ and $\sigma_{22}$ are only functions of the vertical coordinate z and the vertical stress $\sigma_{33}$ is satisfied with:

\begin{equation}
\frac{\partial \sigma_{33}}{\partial z} = \rho g
\end{equation}

Since the face $z = H$ integration gives:

\begin{equation}
\sigma_{33} = \rho (-g) (H-z)
\end{equation}

with no horizontal displacement the horizontal strain is:

\begin{equation}
\epsilon_{11} = \epsilon_{22} = 0
\end{equation}

Hooke's law gives with the axes equal to the coordinate system:

\begin{equation}
\begin{aligned}
0& =& \sigma_{11} - v\,(\sigma_{22} + \sigma_{33})\\
0& =& \sigma_{22} - v\,(\sigma_{11} + \sigma_{33})\\
E\,\epsilon_{33}& =& \sigma_{33} - v\,(\sigma_{11} + \sigma_{22})
\end{aligned}
\end{equation}

solving for $\sigma_{11}$,$\sigma_{22}$ and the vertical strain $\epsilon_{33}$:

\begin{equation}
\begin{aligned}
\sigma_{11} &= \sigma_{22} = \frac{v}{1-v}\sigma_{33} = \frac{v}{1-v}\rho (-g)(H-z)\\
\epsilon_{33} &= \frac{1}{E} \left(1- \frac{2v^2}{1-v}\right) \rho (-g) (H - z)
\end{aligned}
\end{equation}

Integrating the strains with respect to the prescribed fixities yields the displacement vector ($u_x, u_y, u_z$):

\begin{equation}
\begin{aligned}
u_x &= u_y = 0\\
u_z(z) &= \frac{1}{E} \left(1- \frac{2v^2}{1-v}\right) \rho (-g) \left(Hz -\frac{1}{2}z^2 \right)
\end{aligned}
\end{equation}


## Input file

**`[AuxKernels]`** takes the output of the **`[Materials]`** to calculate the stress and strain, this time in the zz direction. as a result the **index_i** and **index_j** are both 2.

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


**`[Materials]`** defines besides the proportions of the material also the gravity. **has_gravity = true** activates the consideration of gravity while **gravity_acceleration** gives the value of the gravity.

```
    [Materials]
      [./MMaterial]
        type = GolemMaterialMElastic
        block = 0
        has_gravity = true
        density_solid_initial = 3058.104
        gravity_acceleration = 9.81
        strain_model = small_strain
        young_modulus = 10.0e+09
        poisson_ratio = 0.25
        porosity_uo = porosity
        fluid_density_uo = fluid_density
      [../]
    []
```


## Analytical Plot

!media media/examples/test_suite/M3_plot.png
       caption=Vertical displacement
       style=width:50%;

