#include "header.h"

typedef struct {
    const Parameters* pars;
    const EdgeConditions* edge;
    double *u;
    int rank;
    int* iteration;   
} ThreadArg;

void * init(void * data) {
    ThreadArg* arg = (ThreadArg*) data;
    int M = arg->pars->M;
    double* phi1 = arg->edge->phi1;
    double c = arg->pars->tau * arg->pars->a / arg->pars->h;
    for (int i = arg->rank + 1; i < M - 1; i += N_THREADS) {
        arg->u[i] = phi1[i];
        arg->u[M + i] = phi1[i] + arg->pars->tau * arg->edge->phi2[i] + 
            c * c / 2 * (phi1[i+1] - 2*phi1[i] + phi1[i-1]);
    }
    for (int i = arg->rank; i < arg->pars->P; i += N_THREADS) {
        arg->u[i*M] = arg->edge->psi1[i];
        arg->u[i*M + M - 1] = arg->edge->psi2[i];
    }
    pthread_exit(0);
}

void * iterate(void * data) {
    ThreadArg* arg = (ThreadArg*) data;
    double c = arg->pars->tau * arg->pars->a / arg->pars->h;
    int M = arg->pars->M;
    int d = *(arg->iteration) * M;
    double* u = arg->u;
    for (int i = arg->rank + 1; i < M - 1; i++) {
        u[d + M + i] = 2 * u[d + i] - u[d - M + i] + 
            c*c*(u[d + i + 1] - 2*u[d + i] + u[d + i -1]);
    }
    pthread_exit(0);
}

