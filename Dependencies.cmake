# Done as a function so that updates to variables like CMAKE_CXX_FLAGS don't
# propagate out to other targets
function(zipcpp_setup_dependencies)

  # For each dependency, see if it's already been provided to us by a parent
  # project

  if(NOT TARGET fmtlib::fmtlib)
    find_package(fmt REQUIRED)
  endif()

  if(NOT TARGET libzip::zip)
    find_package(libzip REQUIRED)
  endif()

  if(ENABLE_TESTING)
    if(NOT TARGET gtest::gtest)
      find_package(GTest REQUIRED)
    endif()
  endif()
endfunction()
