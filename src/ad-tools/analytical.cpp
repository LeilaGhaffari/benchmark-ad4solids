#include "../../include/analytical.h"

void init_analytic(void *ctx) {
    AnalyticContext *context = static_cast<AnalyticContext *>(ctx);
    context->mu = 1.;
    context->lambda = 1.;
    context->stored = new double[NUM_COMPONENTS_STORED_ANALYTICAL];
}

void free_analytic(void *ctx) {
    AnalyticContext *context = static_cast<AnalyticContext *>(ctx);
    delete[] context->stored;
    delete context;
}

void f_analytic(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]) {
    // Context
    AnalyticContext *context = static_cast<AnalyticContext *>(ctx);
    double mu = context->mu;
    double lambda = context->lambda;
    double *stored_values = context->stored;

    double Grad_u[3][3], F_inv[3][3], tau_sym[6], dXdx[3][3], e_sym[6], J_dVdJ;

    // Grad_u = du/dx_initial = du/dX * dX/dx_initial
    MatMatMult(1.0, dudX, dXdx_initial, Grad_u);

    // Compute the Deformation Gradient : F = I + Grad_u
    double F[3][3];
    DeformationGradient(Grad_u, F);
    GreenEulerStrain(Grad_u, e_sym);

    const double Jm1 = MatDetAM1(Grad_u);
    VolumetricFunctionAndDerivatives(Jm1, NULL, &J_dVdJ, NULL);
    KirchhoffTau_NeoHookean(J_dVdJ, lambda, 2*mu, e_sym, tau_sym);
    SymmetricMatUnpack(tau_sym, f1);

    // Compute F^{-1}
    const double detF = Jm1 + 1.;
    MatInverse(F, detF, F_inv);

    // dXdx = dX/dx = dX/dx_initial * F^{-1}
    MatMatMult(1.0, dXdx_initial, F_inv, dXdx);

    // ------------------------------------------------------------------------
    // Store
    // ------------------------------------------------------------------------
    StoredValuesPack(0, 9, (double *)dXdx, stored_values);
    StoredValuesPack(9, 6, (double *)tau_sym, stored_values);
    StoredValuesPack(15, 1, &Jm1, stored_values);

    // ------------------------------------------------------------------------
    // More info
    // ------------------------------------------------------------------------
    printf("\n\ne =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", e_sym[i]);
    printf("\n\nStrain Energy from e = ");
    printf(" %.12lf", StrainEnergy(e_sym, lambda, mu));
    printf("\n\ntau =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", tau_sym[i]);
}

void df_analytic(void *ctx, const double ddudX[3][3], double df1[3][3]){
    // Context
    AnalyticContext *context = static_cast<AnalyticContext *>(ctx);
    const double mu = context->mu;
    const double lambda = context->lambda;
    const double *stored_values = context->stored;

    double grad_du[3][3], tau_sym[6], tau[3][3], grad_du_tau[3][3],
           FdSFTranspose[3][3], dXdx[3][3], Jm1, J_dVdJ, J2_d2VdJ2;

    // ------------------------------------------------------------------------
    // Unpack stored values
    // ------------------------------------------------------------------------
    StoredValuesUnpack(0, 9,  stored_values, (double *)dXdx);
    StoredValuesUnpack(9, 6,  stored_values, (double *)tau_sym);
    StoredValuesUnpack(15, 1, stored_values, &Jm1);

    // Compute grad_du = ddu/dX * dX/dx
    MatMatMult(1.0, ddudX, dXdx, grad_du);

    SymmetricMatUnpack(tau_sym, tau);

    // Compute grad_du_tau = grad_du*tau
    MatMatMult(1.0, grad_du, tau, grad_du_tau);

    // Compute F*dS*F^T
    VolumetricFunctionAndDerivatives(Jm1, NULL, &J_dVdJ, &J2_d2VdJ2);
    ComputeFdSFTranspose_NeoHookean(J_dVdJ, J2_d2VdJ2, lambda, mu, grad_du, FdSFTranspose);

    // df1 = dtau - tau * grad_du^T
    //     = grad_du*tau + F*dS*F^T
    MatMatAdd(1.0, grad_du_tau, 1., FdSFTranspose, df1);

    // ------------------------------------------------------------------------
    // More info
    // ------------------------------------------------------------------------
    printf("\n\n");
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) printf("\t%.12lf", df1[i][j]);
        printf("\n");
    }
    printf("\n\n");
}
