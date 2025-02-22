ZipCpp
======

`ZipCpp` is C++ wrapper for [libzip](https://libzip.org "libzip") with support for memory buffers (via `std::vector<std::byte>`) and memory streams (via `std::istream` interface).

It depends on [libzip](https://libzip.org "libzip") and [{fmt}](https://github.com/fmtlib/fmt "fmt") as libraries.

For example usages you can have a look at [test read](test/src/test_read_zip.cpp) or [test write](test/src/test_write_zip.cpp).



Usage in other projects
-----------------------

If you have `ZipCpp` installed on your current system you can find it using
```cmake
set(ZipCpp_DIR <path_to_zip>/lib/cmake/ZipCpp)
find_package(ZipCpp CONFIG REQUIRED)
```

It is also possible to include the required subdirectories using `FetchContent_Declare`. This ensures that the same compiler flags are used.
You should only include the subfolder containing the source of `ZipCpp`.
You can create a file `cmake/ZipCpp.cmake` with the following content:
```cmake
include(FetchContent)
FetchContent_Declare(
        zip_cpp
        GIT_REPOSITORY https://github.com/maximiliank/ZipCpp.git
        GIT_TAG origin/main
        UPDATE_DISCONNECTED   ON
        SOURCE_SUBDIR somePathThatNeverExists
)

FetchContent_GetProperties(zip_cpp)
if (NOT zip_cpp_POPULATED)
    FetchContent_MakeAvailable(zip_cpp)
endif ()

if (NOT TARGET project_options)
    add_library(project_options INTERFACE)
endif ()
if (NOT TARGET project_warnings)
    add_library(project_warnings INTERFACE)
endif ()
```

and then include it in your `CMakeLists.txt`
```cmake
include(cmake/ZipCpp.cmake)
add_subdirectory(${zip_cpp_SOURCE_DIR}/Source/ZipCpp ${zip_cpp_BINARY_DIR}_zip_cpp)
```

Note that if you follow the same `CMake` setup your `INTERFACE` targets `project_options` and `project_warnings` are also applied to `ZipCpp`.
If you do not have these targets they are defined as empty interfaces.

The finding of `fmt` and `libzip` has to be done on the consuming project in this case.