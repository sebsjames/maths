# CTests

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
  ${PROJECT_SOURCE_DIR}/sm/polysolve
  ${PROJECT_SOURCE_DIR}/sm/util
  PROPERTIES LANGUAGE CXX)

# sm::range
add_executable(range_intersects range_intersects.cpp)
target_sources(range_intersects PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(range_intersects range_intersects)

# sm::range
add_executable(range_adding range_adding.cpp)
target_sources(range_adding PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(range_adding range_adding)

# sm::range
add_executable(range1 range1.cpp)
target_sources(range1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range)
add_test(range1 range1)

# sm::range
add_executable(range_constexpr1 range_constexpr1.cpp)
target_sources(range_constexpr1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range)
add_test(range_constexpr1 range_constexpr1)

# sm::range
add_executable(range_complex1 range_complex1.cpp)
target_sources(range_complex1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range)
add_test(range_complex1 range_complex1)

# sm::range
add_executable(range_vector1 range_vector1.cpp)
target_sources(range_vector1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(range_vector1 range_vector1)

# sm::random
add_executable(random1 random1.cpp)
target_sources(random1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(random1 random1)

# Random string generation
add_executable(randstring1 randstring1.cpp)
target_sources(randstring1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(randstring1 randstring1)

# test sm::vec.
add_executable(vec1 vec1.cpp)
target_sources(vec1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(vec1 vec1)

if(NOT APPLE) # Could review for up to date Mac OS 26+
  if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
    # gcc 14 compiles vec_constexpr1, but appears to generate faulty
    # code (the result of vec::pow(vec) in a constexpr function is
    # wrong). gcc 15 is ok, so perhaps this is a gcc14 bug.

    # constexpr functionality in sm::vec
    # Need C++20 for the way I use constexpr here so that it is legal to
    # have "a definition of a variable for which no initialization is
    # performed".
    add_executable(vec_constexpr1 vec_constexpr1.cpp)
    target_sources(vec_constexpr1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/random    )
    add_test(vec_constexpr1 vec_constexpr1)
  endif()
endif()

add_executable(vec_casting1 vec_casting1.cpp)
target_sources(vec_casting1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(vec_casting1 vec_casting1)

add_executable(vec_asmapkey1 vec_asmapkey1.cpp)
target_sources(vec_asmapkey1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
)
add_test(vec_asmapkey1 vec_asmapkey1)

add_executable(vec_asunordmapkey1 vec_asunordmapkey1.cpp)
target_sources(vec_asunordmapkey1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(vec_asunordmapkey1 vec_asunordmapkey1)

add_executable(array_asmapkey1 array_asmapkey1.cpp)
add_test(array_asmapkey1 array_asmapkey1)

# test sm::vec elemental operations
add_executable(vec_elementops1 vec_elementops1.cpp)
target_sources(vec_elementops1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vec_elementops1 vec_elementops1)

add_executable(vec_set_from1 vec_set_from1.cpp)
target_sources(vec_set_from1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random)
add_test(vec_set_from1 vec_set_from1)

# Test the "mathematic vector made from an std::vector" code (this vvec is better for neural networks)
add_executable(vvec1 vvec1.cpp)
target_sources(vvec1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec1 vvec1)

add_executable(vvec_ofvecs1 vvec_ofvecs1.cpp)
target_sources(vvec_ofvecs1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_ofvecs1 vvec_ofvecs1)

add_executable(vvec_operatormult1 vvec_operatormult1.cpp)
target_sources(vvec_operatormult1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_operatormult1 vvec_operatormult1)

add_executable(vvec_operatordiv1 vvec_operatordiv1.cpp)
target_sources(vvec_operatordiv1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_operatordiv1 vvec_operatordiv1)

add_executable(vvec_operatoradd1 vvec_operatoradd1.cpp)
target_sources(vvec_operatoradd1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_operatoradd1 vvec_operatoradd1)

add_executable(vvec_convolutions1 vvec_convolutions1.cpp)
target_sources(vvec_convolutions1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_convolutions1 vvec_convolutions1)

add_executable(vvec_convolutions2 vvec_convolutions2.cpp)
target_sources(vvec_convolutions2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_convolutions2 vvec_convolutions2)

add_executable(vvec_differentiation1 vvec_differentiation1.cpp)
target_sources(vvec_differentiation1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_differentiation1 vvec_differentiation1)

add_executable(vvec_zerocross1 vvec_zerocross1.cpp)
target_sources(vvec_zerocross1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_zerocross1 vvec_zerocross1)

add_executable(vvec_prune1 vvec_prune1.cpp)
target_sources(vvec_prune1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_prune1 vvec_prune1)

add_executable(vvec_threshold1 vvec_threshold1.cpp)
target_sources(vvec_threshold1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_threshold1 vvec_threshold1)

add_executable(vvec_elementcompare1 vvec_elementcompare1.cpp)
target_sources(vvec_elementcompare1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_elementcompare1 vvec_elementcompare1)

add_executable(vvec_rescale1 vvec_rescale1.cpp)
target_sources(vvec_rescale1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_rescale1 vvec_rescale1)

add_executable(vvec_set_from1 vvec_set_from1.cpp)
target_sources(vvec_set_from1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/vec)
add_test(vvec_set_from1 vvec_set_from1)

add_executable(vvec_at_signed1 vvec_at_signed1.cpp)
target_sources(vvec_at_signed1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_at_signed1 vvec_at_signed1)

add_executable(vvec_nans1 vvec_nans1.cpp)
target_sources(vvec_nans1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_nans1 vvec_nans1)

add_executable(vvec_range_extent1 vvec_range_extent1.cpp)
target_sources(vvec_range_extent1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_range_extent1 vvec_range_extent1)

add_executable(vvec_arange vvec_arange.cpp)
target_sources(vvec_arange PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(vvec_arange vvec_arange)

# Test the scaling code
add_executable(scale1 scale1.cpp)
target_sources(scale1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/scale)
add_test(scale1 scale1)

# Test the Scaling of vecs
add_executable(scale_vector1 scale_vector1.cpp)
target_sources(scale_vector1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/scale)
add_test(scale_vector1 scale_vector1)

# Test scaling complex numbers
add_executable(scale_complex1 scale_complex1.cpp)
target_sources(scale_complex1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/scale)
add_test(scale_complex1 scale_complex1)

if(HDF5_FOUND)
  # Test HDF file access
  add_executable(hdfdata1 hdfdata1.cpp)
  target_sources(hdfdata1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  # Note how if you only use HDF, you only have to link to libhdf
  target_link_libraries(hdfdata1 ${HDF5_C_LIBRARIES})
  add_test(hdfdata1 hdfdata1)

  add_executable(hdfdata2 hdfdata2.cpp)
  target_sources(hdfdata2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  target_link_libraries(hdfdata2 ${HDF5_C_LIBRARIES})
  add_test(hdfdata2 hdfdata2)

  add_executable(hdfdata3 hdfdata3.cpp)
  target_compile_definitions(hdfdata3 PUBLIC FLT=float)
  target_sources(hdfdata3 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  target_link_libraries(hdfdata3 ${HDF5_C_LIBRARIES})
  add_test(hdfdata3 hdfdata3)

  add_executable(hdfdata3d hdfdata3.cpp)
  target_compile_definitions(hdfdata3d PUBLIC FLT=double)
  target_sources(hdfdata3d PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  target_link_libraries(hdfdata3d ${HDF5_C_LIBRARIES})
  add_test(hdfdata3d hdfdata3d)

  add_executable(hdfdata4 hdfdata4.cpp)
  target_compile_definitions(hdfdata4 PUBLIC FLT=float)
  target_sources(hdfdata4 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  target_link_libraries(hdfdata4 ${HDF5_C_LIBRARIES})
  add_test(hdfdata4 hdfdata4)

  add_executable(hdfdata5 hdfdata5.cpp)
  target_sources(hdfdata5 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util
    ${PROJECT_SOURCE_DIR}/sm/hdfdata)
  target_link_libraries(hdfdata5 ${HDF5_C_LIBRARIES})
  add_test(hdfdata5 hdfdata5)
endif(HDF5_FOUND)

# Test sm::quaternion
add_executable(quaternion1 quaternion1.cpp)
target_sources(quaternion1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion)
add_test(quaternion1 quaternion1)

if(NOT APPLE)
  add_executable(quaternion_constexpr1 quaternion_constexpr1.cpp)
  target_sources(quaternion_constexpr1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion)
  add_test(quaternion_constexpr1 quaternion_constexpr1)
endif()

add_executable(quaternion_rotations_float quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_float PUBLIC FLT=float)
target_sources(quaternion_rotations_float PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion)
add_test(quaternion_rotations_float quaternion_rotations_float)

add_executable(quaternion_rotations_double quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_double PUBLIC FLT=double)
target_sources(quaternion_rotations_double PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion)
add_test(quaternion_rotations_double quaternion_rotations_double)

add_executable(mat_non_square mat_non_square.cpp)
target_sources(mat_non_square PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_non_square mat_non_square)

add_executable(mat_2x2_general mat_2x2_general.cpp)
target_sources(mat_2x2_general PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_2x2_general mat_2x2_general)

add_executable(mat_3x3_basic mat_3x3_basic.cpp)
target_sources(mat_3x3_basic PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_3x3_basic mat_3x3_basic)

add_executable(mat_3x3_general mat_3x3_general.cpp)
target_sources(mat_3x3_general PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_3x3_general mat_3x3_general)

# Test sm::mat<T, 4> (4x4 matrix)
add_executable(mat_4x4 mat_4x4.cpp)
target_sources(mat_4x4 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_4x4 mat_4x4)

add_executable(mat_4x4_translate mat_4x4_translate.cpp)
target_sources(mat_4x4_translate PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_4x4_translate mat_4x4_translate)

add_executable(mat_4x4_basic mat_4x4_basic.cpp)
target_sources(mat_4x4_basic PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_4x4_basic mat_4x4_basic)

add_executable(mat_4x4_basic2 mat_4x4_basic2.cpp)
target_sources(mat_4x4_basic2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_4x4_basic2 mat_4x4_basic2)

add_executable(mat_4x4_reflection mat_4x4_reflection.cpp)
target_sources(mat_4x4_reflection PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(mat_4x4_reflection mat_4x4_reflection)

if(NOT APPLE)
  # constexpr mat44
  add_executable(mat_4x4_constexpr mat_4x4_constexpr.cpp)
  target_sources(mat_4x4_constexpr PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat)
  add_test(mat_4x4_constexpr mat_4x4_constexpr)

  if(Eigen3_FOUND)
    add_executable(mat_4x4_vseigen mat_4x4_vseigen.cpp)
    target_include_directories (mat_4x4_vseigen BEFORE PRIVATE ${EIGEN3_INCLUDE_DIR})
    target_sources(mat_4x4_vseigen PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/quaternion
      ${PROJECT_SOURCE_DIR}/sm/mat)
    add_test(mat_4x4_vseigen mat_4x4_vseigen)
  endif(Eigen3_FOUND)
endif()

# Test quaternion and mat44 rotations in float and double precision
add_executable(rotations1_float rotations1.cpp)
target_compile_definitions(rotations1_float PUBLIC FLT=float)
target_sources(rotations1_float PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(rotations1_float rotations1_float)

add_executable(rotations1_double rotations1.cpp)
target_compile_definitions(rotations1_double PUBLIC FLT=double)
target_sources(rotations1_double PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(rotations1_double rotations1_double)

# More rotations
add_executable(rotations2 rotations2.cpp)
target_sources(rotations2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat)
add_test(rotations2 rotations2)

add_executable(test_trait_tests test_trait_tests.cpp)
target_sources(test_trait_tests PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec)
add_test(test_trait_tests test_trait_tests)

# Test Hex::get_vertex_coord()
add_executable(hex_vertexpos1 hex_vertexpos1.cpp)
target_sources(hex_vertexpos1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
  ${PROJECT_SOURCE_DIR}/sm/hex)
add_test(hex_vertexpos1 hex_vertexpos1)

# Test Hex user flags code
add_executable(hex_userflags1 hex_userflags1.cpp)
target_sources(hex_userflags1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/bezcoord
  ${PROJECT_SOURCE_DIR}/sm/hex)

# An example program which is slow compiled on Visual Studio with the /MDd "Debug build" flag
add_executable(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase.cpp)
add_test(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase)

# Compile with stdc++ lib debug mode (but don't run as a test, as it's
# very slow) This is equivalent to the default debug compilation that
# Visual Studio uses (/MDd or /MTd).
add_executable(hex_profile_list_hex_min_erase_dbg hex_profile_list_hex_min_erase.cpp)
target_compile_definitions(hex_profile_list_hex_min_erase_dbg PUBLIC _GLIBCXX_DEBUG)

add_executable(winder1 winder1.cpp)
target_sources(winder1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/winder)
add_test(winder1 winder1)

add_executable(winder2 winder2.cpp)
target_sources(winder2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/winder)
add_test(winder2 winder2)

add_executable(winder3 winder3.cpp)
target_sources(winder3 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/winder)
add_test(winder3 winder3)

# Test bootstrapping
add_executable(bootstrap1 bootstrap1.cpp)
target_sources(bootstrap1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/bootstrap)
# Test disabled - statistical fluctuations can make this fail sometimes
# add_test(bootstrap1 bootstrap1)

add_executable(grid1 grid1.cpp)
target_sources(grid1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid1 grid1)

add_executable(grid_suggest_dims1 grid_suggest_dims1.cpp)
target_sources(grid_suggest_dims1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_suggest_dims1 grid_suggest_dims1)

add_executable(grid_rowcol1 grid_rowcol1.cpp)
target_sources(grid_rowcol1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_rowcol1 grid_rowcol1)

add_executable(grid_shiftindex1 grid_shiftindex1.cpp)
target_sources(grid_shiftindex1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_shiftindex1 grid_shiftindex1)

add_executable(grid_indexlookup1 grid_indexlookup1.cpp)
target_sources(grid_indexlookup1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_indexlookup1 grid_indexlookup1)

add_executable(grid_neighbours1 grid_neighbours1.cpp)
target_sources(grid_neighbours1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_neighbours1 grid_neighbours1)

add_executable(grid_getabscissae1 grid_getabscissae1.cpp)
target_sources(grid_getabscissae1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/grid)
add_test(grid_getabscissae1 grid_getabscissae1)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
  # This program exposes a problem in gcc 14 that is fine in gcc 15. It may be a gcc bug, so just disable.
  add_executable(grid_profile1 grid_profile1.cpp)
  target_sources(grid_profile1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/grid)
  add_test(grid_profile1 grid_profile1)
endif()

# Test Nelder Mead algorithm
add_executable(nmsimplex1 nmsimplex1.cpp)
target_compile_definitions(nmsimplex1 PUBLIC FLT=float)
target_sources(nmsimplex1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
add_test(nmsimplex1 nmsimplex1)

add_executable(histo1 histo1.cpp)
target_sources(histo1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/histo)
add_test(histo1 histo1)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
  add_executable(boxfilter1 boxfilter1.cpp)
  target_sources(boxfilter1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/boxfilter)
  add_test(boxfilter1 boxfilter1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
  # Test sm::config
  add_executable(config1 config1.cpp)
  target_sources(config1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/util)
  add_test(config1 config1)
endif()

add_executable(zernike1 zernike1.cpp)
target_sources(zernike1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo)
add_test(zernike1 zernike1)

add_executable(algo_1 algo_1.cpp)
target_sources(algo_1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo)
add_test(algo_1 algo_1)

add_executable(algo_sigfigs1 algo_sigfigs1.cpp)
target_sources(algo_sigfigs1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo)
add_test(algo_sigfigs1 algo_sigfigs1)

add_executable(algo_roundtocol1 algo_roundtocol1.cpp)
target_sources(algo_roundtocol1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo)
add_test(algo_roundtocol1 algo_roundtocol1)

add_executable(geometry_2 geometry_2.cpp)
target_sources(geometry_2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_2 geometry_2)

add_executable(geometry_3 geometry_3.cpp)
target_sources(geometry_3 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_3 geometry_3)

add_executable(geometry_4 geometry_4.cpp)
target_sources(geometry_4 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_4 geometry_4)

add_executable(geometry_5 geometry_5.cpp)
target_sources(geometry_5 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_5 geometry_5)

add_executable(geometry_6 geometry_6.cpp)
target_sources(geometry_6 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/quaternion
  ${PROJECT_SOURCE_DIR}/sm/mat
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_6 geometry_6)

add_executable(geometry_xyz_to_latlong geometry_xyz_to_latlong.cpp)
target_sources(geometry_xyz_to_latlong PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_xyz_to_latlong geometry_xyz_to_latlong)

add_executable(geometry_sph_projections geometry_sph_projections.cpp)
target_sources(geometry_sph_projections PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(geometry_sph_projections geometry_sph_projections)

add_executable(graham_scan_1 graham_scan_1.cpp)
target_sources(graham_scan_1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/algo
  ${PROJECT_SOURCE_DIR}/sm/geometry)
add_test(graham_scan_1 graham_scan_1)

add_executable(util1 util1.cpp)
target_sources(util1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/util
  ${PROJECT_SOURCE_DIR}/sm/vec)
add_test(util1 util1)

add_executable(trait_tests_number_type trait_tests_number_type.cpp)
target_sources(trait_tests_number_type PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec)
add_test(trait_tests_number_type trait_tests_number_type)

add_executable(flags1 flags1.cpp)
target_sources(flags1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR} FILES ${PROJECT_SOURCE_DIR}/sm/flags)
add_test(flags1 flags1)

if(NOT APPLE)
add_executable(constexpr_math1 constexpr_math1.cpp)
add_test(constexpr_math1 constexpr_math1)
endif()

if(ARMADILLO_FOUND)

  # vvec<vec> warning about attributes in gcc14
  #if(CMAKE_COMPILER_IS_CLANG OR (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
    # BezCurve class
    set(TARGETTEST1 bez1)
    set(SOURCETEST1 bez1.cpp)
    add_executable(${TARGETTEST1} ${SOURCETEST1})
    target_sources(bez1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/quaternion
      ${PROJECT_SOURCE_DIR}/sm/mat
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
    target_link_libraries(${TARGETTEST1} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(bez1 ${TARGETTEST1})
  #endif()

  # BezCurve class (uses hexgrid)
  if(0)
  add_executable(bez2 bez2.cpp)
  target_sources(bez2 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
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
  target_link_libraries(bez2 ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(bez2 bez2)
  endif()
  # vvec<vec> warning about attributes in gcc14
  if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
    # BezCurvePath class
    add_executable(bezcurves bezcurves.cpp)
    target_sources(bezcurves PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
    target_link_libraries(bezcurves ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(bezcurves bezcurves)

    # Testing matrix representation of Bezier curves
    add_executable(bezmatrix bezmatrix.cpp)
    target_sources(bezmatrix PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex
      ${PROJECT_SOURCE_DIR}/sm/quaternion
      ${PROJECT_SOURCE_DIR}/sm/mat
      ${PROJECT_SOURCE_DIR}/sm/hex)
    target_compile_definitions(bezmatrix PUBLIC FLT=float)
    target_link_libraries(bezmatrix ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(bezmatrix bezmatrix)

    # Testing auto-fitting a Bezier curve
    add_executable(bezfit bezfit.cpp)
    target_sources(bezfit PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
    target_compile_definitions(bezfit PUBLIC FLT=float)
    target_link_libraries(bezfit ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(bezfit bezfit)

    # Testing splitting of Bezier curves
    add_executable(bezsplit bezsplit.cpp)
    target_sources(bezsplit PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
    target_compile_definitions(bezsplit PUBLIC FLT=float)
    target_link_libraries(bezsplit ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(bezsplit bezsplit)

    # Test two curves connected together
    add_executable(twocurves twocurves.cpp)
    target_sources(twocurves PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
      FILES
      ${PROJECT_SOURCE_DIR}/sm/range
      ${PROJECT_SOURCE_DIR}/sm/random
      ${PROJECT_SOURCE_DIR}/sm/vec
      ${PROJECT_SOURCE_DIR}/sm/bezcoord
      ${PROJECT_SOURCE_DIR}/sm/vvec
      ${PROJECT_SOURCE_DIR}/sm/algo
      ${PROJECT_SOURCE_DIR}/sm/nm_simplex)
    target_link_libraries(twocurves ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
    add_test(twocurves twocurves)
  endif()

endif()

# Inserting into a list. error: conflicting declaration of template. on g++-14
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99)
  add_executable(cartgrid1 cartgrid1.cpp)
  target_sources(cartgrid1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/bezcoord
    ${PROJECT_SOURCE_DIR}/sm/grid
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/scale
    ${PROJECT_SOURCE_DIR}/sm/boxfilter)
  add_test(cartgrid1 cartgrid1)

  add_executable(cartgrid_gridshiftcoords cartgrid_gridshiftcoords.cpp)
  target_sources(cartgrid_gridshiftcoords PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/bezcoord
    ${PROJECT_SOURCE_DIR}/sm/grid
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/scale
    ${PROJECT_SOURCE_DIR}/sm/boxfilter)
  add_test(cartgrid_gridshiftcoords cartgrid_gridshiftcoords)

  add_executable(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric.cpp)
  target_sources(cartgrid_shiftindicesbymetric PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/bezcoord
    ${PROJECT_SOURCE_DIR}/sm/grid
    ${PROJECT_SOURCE_DIR}/sm/vvec
    ${PROJECT_SOURCE_DIR}/sm/scale
    ${PROJECT_SOURCE_DIR}/sm/boxfilter)
  add_test(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric)
endif()

add_executable(polysolve_1 polysolve_1.cpp)
target_sources(polysolve_1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vvec
  ${PROJECT_SOURCE_DIR}/sm/polysolve)
add_test(polysolve_1 polysolve_1)

#
# mat compile/not compile tests on operators
#

if(NOT WIN32) # exclude Windows CI from doing the compile failure tests for now

  #
  # mat times vec
  #

  # Only 4x4 matrices can be multiplied with an (N-1) D vector
  add_executable(mat_succeed_4x4timesv3 mat_succeed_4x4timesv3.cpp)
  target_sources(mat_succeed_4x4timesv3 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat)
  add_test (mat_succeed_4x4timesv3 mat_succeed_4x4timesv3)

  # Matrix times vector is generally ok, as long as vector is right size
  add_executable(mat_succeed_4x4timesv4 mat_succeed_4x4timesv4.cpp)
  target_sources(mat_succeed_4x4timesv4 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat)
  add_test (mat_succeed_4x4timesv4 mat_succeed_4x4timesv4)

  #
  # mat *= vec
  #

  # To build add "-test" to the NAME
  icm_add_build_failure_test(
    NAME mat_fail_4x4timesv16
    SOURCES mat_fail_4x4timesv16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv3
    SOURCES mat_fail_4x4timeseqlsv3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv4
    SOURCES mat_fail_4x4timeseqlsv4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv16
    SOURCES mat_fail_4x4timeseqlsv16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat plus vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec3
    SOURCES mat_fail_4x4plusvec3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec4
    SOURCES mat_fail_4x4plusvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec16
    SOURCES mat_fail_4x4plusvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat += vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec3
    SOURCES mat_fail_4x4pluseqlsvec3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec4
    SOURCES mat_fail_4x4pluseqlsvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec16
    SOURCES mat_fail_4x4pluseqlsvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat minus vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec3
    SOURCES mat_fail_4x4minusvec3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec4
    SOURCES mat_fail_4x4minusvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec16
    SOURCES mat_fail_4x4minusvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat -= vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec3
    SOURCES mat_fail_4x4minuseqlsvec3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec4
    SOURCES mat_fail_4x4minuseqlsvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec16
    SOURCES mat_fail_4x4minuseqlsvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # Init with vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4initwithv3
    SOURCES mat_fail_4x4initwithv3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4initwithv4
    SOURCES mat_fail_4x4initwithv4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  add_executable(mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16.cpp)
  target_sources(mat_succeed_4x4initwithv16 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat)
  add_test (mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16)

  #
  # mat = vec (allow for same-size vec)
  #
  add_executable(mat_succeed_assignvec mat_succeed_assignvec.cpp)
  target_sources(mat_succeed_assignvec PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat)
  add_test (mat_succeed_assignvec mat_succeed_assignvec)

  icm_add_build_failure_test(
    NAME mat_fail_assignvec_diffsize
    SOURCES mat_fail_assignvec_diffsize.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat == vec (disallow)
  #

  icm_add_build_failure_test(
    NAME mat_fail_equalityvec
    SOURCES mat_fail_equalityvec.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_equalityvec4
    SOURCES mat_fail_equalityvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat != vec (disallow)
  #

  icm_add_build_failure_test(
    NAME mat_fail_nonequalityvec
    SOURCES mat_fail_nonequalityvec.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/random
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/quaternion
    ${PROJECT_SOURCE_DIR}/sm/mat
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

endif(NOT WIN32)
