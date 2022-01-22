#include <stdio.h>

#include "headers/globals.h"
#include "headers/ui_parameters.h"
#include "headers/f.h"
#include "headers/solver.h"

double u[NODES];
double vx[NODES];
double uiParams[UI_PARAM_SIZE];

int
main (void)
{
  /* Read parameters from the GUI */
  readParams(uiParams);

  double tol = uiParams[UI_PARAM_TOL];
  double maxIter = uiParams[UI_PARAM_MAXITER];
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  double rootGuess = uiParams[UI_PARAM_E];
  double xLow = uiParams[UI_PARAM_BRACKET_LOW];
  double xHigh = uiParams[UI_PARAM_BRACKET_HIGH];
  int resolutionMethod = uiParams[UI_PARAM_RESOLUTION_METHOD];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;
  int status = 0;

  /* Solve using the chosen method */
  struct solver_params solverParams = {xLow, xHigh, rootGuess, tol, maxIter};
  status = solver(resolutionMethod, &solverParams);

  /* Get potential values to draw them */
  potentialValues();

  /* Graph data: x, wave function & potential */
  FILE *fp;
  fp = fopen("wave.txt", "w");
  double x = a;

  int i;
  for (i = 0; i <= NODES - 1; i += 1)
  {
    fprintf(fp, "%.32f\t%.32f\t%.32f\n", x, u[i], vx[i]);
    x += h;
  }
  fclose(fp);

  return status;
}
