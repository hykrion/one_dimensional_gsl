#ifndef UI_PARAMETERS_H_INCLUDED
#define UI_PARAMETERS_H_INCLUDED

/* NOTE SIZE number of parameters + 1 */
#define UI_PARAM_SIZE 13
#define UI_PARAM_LEN 16

#define UI_PARAM_A 0
#define UI_PARAM_B 1
#define UI_PARAM_MASS 2
#define UI_PARAM_PLANCK 3
#define UI_PARAM_ALPHA 4
#define UI_PARAM_LAMBDA 5
#define UI_PARAM_E 6
#define UI_PARAM_DE 7
#define UI_PARAM_TOL 8
#define UI_PARAM_MAXITER 9
#define UI_PARAM_BRACKET_LOW 10
#define UI_PARAM_BRACKET_HIGH 11
#define UI_PARAM_RESOLUTION_METHOD 12

void readParams(double uiParams[]);

#endif
