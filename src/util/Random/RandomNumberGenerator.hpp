// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file RandomNumberGenerator.hpp
/// @brief Random Number Generator
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2023-08-24

#pragma once

#include <random>
#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json; ///< json namespace

#include "SHA256.hpp"

namespace NAV
{

/// @brief Manages a thread which calls a specified function at a specified interval
class RandomNumberGenerator
{
  public:
    /// @brief Default constructor
    explicit RandomNumberGenerator(bool useSeed = true) : useSeed(useSeed) {} // NOLINT(cert-msc32-c,cert-msc51-cpp)
    /// @brief Copy constructor
    RandomNumberGenerator(const RandomNumberGenerator&) = delete;
    /// @brief Move constructor
    RandomNumberGenerator(RandomNumberGenerator&&) = delete;
    /// @brief Copy assignment operator
    RandomNumberGenerator& operator=(const RandomNumberGenerator&) = delete;
    /// @brief Move assignment operator
    RandomNumberGenerator& operator=(RandomNumberGenerator&&) = delete;
    /// @brief Destructor
    ~RandomNumberGenerator() = default;

    /// @brief Reset the seed to the internal seed or the system time
    /// @param id Some id used to make a unique hash when using the system time to set the seed
    void resetSeed(size_t id = 0)
    {
        uint64_t seed = useSeed ? this->seed : static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        auto hash = hashSeed(std::to_string(seed) + (useSeed || id == 0 ? "" : " " + std::to_string(id)));
        std::seed_seq seed_seq(hash.begin(), hash.end());
        _generator.seed(seed_seq);
    }

    /// @brief Reset the seed to the specified seed, but do not update the internal seed
    /// @param[in] userSeed Seed to use once
    void resetSeedOnce(uint64_t userSeed)
    {
        auto hash = hashSeed(std::to_string(userSeed));
        std::seed_seq seed_seq(hash.begin(), hash.end());
        _generator.seed(seed_seq);
    }

    /// @brief Gets a random integer number from an uniform distribution
    /// @tparam IntType The result type generated by the generator. The effect is undefined if this is not one of short, int, long, long long, unsigned short, unsigned int, unsigned long, or unsigned long long.
    /// @param min Minimum value
    /// @param max Maximum value
    /// @return Random number
    template<typename IntType = int>
    double getRand_uniformIntDist(IntType min = 0, IntType max = std::numeric_limits<IntType>::max())
    {
        return std::uniform_int_distribution<IntType>(min, max)(_generator);
    }

    /// @brief Gets a random real number from an uniform distribution
    /// @tparam RealType The result type generated by the generator. The effect is undefined if this is not one of float, double, or long double
    /// @param min Minimum value
    /// @param max Maximum value
    /// @return Random number
    template<typename RealType = double>
    double getRand_uniformRealDist(RealType min = 0.0, RealType max = 1.0)
    {
        return std::uniform_real_distribution<RealType>(min, max)(_generator);
    }

    /// @brief Gets a random number from a normal distribution
    /// @tparam RealType The result type generated by the generator. The effect is undefined if this is not one of float, double, or long double
    /// @param mean The μ distribution parameter (mean)
    /// @param stddev The σ distribution parameter (standard deviation)
    /// @return Random number
    template<typename RealType = double>
    double getRand_normalDist(RealType mean = 0.0, RealType stddev = 1.0)
    {
        return std::normal_distribution<RealType>(mean, stddev)(_generator);
    }

    bool useSeed = true; ///< Flag whether to use the seed instead of the system time
    uint64_t seed = 0;   ///< Seed for the random number generator

  private:
    /// @brief Hash the given seed
    /// @param seed Seed
    /// @return Hashed seed
    static std::string hashSeed(const std::string& seed)
    {
        SHA256 sha;
        sha.update(seed);
        uint8_t* digest = sha.digest();

        std::string hashed(digest, digest + 32); // NOLINT //SHA256::ToString method is shady... (manipulates data?). Keep Pointer Arithmetic
        delete[] digest;                         // NOLINT

        return hashed;
    }

    std::mt19937_64 _generator; ///< Random number generator
};

/// @brief Write info to a json object
/// @param[out] j Json output
/// @param[in] rng Object to read info from
void to_json(json& j, const RandomNumberGenerator& rng);
/// @brief Read info from a json object
/// @param[in] j Json variable to read info from
/// @param[out] rng Output object
void from_json(const json& j, RandomNumberGenerator& rng);

} // namespace NAV