// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file KalmanFilter.cpp
/// @brief SPP Kalman Filter
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2023-12-22

#include "KalmanFilter.hpp"

#include <algorithm>
#include <functional>

#include "internal/gui/widgets/InputWithUnit.hpp"
#include "Navigation/Constants.hpp"
#include "Navigation/Transformations/CoordinateFrames.hpp"

namespace NAV::SPP
{
void KalmanFilter::reset(bool useDoppler)
{
    // Covariance of the acceleration 𝜎_a due to user motion in horizontal and vertical component [m²/s³]
    switch (_gui_covarianceAccelUnit)
    {
    case CovarianceAccelUnits::m_sqrts3:
        _covarianceAccel = { std::pow(_gui_covarianceAccel[0], 2), std::pow(_gui_covarianceAccel[1], 2) };
        break;
    case CovarianceAccelUnits::m2_s3:
        _covarianceAccel = _gui_covarianceAccel;
        break;
    }

    // Covariance of the clock phase drift [m²/s]
    switch (_gui_covarianceClkPhaseDriftUnit)
    {
    case CovarianceClkPhaseDriftUnits::m_sqrts:
        _covarianceClkPhaseDrift = std::pow(_gui_covarianceClkPhaseDrift, 2);
        break;
    case CovarianceClkPhaseDriftUnits::m2_s:
        _covarianceClkPhaseDrift = _gui_covarianceClkPhaseDrift;
        break;
    }

    // Covariance of the frequency phase drift [m²/s³]
    switch (_gui_covarianceClkFrequencyDriftUnit)
    {
    case CovarianceClkFrequencyDriftUnits::m_sqrts3:
        _covarianceClkFrequencyDrift = std::pow(_gui_covarianceClkFrequencyDrift, 2);
        break;
    case CovarianceClkFrequencyDriftUnits::m2_s3:
        _covarianceClkFrequencyDrift = _gui_covarianceClkFrequencyDrift;
        break;
    }

    // Covariance of the inter-system clock phase drift [m²/s]
    switch (_gui_covarianceInterSysClkPhaseDriftUnit)
    {
    case CovarianceClkPhaseDriftUnits::m_sqrts:
        _covarianceInterSysClkPhaseDrift = std::pow(_gui_covarianceInterSysClkPhaseDrift, 2);
        break;
    case CovarianceClkPhaseDriftUnits::m2_s:
        _covarianceInterSysClkPhaseDrift = _gui_covarianceInterSysClkPhaseDrift;
        break;
    }

    // Covariance of the inter-system frequency phase drift [m²/s³]
    switch (_gui_covarianceInterSysClkFrequencyDriftUnit)
    {
    case CovarianceClkFrequencyDriftUnits::m_sqrts3:
        _covarianceInterSysClkFrequencyDrift = std::pow(_gui_covarianceInterSysClkFrequencyDrift, 2);
        break;
    case CovarianceClkFrequencyDriftUnits::m2_s3:
        _covarianceInterSysClkFrequencyDrift = _gui_covarianceInterSysClkFrequencyDrift;
        break;
    }

    // ###########################################################################################################

    _kalmanFilter = KeyedKalmanFilterD<SPP::States::StateKeyTypes,
                                       SPP::Meas::MeasKeyTypes>{ useDoppler ? SPP::States::PosVelRecvClk : SPP::States::PosVelRecvClkErr, {} };

    _kalmanFilter.F.block<3>(States::Pos, States::Vel) = Eigen::Matrix3d::Identity();
    _kalmanFilter.W.block<3>(States::Vel, States::Vel) = Eigen::DiagonalMatrix<double, 3>(_covarianceAccel[0], _covarianceAccel[0], _covarianceAccel[1]);

    _kalmanFilter.Phi(all, all).diagonal().setOnes();
    _kalmanFilter.G(States::RecvClkErr, States::RecvClkErr) = 1;
    _kalmanFilter.W(States::RecvClkErr, States::RecvClkErr) = _covarianceClkPhaseDrift;
    _kalmanFilter.Q(States::RecvClkErr, States::RecvClkErr) = _covarianceClkPhaseDrift;
    if (useDoppler)
    {
        _kalmanFilter.F(States::RecvClkErr, States::RecvClkDrift) = 1;
        _kalmanFilter.G(States::RecvClkDrift, States::RecvClkDrift) = 1;
        _kalmanFilter.W(States::RecvClkDrift, States::RecvClkDrift) = _covarianceClkFrequencyDrift;
        _kalmanFilter.Q(States::RecvClkDrift, States::RecvClkDrift) = _covarianceClkFrequencyDrift;
    }

    _initialized = false;
}

void KalmanFilter::initialize(const KeyedVectorXd<States::StateKeyTypes>& states, const KeyedMatrixXd<States::StateKeyTypes, States::StateKeyTypes>& variance)
{
    _kalmanFilter.x(states.rowKeys()) = states(all);
    _kalmanFilter.P(variance.rowKeys(), variance.colKeys()) = variance(all, all);

    if (!states.hasAnyRows(States::Vel)) // We always estimate velocity in the KF, but LSQ could not, so set a default value
    {
        _kalmanFilter.P(States::Vel, States::Vel).diagonal() << Eigen::Vector3d::Ones() * 1e-1;
    }

    _initialized = true;
}

void KalmanFilter::deinitialize()
{
    _initialized = false;
}

void KalmanFilter::predict(const double& dt, const Eigen::Vector3d& lla_pos, [[maybe_unused]] const std::string& nameId)
{
    // Update the State transition matrix (𝚽) and the Process noise covariance matrix (𝐐)

    _kalmanFilter.G.block<3>(States::Vel, States::Vel) = trafo::e_Quat_n(lla_pos(0), lla_pos(1)).toRotationMatrix();

    LOG_DATA("{}: F =\n{}", nameId, _kalmanFilter.F);
    LOG_DATA("{}: G =\n{}", nameId, _kalmanFilter.G);
    LOG_DATA("{}: W =\n{}", nameId, _kalmanFilter.W);

    if (_qCalculationAlgorithm == QCalculationAlgorithm::VanLoan)
    {
        _kalmanFilter.calcPhiAndQWithVanLoanMethod(dt);
    }
    else
    {
        _kalmanFilter.calcTransitionMatrix_Phi_Taylor(dt, 1);
        _kalmanFilter.Q = calcProcessNoiseMatrixGroves(dt, lla_pos, nameId);
    }

    LOG_DATA("{}: Phi =\n{}", nameId, _kalmanFilter.Phi);
    LOG_DATA("{}: Q =\n{}", nameId, _kalmanFilter.Q);

    LOG_DATA("{}: P (a posteriori) =\n{}", nameId, _kalmanFilter.P);
    LOG_DATA("{}: x (a posteriori) =\n{}", nameId, _kalmanFilter.x.transposed());
    _kalmanFilter.predict();
    LOG_DATA("{}: x (a priori    ) =\n{}", nameId, _kalmanFilter.x.transposed());
    LOG_DATA("{}: P (a priori    ) =\n{}", nameId, _kalmanFilter.P);
}

void KalmanFilter::update(const std::vector<Meas::MeasKeyTypes>& measKeys,
                          const KeyedMatrixXd<Meas::MeasKeyTypes, States::StateKeyTypes>& H,
                          const KeyedMatrixXd<Meas::MeasKeyTypes, Meas::MeasKeyTypes>& R,
                          const KeyedVectorXd<Meas::MeasKeyTypes>& dz,
                          [[maybe_unused]] const std::string& nameId)
{
    LOG_DATA("{}: called", nameId);

    _kalmanFilter.setMeasurements(measKeys);

    _kalmanFilter.H = H;
    _kalmanFilter.R = R;
    _kalmanFilter.z = dz;

    _kalmanFilter.correctWithMeasurementInnovation();
}

SatelliteSystem KalmanFilter::updateInterSystemTimeDifferences(const std::set<SatelliteSystem>& usedSatSystems,
                                                               SatelliteSystem oldRefSys,
                                                               SatelliteSystem newRefSys,
                                                               bool useDoppler,
                                                               [[maybe_unused]] const std::string& nameId)
{
    if (oldRefSys != newRefSys)
    {
        LOG_DATA("{}: Switching inter system clock reference system [{}] -> [{}]", nameId, oldRefSys, newRefSys);
        auto nStates = static_cast<int>(getStateKeys().size());
        if (std::any_of(getStateKeys().begin(), getStateKeys().end(),
                        [](const SPP::States::StateKeyTypes& state) { return std::holds_alternative<States::InterSysBias>(state); }))
        {
            auto newKeyBias = SPP::States::InterSysBias{ newRefSys };
            auto newKeyDrift = SPP::States::InterSysDrift{ newRefSys };

            if (!usedSatSystems.contains(oldRefSys) && !_kalmanFilter.x.hasRow(newKeyBias)) // New system not estimated yet, but old not observed anymore
            {
                for (const auto& state : getStateKeys())
                {
                    if (const auto* bias = std::get_if<States::InterSysBias>(&state))
                    {
                        if (usedSatSystems.contains(bias->satSys))
                        {
                            LOG_DEBUG("{}: Switching to [{}] instead of [{}] because old system [{}] not observed, but [{}] not yet estimated",
                                      nameId, bias->satSys, newRefSys, oldRefSys, newRefSys);
                            newRefSys = bias->satSys;
                            newKeyBias = SPP::States::InterSysBias{ newRefSys };
                            newKeyDrift = SPP::States::InterSysDrift{ newRefSys };
                            break;
                        }
                    }
                }
            }

            if (_kalmanFilter.x.hasRow(newKeyBias))
            {
                KeyedMatrixXd<States::StateKeyTypes> D(Eigen::MatrixXd::Identity(nStates, nStates), getStateKeys());

                for (const auto& state : getStateKeys())
                {
                    if (const auto* bias = std::get_if<States::InterSysBias>(&state))
                    {
                        auto keyBias = SPP::States::InterSysBias{ bias->satSys };
                        auto keyDrift = SPP::States::InterSysDrift{ bias->satSys };
                        D(keyBias, newKeyBias) = -1;
                        if (useDoppler) { D(keyDrift, newKeyDrift) = -1; }
                    }
                }

                if (!usedSatSystems.contains(oldRefSys)) // Old Reference system not observed anymore
                {
                    D(newKeyBias, newKeyBias) = 0;
                    if (useDoppler) { D(newKeyDrift, newKeyDrift) = 0; }
                }

                LOG_DATA("{}: D = \n{}", nameId, D);
                LOG_DATA("{}: x_old = \n{}", nameId, _kalmanFilter.x.transposed());
                _kalmanFilter.x(all) = D(all, all) * _kalmanFilter.x(all);
                LOG_DATA("{}: D * x_old = \n{}", nameId, _kalmanFilter.x.transposed());
                _kalmanFilter.P(all, all) = D(all, all) * _kalmanFilter.P(all, all) * D(all, all).transpose();

                if (!usedSatSystems.contains(oldRefSys)) // Old Reference system not observed anymore
                {
                    LOG_DEBUG("{}: Removing inter system time difference states for system [{}]", nameId, newRefSys);
                    if (useDoppler) { _kalmanFilter.removeStates({ newKeyBias, newKeyDrift }); }
                    else { _kalmanFilter.removeState(newKeyBias); }
                }
                else
                {
                    LOG_DEBUG("{}: Changing inter system time difference states for system [{}] into [{}]", nameId, newRefSys, oldRefSys);
                    _kalmanFilter.replaceState(newKeyBias, SPP::States::InterSysBias{ oldRefSys });
                    if (useDoppler) { _kalmanFilter.replaceState(newKeyDrift, SPP::States::InterSysDrift{ oldRefSys }); }
                }
                LOG_DATA("{}: x_new = \n{}", nameId, _kalmanFilter.x.transposed());
            }
            else
            {
                LOG_DATA("{}: Not switching inter system bias reference to [{}], because new system not estimated yet.", nameId, newRefSys);
            }
        }
        else
        {
            LOG_DATA("{}: No inter system biases estimated yet, so accepting new reference system [{}] -> [{}]", nameId, oldRefSys, newRefSys);
        }
    }

    for (const auto& satSys : usedSatSystems)
    {
        auto keyBias = SPP::States::InterSysBias{ satSys };
        auto keyDrift = SPP::States::InterSysDrift{ satSys };
        if (!_kalmanFilter.hasState(keyBias) && satSys != newRefSys)
        {
            LOG_DEBUG("{}: Adding inter system time difference states for system [{}]", nameId, satSys);
            if (useDoppler) { _kalmanFilter.addStates({ keyBias, keyDrift }); }
            else { _kalmanFilter.addState(keyBias); }

            _kalmanFilter.P(keyBias, keyBias) = _kalmanFilter.P(SPP::States::RecvClkErr, SPP::States::RecvClkErr);
            _kalmanFilter.Phi(keyBias, keyBias) = 1;
            _kalmanFilter.G(keyBias, keyBias) = 1;
            _kalmanFilter.W(keyBias, keyBias) = _covarianceInterSysClkPhaseDrift;
            _kalmanFilter.Q(keyBias, keyBias) = _covarianceInterSysClkPhaseDrift;
            if (useDoppler)
            {
                _kalmanFilter.P(keyDrift, keyDrift) = _kalmanFilter.P(SPP::States::RecvClkDrift, SPP::States::RecvClkDrift);
                _kalmanFilter.F(keyBias, keyDrift) = 1;
                _kalmanFilter.Phi(keyDrift, keyDrift) = 1;
                _kalmanFilter.G(keyDrift, keyDrift) = 1;
                _kalmanFilter.W(keyDrift, keyDrift) = _covarianceInterSysClkFrequencyDrift;
                _kalmanFilter.Q(keyDrift, keyDrift) = _covarianceInterSysClkFrequencyDrift;
            }
        }
    }
    for (const auto& state : getStateKeys())
    {
        if (const auto* bias = std::get_if<States::InterSysBias>(&state))
        {
            if (!usedSatSystems.contains(bias->satSys))
            {
                LOG_DEBUG("{}: Removing inter system time difference states for system [{}]", nameId, bias->satSys);
                if (useDoppler) { _kalmanFilter.removeStates({ *bias, SPP::States::InterSysDrift{ bias->satSys } }); }
                else { _kalmanFilter.removeState(*bias); }
            }
        }
    }

    return newRefSys;
}

KeyedMatrixXd<States::StateKeyTypes, States::StateKeyTypes>
    KalmanFilter::calcProcessNoiseMatrixGroves(double dt, const Eigen::Vector3d& lla_pos, [[maybe_unused]] const std::string& nameId) const
{
    Eigen::MatrixXd a_S_n = Eigen::DiagonalMatrix<double, 3>(_covarianceAccel[0], _covarianceAccel[0], _covarianceAccel[1]); // Scaling matrix in n-frame
    Eigen::Matrix3d a_S_e = trafo::e_Quat_n(lla_pos(0), lla_pos(1)) * a_S_n * trafo::n_Quat_e(lla_pos(0), lla_pos(1));       // Scaling matrix in e-frame

    double dt2 = std::pow(dt, 2);
    double dt3 = dt2 * dt;

    KeyedMatrixXd<States::StateKeyTypes, States::StateKeyTypes> Q(Eigen::MatrixXd::Zero(_kalmanFilter.Q.rows(), _kalmanFilter.Q.cols()),
                                                                  _kalmanFilter.Q.rowKeys(), _kalmanFilter.Q.colKeys());

    // Groves ch. 9.4.2.2, eq. 9.152, p. 417
    Q(States::Pos, States::Pos) = dt3 / 3.0 * a_S_e;
    Q(States::Pos, States::Vel) = dt2 / 2.0 * a_S_e;
    Q(States::Vel, States::Pos) = _kalmanFilter.Q(States::Pos, States::Vel).transpose();
    Q(States::Vel, States::Vel) = dt * a_S_e;
    Q(States::RecvClkErr, States::RecvClkErr) = _covarianceClkPhaseDrift * dt + _covarianceClkFrequencyDrift * dt3 / 3.0;
    Q(States::RecvClkErr, States::RecvClkDrift) = _covarianceClkFrequencyDrift * dt2 / 2.0;
    Q(States::RecvClkDrift, States::RecvClkErr) = _kalmanFilter.Q(States::RecvClkErr, States::RecvClkDrift);
    Q(States::RecvClkDrift, States::RecvClkDrift) = _covarianceClkFrequencyDrift * dt;

    for (const auto& state : Q.rowKeys())
    {
        if (const auto* bias = std::get_if<States::InterSysBias>(&state))
        {
            Q(*bias, *bias) = _covarianceInterSysClkPhaseDrift * dt + _covarianceInterSysClkFrequencyDrift * dt3 / 3.0;
            if (auto drift = States::InterSysDrift{ bias->satSys };
                Q.hasRow(drift))
            {
                Q(*bias, drift) = _covarianceInterSysClkFrequencyDrift * dt2 / 2.0;
                Q(drift, *bias) = _kalmanFilter.Q(*bias, drift);
                Q(drift, drift) = _covarianceInterSysClkFrequencyDrift * dt;
            }
        }
    }
    return Q;
}

bool KalmanFilter::ShowGuiWidgets(const char* id, bool useDoppler, float itemWidth, float unitWidth)
{
    bool changed = false;

    itemWidth -= ImGui::GetStyle().IndentSpacing;
    float configWidth = itemWidth + unitWidth;

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::TreeNode(fmt::format("System/Process noise##{}", id).c_str()))
    {
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::Combo(fmt::format("Q calculation algorithm##{}", id).c_str(), reinterpret_cast<int*>(&_qCalculationAlgorithm), "Van Loan\0Taylor 1st Order (Groves 2013)\0\0"))
        {
            LOG_DEBUG("{}: Q calculation algorithm changed to {}", id, fmt::underlying(_qCalculationAlgorithm));
            changed = true;
        }

