# Benchmark 4

## Description

Coupled fluid flow and heat transfer

This example refers to the following publication: [cite:jacquey2017]

In this example the synthetic model contains two intersecting planar faults embedded in a two-layered porous medium. The horizontal extension of the domain is 200 m while the vertical is 100 m (see graphic below).

!media media/examples/benchmarks/model_geometry.png
       caption=The domain contains two intersecting faults in a two-layered porous matrix. Three observation points are marked for later illustration of the results.
       style=width:50%;

The model simulates infiltration of water from the southern border into the model domain and lets the inflowing water advect as a non-isothermal front fixed along the same boundary. As initial conditions, the pore pressure in the center point of the model (z=0) is set to $1.5\,MPa$ and a hydrostatic pore pressure distribution is assumed for the rest of the model. The temperature distribution is set to be homogeneous at $60 \degree C$.

!media media/examples/benchmarks/boundary_conditions.png
       caption=Boundary conditions of the problem: left the fixed pore pressure boundary on the southern and northern border. Right the fixed temperature distribution.
       style=width:100%;

To impose a fluid flow in the model a pore pressure gradient of $\Delta p = 0.5\,MPa$ is imposed by pore pressure boundary conditions on the southern and northern boundary. Across the southern boundary a linear distribution of temperature from $40 \degree C$ to $80 \degree C$ is applied. The fluid is considered incompressible.
The properties of the fluid, the faults and the two layers are listed below:

|Property name             |SI Unit           | Layer 1             | Layer 2             | Fault 1             | Fault 2             |
|--------------------------|------------------|---------------------|---------------------|---------------------|---------------------|
|Average thickness/Aperture|$m$              |$55$                |$45$                |$0.05$              |$0.05$              |
|Porosity                  |$-$              |$0.15$              |$0.08$              |$1$                 |$1$                 |
|Fluid density             |$kg m^{-3}$      |$1000$               |$1000$               |$1000$               |$1000$               |
|Solid density             |$kg m^{-3}$      |$2600$              |$2600$              |$-$                 |$-$                 |
|Storage coefficient       |$Pa^{-1}$        |$7.0 \times 10^{-10}$|$7.0 \times 10^{-10}$|$4.6 \times 10^{-10}$|$4.6 \times 10^{-10}$|
|Permeability              |$m^2$            |$2.0 \times 10^{-14}$|$1.0 \times 10^{-14}$|$1.0 \times 10^{-8}$ |$1.0 \times 10^{-8}$ |
|Fluid viscosity           |$Pa s$           |$5.0 \times 10^{-4}$ |$5.0 \times 10^{-4}$ |$5.0 \times 10^{-4}$ |$5.0 \times 10^{-4}$ |
|Fluid thermal conductivity|$W m^{-1} K^{-1}$ |$0.65$              |$0.65$              |$0.65$              |$0.65$              |
|Solid thermal conductivity|$W m^{-1} K^{-1}$ |$3.0$               |$3.0$               |$-$                 |$-$                 |
|Fluid heat capacity       |$J kg^{-1} K^{-1}$|$4180$              |$4180$              |$4180$              |$4180$              |
|Solid heat capacity       |$J kg^{-1} K^{-1}$|$1000$              |$1000$              |$-$                 |$-$                 |

IAPWS is the International Association for the Properties of Water and Steam, http://www.iapws.org/.

## Results

In the figure below the results of the simulation are shown: Pore pressure distribution on the left, the temperature distribution on the right. It is visible how the two faults influence the distribution of pore pressure and temperature, mainly due to their higher permeability thus allowing pressure compensation and advective heat flow due to the higher fluid flow.

!media media/examples/benchmarks/results_example1.png
       caption=Illustration of the results at the end of the simulation (150 years). On the left the pore pressure distribution at the cross section area. On the right side the temperature isosurfaces.
       style=width:100%;

The graphics below show the pore pressure at the three observation points plotted over the whole simulation time as well as the temperature at those observation points. The evolution of the temperature corresponds to the propagation of the temperature front due to dispersion and advection of the heat. This graphic illustrates how the fractures with their higher permeability have a strong impact on the heat propagation: Fault 1 extends closer to the heat boundary of the southern border thus allowing the fluid to reach it first. Due to a high fluid flow in the fracture the cold water front reaches the observation point by the same time as observation point 2 is reached by the temperature front. The pressure in the model changes within the first hour of simulation time due to the boundary conditions. The values of the pore pressure at the observation points have a relatively small difference, compared to the pressure gradient imposed by the boundary conditions. This is due to the good hydraulic communication between all points within the high permeability fractures. The high permeability also explains the decreasing values of the pore pressure at the same time.

!media media/examples/benchmarks/benchmark4_fig4.png
       caption=Evolutions of (left) pore pressure and (right) temperature at the three observation points whose locations are illustrated in Figure 1 obtained with GOLEM, OpenGeoSys or FEFLOW.
       style=width:100%;

!bibtex bibliography
