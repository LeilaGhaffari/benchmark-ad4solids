#ifndef ENZYME_H
#define ENZYME_H

#include "../utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_COMPONENTS_STORED_ENZYME 15

typedef struct {
    double lambda;
    double mu;
    double *stored;
} EnzymeContext;

void init_enzyme(void *ctx);
void free_enzyme(void *ctx);
void f_enzyme(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]);
void df_enzyme(void *ctx, const double ddudX[3][3], double df1[3][3]);

void __enzyme_autodiff(void *, ...);
void __enzyme_fwddiff(void *, ...);
extern int enzyme_const;
double  StrainEnergy_NeoHookeanCurrentAD_Enzyme(double e_sym[6], double lambda, double mu);
//void *__enzyme_function_like[2] = {(void *)Log1pSeries, (void *)"log1p"};
void Kirchhofftau_sym_NeoHookean_AD_Enzyme(const double lambda, const double mu, double e_sym[6], double tau_sym[6]);
void dtau_fwd_Enzyme(const double lambda, const double mu, double e_sym[6], double de_sym[6], double tau_sym[6], double dtau_sym[6]);

#ifdef __cplusplus
}
#endif

#endif // ENZYME_H
