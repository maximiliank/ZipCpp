#pragma once

#include <vector>
#include <cstddef>
#include <string_view>
#include "memorystream.h"

namespace ZipCpp {
    class MemoryBuffer {
      public:
        using Data_t = std::vector<std::byte>;

        explicit MemoryBuffer(std::size_t size);

        explicit MemoryBuffer(Data_t&& data);

        MemoryBuffer(const MemoryBuffer& other);

        MemoryBuffer& operator=(const MemoryBuffer& other) = delete;

        MemoryBuffer(MemoryBuffer&& other) noexcept;

        MemoryBuffer& operator=(MemoryBuffer&& other) = delete;

        ~MemoryBuffer();

        [[nodiscard]] MemoryStream getMemoryStream() const;

        [[nodiscard]] Data_t& getData();

        [[nodiscard]] const Data_t& getData() const;

      protected:
        Data_t data_;
        std::string_view characterView_;
    };
}
