# 3.28.5 for basic C++20 modules; 3.30 to give access to import std;
cmake_minimum_required(VERSION 3.28.5)

# For future import std; support:
# This specific value changes as experimental support evolves. See
# `Help/dev/experimental.rst` in the CMake source corresponding to
# your CMake build for the exact value to use.
# This value is ok for cmake versions from 3.31.11 up to 4.2.3 (Seb)
# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(maths LANGUAGES CXX)

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  (This can be changed with `cmake -DCMAKE_INSTALL_PREFIX=/some/place`")

set(CMAKE_CXX_STANDARD 20) # Would need to be 23 for future import std; support
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# For clang and future import std; support
# set(CMAKE_CXX_EXTENSIONS ON)

# For future import std; support:
# Tell CMake that we explicitly want `import std`. This will initialize the
# property on all targets declared after this to 1
# set(CMAKE_CXX_MODULE_STD 1)

# Add cmake directory to cmake modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# A build-failure test from https://github.com/iboB/icm
#include(icm_build_failure_parse_and_run)
include(icm_build_failure_testing)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O3")
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES MSVC)
    # Set flags for Windows.
    # -DNOMINMAX to prefer std::min and std::max
    # /EHsc required for use of exceptions
    # /Zc:__cplusplus ensures __cplusplus define does not lie
    # /constexpr:steps is equivalent to -fconstexpr-steps or -fconstexpr-ops-limit
    if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX /EHsc /Zc:__cplusplus /constexpr:steps500000000")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX /EHsc /Zc:__cplusplus")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID MATCHES Intel)
    # To use Intel compiler, you can call cmake as: `cmake -DCMAKE_CXX_COMPILER=icpc ..` or `CXX=icpc cmake ..`
    message(WARNING "Intel compiler has not been tested for some time")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -std=c++20 -xHOST -O3")
  else() # GCC or Clang
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
      # Add compiler version check, to ensure gcc is version 11 or later.
      if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11)
        message(FATAL_ERROR "GCC version must be at least 11")
      else()
        message(STATUS "GCC version ${CMAKE_CXX_COMPILER_VERSION} OK!")
      endif()
    endif()

    set(COMPREHENSIVE_WARNING_FLAGS "-Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wfatal-errors -Wno-psabi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ${COMPREHENSIVE_WARNING_FLAGS} -O3")
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
      # Make it possible to compile complex constexpr functions (gcc only)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-ops-limit=5000000000")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconstexpr-steps=500000000")
    endif()
  endif()
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
# Windows/VS2019.
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

# Use packaged nlohmann json. Required only for config1 test and use of sm::config
# find_package(nlohmann_json REQUIRED)

# For a test program
find_package (Eigen3 NO_MODULE)

# A macro to make a list of modules into a unique list then add then to target_name
function(target_sources_modules target_name)
  # Parse function args for MODULES
  set(multiValueArgs MODULES)
  cmake_parse_arguments(PARSE_ARGV 0 arg "" "" "${multiValueArgs}")
  # Uniqueify arg_MODULES:
  list(REMOVE_DUPLICATES arg_MODULES)
  target_sources(${target_name} PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR} FILES ${arg_MODULES})
endfunction()

#
# Module groups
#
set(SM_CONSTEXPR_MATH_MODULES
  ${PROJECT_SOURCE_DIR}/sm/mathconst
  ${PROJECT_SOURCE_DIR}/sm/constexpr_math
)
set(SM_POLYSOLVE_MODULES
  ${SM_CONSTEXPR_MATH_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/polysolve
)
set(SM_BESSEL_I0_MODULES
  ${SM_POLYSOLVE_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/bessel_i0
)
set(SM_RANDOM_MODULES
  ${SM_BESSEL_I0_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/random
)
set(SM_RANGE_MODULES
  ${SM_CONSTEXPR_MATH_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/trait_tests
  ${PROJECT_SOURCE_DIR}/sm/range
)
set(SM_VEC_MODULES
  ${SM_RANGE_MODULES}
  ${SM_RANDOM_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/vec
)
set(SM_VVEC_MODULES
  ${SM_RANGE_MODULES}
  ${SM_RANDOM_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/vvec
)
set(SM_SCALE_MODULES
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/scale
)
set(SM_UTIL_MODULES
  ${PROJECT_SOURCE_DIR}/sm/trait_tests
  ${PROJECT_SOURCE_DIR}/sm/util
)
set(SM_HDFDATA_MODULES
  ${SM_VEC_MODULES}
  ${SM_VVEC_MODULES}
  ${SM_UTIL_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/hdfdata
)
set(SM_QUATERNION_MODULES
  ${SM_VEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/quaternion
)
set(SM_MAT_MODULES
  ${SM_QUATERNION_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/mat
)
set(SM_BEZCOORD_MODULES
  ${SM_VEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
)
set(SM_HEX_MODULES
  ${SM_BEZCOORD_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/hex
)
set(SM_WINDER_MODULES
  ${SM_VEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/winder
)
set(SM_BOOTSTRAP_MODULES
  ${SM_VEC_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/bootstrap
)
set(SM_GRID_MODULES
  ${SM_VEC_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/grid
)
set(SM_ALGO_MODULES
  ${SM_VEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/algo
)
set(SM_NM_SIMPLEX_MODULES
  ${SM_ALGO_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex
)
set(SM_HISTO_MODULES
  ${SM_VEC_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/histo
)
set(SM_BOXFILTER_MODULES
  ${SM_VEC_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/boxfilter
)
set(SM_GEOMETRY_MODULES
  ${SM_ALGO_MODULES}
  ${SM_VVEC_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/geometry
)
set(SM_BEZCURVE_MODULES
  ${SM_BEZCOORD_MODULES}
  ${SM_MAT_MODULES}
  ${SM_NM_SIMPLEX_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/binomial
  ${PROJECT_SOURCE_DIR}/sm/bezcurve
)
set(SM_BEZCURVEPATH_MODULES
  ${SM_BEZCURVE_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/bezcurvepath
)
set(SM_HEXGRID_MODULES
  ${SM_BEZCURVEPATH_MODULES}
  ${SM_HEX_MODULES}
  ${PROJECT_SOURCE_DIR}/sm/hexgrid
)

# All the headers are here
add_subdirectory(sm)

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
