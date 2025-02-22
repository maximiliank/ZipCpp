#include <ZipCpp/zipcpp_flags.hpp>
#include <gtest/gtest.h>
#include <string>
#include "ZipCpp/zipcpp.h"
#include "utils.h"
#include "get_file_path.hpp"

namespace {
    constexpr auto zipToStringLoader()
    {
        return [](std::istream& stream) { return std::string{std::istreambuf_iterator<char>(stream), {}}; };
    }

    TEST(ZipCpp, readfail1)
    {
        EXPECT_THROW(
                ZipCpp::ZipArchive::open("file_does_not_exists.zip", ZipCpp::LibZipOpen::RDONLY), std::runtime_error);
    }

    TEST(ZipCpp, readfail2)
    {
        auto za = ZipCpp::ZipArchive::open(TestData::getFilePath("test.zip"), ZipCpp::LibZipOpen::RDONLY);
        EXPECT_THROW((void) za.readFile("file_does_not_exists.txt"), std::runtime_error);
    }

    TEST(ZipCpp, read1)
    {
        auto za = ZipCpp::ZipArchive::open(TestData::getFilePath("test.zip"), ZipCpp::LibZipOpen::RDONLY);
        auto loaded = za.load("get_file_path.hpp.in", zipToStringLoader());
        const std::string expected = Tests::loadFileAsString(TestData::getFilePath("get_file_path.hpp.in"));
        ASSERT_EQ(loaded, expected);
    }

    TEST(ZipCpp, readSeek1)
    {
        auto za = ZipCpp::ZipArchive::open(TestData::getFilePath("test.zip"), ZipCpp::LibZipOpen::RDONLY);
        const auto data = za.readFile("get_file_path.hpp.in");
        auto stream = data.getMemoryStream();
        stream.seekg(8, std::ios::beg);
        std::string loaded(4, ' ');
        stream.read(loaded.data(), 4);
        ASSERT_EQ(loaded, "once");
    }

    TEST(ZipCpp, readStream1)
    {
        auto buffer = Tests::readFile(TestData::getFilePath("test.zip"));
        const auto& data = buffer.getData();
        auto za = ZipCpp::ZipArchive::open(data.data(), data.size(), ZipCpp::LibZipOpen::RDONLY);
        auto loaded = za.load("get_file_path.hpp.in", zipToStringLoader());
        const std::string expected = Tests::loadFileAsString(TestData::getFilePath("get_file_path.hpp.in"));
        ASSERT_EQ(loaded, expected);
    }

    TEST(ZipCpp, memoryBuffer)
    {
        auto buffer = Tests::readFile(TestData::getFilePath("test.zip"));
        const auto data = buffer.getData();
        auto bufferMoved = std::move(buffer);
        const auto& dataMoved = bufferMoved.getData();
        ASSERT_EQ(data, dataMoved);
    }
}
