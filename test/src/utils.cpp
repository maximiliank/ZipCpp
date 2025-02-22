#include "utils.h"
#include <fstream>
#include <iterator>

std::string Tests::loadFileAsString(const std::string& fileName)
{
    std::ifstream t(fileName);
    return {(std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()};
}

::ZipCpp::MemoryBuffer Tests::readFile(const std::string& fileName)
{
    // open the file:
    std::ifstream file(fileName, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // create vector with fileSize
    std::vector<std::byte> vec(static_cast<std::size_t>(fileSize));
    // read the data:
    file.read(reinterpret_cast<char*>(vec.data()), fileSize);

    return ::ZipCpp::MemoryBuffer{std::move(vec)};
}
