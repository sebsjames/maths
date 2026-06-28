# CTests

# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# Each C++ module file has to be listed here (as they don't have a .cppm or .ixx file suffix)
set_source_files_properties (${SM_ALL_MODULES} PROPERTIES LANGUAGE CXX)

# On this branch, my_add_executable is just an alias for add_executable
macro (my_add_executable my_target my_sourcefile)
  add_executable(${my_target} ${my_sourcefile})
endmacro()

# sm::interval
add_executable(interval_intersects interval_intersects.cpp)
target_sources_modules(interval_intersects MODULES ${SM_VEC_MODULES} ${SM_INTERVAL_MODULES})
add_test(interval_intersects interval_intersects)

# sm::interval
add_executable(interval_adding interval_adding.cpp)
target_sources_modules(interval_adding MODULES ${SM_VEC_MODULES} ${SM_INTERVAL_MODULES})
add_test(interval_adding interval_adding)

# sm::interval
add_executable(interval1 interval1.cpp)
target_compile_features(interval1 PUBLIC   cxx_std_23)
target_sources_modules(interval1 MODULES ${SM_INTERVAL_MODULES})
add_test(interval1 interval1)

# sm::interval
add_executable(interval_constexpr1 interval_constexpr1.cpp)
target_sources_modules(interval_constexpr1 MODULES ${SM_INTERVAL_MODULES})
add_test(interval_constexpr1 interval_constexpr1)

# sm::interval
add_executable(interval_complex1 interval_complex1.cpp)
target_sources_modules(interval_complex1 MODULES ${SM_INTERVAL_MODULES})
add_test(interval_complex1 interval_complex1)

# sm::interval
add_executable(interval_vector1 interval_vector1.cpp)
target_sources_modules(interval_vector1 MODULES ${SM_VEC_MODULES} ${SM_INTERVAL_MODULES})
add_test(interval_vector1 interval_vector1)

# sm::interval
# FIXME: Getting an odd compiler error for this one.
add_executable(interval_vectorequality interval_vectorequality.cpp)
target_sources_modules(interval_vectorequality MODULES ${SM_VEC_MODULES} ${SM_INTERVAL_MODULES})
add_test(interval_vectorequality interval_vectorequality)

# sm::random
my_add_executable(random1 random1.cpp)
target_sources_modules(random1 MODULES ${SM_RANDOM_MODULES})
add_test(random1 random1)

# Random string generation
my_add_executable(randstring1 randstring1.cpp)
target_sources_modules(randstring1 MODULES ${SM_RANDOM_MODULES})
add_test(randstring1 randstring1)

# test sm::vec.
my_add_executable(vec1 vec1.cpp)
target_sources_modules(vec1 MODULES ${SM_VEC_MODULES})
add_test(vec1 vec1)

if(NOT APPLE) # Could review constexpr on Mac for up to date Mac OS 26+
  # constexpr functionality in sm::vec
  my_add_executable(vec_constexpr1 vec_constexpr1.cpp)
  target_sources_modules(vec_constexpr1 MODULES ${SM_VEC_MODULES})
  add_test(vec_constexpr1 vec_constexpr1)
endif()

my_add_executable(vec_casting1 vec_casting1.cpp)
target_sources_modules(vec_casting1 MODULES ${SM_VEC_MODULES})
add_test(vec_casting1 vec_casting1)

my_add_executable(vec_asmapkey1 vec_asmapkey1.cpp)
target_sources_modules(vec_asmapkey1 MODULES ${SM_VEC_MODULES})
add_test(vec_asmapkey1 vec_asmapkey1)

my_add_executable(vec_asunordmapkey1 vec_asunordmapkey1.cpp)
target_sources_modules(vec_asunordmapkey1 MODULES ${SM_VEC_MODULES})
add_test(vec_asunordmapkey1 vec_asunordmapkey1)

my_add_executable(array_asmapkey1 array_asmapkey1.cpp)
add_test(array_asmapkey1 array_asmapkey1)

