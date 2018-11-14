# Test TM 4

## Description

An elastic cuboid undergoes load due to gravity and instant temperature change

A cuboid with a height of $H = 30\,m$ is investigated.

The solid material has density $\rho = 2038.736\, kg/m^3$, a Poisson's ratio $v = 0.25$, a Young's modulus $E = 5000\,Pa$, zero heat capacity and a thermal expansion of $\alpha = 5 \cdot 10^{-6} \, 1/K$. Gravity is applied in negative z-direction.
The bottom and lateral faces are sliding planes, the top face is free. Simulation start with $T_0 = 10 \degree C$  and uses one time step to instantly change the temperature  to $T_1 = 4 \degree C$ at top of the domain.

The steady state is described by the governing Laplace equation:

\begin{equation}
\frac{d^2 T}{dx^2} =0
\end{equation}

Temperature distribution along the z-axis is given by:

\begin{equation}
T(z) = (T_1 - T_0) \frac{z}{H} + T_0
\end{equation}

The equation of the mechanical equilibrium, is satisfied by zero shear if the horizontal stress $\sigma_{11}$ and $\sigma_{22}$ are functions of $z$ only:

\begin{equation}
0 = \nabla \cdot \sigma - (0,0, p\,g)
\end{equation}

the vertical stress $\sigma_{33}$ is:

\begin{equation}
\frac{d \sigma_{33}}{dz} = p\,g
\end{equation}

The face $z=H$ is free, integration gives:

\begin{equation}
\sigma_{33} = p(-g)(H-z)
\end{equation}

there is no horizontal displacement anywhere:

\begin{equation}
\epsilon_{11} = \epsilon_{22} = 0
\end{equation}

with axes equal to coordinate axes, the constitutive gives:

\begin{equation}
\begin{aligned}
\epsilon_{11} - \alpha (T(z) -T_0) &= \frac{1}{E}[\sigma_{11} - v(\sigma_{22}+\sigma_{33})]\\
\epsilon_{22} - \alpha (T(z) -T_0) &= \frac{1}{E}[\sigma_{22} - v(\sigma_{11}+\sigma_{33})]\\
\epsilon_{33} - \alpha (T(z) -T_0) &= \frac{1}{E}[\sigma_{33} - v(\sigma_{11}+\sigma_{22})]
\end{aligned}
\end{equation}

Solving for $\sigma_{11}$,$\sigma_{22}$ and $\epsilon{33}$ gives:

\begin{equation}
\begin{aligned}
\sigma_{11} &= \sigma_{22} = -\alpha \frac{E}{1-v}(T_1 - T_0) \frac{z}{H}+\frac{v}{1-v}p (-g)(H-z)\\
\epsilon_{33} &= \frac{1+v}{1-v} \alpha (T_1 - T_0) \frac{z}{H} + \bigg( 1- \frac{2v^2}{1-v} \bigg) \frac{1}{E}p (-g)(H-z)
\end{aligned}
\end{equation}

Integrating the strains with respect to the fixities at the sliding planes gives the displacement vector $(u_x,u_y,u_z)$

\begin{equation}
\begin{aligned}
u_x &= u_y = 0\\
u_z(z) &= \frac{1+v}{1-v}\, \alpha (T_1 - T_0) \frac{z^2}{2H} + \bigg( 1 - \frac{2v^2}{1-v} \bigg) \frac{1}{E}p(-g)\bigg(zH - \frac{1}{2}z^2 \bigg)
\end{aligned}
\end{equation}

## Input file

While normally even for 1D or 2D problems a 3D mesh is generated or loaded, the same applies to this 3D problem.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 2
      ny = 2
      nz = 10
      xmin = 0
      xmax = 6
      ymin = 0
      ymax = 6
      zmin = 0
      zmax = 30
    []
```

if an object or material is affected by gravity is adjusted in the **`[Materials]`** part of the input file. With **`[has_gravity]`** the gravitation will be recognized and with **`[gravity_acceleration]`** the actual value can be modified.

```
    [Materials]
      [./TMMaterial]
        type = GolemMaterialMElastic
        block = 0
        strain_model = incr_small_strain
        has_gravity = true
        gravity_acceleration = 9.81
        density_solid_initial = 2038.736
        young_modulus = 5.0e+09
        poisson_ratio = 0.25
        thermal_expansion_solid = 1.5e-05
        thermal_conductivity_fluid_initial = 1.0
        thermal_conductivity_solid_initial_ = 1.0
        porosity_uo = porosity
        fluid_density_uo = fluid_density
      [../]
    []
```

## Analytical Plot

!media media/examples/test_suite/TM4_plot.png
       caption= Thermomechanic deformation
       style=width:50%;