        if (gui::widgets::InputDouble2WithUnit(fmt::format("Acceleration due to user motion (Hor/Ver)##{}", id).c_str(),
                                               configWidth, unitWidth, _gui_covarianceAccel.data(), reinterpret_cast<int*>(&_gui_covarianceAccelUnit), "m/√(s^3)\0m^2/s^3\0\0",
                                               "%.2e", ImGuiInputTextFlags_CharsScientific))
        {
            LOG_DEBUG("{}: _gui_covarianceAccel changed to {}", id, fmt::join(_gui_covarianceAccel.begin(), _gui_covarianceAccel.end(), ", "));
            LOG_DEBUG("{}: _gui_covarianceAccelUnit changed to {}", id, fmt::underlying(_gui_covarianceAccelUnit));
            changed = true;
        }
        if (gui::widgets::InputDoubleWithUnit(fmt::format("Standard deviation of the receiver clock phase drift (RW)##{}", id).c_str(),
                                              configWidth, unitWidth, &_gui_covarianceClkPhaseDrift, reinterpret_cast<int*>(&_gui_covarianceClkPhaseDriftUnit), "m/√(s)\0m^2/s\0\0",
                                              0.0, 0.0, "%.2e", ImGuiInputTextFlags_CharsScientific))
        {
            LOG_DEBUG("{}: _gui_covarianceClkPhaseDrift changed to {}", id, _gui_covarianceClkPhaseDrift);
            LOG_DEBUG("{}: _gui_covarianceClkPhaseDriftUnit changed to {}", id, fmt::underlying(_gui_covarianceClkPhaseDriftUnit));
            changed = true;
        }
        if (useDoppler
            && gui::widgets::InputDoubleWithUnit(fmt::format("Standard deviation of the receiver clock frequency drift (IRW)##{}", id).c_str(),
                                                 configWidth, unitWidth, &_gui_covarianceClkFrequencyDrift, reinterpret_cast<int*>(&_gui_covarianceClkFrequencyDriftUnit), "m/√(s^3)\0m^2/s^3\0\0",
                                                 0.0, 0.0, "%.2e", ImGuiInputTextFlags_CharsScientific))
        {
            LOG_DEBUG("{}: _gui_covarianceClkFrequencyDrift changed to {}", id, _gui_covarianceClkFrequencyDrift);
            LOG_DEBUG("{}: _gui_covarianceClkFrequencyDriftUnit changed to {}", id, fmt::underlying(_gui_covarianceClkFrequencyDriftUnit));
            changed = true;
        }
        if (gui::widgets::InputDoubleWithUnit(fmt::format("Standard deviation of the inter-system clock phase drift (RW)##{}", id).c_str(),
                                              configWidth, unitWidth, &_gui_covarianceInterSysClkPhaseDrift, reinterpret_cast<int*>(&_gui_covarianceInterSysClkPhaseDriftUnit), "m/√(s)\0m^2/s\0\0",
                                              0.0, 0.0, "%.2e", ImGuiInputTextFlags_CharsScientific))
        {
            LOG_DEBUG("{}: _gui_covarianceInterSysClkPhaseDrift changed to {}", id, _gui_covarianceInterSysClkPhaseDrift);
            LOG_DEBUG("{}: _gui_covarianceInterSysClkPhaseDriftUnit changed to {}", id, fmt::underlying(_gui_covarianceInterSysClkPhaseDriftUnit));
            changed = true;
        }
        if (useDoppler
            && gui::widgets::InputDoubleWithUnit(fmt::format("Standard deviation of the inter-system clock frequency drift (IRW)##{}", id).c_str(),
                                                 configWidth, unitWidth, &_gui_covarianceInterSysClkFrequencyDrift, reinterpret_cast<int*>(&_gui_covarianceInterSysClkFrequencyDriftUnit), "m/√(s^3)\0m^2/s^3\0\0",
                                                 0.0, 0.0, "%.2e", ImGuiInputTextFlags_CharsScientific))
        {
            LOG_DEBUG("{}: _gui_covarianceInterSysClkFrequencyDrift changed to {}", id, _gui_covarianceInterSysClkFrequencyDrift);
            LOG_DEBUG("{}: _gui_covarianceInterSysClkFrequencyDriftUnit changed to {}", id, fmt::underlying(_gui_covarianceInterSysClkFrequencyDriftUnit));
            changed = true;
        }