# test sm::vec elemental operations
my_add_executable(vec_elementops1 vec_elementops1.cpp)
target_sources_modules(vec_elementops1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vec_elementops1 vec_elementops1)

my_add_executable(vec_set_from1 vec_set_from1.cpp)
target_sources_modules(vec_set_from1 MODULES ${SM_VEC_MODULES})
add_test(vec_set_from1 vec_set_from1)

# Test the "mathematic vector made from an std::vector" code (this vvec is better for neural networks)
my_add_executable(vvec1 vvec1.cpp)
target_sources_modules(vvec1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec1 vvec1)

my_add_executable(vvec_ofvecs1 vvec_ofvecs1.cpp)
target_sources_modules(vvec_ofvecs1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_ofvecs1 vvec_ofvecs1)

my_add_executable(vvec_operatormult1 vvec_operatormult1.cpp)
target_sources_modules(vvec_operatormult1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_operatormult1 vvec_operatormult1)

my_add_executable(vvec_operatordiv1 vvec_operatordiv1.cpp)
target_sources_modules(vvec_operatordiv1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_operatordiv1 vvec_operatordiv1)

my_add_executable(vvec_operatoradd1 vvec_operatoradd1.cpp)
target_sources_modules(vvec_operatoradd1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_operatoradd1 vvec_operatoradd1)

my_add_executable(vvec_convolutions1 vvec_convolutions1.cpp)
target_sources_modules(vvec_convolutions1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_convolutions1 vvec_convolutions1)

my_add_executable(vvec_convolutions2 vvec_convolutions2.cpp)
target_sources_modules(vvec_convolutions2 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_convolutions2 vvec_convolutions2)

my_add_executable(vvec_differentiation1 vvec_differentiation1.cpp)
target_sources_modules(vvec_differentiation1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_differentiation1 vvec_differentiation1)

my_add_executable(vvec_zerocross1 vvec_zerocross1.cpp)
target_sources_modules(vvec_zerocross1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_zerocross1 vvec_zerocross1)

my_add_executable(vvec_prune1 vvec_prune1.cpp)
target_sources_modules(vvec_prune1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_prune1 vvec_prune1)

my_add_executable(vvec_threshold1 vvec_threshold1.cpp)
target_sources_modules(vvec_threshold1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_threshold1 vvec_threshold1)

my_add_executable(vvec_elementcompare1 vvec_elementcompare1.cpp)
target_sources_modules(vvec_elementcompare1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_elementcompare1 vvec_elementcompare1)

my_add_executable(vvec_rescale1 vvec_rescale1.cpp)
target_sources_modules(vvec_rescale1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_rescale1 vvec_rescale1)

my_add_executable(vvec_set_from1 vvec_set_from1.cpp)
target_sources_modules(vvec_set_from1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_set_from1 vvec_set_from1)

my_add_executable(vvec_at_signed1 vvec_at_signed1.cpp)
target_sources_modules(vvec_at_signed1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_at_signed1 vvec_at_signed1)

my_add_executable(vvec_nans1 vvec_nans1.cpp)
target_sources_modules(vvec_nans1 MODULES ${SM_VVEC_MODULES})
add_test(vvec_nans1 vvec_nans1)

my_add_executable(vvec_range_extent1 vvec_range_extent1.cpp)
target_sources_modules(vvec_range_extent1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_range_extent1 vvec_range_extent1)

my_add_executable(vvec_arange vvec_arange.cpp)
target_sources_modules(vvec_arange MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(vvec_arange vvec_arange)

# Test the scaling code
my_add_executable(scale1 scale1.cpp)
target_sources_modules(scale1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES} ${SM_SCALE_MODULES})
add_test(scale1 scale1)

# Test the Scaling of vecs
my_add_executable(scale_vector1 scale_vector1.cpp)
target_sources_modules(scale_vector1 MODULES ${SM_VEC_MODULES} ${SM_SCALE_MODULES})
add_test(scale_vector1 scale_vector1)

