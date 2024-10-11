#include "../../include/enzyme.h"
#include <math.h>
#include <stdlib.h>

void init_enzyme(void *ctx) {
    EnzymeContext *enzyme_ctx = (EnzymeContext*)ctx;
    enzyme_ctx->lambda = 1.0;
    enzyme_ctx->mu = 0.5;
}

void free_enzyme(void *ctx) {
    EnzymeContext *enzyme_ctx = (EnzymeContext*)ctx;
    free(enzyme_ctx);
}

// Residual Evaluation
void f_enzyme(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double dXdx[3][3], double e_sym[6], double f1[3][3]) {
    const EnzymeContext *context = (EnzymeContext *)ctx;
    const double mu = context->mu;
    const double lambda = context->lambda;

    double Grad_u[3][3], F_inv[3][3], tau_sym[6];

    MatMatMult(1.0, dudX, dXdx_initial, Grad_u);
    double F[3][3];
    DeformationGradient(Grad_u, F);

    const double Jm1  = MatDetAM1(Grad_u);
    const double detF = Jm1 + 1.0;
    MatInverse(F, detF, F_inv);
    MatMatMult(1.0, dXdx_initial, F_inv, dXdx);

    GreenEulerStrain(Grad_u, e_sym);
    Kirchhofftau_sym_NeoHookean_AD_Enzyme(lambda, mu, e_sym, tau_sym);
    SymmetricMatUnpack(tau_sym, f1);
}

// Jacobian Evaluation
void df_enzyme(void *ctx, double dXdx[3][3], double e_sym[6], const double ddudX[3][3], double df1[3][3]) {
    const EnzymeContext *context = (EnzymeContext *)ctx;
    const double mu = context->mu;
    const double lambda = context->lambda;

    double grad_du[3][3], b_sym[6], b[3][3], de_sym[6], tau_sym[6], dtau_sym[6], tau[3][3], dtau[3][3], tau_grad_du[3][3];

    MatMatMult(1.0, ddudX, dXdx, grad_du);
    for (int j = 0; j < 6; j++) b_sym[j] = 2 * e_sym[j] + (j < 3);
    SymmetricMatUnpack(b_sym, b);
    GreenEulerStrain_fwd(grad_du, b, de_sym);

    dtau_fwd_Enzyme(lambda, mu, e_sym, de_sym, tau_sym, dtau_sym);
    SymmetricMatUnpack(tau_sym, tau);
    SymmetricMatUnpack(dtau_sym, dtau);
    MatMatTransposeMult(tau, grad_du, tau_grad_du);

    for (int j = 0; j < 3; j++) {
        for (int k = 0; k < 3; k++) {
            df1[j][k] = dtau[j][k] - tau_grad_du[j][k];
        }
    }
}
