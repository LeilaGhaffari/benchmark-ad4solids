#ifndef BENCH_H
#define BENCH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <cstring>
#include "ad-tools/adolc.h"
#include "ad-tools/enzyme.h"
#include "ad-tools/enzyme-rust.h"
#include "ad-tools/analytical.h"
#include "ad-tools/tapenade.h"

#define SETUP_BENCH(tool_name, context_type)    \
    bench->ad_context = new context_type;       \
    bench->init = init_##tool_name;             \
    bench->free = free_##tool_name;             \
    bench->f = f_##tool_name;                   \
    bench->df = df_##tool_name;

typedef struct Bench {
    void *ad_context;
    void (*init)(void *ctx);
    void (*free)(void *ctx);
    void (*f)(void *ctx, const double dXdx_initial[3][3], const double dudX[3][3], double f1[3][3]);
    void (*df)(void *ctx, const double ddudX[3][3], double df1[3][3]);
} Bench;

int bench_setup(Bench *bench, const char *tool) {
    if (strcmp(tool, "adolc") == 0) {
        SETUP_BENCH(adolc, AdolcContext);
    } else if (strcmp(tool, "enzyme-c") == 0) {
        SETUP_BENCH(enzyme, EnzymeContext);
    } else if (strcmp(tool, "enzyme-rust") == 0) {
        SETUP_BENCH(enzyme_rust, EnzymeRustContext);
    } else if (strcmp(tool, "tapenade") == 0) {
        SETUP_BENCH(tapenade, TapenadeContext);
    } else if (strcmp(tool, "analytical") == 0) {
        SETUP_BENCH(analytic, AnalyticContext);
    } else {
        printf("Unknown model: %s\n", tool);
        printf("Valid options are: analytical, adolc, enzyme-c, enzyme-rust, and tapenade\n");
        return 1;
    }
    return 0;
}

#endif // BENCH_H