# Test scaling complex numbers
my_add_executable(scale_complex1 scale_complex1.cpp)
target_sources_modules(scale_complex1 MODULES ${SM_VEC_MODULES} ${SM_SCALE_MODULES})
add_test(scale_complex1 scale_complex1)

if(HDF5_FOUND
    AND NOT CMAKE_CXX_COMPILER_ID MATCHES MSVC
    AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15.0))
    # Test HDF file access
    my_add_executable(hdfdata1 hdfdata1.cpp)
    target_sources_modules(hdfdata1 MODULES ${SM_HDFDATA_MODULES})
    # Note how if you only use HDF, you only have to link to libhdf
    target_link_libraries(hdfdata1 ${HDF5_C_LIBRARIES})
    add_test(hdfdata1 hdfdata1)

    my_add_executable(hdfdata2 hdfdata2.cpp)
    target_sources_modules(hdfdata2 MODULES ${SM_HDFDATA_MODULES})
    target_link_libraries(hdfdata2 ${HDF5_C_LIBRARIES})
    add_test(hdfdata2 hdfdata2)

    my_add_executable(hdfdata3 hdfdata3.cpp)
    target_compile_definitions(hdfdata3 PUBLIC FLT=float)
    target_sources_modules(hdfdata3 MODULES ${SM_HDFDATA_MODULES})
    target_link_libraries(hdfdata3 ${HDF5_C_LIBRARIES})
    add_test(hdfdata3 hdfdata3)

    my_add_executable(hdfdata3d hdfdata3.cpp)
    target_compile_definitions(hdfdata3d PUBLIC FLT=double)
    target_sources_modules(hdfdata3d MODULES ${SM_HDFDATA_MODULES})
    target_link_libraries(hdfdata3d ${HDF5_C_LIBRARIES})
    add_test(hdfdata3d hdfdata3d)

    my_add_executable(hdfdata4 hdfdata4.cpp)
    target_compile_definitions(hdfdata4 PUBLIC FLT=float)
    target_sources_modules(hdfdata4 MODULES ${SM_HDFDATA_MODULES})
    target_link_libraries(hdfdata4 ${HDF5_C_LIBRARIES})
    add_test(hdfdata4 hdfdata4)

    my_add_executable(hdfdata5 hdfdata5.cpp)
    target_sources_modules(hdfdata5 MODULES ${SM_HDFDATA_MODULES})
    target_link_libraries(hdfdata5 ${HDF5_C_LIBRARIES})
    add_test(hdfdata5 hdfdata5)
endif()

# Test sm::quaternion
my_add_executable(quaternion1 quaternion1.cpp)
target_sources_modules(quaternion1 MODULES ${SM_QUATERNION_MODULES})
add_test(quaternion1 quaternion1)

if(NOT APPLE)
  my_add_executable(quaternion_constexpr1 quaternion_constexpr1.cpp)
  target_sources_modules(quaternion_constexpr1 MODULES ${SM_QUATERNION_MODULES})
  add_test(quaternion_constexpr1 quaternion_constexpr1)
endif()

my_add_executable(quaternion_rotations_float quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_float PUBLIC FLT=float)
target_sources_modules(quaternion_rotations_float MODULES ${SM_QUATERNION_MODULES})
add_test(quaternion_rotations_float quaternion_rotations_float)

my_add_executable(quaternion_rotations_double quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_double PUBLIC FLT=double)
target_sources_modules(quaternion_rotations_double MODULES ${SM_QUATERNION_MODULES})
add_test(quaternion_rotations_double quaternion_rotations_double)

my_add_executable(mat_matrixeqns mat_matrixeqns.cpp)
target_sources_modules(mat_matrixeqns MODULES ${SM_MAT_MODULES})
add_test(mat_matrixeqns mat_matrixeqns)

my_add_executable(mat_non_square mat_non_square.cpp)
target_sources_modules(mat_non_square MODULES ${SM_MAT_MODULES})
add_test(mat_non_square mat_non_square)

