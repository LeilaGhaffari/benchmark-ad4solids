#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NeoHookeanElasticityContext {
    void (*initialize)(struct NeoHookeanElasticityContext *ctx);
    void (*f1)(struct NeoHookeanElasticityContext *ctx, double dXdx[3][3], double dudX[3][3], double e_sym[6], double f1_out[3][3]);
    void (*df1)(struct NeoHookeanElasticityContext *ctx, double dXdx[3][3], double e_sym[6], double ddudX[3][3], double df1_out[3][3]);
    void (*free)(struct NeoHookeanElasticityContext *ctx);

    // Common parameters
    double mu;
    double lambda;

    // Hessian matrix for ADOL-C
    double **H;

} NeoHookeanElasticityContext;

#ifdef __cplusplus
}
#endif

#endif // COMMON_H
