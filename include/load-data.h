#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

void PackMatrix(int i, const vector<vector<double>>& stored, double local[3][3]) {
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
    cout << "\n";
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) cout << mat[i][j] << " ";
        cout << "\n";
    }
    cout << "\n";
}

// Function to read data from CSV into 2D vector arrays
void GetData(const string& filename, vector<vector<double>>& dXdx_init,
             vector<vector<double>>& dudX, vector<vector<double>>& ddudX) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }

    string line;
    getline(file, line);  // Skip the header row

    int row = 0, mat_elem = 9;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        // Extract values from the line
        double value1, value2, value3;
        getline(ss, value, ',');
        value1 = stod(value);
        getline(ss, value, ',');
        value2 = stod(value);
        getline(ss, value, ',');
        value3 = stod(value);

        int i = row / mat_elem;   // quadrature point
        int idx = row % mat_elem; // matrix index

        // Store the values
        dXdx_init[idx][i] = value1;
        dudX[idx][i] = value2;
        ddudX[idx][i] = value3;

        ++row;
    }
    file.close();
}

// Function to determine the number of rows in the CSV file
int QuadraturePointsNumber(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file\n";
        exit(EXIT_FAILURE);
    }

    string line;
    int row_count = 0;

    // Skip the header
    getline(file, line);

    // Count the remaining rows
    while (getline(file, line)) {
        ++row_count;
    }

    file.close();
    return row_count/9;
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

#endif // LOAD_DATA_H