my_add_executable(mat_2x2_general mat_2x2_general.cpp)
target_sources_modules(mat_2x2_general MODULES ${SM_MAT_MODULES})
add_test(mat_2x2_general mat_2x2_general)

my_add_executable(mat_3x3_basic mat_3x3_basic.cpp)
target_sources_modules(mat_3x3_basic MODULES ${SM_MAT_MODULES})
add_test(mat_3x3_basic mat_3x3_basic)

my_add_executable(mat_3x3_general mat_3x3_general.cpp)
target_sources_modules(mat_3x3_general MODULES ${SM_MAT_MODULES})
add_test(mat_3x3_general mat_3x3_general)

# Test sm::mat<T, 4> (4x4 matrix)
my_add_executable(mat_4x4 mat_4x4.cpp)
target_sources_modules(mat_4x4 MODULES ${SM_MAT_MODULES})
add_test(mat_4x4 mat_4x4)

my_add_executable(mat_formatting mat_formatting.cpp)
target_sources_modules(mat_formatting MODULES ${SM_MAT_MODULES})
add_test(mat_formatting mat_formatting)

my_add_executable(mat_4x4_translate mat_4x4_translate.cpp)
target_sources_modules(mat_4x4_translate MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_translate mat_4x4_translate)

my_add_executable(mat_4x4_rotation mat_4x4_rotation.cpp)
target_sources_modules(mat_4x4_rotation MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_rotation mat_4x4_rotation)

my_add_executable(mat_4x4_basic mat_4x4_basic.cpp)
target_sources_modules(mat_4x4_basic MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_basic mat_4x4_basic)

my_add_executable(mat_4x4_basic_complex mat_4x4_basic_complex.cpp)
target_sources_modules(mat_4x4_basic_complex MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_basic_complex mat_4x4_basic_complex)

my_add_executable(mat_4x4_basic2 mat_4x4_basic2.cpp)
target_sources_modules(mat_4x4_basic2 MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_basic2 mat_4x4_basic2)

my_add_executable(mat_4x4_reflection mat_4x4_reflection.cpp)
target_sources_modules(mat_4x4_reflection MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_reflection mat_4x4_reflection)

my_add_executable(mat_4x4_eigenvalues mat_4x4_eigenvalues.cpp)
target_sources_modules(mat_4x4_eigenvalues MODULES ${SM_MAT_MODULES})
add_test(mat_4x4_eigenvalues mat_4x4_eigenvalues)

if(0)
if(NOT APPLE)
  # constexpr mat
  my_add_executable(mat_4x4_constexpr mat_4x4_constexpr.cpp)
  target_sources_modules(mat_4x4_constexpr MODULES ${SM_MAT_MODULES})
  add_test(mat_4x4_constexpr mat_4x4_constexpr)

  if(Eigen3_FOUND)
    my_add_executable(mat_4x4_vseigen mat_4x4_vseigen.cpp)
    target_include_directories (mat_4x4_vseigen BEFORE PRIVATE ${EIGEN3_INCLUDE_DIR})
    target_sources_modules(mat_4x4_vseigen MODULES ${SM_MAT_MODULES})
    add_test(mat_4x4_vseigen mat_4x4_vseigen)
  endif(Eigen3_FOUND)
endif()
endif()

# Test quaternion and mat44 rotations in float and double precision
my_add_executable(rotations1_float rotations1.cpp)
target_compile_definitions(rotations1_float PUBLIC FLT=float)
target_sources_modules(rotations1_float MODULES ${SM_MAT_MODULES})
add_test(rotations1_float rotations1_float)

my_add_executable(rotations1_double rotations1.cpp)
target_compile_definitions(rotations1_double PUBLIC FLT=double)
target_sources_modules(rotations1_double MODULES ${SM_MAT_MODULES})
add_test(rotations1_double rotations1_double)

