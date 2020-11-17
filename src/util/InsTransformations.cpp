#include "InsTransformations.hpp"

#include "util/Logger.hpp"

namespace NAV
{
Eigen::Vector3d trafo::deg2rad3(const Eigen::Vector3d& deg)
{
    return deg * M_PI / 180.0;
}

Eigen::Vector3d trafo::rad2deg3(const Eigen::Vector3d& rad)
{
    return rad * 180.0 / M_PI;
}

Eigen::Vector3d trafo::quat2eulerZYX(const Eigen::Quaterniond& q)
{
    Eigen::Matrix3d dcm = q.toRotationMatrix();

    Eigen::Vector3d EulerAngles = Eigen::Vector3d::Zero();

    EulerAngles(1) = asin(dcm(2, 0));
    if (rad2deg(std::abs(EulerAngles(1))) < 89.0)
    {
        EulerAngles(0) = -std::atan2((dcm(2, 1) / std::cos(EulerAngles(1))), (dcm(2, 2) / std::cos(EulerAngles(1))));

        EulerAngles(2) = -std::atan2((dcm(1, 0) / std::cos(EulerAngles(1))), (dcm(0, 0) / std::cos(EulerAngles(1))));
    }
    else
    {
        EulerAngles(0) = 0.0;
        EulerAngles(2) = 0.0;
    }

    return EulerAngles;

    // return q.toRotationMatrix().eulerAngles(2, 1, 0);
}

Quaterniond<Earth, Inertial> trafo::quat_ei(const double time, const double angularRate_ie)
{
    // Initialize angle-axis rotation from an angle in radian and an axis which must be normalized.
    Eigen::AngleAxisd zAngle(-angularRate_ie * time, Eigen::Vector3d::UnitZ());

    return Quaterniond<Earth, Inertial>(zAngle);
}

Quaterniond<Inertial, Earth> trafo::quat_ie(const double time, const double angularRate_ie)
{
    return quat_ei(time, angularRate_ie).conjugate();
}

Quaterniond<Earth, Navigation> trafo::quat_en(const double latitude, const double longitude)
{
    // Initialize angle-axis rotation from an angle in radian and an axis which must be normalized.
    // Eigen uses here a different sign convention as the physical system.
    Eigen::AngleAxisd longitudeAngle(longitude, Eigen::Vector3d::UnitZ());
    Eigen::AngleAxisd latitudeAngle(-M_PI_2 - latitude, Eigen::Vector3d::UnitY());

    return Quaterniond<Earth, Navigation>(longitudeAngle * latitudeAngle);
}

Quaterniond<Navigation, Earth> trafo::quat_ne(const double latitude, const double longitude)
{
    return quat_en(latitude, longitude).conjugate();
}

Quaterniond<Navigation, Body> trafo::quat_nb(const double roll, const double pitch, const double yaw)
{
    // Initialize angle-axis rotation from an angle in radian and an axis which must be normalized.
    // Eigen uses here a different sign convention as the physical system.
    Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());

    return Quaterniond<Navigation, Body>(yawAngle * pitchAngle * rollAngle);
}

Quaterniond<Body, Navigation> trafo::quat_bn(const double roll, const double pitch, const double yaw)
{
    return quat_nb(roll, pitch, yaw).conjugate();
}

