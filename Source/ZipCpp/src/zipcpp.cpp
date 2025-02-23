#include "ZipCpp/zipcpp.h"

#include <stdexcept>
#include "zip.h"
#include <fmt/format.h>
#include <string_view>
#include <zipconf.h>

namespace ZipCpp {
    namespace {
        zip_int64_t getFileIndex(zip_t* archive, const char* fileName, zip_flags_t flags)
        {
            return zip_name_locate(archive, fileName, flags);
        }
        zip_uint64_t locate_name(zip_t* archive, const char* fileName, zip_flags_t flags)
        {
            auto index = getFileIndex(archive, fileName, flags);
            if (index >= 0)
            {
                return static_cast<zip_uint64_t>(index);
            }
            else
            {
                throw std::runtime_error(fmt::format("Could not open entry {} in zip archive", fileName));
            }
        }
        std::string_view getErrorMessage(zip_t* archive)
        {
            return {zip_error_strerror(zip_get_error(archive))};
        }
        template<typename F>
        auto readFileIntoBuffer(zip_t* archive, const std::string& fileName, const F& resizeBuffer)
        {
            const auto index = locate_name(archive, fileName.c_str(), 0);

            auto* fileInArchive = [archive, index]() {
                if (auto* file = zip_fopen_index(archive, index, 0); file != nullptr)
                {
                    return file;
                }
                else
                {
                    throw std::runtime_error(
                            fmt::format("ZipArchive::readFile: Cannot zip_fopen: {}", getErrorMessage(archive)));
                }
            }();

            const auto bufferSize = [archive, index]() {
                zip_stat_t zst;
                if (zip_stat_index(archive, index, 0, &zst) != ZIP_ER_OK)
                {
                    throw std::runtime_error(fmt::format("ZipArchive::readFile: {}", getErrorMessage(archive)));
                }
                return zst.size;
            }();


            auto* buffer = resizeBuffer(bufferSize);
            const auto bytesRead = zip_fread(fileInArchive, buffer, bufferSize);
            if (bytesRead < 0)
            {
                throw std::runtime_error(
                        fmt::format("ZipArchive::readFile: Cannot read from file: {}", getErrorMessage(archive)));
            }


            if (const int error = zip_fclose(fileInArchive); error != 0)
            {
                throw std::runtime_error(fmt::format("ZipArchive::readFile: Cannot close file: {}", error));
            }

            return buffer;
        }

        struct ZipError {
            zip_error_t error_{};

            ZipError()
            {
                zip_error_init(&error_);
            }

            ~ZipError()
            {
                zip_error_fini(&error_);
            }

            ZipError(const ZipError&) = delete;
            ZipError& operator=(const ZipError&) = delete;

            ZipError(ZipError&& other) noexcept = delete;
            ZipError& operator=(ZipError&& other) noexcept = delete;

            zip_error_t* operator()()
            {
                return &error_;
            }

            std::string_view get()
            {
                return {zip_error_strerror(&error_)};
            }
        };

        zip_source_t* createSourceBuffer(const void* stream, const std::size_t size)
        {
            ZipError error;
            if (auto* src = zip_source_buffer_create(stream, size, 0, error()); src != nullptr)
            {
                return src;
            }
            else
            {
                throw std::runtime_error(
                        fmt::format("ZipCpp::createSourceBuffer: Error during "
                                    "zip_source_buffer_create call: {}",
                                error.get()));
            }
        }

        zip_source_t* createSourceBuffer(const std::string& content)
        {
            return createSourceBuffer(content.c_str(), content.size());
        }

        zip_source_t* createSourceBuffer(const MemoryBuffer::Data_t& data)
        {
            return createSourceBuffer(data.data(), data.size());
        }
    }
}


ZipCpp::ZipArchive::ZipArchive(zip_source* src, LibZipOpen flags)
    : zipFlags_(flags), zip_([src, flags = to_integer(flags)]() {
          if (!is_valid_flag(flags))
          {
              throw std::runtime_error(fmt::format("Unsupported flag for zip_open {}", flags));
          }
          ZipError error;
          if (auto* za = zip_open_from_source(src, flags, error()); za != nullptr)
          {
              return za;
          }
          else
          {
              zip_source_free(src);
              throw std::runtime_error(
                      fmt::format("ZipArchive::ZipArchive zip_open_from_source failed: {}", error.get()));
          }
      }())
{}
bool ZipCpp::ZipArchive::isReadOnly() const
{
    return has_flag(zipFlags_, LibZipOpen::RDONLY);
}
ZipCpp::MemoryBuffer ZipCpp::ZipArchive::readFile(const std::string& fileName)
{
    MemoryBuffer::Data_t buffer;

    ::ZipCpp::readFileIntoBuffer(zip_, fileName, [&buffer](const std::size_t bufferSize) {
        buffer.resize(bufferSize);
        return buffer.data();
    });

    return MemoryBuffer(std::move(buffer));
}

void* ZipCpp::ZipArchive::readFile(const std::string& fileName, const std::function<void*(std::size_t)>& createBuffer)
{
    return ::ZipCpp::readFileIntoBuffer(zip_, fileName, createBuffer);
}

ZipCpp::ZipArchive::~ZipArchive()
{
    close();
}
ZipCpp::ZipArchive::ZipArchive(ZipArchive&& other) noexcept
    : zipFlags_(other.zipFlags_), zip_(other.zip_), addedFiles_(std::move(other.addedFiles_))
{
    other.zip_ = nullptr;
}

