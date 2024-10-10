#include "../../include/adolc.h"


void InitializeAdolcContext(NeoHookeanElasticityContext *ctx) {
    ctx->H = new double*[6];
    for (int i = 0; i < 6; i++) {
        ctx->H[i] = new double[i + 1];  // Symmetric Hessian
    }

    ctx->initialize = [](NeoHookeanElasticityContext *ctx) {
        ctx->mu = 1.0;
        ctx->lambda = 1.0;
    };

    ctx->f1 = [](NeoHookeanElasticityContext *ctx, double dXdx[3][3], double dudX[3][3], double e_sym[6], double f1_out[3][3]) {

    };

    ctx->df1 = [](NeoHookeanElasticityContext *ctx, double dXdx[3][3], double e_sym[6], double ddudX[3][3], double df1_out[3][3]) {

    };

    ctx->free = [](NeoHookeanElasticityContext *ctx) {
        for (int i = 0; i < 6; i++) {
            delete[] ctx->H[i];
        }
        delete[] ctx->H;
    };
}
