# CTests

# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# Each C++ module file has to be listed here (as they don't have a .cppm or .ixx file suffix)
set_source_files_properties (${SM_ALL_MODULES} PROPERTIES LANGUAGE CXX)

# Compile a library with all modules other than those that need third party code or links (sm::config and sm::hdfdata)
add_library(sm STATIC)
target_sources_modules(sm MODULES ${SM_ALL_MODULES})

# sm::interval
add_executable(interval_intersects interval_intersects.cpp)
target_link_libraries(interval_intersects PRIVATE sm)
add_test(interval_intersects interval_intersects)

# sm::interval
add_executable(interval_adding interval_adding.cpp)
target_link_libraries(interval_adding PRIVATE sm)
add_test(interval_adding interval_adding)

# sm::interval
add_executable(interval1 interval1.cpp)
target_compile_features(interval1 PUBLIC cxx_std_23)
target_link_libraries(interval1 PRIVATE sm)
add_test(interval1 interval1)

# sm::interval
add_executable(interval_constexpr1 interval_constexpr1.cpp)
target_link_libraries(interval_constexpr1 PRIVATE sm)
add_test(interval_constexpr1 interval_constexpr1)

# sm::interval
add_executable(interval_complex1 interval_complex1.cpp)
target_link_libraries(interval_complex1 PRIVATE sm)
add_test(interval_complex1 interval_complex1)

# sm::interval
add_executable(interval_vector1 interval_vector1.cpp)
target_link_libraries(interval_vector1 PRIVATE sm)
add_test(interval_vector1 interval_vector1)

# sm::interval
add_executable(interval_vectorequality interval_vectorequality.cpp)
target_link_libraries(interval_vectorequality PRIVATE sm)
add_test(interval_vectorequality interval_vectorequality)

# sm::random
add_executable(random1 random1.cpp)
target_link_libraries(random1 PRIVATE sm)
add_test(random1 random1)

# Random string generation
add_executable(randstring1 randstring1.cpp)
target_link_libraries(randstring1 PRIVATE sm)
add_test(randstring1 randstring1)

# test sm::vec.
add_executable(vec1 vec1.cpp)
target_link_libraries(vec1 PRIVATE sm)
add_test(vec1 vec1)

if(NOT APPLE) # Could review constexpr on Mac for up to date Mac OS 26+
  # constexpr functionality in sm::vec
  add_executable(vec_constexpr1 vec_constexpr1.cpp)
  target_link_libraries(vec_constexpr1 PRIVATE sm)
  add_test(vec_constexpr1 vec_constexpr1)
endif()

add_executable(vec_casting1 vec_casting1.cpp)
target_link_libraries(vec_casting1 PRIVATE sm)
add_test(vec_casting1 vec_casting1)

add_executable(vec_asmapkey1 vec_asmapkey1.cpp)
target_link_libraries(vec_asmapkey1 PRIVATE sm)
add_test(vec_asmapkey1 vec_asmapkey1)

add_executable(vec_asunordmapkey1 vec_asunordmapkey1.cpp)
target_link_libraries(vec_asunordmapkey1 PRIVATE sm)
add_test(vec_asunordmapkey1 vec_asunordmapkey1)

add_executable(array_asmapkey1 array_asmapkey1.cpp)
target_link_libraries(array_asmapkey1 PRIVATE sm)
add_test(array_asmapkey1 array_asmapkey1)

# test sm::vec elemental operations
add_executable(vec_elementops1 vec_elementops1.cpp)
target_link_libraries(vec_elementops1 PRIVATE sm)
add_test(vec_elementops1 vec_elementops1)

add_executable(vec_set_from1 vec_set_from1.cpp)
target_link_libraries(vec_set_from1 PRIVATE sm)
add_test(vec_set_from1 vec_set_from1)

# Test the "mathematic vector made from an std::vector" code (this vvec is better for neural networks)
add_executable(vvec1 vvec1.cpp)
target_link_libraries(vvec1 PRIVATE sm)
add_test(vvec1 vvec1)

add_executable(vvec_ofvecs1 vvec_ofvecs1.cpp)
target_link_libraries(vvec_ofvecs1 PRIVATE sm)
add_test(vvec_ofvecs1 vvec_ofvecs1)

add_executable(vvec_operatormult1 vvec_operatormult1.cpp)
target_link_libraries(vvec_operatormult1 PRIVATE sm)
add_test(vvec_operatormult1 vvec_operatormult1)

