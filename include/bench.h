#ifndef BENCH_H
#define BENCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adolc.h"
#include "enzyme.h"
#include "analytical.h"
#include "tapenade.h"

typedef struct Bench {
    void *ad_context;
    void (*init)(void *ctx);
    void (*free)(void *ctx);
    void (*f)(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]);
    void (*df)(void *ctx, const double ddudX[3][3], double df1[3][3]);
} Bench;

int bench_setup(Bench *bench, const char *tool) {
    if (strcmp(tool, "adolc") == 0) {
        bench->ad_context = new AdolcContext;
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
    } else if (strcmp(tool, "tapenade") == 0) {
        bench->ad_context = new TapenadeContext;
        bench->init = init_tapenade;
        bench->free = free_tapenade;
        bench->f = f_tapenade;
        bench->df = df_tapenade;
    } else if (strcmp(tool, "analytical") == 0) {
        bench->ad_context = new AnalyticContext;
        bench->init = init_analytic;
        bench->free = free_analytic;
        bench->f = f_analytic;
        bench->df = df_analytic;
    } else {
        printf("Unknown model: %s\n", tool);
        printf("Valid options are: analytical, adolc, enzyme, and tapenade\n");
        return 1;
    }
    return 0;
}

#endif // BENCH_H
