/// @file KalmanFilter.hpp
/// @brief Generalized Kalman Filter class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-11-25

#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

namespace NAV
{
/// @brief Generalized Kalman Filter class
class KalmanFilter
{
  public:
    /// @brief Constructor
    /// @param[in] n Number of States
    /// @param[in] m Number of Measurements
    KalmanFilter(int n, int m)
    {
        // x̂ State vector
        x = Eigen::MatrixXd::Zero(n, 1);

        // 𝐏 Error covariance matrix
        P = Eigen::MatrixXd::Zero(n, n);

        // 𝚽 State transition matrix
        Phi = Eigen::MatrixXd::Zero(n, n);

        // 𝐐 System/Process noise covariance matrix
        Q = Eigen::MatrixXd::Zero(n, n);

        // 𝐳 Measurement vector
        z = Eigen::MatrixXd::Zero(m, 1);

        // 𝐇 Measurement sensitivity Matrix
        H = Eigen::MatrixXd::Zero(m, n);

        // 𝐑 = 𝐸{𝐰ₘ𝐰ₘᵀ} Measurement noise covariance matrix
        R = Eigen::MatrixXd::Zero(m, m);

        // 𝐊 Kalman gain matrix
        K = Eigen::MatrixXd::Zero(n, m);

        // 𝑰 Identity Matrix
        I = Eigen::MatrixXd::Identity(n, n);
    }

    /// @brief Default constructor
    KalmanFilter() = delete;

    /// @brief Sets all Vectors and matrices to 0
    void setZero()
    {
        // x̂ State vector
        x.setZero();

        // 𝐏 Error covariance matrix
        P.setZero();

        // 𝚽 State transition matrix
        Phi.setZero();

        // 𝐐 System/Process noise covariance matrix
        Q.setZero();

        // 𝐳 Measurement vector
        z.setZero();

        // 𝐇 Measurement sensitivity Matrix
        H.setZero();

        // 𝐑 = 𝐸{𝐰ₘ𝐰ₘᵀ} Measurement noise covariance matrix
        R.setZero();

        // 𝐊 Kalman gain matrix
        K.setZero();
    }

    /// @brief Do a Time Update
    /// @attention Update the State transition matrix (𝚽) and the Process noise covariance matrix (𝐐) before calling this
    /// @note See P. Groves (2013) - Principles of GNSS, Inertial, and Multisensor Integrated Navigation Systems (ch. 3.2.2)
    void predict()
    {
        // Math: \mathbf{\hat{x}}_k^- = \mathbf{\Phi}_{k-1}\mathbf{\hat{x}}_{k-1}^+ \qquad \text{P. Groves}\,(3.14)
        x = Phi * x;

        // Math: \mathbf{P}_k^- = \mathbf{\Phi}_{k-1} P_{k-1}^+ \mathbf{\Phi}_{k-1}^T + \mathbf{Q}_{k-1} \qquad \text{P. Groves}\,(3.15)
        P = Phi * P * Phi.transpose() + Q;
    }

    /// @brief Do a Measurement Update with a Measurement 𝐳
    /// @attention Update the Measurement sensitivity Matrix (𝐇), the Measurement noise covariance matrix (𝐑)
    ///            and the Measurement vector (𝐳) before calling this
    /// @note See P. Groves (2013) - Principles of GNSS, Inertial, and Multisensor Integrated Navigation Systems (ch. 3.2.2)
    void correct()
    {
        // Math: \mathbf{K}_k = \mathbf{P}_k^- \mathbf{H}_k^T (\mathbf{H}_k \mathbf{P}_k^- \mathbf{H}_k^T + R_k)^{-1} \qquad \text{P. Groves}\,(3.21)
        K = P * H.transpose() * (H * P * H.transpose() + R).inverse();

        // Math: \begin{align*} \mathbf{\hat{x}}_k^+ &= \mathbf{\hat{x}}_k^- + \mathbf{K}_k (\mathbf{z}_k - \mathbf{H}_k \mathbf{\hat{x}}_k^-) \\ &= \mathbf{\hat{x}}_k^- + \mathbf{K}_k \mathbf{\delta z}_k^{-} \end{align*} \qquad \text{P. Groves}\,(3.24)
        x = x + K * (z - H * x);

        // Math: \mathbf{P}_k^+ = (\mathbf{I} - \mathbf{K}_k \mathbf{H}_k) \mathbf{P}_k^- \qquad \text{P. Groves}\,(3.25)
        P = (I - K * H) * P;
    }

