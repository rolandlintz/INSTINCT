// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file Allo223mA_22n.hpp
/// @brief Test Data for the file Allo223mA.22n
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-12-16

#pragma once

// This is a small hack, which lets us change private/protected parameters
#pragma GCC diagnostic push
#if defined(__clang__)
    #pragma GCC diagnostic ignored "-Wkeyword-macro"
    #pragma GCC diagnostic ignored "-Wmacro-redefined"
#endif
#define protected public
#define private public
#include "NodeData/GNSS/GnssNavInfo.hpp"
#undef protected
#undef private
#pragma GCC diagnostic pop

#include "Navigation/GNSS/Satellite/Ephemeris/GPSEphemeris.hpp"

namespace NAV::TESTS::RinexNavFileTests::v3_02
{
/// @brief Test Data for the file Allo223mA.22n
const GnssNavInfo gnssNavInfo_Allo223mA_22n = {
    .satelliteSystems = { GPS },
    .ionosphericCorrections = { {
        { .satSys = GPS, .alphaBeta = IonosphericCorrections::Alpha, .data = { 0.8382e-08, 0.2235e-07, -0.5960e-07, -0.1192e-06 } },
        { .satSys = GPS, .alphaBeta = IonosphericCorrections::Beta, .data = { 0.9216e+05, 0.1147e+06, -0.6554e+05, -0.5898e+06 } },
    } },
    .timeSysCorr = {
        { { GPST, UTC }, { -0.9313225746e-09, 0.000000000e+00 } },
    },
    .m_satellites = {
        { { GPS, 17 }, Satellite{
                           .m_navigationData /* std::vector<std::shared_ptr<SatNavData>> */ = {
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 12, 0, 0, 0.647211447358e-03, 0.409272615798e-11, 0.000000000000e+00, //
                                                              0.440000000000e+02, 0.134468750000e+03, 0.390051961516e-08, -0.303394679645e+01,   //
                                                              0.697933137417e-05, 0.136003979715e-01, 0.107474625111e-04, 0.515360196686e+04,    //
                                                              0.388800000000e+06, 0.262632966042e-06, 0.242162763257e+00, 0.931322574615e-07,    //
                                                              0.978289642399e+00, 0.178468750000e+03, -0.148589873168e+01, -0.743709549917e-08,  //
                                                              0.453590322429e-10, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.111758708954e-07, 0.440000000000e+02,   //
                                                              0.383406000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),   //
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 14, 0, 0, 0.647241249681e-03, 0.409272615798e-11, 0.000000000000e+00, //
                                                              0.450000000000e+02, 0.119031250000e+03, 0.378801492888e-08, -0.198363427817e+01,   //
                                                              0.646524131298e-05, 0.135986971436e-01, 0.109169632196e-04, 0.515360589981e+04,    //
                                                              0.396000000000e+06, 0.150874257088e-06, 0.242109198511e+00, 0.240281224251e-06,    //
                                                              0.978290190993e+00, 0.176562500000e+03, -0.148599244475e+01, -0.746138222509e-08,  //
                                                              0.864321716755e-10, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.111758708954e-07, 0.450000000000e+02,   //
                                                              0.388818000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),   //
                           },
                       } },
        { { GPS, 14 }, Satellite{
                           .m_navigationData /* std::vector<std::shared_ptr<SatNavData>> */ = {
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 12, 0, 0, -0.109632965177e-03, 0.136424205266e-11, 0.000000000000e+00, //
                                                              0.310000000000e+02, -0.260312500000e+02, 0.486877423256e-08, -0.379655971818e+00,   //
                                                              -0.130198895931e-05, 0.219930591993e-02, 0.840239226818e-05, 0.515365441513e+04,    //
                                                              0.388800000000e+06, 0.447034835815e-07, -0.829752086985e+00, 0.502914190292e-07,    //
                                                              0.951562004743e+00, 0.211468750000e+03, -0.305661533675e+01, -0.809640867649e-08,   //
                                                              -0.548594279725e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.791624188423e-08, 0.543000000000e+03,    //
                                                              0.381618000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),    //
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 14, 0, 0, -0.109622720629e-03, 0.147792889038e-11, 0.000000000000e+00, //
                                                              0.320000000000e+02, -0.295000000000e+02, 0.484770192624e-08, 0.670665381025e+00,    //
                                                              -0.150874257088e-05, 0.219960033428e-02, 0.832974910736e-05, 0.515365489197e+04,    //
                                                              0.396000000000e+06, 0.186264514923e-08, -0.829810517396e+00, 0.633299350739e-07,    //
                                                              0.951557926127e+00, 0.215156250000e+03, -0.305674210592e+01, -0.812283834882e-08,   //
                                                              -0.552880172536e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.791624188423e-08, 0.544000000000e+03,    //
                                                              0.388818000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),    //
                           },
                       } },
        { { GPS, 22 }, Satellite{
                           .m_navigationData /* std::vector<std::shared_ptr<SatNavData>> */ = {
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 12, 0, 0, 0.307971611619e-03, 0.659383658785e-11, 0.000000000000e+00, //
                                                              0.780000000000e+02, -0.598750000000e+02, 0.393516391537e-08, 0.306181691022e+01,   //
                                                              -0.294297933578e-05, 0.134400288807e-01, 0.122245401144e-04, 0.515373084068e+04,   //
                                                              0.388800000000e+06, 0.143423676491e-06, -0.287856453846e+01, -0.745058059692e-07,  //
                                                              0.962138004224e+00, 0.140906250000e+03, -0.184511752288e+01, -0.735102048522e-08,  //
                                                              0.460733477113e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.791624188423e-08, 0.780000000000e+02,   //
                                                              0.381618000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),   //
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 14, 0, 0, 0.308019109070e-03, 0.659383658785e-11, 0.000000000000e+00, //
                                                              0.790000000000e+02, -0.620625000000e+02, 0.396695095372e-08, -0.217118207731e+01,  //
                                                              -0.316463410854e-05, 0.134384377161e-01, 0.130757689476e-04, 0.515373399544e+04,   //
                                                              0.396000000000e+06, 0.242143869400e-06, -0.287861742149e+01, -0.353902578354e-07,  //
                                                              0.962141354306e+00, 0.128625000000e+03, -0.184516320104e+01, -0.738423615450e-08,  //
                                                              0.446447167745e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,    //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.791624188423e-08, 0.790000000000e+02,   //
                                                              0.388818000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),   //
                           },
                       } },
        { { GPS, 31 }, Satellite{
                           .m_navigationData /* std::vector<std::shared_ptr<SatNavData>> */ = {
                               std::make_shared<GPSEphemeris>(2022, 8, 11, 12, 0, 0, -0.189237296581e-03, -0.136424205266e-11, 0.000000000000e+00, //
                                                              0.390000000000e+02, 0.250000000000e+02, 0.501699469225e-08, -0.178208980697e+00,     //
                                                              0.153295695782e-05, 0.104991269764e-01, 0.562705099583e-05, 0.515377913284e+04,      //
                                                              0.388800000000e+06, 0.117346644402e-06, -0.189093480196e+01, 0.633299350739e-07,     //
                                                              0.954485201797e+00, 0.265218750000e+03, 0.412827406661e+00, -0.838856370306e-08,     //
                                                              0.506092509356e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,      //
                                                              0.200000000000e+01, 0.000000000000e+00, -0.135041773319e-07, 0.390000000000e+02,     //
                                                              0.387378000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),     //
                           },
                       } },
        { { GPS, 3 }, Satellite{
                          .m_navigationData /* std::vector<std::shared_ptr<SatNavData>> */ = {
                              std::make_shared<GPSEphemeris>(2022, 8, 11, 14, 0, 0, -0.329373404384e-03, -0.852651282912e-11, 0.000000000000e+00, //
                                                             0.480000000000e+02, -0.130531250000e+03, 0.439375444608e-08, 0.939903317146e+00,     //
                                                             -0.680051743984e-05, 0.445418071467e-02, 0.368803739548e-05, 0.515357682991e+04,     //
                                                             0.396000000000e+06, -0.316649675369e-07, 0.227508457870e+01, 0.204890966415e-07,     //
                                                             0.974888529026e+00, 0.317031250000e+03, 0.103050388334e+01, -0.824855787126e-08,     //
                                                             -0.451447376024e-09, 0.100000000000e+01, 0.222200000000e+04, 0.000000000000e+00,     //
                                                             0.200000000000e+01, 0.000000000000e+00, 0.186264514923e-08, 0.480000000000e+02,      //
                                                             0.388818000000e+06, 0.400000000000e+01, 0.000000000000e+00, 0.000000000000e+00),     //
                          },
                      } },
    },
};

} // namespace NAV::TESTS::RinexNavFileTests::v3_02