cmake_minimum_required(VERSION 3.28)

project(use_range LANGUAGES CXX)

include_directories(BEFORE ${PROJECT_SOURCE_DIR})

set_source_files_properties (sm/range PROPERTIES LANGUAGE CXX)

add_executable(use_range use_range.cpp)
target_sources(use_range
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            sm/range
)
target_compile_features(use_range PUBLIC cxx_std_20)

add_executable(use_range2 use_range2.cpp)
target_sources(use_range2
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            sm/range
)
target_compile_features(use_range2 PUBLIC cxx_std_20)
