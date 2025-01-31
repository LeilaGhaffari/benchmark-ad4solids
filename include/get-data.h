#ifndef GET_DATA_H
#define GET_DATA_H

void PackMatrix(int i, const std::vector<std::vector<double>>& stored, double local[3][3]) {
    local[0][0] = stored[0][i];
    local[0][1] = stored[1][i];
    local[0][2] = stored[2][i];
    local[1][0] = stored[3][i];
    local[1][1] = stored[4][i];
    local[1][2] = stored[5][i];
    local[2][0] = stored[6][i];
    local[2][1] = stored[7][i];
    local[2][2] = stored[8][i];
}

void PrintMatrix(double mat[3][3]) {
    std::cout << "\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << mat[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void GetData(const std::string& filename, int Q, std::vector<double>& dXdx_init,
             std::vector<double>& dudX, std::vector<double>& ddudX) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    std::getline(file, line);  // Skip the header row

    int idx = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;

        double value1, value2, value3;
        std::getline(ss, value, ',');
        value1 = std::stod(value);
        std::getline(ss, value, ',');
        value2 = std::stod(value);
        std::getline(ss, value, ',');
        value3 = std::stod(value);

        dXdx_init[idx] = value1;
        dudX[idx]= value2;
        ddudX[idx] = value3;

        ++idx;
    }
    file.close();
}

int QuadraturePointsNumber(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    int row_count = 0;

    std::getline(file, line); // Skip the header
    while (std::getline(file, line)) {
        ++row_count;
    }

    file.close();
    return row_count / 9;
}

double ComputeError(const double *f, const double *f_ref, int n) {
    double error = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = f[i] - f_ref[i];
        error += diff * diff;
    }
    return std::sqrt(error);
}

// Helper function for timing and displaying results for the residual evaluation
void TimeAndDisplayOperation(const std::vector<std::string> &ad_tools,
                             int Q,
                             std::vector<double>& dXdx_init,
                             std::vector<double>& dudX,
                             std::vector<double>& ddudX,
                             const std::string &operation) {
    int tool_width = 15, time_width = 16;
    const double mu = 1., lambda = 1.0;
    for (const auto &ad_tool : ad_tools) {
        bool enzyme_rust_df = (ad_tool == "enzyme-rust") && (operation == "df");
        if (!enzyme_rust_df) {
            Bench bench;
            if (bench_setup(&bench, ad_tool.c_str()) != 0) {
                std::cerr << "Failed to set up bench for AD tool: " << ad_tool << std::endl;
                return;
            }
            double *stored_values = NULL;
            bench.init_data(&stored_values, Q);
            auto start_time = std::chrono::high_resolution_clock::now();
            double *f = (double *)malloc(Q * 9 * sizeof(double));
            double *df = (double *)malloc(Q * 9 * sizeof(double));
            if (operation == "f") {
                bench.f(Q, mu, lambda, dXdx_init.data(), dudX.data(), &stored_values, f);
            } else if (operation == "df") {
                bench.df(Q, mu, lambda, ddudX.data(), &stored_values, df);
            } else {
                std::cerr << "Invalid operation specified: " << operation << std::endl;
                return;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;
            std::cout << std::left << std::setw(tool_width) << ad_tool
                      << std::setw(time_width) << elapsed.count() << std::endl;
            bench.free_data(&stored_values);
        }
    }
    std::cout << std::endl;
}

//Helper function to compute errors in residual evaluation
void ComputeAndDisplayErrors(const std::vector<std::string> &ad_tools, int Q,
                             std::vector<double>& dXdx_init,
                             std::vector<double>& dudX,
                             std::vector<double>& ddudX,
                             const std::string &operation) {
    Bench bench_ref;
    const double mu = 1., lambda = 1.0;
    const std::string &analytical = "analytical";
    bench_setup(&bench_ref, analytical.c_str());
    double *stored_values_ref = NULL;
    bench_ref.init_data(&stored_values_ref, Q);
    int tool_width = 15, error_width = 10;
    for (const auto &ad_tool : ad_tools) {
        bool enzyme_rust_df = (ad_tool == "enzyme-rust") && (operation == "df");
        if ((ad_tool != analytical) && !enzyme_rust_df) {
            Bench bench;
            if (bench_setup(&bench, ad_tool.c_str()) != 0) {
                std::cerr << "Failed to set up bench for AD tool: " << ad_tool << std::endl;
                continue;
            }
            double *stored_values = NULL;
            bench.init_data(&stored_values, Q);
            double f_total_error = 0.0;
            for (int i = 0; i < Q; i++) {
                double *f = (double *)malloc(Q * 9 * sizeof(double));
                double *f_ref = (double *)malloc(Q * 9 * sizeof(double));
                double *df = (double *)malloc(Q * 9 * sizeof(double));
                double *df_ref = (double *)malloc(Q * 9 * sizeof(double));
                if (operation == "f") {
                    bench.f(Q, mu, lambda, dXdx_init.data(), dudX.data(), &stored_values, f);
                    bench_ref.f(Q, mu, lambda, dXdx_init.data(), dudX.data(), &stored_values_ref, f_ref);
                    f_total_error += ComputeError(f, f_ref, Q*9);
                } else if (operation == "df") {
                    bench.df(Q, mu, lambda, ddudX.data(), &stored_values, df);
                    bench_ref.df(Q, mu, lambda, ddudX.data(), &stored_values_ref, df_ref);
                    f_total_error += ComputeError(df, df_ref, Q*9);
                } else {
                    std::cerr << "Invalid operation specified: " << operation << std::endl;
                    return;
                }
            }
            std::cout << std::left << std::setw(tool_width) << ad_tool
                      << std::setw(error_width) << f_total_error << std::endl;
            bench.free_data(&stored_values);
        }
    }
    std::cout << std::endl;
    bench_ref.free_data(&stored_values_ref);
}

#endif // GET_DATA_H
