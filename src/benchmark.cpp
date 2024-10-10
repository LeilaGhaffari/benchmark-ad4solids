#include <iostream>
#include <cstring>
#include "../include/common.h"
#include "../include/adolc.h"
#include "../include/enzyme.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <adolc|enzyme>" << std::endl;
        return 1;
    }

    NeoHookeanElasticityContext ctx;

    // Choose the AD tool based on the command-line argument
    if (strcmp(argv[1], "adolc") == 0) {
        InitializeAdolcContext(&ctx); // TODO: define an init function which takes ctx and initializes ctx based on the CL argument
        std::cout << "Initialized with ADOL-C." << std::endl;
    } else if (strcmp(argv[1], "enzyme") == 0) {
        InitializeEnzymeContext(&ctx);
        std::cout << "Initialized with Enzyme." << std::endl;
    } else {
        std::cout << "Unknown AD tool: " << argv[1] << std::endl;
        std::cout << "Valid options are: adolc, enzyme" << std::endl;
        return 1;
    }

    // Initialize the context
    ctx.initialize(&ctx);

    // Read random data points (TODO)

    // Residual evaluation
    double dXdx[3][3], dudX[3][3], e_sym[6], f1_out[3][3];
    ctx.f1(&ctx, dXdx, dudX, e_sym, f1_out);

    // Jacobian evaluation
    double ddudX[3][3], df1_out[3][3];
    ctx.df1(&ctx, dXdx, e_sym, ddudX, df1_out);

    // Cleanup
    ctx.free(&ctx);

    return 0;
}