add_executable(vvec_operatordiv1 vvec_operatordiv1.cpp)
target_link_libraries(vvec_operatordiv1 PRIVATE sm)
add_test(vvec_operatordiv1 vvec_operatordiv1)

add_executable(vvec_operatoradd1 vvec_operatoradd1.cpp)
target_link_libraries(vvec_operatoradd1 PRIVATE sm)
add_test(vvec_operatoradd1 vvec_operatoradd1)

add_executable(vvec_convolutions1 vvec_convolutions1.cpp)
target_link_libraries(vvec_convolutions1 PRIVATE sm)
add_test(vvec_convolutions1 vvec_convolutions1)

add_executable(vvec_convolutions2 vvec_convolutions2.cpp)
target_link_libraries(vvec_convolutions2 PRIVATE sm)
add_test(vvec_convolutions2 vvec_convolutions2)

add_executable(vvec_differentiation1 vvec_differentiation1.cpp)
target_link_libraries(vvec_differentiation1 PRIVATE sm)
add_test(vvec_differentiation1 vvec_differentiation1)

add_executable(vvec_zerocross1 vvec_zerocross1.cpp)
target_link_libraries(vvec_zerocross1 PRIVATE sm)
add_test(vvec_zerocross1 vvec_zerocross1)

add_executable(vvec_prune1 vvec_prune1.cpp)
target_link_libraries(vvec_prune1 PRIVATE sm)
add_test(vvec_prune1 vvec_prune1)

add_executable(vvec_threshold1 vvec_threshold1.cpp)
target_link_libraries(vvec_threshold1 PRIVATE sm)
add_test(vvec_threshold1 vvec_threshold1)

add_executable(vvec_elementcompare1 vvec_elementcompare1.cpp)
target_link_libraries(vvec_elementcompare1 PRIVATE sm)
add_test(vvec_elementcompare1 vvec_elementcompare1)

add_executable(vvec_rescale1 vvec_rescale1.cpp)
target_link_libraries(vvec_rescale1 PRIVATE sm)
add_test(vvec_rescale1 vvec_rescale1)

add_executable(vvec_set_from1 vvec_set_from1.cpp)
target_link_libraries(vvec_set_from1 PRIVATE sm)
add_test(vvec_set_from1 vvec_set_from1)

add_executable(vvec_at_signed1 vvec_at_signed1.cpp)
target_link_libraries(vvec_at_signed1 PRIVATE sm)
add_test(vvec_at_signed1 vvec_at_signed1)

add_executable(vvec_nans1 vvec_nans1.cpp)
target_link_libraries(vvec_nans1 PRIVATE sm)
add_test(vvec_nans1 vvec_nans1)

add_executable(vvec_range_extent1 vvec_range_extent1.cpp)
target_link_libraries(vvec_range_extent1 PRIVATE sm)
add_test(vvec_range_extent1 vvec_range_extent1)

add_executable(vvec_arange vvec_arange.cpp)
target_link_libraries(vvec_arange PRIVATE sm)
add_test(vvec_arange vvec_arange)

# Test the scaling code
add_executable(scale1 scale1.cpp)
target_link_libraries(scale1 PRIVATE sm)
add_test(scale1 scale1)

# Test the Scaling of vecs
add_executable(scale_vector1 scale_vector1.cpp)
target_link_libraries(scale_vector1 PRIVATE sm)
add_test(scale_vector1 scale_vector1)

# Test scaling complex numbers
add_executable(scale_complex1 scale_complex1.cpp)
target_link_libraries(scale_complex1 PRIVATE sm)
add_test(scale_complex1 scale_complex1)

