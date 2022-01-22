#include "headers/solver.h"

#include <stdio.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "headers/f.h"

static double m_root;
static int m_numIter;

/**
  @brief  Bounded resolution methods
*/
static int
solverBracketing(int type,
                 double xLow,
                 double xHigh,
                 double rootGuess,
                 double tol,
                 int maxIter)
{
  int status = 0;
  double x1;

  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  gsl_function F;

  F.function = &f;
  F.params = 0;

  switch (type)
  {
    case SOLVER_BISECTION:
      T = gsl_root_fsolver_bisection;
      break;
    case SOLVER_FALSEPOS:
      T = gsl_root_fsolver_falsepos;
      break;
    case SOLVER_BRENT:
      T = gsl_root_fsolver_brent;
      break;
  }
  s = gsl_root_fsolver_alloc (T);
  gsl_root_fsolver_set (s, &F, xLow, xHigh);

  x1 = xLow;
  int iter = 0;
  do
  {
    status = gsl_root_fsolver_iterate (s);
    x1 = gsl_root_fsolver_root (s);
    xLow = gsl_root_fsolver_x_lower (s);
    xHigh = gsl_root_fsolver_x_upper (s);
    status = gsl_root_test_interval(xLow, xHigh, 0.0, tol);
    iter++;
  }
  while (status == GSL_CONTINUE && iter < maxIter);

  gsl_root_fsolver_free (s);

  m_root = x1;
  m_numIter = iter;

  return status;
}

/**
  @brief  Resolution methods using derivatives
*/
static int
solverDerivatives(int type,
                 double rootGuess,
                 double tol,
                 int maxIter)
{
  int status = 0;
  double x0;
  double x1;

  const gsl_root_fdfsolver_type *T;
  gsl_root_fdfsolver *s;
  gsl_function_fdf FDF;

  FDF.f = &f;
  FDF.df = &f_df;
  FDF.fdf = &f_fdf;
  FDF.params = 0;

  /* With Newton or Steffeson does not converge... */
  switch (type)
  {
    case SOLVER_NEWTON:
      T = gsl_root_fdfsolver_newton;
      break;
    case SOLVER_SECANT:
      T = gsl_root_fdfsolver_secant;
      break;
    case SOLVER_STEFFENSON:
      T = gsl_root_fdfsolver_steffenson;
      break;
  }

  s = gsl_root_fdfsolver_alloc (T);
  gsl_root_fdfsolver_set (s, &FDF, rootGuess);

  x1 = rootGuess;
  int iter = 0;
  do
  {
    status = gsl_root_fdfsolver_iterate (s);
    x0 = x1;
    x1 = gsl_root_fdfsolver_root (s);

    status = gsl_root_test_delta (x1, x0, 0, tol);

    iter++;
  } while (status == GSL_CONTINUE && iter < maxIter);

  gsl_root_fdfsolver_free (s);

  m_root = x1;
  m_numIter = iter;

  return status;
}

/**
  @brief  Resolution methods. We save the result and additional data in the file
          'energy-eigenvalue.txt'
*/
int
solver(int type, void *params)
{
  int status = 0;
  struct solver_params *p = (struct solver_params *)params;
  double xLow = p->xLow;
  double xHigh = p->xHigh;
  double rootGuess = p->rootGuess;
  double tol = p->tol;
  int maxIter = p->maxIter;

  switch(type)
  {
    case SOLVER_BISECTION:
    case SOLVER_FALSEPOS:
    case SOLVER_BRENT:
      status = solverBracketing(type, xLow, xHigh, rootGuess, tol, maxIter);
      break;
    case SOLVER_NEWTON:
    case SOLVER_SECANT:
    case SOLVER_STEFFENSON:
      status = solverDerivatives(type, rootGuess, tol, maxIter);
      break;
  }

  /* Save the info */
  FILE *fp;
  fp = fopen("energy-eigenvalue.txt", "w");
  fprintf(fp, "%.2f\n", m_root);
  fprintf(fp, "%d\n", m_numIter);
  fclose(fp);

  return status;
}
