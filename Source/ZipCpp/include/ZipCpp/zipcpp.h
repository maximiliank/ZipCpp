#pragma once

#include <string>
#include <variant>
#include <deque>
#include <filesystem>
#include <functional>
#include "memorybuffer.h"
#include "zipcpp_flags.hpp"

struct zip;
struct zip_source;

namespace ZipCpp {
    class ZipArchive {
        using DataStorage_t = std::variant<std::string, MemoryBuffer>;

        LibZipOpen zipFlags_;

        struct zip* zip_ = nullptr;

        std::deque<DataStorage_t> addedFiles_;

        explicit ZipArchive(zip_source* src, LibZipOpen flags);

      public:
        static ZipArchive open(const std::string& archiveName, LibZipOpen flags);

        static ZipArchive open(const void* stream, std::size_t size, LibZipOpen flags);

        ~ZipArchive();

        ZipArchive(const ZipArchive&) = delete;
        ZipArchive& operator=(const ZipArchive&) = delete;

        ZipArchive(ZipArchive&& other) noexcept;
        ZipArchive& operator=(ZipArchive&& other) noexcept;

        [[nodiscard]] MemoryBuffer readFile(const std::string& fileName);

        [[nodiscard]] void* readFile(const std::string& fileName, std::function<void*(std::size_t)> createBuffer);

        template<typename F>
        auto load(const char* zipFilename, F func) -> std::invoke_result_t<F, std::istream&>
        {
            auto data = readFile(zipFilename);
            auto stream = data.getMemoryStream();
            return func(stream);
        }

        void add(const std::string& name, const std::filesystem::path& filePath);

        void add(const std::string& name, const std::string& content);

        void add(const std::string& name, std::string&& content);

        void add(const std::string& name, const MemoryBuffer& data);

        void add(const std::string& name, MemoryBuffer&& data);

        void add(const std::string& name, const char* stream, std::size_t size);

        void add(const std::string& name, const unsigned char* stream, std::size_t size);

        void addDirectory(const std::string& name, int flags = 0);

        [[nodiscard]] bool hasFile(const std::string& filename) const;

        [[nodiscard]] LibZipOpen getCurrentFlags() const;

      private:
        std::string_view getErrorMessage();

        void add(zip_source* source, const char* fileName, int flags);

        void close();

        [[nodiscard]] bool isReadOnly() const;
    };
}