if(HDF5_FOUND
    AND NOT CMAKE_CXX_COMPILER_ID MATCHES MSVC
    AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15.0))

  # Make an additional library that links in libhdf
  add_library (sm_hdfdata SHARED)
  target_sources_modules(sm_hdfdata MODULES ${SM_HDFDATA_MODULES})
  target_link_libraries(sm_hdfdata ${HDF5_C_LIBRARIES})

  # Test HDF file access
  add_executable(hdfdata1 hdfdata1.cpp)
  target_link_libraries (hdfdata1 PRIVATE sm_hdfdata)
  add_test(hdfdata1 hdfdata1)

  add_executable(hdfdata2 hdfdata2.cpp)
  target_link_libraries (hdfdata2 PRIVATE sm_hdfdata)
  add_test(hdfdata2 hdfdata2)

  add_executable(hdfdata3 hdfdata3.cpp)
  target_compile_definitions(hdfdata3 PUBLIC FLT=float)
  target_link_libraries (hdfdata3 PRIVATE sm_hdfdata)
  add_test(hdfdata3 hdfdata3)

  add_executable(hdfdata3d hdfdata3.cpp)
  target_compile_definitions(hdfdata3d PUBLIC FLT=double)
  target_link_libraries (hdfdata3d PRIVATE sm_hdfdata)
  add_test(hdfdata3d hdfdata3d)

  add_executable(hdfdata4 hdfdata4.cpp)
  target_compile_definitions(hdfdata4 PUBLIC FLT=float)
  target_link_libraries (hdfdata4 PRIVATE sm_hdfdata)
  add_test(hdfdata4 hdfdata4)

  add_executable(hdfdata5 hdfdata5.cpp)
  target_link_libraries (hdfdata5 PRIVATE sm_hdfdata)
  add_test(hdfdata5 hdfdata5)
endif()

# Test sm::quaternion
add_executable(quaternion1 quaternion1.cpp)
target_link_libraries(quaternion1 PRIVATE sm)
add_test(quaternion1 quaternion1)

if(NOT APPLE)
  add_executable(quaternion_constexpr1 quaternion_constexpr1.cpp)
  target_link_libraries(quaternion_constexpr1 PRIVATE sm)
  add_test(quaternion_constexpr1 quaternion_constexpr1)
endif()

add_executable(quaternion_rotations_float quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_float PUBLIC FLT=float)
target_link_libraries(quaternion_rotations_float PRIVATE sm)
add_test(quaternion_rotations_float quaternion_rotations_float)

add_executable(quaternion_rotations_double quaternion_rotations.cpp)
target_compile_definitions(quaternion_rotations_double PUBLIC FLT=double)
target_link_libraries(quaternion_rotations_double PRIVATE sm)
add_test(quaternion_rotations_double quaternion_rotations_double)

add_executable(mat_matrixeqns mat_matrixeqns.cpp)
target_link_libraries(mat_matrixeqns PRIVATE sm)
add_test(mat_matrixeqns mat_matrixeqns)

add_executable(mat_non_square mat_non_square.cpp)
target_link_libraries(mat_non_square PRIVATE sm)
add_test(mat_non_square mat_non_square)

add_executable(mat_2x2_general mat_2x2_general.cpp)
target_link_libraries(mat_2x2_general PRIVATE sm)
add_test(mat_2x2_general mat_2x2_general)

add_executable(mat_3x3_basic mat_3x3_basic.cpp)
target_link_libraries(mat_3x3_basic PRIVATE sm)
add_test(mat_3x3_basic mat_3x3_basic)

add_executable(mat_3x3_general mat_3x3_general.cpp)
target_link_libraries(mat_3x3_general PRIVATE sm)
add_test(mat_3x3_general mat_3x3_general)

# Test sm::mat<T, 4> (4x4 matrix)
add_executable(mat_4x4 mat_4x4.cpp)
target_link_libraries(mat_4x4 PRIVATE sm)
add_test(mat_4x4 mat_4x4)

add_executable(mat_formatting mat_formatting.cpp)
target_link_libraries(mat_formatting PRIVATE sm)
add_test(mat_formatting mat_formatting)

add_executable(mat_4x4_translate mat_4x4_translate.cpp)
target_link_libraries(mat_4x4_translate PRIVATE sm)
add_test(mat_4x4_translate mat_4x4_translate)

add_executable(mat_4x4_rotation mat_4x4_rotation.cpp)
target_link_libraries(mat_4x4_rotation PRIVATE sm)
add_test(mat_4x4_rotation mat_4x4_rotation)

add_executable(mat_4x4_basic mat_4x4_basic.cpp)
target_link_libraries(mat_4x4_basic PRIVATE sm)
add_test(mat_4x4_basic mat_4x4_basic)

add_executable(mat_4x4_basic_complex mat_4x4_basic_complex.cpp)
target_link_libraries(mat_4x4_basic_complex PRIVATE sm)
add_test(mat_4x4_basic_complex mat_4x4_basic_complex)

add_executable(mat_4x4_basic2 mat_4x4_basic2.cpp)
target_link_libraries(mat_4x4_basic2 PRIVATE sm)
add_test(mat_4x4_basic2 mat_4x4_basic2)

