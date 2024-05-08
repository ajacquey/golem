<h1 align="center">
  <br>
  <a href="https://github.com/ajacquey/Golem"><img src="images/golem_logo.png" alt="GOLEM" width="600"></a>
  <br>
  A MOOSE-based application
  <br>
</h1>

<h4 align="center">A numerical simulator for modelling coupled THM processes in faulted geothermal reservoirs based on <a href="http://mooseframework.org/" target="blank">MOOSE</a>.</h4>

<p align="center">
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/license-GPLv3-blue.svg"
         alt="GitHub License">
  </a>
  <a href="https://gitter.im/Golem-Moose/golem">
    <img src="https://img.shields.io/gitter/room/nwjs/nw.js.svg"
         alt="Gitter">
  </a>
  <a href="https://zenodo.org/record/999401#.Wc5NqBdx1pg">
    <img src="https://zenodo.org/badge/DOI/10.5281/zenodo.999401.svg"
         alt="DOI">
  </a>
</p>

## About
GOLEM is a numerical simulator for modelling coupled Thermo-Hydro-Mechanical processes in faulted geothermal reservoirs.
The simulator is developed by [Antoine Jacquey](http://www.gfz-potsdam.de/en/staff/antoine-jacquey/) <a href="https://orcid.org/0000-0002-6259-4305" target="orcid.widget" rel="noopener noreferrer" style="vertical-align:top;"><img src="https://orcid.org/sites/default/files/images/orcid_16x16.png" style="width:1em;margin-right:.5em;" alt="ORCID iD icon"></a><a href="https://github.com/ajacquey/" target="github.widget" rel="noopener noreferrer" style="vertical-align:top;"><img src="images/GitHub-Mark-32px.png" width="16" margin-right=".5em;" alt="GitHub icon id"></a> and [Mauro Cacace](http://www.gfz-potsdam.de/en/section/basin-modeling/staff/profil/mauro-cacace/) <a href="https://orcid.org/0000-0001-6101-9918" target="orcid.widget" rel="noopener noreferrer" style="vertical-align:top;"><img src="https://orcid.org/sites/default/files/images/orcid_16x16.png" style="width:1em;margin-right:.5em;" alt="ORCID iD icon"></a><a href="https://github.com/mcacace" target="github.widget" rel="noopener noreferrer" style="vertical-align:top;"><img src="images/GitHub-Mark-32px.png" width="16" margin-right=".5em;" alt="GitHub icon id"></a> at the [GFZ German Research Centre for Geosciences](http://www.gfz-potsdam.de/en/home/) from the section [Basin Modelling](http://www.gfz-potsdam.de/en/section/basin-modeling/).


GOLEM is a MOOSE-based application. Visit the [MOOSE framework](http://mooseframework.org) page for more information.

## Licence
GOLEM is distributed under the [GNU GENERAL PUBLIC LICENSE v3](https://github.com/ajacquey/Golem/blob/master/LICENSE).


## Getting Started

#### Minimum System Requirements
The following system requirements are from the MOOSE framework (see [Getting Started](http://mooseframework.inl.gov/getting_started/) for more information):
* Compiler: C++11 Compliant GCC 4.8.4, Clang 3.4.0, Intel20130607
* Python 2.7+
* Memory: 16 GBs (debug builds)
* Processor: 64-bit x86
* Disk: 30 GBs
* OS: UNIX compatible (OS X, most flavors of Linux)

#### 1. Setting Up a MOOSE Installation
To install GOLEM, you need first to have a working and up-to-date installation of the MOOSE framework.  
To do so, please visit the [Getting Started](http://mooseframework.inl.gov/getting_started/) page of the MOOSE framework and follow the instructions. If you encounter difficulties at this step, you can ask for help on the [MOOSE-users Google group](https://groups.google.com/forum/#!forum/moose-users).

#### 2. Clone GOLEM
GOLEM can be cloned directly from [GitHub](https://github.com/ajacquey/Golem) using [Git](https://git-scm.com/). In the following, we refer to the directory `projects` which you created during the MOOSE installation (by default `~/projects`):  

    cd ~/projects
    git clone https://github.com/ajacquey/Golem.git
    cd ~/projects/golem
    git checkout master

*Note: the "master" branch of GOLEM is the "stable" branch which is updated only if all tests are passing.*

#### 3. Compile GOLEM
You can compile GOLEM by following these instructions:

    cd ~/projects/golem
    make -j4

#### 4. Test GOLEM
To make sure that everything was installed properly, you can run the tests suite of GOLEM:

    cd ~/projects/golem
    ./run_tests -j2

If all the tests passed, then your installation is working properly. You can now use the GOLEM simulator!

## Usage
To run GOLEM from the command line with multiple processors, use the following command:

    mpiexec -n <nprocs> ~/projects/golem/golem-opt -i <input-file>

Where `<nprocs>` is the number of processors you want to use and `<input-file>` is the path to your input file (extension `.i`).  

Information about the structure of the GOLEM input files can be found in the documentation (link to follow).
## Cite

If you use GOLEM for your work please cite:
* This repository:  
Antoine B. Jacquey, & Mauro Cacace. (2017, September 29). GOLEM, a MOOSE-based application. Zenodo. http://doi.org/10.5281/zenodo.999401
* The publication presenting GOLEM:  
 Cacace, M. and Jacquey, A. B.: Flexible parallel implicit modelling of coupled thermal–hydraulic–mechanical processes in fractured rocks, Solid Earth, 8, 921-941, https://doi.org/10.5194/se-8-921-2017, 2017.  


Please read the [CITATION](https://github.com/ajacquey/Golem/blob/master/CITATION) file for more information.

## Publications using GOLEM

* Freymark, J., Bott, J., Cacace, M., Ziegler, M., Scheck-Wenderoth, M.: Influence of the Main Border Faults on the 3D Hydraulic Field of the Central Upper Rhine Graben, *Geofluids*, 2019.
* Blöcher, G.,  Cacace, M.,  Jacquey, A. B.,  Zang, A.,  Heidbach, O.,  Hofmann, H.,  Kluge, C.,  Zimmermann, G.: Evaluating Micro-Seismic Events Triggered by Reservoir Operations at the Geothermal Site of Groß Schönebeck (Germany), *Rock Mechanics and Rock Engineering*, 2018.
* Jacquey, A. B.,  Urpi, L.,  Cacace, M.,  Blöcher, G.,  Zimmermann, G.,  Scheck-Wenderoth, M.: Far field poroelastic response of geothermal reservoirs to hydraulic stimulation treatment: Theory and application at the Groß Schönebeck geothermal research facility, *International Journal of Rock Mechanics and Mining Sciences*, 2018.
* Peters, E., Blöcher, G., Salimzadeh, S., Egberts, P. J. P., Cacace, M.: Modelling of multi-lateral well geometries for geothermal applications, *Advances in Geosciences*, 2018.
* Magri, F., Cacace, M., Fischer, T., Kolditz, O., Wang, W., Watanabe, N.: Thermal convection of viscous fluids in a faulted system: 3D benchmark for numerical codes, *Energy Procedia*, 2017.
* Cacace, M. and Jacquey, A. B.: Flexible parallel implicit modelling of coupled Thermal-Hydraulic-Mechanical processes in fractured rocks, Solid Earth, 2017.
* Jacquey, A. B.: Coupled Thermo-Hydro-Mechanical Processes in Geothermal Reservoirs: a Multiphysic and Multiscale Approach Linking Geology and 3D Numerical Modelling, PhD thesis, RWTH Aachen, 2017.
* Jacquey, A. B., Cacace, M., Blöcher, G.: Modelling coupled fluid flow and heat transfer in fractured reservoirs: description of a 3D benchmark numerical case, Energy Procedia, 2017.
* Jacquey, A. B., Cacace, M., Blöcher, G., Milsch, H., Deon, F., Scheck-Wenderoth, M.: Processes Responsible for Localized Deformation within Porous Rocks: Insights from Laboratory Experiments and Numerical Modelling, 6th Biot Conference on Poromechanics, Paris 2017.