        ImGui::TreePop();
    }

    return changed;
}

void KalmanFilter::setClockBiasErrorCovariance(double clkPhaseDrift)
{
    _kalmanFilter.P(SPP::States::RecvClkErr, SPP::States::RecvClkErr) = clkPhaseDrift;
}

const std::vector<SPP::States::StateKeyTypes>& KalmanFilter::getStateKeys() const
{
    return _kalmanFilter.x.rowKeys();
}

const KeyedVectorX<double, States::StateKeyTypes>& KalmanFilter::getState() const
{
    return _kalmanFilter.x;
}

const KeyedMatrixXd<States::StateKeyTypes, States::StateKeyTypes>& KalmanFilter::getErrorCovarianceMatrix() const
{
    return _kalmanFilter.P;
}

void to_json(json& j, const KalmanFilter& data)
{
    j["qCalculationAlgorithm"] = data._qCalculationAlgorithm;
    j["covarianceAccelUnit"] = data._gui_covarianceAccelUnit;
    j["covarianceAccel"] = data._gui_covarianceAccel;
    j["covarianceClkPhaseDriftUnit"] = data._gui_covarianceClkPhaseDriftUnit;
    j["covarianceClkPhaseDrift"] = data._gui_covarianceClkPhaseDrift;
    j["covarianceClkFrequencyDriftUnit"] = data._gui_covarianceClkFrequencyDriftUnit;
    j["covarianceClkFrequencyDrift"] = data._gui_covarianceClkFrequencyDrift;
    j["covarianceInterSysClkPhaseDriftUnit"] = data._gui_covarianceInterSysClkPhaseDriftUnit;
    j["covarianceInterSysClkPhaseDrift"] = data._gui_covarianceInterSysClkPhaseDrift;
    j["covarianceInterSysClkFrequencyDriftUnit"] = data._gui_covarianceInterSysClkFrequencyDriftUnit;
    j["covarianceInterSysClkFrequencyDrift"] = data._gui_covarianceInterSysClkFrequencyDrift;
}

