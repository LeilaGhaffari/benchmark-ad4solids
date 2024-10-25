#ifndef ADOLC_H
#define ADOLC_H

#include "utils.h"
#include <adolc/adolc.h>

typedef struct {
    double lambda;
    double mu;
} AdolcContext;

// Declare function prototypes
void init_adolc(void *ctx);
void free_adolc(void *ctx);
void f_adolc(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double dXdx[3][3], double e_sym[6], double f1[3][3]);
void df_adolc(void *ctx, double dXdx[3][3], double e_sym[6], const double ddudX[3][3], double df1[3][3]);

// Declare helper functions
adouble MatDetAM1Symmetric(adouble A_sym[6]);
adouble MatTraceSymmetric(adouble A_sym[6]);
adouble Log1pSeries(adouble x);
adouble StrainEnergy_NeoHookeanCurrentAD_ADOLC(adouble e_sym[6], const double lambda, const double mu);
void ComputeGradPsi(double grad[6], double e_sym[6], AdolcContext *data);
void ComputeHessianPsi(double hess[6][6], double e_sym[6], AdolcContext *data);

#endif // ADOLC_H
