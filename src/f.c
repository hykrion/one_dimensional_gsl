#include "headers/f.h"

#include <stdio.h>
#include <math.h>

#include "headers/ui_parameters.h"
#include "headers/globals.h"

static int nr;
static int nl;

static double ul[NODES];
static double ur[NODES];
static double ql[NODES];
static double qr[NODES];
static double s[NODES];

/* --------------------------------------------------------------------------
 Helper functions
----------------------------------------------------------------------------- */
/**
  @brief  Potential equation
  @param  IN x: x axis position

  @return result: Potential value at x point
  */
static double
vScattering(double x)
{
  double result = 0.0;
  /* double alpha = uiParams[UI_PARAM_ALPHA]; */
  double x0 = 0.0;
  double x1 = 1.0;
  double v0 = 2.0;

  if (x > x0 && x < x1)
    result = v0;

  return result;
}

/* Potential equation */
static double
v(double x)
{
  double alpha = uiParams[UI_PARAM_ALPHA];
  double lambda = uiParams[UI_PARAM_LAMBDA];
  double planck = uiParams[UI_PARAM_PLANCK];
  double mass = uiParams[UI_PARAM_MASS];

  return (planck*planck * alpha*alpha * lambda*(lambda - 1)*(0.5 - 1/(cosh(alpha*x)*cosh(alpha*x))))/(2*mass);
}

/**
  @brief  Potential values. This way we can draw them
*/
void
potentialValues()
{
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;
  double x = a;
  int i;

  for (i = 0; i <= n; i++)
  {
    vx[i] = v(x);
    /*vx[i] = vScattering(x);*/
    x = a + i*h;
  }
}

/**
  @brief  Numerov method
*/
static void
numerov(int m, double h, double u0, double u1, double q[], double s[], double u[])
{
  u[0] = u0;
  u[1] = u1;
  double h2 = h*h/12;

  int i;
  for (i = 1; i < m - 1; ++i)
  {
    double c0 = 1 + h2*q[i - 1];
    double c1 = 2 - 10*h2*q[i];
    double c2 = 1 + h2*q[i + 1];
    double d = h2*(s[i + 1] + 10*s[i] + s[i - 1]);

    u[i + 1] = (c1*u[i] + d - c0*u[i - 1])/c2;
  }
}

/* Simpson rule */
static double
simpson(int ySize, double y[], double h)
{
  int n = ySize - 1;
  double s0 = 0;
  double s1 = 0;
  double s2 = 0;

  int i;
  for (i = 1; i < n; i += 2)
  {
    s0 += y[i];
    s1 += y[i - 1];
    s2 += y[i + 1];
  }
  double s = (s1 + 4*s0 + s2)/3;

  if (ySize % 2 == 0)
    return h*(s + (5*y[n] + 8*y[n - 1] - y[n - 2])/12);
  else
    return h*s;
}


/* Calculate the wave function */
static void
wave(double energy)
{
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;

  double mass = uiParams[UI_PARAM_MASS];
  double planck = uiParams[UI_PARAM_PLANCK];

  double y[NODES];
  /* NOTE phi(inf) = 0, on the next step it will be a little bigger */
  double phiN0 = 0;
  double phiN1 = 0.01;

  /* Calculate q(x) in Sturm-Liouville equation */
  int i;
  for (i = 0; i <= n; i++)
  {
    double x = a + i*h;
    ql[i] = 2*mass*(energy - v(x))/(planck*planck);
    /*ql[i] = 2*mass*(energy - vScattering(x))/(planck*planck);*/
    qr[n - i] = ql[i];
  }
  /* Index matching */
  int im = 0;

  for (i = 0; i < n; i++)
  {
    if ((ql[i]*ql[i + 1] < 0) && (ql[i] > 0))
      im = i;
  }
  /* Search for the next root just a little bit on the right */
  nl = im + 2;
  nr = n - im + 2;
  numerov(nl, h, phiN0, phiN1, ql, s, ul);
  numerov(nr, h, phiN0, phiN1, qr, s, ur);
  /* Left matching point as it was */
  nl = im;
  nr = n - im;
  /* Before the matching, we need to rescale */
  double ratio = ur[nr]/ul[im];

  /* Left wave function */
  for (i = 0; i <= nl; i++)
  {
    u[i] = ratio*ul[i];
    y[i] = u[i]*u[i];
  }

  /* Right wave function */
  for (i = 0; i < nr - 1; ++i)
  {
    u[i + im] = ur[nr - i];
    y[i + im] = u[i + im]*u[i + im];
  }
  /* Normalize the function */
  double sum = simpson(sizeof(y)/sizeof(y[0]), y, h);
  sum = sqrt(sum);

  for (i = 0; i <= n; ++i)
    u[i] /= sum;
}

/* --------------------------------------------------------------------------
 Problem functions
----------------------------------------------------------------------------- */
double
f(double x, void *params)
{
  (void)(params);
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;

  wave(x);

  return (ur[nr + 1] - ur[nr - 1] - ul[nl - 1] + ul[nl + 1])/(2*h*ur[nr]);
}

double f_df(double x, void *params)
{
  (void)(params);
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;
  wave(x);

  return (ur[nr + 1] - 2*ur[nr] + ur[nr - 1] - ul[nl - 1] + 2*ul[nl] - ul[nl + 1])/(h*h*ur[nr]);
}

void f_fdf(double x, void *params, double *y, double *dy)
{
  (void)(params);
  double a = uiParams[UI_PARAM_A];
  double b = uiParams[UI_PARAM_B];
  /* TODO
  double h = uiParams[UI_PARAM_H];
  */
  int n = NODES - 1;
  double h = (b - a)/n;
  wave(x);

  *y = (ur[nr + 1] - ur[nr - 1] - ul[nl - 1] + ul[nl + 1])/(2*h*ur[nr]);
  *dy = (ur[nr + 1] - 2*ur[nr] + ur[nr - 1] - ul[nl - 1] + 2*ul[nl] - ul[nl + 1])/(h*h*ur[nr]);
}
