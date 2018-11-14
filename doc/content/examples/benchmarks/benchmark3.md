# Benchmark 3

## Description
Mechanical processes: 3D elastoplastic oedometer test

This example refers to the following publication: [cite:cacace2017]

In the following test a cubic block of porous medium with edge length of 1 m is considered. This cube is subjected to axial loading with constant solid velocity $v_x$ under the conditions of an oedometer test. Undergoing continuous loading, the porous material behaves elastically until the strength of the material is reached and the behavior becomes plastic. The following graphic (left side) shows the setting of the simulation:

!media media/examples/benchmarks/oedometer_benchmark.png
       caption=Problem formulation and results of the oedometer benchmark. Panel (a, left) shows the problem formulation. Panel (b, right) illustrates the results for different dilation angles with the stress-displacement curves.
       style=width:100%;

## Solution

The Drucker-Prager plasticity model is adopted for the elastoplastic constitutive laws, which is a smoother version of the classical Mohr-Coulomb failure criterion. Under these conditions an analytical solution to the stress state of the cube can be given and used to compare the numerical results with. The physical properties of the porous material can be found in the table below:

|Property name    | Symbol | Value                | Unit       |
|-----------------|--------|----------------------|------------|
|Bulk modulus     | $K$    | $2 \times 10^{3}$    | $MPa$      |
|Shear modulus    | $G$    | $2 \times 10^{3}$    | $MPa$      |
|Cohesion         | $C$    | $1$                  | $MPa$      |
|Friction angle   | $\phi$ | $20$                 | $\degree$  |
|Dilation angle   | $\psi$ | $0$, $10$ or $20$    | $\degree$  |
|Velocity         | $v_x$  | $1.0 \times 10^{5}$  | $m s^{-1}$ |
|Edge of the cube | $L$    | $1$                  | $m$        |


The right side of the figure above shows the comparison between the numerical and analytical solution, in this example in very good agreement, wherein the red dots and line show an associative plastic potential and the green and blue dots and lines show a non-associative plastic potential (there the dilation angle differs from the friction angle).

## Inclusion of pore pressure


To promote the test further and to better quantify the internal coupling among the elastic and inelastic component of the deformation, additionally pore pressure coupling is included in the simulation thus solving for a hydraulic-mechanical (poroelasticity and plasticity) problem. The fluid parameters comprise a constant density of $\rho_f = 1000\,kg\,m^{-3}$, a viscosity of $\mu_f = 1.0 \times 10^{-3} Pa\,s$, a porosity of $n = 0.1$ and a permeability of $k = 1.0 \times 10^{-15} m^2$ and lastly a Biot coefficient of $\alpha = 0.6$. Typical undrained laboratory experimental conditions are simulated by setting the initial pore pressure to zero while setting the boundary conditions to be closed to pressure. The mechanical boundary conditions are not changed from the simulation without pore pressure coupling and comprise only the constant velocity of the deformation from the right side. The value of the velocity can be set to any constant as it does not have an influence on the deformation itself as the process is time independent. Inelastic behavior is introduced in the form of non-associated plasticity with a friction angle of $\phi = 20 \degree $ and a dilation angle of $\psi = 10 \degree$ (see figure below).

!media media/examples/benchmarks/modified_oedometer_test.png
       caption=Results of the modified oedometer test to account for pore pressure coupling. Panel (a, left) shows the evolution of pore pressure (diamonds) and stress (circle) considering either a purely elastic case (red colours) or additional plastic contributions (green colours). Panel (b, right) illustrates the evolution of the difference (elastic minus plastic).
       style=width:100%;

The figure shows how both stress and pore pressure increase as the loading from the right lateral boundary continues. The onset of the plastic behavior of the porous medium is indicated by the altered slope beyond a certain loading and affects the pore pressure distribution and evolution. The rates of pore pressure build up decreases this validating the poroelastic and plastic coupling as implemented.

!bibtex bibliography
