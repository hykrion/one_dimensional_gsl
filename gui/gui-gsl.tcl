#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" ${1+"$@"}

package require Tk

# -----------------------------------------------------------------------------
# @brief  Interfaz gráfica para one-dimensional
#
#         Simplemente escribimos los parámetros en modo texto en el fichero
#         parameters.txt
# -----------------------------------------------------------------------------

# Datos habituales para los cálculos
set uiCommon(RangoInf) -10.0
set uiCommon(RangoSup) 10.0
set uiCommon(Tol) 1e-6
set uiCommon(MaxIter) 100
set uiCommon(Ener) 2.0
set uiCommon(DerEner) 0.01
set uiCommon(Masa) 1.0
set uiCommon(Planck) 1.0
set uiCommon(Alfa) 1.0
set uiCommon(Lambda) 4.0
set uiCommon(N) 2
# Datos habituales para la gráfica
set uiCommon(X_LOW) -4.0
set uiCommon(X_HIG) 4.0
set uiCommon(Y_LOW) -3.0
set uiCommon(Y_HIG) 3.0
# Métodos de resolución
set uiCommon(TipoMetodo) biseccion
set uiCommon(Inf) 2.1
set uiCommon(Sup) 2.9

# -----------------------------------------------------------------------------
# @brief  Leemos los datos arrojados por el resolvedor.
#         El orden debe coincidir con el definido en 'solver.c'
#         0: eigenvalue de la energía
#         1: número de iteraciones
# -----------------------------------------------------------------------------
proc readSolverData {} {  
  set fd [open energy-eigenvalue.txt r]
  set data [read $fd]
  close $fd
  
  set i 0
  set lines [split $data \n]
  foreach line $lines {
    set ::uiCommon(Solver_Result_$i) $line
    incr i
  }
}

# -----------------------------------------------------------------------------
# @brief  Traducir el tipo de método de resolución de la GUI a C.
#         Los enteros del result han de coincidir con los definidos en solver.h
# -----------------------------------------------------------------------------
proc resolutionMethodToC {} {
  # Si no se coge ninguno... usaremos bisección
  set result 0
  
  switch $::uiCommon(TipoMetodo) {
    biseccion {set result 0}
    regulaFalsi {set result 1}
    brent {set result 2}
    newton {set result 3}
    secant {set result 4}
    steffenson {set result 5}
  }
  
  return $result
}

# -----------------------------------------------------------------------------
# @brief  Modificar algunos parámetros de gnuplot
# -----------------------------------------------------------------------------
proc writeGnuplotSettings {} {
  set fd [open plot.gnu w+]
  
  puts $fd "set title 'Ecuación 1-dimensional Schrödinger'"
  puts $fd "set xrange \[$::uiCommon(X_LOW) : $::uiCommon(X_HIG)\]"
  puts $fd "set yrange \[$::uiCommon(Y_LOW) : $::uiCommon(Y_HIG)\]"
  puts $fd "style = \"lines\""
  puts $fd "range1 = \"1:2\""
  puts $fd "range2 = \"1:3\""
  puts $fd "plot 'wave.txt' using 1:2 with lines title \"Φ\", 'wave.txt' using 1:3 with lines title \"V(x)\""
  
  close $fd
}

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Comandos
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# -----------------------------------------------------------------------------
# @brief  Salvar los parámetros el el fichero 'parameters.txt', realizar los
#         cálculos ejecutando 'one-dimensional.exe' y ejecutar gnuplot para
#         visualizar los datos
# -----------------------------------------------------------------------------
proc cmd_visualizar {} {
  # Salvar los parámetros
  set fd [open parameters.txt w+]
  
  puts $fd $::uiCommon(RangoInf)
  puts $fd $::uiCommon(RangoSup)
  puts $fd $::uiCommon(Masa)
  puts $fd $::uiCommon(Planck)
  puts $fd $::uiCommon(Alfa)
  puts $fd $::uiCommon(Lambda)
  puts $fd $::uiCommon(Ener)
  puts $fd $::uiCommon(DerEner)
  puts $fd $::uiCommon(Tol)
  puts $fd $::uiCommon(MaxIter)
  puts $fd $::uiCommon(Inf)
  puts $fd $::uiCommon(Sup)
  puts $fd [resolutionMethodToC]
  
  close $fd
  
  # Realizar los cálculos
  set oneDimensionalCmd [list "one_dimensional_gsl.exe"] 
  exec {*}$oneDimensionalCmd
  
  # Leer los datos obtenido por el resolvedor
  after 1000
  readSolverData
  
  # Dibujar los resultados del potencial y la función de onda
  writeGnuplotSettings
  after 1000
  set gnuplotCmd [list gnuplot.exe -p plot.gnu]
  exec {*}$gnuplotCmd &
}

