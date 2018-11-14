# Application 1

## Description

Thermal and hydraulic processes - prototype of multi-fractured geothermal reservoir. This case refers to the application studied in the publication by [cite:cacace2017].

This example comprises a typical geothermal reservoir application. Injection and production of cold and, respectively, warm fluid is simulated for a facility life time of approximately 100 years. The model consists of four different geological formations of which the middle ones represent the reservoir while the overlying unit as well as the underlying bed act as cap rocks. The extent of the model  domain is $10 \times 10 \times 3\,km$ in the $x, y,$ and $z$ directions and the target reservoir is located at a depth of about 4.6 km sub sea. The model also shows a natural fault showing a slip of some hundred meters in the reservoir. The injection and production well are kept parallel and horizontal with open hole sections of approximately 1.5 km in the target reservoir. The open hole sections are modeled as 1D finite elements and the surface area of the well bore is considered as scaling parameter. It is assumed that a hydraulic stimulation has been carried out resulting in a multifrac system of ten fractures with a spacing of 100 m in between enhancing the hydraulic connection between the two wells along their horizontal sections. All fractures share the same properties, listed in the table below. The figure below (panel A, top graphic) shows the model domain with fracs and the wells.

!media media/examples/applications/Figure_7a.jpg
       caption=Problem formulation and results of the prototype of multifractured geothermal reservoir. Left panel shows the geometry and setup of the simulation and right panel the distribution of the temperature after approximately 100 years of production.
       style=width:100%;

The boundary conditions adopted for the simulation include a constant temperature of $T = 137.5 \degree C$ fixed along the top of the reservoir, a fixed heat flow of $0.072\,W m^{-2}$ at the bottom of the reservoir as well as a fixed hydrostatic pressure along the lateral borders of the model. By running a steady state simulation a natural and balanced system can be modeled thus avoiding possible oscillations or inconsistency issues.
Operations are simulated by the injection of water at a temperature of $T = 55 \degree C$ and a fluid flow of $q_{in} = 30\,L s^{-1}$ for the entire duration of the simulation. Production rates are kept equal to those of the injection. The figure above (panel B, bottom graphic) illustrates the state of the reservoir at the end of the simulation.

## Material properties

Material properties of the simulation are:

|  Unit             | Property name             | Symbol      | Value                  | SI unit             |
|-------------------|---------------------------|-------------|------------------------|---------------------|
| Cap rock I        | Porosity                  | $n$         | $0.01$                 | -                   |
|                   | Permeability              | $k_x$       | $1 \cdot 10^{-20}$     | $m^2$               |
|                   |                           | $k_y$       | $1 \cdot 10^{-20}$     | $m^2$               |
|                   |                           | $k_z$       | $0.25 \cdot 10^{-20}$  | $m^2$               |
|                   | Fluid modulus             | $K_f$       | $1 \cdot 10^{8}$       | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Rock density              | $\rho_S$    | $2650$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $4$                    | $W m^{-1} K^{-1}$   |
|                   | Rock heat capacity        | $c_S$       | $920$                  | $J kg^{-1} K^{-1}$  |
|  Cap rock II      | Porosity                  | $n$         | $0.01$                 | -                   |
|                   | Permeability              | $k_x$       | $1 \cdot 10^{-20}$     | $m^2$               |
|                   |                           | $k_y$       | $1 \cdot 10^{-20}$     | $m^2$               |
|                   |                           | $k_z$       | $1 \cdot 10^{-20}$     | $m^2$               |
|                   | Fluid modulus             | $K_f$       | $1 \cdot 10^{8}$       | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Rock density              | $\rho_S$    | $2650$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $2.31$                 | $W m^{-1} K^{-1}$   |
|                   | Rock heat capacity        | $c_S$       | $1380$                 | $J \, kg^{-1} K^{-1}$|
|  Reservoir Unit I | Porosity                  | $n$         | $0.15$                 | -                   |
|                   | Permeability              | $k_x$       | $1.28 \cdot 10^{-15}$  | $m^2$               |
|                   |                           | $k_y$       | $1.28 \cdot 10^{-15}$  | $m^2$               |
|                   |                           | $k_z$       | $3.2 \cdot 10^{-16}$   | $m^2$               |
|                   | Fluid modulus             | $K_f$       | $1.5 \cdot 10^{9}$     | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Rock density              | $\rho_S$    | $2650$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $3.18$                 | $W m^{-1} K^{-1}$   |
|                   | Rock heat capacity        | $c_S$       | $920$                  | $J \, kg^{-1} K^{-1}$|
| Reservoir Unit II | Porosity                  | $n$         | $0.005$                | -                   |
|                   | Permeability              | $k_x$       | $9.87 \cdot 10^{-17}$  | $m^2$               |
|                   |                           | $k_y$       | $9.87 \cdot 10^{-17}$  | $m^2$               |
|                   |                           | $k_z$       | $2.4675 \cdot 10^{-17}$| $m^2$               |
|                   | Fluid modulus             | $K_f$       | $1 \cdot 10^{8}$       | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Rock density              | $\rho_S$    | $2650$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $2.31$                 | $W m^{-1} K^{-1}$   |
|                   | Rock heat capacity        | $c_S$       | $1380$                 | $J \, kg^{-1} K^{-1}$|
| Fault             | Porosity                  | $n$         | $1$                    | -                   |
|                   | Aperture                  | $h$         | $1 \cdot 10^{-2}$      | $m$                 |
|                   | Permeability              | $k_x$       | $1 \cdot 10^{-15}$     | $m^2$               |
|                   |                           | $k_y$       | $1 \cdot 10^{-15}$     | $m^2$               |
|                   |                           | $k_z$       | $1 \cdot 10^{-15}$     | $m^2$               |
|                   | Fluid modulus             | $K_f$       | $2.5 \cdot 10^{9}$     | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Fluid density             | $\rho_S$    | $1148$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $0.65$                 | $W m^{-1} K^{-1}$   |
|                   | Fluid heat capacity       | $c_S$       | $4193.5$               | $J \, kg^{-1} K^{-1}$|
| Fractures         | Porosity                  | $n$         | $1$                    | -                   |
|                   | Aperture                  | $h$         | $2.28 \cdot 10^{-4}$   | $m$                 |
|                   | Permeability              | $k_x$       | $4.33 \cdot 10^{-9}$   | $m^2$               |
|                   |                           | $k_y$       | $4.33 \cdot 10^{-9}$   | $m^2$               |
|                   |                           | $k_z$       | $4.33 \cdot 10^{-9}$   | $m^2$               |
|                   | Fluid modulus             | $K_f$       | $2.5 \cdot 10^{9}$     | $Pa$                |
|                   | Fluid viscosity           | $\mu_f$     | $3.0 \cdot 10^{-4}$    | $Pa \, s$           |
|                   | Fluid density             | $\rho_S$    | $1148$                 | $kg \, m^{-3}$      |
|                   | Rock thermal conductivity | $\lambda_S$ | $0.65$                 | $W m^{-1} K^{-1}$   |
|                   | Fluid heat capacity       | $c_S$       | $4193.5$               | $J \, kg^{-1} K^{-1}$|



## Results

!media media/examples/applications/Figure_7b.jpg
       caption=Results of the prototype of a multifractured geothermal reservoir: Temperature distribution after approximately 100 years of production.
       style=width:100%;

!bibtex bibliography
