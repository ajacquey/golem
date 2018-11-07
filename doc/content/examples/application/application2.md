# Application 2

## Description
Thermal and hydraulic and mechanical processes - thermo-poroelastic response of a synthetic geothermal doublet

This example refers to the following publication: Cacace and Jacquey, 2017: Flexible parallel implicit modelling of coupled thermal-hydraulic-mechanical processes in fractured rocks, Solid Earth, 8, 921-941.

A synthetic geothermal doublet with induced fractures within a low permeability formation is considered. The model aims to describing the thermo-poroelastic response of the reservoir due to geothermal operations. The extent of the model domain is $500 \times 500 \times 200 \,m$ in the $x, y, z$ directions. The depth of the target reservoir is approximatively 4 km below sea level. Pore pressure and temperature distributions are assumed to be homogeneous at the beginning of the simulation, and equal to $40\,MPa$ and $150 \degree C$ respectively. A regional stress field is applied as background stress, to simulate a normal faulting regime with the following magnitudes:

* Vertical stress $S_1 = 100 MPa$ in the $z$ direction.
* Maximum horizontal stress $S_2 = 90 MPa$ in the $y$ direction.
* Minimum horizontal stress $S_3 = 50 MPa$ in the $x$ direction.

!media media/examples/benchmarks/geometry_application2.png
       caption=Problem formulation and results of the thermo-poroelastic response of a geothermal doublet. Panel (a) shows the problem formulation. Panels (b) and (c) illustrate the pore pressure (with fluid velocity) and temperature distributions respectively after 50 years of operations.
       style=width:100%;

The reservoir properties are enhanced by two fractures that have been induced perpendicular to the minimum horizontal stress. Their geometry is square with 100 m side length and 200m in-between the two fractures. In this simulation non linear changes of the fluid (i.e. fluid density and viscosity) or rock properties (i.e. porosity and permeability) shall be considered.
Geothermal operations consist of water injection at $T_{in} = 70 \degree C$ and at a constant rate $q_{in} = 5\,L \, s^{-1}$ and producing geothermal fluid at the same rate. Pore pressure and temperature are kept constant on all sides of the model as boundary conditions. All faces are considered to be sliding faces for boundary conditions in terms of displacements. The simulation covers a complete time of 50 years of geothermal operations. The properties of the reservoir rock are listed in the table below.

|    Unit            | Property name              | Symbol         | Value                    | SI Unit              |
|--------------------|----------------------------|----------------|--------------------------|----------------------|
|    Reservoir rock  | Porosity                   | $ n $          | $ 0.1 $                  | -                    |
|                    | Permeability               | $ k $          | $ 1.0 \cdot 10^{-15} $   | $ m^2 $              |
|                    | Fluid modulus              | $ K_f $        | $ 1.0 \cdot 10^{8} $     | $ Pa $               |
|                    | Rock density               | $ \rho_S $     | $ 2600 $                 | $ kg\,m^{-1}   $      |
|                    | Rock thermal conductivity  | $ \lambda_S $  | $ 3 $                    | $ W m^{-1} K^{-1} $   |
|                    | Rock heat capacity         | $ c_S $        | $ 950 $                  | $ J \, kg^{-1} K^{-1} $ |
|   Fractures        | Porosity                   | $ n $          | $ 0.1 $                  | -                    |
|                    | Aperture                   | $ h $          | $ 1 \cdot 10^{-2} $      | $ m $                |
|                    | Permeability               | $ k $          | $ 8.333 \cdot 10^{-10} $ | $ m^2 $              |
|                    | Fluid modulus              | $ K_f $        | $ 2.5 \cdot 10^{9} $     | $ Pa $               |
|                    | Fluid density              | $ \rho_S $     | $IAPWS$                  | $ kg\,m^{-1}  $      |
|                    | Fluid thermal conductivity | $ \lambda_S $  | $ 0.65 $                 | $ W m^{-1} K^{-1} $   |

## Results

!media media/examples/benchmarks/results_application2.png
       caption=Horizontal slices of the model illustrating the changes in transport properties. Panel (a) shows the distribution of the changes in porosity and panel (b) the changes in permeability after 50 years of operations.
       style=width:100%;
