#include "ekf_soc.hpp"

Matrix EKF_SOC::f() const {
    // Ignoring T for now since we aren't measuring temperature
    float R1 = get_piecewise(R1_mat, x(0, 0));
    float tau1 = get_piecewise(tau1_mat, x(0, 0));
    float C1 = tau1 / R1;

    float a = std::expf(-Ts/tau1);
    float b = R1 * (1.0f - a);

    return {2, 1, {x(0,0)-current * Ts / (3600.0f * AH), a * x(1, 0) + b * current}};
}

float EKF_SOC::h() const {
    float V0 = get_piecewise(V0_mat, x(0, 0));
    float R0 = get_piecewise(R0_mat, x(0, 0));
    return V0 - current * R0 - x(1, 0);
}

Matrix EKF_SOC::F() const {
    float R1 = get_piecewise(R1_mat, x(0, 0));
    float tau1 = get_piecewise(tau1_mat, x(0, 0));
    float C1 = tau1 / R1;

    float tmp = std::expf(-Ts/tau1);

    return {2, 2, {1.0f, 0.0f, 0.0f, tmp}};
}

Matrix EKF_SOC::H() const {
    float jacobian = get_piecewise(dV_dSOC, x(0, 0));
    return {1, 2, {jacobian, -1}};
}

void EKF_SOC::input_step(float current_meas, float voltage_meas) {
    this->current = -current_meas;
    this->voltage = voltage_meas;
    if (k == 0)
        x(1, 0) = get_piecewise(V0_mat, SOC0) - voltage_meas - current*get_piecewise(R0_mat, SOC0);
}

void EKF_SOC::prediction_step() {
    this->x_hat_k1k = f();
    Matrix jacobianF {F()};
    this->p_hat_k1k = jacobianF * p_hat * jacobianF.transpose() + Q;
}

void EKF_SOC::correction_step() {
    float tmp {float(H() * p_hat_k1k * H().transpose()) + R};
    this->kalman_gain = p_hat_k1k * H().transpose() * (1.0f / tmp);

    // Update
    this->x = x_hat_k1k + kalman_gain * (voltage - h());
    Matrix I {2, 2, {1, 0, 0, 1}};
    this->p_hat = ((-1.0f) * (kalman_gain * H()) + I) * p_hat_k1k;
}