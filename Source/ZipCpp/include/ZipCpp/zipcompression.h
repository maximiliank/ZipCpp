#pragma once

#include <cstdint>
namespace ZipCpp {

    class ZipCompression {
      public:
        enum class Algorithm : std::uint8_t {
            Store,   // No compression (ZIP_CM_STORE)
            Deflate, // Standard ZIP compression (ZIP_CM_DEFLATE)
            Bzip2,   // Bzip2 compression (ZIP_CM_BZIP2)
            Xz,      // XZ compression (ZIP_CM_XZ)
            Zstd     // Zstd compression (ZIP_CM_ZSTD)
        };

        explicit ZipCompression() noexcept;

        explicit ZipCompression(Algorithm algorithm, std::uint32_t level) noexcept;

        [[nodiscard]] Algorithm getAlgorithm() const;
        [[nodiscard]] std::uint32_t getLevel() const;

        [[nodiscard]] int toLibzipAlgorithm() const;

      private:
        Algorithm algorithm_;
        std::uint32_t level_;
    };


}