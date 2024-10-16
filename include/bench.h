#ifndef BENCH_H
#define BENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adolc.h"
#include "enzyme.h"

typedef struct Bench {
    void *ad_context;
    void (*init)(void *ctx);
    void (*free)(void *ctx);
    void (*f)(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double dXdx[3][3], double e_sym[6], double f1[3][3]);
    void (*df)(void *ctx, double dXdx[3][3], double e_sym[6], const double ddudX[3][3], double df1[3][3]);
} Bench;

int bench_setup(Bench *bench, const char *tool) {
    if (strcmp(tool, "adolc") == 0) {
        bench->ad_context = malloc(sizeof(AdolcContext));
        bench->init = init_adolc;
        bench->free = free_adolc;
        bench->f = f_adolc;
        bench->df = df_adolc;
    } else if (strcmp(tool, "enzyme") == 0) {
        bench->ad_context = malloc(sizeof(EnzymeContext));
        bench->init = init_enzyme;
        bench->free = free_enzyme;
        bench->f = f_enzyme;
        bench->df = df_enzyme;
    } else {
        printf("Unknown AD tool: %s\n", tool);
        printf("Valid options are: adolc, enzyme\n");
        return 1;
    }
    return 0;
}

#endif // BENCH_H