# -----------------------------------------------------------------------------
# @brief  El nivel de energía que soluciona la ecuación del autovalor depende de 'n'
# -----------------------------------------------------------------------------
proc cmd_calcular_energia {} {
  set a $::uiCommon(Alfa)
  set l $::uiCommon(Lambda)
  set n $::uiCommon(N)
  set h $::uiCommon(Planck)
  set m $::uiCommon(Masa)
  
  set ::uiCommon(Ener) [expr {($h**2*$a**2*(($l*($l - 1))/2.0 - ($l - 1 - $n)**2))/(2*$m)}]
}

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# UI
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

# -----------------------------------------------------------------------------
# @brief  Aquí vamos a modificar los parámetros del método numérico
# -----------------------------------------------------------------------------
proc ui_numerico {} {
  set mainFrame [frame .ntb.frmNumerico.frmMain]

  set w1 [label $mainFrame.lblRango -text "Rango: "]
  set w2 [label $mainFrame.lblRangoInf -text "Inf"]
  set w3 [entry $mainFrame.entRangoInf -textvariable uiCommon(RangoInf) -width 5 -background white]
  set w4 [label $mainFrame.lblRangoSup -text "Sup"]
  set w5 [entry $mainFrame.entRangoSup -textvariable uiCommon(RangoSup) -width 5 -background white]
  grid $w1 $w2 $w3 $w4 $w5
  
  foreach id {Tol MaxIter} {
    set w1 [label $mainFrame.lbl$id -text "$id: "]
    set w2 [entry $mainFrame.ent$id -textvariable uiCommon($id) -width 5 -background white]
    
    grid $w1 $w2
  }
  
  # Información devuelta por el resolvedor
  set w1 [label $mainFrame.lblSolver_Result_0 -text "Eigen.Energ\u00EDa"]
  set w2 [entry $mainFrame.entSolver_Result_0 -textvariable uiCommon(Solver_Result_0) -width 5 -background white]
  grid $w1 $w2
  set w1 [label $mainFrame.lblSolver_Result_1 -text "N\u00FAm. ite"]
  set w2 [entry $mainFrame.entSolver_Result_1 -textvariable uiCommon(Solver_Result_1) -width 5 -background white]
  grid $w1 $w2
  
  set w [button $mainFrame.btnVisualizar -text "Visualizar" -command cmd_visualizar]
  grid $w
  
  # Layout
  grid $mainFrame -sticky ew
}

