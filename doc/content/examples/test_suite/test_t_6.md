# Test T 6

## Discription

Transient 1D temperature distribution, time dependent boundary conditions of 1st kind

The temperature distribution of a beam from -L to L ($L = 50m$) with a thermal conductivity $\lambda = 0.5787037\, W/(m \cdot K)$, heat capacity $c = 0.01\, J/(kg\cdot K)$ and density $\rho = 2,500 \, kg/m^3$ is investigated. The temperature increases linearly with time $t$ ($T_1*t, \, T_1 = 2\, \degree C/d$) and is applied to the two ends of the beam with $t > 0$. The temperature is investigated after 0.25 days and 0.5 days.

The heat conduction equation is equation describing the transient temperature distribution:

\begin{equation}
 \rho c \frac{\partial T}{\partial t} = \lambda \nabla \cdot \nabla T
\end{equation}

with:

\begin{equation}
\chi = \frac{\lambda}{\rho c}
\end{equation}


The closed form solution of the problem is given by Carslaw and Jaeger (Citation from OGS Book, page 19+)

\begin{equation}
\begin{aligned}
T(x,t)& = T_1 \cdot t + \frac{T_1 \cdot (x^2 - L^2)}{2\chi} + \frac{16 \cdot T_1 \cdot L^2}{\chi \pi^3} \\
 & \times \sum_{n=0}^\infty \frac{(-1)^n}{(2n + 1)^3} \text{cos} \left( \frac{(2n + 1)\pi x}{2L}\right) \text{exp}\left(-\chi (2n + 1)^2 \pi^2 \frac{t}{4L^2}\right)
\end{aligned}
 \end{equation}

## Input file

**`[Mesh]`** generates a mesh with 200 elements **`[nx = 200]`** along the x-axis from -50 meter **`[xmin = -50]`** to 50 meters **`[xmax = 50]`**.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 200
      ny = 1
      nz = 1
      xmin = -50
      xmax = 50
      ymin = 0
      ymax = 0.5
      zmin = 0
      zmax = 0.5
    []
```

**`[Variables]`** is now given an extra **`[initial_condition = 0.0]`** for the temperature.

```
    [Variables]
      [./temperature]
        order = FIRST
        family = LAGRANGE
        initial_condition = 0.0
      [../]
    []
```

**`[Kernels]`** loads two Kernels for the temperature. **`[GolemKernelT]`** is responsible for the right part of the governing equation, while *GolemKernelTimeT* handles the left part of the equation.

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
    []
```

**`[Functions]`** describes the function for the linear increase of the temperature with **`[vals = '0.000023148]`** being the increase of temperature in $\degree$ Celsius per second.

```
    [Functions]
      [./T_time_func]
        type = ParsedFunction
        value = 'T1*t'
        vars = 'T1'
        vals = '0.000023148'
      [../]
    []
```

**`[Materials]`** gives the thermal conductivity of the solid, which is $\lambda = 0.5787037\, W/(m*K)$ as well as the heat  capacity with $c = 0.01\, J/(kg\cdot K)$. Since the density is also given and has to be defined **`[density_solid_initial = 2500]`** there has to be a UserObject for the density, in this case **`[fluid_density_uo = fluid_density]`** which is defined in the next block.

```
    [Materials]
      [./thermal]
        type = GolemMaterialT
        block = 0
        thermal_conductivit_initial = 1.0
        thermal_conductivity_solid_initial = 0.5787037
        heat_capacity_solid_initial = 0.01
        density_solid_initial = 2500
        porosity_uo = porosity
        fluid_density_uo = fluid_density
      [../]
    []
```

**`[UserObjects]`** defines the type of simulation for the different parameters. The density is defined with **`[type = GolemFluidDensityConstant]`** The chosen names have to fit the names given in the materials.

```
    [UserObjects]
      [./porosity]
        type = GolemPorosityConstant
      [../]
      [./fluid_density]
        type = GolemFluidDensityConstant
      [../]
    []
```

**`[Executioner]`** is given a start **`[start_time]`** and end **`[end_time]`** for the simulation as well as a distance **`[dt = 4320]`** for the time steps of the simulation. While the chosen time step of 4320 seconds is a lot shorter than the demanded time steps of 0.25 days (or 21600 seconds) it is necessary to keep the simulation stable.

```
    [Executioner]
      type = Transient
      solve_type = Newton
      start_time = 0.0
      end_time = 43200
      dt = 4320
    []
```

**`[Outputs]`** now has an interval assigned. Since the **`[Executioner]`** will return 10 time steps and only the 5th and last time step are required the interval of 5 will make the Output only contain the required time steps.

```
    [Outputs]
      interval = 5
      print_linear_residuals = true
      print_perf_log = true
      exodus = true
    []
```

## Analytical Plot

!media media/examples/test_suite/T6_plot_0_25_days.png
       caption=Temperature distribution after 0.25 days
       style=width:50%;

!media media/examples/test_suite/T6_plot_0_5_days.png
       caption=Temperature distribution after 0.25 days
       style=width:50%;
