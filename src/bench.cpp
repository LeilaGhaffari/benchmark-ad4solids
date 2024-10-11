#include <iostream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include "../include/bench.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <adolc|enzyme>" << std::endl;
        return 1;
    }

    // Create a bench instance
    Bench bench;
    if (bench_setup(&bench, argv[1]) != 0) {
        return 1;
    }
    bench.init(bench.ad_context);

    double df[3][3], dXdx[3][3], e_sym[6], f[3][3];
    const double dXdx_initial[3][3] = {
      {0.0702417, 0.4799115, 0.3991242},
      {0.6756593, 0.0633284, 0.0959267},
      {0.2241923, 0.0281781, 0.0917613}
    };
    const double dudX[3][3] = {
      {0.1425560,  0.115120,  0.551640},
      {0.0591922,  0.123535,  0.166572},
      {0.1617210,  0.478828,  0.646217}
    };
    double ddudX[3][3] = {
        {0.23617975,  0.60250516,  0.1717169},
        {0.86615524,  0.3365063,   0.17371375},
        {0.0441905,   0.16762188,  0.45047923}
    };

    bench.f(bench.ad_context, dXdx_initial, dudX, dXdx, e_sym, f);
    bench.df(bench.ad_context, dXdx, e_sym, ddudX, df);

    bench.free(bench.ad_context);

    return 0;
}
