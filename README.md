<p align="center">
  <img src="/img/one-dimensional-gui-4.PNG">
</p>

# one_dimensional_gsl
Rought test to solve the one-dimensional Schrödinger equation using GSL (An Introduction to Computational Physics - Tao Pang)

If you want to compile the code you'll need the [GSL library](https://www.gnu.org/software/gsl/). I've done all the development in Windows so I've used the [MSYS2](https://www.msys2.org/) system. You can use [Chocolatey](https://chocolatey.org/) or install MSYS2 directly.

After that you just have to install the GSL packages using [pacman](https://archlinux.org/pacman/pacman.8.html). pacman is very convenient...

# GUI
The GUI was made with [Tcl/Tk](https://www.tcl.tk/) You need one tcl distribution to use the GUI (but you don't need the GUI to use the program). Another useful tool is [gnuplot](http://www.gnuplot.info/), you can install it with pacman too.
This's how the GUI looks like.

![one dimensional Schrödinger](/img/one-dimensional-gui-1.PNG)

you can choose the bounds and the resolution method

![one dimensional Schrödinger bounds](/img/one-dimensional-gui-2.PNG)

Clicking 'Visualizar' you can visualise the graph

![one dimensional Schrödinger bounds](/img/one-dimensional-gui-4.PNG)

and get the energy value and the number of iterations

![one dimensional Schrödinger bounds](/img/one-dimensional-gui-3.PNG)

If you want to modify the GUI you just need to be sure that the order and the number of parameters in *ui_parameters.h* and the procedure *cmd_visualizar* in gui-gsl.tcl must match. 

# None GUI
If you just want to use the program you can use it with this two files:

- one_dimensional_gsl.exe
- parameters.txt

The parameters and their meanings are in: *headers/ui_parameters.h* and/or *gui-gsl.tcl*. The names should be self explanatory. As this's a rough test may be some parameters are not used, as UI_PARAM_DE...

# Just the program
If you just want to test the program, don't want to install MSYS2 nor GSL, nor Tcl... here your are the exe (build in debug) and the necessary dlls **barebone**
