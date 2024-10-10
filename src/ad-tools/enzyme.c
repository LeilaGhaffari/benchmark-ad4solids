#include "../../include/enzyme.h"  // Include the Enzyme header


void InitializeEnzymeContext(NeoHookeanElasticityContext *ctx) {
    ctx->mu = 1.0;
    ctx->lambda = 1.0;
    ctx->f1 = enzyme_f1;
    ctx->df1 = enzyme_df1;
    ctx->free = EnzymeFree;
}

void enzyme_f1(NeoHookeanElasticityContext *ctx, double dXdx[3][3], double dudX[3][3], double e_sym[6], double f1_out[3][3]) {

}

void enzyme_df1(NeoHookeanElasticityContext *ctx, double dXdx[3][3], double e_sym[6], double ddudX[3][3], double df1_out[3][3]) {

}

void EnzymeFree(NeoHookeanElasticityContext *ctx) {

}
