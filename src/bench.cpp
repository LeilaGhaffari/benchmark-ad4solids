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
  TimeAndDisplayOperation(ad_tools, Q, dXdx_init, dudX, ddudX, "f");
  ComputeAndDisplayErrors("analytical", ad_tools, Q, dXdx_init, dudX, ddudX);

  return 0;
}
