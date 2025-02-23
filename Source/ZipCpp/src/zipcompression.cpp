#include "ZipCpp/zipcompression.h"

#include <zip.h>

namespace {
    constexpr std::uint32_t defaultCompressionLevel = 6;
}
ZipCpp::ZipCompression::ZipCompression() noexcept
    : algorithm_(ZipCompression::Algorithm::Deflate), level_(defaultCompressionLevel)
{}

ZipCpp::ZipCompression::ZipCompression(const Algorithm algorithm, const std::uint32_t level) noexcept
    : algorithm_(algorithm), level_(level)
{}

ZipCpp::ZipCompression::Algorithm ZipCpp::ZipCompression::getAlgorithm() const
{
    return algorithm_;
}

std::uint32_t ZipCpp::ZipCompression::getLevel() const
{
    return level_;
}

int ZipCpp::ZipCompression::toLibzipAlgorithm() const
{
    switch (algorithm_)
    {
        case Algorithm::Store:
            return ZIP_CM_STORE;
        case Algorithm::Deflate:
            return ZIP_CM_DEFLATE;
        case Algorithm::Bzip2:
            return ZIP_CM_BZIP2;
        case Algorithm::Xz:
            return ZIP_CM_XZ;
        case Algorithm::Zstd:
            return ZIP_CM_ZSTD;
        default:
            return ZIP_CM_STORE; // Default to no compression
    }
}
