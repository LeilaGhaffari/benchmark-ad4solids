#include "../../include/ad-tools/enzyme-rust.h"

void init_enzyme_rust(void *ctx) {
    EnzymeRustContext *context = static_cast<EnzymeRustContext *>(ctx);
    context->mu = 1.;
    context->lambda = 1.;
    context->stored = new double[NUM_COMPONENTS_STORED_ENZYME_RUST];
}

void free_enzyme_rust(void *ctx) {
    EnzymeRustContext *context = static_cast<EnzymeRustContext *>(ctx);
    delete[] context->stored;
    delete context;
}

void tau_sym_enzyme_rust(const double e_sym[6], const double lambda, const double mu, double tau_sym[6]) {
    compute_stress(lambda, mu, e_sym, tau_sym);
}

// Residual Evaluation
void f_enzyme_rust(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]) {
  // Context
  const EnzymeRustContext *context = (EnzymeRustContext *)ctx;
  const double mu = context->mu;
  const double lambda = context->lambda;
  double *stored_values = context->stored; // TODO: move up to argument

  double Grad_u[3][3], F_inv[3][3], tau_sym[6], dXdx[3][3], e_sym[6];

  MatMatMult(1.0, dudX, dXdx_initial, Grad_u);

  double F[3][3];
  DeformationGradient(Grad_u, F);

  const double Jm1  = MatDetAM1(Grad_u);
  const double detF = Jm1 + 1.0;
  MatInverse(F, detF, F_inv);

  MatMatMult(1.0, dXdx_initial, F_inv, dXdx);

  GreenEulerStrain(Grad_u, e_sym);

  tau_sym_enzyme_rust(e_sym, lambda, mu, tau_sym);
  SymmetricMatUnpack(tau_sym, f1);

  // ------------------------------------------------------------------------
  // Store
  // ------------------------------------------------------------------------
  StoredValuesPack(0, 9, (double *)dXdx, stored_values);
  StoredValuesPack(9, 6, (double *)e_sym, stored_values);
}

void df_enzyme_rust(void *ctx, const double ddudX[3][3], double df1[3][3]) {

}