# More rotations
my_add_executable(rotations2 rotations2.cpp)
target_sources_modules(rotations2 MODULES ${SM_MAT_MODULES})
add_test(rotations2 rotations2)

# Spline interpolation
my_add_executable(spline1 spline1.cpp)
target_sources_modules(spline1 MODULES ${SM_SPLINE_MODULES})
add_test(spline1 spline1)

my_add_executable(test_trait_tests test_trait_tests.cpp)
target_sources_modules(test_trait_tests MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(test_trait_tests test_trait_tests)

# Test Hex::get_vertex_coord()
my_add_executable(hex_vertexpos1 hex_vertexpos1.cpp)
target_sources_modules(hex_vertexpos1 MODULES ${SM_VVEC_MODULES} ${SM_HEX_MODULES})
add_test(hex_vertexpos1 hex_vertexpos1)

# Test Hex user flags code
my_add_executable(hex_userflags1 hex_userflags1.cpp)
target_sources_modules(hex_userflags1 MODULES ${SM_HEX_MODULES})

# An example program which is slow compiled on Visual Studio with the /MDd "Debug build" flag
my_add_executable(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase.cpp)
add_test(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase)

# Compile with stdc++ lib debug mode (but don't run as a test, as it's
# very slow) This is equivalent to the default debug compilation that
# Visual Studio uses (/MDd or /MTd).
my_add_executable(hex_profile_list_hex_min_erase_dbg hex_profile_list_hex_min_erase.cpp)
target_compile_definitions(hex_profile_list_hex_min_erase_dbg PUBLIC _GLIBCXX_DEBUG)

my_add_executable(winder1 winder1.cpp)
target_sources_modules(winder1 MODULES ${SM_VVEC_MODULES} ${SM_WINDER_MODULES})
add_test(winder1 winder1)

my_add_executable(winder2 winder2.cpp)
target_sources_modules(winder2 MODULES ${SM_WINDER_MODULES})
add_test(winder2 winder2)

my_add_executable(winder3 winder3.cpp)
target_sources_modules(winder3 MODULES ${SM_VVEC_MODULES} ${SM_WINDER_MODULES})
add_test(winder3 winder3)

# Test bootstrapping
my_add_executable(bootstrap1 bootstrap1.cpp)
target_sources_modules(bootstrap1 MODULES ${SM_BOOTSTRAP_MODULES})
# Test disabled - statistical fluctuations can make this fail sometimes
# add_test(bootstrap1 bootstrap1)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  my_add_executable(grid1 grid1.cpp)
  target_sources_modules(grid1 MODULES ${SM_GRID_MODULES})
  add_test(grid1 grid1)

  my_add_executable(grid_suggest_dims1 grid_suggest_dims1.cpp)
  target_sources_modules(grid_suggest_dims1 MODULES ${SM_GRID_MODULES})
  add_test(grid_suggest_dims1 grid_suggest_dims1)

  my_add_executable(grid_rowcol1 grid_rowcol1.cpp)
  target_sources_modules(grid_rowcol1 MODULES ${SM_GRID_MODULES})
  add_test(grid_rowcol1 grid_rowcol1)

  my_add_executable(grid_shiftindex1 grid_shiftindex1.cpp)
  target_sources_modules(grid_shiftindex1 MODULES ${SM_GRID_MODULES})
  add_test(grid_shiftindex1 grid_shiftindex1)

  my_add_executable(grid_indexlookup1 grid_indexlookup1.cpp)
  target_sources_modules(grid_indexlookup1 MODULES ${SM_GRID_MODULES})
  add_test(grid_indexlookup1 grid_indexlookup1)

  my_add_executable(grid_neighbours1 grid_neighbours1.cpp)
  target_sources_modules(grid_neighbours1 MODULES ${SM_GRID_MODULES})
  add_test(grid_neighbours1 grid_neighbours1)

  my_add_executable(grid_getabscissae1 grid_getabscissae1.cpp)
  target_sources_modules(grid_getabscissae1 MODULES ${SM_GRID_MODULES})
  add_test(grid_getabscissae1 grid_getabscissae1)

  my_add_executable(grid_profile1 grid_profile1.cpp)
  target_sources_modules(grid_profile1 MODULES  ${SM_GRID_MODULES})
  add_test(grid_profile1 grid_profile1)
endif()

# Test Nelder Mead algorithm
my_add_executable(nmsimplex1 nmsimplex1.cpp)
target_compile_definitions(nmsimplex1 PUBLIC FLT=float)
target_sources_modules(nmsimplex1 MODULES ${SM_NM_SIMPLEX_MODULES})
add_test(nmsimplex1 nmsimplex1)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  my_add_executable(histo1 histo1.cpp)
  target_sources_modules(histo1 MODULES ${SM_HISTO_MODULES})
  add_test(histo1 histo1)

  my_add_executable(boxfilter1 boxfilter1.cpp)
  target_sources_modules(boxfilter1 MODULES ${SM_BOXFILTER_MODULES})
  add_test(boxfilter1 boxfilter1)
endif()

# Test sm::config

# GCC < version 16.1 can't compile sm::config (something to do with the nlohmann::json module). It also seems to be an issue on Windows
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 16.1))
  my_add_executable(config1 config1.cpp)
  target_sources_modules(config1 MODULES ${SM_CONFIG_MODULES} ${SM_UTIL_MODULES} ${SM_VVEC_MODULES} ${SM_VEC_MODULES})
  add_test(config1 config1)
