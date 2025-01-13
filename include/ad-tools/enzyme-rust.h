#ifndef ENZYME_RUST_H
#define ENZYME_RUST_H

#include "../utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_COMPONENTS_STORED_ENZYME_RUST 15

typedef struct {
    double lambda;
    double mu;
    double *stored;
} EnzymeRustContext;

void init_enzyme_rust(void *ctx);
void free_enzyme_rust(void *ctx);
void f_enzyme_rust(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]);
void df_enzyme_rust(void *ctx, const double ddudX[3][3], double df1[3][3]);
void compute_stress(double youngs_modulus, double poisson_ratio, const double e_sym[6], double tau_sym[6]);

#ifdef __cplusplus
}
#endif

#endif // ENZYME_RUST_H
