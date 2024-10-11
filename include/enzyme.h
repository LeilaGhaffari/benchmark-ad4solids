#ifndef ENZYME_H
#define ENZYME_H

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double lambda;
    double mu;
} EnzymeContext;

void init_enzyme(void *ctx);
void free_enzyme(void *ctx);
void f_enzyme(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double dXdx[3][3], double e_sym[6], double f1[3][3]);
void df_enzyme(void *ctx, double dXdx[3][3], double e_sym[6], const double ddudX[3][3], double df1[3][3]);


int SymmetricMatUnpack_Enzyme(const double sym[6], double full[3][3]) {
  full[0][0] = sym[0];
  full[0][1] = sym[5];
  full[0][2] = sym[4];
  full[1][0] = sym[5];
  full[1][1] = sym[1];
  full[1][2] = sym[3];
  full[2][0] = sym[4];
  full[2][1] = sym[3];
  full[2][2] = sym[2];
  return 0;
};
double MatTraceSymmetric_Enzyme(const double A_sym[6]) { return A_sym[0] + A_sym[1] + A_sym[2]; };
double Log1pSeries_Enzyme(double x) {
    double sum = 0;
    double y = x / (2. + x);
    double y2 = y*y;
    sum += y;
    for (int i=0; i<5; i++) {
      y *= y2;
      sum += y / (2*i + 3);
    }
    return 2 * sum;
};
double MatDetAM1Symmetric_Enzyme(const double A_sym[6]) {
  return A_sym[0] * (A_sym[1] * A_sym[2] - A_sym[3] * A_sym[3]) +
         A_sym[5] * (A_sym[3] * A_sym[4] - A_sym[5] * A_sym[2]) +
         A_sym[4] * (A_sym[5] * A_sym[3] - A_sym[4] * A_sym[1]) +
         A_sym[0] + A_sym[1] + A_sym[2] +
         A_sym[0] * A_sym[1] + A_sym[0] * A_sym[2] + A_sym[1] * A_sym[2] -
         A_sym[5] * A_sym[5] - A_sym[4] * A_sym[4] - A_sym[3] * A_sym[3];
};
int MatMatMult_Enzyme(double alpha, const double A[3][3], const double B[3][3], double C[3][3]) {
  for (int j = 0; j < 3; j++) {
    for (int k = 0; k < 3; k++) {
      C[j][k] = 0;
      for (int m = 0; m < 3; m++) {
        C[j][k] += alpha * A[j][m] * B[m][k];
      }
    }
  }
  return 0;
};
int SymmetricMatPack_Enzyme(const double full[3][3], double sym[6]) {
  sym[0] = full[0][0];
  sym[1] = full[1][1];
  sym[2] = full[2][2];
  sym[3] = full[1][2];
  sym[4] = full[0][2];
  sym[5] = full[0][1];
  return 0;
};

void __enzyme_autodiff(void *, ...);
void __enzyme_fwddiff(void *, ...);
extern int enzyme_const;

double  StrainEnergy_NeoHookeanCurrentAD_Enzyme(double e_sym[6], double lambda, double mu) {
  double e2_sym[6];

  // J and log(J)
  for (int i = 0; i < 6; i++) e2_sym[i] = 2 * e_sym[i];
  const double detbm1 = MatDetAM1Symmetric_Enzyme(e2_sym);
  const double J      = sqrt(detbm1 + 1);
  const double logJ   = Log1pSeries_Enzyme(detbm1) / 2.;

  // trace(e)
  const double trace_e = MatTraceSymmetric_Enzyme(e_sym);

  return lambda * (J * J - 1) / 4 - lambda * logJ / 2 + mu * (-logJ + trace_e);
}

//void *__enzyme_function_like[2] = {(void *)Log1pSeries_Enzyme, (void *)"log1p"};

void Kirchhofftau_sym_NeoHookean_AD_Enzyme(const double lambda, const double mu, double e_sym[6], double tau_sym[6]) {
  double dPsi_sym[6] = {0.}, b_sym[6], dPsi[3][3], b[3][3], tau[3][3];

  // dPsi / de
  __enzyme_autodiff((void *)StrainEnergy_NeoHookeanCurrentAD_Enzyme, e_sym, dPsi_sym, enzyme_const, lambda, enzyme_const, mu);
  for (int i = 3; i < 6; i++) dPsi_sym[i] /= 2.;

  // b = 2 e + I
  for (int j = 0; j < 6; j++) b_sym[j] = 2 * e_sym[j] + (j < 3);

  // tau = (dPsi / de) b
  SymmetricMatUnpack_Enzyme(dPsi_sym, dPsi);
  SymmetricMatUnpack_Enzyme(b_sym, b);
  MatMatMult_Enzyme(1., dPsi, b, tau);
  SymmetricMatPack_Enzyme(tau, tau_sym);
}

void dtau_fwd_Enzyme(const double lambda, const double mu, double e_sym[6], double de_sym[6],
                                         double tau_sym[6], double dtau_sym[6]) {
  __enzyme_fwddiff((void *)Kirchhofftau_sym_NeoHookean_AD_Enzyme, enzyme_const, lambda, enzyme_const, mu, e_sym, de_sym, tau_sym, dtau_sym);
}

#ifdef __cplusplus
}
#endif

#endif // ENZYME_H
