# Application 5

## Description

This example refers to the following publication: [cite:cacace2017]

A non linear convective flow problem after [cite:elder1967] is considered. In this problem, the flow which develops when a portion of the base of a porous, homogeneous, isotropic and saturated medium is suddenly heated is simulated. The fluid density and viscosity are a function of the fluid temperature and pressure. The model domain is a two-dimensional, rectangular plane. The side lengths are dimensionless with a ratio of 1:4, in this case 150 units of height to 600 units of length.

Additional information can be found in the literature:
[cite:elder1967]

The material properties can be found in the table below:

|             | Property name         | Value                |SI Unit        |
|-------------|-----------------------|----------------------|---------------|
|Rock         | Permeability          | $10^{-11}$          | $m^2$         |
|             | Porosity              | $0.04$               | -             |
|             | Density               | $2480$               | $kg \, m^{3}$|
|             | Specific heat capacity| $920$                | $kg/J/K$      |
|             | Thermal conductivity  | $1.233$              | $W/m/K$       |
|Fluid        | Density               | IAPWS                | $kg \, m^{-3}$|
|             | Viscosity             | IAPWS                | $Pa \, s$     |
|             | Specific heat capacity| $4180$               | $J/kg/K$      |
|             | Thermal conductivity  | $0.65$               | $W/m/K$       |

IAPWS is the International Association for the Properties of Water and Steam, http://www.iapws.org/.

## Results

Due to the temporal evolution in fluid property gradients, thermal bouyant forces develop thus giving rise to an unstable convective flow regime as illustrated in the graphic below.

!media media/examples/benchmarks/results1_example4.png
       caption=Results for the non linear convective flow
       style=width:100%;

!bibtex bibliography