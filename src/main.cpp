#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "ekf_soc.hpp"
#include "matrix.hpp"


void readCSV(const std::string& filename,
             std::vector<float>& current,
             std::vector<float>& voltage,
             std::vector<float>& temperature,
             std::vector<float>& soc)
{
    std::ifstream file(filename);
    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string val;
        float c, v, t, s;

        std::getline(ss, val, ','); c = std::stof(val);
        std::getline(ss, val, ','); v = std::stof(val);
        std::getline(ss, val, ','); t = std::stof(val);
        std::getline(ss, val, ','); s = std::stof(val);

        current.push_back(c);
        voltage.push_back(v);
        temperature.push_back(t);
        soc.push_back(s);
    }
}

void writeCSV(const std::string& filename,
              const std::vector<float>& current,
              const std::vector<float>& voltage,
              const std::vector<float>& temperature,
              const std::vector<float>& soc,
              const std::vector<float>& soc_est,
              const std::vector<float>& x_hat1,
              const std::vector<float>& x_hat2)
{
    std::ofstream out(filename);
    out << "Current,Voltage,Temperature,SOC,SOC_Estimate,x_hat1,x_hat2\n";
    for (size_t i = 0; i < current.size(); i++) {
        out << current[i] << ","
            << voltage[i] << ","
            << temperature[i] << ","
            << soc[i] << ","
            << soc_est[i] << ","
            << x_hat1[i] << ","
            << x_hat2[i] << "\n";
    }
}

int main() {
    std::cout << "EKF-SoC running.\n";

    // Matrix test
    // Matrix A {2, 2, {1, 2, 3, 4}};
    // Matrix B {2, 2, {5, 6, 7, 8}};
    // (A * B).transpose().print();

    std::vector<float> current, voltage, temperature, soc, soc_est, x_hat1, x_hat2;
    readCSV("signals.csv", current, voltage, temperature, soc);

    EKF_SOC ekf_soc;

    for (int i = 0; i < current.size(); ++i) {
        soc_est.push_back(ekf_soc.get_SOC_estimate());
        x_hat1.push_back(ekf_soc.get_x_hat1());
        x_hat2.push_back(ekf_soc.get_x_hat2());
        ekf_soc.step(current[i], voltage[i], temperature[i]);
    }

    writeCSV("signals_with_est.csv", current, voltage, temperature, soc, soc_est, x_hat1, x_hat2);

    std::cout << "EKF-SoC complete.\n";
    return 0;
}