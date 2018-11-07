# Application 3

## Description

This example refers to the following publication: Watanabe, Blöcher, Cacace Held and Kohl: Geoenergy Modeling 3, Enhanced Geothermal Systems, Springer Verlag, 2017.

In this example heat extraction from a hot, dry granite rock is simulated. The heat extraction is achieved by a fluid circulation in an artificial fracture in the granite layer. The target horizon is located in a depth of 4000 m with in situ temperatures of $200 \degree C$ thus resulting in an initial pore pressure of $10 \, MPa$ at the center of the domain. The fracture geometry is assumed to be square with an edge length of 300 m. To achieve a fluid circulation a doublet system with two wells in a distance of 150 m of each other and 75m to the edges of the fracture is installed. The production rate is set to $1 \, L/s$ and the injection temperature is set to $70 \degree C$ at which the water exists in a liquid phase due to the pressure.

!media media/examples/benchmarks/geometry_example2.png
       caption=Geometry of the domain. Edge length of the matrix cube is 500 m and the artificial fracture with a square geometry has a side length of 300 m.
       style=width:100%;

The injection and production point are located at a certain depth so that they are in the middle of the domain regarding the z-axis. The permeability of the fracture is determined by the cubic law and the fluid viscosity is regarded as temperature dependent, given by the following equation:

$ \mu = 2.393 \times 10^{\frac{248.37}{T \times 133.15}} \times 10^{-5}$

where T is the temperature in $\degree C$.

All further fluid and solid properties are listed in the table below:

|             | Property name         | Value                |SI Unit          |
|-------------|-----------------------|----------------------|-----------------|
|Fracture     | Edge length           | $300$                | $m$             |
|             | Mean aperture         | $1 \times 10^{-4}$   | $m$             |
|             | Permeability          | $8.3 \times 10^{-10}$| $m^2$           |
|             | Porosity              | $1$                  | -               |
|             | Storage               | $4 \times 10^{-10}$  | $Pa^{-1}$       |
|Rock         | Permeability          | $10^{-17}$           | $m^2$           |
|             | Porosity              | $0.01$               | -               |
|             | Density               | $2600$               | $kg m^{-3}$     |
|             | Specific heat capacity| $950$                | $Jkg^{-1}K^{-1}$|      
|             | Thermal conductivity  | $3.0$                | $Wm^{-1}K^{-1}$ |
|             | Storage               | $10^{-10}$           | $Pa^{-1}$       |
|Fluid        | Density               | IAPWS                | $kg m^{-3}$     |
|             | Viscosity             | IAPWS                | $Pa  s$         |
|             | Specific heat capacity| $4200$               | $Jkg^{-1}K^{-1}$|
|             | Thermal conductivity  | $0.65$               | $Wm^{-1}K^{-1}$ |   

IAPWS is the International Association for the Properties of Water and Steam, http://www.iapws.org/.

## Results
The graphics show the pressure and the fluid velocity distribution after 30 years of production.

!media media/examples/benchmarks/results2_example2.png
       caption=Pore pressure distribution after the pressure reached a steady state. View along the z-axis on a slice on the x-y-plane through the production and injection point (Left). Illustrated on the right side is the fluid velocity along the x-axis, marked by an arrow along the intersection between the x-y- and the x-z-planes.
       style=width:50%;
