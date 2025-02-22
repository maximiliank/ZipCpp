#include <gtest/gtest.h>
#include <string>
#include "ZipCpp/zipcpp.h"
#include "utils.h"
#include "get_file_path.hpp"

namespace {
    TEST(ZipCpp, write1)
    {
        std::string content{"This is a test"};
        {
            using namespace ZipCpp;
            auto za = ZipArchive::open("test_write1.zip", LibZipOpen::CREATE | LibZipOpen::TRUNCATE);
            za.add("dir/test.txt", content);
            za.addDirectory("dirB");
        }
    }

    TEST(ZipCpp, write2)
    {
        using namespace ZipCpp;
        auto za = ZipArchive::open("test_write2.zip", LibZipOpen::CREATE | LibZipOpen::TRUNCATE);
        {
            std::string content{"This is a test"};
            za.add("test.txt", std::move(content));
        }
    }

    TEST(ZipCpp, writeStream1)
    {
        auto data = Tests::readFile(TestData::getFilePath("CMakeLists.txt"));
        {
            using namespace ZipCpp;
            auto za = ZipArchive::open("test_writeStream1.zip", LibZipOpen::CREATE | LibZipOpen::TRUNCATE);
            za.add("dir/test.txt", data);
        }
    }

    TEST(ZipCpp, writeStream2)
    {
        using namespace ZipCpp;
        auto za = ZipArchive::open("test_writeStream2.zip", LibZipOpen::CREATE | LibZipOpen::TRUNCATE);
        auto data = Tests::readFile(TestData::getFilePath("CMakeLists.txt"));
        za.add("dir/test.txt", std::move(data));
    }

    TEST(ZipCpp, writeFile1)
    {
        using namespace ZipCpp;
        auto za = ZipArchive::open("test_writeFile1.zip", LibZipOpen::CREATE | LibZipOpen::TRUNCATE);
        za.add("dir/test.txt", std::filesystem::path(TestData::getFilePath("CMakeLists.txt")));
    }
}
