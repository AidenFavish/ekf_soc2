#ifndef EKF_SOC_H
#define EKF_SOC_H

#include "matrix.hpp"
#include <cmath>

class EKF_SOC {
public:
    // Equations derived from the following matlab article
    // https://www.mathworks.com/help/simscape-battery/ref/socestimatorkalmanfilter.html

    // System Parameters
    const Matrix SOC_vec {1, 7, {0.0f, .1f, .25f, .5f, .75f, .9f, 1.0f}};  // Vector of state-of-charge values, SOC
    const Matrix T_vec {1, 1, {300.0f}};  // Vector of temperatures, T, (K)
    const float AH {27.0f};  // Cell capacity, AH, (A*hr)
    const Matrix V0_mat {7, 1, {3.5f, 3.57f, 3.63f, 3.71f, 3.93f, 4.08f, 4.19f}};  // Open-circuit voltage, V0(SOC,T), (V)
    const Matrix R0_mat {7, 1, {0.0085f, 0.0085f, 0.0087f, 0.0082f, 0.0083f, 0.0085f, 0.0085f}};  // Terminal resistance, R0(SOC,T), (ohm)
    const Matrix R1_mat {7, 1, {0.0029f, 0.0024f, 0.0026f, 0.0016f, 0.0023f, 0.0018f, 0.0017f}};  // First polarization resistance, R1(SOC,T), (ohm)
    const Matrix tau1_mat {7, 1, {36.0f, 45.0f, 105.0f, 29.0f, 77.0f, 33.0f, 39.0f}};  // First time constant, tau1(SOC,T), (s)
    Matrix dV_dSOC {7, 1};

    // Kalman Filter Parameters
    const Matrix Q {2, 2, {0.000005f, 0.0f, 0.0f, 0.0004f}};  // Covariance of the process noise, Q
    const float R {0.7f};  // Covariance of the measurement noise, R
    const Matrix P0 {2, 2, {0.01f, 0.0f, 0.0f, 0.0004f}};  // Initial state error covariance, P0
    const float SOC0 {0.6f};  // Initial SOC for estimator 
    const float Ts {1.0f};  // Sample time

    EKF_SOC() {
        x(0, 0) = SOC0;
        x(1, 0) = 0.0f;  // set at first step
        p_hat(0, 0) = P0(0, 0);
        p_hat(1, 0) = P0(1, 0);
        p_hat(0, 1) = P0(0, 1);
        p_hat(1, 1) = P0(1, 1);

        // Generate jacobian dV/dSOC
        float deltaSOC;
        float deltaV;
        for (int i = 0; i < dV_dSOC.get_rows(); ++i) {
            if (i == 0) {
                deltaSOC = 0.0f;
            } else {
                deltaSOC = SOC_vec(0, i) - SOC_vec(0, i-1);
            }
            for (int j = 0; j < T_vec.get_cols(); ++j) {
                deltaV = V0_mat(i, j) - V0_mat(i-1, j);
                if (i == 0)
                    dV_dSOC(i, j) = 0.0f;
                else
                    dV_dSOC(i, j) = deltaV / deltaSOC;
            }
        }
    }

    float get_SOC_estimate() {
        return x(0, 0);
    }

    float get_x_hat1() {
        return x_hat_k1k(0, 0);
    }

    float get_x_hat2() {
        return x_hat_k1k(1, 0);
    }

    void step(float current_meas, float voltage_meas, float temp_meas) {
        input_step(current_meas, voltage_meas);
        prediction_step();
        correction_step();
        ++k;
    }

private:
    // Filter variables
    Matrix x {2, 1};  // State (SOC, V1)
    Matrix x_hat_k1k {2, 1};

    Matrix p_hat {2, 2};
    Matrix p_hat_k1k {2, 2};

    Matrix kalman_gain {2, 1};

    int k = 0;

    float current = 0.0f;  // measured A
    float voltage = 0.0f;  // measured V
    float temperature = 0.0f;  // measured K (unused)

    float get_piecewise(Matrix func, float SOC) const {
        int high = SOC_vec.get_cols() - 1;
        for (int i = 1; i < SOC_vec.get_cols(); ++i) {
            if (SOC < SOC_vec(0, i)) {
                high = i;
                break;
            }
        }
        int low = high - 1;
        float p = (SOC - SOC_vec(0, low)) / (SOC_vec(0, high) - SOC_vec(0, low));

        return func(low, 0) + p * (func(high, 0) - func(low, 0));
    }

    Matrix f() const;

    float h() const;

    Matrix F() const;

    Matrix H() const;

    void input_step(float current_meas, float voltage_meas);

    void prediction_step();

    void correction_step();

};

#endif // EKF_SOC_H