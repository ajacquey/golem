# Test M 1
## Description

An elastic beam undergoes axial load

The axial load of a beam is investigated. The material of the beam has a Poisson's ratio of $v = 0.25$ and a Young's modulus of $E = 10,000\,MPa$. The material has a zero density so that gravity can be neglected. The coordinate planes are sliding and the top and rear faces of the beam are free. A tensile stress of $\sigma_0 = 2\, MPa$ is applied at the $x = 1\,m$ face.
With $\sigma$ as the stress tensor:

\begin{equation}
\nabla \cdot \sigma = 0
\end{equation}

mechanical equilibrium is satisfied with zero shear and constant stress:

\begin{equation}
\begin{aligned}
\sigma_{11}& =& \sigma_0 & &\\
\sigma_{22}& =& \sigma_{33} &=& 0
\end{aligned}
\end{equation}

considering the principal axis used in Hooke's law to be equal to the coordinate axis:

\begin{equation}
\begin{aligned}
\epsilon_{11} &=& \frac{1}{E}[\sigma_{11} - v(\sigma_{22} + \sigma_{33})] &=& \frac{\sigma_0}{E}\\
\epsilon_{22} &=& \frac{1}{E}[\sigma_{22} - v(\sigma_{11} + \sigma_{33})] &=&-v \frac{\sigma_0}{E}\\
\epsilon_{33} &=& \frac{1}{E}[\sigma_{33} - v(\sigma_{11} + \sigma_{22})] &=&-v \frac{\sigma_0}{E}
\end{aligned}
\end{equation}

Integrating the strains gives the displacement vector:

\begin{equation}
\begin{aligned}
u_x(x)&=& \frac{\sigma_0}{E}x,\\
u_y(y)&=& -v\frac{\sigma_0}{E}y,\\
u_z(z)&=& -v\frac{\sigma_0}{E}z.
\end{aligned}
\end{equation}

## Input file

**`[Mesh]`** generates a mesh with three dimensions and with 20 Elements in the x direction and 2 Elements in the y and z direction.

```
    [Mesh]
      type = GeneratedMesh
      dim = 3
      nx = 20
      ny = 2
      nz = 2
      xmin = 0
      xmax = 1
      ymin = 0
      ymax = 0.1
      zmin = 0
      zmax = 0.1
    []
```

**`[GlobalParams]`** defines within the input file, that it will give **'disp_x disp_y disp_z'** when ever **displacement** is written. Both the **Kernel** and the **Material** will ask for a displacement vector. Since it is already defined here, it does not need to be defined again in the block of the **Kernel** or the **Material**.

```
    [GlobalParams]
      displacements = 'disp_x disp_y disp_z'
    []
```

**`[Kernels]`** will call the **GolemKernelM** Kernel which can handle the deformation in all three directions. However, the Kernel needs to know which variable equals which direction of the displacement. This is done with the **component**. The order of the variables is fixed, meaning **component = 0** is always the displacement in the x-direction, **component = 1** is the y-direction and **component = 2** is the z-direction. While the name of the variables can be chosen the sequence of the components is fixed.

```
    [Kernels]
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

**`[BCs]`** defines the boundary conditions. In this case there three boundary conditions of the first kind, which are defined with **type = PresetBC** and one boundary condition of the 2nd kind with **type = NeumannBC** and a **value = 2.0e+06**, which equals the tensile stress $\sigma_0 = 2M\,Pa$ at the right end on the beam (**boundary = right**).

```
    [BCs]
      [./no_x_left]
        type = PresetBC
        variable = disp_x
        boundary = left
        value = 0.0
      [../]
      [./load_x_right]
        type = NeumannBC
        variable = disp_x
        boundary = right
        value = 2.0e+06
      [../]
      [./no_y_bottom]
        type = PresetBC
        variable = disp_y
        boundary = bottom
        value = 0.0
      [../]
      [./no_z_back]
        type = PresetBC
        variable = disp_z
        boundary = back
        value = 0.0
      [../]
    []
```

**`[Materials]`** defines the necessary properties of the material as well as strain model which acts a basis for the calculation. In this case the **small_strain**:
\begin{equation}
\epsilon_{ij} = \frac{1}{2} \bigl( \frac{\partial u_i}{\partial j} + \frac{\partial u_j}{\partial i}  \bigr)
\end{equation}
The porosity and fluid density, which have to be defined are linked to the **`[UserObjects]`**.

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

**`[UserObjects]`** defines the necessary porosity and fluid density and chooses the suitable types, in this case both porosity and density are constant.

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

!media media/examples/test_suite/M1_plot.png 
       caption=Displacement in x direction
       style=width:50%;
