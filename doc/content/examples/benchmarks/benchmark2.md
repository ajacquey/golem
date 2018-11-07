# Benchmark 2

## Description

Hydraulic processes in the presence of discrete fractures - flow in a fractured porous medium

This example refers to the following publication: [cite:cacace2017]

Consider a three dimensional quadrilateral box ($10 \times 10 \times 1 \, m $ in the $x, y$ and $z$ dimension respectively) representing a porous medium. A fracture is implemented to be of $1 \, m$ height thus cutting the box along the entire vertical. It has a finite horizontal length of $L = 2 \, m$ with an inclination of $\alpha = 45 \degree$ with respect to the model boundaries (see Figure below).

!media media/examples/benchmarks/threeD_model_box.png
       caption=Geometry and boundary conditions for the benchmark case of groundwater flow in a fractured porous medium.
       style=width:50%;

A specific constant pressure gradient along the $x$ axis of $\Delta p = 1 MPa$ between the left and right boundary inside the domain is imposed by constant pressure boundaries to ensure uniform flow and a constant discharge. No flow conditions are imposed on the remaining two model boundaries. The value of the pressure gradient has been chosen to match an analytical solution derived from a two dimensional approach. The analytical complex potential flow can be described by the following equation given by [cite:strack1982] and [cite:watanabe2011]:

\begin{equation}
\Omega = -A \sqrt{(Z-1)(Z+1)} + AZ - \frac{1}{2}q_0 L e^{i \alpha} Z
\end{equation}

where $q_0$ is the magnitude of the flow occurring within the model domain (assumed uniform), Z is a fracture related dimensionless variable and A is a scaling factor.
Laminar flow in the fracture plane is assumed and pressure variations across its width are neglected. Further parameters are listed in the table below.

|               | Property name  | Symbol        | Value              | SI unit   |
|---------------|----------------|:-------------:|-------------------:|-----------|
| Fracture      | Angle          | $\alpha$      | $45$                 | $\degree$ |
|               | Length         | $L$            | $2$                  | $m$         |
|               | Aperture       | $h$             | $0.05$               | $m$         |
|               | permeability   | $k^f$         |$1.0 \cdot 10{-10}$ | $m^2$     |
|               | Viscosity      | $\mu_{f}^{f}$   |$1.0 \cdot 10^{-3}$ | $Pa s$      |
| Porous medium | Porosity       | $n$             | $0.15$               | -         |
|               | Permeability   | $k$             |$1.0 \cdot 10^{-12}$| $m^2$     |
|               | Viscosity      | $\mu_f$       | $1.0 \cdot 10^{-3}$| $Pa s$      |

In the following illustration the pore pressure gradient is plotted along a line from the bottom left corner to the top right corner. Between 6 and 8 meters the profile follows exactly the orientation of the fracture.

!media media/examples/benchmarks/pressure_gradient.png
       caption= Isolines of pressure computed from the 3-D numerical simulation extracted along a horizontal plane cutting the model domain. Comparison between simulated (continuous red curve) and analytical derived (empty black circles) pressure distribution along a line through the model.
       style=width:100%;

The figure shows the distortion of the uniform horizontal flow in the vicinity of the fracture resulting in a preferred and faster flow along (and parallel to) the fracture plane. The plot shows a very good fit of the results of the numerical simulation and the analytic result thus indicating the applicability of the simulation in case of the discrete fracture approach.

!bibtex bibliography