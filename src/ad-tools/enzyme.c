#include "../../include/ad-tools/enzyme.h"
#include <math.h>
#include <stdlib.h>

void init_enzyme(void *ctx) {
    EnzymeContext *context = (EnzymeContext*)ctx;
    context->lambda = 1.;
    context->mu = 1.;
    context->stored = malloc(NUM_COMPONENTS_STORED_ENZYME * sizeof(double));
}

void free_enzyme(void *ctx) {
    EnzymeContext *context = (EnzymeContext*)ctx;
    free(context->stored);
    free(context);
}

double  StrainEnergy_NeoHookeanCurrentAD_Enzyme(double e_sym[6], double lambda, double mu) {
  double e2_sym[6];

  // J and log(J)
  for (int i = 0; i < 6; i++) e2_sym[i] = 2 * e_sym[i];
  const double detbm1 = MatDetAM1Symmetric(e2_sym);
  const double J      = sqrt(detbm1 + 1);
  const double logJ   = Log1pSeries(detbm1) / 2.;

  // trace(e)
  const double trace_e = MatTraceSymmetric(e_sym);

  return lambda * (J * J - 1) / 4 - lambda * logJ / 2 + mu * (-logJ + trace_e);
}

//void *__enzyme_function_like[2] = {(void *)Log1pSeries, (void *)"log1p"};

void Kirchhofftau_sym_NeoHookean_AD_Enzyme(const double lambda, const double mu, double e_sym[6], double tau_sym[6]) {
  double dPsi_sym[6] = {0.}, b_sym[6], dPsi[3][3], b[3][3], tau[3][3];

  // dPsi / de
  __enzyme_autodiff((void *)StrainEnergy_NeoHookeanCurrentAD_Enzyme, e_sym, dPsi_sym, enzyme_const, lambda, enzyme_const, mu);
  for (int i = 3; i < 6; i++) dPsi_sym[i] /= 2.;

  // b = 2 e + I
  for (int j = 0; j < 6; j++) b_sym[j] = 2 * e_sym[j] + (j < 3);

  // tau = (dPsi / de) b
  SymmetricMatUnpack(dPsi_sym, dPsi);
  SymmetricMatUnpack(b_sym, b);
  MatMatMult(1., dPsi, b, tau);
  SymmetricMatPack(tau, tau_sym);
}

void dtau_fwd_Enzyme(const double lambda, const double mu, double e_sym[6], double de_sym[6],
                                         double tau_sym[6], double dtau_sym[6]) {
  __enzyme_fwddiff((void *)Kirchhofftau_sym_NeoHookean_AD_Enzyme, enzyme_const, lambda, enzyme_const, mu, e_sym, de_sym, tau_sym, dtau_sym);
}

// Residual Evaluation
void f_enzyme(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]) {
  // Context
  const EnzymeContext *context = (EnzymeContext *)ctx;
  const double mu = context->mu;
  const double lambda = context->lambda;
  double *stored_values = context->stored;

  double Grad_u[3][3], F_inv[3][3], tau_sym[6], dXdx[3][3], e_sym[6];

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

  // ------------------------------------------------------------------------
  // Store
  // ------------------------------------------------------------------------
  StoredValuesPack(0, 9, (double *)dXdx, stored_values);
  StoredValuesPack(9, 6, (double *)e_sym, stored_values);
}

// Jacobian Evaluation
void df_enzyme(void *ctx, const double ddudX[3][3], double df1[3][3]) {
  // Context
  const EnzymeContext *context = (EnzymeContext *)ctx;
  const double mu = context->mu;
  const double lambda = context->lambda;
  const double *stored_values = context->stored;

  double grad_du[3][3], b_sym[6], b[3][3], de_sym[6], tau_sym[6], dtau_sym[6], tau[3][3], dtau[3][3],
         tau_grad_du[3][3], dXdx[3][3], e_sym[6];
  // ------------------------------------------------------------------------
  // Unpack stored values
  // ------------------------------------------------------------------------
  StoredValuesUnpack(0, 9,  stored_values, (double *)dXdx);
  StoredValuesUnpack(9, 6,  stored_values, (double *)e_sym);

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
