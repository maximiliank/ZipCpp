#pragma once

#include <streambuf>
#include <istream>
#include <string_view>

namespace ZipCpp {
    // See https://stackoverflow.com/questions/13059091/creating-an-input-stream-from-constant-memory/13059195#13059195
    struct membuf : std::streambuf {
        membuf(std::string_view characterView);

        membuf(const membuf&) = delete;

        membuf(membuf&& other) noexcept;

        membuf operator=(const membuf&) = delete;

        membuf operator=(membuf&&) = delete;

      private:
        std::string_view characterView_;
    };

    struct MemoryStream : virtual membuf, std::istream {
        MemoryStream(std::string_view characterView);

        MemoryStream(const MemoryStream&) = delete;

        MemoryStream(MemoryStream&& other) noexcept;

        MemoryStream operator=(const MemoryStream&) = delete;

        MemoryStream operator=(MemoryStream&&) = delete;

      protected:
        std::iostream::pos_type seekpos(std::iostream::pos_type sp, std::ios_base::openmode which) override;

        std::iostream::pos_type seekoff(
                std::iostream::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    };
}
