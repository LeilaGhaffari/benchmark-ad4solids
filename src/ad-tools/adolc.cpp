#include "../../include/adolc.h"

void init_adolc(void *ctx) {
    AdolcContext *context = static_cast<AdolcContext *>(ctx);
    context->mu = 1.;
    context->lambda = 1.;
    context->H = new double*[6];
    for (int i = 0; i < 6; i++) {
        context->H[i] = new double[i + 1];
    }
}

void free_adolc(void *ctx) {
    AdolcContext *context = static_cast<AdolcContext *>(ctx);
    for (int i = 0; i < 6; i++) {
        delete[] context->H[i];
    }
    delete[] context->H;
}

adouble MatDetAM1Symmetric(adouble A_sym[6]) {
    return A_sym[0] * (A_sym[1] * A_sym[2] - A_sym[3] * A_sym[3]) +
           A_sym[5] * (A_sym[3] * A_sym[4] - A_sym[5] * A_sym[2]) +
           A_sym[4] * (A_sym[5] * A_sym[3] - A_sym[4] * A_sym[1]) +
           A_sym[0] + A_sym[1] + A_sym[2] +
           A_sym[0] * A_sym[1] + A_sym[0] * A_sym[2] + A_sym[1] * A_sym[2] -
           A_sym[5] * A_sym[5] - A_sym[4] * A_sym[4] - A_sym[3] * A_sym[3];
}

adouble MatTraceSymmetric(adouble A_sym[6]) {
    return A_sym[0] + A_sym[1] + A_sym[2];
}

adouble Log1pSeries(adouble x) {
    adouble sum = 0;
    adouble y = x / (2. + x);
    adouble y2 = y * y;
    sum += y;
    for (int i = 0; i < 5; i++) {
        y *= y2;
        sum += y / (2 * i + 3);
    }
    return 2 * sum;
}

adouble StrainEnergy_NeoHookeanCurrentAD_ADOLC(adouble e_sym[6], const double lambda, const double mu) {
    adouble E2_sym[6];
    for (int i = 0; i < 6; i++) E2_sym[i] = 2 * e_sym[i];
    adouble detCm1 = MatDetAM1Symmetric(E2_sym);
    adouble J = sqrt(detCm1 + 1);
    adouble logJ = Log1pSeries(detCm1) / 2.;
    adouble traceE = MatTraceSymmetric(e_sym);
    return lambda * (J * J - 1) / 4 - lambda * logJ / 2 + mu * (-logJ + traceE);
}

void ComputeGradPsi(double grad[6], double e_sym[6], AdolcContext *data) {
    double Fp[1];
    adouble ea[6], Fa[1];
    int tag = 1;
    trace_on(tag);
    for (int i = 0; i < 6; i++) ea[i] <<= e_sym[i];
    Fa[0] = StrainEnergy_NeoHookeanCurrentAD_ADOLC(ea, data->lambda, data->mu);
    Fa[0] >>= Fp[0];
    trace_off();
    gradient(tag, 6, e_sym, grad);
    for (int i = 0; i < 6; i++) if (i > 2) grad[i] /= 2.;
}

void ComputeHessianPsi(double hess[6][6], double e_sym[6], AdolcContext *data) {
    double Fp[1];
    adouble ea[6], Fa[1];
    int tag = 1;
    trace_on(tag);
    for (int i = 0; i < 6; i++) ea[i] <<= e_sym[i];
    Fa[0] = StrainEnergy_NeoHookeanCurrentAD_ADOLC(ea, data->lambda, data->mu);
    Fa[0] >>= Fp[0];
    trace_off();
    hessian(tag, 6, e_sym, data->H);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < i + 1; j++) {
            hess[i][j] = data->H[i][j];
            if (i != j) hess[j][i] = hess[i][j];
        }
    }
    for (int i = 3; i < 6; i++) for (int j = 0; j < 6; j++) hess[i][j] /= 2.;
}

