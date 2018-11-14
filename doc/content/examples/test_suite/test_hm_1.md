# Test HM 1

## Description

A permeable elastic beam deforms under transient internal fluid pressure. Specified boundary conditions are time-dependent and of 1st kind

A rectangle beam with the length of $L = 1\,m$ investigated. The beam is made of a permeable material with a permeability $k = 10^{-10}\,m^2 $ and zero porosity. The fluid has a viscosity of $\mu = 1\,mPa\cdot s $. Gravity is neglected. The solid material has a Poisson's ratio $v = 0.2$ a Young's modulus $E = 27000\,Pa$ and a Biot number one.
The face $x = 0$ is free, while all other faces are sliding planes. Zero pressure is specified at the face $x = 0$, the pressure increases linearly with time $p_1 \cdot t$ with $(p_1 = 100\,Pa/s)$ and is applied at the face $x = L$ for $t>0$.
The simulation evaluates the transient pressure, stress, strain and displacement.

Using Biot's simplified theory, with $\sigma$ as the stress tensor and $I$ as the unit tensor, the equation of the mechanical equilibrium is:

\begin{equation}
0 = \nabla \cdot (\sigma - p I)
\end{equation}

and with the stress $\sigma_{22}$ and $\sigma_{33}$ being only functions of $x$ the horizontal stress $\sigma_{11}$ satisfies

\begin{equation}
\frac{\partial}{\partial x} (\sigma_{11} - p) = 0
\end{equation}

Introducing:

\begin{equation}
\chi = \frac{kE}{\mu} \bigg/ \bigg( 1 - \frac{2v^2}{1-v} \bigg)
\end{equation}

Applying the LaPlace transform with respect to $t$ yields the ordinary differential equation:

\begin{equation}
\chi \, \overline{p}'' - s \, \overline{p} = 0
\end{equation}
with $\overline{p}$ as the transform of $p$, $s$ is the transformation parameter and the prime denotes the derivative with respect to $x$.

the solution for $\overline{p}(x,s)$ is:

\begin{equation}
\overline{p}(x,s) = p_1 \frac{\sinh(\sqrt{s/ \chi}x)}{s^2 \sinh (\sqrt{s/ \chi}L)}
\end{equation}

the solution for the displacement reads:

\begin{equation}
\overline{u}_x(x,s) = \frac{p_1}{E} \bigg( 1- \frac{2v^2}{1-v} \bigg) \frac{\cosh (\sqrt{s / \chi}x) - \cosh (\sqrt{s / \chi}L)}{s^2 \sqrt{s / \chi} \sinh (\sqrt{s / \chi}L)}
\end{equation}

## Input file

**`[Mesh]`** loads a previously made mesh with the name **HM_1D_transient.msh** and assigns the boundary id from the mesh to new boundary names like left. The rest of the input file will then us these new names instead of the boundary id.

```
    [Mesh]
      type = FileMesh
      file = HM_1D_transient.msh
      boundary_id = '0 1 2 3 4 5'
      boundary_name = 'left right bottom top front back'
    []
```


**`[GlobalParams]`** needs to define the name of the displacement as well as the name of the pore_pressure. Even if the names are identical (pore_pressure), they need to be defined in order to be found by the program.

```
    [GlobalParams]
      displacements = 'disp_x disp_y disp_z'
      pore_pressure = pore_pressure
    []
```


The **`[Kernels]`** are chosen appropriate to the given problem and boundary conditions. The **MKernel** are for the displacement while the **GolemKernelHPoroElastic** is chosen because a  permeable elastic beam is the subject of the problem given. This kernel gives the response of the deformation on the pore pressure.

```
    [Kernels]
      [./HKernel]
        type = GolemKernelH
        variable = pore_pressure
      [../]
      [./HMKernel]
        type = GolemKernelHPoroElastic
        variable = pore_pressure
      [../]
      [./MKernel_x]
        type = GolemKernelM
        variable = disp_x
        component = 0
      [../]
      [./MKernel_y]
        type = GolemKernelM
        variable = disp_y
        component = 1
      [../]
      [./MKernel_z]
        type = GolemKernelM
        variable = disp_z
        component = 2
      [../]
    []
```

**`[AuxKernels]`** defines the stress and strain and which variable (defined in **`[AuxVariables]`**) is pointing in which direction. This being a rank two tensor the index for $i$ and $j$ for x are **0 0** , for y **1 1** and for z **2 2**.

```
    [AuxKernels]
      [./strain_xx]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_xx
        index_i = 0
        index_j = 0
      [../]
      [./stress_xx]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_xx
        index_i = 0
        index_j = 0
      [../]
      [./strain_yy]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_yy
        index_i = 1
        index_j = 1
          [../]
      [./stress_yy]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_yy
        index_i = 1
        index_j = 1
      [../]
      [./strain_zz]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_zz
        index_i = 2
        index_j = 2
      [../]
      [./stress_zz]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_zz
        index_i = 2
        index_j = 2
      [../]
```

**`[Functions]`** defines the linear functions which describes the increasing pore pressure over time.

```
    [Functions]
      [./func_p1_right]
        type = ParsedFunction
        value = 'p_rate*t'
        vars = 'p_rate'
        vals = '100'
      [../]
    []
```


## Analytical Plot

!media media/examples/test_suite/HM1_disp_x_plot.png
       caption=x displacement
       style=width:50%;

!media media/examples/test_suite/HM1_pore_pressure_plot.png        
       caption=pore pressure distribution
       style=width:50%;