ZipCpp::ZipArchive& ZipCpp::ZipArchive::operator=(ZipArchive&& other) noexcept
{
    if (this != &other)
    {
        close();

        zipFlags_ = other.zipFlags_;
        zip_ = other.zip_;
        addedFiles_ = std::move(other.addedFiles_);

        other.zip_ = nullptr;
    }
    return *this;
}

void ZipCpp::ZipArchive::close()
{
    if (zip_ != nullptr)
    {
        if (isReadOnly())
        {
            zip_discard(zip_);
        }
        else
        {
            zip_close(zip_);
        }
    }
}
ZipCpp::ZipArchive ZipCpp::ZipArchive::open(const std::string& archiveName, const LibZipOpen flags)
{
    ZipError error;
    if (auto* src = zip_source_file_create(archiveName.c_str(), 0, -1, error()); src != nullptr)
    {
        return ZipCpp::ZipArchive(src, flags);
    }
    else
    {
        throw std::runtime_error(
                fmt::format("ZipCpp::load: Error during zip_source_file_create call: {}", error.get()));
    }
}

ZipCpp::ZipArchive ZipCpp::ZipArchive::open(const void* stream, const std::size_t size, const LibZipOpen flags)
{
    return ZipCpp::ZipArchive(createSourceBuffer(stream, size), flags);
}

std::string_view ZipCpp::ZipArchive::getErrorMessage()
{
    return ::ZipCpp::getErrorMessage(zip_);
}

void ZipCpp::ZipArchive::add(const std::string& name, const std::string& content, const ZipCompression& compression)
{
    auto* source = createSourceBuffer(content);
    addToZipArchive(source, name.c_str(), 0, compression);
}

void ZipCpp::ZipArchive::add(
        const std::string& name, const ZipCpp::MemoryBuffer& data, const ZipCompression& compression)
{
    auto* source = createSourceBuffer(data.getData());
    addToZipArchive(source, name.c_str(), 0, compression);
}

void ZipCpp::ZipArchive::add(
        const std::string& name, const char* stream, const std::size_t size, const ZipCompression& compression)
{
    auto* source = createSourceBuffer(stream, size);
    addToZipArchive(source, name.c_str(), 0, compression);
}

void ZipCpp::ZipArchive::add(
        const std::string& name, const unsigned char* stream, const std::size_t size, const ZipCompression& compression)
{
    auto* source = createSourceBuffer(stream, size);
    addToZipArchive(source, name.c_str(), 0, compression);
}

void ZipCpp::ZipArchive::addToZipArchive(
        zip_source* source, const char* name, const int flags, const ZipCompression& compression)
{
    auto index = getFileIndex(zip_, name, 0);
    if (index >= 0)
    {
        const int status = zip_delete(zip_, static_cast<zip_uint64_t>(index));
        if (status < 0)
        {
            throw std::runtime_error(
                    fmt::format("ZipCpp::ZipArchive::add failed to delete file {}: {}", name, getErrorMessage()));
        }
    }

    index = zip_file_add(zip_, name, source, static_cast<zip_flags_t>(flags));
    if (index < 0)
    {
        throw std::runtime_error(fmt::format("ZipCpp::ZipArchive::add failed: {}", getErrorMessage()));
    }

    const auto statusCompression = zip_set_file_compression(
            zip_, static_cast<zip_uint64_t>(index), compression.toLibzipAlgorithm(), compression.getLevel());
    if (statusCompression < 0)
    {
        throw std::runtime_error(fmt::format(
                "ZipCpp::ZipArchive::add failed to set compression for file {}: {}", name, getErrorMessage()));
    }
}

void ZipCpp::ZipArchive::addDirectory(const std::string& name, const int flags)
{
    const auto index = zip_dir_add(zip_, name.c_str(), static_cast<zip_flags_t>(flags));
    if (index < 0)
    {
        throw std::runtime_error(fmt::format("ZipCpp::ZipArchive::addDirectory failed: {}", getErrorMessage()));
    }
}

void ZipCpp::ZipArchive::add(const std::string& name, std::string&& content, const ZipCompression& compression)
{
    const auto& contentVariant = addedFiles_.emplace_back(std::move(content));
    const auto& contentRef = std::get<std::string>(contentVariant);
    add(name, contentRef, compression);
}

void ZipCpp::ZipArchive::add(const std::string& name, MemoryBuffer&& data, const ZipCompression& compression)
{
    const auto& dataVariant = addedFiles_.emplace_back(std::move(data));
    const auto& dataRef = std::get<MemoryBuffer>(dataVariant);
    add(name, dataRef, compression);
}

void ZipCpp::ZipArchive::add(
        const std::string& name, const std::filesystem::path& filePath, const ZipCompression& compression)
{
    ZipError error;
    if (auto* src = zip_source_file_create(filePath.c_str(), 0, -1, error()); src != nullptr)
    {
        addToZipArchive(src, name.c_str(), 0, compression);
    }
    else
    {
        throw std::runtime_error(
                fmt::format("ZipCpp::ZipArchive::addFile: Error during zip_source_file_create call: {}", error.get()));
    }
}

bool ZipCpp::ZipArchive::hasFile(const std::string& filename) const
{
    auto index = zip_name_locate(zip_, filename.c_str(), 0);
    return index >= 0;
}

ZipCpp::LibZipOpen ZipCpp::ZipArchive::getCurrentFlags() const
{
    return zipFlags_;
}