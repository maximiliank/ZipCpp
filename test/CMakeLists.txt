find_package(GTest REQUIRED)

add_subdirectory(TestData)
set(TARGET_NAME UnitTests)

file(GLOB_RECURSE sources src/*.cpp src/*.h src/*.hpp)

add_executable(${TARGET_NAME} main.cpp ${sources})
target_link_libraries(
  ${TARGET_NAME} PRIVATE project_options project_warnings GTest::GTest ZipCpp
                         ZipCpp::test_data)
# add Source directory to enable testing of internal implementations
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Source)

add_test(NAME unit COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME})
