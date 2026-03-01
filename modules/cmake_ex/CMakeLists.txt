cmake_minimum_required(VERSION 3.28)

project(test_modules LANGUAGES CXX)

add_executable(test_modules
    main.cpp
)

target_sources(test_modules
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            foo.ixx
            foo_part.ixx
    PRIVATE
        foo.cpp
        foo_part.cpp
)

target_compile_features(test_modules PUBLIC cxx_std_20)