add_executable(mat_4x4_reflection mat_4x4_reflection.cpp)
target_link_libraries(mat_4x4_reflection PRIVATE sm)
add_test(mat_4x4_reflection mat_4x4_reflection)

add_executable(mat_4x4_eigenvalues mat_4x4_eigenvalues.cpp)
target_link_libraries(mat_4x4_eigenvalues PRIVATE sm)
add_test(mat_4x4_eigenvalues mat_4x4_eigenvalues)

if(0)
if(NOT APPLE)
  # constexpr mat
  add_executable(mat_4x4_constexpr mat_4x4_constexpr.cpp)
  target_link_libraries(mat_4x4_constexpr PRIVATE sm)
  add_test(mat_4x4_constexpr mat_4x4_constexpr)

  if(Eigen3_FOUND)
    add_executable(mat_4x4_vseigen mat_4x4_vseigen.cpp)
    target_include_directories (mat_4x4_vseigen BEFORE PRIVATE ${EIGEN3_INCLUDE_DIR})
    target_link_libraries(mat_4x4_vseigen PRIVATE sm)
    add_test(mat_4x4_vseigen mat_4x4_vseigen)
  endif(Eigen3_FOUND)
endif()
endif()

# Test quaternion and mat44 rotations in float and double precision
add_executable(rotations1_float rotations1.cpp)
target_compile_definitions(rotations1_float PUBLIC FLT=float)
target_link_libraries(rotations1_float PRIVATE sm)
add_test(rotations1_float rotations1_float)

add_executable(rotations1_double rotations1.cpp)
target_compile_definitions(rotations1_double PUBLIC FLT=double)
target_link_libraries(rotations1_double PRIVATE sm)
add_test(rotations1_double rotations1_double)

# More rotations
add_executable(rotations2 rotations2.cpp)
target_link_libraries(rotations2 PRIVATE sm)
add_test(rotations2 rotations2)

# Spline interpolation
add_executable(spline1 spline1.cpp)
target_link_libraries(spline1 PRIVATE sm)
add_test(spline1 spline1)

add_executable(test_trait_tests test_trait_tests.cpp)
target_link_libraries(test_trait_tests PRIVATE sm)
add_test(test_trait_tests test_trait_tests)

# Test Hex::get_vertex_coord()
add_executable(hex_vertexpos1 hex_vertexpos1.cpp)
target_link_libraries(hex_vertexpos1 PRIVATE sm)
add_test(hex_vertexpos1 hex_vertexpos1)

# Test Hex user flags code
add_executable(hex_userflags1 hex_userflags1.cpp)
target_link_libraries(hex_userflags1 PRIVATE sm)

# An example program which is slow compiled on Visual Studio with the /MDd "Debug build" flag
add_executable(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase.cpp)
add_test(hex_profile_list_hex_min_erase hex_profile_list_hex_min_erase)

# Compile with stdc++ lib debug mode (but don't run as a test, as it's
# very slow) This is equivalent to the default debug compilation that
# Visual Studio uses (/MDd or /MTd).
add_executable(hex_profile_list_hex_min_erase_dbg hex_profile_list_hex_min_erase.cpp)
target_compile_definitions(hex_profile_list_hex_min_erase_dbg PUBLIC _GLIBCXX_DEBUG)

add_executable(winder1 winder1.cpp)
target_link_libraries(winder1 PRIVATE sm)
add_test(winder1 winder1)

add_executable(winder2 winder2.cpp)
target_link_libraries(winder2 PRIVATE sm)
add_test(winder2 winder2)

add_executable(winder3 winder3.cpp)
target_link_libraries(winder3 PRIVATE sm)
add_test(winder3 winder3)

