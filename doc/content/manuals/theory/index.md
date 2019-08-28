# Theory manual

## Introduction

This text refers to the publication [!cite](cacace2017).

In the following the main governing equations are described. The main variables solved for in the kernels are the pore fluid pressure $p_f$, the temperature $T$ and the solid displacement vector $u$. These are the primary variables and all other variables, such as stress and strain for example, are auxiliary variables and correspond directly to these primary variables. Pore pressure is defined as compression positive for water, while stress is defined as tension positive for the solid phase.


## Pore pressure

The equation governing the pore pressure reads
\begin{equation}
\underbrace{\frac{1}{M_b} \frac{\partial p_f}{\partial t}}_{GolemKernelTimeH} - \underbrace{\alpha \beta_b \frac{\partial T}{\partial t} + \alpha \dot{\epsilon}_{kk} + (1 -\alpha) \dot{\epsilon}_{kk}^{\star p}}_{GolemKernelHPoroElastic} + \underbrace{\nabla \cdot q_D}_{GolemKernelH} = 0.
\end{equation}

The first term $\frac{1}{M_b} \frac{\partial p_f}{\partial t}$ refers to the specific storage of the porous medium according to Biot's theory. The second term $\alpha \beta_b \frac{\partial T}{\partial t}$ takes into account the thermal expansion of the bulk volume (solid and fluid phases) due to a temperature change. The terms $\alpha \dot{\epsilon}_{kk}$ and $+ (1 -\alpha) \dot{\epsilon}_{kk}^{\star p}$ take into account the strain rates: firstly the total strain and secondly the plastic strain rate, respectively. Both strains need to be corrected by the Biot coefficient $\alpha$. The last term $\nabla \cdot q_D$ refers to the gradient of the Darcy flux.

## Mechanics

The equation governing the mechanics is given by the stress tensor:
\begin{equation}
\underbrace{\nabla \cdot \left(\sigma^{\prime} - \alpha p_f \mathbb{I}\right) + \rho_b g}_{GolemKernelM} = 0.
\end{equation}


where $\mathbb{I}$ is the rank-two identity tensor, $\rho_b$ is the bulk density of the fluid–solid mixture. The last term $\rho_b g$ corresponds to the static stress due to overburden rock formations. In the first term, $\alpha = 1 - \frac{K}{K_s}$ is the Biot coefficient (with $K$ being the drained bulk modulus and $K_s$ the bulk modulus of the solid grains) used to apply the effective stress law.

The strain-displacement relationship is given by:
\begin{equation}
\epsilon = \frac{1}{2} \left(\nabla u + \nabla^{T} u \right) = \nabla ^{S} u.
\end{equation}

The strain - elastic stress relationship is given by:
\begin{equation}
\dot{\sigma}^{\prime} = \dot{\sigma}^{\prime}_{ij} = \mathbb{C}_{ijkl} \dot{\epsilon}_{kl}^{e} =  \mathbb{C} : \dot{\epsilon}^{e}
\end{equation}


## Temperature

The equation governing the temperature reads
\begin{equation}
\underbrace{(\rho c)_b \frac{\partial T}{\partial t}}_{GolemKernelTimeT} + \underbrace{\nabla \cdot \left(\rho_f c_f q_D T\right)}_{GolemKernelTH} - \underbrace{\nabla \cdot \left(\lambda_b \nabla T\right) - \dot{H}}_{GolemKernelT} = 0.
\end{equation}

where the first term corresponds to the storage of heat energy in the fluid and solid phase. In the brackets there are two terms of which the fist describes advective heat flow proportional to the darcy flux and the second term describes the dispersion of heat along the termperature gradient. $(\rho c)_b = n \rho_f c_f + (1-n) \rho_s c_s$ is the bulk specific heat of the porous medium, $\lambda_b = n \lambda_f + (1-n) \lambda_s$ is the bulk thermal conductivity and $\dot{H}$ is the rate of the (heat) energy production.

!bibtex bibliography