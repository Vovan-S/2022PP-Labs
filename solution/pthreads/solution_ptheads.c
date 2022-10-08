#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"

#include "header.h"

void accuracy_test_data(Parameters* pars, EdgeConditions* edge) {
    if (!pars || !edge) return;
    pars->a = 0.5;
    pars->h = pars->tau = 0.1;
    pars->M = pars->P = 500;
    double l = pars->h * pars->M;
    size_t phi_size = sizeof(double) * pars->M;
    edge->phi1 = malloc(phi_size);
    edge->phi2 = malloc(phi_size);
    edge->psi1 = malloc(phi_size);
    edge->psi2 = malloc(phi_size);
    for (size_t i = 0; i < pars->M; i++) {
        edge->phi1[i] = 0;
        edge->phi2[i] = M_PI * pars->a / l * sin(M_PI * i / (pars->M - 1));
    }
    for (size_t i = 0; i < pars->P; i++) {
        edge->psi1[i] = edge->psi2[i] = 0;
    }
}

int main() {

}