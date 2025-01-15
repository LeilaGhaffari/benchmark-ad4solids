#ifndef LOAD_DATA_H
#define LOAD_DATA_H

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

void GetData(const std::string& filename, std::vector<std::vector<double>>& dXdx_init,
             std::vector<std::vector<double>>& dudX, std::vector<std::vector<double>>& ddudX) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    std::getline(file, line);  // Skip the header row

    int row = 0, mat_elem = 9;
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

        int i = row / mat_elem;   // quadrature point
        int idx = row % mat_elem; // matrix index

        dXdx_init[idx][i] = value1;
        dudX[idx][i] = value2;
        ddudX[idx][i] = value3;

        ++row;
    }
    file.close();
}

int QuadraturePointsNumber(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file\n";
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

double ComputeError(const double f[3][3], const double f_analytical[3][3]) {
    double error = 0.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double diff = f[i][j] - f_analytical[i][j];
            error += diff * diff;
        }
    }
    return std::sqrt(error);
}

// Helper function for timing and displaying results for the residual evaluation
void TimeAndDisplayOperation(const std::vector<std::string> &ad_tools,
                            int Q,
                            const std::vector<std::vector<double>> &dXdx_init,
                            const std::vector<std::vector<double>> &dudX,
                            const std::vector<std::vector<double>> &ddudX,
                            const std::string &operation) {
    double dXdx_init_loc[3][3], dudX_loc[3][3], ddudX_loc[3][3];
    int tool_width = 15, time_width = 16;
    std::cout << std::left << std::setw(tool_width) << "AD Tool"
              << std::setw(time_width) << "Residual Time (s)" << std::endl;
    std::cout << std::string(tool_width + time_width + 1, '-') << std::endl;
    for (const auto &ad_tool : ad_tools) {
        Bench bench;
        if (bench_setup(&bench, ad_tool.c_str()) != 0) {
            std::cerr << "Failed to set up bench for AD tool: " << ad_tool << std::endl;
            return;
        }
        bench.init(bench.ad_context);
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < Q; i++) {
            double f[3][3], df[3][3];
            PackMatrix(i, dXdx_init, dXdx_init_loc);
            PackMatrix(i, dudX, dudX_loc);
            PackMatrix(i, ddudX, ddudX_loc);
            if (operation == "f") {
                bench.f(bench.ad_context, dXdx_init_loc, dudX_loc, f);
            } else if (operation == "df") {
                bench.df(bench.ad_context, ddudX_loc, df);
            } else {
                std::cerr << "Invalid operation specified: " << operation << std::endl;
                return;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << std::left << std::setw(tool_width) << ad_tool
                  << std::setw(time_width) << elapsed.count() << std::endl;
        bench.free(bench.ad_context);
    }
    std::cout << std::endl;
}

// Helper function to compute errors in residual evaluation
void ComputeAndDisplayErrors(const std::string &analytical,
                             const std::vector<std::string> &ad_tools, int Q,
                             const std::vector<std::vector<double>> &dXdx_init,
                             const std::vector<std::vector<double>> &dudX,
                             const std::vector<std::vector<double>> &ddudX) {
    Bench bench_ref;
    bench_setup(&bench_ref, analytical.c_str());
    bench_ref.init(bench_ref.ad_context);

    double dXdx_init_loc[3][3], dudX_loc[3][3], ddudX_loc[3][3];
    int tool_width = 15, error_width = 10;
    std::cout << std::left << std::setw(tool_width) << "AD Tool"
              << std::setw(error_width) << "Residual Error" << std::endl;
    std::cout << std::string(tool_width + error_width + 5, '-') << std::endl;

    for (const auto &ad_tool : ad_tools) {
        if (ad_tool != analytical) {
            Bench bench;
            if (bench_setup(&bench, ad_tool.c_str()) != 0) {
                std::cerr << "Failed to set up bench for AD tool: " << ad_tool << std::endl;
                continue;
            }
            bench.init(bench.ad_context);

            double f_total_error = 0.0;
            for (int i = 0; i < Q; i++) {
                double f[3][3], f_ref[3][3];
                PackMatrix(i, dXdx_init, dXdx_init_loc);
                PackMatrix(i, dudX, dudX_loc);
                PackMatrix(i, ddudX, ddudX_loc);

                bench.f(bench.ad_context, dXdx_init_loc, dudX_loc, f);
                bench_ref.f(bench_ref.ad_context, dXdx_init_loc, dudX_loc, f_ref);
                f_total_error += ComputeError(f, f_ref);
            }

            std::cout << std::left << std::setw(tool_width) << ad_tool
                      << std::setw(error_width) << f_total_error << std::endl;

            bench.free(bench.ad_context);
        }
    }
    std::cout << std::endl;
    bench_ref.free(bench_ref.ad_context);
}

#endif // LOAD_DATA_H
