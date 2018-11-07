# Benchmark 1

## Description

Thermal and hydraulic processes - heat transport in a fracture.

This example refers to the following publication: [cite:cacace2017]

Given a homogenous and isotropic fracture fully saturated with water ($n=1$), groundwater flow is assumed to be unidirectional with a constant average velocity throughout the length of the flow field. The initial temperature is set to zero. At the time $t = 0$ a sudden increase of temperature is imposed at along the inlet boundary so that $T = T_0$. The increase itself can be described by the Heaviside function. With these assumptions an analytical solution to the temperature distribution is given by [cite:ogata1961]:

\begin{equation}
T(x,t) = \frac{T_0}{2} erfc(\frac{x - v_x t}{\sqrt{4kt}}) + \frac{T_0}{2} exp(\frac{v_x x}{k}) erfc(\frac{x + v_x  t}{\sqrt{4kt}})
\end{equation}

For the numerical solution a fracture length of $L = 100m$, discretized into 1000 line elements of equal length is considered. A pressure gradient of

\begin{equation}
\nabla p_f = 3.0 \cdot 10^{-7} ms^{-1}
\end{equation}

Material properties are listed in the table below.

| Property name              |     Symbol    |                  Value | SI Unit             |
|----------------------------|:-------------:|-----------------------:|---------------------|
| Fluid density              |   $\rho_f$  | $1000$               | $kg \, m^{-3}$    |
| Fluid thermal conductivity | $\lambda_f$ | $0.65$               | $W m^{-1} K^{-1}$ |
| Fluid heat capacity        |    $c_f$    | $4000$               | $J \, kg^{-1} K{-1}$ |
| Fluid viscosity            | $\mu_f$     | $1.0 \cdot 10^{-3}$  | $Pa s$         |
| Fluid permeability         | $k$         | $1.0 \cdot 10^{-10}$ | $m^2$             |

## Solution

Given these parameters a constant fluid velocity of $v_x = 3.0 \cdot 10^{-7} \, ms^{-1}$ is obtained. Initial conditions for pressure and temperature were $p_f = 0.1 \, MPa$ and $T = 0 \degree C$. In this benchmark the evolution of the temperature field obtained by the numerical solution is compared with the analytical solution (given by the equations above). To compare the results, the temperature evolution according to analytical and numerical results  are plotted at four distinct points along the length of the fractured.

!media media/examples/benchmarks/heat_transport_fracture_comparison.png
       caption=Comparison of numerical results (red curves) and analytical solutions (black circles) for heat transport in a fracture. The figure shows the temperature evolution at different positions along the fracture as a function of time.
       style=width:50%;

It can be stated that in this case there is a generally good fit in terms of the temporal evolution of the advective front at all locations of the frature plane. The modelled thermal front moves slightly faster than the analytical solution predicts.

!bibtex bibliography