endif()

my_add_executable(zernike1 zernike1.cpp)
target_sources_modules(zernike1 MODULES ${SM_ALGO_MODULES} ${SM_VVEC_MODULES})
add_test(zernike1 zernike1)

my_add_executable(algo_1 algo_1.cpp)
target_sources_modules(algo_1 MODULES ${SM_ALGO_MODULES} ${SM_VVEC_MODULES})
add_test(algo_1 algo_1)

my_add_executable(algo_sigfigs1 algo_sigfigs1.cpp)
target_sources_modules(algo_sigfigs1 MODULES ${SM_ALGO_MODULES} ${SM_VVEC_MODULES})
add_test(algo_sigfigs1 algo_sigfigs1)

my_add_executable(algo_roundtocol1 algo_roundtocol1.cpp)
target_sources_modules(algo_roundtocol1 MODULES ${SM_ALGO_MODULES} ${SM_VVEC_MODULES})
add_test(algo_roundtocol1 algo_roundtocol1)

my_add_executable(algo_ransac algo_ransac.cpp)
target_sources_modules(algo_ransac MODULES ${SM_ALGO_MODULES} ${SM_VVEC_MODULES})
add_test(algo_ransac algo_ransac)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  my_add_executable(geometry_2 geometry_2.cpp)
  target_sources_modules(geometry_2 MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_2 geometry_2)

  my_add_executable(geometry_3 geometry_3.cpp)
  target_sources_modules(geometry_3 MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_3 geometry_3)

  my_add_executable(geometry_4 geometry_4.cpp)
  target_sources_modules(geometry_4 MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_4 geometry_4)

  my_add_executable(geometry_5 geometry_5.cpp)
  target_sources_modules(geometry_5 MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_5 geometry_5)

  my_add_executable(geometry_6 geometry_6.cpp)
  target_sources_modules(geometry_6 MODULES ${SM_MAT_MODULES} ${SM_GEOMETRY_MODULES})
  add_test(geometry_6 geometry_6)

  my_add_executable(geometry_xyz_to_latlong geometry_xyz_to_latlong.cpp)
  target_sources_modules(geometry_xyz_to_latlong MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_xyz_to_latlong geometry_xyz_to_latlong)

  my_add_executable(geometry_sph_projections geometry_sph_projections.cpp)
  target_sources_modules(geometry_sph_projections MODULES ${SM_GEOMETRY_MODULES})
  add_test(geometry_sph_projections geometry_sph_projections)

  my_add_executable(graham_scan_1 graham_scan_1.cpp)
  target_sources_modules(graham_scan_1 MODULES ${SM_GEOMETRY_MODULES})
  add_test(graham_scan_1 graham_scan_1)
