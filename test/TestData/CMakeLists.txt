set(TARGET_NAME test_data)

add_library(${TARGET_NAME} INTERFACE)

set(TEST_DATA_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/get_file_path.hpp.in"
        "${CMAKE_CURRENT_BINARY_DIR}/get_file_path.hpp" @ONLY)

target_sources(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/get_file_path.hpp")
target_include_directories(${TARGET_NAME} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${TARGET_NAME} INTERFACE fmt::fmt)
add_library(ZipCpp::${TARGET_NAME} ALIAS ${TARGET_NAME})