    /// @brief Do a Measurement Update with a Measurement Innovation 𝜹𝐳
    /// @attention Update the Measurement sensitivity Matrix (𝐇), the Measurement noise covariance matrix (𝐑)
    ///            and the Measurement vector (𝐳) before calling this
    /// @note See P. Groves (2013) - Principles of GNSS, Inertial, and Multisensor Integrated Navigation Systems (ch. 3.2.2)
    /// @note See Brown & Hwang (2012) - Introduction to Random Signals and Applied Kalman Filtering (ch. 5.5 - figure 5.5)
    void correctWithMeasurementInnovation()
    {
        // Math: \mathbf{K}_k = \mathbf{P}_k^- \mathbf{H}_k^T (\mathbf{H}_k \mathbf{P}_k^- \mathbf{H}_k^T + R_k)^{-1} \qquad \text{P. Groves}\,(3.21)
        K = P * H.transpose() * (H * P * H.transpose() + R).inverse();

        // Math: \begin{align*} \mathbf{\hat{x}}_k^+ &= \mathbf{\hat{x}}_k^- + \mathbf{K}_k (\mathbf{z}_k - \mathbf{H}_k \mathbf{\hat{x}}_k^-) \\ &= \mathbf{\hat{x}}_k^- + \mathbf{K}_k \mathbf{\delta z}_k^{-} \end{align*} \qquad \text{P. Groves}\,(3.24)
        x = x + K * z;

        // Math: \mathbf{P}_k^+ = (\mathbf{I} - \mathbf{K}_k \mathbf{H}_k) \mathbf{P}_k^- \qquad \text{P. Groves}\,(3.25)
        // P = (I - K * H) * P;

        // Math: \mathbf{P}_k^+ = (\mathbf{I} - \mathbf{K}_k \mathbf{H}_k) \mathbf{P}_k^- (\mathbf{I} - \mathbf{K}_k \mathbf{H}_k)^T + \mathbf{K}_k \mathbf{R}_k \mathbf{K}_k^T \qquad \text{Brown & Hwang}\,(p. 145, eq. 4.2.11)
        P = (I - K * H) * P * (I - K * H).transpose() + K * R * K.transpose();
    }

    /// @brief Updates the state transition matrix 𝚽 limited to first order in 𝐅𝜏ₛ
    /// @param[in] F System Matrix
    /// @param[in] tau_s time interval in [s]
    /// @note See Groves (2013) chapter 14.2.4, equation (14.72)
    static Eigen::MatrixXd transitionMatrix(const Eigen::MatrixXd& F, double tau_s)
    {
        // Transition matrix 𝚽
        return Eigen::MatrixXd::Identity(F.rows(), F.cols()) + F * tau_s;
    }

    /// x̂ State vector
    Eigen::MatrixXd x;

    /// 𝐏 Error covariance matrix
    Eigen::MatrixXd P;

    /// 𝚽 State transition matrix
    Eigen::MatrixXd Phi;

    /// 𝐐 System/Process noise covariance matrix
    Eigen::MatrixXd Q;

    /// 𝐳 Measurement vector
    Eigen::MatrixXd z;

    /// 𝐇 Measurement sensitivity Matrix
    Eigen::MatrixXd H;

    /// 𝐑 = 𝐸{𝐰ₘ𝐰ₘᵀ} Measurement noise covariance matrix
    Eigen::MatrixXd R;

    /// 𝐊 Kalman gain matrix
    Eigen::MatrixXd K;

  private:
    /// 𝑰 Identity Matrix (n x n)
    Eigen::MatrixXd I;
};

/// @brief Updates the state transition matrix 𝚽 limited to first order in 𝐅𝜏ₛ
/// @param[in] F System Matrix
/// @param[in] tau_s time interval in [s]
/// @note See Groves (2013) chapter 14.2.4, equation (14.72)
template<typename _Scalar, int _n>
Eigen::Matrix<_Scalar, _n, _n> transitionMatrixApproxOrder1(const Eigen::Matrix<_Scalar, _n, _n>& F, double tau_s)
{
    // Transition matrix 𝚽
    return Eigen::Matrix<_Scalar, _n, _n>::Identity() + F * tau_s;
}

/// @brief Updates the state transition matrix 𝚽 limited to second order in 𝐅𝜏ₛ
/// @param[in] F System Matrix
/// @param[in] tau_s time interval in [s]
/// @note See Groves (2013) chapter 14.2.4, equation (14.72)
template<typename _Scalar, int _n>
Eigen::Matrix<_Scalar, _n, _n> transitionMatrixApproxOrder2(const Eigen::Matrix<_Scalar, _n, _n>& F, double tau_s)
{
    // Transition matrix 𝚽
    return Eigen::Matrix<_Scalar, _n, _n>::Identity() + F * tau_s + 1. / 2. * F * F * tau_s * tau_s;
}

} // namespace NAV
