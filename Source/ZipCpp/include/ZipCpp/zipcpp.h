#pragma once

#include <string>
#include <variant>
#include <deque>
#include <filesystem>
#include <functional>
#include "memorybuffer.h"
#include "zipcpp_flags.hpp"
#include "zipcompression.h"

struct zip;
struct zip_source;

namespace ZipCpp {
    class ZipArchive {
        using DataStorage_t = std::variant<std::string, MemoryBuffer>;

        LibZipOpen zipFlags_;

        zip* zip_ = nullptr;

        std::deque<DataStorage_t> addedFiles_;

        explicit ZipArchive(zip* za, LibZipOpen flags);

      public:
        static ZipArchive open(const std::string& archiveName, LibZipOpen flags);

        static ZipArchive open(const void* stream, std::size_t size, LibZipOpen flags);

        ~ZipArchive();

        ZipArchive(const ZipArchive&) = delete;
        ZipArchive& operator=(const ZipArchive&) = delete;

        ZipArchive(ZipArchive&& other) noexcept;
        ZipArchive& operator=(ZipArchive&& other) noexcept;

        [[nodiscard]] MemoryBuffer readFile(const std::string& fileName);

        [[nodiscard]] void* readFile(
                const std::string& fileName, const std::function<void*(std::size_t)>& createBuffer);

        template<typename F>
        auto load(const char* zipFilename, F func) -> std::invoke_result_t<F, std::istream&>
        {
            auto data = readFile(zipFilename);
            auto stream = data.getMemoryStream();
            return func(stream);
        }

        void add(const std::string& name, const std::filesystem::path& filePath, const ZipCompression& compression);

        void add(const std::string& name, const std::string& content, const ZipCompression& compression);

        void add(const std::string& name, std::string&& content, const ZipCompression& compression);

        void add(const std::string& name, const MemoryBuffer& data, const ZipCompression& compression);

        void add(const std::string& name, MemoryBuffer&& data, const ZipCompression& compression);

        void add(const std::string& name, const char* stream, std::size_t size, const ZipCompression& compression);

        void add(const std::string& name, const unsigned char* stream, std::size_t size,
                const ZipCompression& compression);

        void addDirectory(const std::string& name, int flags = 0);

        [[nodiscard]] bool hasFile(const std::string& filename) const;

        [[nodiscard]] LibZipOpen getCurrentFlags() const;

        void writeAndClose();

      private:
        std::string_view getErrorMessage();

        void addToZipArchive(zip_source* source, const char* fileName, int flags, const ZipCompression& compression);

        [[nodiscard]] bool isReadOnly() const;
    };
}
