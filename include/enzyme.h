#ifndef ENZYME_H
#define ENZYME_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitializeEnzymeContext(NeoHookeanElasticityContext *ctx);
void EnzymeFree(NeoHookeanElasticityContext *ctx);
void enzyme_f1(NeoHookeanElasticityContext *ctx, double dXdx[3][3], double dudX[3][3], double e_sym[6], double f1_out[3][3]);
void enzyme_df1(NeoHookeanElasticityContext *ctx, double dXdx[3][3], double e_sym[6], double ddudX[3][3], double df1_out[3][3]);

#ifdef __cplusplus
}
#endif

#endif // ENZYME_H