# Test bootstrapping
add_executable(bootstrap1 bootstrap1.cpp)
target_link_libraries(bootstrap1 PRIVATE sm)
# Test disabled - statistical fluctuations can make this fail sometimes
# add_test(bootstrap1 bootstrap1)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  add_executable(grid1 grid1.cpp)
  target_link_libraries(grid1 PRIVATE sm)
  add_test(grid1 grid1)

  add_executable(grid_suggest_dims1 grid_suggest_dims1.cpp)
  target_link_libraries(grid_suggest_dims1 PRIVATE sm)
  add_test(grid_suggest_dims1 grid_suggest_dims1)

  add_executable(grid_rowcol1 grid_rowcol1.cpp)
  target_link_libraries(grid_rowcol1 PRIVATE sm)
  add_test(grid_rowcol1 grid_rowcol1)

  add_executable(grid_shiftindex1 grid_shiftindex1.cpp)
  target_link_libraries(grid_shiftindex1 PRIVATE sm)
  add_test(grid_shiftindex1 grid_shiftindex1)

  add_executable(grid_indexlookup1 grid_indexlookup1.cpp)
  target_link_libraries(grid_indexlookup1 PRIVATE sm)
  add_test(grid_indexlookup1 grid_indexlookup1)

  add_executable(grid_neighbours1 grid_neighbours1.cpp)
  target_link_libraries(grid_neighbours1 PRIVATE sm)
  add_test(grid_neighbours1 grid_neighbours1)

  add_executable(grid_getabscissae1 grid_getabscissae1.cpp)
  target_link_libraries(grid_getabscissae1 PRIVATE sm)
  add_test(grid_getabscissae1 grid_getabscissae1)

  add_executable(grid_profile1 grid_profile1.cpp)
  target_link_libraries(grid_profile1 PRIVATE sm)
  add_test(grid_profile1 grid_profile1)
endif()

# Test Nelder Mead algorithm
add_executable(nmsimplex1 nmsimplex1.cpp)
target_compile_definitions(nmsimplex1 PUBLIC FLT=float)
target_link_libraries(nmsimplex1 PRIVATE sm)
add_test(nmsimplex1 nmsimplex1)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  add_executable(histo1 histo1.cpp)
  target_link_libraries(histo1 PRIVATE sm)
  add_test(histo1 histo1)

  add_executable(boxfilter1 boxfilter1.cpp)
  target_link_libraries(boxfilter1 PRIVATE sm)
  add_test(boxfilter1 boxfilter1)
endif()

# Test sm::config

# GCC < version 16.1 can't compile sm::config (something to do with the nlohmann::json module). It also seems to be an issue on Windows
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 16.1))
  add_executable(config1 config1.cpp)
  target_sources_modules(config1 MODULES ${SM_CONFIG_MODULES} ${SM_UTIL_MODULES} ${SM_VVEC_MODULES} ${SM_VEC_MODULES})
  add_test(config1 config1)
endif()

add_executable(zernike1 zernike1.cpp)
target_link_libraries(zernike1 PRIVATE sm)
add_test(zernike1 zernike1)

add_executable(algo_1 algo_1.cpp)
target_link_libraries(algo_1 PRIVATE sm)
add_test(algo_1 algo_1)

add_executable(algo_sigfigs1 algo_sigfigs1.cpp)
target_link_libraries(algo_sigfigs1 PRIVATE sm)
add_test(algo_sigfigs1 algo_sigfigs1)

add_executable(algo_roundtocol1 algo_roundtocol1.cpp)
target_link_libraries(algo_roundtocol1 PRIVATE sm)
add_test(algo_roundtocol1 algo_roundtocol1)

add_executable(algo_ransac algo_ransac.cpp)
target_link_libraries(algo_ransac PRIVATE sm)
add_test(algo_ransac algo_ransac)

if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  add_executable(geometry_2 geometry_2.cpp)
  target_link_libraries(geometry_2 PRIVATE sm)
  add_test(geometry_2 geometry_2)

  add_executable(geometry_3 geometry_3.cpp)
  target_link_libraries(geometry_3 PRIVATE sm)
  add_test(geometry_3 geometry_3)

  add_executable(geometry_4 geometry_4.cpp)
  target_link_libraries(geometry_4 PRIVATE sm)
  add_test(geometry_4 geometry_4)

  add_executable(geometry_5 geometry_5.cpp)
  target_link_libraries(geometry_5 PRIVATE sm)
  add_test(geometry_5 geometry_5)

  add_executable(geometry_6 geometry_6.cpp)
  target_link_libraries(geometry_6 PRIVATE sm)
  add_test(geometry_6 geometry_6)

  add_executable(geometry_xyz_to_latlong geometry_xyz_to_latlong.cpp)
  target_link_libraries(geometry_xyz_to_latlong PRIVATE sm)
  add_test(geometry_xyz_to_latlong geometry_xyz_to_latlong)

  add_executable(geometry_sph_projections geometry_sph_projections.cpp)
  target_link_libraries(geometry_sph_projections PRIVATE sm)
  add_test(geometry_sph_projections geometry_sph_projections)

  add_executable(graham_scan_1 graham_scan_1.cpp)
  target_link_libraries(graham_scan_1 PRIVATE sm)
  add_test(graham_scan_1 graham_scan_1)
