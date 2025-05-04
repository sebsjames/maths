cmake_minimum_required(VERSION 3.10)
project(maths)

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  (This can be changed with `cmake -DCMAKE_INSTALL_PREFIX=/some/place`")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckIncludeFileCXX) # CHECK_INCLUDE_FILE_CXX will be used in later scripts

# If I *require* std::format, I can start using it across this library. That
# limits us to gcc-13 on Ubuntu (though it's the libc++ that contains
# <format>). For now, test and use it only in examples.
check_include_file_cxx(format HAVE_STD_FORMAT)
if(NOT HAVE_STD_FORMAT)
  message(INFO " Your compiler does not appear to provide std::format, some examples won't build.")
  message(INFO " In future, <format> may be required.")
endif()

# Add the host definition to CXXFLAGS along with other switches
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wall -g -O3")
else()
  if(NOT CMAKE_SYSTEM MATCHES Win.*)
    # To use Intel compiler, you can call cmake as: `cmake -DCMAKE_CXX_COMPILER=icpc ..` or `CXX=icpc cmake ..`
    if(CMAKE_CXX_COMPILER_ID MATCHES Intel)
      message(WARNING "Intel compiler has not been tested for some time")
      set(CMAKE_CXX_FLAGS "-Wall -g -std=c++20 -xHOST -O3")
    else() # GCC or Clang
      if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        # Add compiler version check, to ensure gcc is version 11 or later.
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
          message(FATAL_ERROR "GCC version must be at least 11")
        else()
          message(STATUS "GCC version ${CMAKE_CXX_COMPILER_VERSION} OK!")
        endif()
      endif()
      set(COMPREHENSIVE_WARNING_FLAGS "-Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wfatal-errors -Wno-psabi")
      set(CMAKE_CXX_FLAGS "-g ${COMPREHENSIVE_WARNING_FLAGS} -O3")
      if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        # Make it possible to compile complex constexpr functions (gcc only)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-ops-limit=5000000000")
      else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=500000000")
      endif()
    endif()
  else() # Windows
    # Set flags for Windows.
    # -DNOMINMAX to prefer std::min and std::max
    # /EHsc required for use of exceptions
    # /Zc:__cplusplus ensures __cplusplus define does not lie
    # /constexpr:steps is equivalent to -fconstexpr-steps or -fconstexpr-ops-limit
    set(CMAKE_CXX_FLAGS "-DNOMINMAX /EHsc /Zc:__cplusplus /constexpr:steps500000000")
  endif() # CMAKE_SYSTEM MATCHES Win.*
endif()

# If you tell the code you have std::format, then it may use it
if(HAVE_STD_FORMAT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMATHS_HAVE_STD_FORMAT")
endif()

# In Visual Studio, if CMAKE_BUILD_TYPE is "Debug", some programs will run very slowly
if(WIN32)
  set(CMAKE_BUILD_TYPE "Release")
endif(WIN32)

# Some programs may require the ability to define literal template args of
# type std::array (a C++-20 feature). Clang versions less than 18 can't.
set(ARRAY_LITERAL_TEMPL_ARGS_ARE_OK 0)
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
    set(ARRAY_LITERAL_TEMPL_ARGS_ARE_OK 1)
  endif()
else()
  set(ARRAY_LITERAL_TEMPL_ARGS_ARE_OK 1)
endif()

# Note that we require OpenMP version 3 or above for ability to use
# unsigned integral types in parallel for loops. At time of writing,
# Visual Studio 2019 offers OpenMP 2 by default and 3 with the special
# switch -openmp:llvm. I don't know how to get cmake to use the
# special version, so OpenMP will not work at present on
# Windows/VS2019. But then, you wouldn't use Windows for serious
# numerical work anyway.
find_package(OpenMP 3)
if(OpenMP_CXX_FOUND)
  message(STATUS "Found OpenMP version ${OpenMP_C_VERSION}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  if(APPLE)
    # FIXME: Check the OpenMP root environment variable, which is required on some Mac versions
  endif()
endif()

# Lib finding

# Find the HDF5 library. To prefer the use of static linking of HDF5, set HDF5_USE_STATIC_LIBRARIES first
#set(HDF5_USE_STATIC_LIBRARIES ON)
find_package(HDF5)
if(HDF5_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HDF5_DEFINITIONS}")
endif()

if(HDF5_FOUND)
  include_directories(${HDF5_INCLUDE_DIR})
endif()

find_package(Armadillo)
if(ARMADILLO_FOUND)
  include_directories(${ARMADILLO_INCLUDE_DIR} ${ARMADILLO_INCLUDE_DIRS})
endif()

# Use packaged nlohmann json
#find_package(nlohmann_json REQUIRED)

# All the headers are here
add_subdirectory(morph)

# Unit testing using the ctest framework
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif(BUILD_TESTS)

# Example code (you can also see tests/ for examples)
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

# Additional examples (requires BUILD_EXAMPLES ON)
option(BUILD_DOC_SCREENSHOTS "Build documentation screenshot examples" OFF)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else(DOXYGEN_FOUND)
    message("Need doxygen for documentation")
  endif(DOXYGEN_FOUND)
endif(BUILD_DOC)

# For debugging of variables:
option(DEBUG_VARIABLES OFF)
if(DEBUG_VARIABLES)
  get_cmake_property(_variableNames VARIABLES)
  foreach(_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif(DEBUG_VARIABLES)
