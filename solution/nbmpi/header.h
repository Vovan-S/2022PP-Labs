#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "../common/iteration.h"

#ifndef V
#define V 0
#endif
#define LOG if (V) printf
#define TAG 1

void exchange(double* left, double* right, int rank, int np);

/**
 * @param arr initial array
 * @param x_dim allocated size for x axis
 * @param m quant in x axis
 * @param p quant in t axis
 * @param value value to be set
*/
void set_to_local_result(double* arr, int x_dim, int m, int p, double value);

double get_from_local_result(double* arr, int x_dim, int m, int p);

void calculate_row(int rank, int np, double *buffer, int row_size, int row,
                   double *prev_row, int prev_row_dim,
                   EdgeConditions *edge,
                   double(*func)(double, double, double, double));

double iterate(double u11, double u12, double u10, double u01);