endif()

add_executable(util1 util1.cpp)
target_link_libraries(util1 PRIVATE sm)
add_test(util1 util1)

add_executable(trait_tests_number_type trait_tests_number_type.cpp)
target_link_libraries(trait_tests_number_type PRIVATE sm)
add_test(trait_tests_number_type trait_tests_number_type)

add_executable(trait_tests_has_size trait_tests_has_size.cpp)
target_link_libraries(trait_tests_has_size PRIVATE sm)
add_test(trait_tests_has_size trait_tests_has_size)

add_executable(flags1 flags1.cpp)
target_link_libraries (flags1 PRIVATE sm)
add_test(flags1 flags1)

if(NOT APPLE)
  add_executable(constexpr_math1 constexpr_math1.cpp)
  target_link_libraries(constexpr_math1 PRIVATE sm)
  add_test(constexpr_math1 constexpr_math1)
endif()

# vvec<vec> warning about attributes in gcc14
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 14.99))
  # BezCurve class
  set(TARGETTEST1 bez1)
  set(SOURCETEST1 bez1.cpp)
  add_executable(${TARGETTEST1} ${SOURCETEST1})
  target_link_libraries(bez1 PRIVATE sm)
  add_test(bez1 ${TARGETTEST1})

  # BezCurve class (uses hexgrid)
  add_executable(bez2 bez2.cpp)
  target_link_libraries(bez2 PRIVATE sm)
  add_test(bez2 bez2)

  # BezCurvePath class
  add_executable(bezcurves bezcurves.cpp)
  target_link_libraries(bezcurves PRIVATE sm)
  add_test(bezcurves bezcurves)

  # Testing matrix representation of Bezier curves
  add_executable(bezmatrix bezmatrix.cpp)
  target_link_libraries(bezmatrix PRIVATE sm)
  target_compile_definitions(bezmatrix PUBLIC FLT=float)
  add_test(bezmatrix bezmatrix)

  # Testing auto-fitting a Bezier curve
  add_executable(bezfit bezfit.cpp)
  target_link_libraries(bezfit PRIVATE sm)
  target_compile_definitions(bezfit PUBLIC FLT=float)
  add_test(bezfit bezfit)

  # Testing splitting of Bezier curves
  add_executable(bezsplit bezsplit.cpp)
  target_link_libraries(bezsplit PRIVATE sm)
  target_compile_definitions(bezsplit PUBLIC FLT=float)
  add_test(bezsplit bezsplit)

  # Test two curves connected together
  add_executable(twocurves twocurves.cpp)
  target_link_libraries(twocurves PRIVATE sm)
  add_test(twocurves twocurves)

  # Inserting into a list. error: conflicting declaration of template. on g++-14
  add_executable(cartgrid1 cartgrid1.cpp)
  target_link_libraries(cartgrid1 PRIVATE sm)
  add_test(cartgrid1 cartgrid1)

  add_executable(cartgrid_gridshiftcoords cartgrid_gridshiftcoords.cpp)
  target_link_libraries(cartgrid_gridshiftcoords PRIVATE sm)
  add_test(cartgrid_gridshiftcoords cartgrid_gridshiftcoords)

  add_executable(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric.cpp)
  target_link_libraries(cartgrid_shiftindicesbymetric PRIVATE sm)
  add_test(cartgrid_shiftindicesbymetric cartgrid_shiftindicesbymetric)
endif()

add_executable(polysolve_1 polysolve_1.cpp)
target_link_libraries(polysolve_1 PRIVATE sm)
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
  add_executable(mat_succeed_4x4timesv3 mat_succeed_4x4timesv3.cpp)
  target_sources_modules(mat_succeed_4x4timesv3 MODULES ${SM_MAT_MODULES})
  add_test (mat_succeed_4x4timesv3 mat_succeed_4x4timesv3)

  # Matrix times vector is generally ok, as long as vector is right size
  add_executable(mat_succeed_4x4timesv4 mat_succeed_4x4timesv4.cpp)
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

  add_executable(mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16.cpp)
  target_sources_modules(mat_succeed_4x4initwithv16 MODULES ${SM_MAT_MODULES} ${SM_VEC_MODULES})
  add_test (mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16)

  #
  # mat = vec (allow for same-size vec)
  #
  add_executable(mat_succeed_assignvec mat_succeed_assignvec.cpp)
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
