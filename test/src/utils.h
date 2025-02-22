#pragma once

#include <string>
#include "ZipCpp/memorybuffer.h"

namespace Tests {
    std::string loadFileAsString(const std::string& fileName);

    ::ZipCpp::MemoryBuffer readFile(const std::string& fileName);
}
