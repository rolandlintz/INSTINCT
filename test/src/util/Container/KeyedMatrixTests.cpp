// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file KeyedMatrixTests.cpp
/// @brief UnitTests for the KeyedMatrix class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2023-07-06

#include <catch2/catch_test_macros.hpp>
#include <variant>

#include "Logger.hpp"
#include "util/Container/KeyedMatrix.hpp"

namespace NAV::TESTS
{

TEST_CASE("[KeyedMatrix] Special member functions (static size)", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };
    std::vector<int> colKeys = { 1 };

    KeyedMatrix<double, int, int, 2, 1> a(matA, rowKeys, colKeys);
    REQUIRE(a(all, all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);
    REQUIRE(a.colKeys() == colKeys);

    KeyedMatrix<double, int, int, 2, 1> b(matB, rowKeys, colKeys);
    REQUIRE(b(all, all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedMatrix<double, int, int, 2, 1> c(a);
    REQUIRE(c(all, all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all, all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedMatrix<double, int, int, 2, 1> d(std::move(a));
    REQUIRE(d(all, all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all, all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedMatrix] Special member functions (dynamic size)", "[KeyedMatrix][Debug]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };
    std::vector<int> colKeys = { 1 };

    KeyedMatrixX<double, int, int> a(matA, rowKeys, colKeys);
    REQUIRE(a(all, all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);
    REQUIRE(a.colKeys() == colKeys);

    KeyedMatrixX<double, int, int> b(matB, rowKeys, colKeys);
    REQUIRE(b(all, all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedMatrixX<double, int, int> c(a);
    REQUIRE(c(all, all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all, all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedMatrixX<double, int, int> d(std::move(a));
    REQUIRE(d(all, all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all, all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedMatrix] Special member functions (static <- dynamic)", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };
    std::vector<int> colKeys = { 1 };

    KeyedMatrixX<double, int, int> a(matA, rowKeys, colKeys);
    REQUIRE(a(all, all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);
    REQUIRE(a.colKeys() == colKeys);

    KeyedMatrixX<double, int, int> b(matB, rowKeys, colKeys);
    REQUIRE(b(all, all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedMatrix<double, int, int, 2, 1> c(a);
    REQUIRE(c(all, all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all, all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedMatrix<double, int, int, 2, 1> d(std::move(a));
    REQUIRE(d(all, all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all, all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedMatrix] Special member functions (dynamic <- static)", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };
    std::vector<int> colKeys = { 1 };

    KeyedMatrix<double, int, int, 2, 1> a(matA, rowKeys, colKeys);
    REQUIRE(a(all, all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);
    REQUIRE(a.colKeys() == colKeys);

    KeyedMatrix<double, int, int, 2, 1> b(matB, rowKeys, colKeys);
    REQUIRE(b(all, all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedMatrixX<double, int, int> c(a);
    REQUIRE(c(all, all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all, all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedMatrixX<double, int, int> d(std::move(a));
    REQUIRE(d(all, all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all, all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedVector] Special member functions (static size)", "[KeyedVector]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };

    KeyedVector<double, int, 2> a(matA, rowKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);

    KeyedVector<double, int, 2> b(matB, rowKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);

    // Copy constructor
    KeyedVector<double, int, 2> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);

    // Move constructor
    KeyedVector<double, int, 2> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
}

TEST_CASE("[KeyedVector] Special member functions (dynamic size)", "[KeyedVector]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };

    KeyedVectorX<double, int> a(matA, rowKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);

    KeyedVectorX<double, int> b(matB, rowKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);

    // Copy constructor
    KeyedVectorX<double, int> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);

    // Move constructor
    KeyedVectorX<double, int> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
}

TEST_CASE("[KeyedVector] Special member functions (static <- dynamic)", "[KeyedVector]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };

    KeyedVector<double, int, 2> a(matA, rowKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);

    KeyedVector<double, int, 2> b(matB, rowKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);

    // Copy constructor
    KeyedVectorX<double, int> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);

    // Move constructor
    KeyedVectorX<double, int> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
}

TEST_CASE("[KeyedVector] Special member functions (dynamic <- static)", "[KeyedVector]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector2d matA(1, 2);
    Eigen::Vector2d matB(3, 4);
    std::vector<int> rowKeys = { 1, 2 };

    KeyedVectorX<double, int> a(matA, rowKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.rowKeys() == rowKeys);

    KeyedVectorX<double, int> b(matB, rowKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.rowKeys() == rowKeys);

    // Copy constructor
    KeyedVector<double, int, 2> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.rowKeys() == rowKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.rowKeys() == rowKeys);

    // Move constructor
    KeyedVector<double, int, 2> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.rowKeys() == rowKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.rowKeys() == rowKeys);
}

TEST_CASE("[KeyedRowVector] Special member functions (static size)", "[KeyedRowVector]")
{
    auto logger = initializeTestLogger();

    Eigen::RowVector2d matA(1, 2);
    Eigen::RowVector2d matB(3, 4);
    std::vector<int> colKeys = { 1, 2 };

    KeyedRowVector<double, int, 2> a(matA, colKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.colKeys() == colKeys);

    KeyedRowVector<double, int, 2> b(matB, colKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedRowVector<double, int, 2> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedRowVector<double, int, 2> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedRowVector] Special member functions (dynamic size)", "[KeyedRowVector]")
{
    auto logger = initializeTestLogger();

    Eigen::RowVector2d matA(1, 2);
    Eigen::RowVector2d matB(3, 4);
    std::vector<int> colKeys = { 1, 2 };

    KeyedRowVectorX<double, int> a(matA, colKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.colKeys() == colKeys);

    KeyedRowVectorX<double, int> b(matB, colKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedRowVectorX<double, int> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedRowVectorX<double, int> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedRowVector] Special member functions (static <- dynamic)", "[KeyedRowVector]")
{
    auto logger = initializeTestLogger();

    Eigen::RowVector2d matA(1, 2);
    Eigen::RowVector2d matB(3, 4);
    std::vector<int> colKeys = { 1, 2 };

    KeyedRowVector<double, int, 2> a(matA, colKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.colKeys() == colKeys);

    KeyedRowVector<double, int, 2> b(matB, colKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedRowVectorX<double, int> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedRowVectorX<double, int> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedRowVector] Special member functions (dynamic <- static)", "[KeyedRowVector]")
{
    auto logger = initializeTestLogger();

    Eigen::RowVector2d matA(1, 2);
    Eigen::RowVector2d matB(3, 4);
    std::vector<int> colKeys = { 1, 2 };

    KeyedRowVectorX<double, int> a(matA, colKeys);
    REQUIRE(a(all) == matA);
    REQUIRE(a.colKeys() == colKeys);

    KeyedRowVectorX<double, int> b(matB, colKeys);
    REQUIRE(b(all) == matB);
    REQUIRE(b.colKeys() == colKeys);

    // Copy constructor
    KeyedRowVector<double, int, 2> c(a);
    REQUIRE(c(all) == matA);
    REQUIRE(c.colKeys() == colKeys);

    // Copy assignment
    c = b;
    REQUIRE(c(all) == matB);
    REQUIRE(c.colKeys() == colKeys);

    // Move constructor
    KeyedRowVector<double, int, 2> d(std::move(a));
    REQUIRE(d(all) == matA);
    REQUIRE(d.colKeys() == colKeys);

    // Move assignment
    d = std::move(b);
    REQUIRE(d(all) == matB);
    REQUIRE(d.colKeys() == colKeys);
}

TEST_CASE("[KeyedMatrix] Constructors static sized", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Matrix3d eigMat;
    eigMat << 1, 2, 3,
        4, 5, 6,
        7, 8, 9;

    const KeyedMatrix<double, int, int, 3, 3> mat(eigMat, { 1, 2, 3 }, { 1, 2, 3 });
    REQUIRE(mat(1, 2) == 2);
    REQUIRE(mat(1, 3) == 3);
    REQUIRE(mat(2, 1) == 4);
    REQUIRE(mat(2, 2) == 5);
    REQUIRE(mat(2, 3) == 6);
    REQUIRE(mat(3, 1) == 7);
    REQUIRE(mat(3, 2) == 8);
    REQUIRE(mat(3, 3) == 9);

    KeyedMatrix<double, int, int, 3, 3> mat2(eigMat, { 1, 2, 3 });
    REQUIRE(mat(1, 1) == 1);
    REQUIRE(mat(1, 2) == 2);
    REQUIRE(mat(1, 3) == 3);
    REQUIRE(mat(2, 1) == 4);
    REQUIRE(mat(2, 2) == 5);
    REQUIRE(mat(2, 3) == 6);
    REQUIRE(mat(3, 1) == 7);
    REQUIRE(mat(3, 2) == 8);
    REQUIRE(mat(3, 3) == 9);
}

TEST_CASE("[KeyedMatrix] Constructors dynamic sized", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    enum Keys
    {
        ONE,
        TWO,
        THREE,
    };

    Eigen::Matrix3d eigMat;
    eigMat << 1, 2, 3,
        4, 5, 6,
        7, 8, 9;

    KeyedMatrixX<double, Keys, int> mat;
    mat = KeyedMatrixX<double, Keys, int>(eigMat, { ONE, TWO, THREE }, { 1, 2, 3 });
    REQUIRE(mat(ONE, 1) == 1);
    REQUIRE(mat(ONE, 2) == 2);
    REQUIRE(mat(ONE, 3) == 3);
    REQUIRE(mat(TWO, 1) == 4);
    REQUIRE(mat(TWO, 2) == 5);
    REQUIRE(mat(TWO, 3) == 6);
    REQUIRE(mat(THREE, 1) == 7);
    REQUIRE(mat(THREE, 2) == 8);
    REQUIRE(mat(THREE, 3) == 9);

    KeyedMatrixX<double, int, int> mat2(eigMat, { 1, 2, 3 });
    REQUIRE(mat2(1, 1) == 1);
    REQUIRE(mat2(1, 2) == 2);
    REQUIRE(mat2(1, 3) == 3);
    REQUIRE(mat2(2, 1) == 4);
    REQUIRE(mat2(2, 2) == 5);
    REQUIRE(mat2(2, 3) == 6);
    REQUIRE(mat2(3, 1) == 7);
    REQUIRE(mat2(3, 2) == 8);
    REQUIRE(mat2(3, 3) == 9);
}

TEST_CASE("[KeyedMatrix] rowKeys & colKeys", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    enum Keys
    {
        ONE,
        TWO,
        THREE,
    };

    KeyedMatrixX<double, Keys, int> mat(Eigen::Matrix3d{}, { ONE, TWO, THREE }, { 1, 2, 3 });
    REQUIRE(mat.rowKeys() == std::vector<Keys>{ ONE, TWO, THREE });
    REQUIRE(mat.colKeys() == std::vector<int>{ 1, 2, 3 });
}

TEST_CASE("[KeyedMatrix] hasRow(s) & hasCol(s)", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    enum Keys
    {
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
    };

    KeyedMatrixX<double, Keys, int> mat(Eigen::Matrix3d{}, { ONE, TWO, THREE }, { 1, 2, 3 });
    REQUIRE(mat.hasRow(TWO));
    REQUIRE(!mat.hasRow(FOUR));

    REQUIRE(mat.hasCol(1));
    REQUIRE(!mat.hasCol(4));

    REQUIRE(mat.hasRows({ ONE }));
    REQUIRE(mat.hasRows({ ONE, THREE }));
    REQUIRE(!mat.hasRows({ ONE, FOUR }));

    REQUIRE(mat.hasCols({ 2 }));
    REQUIRE(mat.hasCols({ 2, 3 }));
    REQUIRE(!mat.hasCols({ 9 }));
    REQUIRE(!mat.hasCols({ 1, 9 }));

    REQUIRE(mat.hasAnyRows({ ONE }));
    REQUIRE(mat.hasAnyRows({ ONE, THREE }));
    REQUIRE(mat.hasAnyRows({ ONE, FOUR }));
    REQUIRE(!mat.hasAnyRows({ FOUR, FIVE }));

    REQUIRE(mat.hasAnyCols({ 2 }));
    REQUIRE(mat.hasAnyCols({ 2, 3 }));
    REQUIRE(!mat.hasAnyCols({ 9 }));
    REQUIRE(mat.hasAnyCols({ 1, 9 }));
}

TEST_CASE("[KeyedMatrix] operator(rowKey(s), colKey(s))", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Matrix3d eigMat;
    eigMat << 1, 2, 3,
        4, 5, 6,
        7, 8, 9;
    {
        enum Keys
        {
            ONE,
            TWO,
            THREE,
        };

        KeyedMatrixX<double, Keys, int> mat(eigMat, { ONE, TWO, THREE }, { 1, 2, 3 });

        REQUIRE(mat(all, all) == eigMat);
        REQUIRE(mat(ONE, 1) == 1.0);
        REQUIRE(mat({ ONE, THREE }, 1) == Eigen::Vector2d(1, 7));
        REQUIRE(mat(TWO, { 2, 3 }) == Eigen::RowVector2d(5, 6));
        REQUIRE(mat({ TWO, THREE }, { 2, 3 }) == (Eigen::Matrix2d(2, 2) << 5, 6, 8, 9).finished());
    }
    {
        KeyedMatrixX<double, int, int> mat(eigMat, { 1, 2, 3 }, { 1, 2, 3 });

        REQUIRE(mat(all, all) == eigMat);
        REQUIRE(mat(1, 1) == 1.0);
        REQUIRE(mat({ 1, 3 }, 1) == Eigen::Vector2d(1, 7));
        REQUIRE(mat(2, { 2, 3 }) == Eigen::RowVector2d(5, 6));
        REQUIRE(mat({ 2, 3 }, { 2, 3 }) == (Eigen::Matrix2d(2, 2) << 5, 6, 8, 9).finished());
    }
    {
        // The string literal ""s is needed
        using namespace std::string_literals;

        KeyedMatrixX<double, const char*, std::string> mat(eigMat, { "1", "2", "3" }, { "1"s, "2"s, "3"s });

        REQUIRE(mat(all, all) == eigMat);
        REQUIRE(mat("1", "1") == 1.0);
        REQUIRE(mat({ "1", "3" }, "1"s) == Eigen::Vector2d(1, 7));
        REQUIRE(mat("2", { "2"s, "3"s }) == Eigen::RowVector2d(5, 6));
        REQUIRE(mat({ "2", "3" }, { "2"s, "3"s }) == (Eigen::Matrix2d(2, 2) << 5, 6, 8, 9).finished());
    }
}

TEST_CASE("[KeyedMatrix] addRows & addCols", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    KeyedMatrixX<double, const char*, int> mat;
    mat.addRow("1");
    mat.addCol(1);
    REQUIRE(mat(all, all) == Eigen::VectorXd::Zero(1));
    REQUIRE(mat("1", 1) == 0);
    REQUIRE(mat.rowKeys() == std::vector{ "1" });
    REQUIRE(mat.colKeys() == std::vector{ 1 });

    mat.addRows({ "2", "3" });
    REQUIRE(mat(all, all) == Eigen::MatrixXd::Zero(3, 1));
    REQUIRE(mat("2", 1) == 0);
    REQUIRE(mat.rowKeys() == std::vector{ "1", "2", "3" });
    REQUIRE(mat.colKeys() == std::vector{ 1 });

    mat.addCols({ 2, 3 });
    REQUIRE(mat(all, all) == Eigen::Matrix3d::Zero());

    Eigen::Matrix3d eigMat;
    eigMat << 1, 2, 3,
        4, 5, 6,
        7, 8, 9;
    mat(all, all) = eigMat;

    REQUIRE(mat("2", 1) == 4);

    mat.addRowsCols({ "4", "5" }, { 4, 5 });
    REQUIRE(mat.rowKeys() == std::vector{ "1", "2", "3", "4", "5" });
    REQUIRE(mat.colKeys() == std::vector{ 1, 2, 3, 4, 5 });
    // clang-format off
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(5, 5) << 1, 2, 3, 0, 0,
                                                       4, 5, 6, 0, 0,
                                                       7, 8, 9, 0, 0,
                                                       0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0).finished());
    // clang-format on

    mat.removeRowsCols({ "1", "3", "4" }, { 2, 4 });
    REQUIRE(mat.rowKeys() == std::vector{ "2", "5" });
    REQUIRE(mat.colKeys() == std::vector{ 1, 3, 5 });
    // clang-format off
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(2, 3) << 4, 6, 0,
                                                       0, 0, 0).finished());
    // clang-format on
}

TEST_CASE("[KeyedMatrix] removeRows & removeCols", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Matrix4d eigMat;
    eigMat << 1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16;

    enum Keys
    {
        ONE,
        TWO,
        THREE,
        FOUR,
    };

    //          1   2   3   4
    // ONE   |  1   2   3   4
    // TWO   |  5   6   7   8
    // THREE |  9  10  11  12
    // FOUR  | 13  14  15  16
    KeyedMatrixX<double, Keys, int> mat(eigMat, { ONE, TWO, THREE, FOUR }, { 1, 2, 3, 4 });

    mat.removeRow(TWO);
    //          1   2   3   4
    // ONE   |  1   2   3   4
    // THREE |  9  10  11  12
    // FOUR  | 13  14  15  16
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(3, 4) << 1, 2, 3, 4, 9, 10, 11, 12, 13, 14, 15, 16).finished());
    REQUIRE(mat.rowKeys() == std::vector{ ONE, THREE, FOUR });
    REQUIRE(mat.colKeys() == std::vector{ 1, 2, 3, 4 });
    REQUIRE(mat({ ONE, FOUR }, { 3, 4 }) == (Eigen::Matrix2d() << 3, 4, 15, 16).finished());

    mat.removeCol(1);
    //          2   3   4
    // ONE   |  2   3   4
    // THREE | 10  11  12
    // FOUR  | 14  15  16
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(3, 3) << 2, 3, 4, 10, 11, 12, 14, 15, 16).finished());
    REQUIRE(mat.rowKeys() == std::vector{ ONE, THREE, FOUR });
    REQUIRE(mat.colKeys() == std::vector{ 2, 3, 4 });
    REQUIRE(mat({ ONE, THREE }, { 3, 4 }) == (Eigen::Matrix2d() << 3, 4, 11, 12).finished());

    mat.removeCols({ 2, 4 });
    //          3
    // ONE   |  3
    // THREE | 11
    // FOUR  | 15
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(3, 1) << 3, 11, 15).finished());
    REQUIRE(mat.rowKeys() == std::vector{ ONE, THREE, FOUR });
    REQUIRE(mat.colKeys() == std::vector{ 3 });
    REQUIRE(mat({ THREE, FOUR }, 3) == (Eigen::Vector2d() << 11, 15).finished());

    mat.removeRows({ ONE, THREE });
    //          3
    // FOUR  | 15
    REQUIRE(mat(all, all) == (Eigen::MatrixXd(1, 1) << 15).finished());
    REQUIRE(mat.rowKeys() == std::vector{ FOUR });
    REQUIRE(mat.colKeys() == std::vector{ 3 });
    REQUIRE(mat(FOUR, 3) == 15);
}

TEST_CASE("[KeyedMatrix] Access with all", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    Eigen::Matrix3d eigMat;
    eigMat << 1, 2, 3,
        4, 5, 6,
        7, 8, 9;

    KeyedMatrixX<double, const char*, int> mat(eigMat, { "1", "2", "3" }, { 1, 2, 3 });

    REQUIRE(mat(all, all) == eigMat);
    REQUIRE(mat("1", all) == eigMat.row(0));
    REQUIRE(mat(all, 2) == eigMat.col(1));
    REQUIRE(mat({ "1", "3" }, all) == eigMat({ 0, 2 }, Eigen::all));
    REQUIRE(mat(all, { 1, 2 }) == eigMat.leftCols(2));
}

TEST_CASE("[KeyedMatrix] Access with alias", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    enum Keys
    {
        Px,
        Py,
        Vx,
        Vy,
    };
    Eigen::Matrix4<std::string> eigMat;
    eigMat << "Px-Px", "Px-Py", "Px-Vx", "Px-Vy",
        "Py-Px", "Py-Py", "Py-Vx", "Py-Vy",
        "Vx-Px", "Vx-Py", "Vx-Vx", "Vx-Vy",
        "Vy-Px", "Vy-Py", "Vy-Vx", "Vy-Vy";

    KeyedMatrixX<std::string, Keys> mat(eigMat, { Px, Py, Vx, Vy });

    static const std::vector<Keys> Pos = { Px, Py };
    static const std::vector<Keys> Vel = { Vx, Vy };

    REQUIRE(mat(Pos, all) == eigMat({ 0, 1 }, Eigen::all));
    REQUIRE(mat(Pos, Vel) == eigMat({ 0, 1 }, { 2, 3 }));
    Eigen::Matrix2<std::string> refMat;
    refMat << "Px-Vx", "Px-Vy",
        "Py-Vx", "Py-Vy";
    REQUIRE(mat(Pos, Vel) == refMat);
}

} // namespace NAV::TESTS

namespace keym
{
enum Keys
{
    Position,
    Velocity,
    COUNT
};

struct Ambiguity
{
    constexpr bool operator==(const Ambiguity& rhs) const { return number == rhs.number; }
    size_t number = 0;
};

struct Pseudorange
{
    constexpr bool operator==(const Pseudorange& rhs) const { return number == rhs.number; }
    size_t number = 0;
};
struct Carrierphase
{
    constexpr bool operator==(const Carrierphase& rhs) const { return number == rhs.number; }
    size_t number = 0;
};
} // namespace keym

namespace std
{
/// @brief Hash function (needed for unordered_map)
template<>
struct hash<keym::Ambiguity>
{
    /// @brief Hash function
    /// @param[in] a Ambiguity
    size_t operator()(const keym::Ambiguity& a) const { return keym::Keys::COUNT + a.number; }
};
/// @brief Hash function (needed for unordered_map)
template<>
struct hash<keym::Pseudorange>
{
    /// @brief Hash function
    /// @param[in] psr Pseudorange
    size_t operator()(const keym::Pseudorange& psr) const { return psr.number; }
};
/// @brief Hash function (needed for unordered_map)
template<>
struct hash<keym::Carrierphase>
{
    /// @brief Hash function
    /// @param[in] cp Carrierphase
    size_t operator()(const keym::Carrierphase& cp) const { return 1000 + cp.number; }
};

} // namespace std

namespace NAV::TESTS
{
TEST_CASE("[KeyedMatrix] std::variant as Keys", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();

    using keym::Keys, keym::Ambiguity, keym::Pseudorange, keym::Carrierphase;

    Eigen::Matrix4d eigMat;
    eigMat << 1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16;

    using RowKeys = std::variant<Keys, Ambiguity>;
    using ColKeys = std::variant<Pseudorange, Carrierphase>;

    KeyedMatrixX<double, RowKeys, ColKeys> mat(eigMat,
                                               { Keys::Position, Keys::Velocity, Ambiguity{ 0 }, Ambiguity{ 1 } },
                                               { Pseudorange{ 0 }, Pseudorange{ 1 }, Carrierphase{ 0 }, Carrierphase{ 1 } });
    REQUIRE(mat(all, all) == eigMat);
    REQUIRE(mat(Keys::Position, all) == eigMat(0, Eigen::all));
    REQUIRE(mat({ Keys::Position, Ambiguity{ 1 } }, all) == eigMat({ 0, 3 }, Eigen::all));
}

TEST_CASE("[KeyedVector] All functions", "[KeyedVector]")
{
    auto logger = initializeTestLogger();

    Eigen::Vector3d eigVec(1, 2, 3);
    {
        KeyedVector3d<const char*> vec(eigVec, { "1", "2", "3" });
        REQUIRE(vec.rowKeys() == std::vector{ "1", "2", "3" });
        REQUIRE(vec(all) == eigVec);
    }

    KeyedVectorXd<const char*> vec(eigVec, { "1", "2", "3" });

    vec.addRow("4");
    REQUIRE(vec.rowKeys() == std::vector{ "1", "2", "3", "4" });
    REQUIRE(vec.hasRow("2"));
    REQUIRE(vec.hasRows({ "2", "3" }));
    REQUIRE(vec(all) == (Eigen::VectorXd(4) << 1, 2, 3, 0).finished());

    vec.addRows({ "5", "6", "7", "8" });
    REQUIRE(vec.rowKeys() == std::vector{ "1", "2", "3", "4", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::VectorXd(8) << 1, 2, 3, 0, 0, 0, 0, 0).finished());

    vec(all) = (Eigen::VectorXd(8) << 1, 2, 3, 4, 5, 6, 7, 8).finished();
    REQUIRE(vec(all) == (Eigen::VectorXd(8) << 1, 2, 3, 4, 5, 6, 7, 8).finished());

    vec.removeRow("2");
    REQUIRE(vec.rowKeys() == std::vector{ "1", "3", "4", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::VectorXd(7) << 1, 3, 4, 5, 6, 7, 8).finished());

    vec.removeRows({ "3", "4" });
    REQUIRE(vec.rowKeys() == std::vector{ "1", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::VectorXd(5) << 1, 5, 6, 7, 8).finished());
}

TEST_CASE("[KeyedRowVector] All functions", "[KeyedRowVector]")
{
    auto logger = initializeTestLogger();

    Eigen::RowVector3d eigVec(1, 2, 3);
    {
        KeyedRowVector3d<const char*> vec(eigVec, { "1", "2", "3" });
        REQUIRE(vec.colKeys() == std::vector{ "1", "2", "3" });
        REQUIRE(vec(all) == eigVec);
    }

    KeyedRowVectorXd<const char*> vec(eigVec, { "1", "2", "3" });

    vec.addCol("4");
    REQUIRE(vec.colKeys() == std::vector{ "1", "2", "3", "4" });
    REQUIRE(vec.hasCol("2"));
    REQUIRE(vec.hasCols({ "2", "3" }));
    REQUIRE(vec(all) == (Eigen::RowVectorXd(4) << 1, 2, 3, 0).finished());

    vec.addCols({ "5", "6", "7", "8" });
    REQUIRE(vec.colKeys() == std::vector{ "1", "2", "3", "4", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::RowVectorXd(8) << 1, 2, 3, 0, 0, 0, 0, 0).finished());

    vec(all) = (Eigen::RowVectorXd(8) << 1, 2, 3, 4, 5, 6, 7, 8).finished();
    REQUIRE(vec(all) == (Eigen::RowVectorXd(8) << 1, 2, 3, 4, 5, 6, 7, 8).finished());

    vec.removeCol("2");
    REQUIRE(vec.colKeys() == std::vector{ "1", "3", "4", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::RowVectorXd(7) << 1, 3, 4, 5, 6, 7, 8).finished());

    vec.removeCols({ "3", "4" });
    REQUIRE(vec.colKeys() == std::vector{ "1", "5", "6", "7", "8" });
    REQUIRE(vec(all) == (Eigen::RowVectorXd(5) << 1, 5, 6, 7, 8).finished());
}

TEST_CASE("[KeyedMatrix] Type aliases", "[KeyedMatrix]")
{
    auto logger = initializeTestLogger();
    {
        Eigen::MatrixXd eigMat = (Eigen::MatrixXd(2, 2) << 1, 2, 3, 4).finished();
        KeyedMatrixXd<const char*, int> mat(eigMat, { "1", "2" }, { 1, 2 });
        REQUIRE(mat(all, all) == eigMat);
        mat.addCol(3);
        REQUIRE(mat.colKeys() == std::vector{ 1, 2, 3 });
    }
    {
        Eigen::Matrix2d eigMat = (Eigen::Matrix2d() << 1, 2, 3, 4).finished();
        KeyedMatrix2d<const char*, int> mat(eigMat, { "1", "2" }, { 1, 2 });
        REQUIRE(mat(all, all) == eigMat);
    }
    {
        Eigen::Matrix3d eigMat = (Eigen::Matrix3d() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished();
        KeyedMatrix3d<const char*, int> mat(eigMat, { "1", "2", "3" }, { 1, 2, 3 });
        REQUIRE(mat(all, all) == eigMat);
    }
    {
        Eigen::Matrix4d eigMat = (Eigen::Matrix4d() << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16).finished();
        KeyedMatrix4d<const char*, int> mat(eigMat, { "1", "2", "3", "4" }, { 1, 2, 3, 4 });
        REQUIRE(mat(all, all) == eigMat);
    }
}

} // namespace NAV::TESTS