void f_adolc(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double dXdx[3][3], double e_sym[6], double f1[3][3]) {
    // Context
    AdolcContext *context = static_cast<AdolcContext *>(ctx);
    double Grad_u[3][3], F_inv[3][3], tau_sym[6];
    // X is natural coordinate sys OR Reference system
    // x_initial is initial config coordinate system
    // Grad_u = du/dx_initial = du/dX * dX/dx_initial
    MatMatMult(1.0, dudX, dXdx_initial, Grad_u);
    // Compute the Deformation Gradient : F = I + Grad_u
    double F[3][3];
    DeformationGradient(Grad_u, F);
    // Compute F^{-1}
    const double Jm1  = MatDetAM1(Grad_u);
    const double detF = Jm1 + 1.;
    MatInverse(F, detF, F_inv);
    // x is current config coordinate system
    // dXdx = dX/dx = dX/dx_initial * F^{-1}
    // Note that F^{-1} = dx_initial/dx
    MatMatMult(1.0, dXdx_initial, F_inv, dXdx);
    // Compute Green Euler Strain tensor (e)
    GreenEulerStrain(Grad_u, e_sym);
    // Compute gradient of Psi (dPsi/de) with ADOL-C
    double gradPsi_sym[6] = {0.};
    ComputeGradPsi(gradPsi_sym, e_sym, context);
    // b = 2 e + I
    double b_sym[6], b[3][3];
    for (int j = 0; j < 6; j++) b_sym[j] = 2. * e_sym[j] + (j < 3);
    SymmetricMatUnpack(b_sym, b);
    // tau = dPsi/de * b
    double tau[3][3], gradPsi[3][3];
    SymmetricMatUnpack(gradPsi_sym, gradPsi);
    MatMatMult(1.0, gradPsi, b, tau);
    SymmetricMatPack(tau, tau_sym);
    SymmetricMatUnpack(tau_sym, f1);
    // ------------------------------------------------------------------------
    // More info
    // ------------------------------------------------------------------------
    printf("\n\ne =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", e_sym[i]);
    printf("\n\nStrain Energy from e = ");
    printf(" %.12lf", StrainEnergy(e_sym, context->lambda, context->mu));
    printf("\n\ntau =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", tau_sym[i]);
}

void df_adolc(void *ctx, double dXdx[3][3], double e_sym[6], const double ddudX[3][3], double df1[3][3]) {
    // Context
    AdolcContext *context = static_cast<AdolcContext *>(ctx);
    double grad_du[3][3], b_sym[6], b[3][3], de_sym[6], dtau_sym[6], tau[3][3], dtau[3][3], tau_grad_du[3][3];
    // Compute grad_du = ddu/dX * dX/dx
    // X is ref coordinate [-1,1]^3; x is physical coordinate in current configuration
    MatMatMult(1.0, ddudX, dXdx, grad_du);
    // Compute b = 2 e + I
    for (int j = 0; j < 6; j++) b_sym[j] = 2 * e_sym[j] + (j < 3);
    SymmetricMatUnpack(b_sym, b);
    // Compute de = db / 2 = (grad_du b + b (grad_du)^T) / 2
    double de[3][3];
    GreenEulerStrain_fwd(grad_du, b, de_sym);
    SymmetricMatUnpack(de_sym, de);
    // ADOL-C
    double gradPsi_sym[6] = {0.};
    ComputeGradPsi(gradPsi_sym, e_sym, context);
    double hessPsi_curr[6][6] = {{0.}};
    ComputeHessianPsi(hessPsi_curr, e_sym, context); // hessian: d2Psi/de2
    // dtau = (hessPsi : de) b + 2 gradPsi (I_4 : de)
    //      = dGradPsi b + 2 gradPsi de
    //      = dtau_1 + dtau_2
    //---------------------------------------------------
    // dGradPsi = hessPsi : de
    double dGradPsi[3][3], dGradPsi_sym[6] = {0.};
    for (int i=0; i<6; i++) for (int j=0; j<6; j++) dGradPsi_sym[i] += hessPsi_curr[i][j] * de_sym[j];
    SymmetricMatUnpack(dGradPsi_sym, dGradPsi);
    // dtau_1 = dGradPsi b
    double dtau_1[3][3];
    MatMatMult(1.0, dGradPsi, b, dtau_1);
    // dtau_2 = 2 gradPsi de
    double dtau_2[3][3], gradPsi[3][3];
    SymmetricMatUnpack(gradPsi_sym, gradPsi);
    MatMatMult(2., gradPsi, de, dtau_2);
    // dtau = dtau_1 + dtau_2
    MatMatAdd(1., dtau_1, 1., dtau_2, dtau);
    SymmetricMatPack(dtau, dtau_sym);
    // Compute tau_grad_du = tau * grad_du^T
    MatMatTransposeMult(tau, grad_du, tau_grad_du);
    // Compute df1 = dtau - tau * grad_du^T
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        df1[j][k] = dtau[j][k] - tau_grad_du[j][k];
      }
    }
    // ------------------------------------------------------------------------
    // More info
    // ------------------------------------------------------------------------
    printf("\n\nde =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", de_sym[i]);
    printf("\n\ndtau =");
    for (int i=0; i<6; i++) printf("\n\t%.12lf", dtau_sym[i]);
    printf("\n\n");
}
