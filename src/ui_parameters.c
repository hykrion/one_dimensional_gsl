#include "headers/ui_parameters.h"

#include <stdio.h>
#include <stdlib.h>

/**
  @brief  Read the different params from the GUI.
          By simplicity these parameters are in a text file, parameters.txt, that
          will have a value per line.
  */
void readParams(double uiParams[])
{
  char str[UI_PARAM_SIZE][UI_PARAM_LEN];
  /* Read parameters from file */
  FILE *fp;
  fp = fopen("parameters.txt", "r");

  int i = 0;
  while(!feof(fp))
  {
    fgets(str[i], UI_PARAM_LEN - 1, fp);
    i++;
  }

  fclose(fp);

  i = 0;
  while(i < UI_PARAM_SIZE)
  {
    uiParams[i] = atof(str[i]);
    i++;
  }
}
