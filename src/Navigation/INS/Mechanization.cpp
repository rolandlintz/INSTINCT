#include "Mechanization.hpp"

#include "Navigation/Constants.hpp"
#include "Navigation/Ellipsoid/Ellipsoid.hpp"
#include "Navigation/INS/Functions.hpp"
#include "Navigation/Math/Math.hpp"
#include "Navigation/Math/NumericalIntegration.hpp"
#include "Navigation/Transformations/CoordinateFrames.hpp"
#include "util/Logger.hpp"

#include <cmath>

namespace NAV
{

Eigen::Vector4d calcTimeDerivativeForQuaternion_nb(const Eigen::Vector3d& omega_nb_b, const Eigen::Vector4d& q_nb_coeffs)
{
    // Angular rates in matrix form (Titterton (2005), eq. (11.35))
    Eigen::Matrix4d A;

    // clang-format off
    A <<       0.0     , -omega_nb_b(0), -omega_nb_b(1), -omega_nb_b(2),
          omega_nb_b(0),       0.0     ,  omega_nb_b(2), -omega_nb_b(1),
          omega_nb_b(1), -omega_nb_b(2),       0.0     ,  omega_nb_b(0),
          omega_nb_b(2),  omega_nb_b(1), -omega_nb_b(0),       0.0     ;
    // clang-format on

    // Propagation of an attitude Quaternion with time (Titterton, ch. 11.2.5, eq. 11.33-11.35, p. 319)
    return 0.5 * A * q_nb_coeffs; // (w, x, y, z)
}

Eigen::Vector3d calcTimeDerivativeForVelocity_n(const Eigen::Vector3d& f_n,
                                                const Eigen::Vector3d& omega_ie_e,
                                                const Eigen::Vector3d& omega_ie_n,
                                                const Eigen::Vector3d& omega_en_n,
                                                const Eigen::Vector3d& velocity_n,
                                                const Eigen::Vector3d& gravitation_n,
                                                const Eigen::Quaterniond& q_ne,
                                                const Eigen::Vector3d& x_e,
                                                bool coriolisAccelerationCompensationEnabled,
                                                bool centrifgalAccelerationCompensationEnabled)
{
    Eigen::Vector3d v_dot = f_n;
    if (coriolisAccelerationCompensationEnabled)
    {
        v_dot -= calcCoriolisAcceleration_n(omega_ie_n, omega_en_n, velocity_n);
    }
    v_dot += gravitation_n;
    if (centrifgalAccelerationCompensationEnabled)
    {
        v_dot -= q_ne * calcCentrifugalAcceleration_e(x_e, omega_ie_e);
    }
    return v_dot;
}

Eigen::Vector3d calcTimeDerivativeForPosition_lla(const Eigen::Vector3d& velocity_n,
                                                  const double& phi,
                                                  const double& h,
                                                  const double& R_N,
                                                  const double& R_E)
{
    // Velocity North in [m/s]
    const auto& v_N = velocity_n(0);
    // Velocity East in [m/s]
    const auto& v_E = velocity_n(1);
    // Velocity Down in [m/s]
    const auto& v_D = velocity_n(2);

    return { v_N / (R_N + h),
             v_E / ((R_E + h) * std::cos(phi)),
             -v_D };
}

Eigen::Matrix<double, 10, 1> calcPosVelAttDerivative_n(const Eigen::Matrix<double, 10, 1>& y, const PosVelAttDerivativeConstants_n& c)
{
    //       0  1  2  3   4    5    6   7  8  9
    // ∂/∂t [w, x, y, z, v_N, v_E, v_D, 𝜙, λ, h]^T
    Eigen::Matrix<double, 10, 1> y_dot = Eigen::Matrix<double, 10, 1>::Zero();

    const auto R_N = calcEarthRadius_N(y(7));
    const auto R_E = calcEarthRadius_E(y(7));

    const Eigen::Quaterniond q_nb{ y(0), y(1), y(2), y(3) };
    const Eigen::Quaterniond q_ne = trafo::quat_ne(y(7), y(8));
    // ω_ie_e Turn rate of the Earth expressed in Earth frame coordinates
    const Eigen::Vector3d& omega_ie_e = c.angularRateEarthRotationCompensationEnabled ? InsConst::angularVelocity_ie_e
                                                                                      : Eigen::Vector3d::Zero();
    // ω_ie_n Turn rate of the Earth expressed in local-navigation frame coordinates
    const Eigen::Vector3d omega_ie_n = q_ne * omega_ie_e;
    // ω_en_n Turn rate of the local frame with respect to the Earth-fixed frame, called the transport rate, expressed in local-navigation frame coordinates
    const Eigen::Vector3d omega_en_n = c.angularRateTransportRateCompensationEnabled ? calcTransportRate_n(y.segment<3>(7), y.segment<3>(4), R_N, R_E)
                                                                                     : Eigen::Vector3d::Zero();

    // ω_nb_b = ω_ib_b - C_bn * (ω_ie_n + ω_en_n)
    const Eigen::Vector3d omega_nb_b = c.omega_ib_b - q_nb.conjugate() * (omega_ie_n + omega_en_n);

    y_dot.segment<4>(0) = calcTimeDerivativeForQuaternion_nb(omega_nb_b,       // ω_nb_b Body rate with respect to the navigation frame, expressed in the body frame
                                                             y.segment<4>(0)); // q_nb_coeffs Coefficients of the quaternion q_nb in order w, x, y, z (q = w + ix + jy + kz)

    y_dot.segment<3>(4) = calcTimeDerivativeForVelocity_n(q_nb * c.f_b,                                       // f_n Specific force vector as measured by a triad of accelerometers and resolved into local-navigation frame coordinates
                                                          omega_ie_e,                                         // ω_ie_e Turn rate of the Earth expressed in Earth frame coordinates
                                                          omega_ie_n,                                         // ω_ie_n Turn rate of the Earth expressed in local-navigation frame coordinates
                                                          omega_en_n,                                         // ω_en_n Turn rate of the local frame with respect to the Earth-fixed frame, called the transport rate, expressed in local-navigation frame coordinates
                                                          y.segment<3>(4),                                    // v_n Velocity with respect to the Earth in local-navigation frame coordinates [m/s]
                                                          calcGravitation_n(y.segment<3>(7), c.gravityModel), // gravitation_n Local gravitation vector (caused by effects of mass attraction) in local-navigation frame coordinates [m/s^2]
                                                          q_ne,                                               // q_ne Rotation quaternion which converts vectors in Earth frame to local-navigation frame coordinates (r_n = q_ne * r_e)
                                                          trafo::lla2ecef_WGS84(y.segment<3>(7)));            // x_e Position in ECEF coordinates

    y_dot.segment<3>(7) = calcTimeDerivativeForPosition_lla(y.segment<3>(4), // v_n Velocity with respect to the Earth in local-navigation frame coordinates [m/s]
                                                            y(7),            // 𝜙 Latitude in [rad]
                                                            y(9),            // h Altitude in [m]
                                                            R_N,             // North/South (meridian) earth radius [m]
                                                            R_E);            // East/West (prime vertical) earth radius [m]
    return y_dot;
};

// TODO: M.M.: Remove this after migrating it to the new functions
// ###########################################################################################################
//                                             Private Functions
// ###########################################################################################################

/// @brief Stores information of the state needed for the velocity update
struct VelocityUpdateState
{
    /// a_n Taylor-Approximation of acceleration in [m/s^2]
    Eigen::Vector3d accel_n;
    /// ω_ie_n Nominal mean angular velocity of the Earth in [rad/s], in navigation coordinates
    Eigen::Vector3d angularVelocity_ie_n;
    /// ω_ie_n Transport Rate in [rad/s], in navigation coordinates
    Eigen::Vector3d angularVelocity_en_n;
    /// g_n (tₖ₋₁) Gravity vector in [m/s^2], in navigation coordinates (including centrifugal acceleration)
    Eigen::Vector3d gravity_n;
};

/// @brief Equations to perform an update of the velocity, including rotational correction
/// @param[in] x State information needed for the update
/// @param[in] velocity_n Old velocity in navigation coordinates
/// @param[in] angularVelocity_ip_b__t0 Angular velocity of platform system with respect to inertial system, represented in body coordinates in [rad/s]
/// @param[in] angularVelocity_ie_n__t1 Angular velocity of earth with respect to inertial system, represented in n-sys
/// @param[in] angularVelocity_en_n__t1 Transport rate represented in n-sys
/// @param[in] quaternion_nb__t1 Orientation of body with respect to n-sys
/// @param[in] timeDifferenceSec__t0 Time difference Δtₖ = (tₖ - tₖ₋₁) in [seconds]
/// @return Derivative of the velocity
/// @note See Zwiener (2019) - Robuste Zustandsschätzung zur Navigation und Regelung autonomer und bemannter Multikopter mit verteilten Sensoren, eqns. (3.39) and (3.44)
Eigen::Vector3d velocityUpdateModel_Rotation(const VelocityUpdateState& x, const Eigen::Vector3d& velocity_n, const Eigen::Vector3d& angularVelocity_ip_b__t0, const Eigen::Vector3d& angularVelocity_ie_n__t1, const Eigen::Vector3d& angularVelocity_en_n__t1, const Eigen::Quaterniond& quaternion_nb__t1, const long double& timeDifferenceSec__t0)
{
    // q (tₖ₋₁) Quaternion, from n-system to b-system, at the time tₖ₋₁
    const Eigen::Quaterniond quaternion_bn__t1 = quaternion_nb__t1.conjugate();

    // Δβ⁠_nb_p (tₖ) The angular velocities in [rad],
    // of the navigation to body system, in body coordinates, at the time tₖ (eq. 8.9)
    const Eigen::Vector3d angularVelocity_nb_b__t0 = angularVelocity_ip_b__t0
                                                     - quaternion_bn__t1 * (angularVelocity_ie_n__t1 + angularVelocity_en_n__t1);

    // TODO: Consider suppressCoriolis option from the GUI here
    // The Coriolis acceleration accounts for the fact that the NED frame is noninertial
    const Eigen::Vector3d coriolisAcceleration_n = (2 * x.angularVelocity_ie_n + x.angularVelocity_en_n).cross(velocity_n);

    Eigen::Matrix3d rotA = skewSymmetricMatrix(angularVelocity_nb_b__t0) * 0.5 * std::pow(timeDifferenceSec__t0, 2);
    Eigen::Matrix3d rotB = (std::pow(timeDifferenceSec__t0, 3) / 6.0 - std::pow(std::sqrt(std::pow(angularVelocity_nb_b__t0(0), 2) + std::pow(angularVelocity_nb_b__t0(1), 2) + angularVelocity_nb_b__t0(2)), 2) / 120.0 * std::pow(timeDifferenceSec__t0, 5)) * skewSymmetricMatrix2(angularVelocity_nb_b__t0);

    Eigen::Matrix3d rotCorr = Eigen::Matrix3d::Identity(3, 3) + rotA + rotB;

    // Jekeli (eq. 4.88) - g includes centrifugal acceleration
    return rotCorr * x.accel_n - coriolisAcceleration_n + x.gravity_n;
}

} // namespace NAV