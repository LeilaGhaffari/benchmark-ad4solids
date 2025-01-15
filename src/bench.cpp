#include "../include/bench.h"
#include "../include/get-data.h"

int main(int argc, char *argv[]) {
  // AD tools
  std::vector<std::string> ad_tools = {"analytical", "enzyme-c", "enzyme-rust", "tapenade", "adolc"};

  // File setup
  std::string filename = "random-data.csv";
  int mat_elem = 9, Q = QuadraturePointsNumber(filename);
  std::cout << "\nQuadrature Points = " << Q << "\n\n";

  // Allocate input arrays
  std::vector<std::vector<double>> dXdx_init(mat_elem, std::vector<double>(Q));
  std::vector<std::vector<double>> dudX(mat_elem, std::vector<double>(Q));
  std::vector<std::vector<double>> ddudX(mat_elem, std::vector<double>(Q));

  // Read the data from the CSV file
  GetData(filename, dXdx_init, dudX, ddudX);

  // Residual evaluation - Time and Error
  int tool_width = 15, time_width = 16, error_width = 10;
  std::cout << std::string(tool_width + time_width + 1, '*') << std::endl
            << "          Residual\n"
            << std::string(tool_width + time_width + 1, '*') << std::endl;
  std::cout << std::left << std::setw(tool_width) << "AD Tool"
            << std::setw(time_width) << "Time (s)" << std::endl;
  std::cout << std::string(tool_width + time_width + 1, '-') << std::endl;
  TimeAndDisplayOperation(ad_tools, Q, dXdx_init, dudX, ddudX, "f");

  std::cout << std::left << std::setw(tool_width) << "AD Tool"
            << std::setw(error_width) << "Error" << std::endl;
  std::cout << std::string(tool_width + error_width + 5, '-') << std::endl;
  ComputeAndDisplayErrors(ad_tools, Q, dXdx_init, dudX, ddudX, "f");

  // Jacobian evaluation - Time and Error
  std::cout << std::string(tool_width + time_width + 1, '*') << std::endl
            << "          Jacobian\n"
            << std::string(tool_width + time_width + 1, '*') << std::endl;
  std::cout << std::left << std::setw(tool_width) << "AD Tool"
            << std::setw(time_width) << "Time (s)" << std::endl;
  std::cout << std::string(tool_width + time_width + 1, '-') << std::endl;
  TimeAndDisplayOperation(ad_tools, Q, dXdx_init, dudX, ddudX, "df");

  std::cout << std::left << std::setw(tool_width) << "AD Tool"
            << std::setw(error_width) << "Error" << std::endl;
  std::cout << std::string(tool_width + error_width + 5, '-') << std::endl;
  ComputeAndDisplayErrors(ad_tools, Q, dXdx_init, dudX, ddudX, "df");

  return 0;
}