# -----------------------------------------------------------------------------
# @brief  Configurar los métodos de resolución que usan intervalos
#         (36.8 Root Bracketing Algorithms, pág 406 de gsl-ref.pdf)
#         -bisection
#         -falsepos
#         -brent
# -----------------------------------------------------------------------------
proc ui_resolucion_intervalo {} {
  set mainFrame [frame .ntb.frmResIntervalo.frmMain]
  
  set w1 [ttk::radiobutton $mainFrame.radBiseccion -text "Bisecci\u00F3n" -variable ::uiCommon(TipoMetodo) -value biseccion]
  set w2 [ttk::radiobutton $mainFrame.radRegula -text "RegulaFalsi" -variable ::uiCommon(TipoMetodo) -value regulaFalsi]
  set w3 [ttk::radiobutton $mainFrame.radBrent -text "Brent" -variable ::uiCommon(TipoMetodo) -value brent]
  grid $w1 $w2 $w3
  
  foreach id {Inf Sup} {
    set w1 [label $mainFrame.lbl$id -text "$id: "]
    set w2 [entry $mainFrame.ent$id -textvariable ::uiCommon($id) -width 5 -background white]
    
    grid $w1 $w2
  }
  
  # Layout
  grid $mainFrame
}

proc ui_resolucion_derivada {} {
  set mainFrame [frame .ntb.frmResDerivada.frmMain]
  
  set w1 [ttk::radiobutton $mainFrame.radNewton -text "Newton" -variable ::uiCommon(TipoMetodo) -value newton]
  set w2 [ttk::radiobutton $mainFrame.radSecant -text "Secante" -variable ::uiCommon(TipoMetodo) -value secant]
  set w3 [ttk::radiobutton $mainFrame.radSteffenson -text "Steffenson" -variable ::uiCommon(TipoMetodo) -value steffenson]
  grid $w1 $w2 $w3
    
  # Layout
  grid $mainFrame
}

# -----------------------------------------------------------------------------
# @brief  Modificar algunos aspectos de la gráfica mostrada por gnuplot
# -----------------------------------------------------------------------------
proc ui_fisico {} {
  set mainFrame [frame .ntb.frmFisico.frmMain]

  # En lugar de introducir un nivel de energía, introducimos un 'n' en la ecuación
  # para calcular la energía
  set w1 [label $mainFrame.lblN -text "N:"]
  set w2 [entry $mainFrame.entN -textvariable uiCommon(N) -width 5 -background white]
  set w3 [button $mainFrame.btnN -text "Calcular Ener" -command cmd_calcular_energia]
  grid $w1 $w2 $w3
  
  foreach id {Ener DerEner Masa Planck Alfa Lambda} {
    set w1 [label $mainFrame.lbl$id -text "$id: "]
    set w2 [entry $mainFrame.ent$id -textvariable uiCommon($id) -width 5 -background white]
    
    grid $w1 $w2
  }
  
  # Layout
  grid $mainFrame -sticky ew
}

# -----------------------------------------------------------------------------
# @brief  Aquí vamos a modificar los parámetros físicos
# -----------------------------------------------------------------------------
proc ui_gnuplot {} {
  set mainFrame [frame .ntb.frmGnuplot.frmMain]
  
  foreach id {X_LOW X_HIG Y_LOW Y_HIG} {
    set w1 [label $mainFrame.lbl$id -text "$id: "]
    set w2 [entry $mainFrame.ent$id -textvariable uiCommon($id) -width 5 -background white]
    
    grid $w1 $w2
  }
  
  # Layout
  grid $mainFrame -sticky ew
}

# -----------------------------------------------------------------------------
# @brief  Iniciar el sencillo GUI
# -----------------------------------------------------------------------------
proc ui_init {} {
  ttk::notebook .ntb
  .ntb add [frame .ntb.frmNumerico] -text "Num\u00E9rico"
  .ntb add [frame .ntb.frmResIntervalo] -text "Res.Intervalo"
  .ntb add [frame .ntb.frmResDerivada] -text "Res.Derivada"
  .ntb add [frame .ntb.frmFisico] -text "F\u00EDsico"
  .ntb add [frame .ntb.frmGnuplot] -text "gnuplot"
  .ntb select .ntb.frmNumerico
  
  ui_numerico
  ui_resolucion_intervalo
  ui_resolucion_derivada
  ui_fisico
  ui_gnuplot

  grid .ntb
}

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Main
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
proc main {} {
  ui_init
}

main