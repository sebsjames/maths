# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# Each C++ module file has to be listed here (as they don't have a .cppm or .ixx file suffix)
set_source_files_properties (${SM_ALL_MODULES} PROPERTIES LANGUAGE CXX)

add_executable(readme readme.cpp)
target_sources_modules(readme MODULES ${SM_QUATERNION_MODULES} ${SM_VEC_MODULES})

add_executable(vvec vvec.cpp)
target_sources_modules(vvec MODULES ${SM_VVEC_MODULES})

add_executable(scale scale.cpp)
target_sources_modules(scale MODULES ${SM_SCALE_MODULES} ${SM_VEC_MODULES})

add_executable(mat33 mat33.cpp)
target_sources_modules(mat33 MODULES ${SM_MAT_MODULES})

add_executable(mat44 mat44.cpp)
target_sources_modules(mat44 MODULES ${SM_MAT_MODULES})

add_executable(mat44_frombasis mat44_frombasis.cpp)
target_sources_modules(mat44_frombasis MODULES ${SM_MAT_MODULES})

add_executable(mat_nonsquare mat_nonsquare.cpp)
target_sources_modules(mat_nonsquare MODULES ${SM_MAT_MODULES})

add_executable(hexgrid hexgrid.cpp)
target_sources_modules(hexgrid MODULES ${SM_HEXGRID_MODULES})

add_executable(bezcurve bezcurve.cpp)
target_sources_modules(bezcurve MODULES ${SM_BEZCURVE_MODULES})

add_executable(vonmises vonmises.cpp)
target_sources_modules(vonmises MODULES ${SM_VVEC_MODULES})

add_executable(nelder_mead nelder_mead.cpp)
target_sources_modules(nelder_mead MODULES ${SM_NM_SIMPLEX_MODULES} ${SM_VVEC_MODULES})
