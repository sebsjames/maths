# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# Each C++ module file has to be listed here (as they don't have a .cppm or .ixx file suffix)
set_source_files_properties (
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/scale
  ${PROJECT_SOURCE_DIR}/sm/hdfdata
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat
  ${PROJECT_SOURCE_DIR}/sm/hex
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
  ${PROJECT_SOURCE_DIR}/sm/bezcurve
  ${PROJECT_SOURCE_DIR}/sm/winder
  ${PROJECT_SOURCE_DIR}/sm/bootstrap
  ${PROJECT_SOURCE_DIR}/sm/grid
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex
  ${PROJECT_SOURCE_DIR}/sm/histo
  ${PROJECT_SOURCE_DIR}/sm/boxfilter
  ${PROJECT_SOURCE_DIR}/sm/config
  ${PROJECT_SOURCE_DIR}/sm/geometry
  ${PROJECT_SOURCE_DIR}/sm/flags
  PROPERTIES LANGUAGE CXX)

add_executable(readme readme.cpp)
target_sources(readme PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion)

add_executable(vvec vvec.cpp)
target_sources(vvec PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)

add_executable(scale scale.cpp)
target_sources(scale PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/scale)

add_executable(mat33 mat33.cpp)
target_sources(mat33 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)

add_executable(mat44 mat44.cpp)
target_sources(mat44 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)

add_executable(mat44_frombasis mat44_frombasis.cpp)
target_sources(mat44_frombasis PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)

add_executable(mat_nonsquare mat_nonsquare.cpp)
target_sources(mat_nonsquare PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)

add_executable(hexgrid hexgrid.cpp)
target_sources(hexgrid PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
  ${PROJECT_SOURCE_DIR}/sm/hex
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex)

add_executable(bezcurve bezcurve.cpp)
target_sources(bezcurve PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex)

add_executable(vonmises vonmises.cpp)
target_sources(vonmises PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR} FILES ${PROJECT_SOURCE_DIR}/sm/random)

add_executable(nelder_mead nelder_mead.cpp)
target_sources(nelder_mead PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
