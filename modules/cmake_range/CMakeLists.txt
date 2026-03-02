cmake_minimum_required(VERSION 3.28.5)

project(use_range LANGUAGES CXX)

set(COMPREHENSIVE_WARNING_FLAGS "-Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wfatal-errors -Wno-psabi")
set(CMAKE_CXX_FLAGS "-g ${COMPREHENSIVE_WARNING_FLAGS} -O3")

include_directories(BEFORE ${PROJECT_SOURCE_DIR})

set_source_files_properties (sm/range sm/vec PROPERTIES LANGUAGE CXX)

add_executable(use_include_range use_include_range.cpp)
target_compile_definitions(use_include_range PUBLIC sm_trad_include=1)
target_compile_features(use_include_range PUBLIC cxx_std_20)

# In this example, I write the #include in use_include_range2.cpp
add_executable(use_include_range2 use_include_range2.cpp)
target_compile_features(use_include_range2 PUBLIC cxx_std_20)

add_executable(use_range use_range.cpp)
target_sources(use_range PUBLIC FILE_SET CXX_MODULES FILES sm/range)
target_compile_features(use_range PUBLIC cxx_std_20)

add_executable(use_range2 use_range2.cpp)
target_sources(use_range2
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            sm/range
)
target_compile_features(use_range2 PUBLIC cxx_std_20)

add_executable(use_vec use_vec.cpp)
target_sources(use_vec
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            sm/range
            sm/vec
)
target_compile_features(use_vec PUBLIC cxx_std_20)

add_executable(use_vec2 use_vec2.cpp)
target_sources(use_vec2
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            sm/range
            sm/vec
)
target_compile_features(use_vec2 PUBLIC cxx_std_20)