Quaterniond<Body, Platform> trafo::quat_bp(double mountingAngleX, double mountingAngleY, double mountingAngleZ)
{
    // Initialize angle-axis rotation from an angle in radian and an axis which must be normalized.
    Eigen::AngleAxisd xAngle(mountingAngleX, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd yAngle(mountingAngleY, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd zAngle(mountingAngleZ, Eigen::Vector3d::UnitZ());

    return Quaterniond<Body, Platform>(zAngle * yAngle * xAngle);
}

Quaterniond<Platform, Body> trafo::quat_pb(double mountingAngleX, double mountingAngleY, double mountingAngleZ)
{
    return quat_bp(mountingAngleX, mountingAngleY, mountingAngleZ).conjugate();
}

Vector3d<Navigation> trafo::ecef2ned(const Vector3d<Earth>& position_e, Vector3d<LLA> latLonAlt_ref)
{
    const auto& latitude_ref = latLonAlt_ref(0);  // 𝜙 Geodetic latitude
    const auto& longitude_ref = latLonAlt_ref(1); // λ Geodetic longitude

    auto position_e_ref = lla2ecef_WGS84(latLonAlt_ref);

    Matrix3d<Navigation, Earth> R_ne;
    R_ne << -std::sin(latitude_ref) * std::cos(longitude_ref), -std::sin(latitude_ref) * std::sin(longitude_ref), std::cos(latitude_ref),
        -std::sin(longitude_ref), std::cos(longitude_ref), 0,
        -std::cos(latitude_ref) * std::cos(longitude_ref), -std::cos(latitude_ref) * std::sin(longitude_ref), -std::sin(latitude_ref);

    Vector3d<Navigation> position_n = R_ne * (position_e - position_e_ref);

    return position_n;
}

Vector3d<Earth> trafo::ned2ecef(const Vector3d<Navigation>& position_n, Vector3d<LLA> latLonAlt_ref)
{
    const auto& latitude_ref = latLonAlt_ref(0);  // 𝜙 Geodetic latitude
    const auto& longitude_ref = latLonAlt_ref(1); // λ Geodetic longitude

    auto position_e_ref = lla2ecef_WGS84(latLonAlt_ref);

    Matrix3d<Earth, Navigation> R_en;
    R_en << -std::sin(latitude_ref) * std::cos(longitude_ref), -std::sin(longitude_ref), -std::cos(latitude_ref) * std::cos(longitude_ref),
        -std::sin(latitude_ref) * std::sin(longitude_ref), std::cos(longitude_ref), -std::cos(latitude_ref) * std::sin(longitude_ref),
        std::cos(latitude_ref), 0, -std::sin(latitude_ref);

    Vector3d<Earth> position_e = position_e_ref + R_en * position_n;

    return position_e;
}

Vector3d<Earth> trafo::lla2ecef(const Vector3d<LLA>& latLonAlt, double a, double e_squared)
{
    const auto& latitude = latLonAlt(0);  // 𝜙 Geodetic latitude
    const auto& longitude = latLonAlt(1); // λ Geodetic longitude
    const auto& altitude = latLonAlt(2);  // Altitude (Height above ground)

    /// Radius of curvature of the ellipsoid in the prime vertical plane,
    /// i.e., the plane containing the normal at P and perpendicular to the meridian (eq. 1.81)
    double N = a / std::sqrt(1 - e_squared * std::pow(std::sin(latitude), 2));

    // Jekeli, 2001 (eq. 1.80) (see  Torge, 1991, for further details)
    return Vector3d<Earth>((N + altitude) * std::cos(latitude) * std::cos(longitude),
                           (N + altitude) * std::cos(latitude) * std::sin(longitude),
                           (N * (1 - e_squared) + altitude) * std::sin(latitude));
}

Vector3d<Earth> trafo::lla2ecef_WGS84(const Vector3d<LLA>& latLonAlt)
{
    return lla2ecef(latLonAlt, InsConst::WGS84_a, InsConst::WGS84_e_squared);
}

Vector3d<Earth> trafo::lla2ecef_GRS80(const Vector3d<LLA>& latLonAlt)
{
    return lla2ecef(latLonAlt, InsConst::GRS80_a, InsConst::GRS80_e_squared);
}

Vector3d<LLA> trafo::ecef2lla(const Vector3d<Earth>& ecef, double a, double b, double e_squared)
{
    auto x = ecef(0);
    auto y = ecef(1);
    auto z = ecef(2);

    // Calculate longitude

    auto lon = std::atan2(y, x);

    // Start computing intermediate variables needed to compute altitude

    auto p = ecef.head(2).norm();
    auto E = std::sqrt(a * a - b * b);
    auto F = 54 * std::pow(b * z, 2);
    auto G = p * p + (1 - e_squared) * z * z - e_squared * E * E;
    auto c = e_squared * e_squared * F * p * p / std::pow(G, 3);
    auto s = std::pow(1 + c + std::sqrt(c * c + 2 * c), 1.0 / 3.0);
    auto P = (F / (3 * G * G)) / std::pow(s + (1.0 / s) + 1, 2);
    auto Q = std::sqrt(1 + 2 * e_squared * e_squared * P);
    auto k_1 = -P * e_squared * p / (1 + Q);
    auto k_2 = 0.5 * a * a * (1 + 1 / Q);
    auto k_3 = -P * (1 - e_squared) * z * z / (Q * (1 + Q));
    auto k_4 = -0.5 * P * p * p;
    auto r_0 = k_1 + std::sqrt(k_2 + k_3 + k_4);
    auto k_5 = (p - e_squared * r_0);
    auto U = std::sqrt(k_5 * k_5 + z * z);
    auto V = std::sqrt(k_5 * k_5 + (1 - e_squared) * z * z);

    auto alt = U * (1 - (b * b / (a * V)));

    // Compute additional values required for computing latitude

    auto z_0 = (b * b * z) / (a * V);
    auto e_p = (a / b) * std::sqrt(e_squared);

    auto lat = std::atan((z + z_0 * (e_p * e_p)) / p);

    return Vector3d<LLA>(lat, lon, alt);
}

Vector3d<LLA> trafo::ecef2lla_WGS84(const Vector3d<Earth>& ecef)
{
    return ecef2lla(ecef, InsConst::WGS84_a, InsConst::WGS84_b, InsConst::WGS84_e_squared);
}

Vector3d<LLA> trafo::ecef2lla_GRS80(const Vector3d<Earth>& ecef)
{
    return ecef2lla(ecef, InsConst::GRS80_a, InsConst::GRS80_b, InsConst::GRS80_e_squared);
}

} // namespace NAV