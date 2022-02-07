/// @file PosVel.hpp
/// @brief Position, Velocity and Attitude Storage Class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-10-27

#pragma once

#include "NodeData/State/Pos.hpp"

namespace NAV
{
/// Position, Velocity and Attitude Storage Class
class PosVel : public Pos
{
  public:
    /// @brief Returns the type of the data class
    /// @return The data type
    [[nodiscard]] static std::string type()
    {
        return "PosVel";
    }

    /// @brief Returns the parent types of the data class
    /// @return The parent data types
    [[nodiscard]] static std::vector<std::string> parentTypes()
    {
        auto parent = Pos::parentTypes();
        parent.push_back(Pos::type());
        return parent;
    }

    /* -------------------------------------------------------------------------------------------------------- */
    /*                                                 Velocity                                                 */
    /* -------------------------------------------------------------------------------------------------------- */

    /// Returns the velocity in [m/s], in earth coordinates
    [[nodiscard]] const Eigen::Vector3d& velocity_e() const { return _v_e; }

    /// Returns the velocity in [m/s], in navigation coordinates
    [[nodiscard]] const Eigen::Vector3d& velocity_n() const { return _v_n; }

    // ###########################################################################################################
    //                                                  Setter
    // ###########################################################################################################

    /// @brief Set the Velocity in the earth frame
    /// @param[in] vel_e The new velocity in the earth frame
    void setVelocity_e(const Eigen::Vector3d& vel_e)
    {
        _v_e = vel_e;
        _v_n = quaternion_ne() * vel_e;
    }

    /// @brief Set the Velocity in the NED frame
    /// @param[in] vel_n The new velocity in the NED frame
    void setVelocity_n(const Eigen::Vector3d& vel_n)
    {
        _v_e = e_Quat_n() * vel_n;
        _v_n = vel_n;
    }

    /* -------------------------------------------------------------------------------------------------------- */
    /*                                             Member variables                                             */
    /* -------------------------------------------------------------------------------------------------------- */

  private:
    /// Velocity in earth coordinates [m/s]
    Eigen::Vector3d _v_e{ std::nan(""), std::nan(""), std::nan("") };
    /// Velocity in navigation coordinates [m/s]
    Eigen::Vector3d _v_n{ std::nan(""), std::nan(""), std::nan("") };
};

} // namespace NAV
