#include "../include/bench.h"
#include "../include/load-data.h"
#include <cstring>
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

int main(int argc, char *argv[]) {
  // Define the AD tools
  std::vector<std::string> ad_tools = {"analytical", "enzyme-c", "enzyme-rust", "tapenade", "adolc"};

  // File setup
  std::string filename = "random-data.csv";
  int mat_elem = 9, Q = QuadraturePointsNumber(filename);
  std::cout << "\nQuadrature Points = "<<Q<<"\n\n";

  // Allocate 2D arrays
  std::vector<std::vector<double>> dXdx_init(mat_elem, std::vector<double>(Q));
  std::vector<std::vector<double>> dudX(mat_elem, std::vector<double>(Q));
  std::vector<std::vector<double>> ddudX(mat_elem, std::vector<double>(Q));

  // Read the data from the CSV file and populate the arrays
  GetData(filename, dXdx_init, dudX, ddudX);

  // Time Residual and Jacobian evaluations
  int tool_width = 15, time_width = 10;
  std::cout << std::left << std::setw(tool_width) << "AD Tool" << std::setw(time_width) << "Time (s)" << std::endl;
  std::cout << std::string(tool_width + time_width + 1, '-') << std::endl;

  double dXdx_init_loc[3][3], dudX_loc[3][3], ddudX_loc[3][3];
  for (const auto& ad_tool : ad_tools) {
    Bench bench;
    if (bench_setup(&bench, ad_tool.c_str()) != 0) {
      std::cerr << "Failed to set up bench for AD tool: " << ad_tool << std::endl;
      continue;
    }
    bench.init(bench.ad_context);

    auto start_time = std::chrono::high_resolution_clock::now();

    // Process each quadrature point
    for (int i = 0; i < Q; i++) {
      double f[3][3]; // df[3][3];
      PackMatrix(i, dXdx_init, dXdx_init_loc);
      PackMatrix(i, dudX, dudX_loc);
      PackMatrix(i, ddudX, ddudX_loc);

      bench.f(bench.ad_context, dXdx_init_loc, dudX_loc, f);
      //bench.df(bench.ad_context, ddudX_loc, df);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << std::left << std::setw(tool_width) << ad_tool << std::setw(time_width) << elapsed.count() << std::endl;
    bench.free(bench.ad_context);
  }
  std::cout << std::endl;

  // Compute error
  Bench bench_ref;
  std::string analytical = "analytical";
  bench_setup(&bench_ref, analytical.c_str());
  bench_ref.init(bench_ref.ad_context);
  int error_width = 10;
  std::cout << std::left << std::setw(tool_width) << "AD Tool" << std::setw(error_width) << "Residual Error" << std::endl;
  std::cout << std::string(tool_width + error_width + 5, '-') << std::endl;
  for (const auto& ad_tool : ad_tools) {
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
      std::cout << std::left << std::setw(tool_width) << ad_tool << std::setw(error_width) << f_total_error << std::endl;
      bench.free(bench.ad_context);
    }
  }
  std::cout << std::endl;
  bench_ref.free(bench_ref.ad_context);

  return 0;
}
