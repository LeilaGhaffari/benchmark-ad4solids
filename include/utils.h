#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern int SymmetricMatUnpack(const double sym[6], double full[3][3]);
extern int SymmetricMatPack(const double full[3][3], double sym[6]);
extern int MatInverse(const double A[3][3], double det_A, double A_inv[3][3]);
extern int MatMatMult(double alpha, const double A[3][3], const double B[3][3], double C[3][3]);
extern int MatComputeInverseSymmetric(const double A[3][3], const double det_A, double A_inv[6]);
extern double MatDetAM1Symmetric(const double A_sym[6]);
extern double MatTraceSymmetric(const double A_sym[6]);
extern double MatDetAM1(const double A[3][3]);
extern void DeformationGradient(double grad_u[3][3], double F[3][3]);
extern double Log1pSeries(double x);
extern int LinearStrain(const double grad_u[3][3], double e_sym[6]);
extern int GreenEulerStrain(const double grad_u[3][3], double e_sym[6]);
extern int GreenEulerStrain_fwd(const double grad_du[3][3], const double b[3][3], double de_sym[6]);
extern double StrainEnergy(double e_sym[6], const double lambda, const double mu);
extern int MatMatTransposeMult(const double A[3][3], const double B[3][3], double C[3][3]);
extern void PullBack_symmetric(double Grad_u[3][3], double a_sym[6], double A_sym[6]);
extern void dPullBack_symmetric(double F_inv[3][3], double dF[3][3], double a_sym[6], double da_sym[6], double dA_sym[6]);
extern void PushForward_symmetric(double F[3][3], double A_sym[6], double a_sym[6]);
extern void dPushForward_symmetric(const double F[3][3], double dF[3][3], double A_sym[6], double dA_sym[6], double da_sym[6]);
extern void MatTransposeMatMult(double alpha, const double A[3][3], const double B[3][3], double C[3][3]);
extern int MatMatAdd(double alpha, const double A[3][3], double beta, const double B[3][3], double C[3][3]);

#endif // UTILS_H
