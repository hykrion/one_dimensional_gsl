#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED

/* Resolution methods */
#define SOLVER_BISECTION 0
#define SOLVER_FALSEPOS 1
#define SOLVER_BRENT 2
#define SOLVER_NEWTON 3
#define SOLVER_SECANT 4
#define SOLVER_STEFFENSON 5

struct solver_params
{
  double xLow;
  double xHigh;
  double rootGuess;
  double tol;
  int maxIter;
};

int solver(int type, void *params);

#endif
