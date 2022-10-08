#include "pthread.h"

#ifndef N_THREADS
#define N_THREADS 8
#endif

typedef struct {
    int M;
    int P;
    double h;
    double tau;
    double a;
} Parameters;

typedef struct {
    double *phi1;
    double *phi2;
    double *psi1;
    double *psi2;
} EdgeConditions;
