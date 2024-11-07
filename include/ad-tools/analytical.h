#ifndef ANALYTIC_H
#define ANALYTIC_H

#include "../utils.h"

#define NUM_COMPONENTS_STORED_ANALYTICAL 16

typedef struct {
    double lambda;
    double mu;
    double *stored;
} AnalyticContext;

// Declare function prototypes
void init_analytic(void *ctx);
void free_analytic(void *ctx);
void f_analytic(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]);
void df_analytic(void *ctx, const double ddudX[3][3], double df1[3][3]);

#endif // ANALYTIC_H