void from_json(const json& j, KalmanFilter& data)
{
    if (j.contains("qCalculationAlgorithm")) { j.at("qCalculationAlgorithm").get_to(data._qCalculationAlgorithm); }
    if (j.contains("covarianceAccelUnit")) { j.at("covarianceAccelUnit").get_to(data._gui_covarianceAccelUnit); }
    if (j.contains("covarianceAccel")) { j.at("covarianceAccel").get_to(data._gui_covarianceAccel); }
    if (j.contains("covarianceClkPhaseDriftUnit")) { j.at("covarianceClkPhaseDriftUnit").get_to(data._gui_covarianceClkPhaseDriftUnit); }
    if (j.contains("covarianceClkPhaseDrift")) { j.at("covarianceClkPhaseDrift").get_to(data._gui_covarianceClkPhaseDrift); }
    if (j.contains("covarianceClkFrequencyDriftUnit")) { j.at("covarianceClkFrequencyDriftUnit").get_to(data._gui_covarianceClkFrequencyDriftUnit); }
    if (j.contains("covarianceClkFrequencyDrift")) { j.at("covarianceClkFrequencyDrift").get_to(data._gui_covarianceClkFrequencyDrift); }
    if (j.contains("covarianceInterSysClkPhaseDriftUnit")) { j.at("covarianceInterSysClkPhaseDriftUnit").get_to(data._gui_covarianceInterSysClkPhaseDriftUnit); }
    if (j.contains("covarianceInterSysClkPhaseDrift")) { j.at("covarianceInterSysClkPhaseDrift").get_to(data._gui_covarianceInterSysClkPhaseDrift); }
    if (j.contains("covarianceInterSysClkFrequencyDriftUnit")) { j.at("covarianceInterSysClkFrequencyDriftUnit").get_to(data._gui_covarianceInterSysClkFrequencyDriftUnit); }
    if (j.contains("covarianceInterSysClkFrequencyDrift")) { j.at("covarianceInterSysClkFrequencyDrift").get_to(data._gui_covarianceInterSysClkFrequencyDrift); }
}

} // namespace NAV::SPP