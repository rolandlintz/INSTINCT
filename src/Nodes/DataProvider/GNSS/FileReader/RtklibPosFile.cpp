#include "RtklibPosFile.hpp"

#include "util/Logger.hpp"
#include "Navigation/Transformations/CoordinateFrames.hpp"
#include "util/Time/TimeBase.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/GNSS/RtklibPosObs.hpp"

NAV::RtklibPosFile::RtklibPosFile()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);

    _hasConfig = true;
    _guiConfigDefaultWindowSize = { 380, 290 };

    nm::CreateOutputPin(this, "RtklibPosObs", Pin::Type::Flow, { NAV::RtklibPosObs::type() }, &RtklibPosFile::pollData);
    nm::CreateOutputPin(this, "Header Columns", Pin::Type::Object, { "std::vector<std::string>" }, &_headerColumns);
}

NAV::RtklibPosFile::~RtklibPosFile()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::RtklibPosFile::typeStatic()
{
    return "RtklibPosFile";
}

std::string NAV::RtklibPosFile::type() const
{
    return typeStatic();
}

std::string NAV::RtklibPosFile::category()
{
    return "Data Provider";
}

void NAV::RtklibPosFile::guiConfig()
{
    if (FileReader::guiConfig(".pos", { ".pos" }, size_t(id), nameId()))
    {
        flow::ApplyChanges();
        deinitializeNode();
    }

    // Header info
    if (ImGui::BeginTable(fmt::format("##RtklibPos ({})", id.AsPointer()).c_str(), 3,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Basic", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("LLA", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("XYZ", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        auto TextColoredIfExists = [this](int index, const char* displayText, const char* searchText, bool alwaysNormal = false) {
            ImGui::TableSetColumnIndex(index);
            if (alwaysNormal || std::find(_headerColumns.begin(), _headerColumns.end(), searchText) != _headerColumns.end())
            {
                ImGui::TextUnformatted(displayText);
            }
            else
            {
                ImGui::TextDisabled("%s", displayText);
            }
        };

        ImGui::TableNextRow();
        TextColoredIfExists(0, "GpsWeek", "GpsWeek");
        TextColoredIfExists(1, "latitude(deg)", "latitude(deg)");
        TextColoredIfExists(2, "x-ecef(m)", "x-ecef(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(0, "GpsToW", "GpsToW");
        TextColoredIfExists(1, "longitude(deg)", "longitude(deg)");
        TextColoredIfExists(2, "y-ecef(m)", "y-ecef(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(0, "age(s)", "age(s)");
        TextColoredIfExists(1, "height(m)", "height(m)");
        TextColoredIfExists(2, "z-ecef(m)", "z-ecef(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(0, "ratio", "ratio");
        TextColoredIfExists(1, "sdn(m)", "sdn(m)");
        TextColoredIfExists(2, "sdx(m)", "sdx(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(0, "Q", "Q");
        TextColoredIfExists(1, "sde(m)", "sde(m)");
        TextColoredIfExists(2, "sdy(m)", "sdy(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(0, "ns", "ns");
        TextColoredIfExists(1, "sdu(m)", "sdu(m)");
        TextColoredIfExists(2, "sdz(m)", "sdz(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(1, "sdne(m)", "sdne(m)");
        TextColoredIfExists(2, "sdxy(m)", "sdxy(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(1, "sdeu(m)", "sdeu(m)");
        TextColoredIfExists(2, "sdyz(m)", "sdyz(m)");
        ImGui::TableNextRow();
        TextColoredIfExists(1, "sdun(m)", "sdun(m)");
        TextColoredIfExists(2, "sdzx(m)", "sdzx(m)");

        ImGui::EndTable();
    }
}

[[nodiscard]] json NAV::RtklibPosFile::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["FileReader"] = FileReader::save();

    return j;
}

void NAV::RtklibPosFile::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("FileReader"))
    {
        FileReader::restore(j.at("FileReader"));
    }
}

bool NAV::RtklibPosFile::initialize()
{
    LOG_TRACE("{}: called", nameId());

    return FileReader::initialize();
}

void NAV::RtklibPosFile::deinitialize()
{
    LOG_TRACE("{}: called", nameId());

    FileReader::deinitialize();
}

bool NAV::RtklibPosFile::resetNode()
{
    FileReader::resetReader();

    return true;
}

std::shared_ptr<const NAV::NodeData> NAV::RtklibPosFile::pollData(bool peek)
{
    auto obs = std::make_shared<RtklibPosObs>();
    // Get current position
    auto pos = _filestream.tellg();

    // Read line
    std::string line;
    std::getline(_filestream, line);
    // Remove any starting non text characters
    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) { return std::isgraph(ch); }));

    if (line.empty())
    {
        return nullptr;
    }

    std::istringstream lineStream(line);
    std::string cell;

    std::optional<uint16_t> gpsWeek;
    std::optional<long double> gpsToW;
    std::optional<double> positionX;
    std::optional<double> positionY;
    std::optional<double> positionZ;
    std::optional<double> positionLat;
    std::optional<double> positionLon;
    std::optional<double> positionHeight;
    std::optional<double> sdX;
    std::optional<double> sdY;
    std::optional<double> sdZ;
    std::optional<double> sdN;
    std::optional<double> sdE;
    std::optional<double> sdU;

    for (const auto& column : _headerColumns)
    {
        if (lineStream >> cell)
        {
            // Remove any trailing non text characters
            cell.erase(std::find_if(cell.begin(), cell.end(), [](int ch) { return std::iscntrl(ch); }), cell.end());
            if (cell.empty())
            {
                continue;
            }

            if (column == "GpsWeek")
            {
                gpsWeek = static_cast<uint16_t>(std::stoul(cell));
            }
            else if (column == "GpsToW")
            {
                gpsToW = std::stold(cell);
            }
            else if (column == "x-ecef(m)")
            {
                positionX = std::stod(cell);
            }
            else if (column == "y-ecef(m)")
            {
                positionY = std::stod(cell);
            }
            else if (column == "z-ecef(m)")
            {
                positionZ = std::stod(cell);
            }
            else if (column == "latitude(deg)")
            {
                positionLat = trafo::deg2rad(std::stod(cell));
            }
            else if (column == "longitude(deg)")
            {
                positionLon = trafo::deg2rad(std::stod(cell));
            }
            else if (column == "height(m)")
            {
                positionHeight = std::stod(cell);
            }
            else if (column == "Q")
            {
                obs->Q = static_cast<uint8_t>(std::stoul(cell));
            }
            else if (column == "ns")
            {
                obs->ns = static_cast<uint8_t>(std::stoul(cell));
            }
            else if (column == "sdx(m)")
            {
                sdX = std::stod(cell);
            }
            else if (column == "sdy(m)")
            {
                sdY = std::stod(cell);
            }
            else if (column == "sdz(m)")
            {
                sdZ = std::stod(cell);
            }
            else if (column == "sdn(m)")
            {
                sdN = std::stod(cell);
            }
            else if (column == "sde(m)")
            {
                sdE = std::stod(cell);
            }
            else if (column == "sdu(m)")
            {
                sdU = std::stod(cell);
            }
            else if (column == "sdxy(m)")
            {
                obs->sdxy = std::stod(cell);
            }
            else if (column == "sdyz(m)")
            {
                obs->sdyz = std::stod(cell);
            }
            else if (column == "sdzx(m)")
            {
                obs->sdzx = std::stod(cell);
            }
            else if (column == "sdne(m)")
            {
                obs->sdne = std::stod(cell);
            }
            else if (column == "sdeu(m)")
            {
                obs->sdeu = std::stod(cell);
            }
            else if (column == "sdun(m)")
            {
                obs->sdun = std::stod(cell);
            }
            else if (column == "age(s)")
            {
                obs->age = std::stod(cell);
            }
            else if (column == "ratio")
            {
                obs->ratio = std::stod(cell);
            }
        }
    }

    if (gpsWeek.has_value() && gpsToW.has_value())
    {
        obs->insTime.emplace(0, gpsWeek.value(), gpsToW.value());
    }
    if (positionX.has_value() && positionY.has_value() && positionZ.has_value())
    {
        obs->e_position.emplace(positionX.value(), positionY.value(), positionZ.value());
    }
    if (positionLat.has_value() && positionLon.has_value() && positionHeight.has_value())
    {
        if (!obs->e_position.has_value())
        {
            obs->e_position.emplace(trafo::lla2ecef_WGS84({ positionLat.value(), positionLon.value(), positionHeight.value() }));
        }
    }
    if (sdX.has_value() && sdY.has_value() && sdZ.has_value())
    {
        obs->sdXYZ.emplace(sdX.value(), sdY.value(), sdZ.value());
    }
    if (sdN.has_value() && sdE.has_value() && sdU.has_value())
    {
        obs->sdNEU.emplace(sdN.value(), sdE.value(), sdU.value());
    }

    if (obs->insTime.has_value())
    {
        if (util::time::GetMode() == util::time::Mode::REAL_TIME)
        {
            util::time::SetCurrentTime(obs->insTime.value());
        }
    }
    else if (auto currentTime = util::time::GetCurrentInsTime();
             !currentTime.empty())
    {
        obs->insTime = currentTime;
    }

    if (peek)
    {
        // Return to position before "Read line".
        _filestream.seekg(pos, std::ios_base::beg);
    }

    // Calls all the callbacks
    if (!peek)
    {
        invokeCallbacks(OUTPUT_PORT_INDEX_RTKLIB_POS_OBS, obs);
    }

    return obs;
}

NAV::FileReader::FileType NAV::RtklibPosFile::determineFileType()
{
    return FileReader::FileType::CSV;
}

void NAV::RtklibPosFile::readHeader()
{
    // Read header line
    std::string line;
    do
    {
        std::getline(_filestream, line);
        // Remove any starting non text characters
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](int ch) { return std::isgraph(ch); }));
    } while (!line.empty() && line.find("%  ") == std::string::npos);

    // Convert line into stream
    std::istringstream lineStream(line);

    for (std::string cell; lineStream >> cell;)
    {
        if (cell != "%")
        {
            if (cell == "GPST")
            {
                _headerColumns.emplace_back("GpsWeek");
                _headerColumns.emplace_back("GpsToW");
            }
            else
            {
                _headerColumns.push_back(cell);
            }
        }
    }
}