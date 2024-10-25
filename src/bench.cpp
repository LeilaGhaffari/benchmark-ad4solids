#include <iostream>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include "../include/load-data.h"
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

    string filename = "random_data.csv";
    int mat_elem = 9, Q = QuadraturePointsNumber(filename);

    // Allocate 2D arrays
    vector<vector<double>> dXdx_init(mat_elem, vector<double>(Q));
    vector<vector<double>> dudX(mat_elem, vector<double>(Q));
    vector<vector<double>> ddudX(mat_elem, vector<double>(Q));

    // Read the data from the CSV file and populate the arrays
    GetData(filename, dXdx_init, dudX, ddudX);

    double dXdx_init_loc[3][3], dudX_loc[3][3], ddudX_loc[3][3];
    for (int i=0; i<Q; i++) {
        cout << "---------------------" << "\n";
        cout << "qpt = " << i << "\n\n";
        double df[3][3], f[3][3];
        PackMatrix(i, dXdx_init, dXdx_init_loc);
        PackMatrix(i, dudX, dudX_loc);
        PackMatrix(i, ddudX, ddudX_loc);
        bench.f(bench.ad_context, dXdx_init_loc, dudX_loc, f);
        bench.df(bench.ad_context, ddudX_loc, df);
    }
    bench.free(bench.ad_context);
    return 0;
}
