# Application 4
## Description

This example refers to the following publication: Watanabe, Blöcher, Cacace Held and Kohl: Geoenergy Modeling 3, Enhanced Geothermal Systems, Springer Verlag, 2017.

In this example energy production through an enhanced geothermal energy system is simulated. The target horizon lies in a depth of about 4 km resulting in a reservoir pressure of $10 \, MPa$ and has a sandstone lithology with porosities of 10% and permeabilities of $10 \times 10^{-15} \,m^2$. The Sandstone is entirely saturated with fluid of a temperature of $150 \degree C$. The installed doublet system comprises two parallel hydraulic fractures in a distance of 200 m between each other and square geometries with 100 m side lengths with production and injection points lying in the middle of each fracture. The injection temperature is set to $70 \degree C$ while the production and injection rate is set to $3 \, L/s$. The whole model has a cubic geometry with a cube side length of 500 m (see figure below).

!media media/examples/benchmarks/model_geometry_example3.png
       caption=Geometry of the domain. Edge length of the matrix cube is 500m and the artificial fractures have a square geometry with side lengths of 100 m.
       style=width:100%;

Listed in the table below there are further properties used for this simulation:

|             | Property name         | Value                |SI Unit        |
|-------------|-----------------------|----------------------|---------------|
|Fracture     | Edge length           | $100$                | $m$           |
|             | Mean aperture         | $ 1 \times 10^{-4} $ | $m$           |
|             | Permeability          | $8.3 \times 10^{-10}$| $m^2$         |
|             | Porosity              | $1$                  | -             |
|             | Storage               | $4 \times 10^{-10}$  | $Pa^{-1} $    |
|Rock         | Permeability          | $10^{-15} $          | $m^2$         |
|             | Porosity              | $0,01$               | -             |
|             | Density               | $2600$               | $kg \, m^{3}$|
|             | Specific heat capacity| $950$                | $kg/J/K$      |
|             | Thermal conductivity  | $3.0$                | $W/m/K$       |
|             | Storage               | $10^{-10}$           | $Pa^{-1}$     |
|Fluid        | Density               | $IAPWS$              | $kg \, m^{-3}$|
|             | Viscosity             | $IAPWS$              | $Pa \, s$     |
|             | Specific heat capacity| $4200$               | $J/kg/K$      |
|             | Thermal conductivity  | $0.65$               | $W/m/K$       |

$IAPWS$ is the International Association for the Properties of Water and Steam, http://www.iapws.org/.

## Results

The results show how the cold water bubble expands from the injection well and how the cold water front moves advectively with the fluid flow towards the production well. After 30 years of production the cold water front has not yet quiet reached the production well or the fracture at the production side.

!media media/examples/benchmarks/results1_example3.png
       caption=Results of the simulation after 60 years simulation time.
       style=width:100%;

In the following graphic the fluid flow is illustrated using the stream line tracer in Paraview. The production well on the right side produces water that stems from reservoir fluid (coming from the right side) and fluid that has been injected in the left well. Thus the production temperature is expected to be dropping.

!media media/examples/benchmarks/results2_example3.png
       caption=Stream lines of the fluid flow after 60 years of simulation time. The colouring shows the temperature of the fluid along the stream lines.
       style=width:100%;
