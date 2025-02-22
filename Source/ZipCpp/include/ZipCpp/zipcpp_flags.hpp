#pragma once

#include <cstdint>
#include <fmt/format.h>
#include <bit>


namespace ZipCpp {

    enum class LibZipOpen : std::uint8_t {
        CREATE = 1U << 0U,    // 1
        EXCL = 1U << 1U,      // 2
        CHECKCONS = 1U << 2U, // 4
        TRUNCATE = 1U << 3U,  // 8
        RDONLY = 1U << 4U     // 16
    };


    inline auto format_as(LibZipOpen s)
    {
        return fmt::underlying(s);
    }

    constexpr std::uint8_t to_integer(LibZipOpen flag)
    {
        return std::bit_cast<std::uint8_t>(flag);
    }

    constexpr LibZipOpen operator|(LibZipOpen lhs, LibZipOpen rhs)
    {
        return static_cast<LibZipOpen>(to_integer(lhs) | to_integer(rhs));
    }

    constexpr bool has_flag(LibZipOpen value, LibZipOpen flag)
    {
        return (to_integer(value) & to_integer(flag)) == to_integer(flag);
    }

    constexpr bool is_valid_flag(std::uint8_t value)
    {
        constexpr std::uint8_t valid_flags_mask =
                to_integer(LibZipOpen::CREATE | LibZipOpen::EXCL | LibZipOpen::CHECKCONS | LibZipOpen::TRUNCATE |
                           LibZipOpen::RDONLY);
        return (value & static_cast<std::uint8_t>(~valid_flags_mask)) == 0;
    }
}
