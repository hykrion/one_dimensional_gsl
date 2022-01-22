#ifndef F_H_INCLUDED
#define F_H_INCLUDED

struct f_params
{
  double planck;
  double mass;
  double alpha;
  double lambda;
};

double f(double x, void *params);
double f_df(double x, void *params);
void f_fdf(double x, void *params, double *y, double *dy);
/* Not related... it's here for simplicity... */
void potentialValues();

#endif
