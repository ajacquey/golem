# Application 6

## Description

In this example a case inspired by an ATES (aquifer thermal energy storage) drilling project in Berlin, Germany, is studied. An shallow aquifer in the depth of $187\,m$ sub sea is targeted for ATES. In summer, when heat and electrical energy is available in abundance, hot water is injected in the aquifer for later recovery in winter, to be used for heat production. The aquifer is embedded between two low permeability units and there is no groundwater flow, which is reflected by the absence of a gradient between at least two of the boundaries.
Hot water is both injected and produced in the 'hot well' at $80 \degree C$, while the cold well, respectively, produces and injects cold water at $35 \degree C$. The two wells have a distance of $50\,m$ between each other, while the complete model comprises side lengths of $500 \times 500 \times 30 \,m$.

!media media/examples/benchmarks/ATES_domain.png
       caption= Simulation domain. The top and bottom unit (purple) have low permeabilities, while the middle unit is the target horizon. In blue and red are the cold and the hot well, respectively.
       style=width:50%;

|                | Property name         | Value                |SI Unit        |
|----------------|-----------------------|----------------------|---------------|
|Rock Top/Bottom | Permeability          | $10^{-13}$           | $m^2$         |
|                | Porosity              | $0.02$               | -             |
|                | Density               | $2650$               | $kg \, m^{3}$ |
|                | Specific heat capacity| $870$                | $kg/J/K$      |
|                | Thermal conductivity  | $1.5$                | $W/m/K$       |
|Target formation| Permeability          | $3.125 \times 10^{-4}$| $m^2$         |
|                | Porosity              | $0.02$               | -             |
|                | Density               | $2650$               | $kg \, m^{3}$ |
|                | Specific heat capacity| $800$                | $kg/J/K$      |
|                | Thermal conductivity  | $2.9$                | $W/m/K$       |
|Fluid           | Density               | $1000$               | $kg \, m^{-3}$|
|                | Viscosity             | $1.34 \times 10^{-3}$| $Pa \, s$     |
|                | Specific heat capacity| $4180$               | $J/kg/K$      |
|                | Thermal conductivity  | $0.579$              | $W/m/K$       |

## Results

The following graphics illustrate one cycle of the ATES in operation: First the hot fluid is injected into the reservoir. Later the fluid is extracted and the heat is used so the fluid cools down and is reinjected into the reservoir at the cold well. This leads to a cooling down of the reservoir:

!media media/examples/benchmarks/results_example5.png
       caption= Heat distribution after 170 days of hot fluid injection and after 230 days during the production of the previously injected hot water
       style=width:100%;

During the production a steady state regarding the pore pressure distribution is achieved (values in MPa), while the temperature distribution changes quite significantly due to advective heat transport during the loading and discharge of the reservoir: Time on the abscissa is given in seconds.

!media media/examples/benchmarks/Pore_Pressure_Example5.png
       caption= Pore pressure plotted over time showing steady state pressures during phases of injection and production.
       style=width:50%;

The Temperature plot at the production well shows five cycles of loading and production of heat. At the end of every cycle the Temperature drops as low as $35 \degree C$, which is the same Temperature as the injected fluid. Energy production is no longer possible at that point. However, the simulation shows how the target reservoir is heating over time with subsequently lower temperature drops at the end of each cycle: The reservoir conditions improve thus making the ATES working with higher efficiency.

!media media/examples/benchmarks/Temperature_Example5.png
       caption= Temperature plotted over time shows the loading and production of the storage unit.
       style=width:50%;