endif()

my_add_executable(util1 util1.cpp)
target_sources_modules(util1 MODULES ${SM_VEC_MODULES} ${SM_UTIL_MODULES})
add_test(util1 util1)

my_add_executable(trait_tests_number_type trait_tests_number_type.cpp)
target_sources_modules(trait_tests_number_type MODULES ${SM_VEC_MODULES})
add_test(trait_tests_number_type trait_tests_number_type)

my_add_executable(trait_tests_has_size trait_tests_has_size.cpp)
target_sources_modules(trait_tests_has_size MODULES ${SM_VEC_MODULES})
add_test(trait_tests_has_size trait_tests_has_size)

my_add_executable(flags1 flags1.cpp)
target_sources_modules (flags1 MODULES ${PROJECT_SOURCE_DIR}/sm/flags.cppm)
add_test(flags1 flags1)

if(NOT APPLE)
  my_add_executable(constexpr_math1 constexpr_math1.cpp)
  target_sources_modules(constexpr_math1 MODULES ${SM_CONSTEXPR_MATH_MODULES})
  add_test(constexpr_math1 constexpr_math1)
endif()

# vvec<vec> warning about attributes in gcc14
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  # BezCurve class
  set(TARGETTEST1 bez1)
  set(SOURCETEST1 bez1.cpp)
  my_add_executable(${TARGETTEST1} ${SOURCETEST1})
  target_sources_modules(bez1 MODULES ${SM_BEZCURVE_MODULES})
  add_test(bez1 ${TARGETTEST1})

  # BezCurve class (uses hexgrid)
  my_add_executable(bez2 bez2.cpp)
  target_sources_modules(bez2 MODULES ${SM_HEXGRID_MODULES})
  add_test(bez2 bez2)

  # BezCurvePath class
  my_add_executable(bezcurves bezcurves.cpp)
  target_sources_modules(bezcurves MODULES ${SM_BEZCURVEPATH_MODULES})
  add_test(bezcurves bezcurves)

  # Testing matrix representation of Bezier curves
  my_add_executable(bezmatrix bezmatrix.cpp)
  target_sources_modules(bezmatrix MODULES ${SM_HEXGRID_MODULES})
  target_compile_definitions(bezmatrix PUBLIC FLT=float)
  add_test(bezmatrix bezmatrix)

  # Testing auto-fitting a Bezier curve
  my_add_executable(bezfit bezfit.cpp)
  target_sources_modules(bezfit MODULES ${SM_BEZCURVE_MODULES})
  target_compile_definitions(bezfit PUBLIC FLT=float)
  add_test(bezfit bezfit)

  # Testing splitting of Bezier curves
  my_add_executable(bezsplit bezsplit.cpp)
  target_sources_modules(bezsplit MODULES ${SM_BEZCURVE_MODULES})
  target_compile_definitions(bezsplit PUBLIC FLT=float)
  add_test(bezsplit bezsplit)

  # Test two curves connected together
  my_add_executable(twocurves twocurves.cpp)
  target_sources_modules(twocurves MODULES ${SM_BEZCURVE_MODULES})
  add_test(twocurves twocurves)

  # Inserting into a list. error: conflicting declaration of template. on g++-14
  my_add_executable(cartgrid1 cartgrid1.cpp)
  target_sources_modules(cartgrid1 MODULES ${SM_BEZCOORD_MODULES} ${SM_GRID_MODULES} ${SM_SCALE_MODULES} ${SM_BOXFILTER_MODULES} ${SM_CARTGRID_MODULES})
  add_test(cartgrid1 cartgrid1)

  my_add_executable(cartgrid_gridshiftcoords cartgrid_gridshiftcoords.cpp)
  target_sources_modules(cartgrid_gridshiftcoords MODULES ${SM_BEZCOORD_MODULES} ${SM_GRID_MODULES} ${SM_SCALE_MODULES} ${SM_BOXFILTER_MODULES} ${SM_CARTGRID_MODULES})
  add_test(cartgrid_gridshiftcoords cartgrid_gridshiftcoords)

  my_add_executable(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric.cpp)
  target_sources_modules(cartgrid_shiftindicesbymetric MODULES ${SM_BEZCOORD_MODULES} ${SM_GRID_MODULES} ${SM_SCALE_MODULES} ${SM_BOXFILTER_MODULES} ${SM_SCALE_MODULES} ${SM_CARTGRID_MODULES})
  add_test(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric)
