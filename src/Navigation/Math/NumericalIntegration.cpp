#include "NumericalIntegration.hpp"

namespace NAV
{

const char* to_string(IntegrationAlgorithm algorithm)
{
    switch (algorithm)
    {
    case IntegrationAlgorithm::RectangularRule:
        return "Rectangular Rule";
    case IntegrationAlgorithm::Simpson:
        return "Simpson";
    case IntegrationAlgorithm::RungeKutta1:
        return "Runge Kutta 1st Order";
    case IntegrationAlgorithm::RungeKutta3:
        return "Runge Kutta 3rd Order";
    case IntegrationAlgorithm::COUNT:
        return "";
    }
    return "";
}

} // namespace NAV
