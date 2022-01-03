/// @file UlogFileFormat.hpp
/// @brief List of all ulog message types
/// @author M. Maier (marcel.maier@ins.uni-stuttgart.de)
/// @date 2022-01-03
/// @note See PX4 User Guide - ULog File Format (https://docs.px4.io/master/en/dev_log/ulog_file_format.html)

#include <array>
#include <cstdint>

// --------------------------------------------------------- Definitions Section ---------------------------------------------------
struct message_header_s
{
    uint16_t msg_size{ 0 };
    uint8_t msg_type{ 'B' }; //TODO: Validate default value
};

struct ulog_message_flag_bits_s
{
    struct message_header_s;
    std::array<uint8_t, 8> compat_flags;
    std::array<uint8_t, 8> incompat_flags;
    std::array<uint64_t, 3> appended_offsets;
};

struct message_format_s
{
    struct message_header_s header;
    char format = static_cast<char>(header.msg_size);
};

struct message_info_s
{
    struct message_header_s header;
    uint8_t key_len{ 0 };
    char key = static_cast<char>(key_len);
};

struct ulog_message_info_multiple_header_s
{
    struct message_header_s header;
    uint8_t is_continued{ 0 }; ///< can be used for arrays
    uint8_t key_len{ 0 };
    char key = static_cast<char>(key_len);
    char value = static_cast<char>(header.msg_size - 2 - key_len);
    // char value[header.msg_size - 2 - key_len]
};

struct ulog_message_parameter_default_header_s
{
    struct message_header_s header;
    uint8_t default_types{ 'B' }; //TODO: Validate default value
    uint8_t key_len{ 0 };
    char key = static_cast<char>(key_len);
    char value = static_cast<char>(header.msg_size - 2 - key_len);
};

// ------------------------------------------------------------ Data Section --------------------------------------------------------

struct message_add_logged_s
{
    struct message_header_s header;
    uint8_t multi_id{ 0 };
    uint16_t msg_id{ 0 };
    char message_name = static_cast<char>(header.msg_size - 3);
};

struct message_remove_logged_s
{
    struct message_header_s header;
    uint16_t msg_id{ 0 };
};

struct message_data_s
{
    struct message_header_s header;
    uint16_t msg_id{ 0 };
    uint8_t data = static_cast<uint8_t>(header.msg_size - 2);
};

struct message_logging_s
{
    struct message_header_s header;
    uint8_t log_level{ 0 };
    uint64_t timestamp{ 0 };
    char message = static_cast<char>(header.msg_size - 9);
};

struct message_logging_tagged_s
{
    struct message_header_s header;
    uint8_t log_level{ 0 };
    uint16_t tag{ 'C' }; //TODO: Validate default value
    uint64_t timestamp{ 0 };
    char message = static_cast<char>(header.msg_size - 9);
};

enum class ulog_tag : uint16_t
{
    unassigned,
    mavlink_handler,
    ppk_handler,
    camera_handler,
    ptp_handler,
    serial_handler,
    watchdog,
    io_service,
    cbuf,
    ulg
};

struct message_sync_s
{
    struct message_header_s header;
    std::array<uint8_t, 8> snyc_magic{};
};

struct message_dropout_s
{
    struct message_header_s header;
    uint16_t duration{ 0 };
};