endif()

my_add_executable(polysolve_1 polysolve_1.cpp)
target_sources_modules(polysolve_1 MODULES ${SM_VEC_MODULES} ${SM_VVEC_MODULES})
add_test(polysolve_1 polysolve_1)

#
# mat compile/not compile tests on operators
#

# exclude some CI (Windows, toolchains that use a lot of data) from doing the compile failure tests for now
if(BUILD_COMPILE_FAILURE_TESTS)

  #
  # mat times vec
  #

  # Only 4x4 matrices can be multiplied with an (N-1) D vector
  my_add_executable(mat_succeed_4x4timesv3 mat_succeed_4x4timesv3.cpp)
  target_sources_modules(mat_succeed_4x4timesv3 MODULES ${SM_MAT_MODULES})
  add_test (mat_succeed_4x4timesv3 mat_succeed_4x4timesv3)

  # Matrix times vector is generally ok, as long as vector is right size
  my_add_executable(mat_succeed_4x4timesv4 mat_succeed_4x4timesv4.cpp)
  target_sources_modules(mat_succeed_4x4timesv4 MODULES ${SM_MAT_MODULES})
  add_test (mat_succeed_4x4timesv4 mat_succeed_4x4timesv4)

  #
  # mat *= vec
  #

  # To build add "-test" to the NAME
  icm_add_build_failure_test(
    NAME mat_fail_4x4timesv16
    SOURCES mat_fail_4x4timesv16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv3
    SOURCES mat_fail_4x4timeseqlsv3.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv4
    SOURCES mat_fail_4x4timeseqlsv4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv16
    SOURCES mat_fail_4x4timeseqlsv16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec4
    SOURCES mat_fail_4x4plusvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec16
    SOURCES mat_fail_4x4plusvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec4
    SOURCES mat_fail_4x4pluseqlsvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec16
    SOURCES mat_fail_4x4pluseqlsvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec4
    SOURCES mat_fail_4x4minusvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec16
    SOURCES mat_fail_4x4minusvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec4
    SOURCES mat_fail_4x4minuseqlsvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec16
    SOURCES mat_fail_4x4minuseqlsvec16.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4initwithv4
    SOURCES mat_fail_4x4initwithv4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  my_add_executable(mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16.cpp)
  target_sources_modules(mat_succeed_4x4initwithv16 MODULES ${SM_MAT_MODULES} ${SM_VEC_MODULES})
  add_test (mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16)

  #
  # mat = vec (allow for same-size vec)
  #
  my_add_executable(mat_succeed_assignvec mat_succeed_assignvec.cpp)
  target_sources_modules(mat_succeed_assignvec  MODULES ${SM_MAT_MODULES} ${SM_VEC_MODULES})
  add_test (mat_succeed_assignvec mat_succeed_assignvec)

  icm_add_build_failure_test(
    NAME mat_fail_assignvec_diffsize
    SOURCES mat_fail_assignvec_diffsize.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_equalityvec4
    SOURCES mat_fail_equalityvec4.cpp
    MODULE_BASE ${PROJECT_SOURCE_DIR}
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
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
    MODULE_FILES ${SM_MAT_MODULES} ${SM_VEC_MODULES}
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

endif(BUILD_COMPILE_FAILURE_TESTS)
