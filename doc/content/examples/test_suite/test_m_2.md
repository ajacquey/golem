# Test M2

## Description

An elastic plate undergoes simple shear

The stress of a rectangular plate (10m by 10m) is investigated. The material of the plate has a Poisson's ratio of $v = 0.25$ and a Young's modulus of $E = 10,000\,MPa$. The material has a zero density so that gravity can be neglected. The load is applied with a prescribed displacement which covers the entire surface. Two time steps are investigated with increasing deformation.

Employing the notation:

\begin{equation}
\frac{\partial u_x}{\partial x} + \frac{\partial u_y}{\partial y} + \frac{\partial u_z}{\partial z} = e
\end{equation}

The Navier equations read:

\begin{equation}
\begin{aligned}
\frac{\partial^2 u_x}{\partial x^2} + \frac{\partial^2 u_x}{\partial y^2} + \frac{\partial^2 u_x}{\partial z^2} + \frac{1}{1-2v} \frac{\partial e}{\partial x} & = & 0\\
\frac{\partial^2 u_y}{\partial x^2} + \frac{\partial^2 u_y}{\partial y^2} + \frac{\partial^2 u_y}{\partial z^2} + \frac{1}{1-2v} \frac{\partial e}{\partial y} & = & 0\\
\frac{\partial^2 u_z}{\partial x^2} + \frac{\partial^2 u_z}{\partial y^2} + \frac{\partial^2 u_z}{\partial z^2} + \frac{1}{1-2v} \frac{\partial e}{\partial z} & = & 0\\
\end{aligned}
\end{equation}

with a given slope $m$ the following boundary conditions apply:

\begin{equation}
\begin{aligned}
u_x (x,y,z) &=& 0&\qquad \text{on the entire surface},\\
u_y (x,y,z) &=& m x&\qquad \text{on the entire surface},\\
u_z (x,y,z) &=& 0&\qquad \text{on the entire surface},\\
\end{aligned}
\end{equation}

the only non-zero strain is:

\begin{equation}
\epsilon_{12} = \frac{\partial u_y}{\partial x} = m
\end{equation}

with Hooke's law the associated stress is:

\begin{equation}
\sigma_{12} = \frac{E}{2(1+v)}m
\end{equation}


## Input file

**`[Auxvariables]`** defines the variables for the **`[AuxKernels]`**. The variables are not specifically solved for, but serve as an input for the kernels. The names can be chosen by desire.

```
    [AuxVariables]
      [./strain_xy]
        order = CONSTANT
        family = MONOMIAL
      [../]
      [./stress_xy]
        order = CONSTANT
        family = MONOMIAL
      [../]
    []
```

**`[AuxKernels]`** returns the strain and stress based on the displacement. The values are valid for the spaces in between the calculated displacement points. **`[type = RankTwoAux]`** defines the Kernel for a tensor and **`[rank_two_tensor = mechanical_strain]`** chooses the suitable tensor given by the material (in this case: **`[GolemMaterialMElastic]`**). Since the data on which this kernel is based is given by the material, the name is also given by the material and can not be changed (hard coded). The same is valid for the stress. The **`[index_i]`** and **`[index_j]`** define the position in the tensor (from 0 to 2) and therefore in which direction the strain and/or stress is supposed to be calculated (i = 0 and j = 1 equals the direction xy)

```
    [AuxKernels]
      [./strain_xy]
        type = RankTwoAux
        rank_two_tensor = mechanical_strain
        variable = strain_xy
        index_i = 0
        index_j = 1
      [../]
      [./stress_xy]
        type = RankTwoAux
        rank_two_tensor = stress
        variable = stress_xy
        index_i = 0
        index_j = 1
      [../]
    []
```

**`[Materials]`** defines the material properties as well as the calculation for the strain and stress. It returns the **`[rank_two_tensor** **stress]`** and **`[mechanical_strain]`** which are used in the **`[AuxKernels]`**.

```
    [Materials]
      [./MMaterial]
        type = GolemMaterialMElastic
        block = 0
        strain_model = small_strain
        young_modulus = 10.0e+09
        poisson_ratio = 0.25
        porosity_uo = porosity
        fluid_density_uo = fluid_density
      [../]
    []
```

**`[UserObjects]`** defines the necessary porosity and fluid density and chooses the suitable types, in this case both porosity and density are constant. Both of this objects are needed in **`[Materials]`** as **`[porosity_uo]`** and **`[fluid_density]`**.

```
    [UserObjects]
      [./porosity]
        type = GolemPorosityConstant
      [../]
      [./fluid_density]
        type = GolemFluidDensityConstant
      [../]
    []
```

## Analytical Plot

  !media media/examples/test_suite/M2_plot.png
         caption=Shear stress in beam over time
         style=width:50%;
