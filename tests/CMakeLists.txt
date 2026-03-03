# CTests

# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

set_source_files_properties (
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/random
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/vvec
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
  ${PROJECT_SOURCE_DIR}/sm/random
)
add_test(vec1 vec1)

if(NOT APPLE)
  # constexpr functionality in sm::vec
  # Need C++20 for the way I use constexpr here so that it is legal to
  # have "a definition of a variable for which no initialization is
  # performed".
  add_executable(vec_constexpr1 vec_constexpr1.cpp)
  target_sources(vec_constexpr1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES
    ${PROJECT_SOURCE_DIR}/sm/range
    ${PROJECT_SOURCE_DIR}/sm/vec
    ${PROJECT_SOURCE_DIR}/sm/random
  )
  add_test(vec_constexpr1 vec_constexpr1)
endif()

add_executable(vec_casting1 vec_casting1.cpp)
target_sources(vec_casting1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
)
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
  ${PROJECT_SOURCE_DIR}/sm/random
)
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
  ${PROJECT_SOURCE_DIR}/sm/vvec
)
add_test(vec_elementops1 vec_elementops1)

add_executable(vec_set_from1 vec_set_from1.cpp)
target_sources(vec_set_from1 PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${PROJECT_SOURCE_DIR}
  FILES
  ${PROJECT_SOURCE_DIR}/sm/range
  ${PROJECT_SOURCE_DIR}/sm/vec
  ${PROJECT_SOURCE_DIR}/sm/random
)
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

# Test the scaling code
add_executable(testScale testScale.cpp)
add_test(testScale testScale)


if (0) # DEVELOPMENT

if(HDF5_FOUND)
  # Test HDF file access
  add_executable(testhdfdata1 testhdfdata1.cpp)
  # Note how if you only use HDF, you only have to link to libhdf
  target_link_libraries(testhdfdata1 ${HDF5_C_LIBRARIES})
  add_test(testhdfdata1 testhdfdata1)

  # More HDF data tests
  add_executable(testhdfdata2 testhdfdata2.cpp)
  target_link_libraries(testhdfdata2 ${HDF5_C_LIBRARIES})
  add_test(testhdfdata2 testhdfdata2)

  add_executable(testhdfdata3f testhdfdata3.cpp)
  target_compile_definitions(testhdfdata3f PUBLIC FLT=float)
  target_link_libraries(testhdfdata3f ${HDF5_C_LIBRARIES})
  add_test(testhdfdata3f testhdfdata3f)

  add_executable(testhdfdata3d testhdfdata3.cpp)
  target_compile_definitions(testhdfdata3d PUBLIC FLT=float)
  target_link_libraries(testhdfdata3d ${HDF5_C_LIBRARIES})
  add_test(testhdfdata3d testhdfdata3d)

  add_executable(testhdfdata4f testhdfdata4.cpp)
  target_compile_definitions(testhdfdata4f PUBLIC FLT=float)
  target_link_libraries(testhdfdata4f ${HDF5_C_LIBRARIES})
  add_test(testhdfdata4f testhdfdata4f)

  add_executable(testhdfdata5 testhdfdata5.cpp)
  target_link_libraries(testhdfdata5 ${HDF5_C_LIBRARIES})
  add_test(testhdfdata5 testhdfdata5)

endif(HDF5_FOUND)

# Test sm::quaternion
add_executable(testQuaternion testQuaternion.cpp)
add_test(testQuaternion testQuaternion)

# Test quaternion and mat44 rotations in float and double precision
add_executable(testRotations_float testRotations.cpp)
target_compile_definitions(testRotations_float PUBLIC FLT=float)
add_test(testRotations_float testRotations_float)

add_executable(testRotations_double testRotations.cpp)
target_compile_definitions(testRotations_double PUBLIC FLT=double)
add_test(testRotations_double testRotations_double)

add_executable(testRotationsQuat_float testRotationsQuat.cpp)
target_compile_definitions(testRotationsQuat_float PUBLIC FLT=float)
add_test(testRotationsQuat_float testRotationsQuat_float)

add_executable(testRotationsQuat_double testRotationsQuat.cpp)
target_compile_definitions(testRotationsQuat_double PUBLIC FLT=double)
add_test(testRotationsQuat_double testRotationsQuat_double)

# More rotations
add_executable(rotations rotations.cpp)
add_test(rotations rotations)

if(NOT APPLE)
  add_executable(testQuat_constexpr testQuat_constexpr.cpp)
  add_test(testQuat_constexpr testQuat_constexpr)
endif()

add_executable(test_trait_tests test_trait_tests.cpp)
add_test(test_trait_tests test_trait_tests)

add_executable(testarange testarange.cpp)
add_test(testarange testarange)

# Test the Scaling of vecs
add_executable(testScaleVector testScaleVector.cpp)
add_test(testScaleVector testScaleVector)

# Test scaling complex numbers
add_executable(testScale_complex testScale_complex.cpp)
add_test(testScale_complex testScale_complex)

# Test Hex::get_vertex_coord()
add_executable(testHexVertexPos testHexVertexPos.cpp)
add_test(testHexVertexPos testHexVertexPos)

# Test Hex user flags code
add_executable(testHexUserFlags testHexUserFlags.cpp)
add_test(testHexUserFlags testHexUserFlags)

# An example program which is slow compiled on Visual Studio with the /MDd "Debug build" flag
add_executable(profileListHexminErase profileListHexminErase.cpp)
add_test(profileListHexminErase profileListHexminErase)

# Compile with stdc++ lib debug mode (but don't run as a test, as it's
# very slow) This is equivalent to the default debug compilation that
# Visual Studio uses (/MDd or /MTd).
add_executable(profileListHexminEraseDbg profileListHexminErase.cpp)
target_compile_definitions(profileListHexminEraseDbg PUBLIC _GLIBCXX_DEBUG)

# Test Nelder Mead algorithm
#add_executable(testNMSimplex testNMSimplex.cpp)
#target_compile_definitions(testNMSimplex PUBLIC FLT=float)
#add_test(testNMSimplex testNMSimplex)

# Test winding number code
add_executable(winder1 winder1.cpp)
add_test(winder1 winder1)

add_executable(winder2 winder2.cpp)
add_test(winder2 winder2)

add_executable(winder3 winder3.cpp)
add_test(winder3 winder3)

# Test bootstrapping
add_executable(testbootstrap testbootstrap.cpp)
# Test disabled - statistical fluctuations can make this fail sometimes
# add_test(testbootstrap testbootstrap)

##add_executable(testboxfilter testboxfilter.cpp)
##add_test(testboxfilter testboxfilter)

add_executable(testGrid testGrid.cpp)
add_test(testGrid testGrid)

add_executable(testGrid_suggest_dims testGrid_suggest_dims.cpp)
add_test(testGrid_suggest_dims testGrid_suggest_dims)

add_executable(testGridRowCol testGridRowCol.cpp)
add_test(testGridRowCol testGridRowCol)

add_executable(testGrid_shiftindex testGrid_shiftindex.cpp)
add_test(testGrid_shiftindex testGrid_shiftindex)

add_executable(testGridIndexLookup testGridIndexLookup.cpp)
add_test(testGridIndexLookup testGridIndexLookup)

add_executable(testGridNeighbours testGridNeighbours.cpp)
add_test(testGridNeighbours testGridNeighbours)

add_executable(testGrid_getabscissae testGrid_getabscissae.cpp)
add_test(testGrid_getabscissae testGrid_getabscissae)

add_executable(testGrid_profile testGrid_profile.cpp)
add_test(testGrid_profile testGrid_profile)

add_executable(test_histo test_histo.cpp)
add_test(test_histo test_histo)

add_executable(test_number_type test_number_type.cpp)
add_test(test_number_type test_number_type)

add_executable(test_flags test_flags.cpp)
add_test(test_flags test_flags)

if(NOT APPLE)
add_executable(testcmath testcmath.cpp)
add_test(testcmath testcmath)
endif()

add_executable(testsigfigs testsigfigs.cpp)
add_test(testsigfigs testsigfigs)

add_executable(testroundtocol testroundtocol.cpp)
add_test(testroundtocol testroundtocol)

if(ARMADILLO_FOUND)
  # BezCurve class
  set(TARGETTEST1 testbez)
  set(SOURCETEST1 testbez.cpp)
  add_executable(${TARGETTEST1} ${SOURCETEST1})
  target_link_libraries(${TARGETTEST1} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbez ${TARGETTEST1})

  # BezCurve class
  set(TARGETTEST1_1 testbez2)
  set(SOURCETEST1_1 testbez2.cpp)
  add_executable(${TARGETTEST1_1} ${SOURCETEST1_1})
  target_link_libraries(${TARGETTEST1_1} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbez2 ${TARGETTEST1_1})

  # BezCurvePath class
  set(TARGETTEST1_2 testbezcurves)
  set(SOURCETEST1_2 testbezcurves.cpp)
  add_executable(${TARGETTEST1_2} ${SOURCETEST1_2})
  target_link_libraries(${TARGETTEST1_2} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbezcurves ${TARGETTEST1_2})

  # Testing matrix representation of Bezier curves
  set(TARGETTEST1_3 testbezmatrix)
  set(SOURCETEST1_3 testbezmatrix.cpp)
  add_executable(${TARGETTEST1_3} ${SOURCETEST1_3})
  # Disable warning -Wunused-but-set-variable:
  #target_compile_options(${TARGETTEST1_3} PRIVATE -Wno-unused-but-set-variable)
  target_compile_definitions(${TARGETTEST1_3} PUBLIC FLT=float)
  target_link_libraries(${TARGETTEST1_3} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbezmatrix ${TARGETTEST1_3})

  # Testing auto-fitting a Bezier curve
  set(TARGETTEST1_4 testbezfit)
  set(SOURCETEST1_4 testbezfit.cpp)
  add_executable(${TARGETTEST1_4} ${SOURCETEST1_4})
  target_compile_definitions(${TARGETTEST1_4} PUBLIC FLT=float)
  target_link_libraries(${TARGETTEST1_4} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbezfit ${TARGETTEST1_4})

  # Testing splitting of Bezier curves
  set(TARGETTEST1_5 testbezsplit)
  set(SOURCETEST1_5 testbezsplit.cpp)
  add_executable(${TARGETTEST1_5} ${SOURCETEST1_5})
  target_compile_definitions(${TARGETTEST1_5} PUBLIC FLT=float)
  target_link_libraries(${TARGETTEST1_5} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(testbezsplit ${TARGETTEST1_5})

  # Test two curves connected together
  set(TARGETTEST2 twocurves)
  set(SOURCETEST2 twocurves.cpp)
  add_executable(${TARGETTEST2} ${SOURCETEST2})
  target_link_libraries(${TARGETTEST2} ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES})
  add_test(twocurves ${TARGETTEST2})
endif()

add_executable(testcartgrid testcartgrid.cpp)
add_test(testcartgrid testcartgrid)

add_executable(testCartGridShiftCoords testCartGridShiftCoords.cpp)
add_test(testCartGridShiftCoords testCartGridShiftCoords)

add_executable(testCartGridShiftIndiciesByMetric testCartGridShiftIndiciesByMetric.cpp)
add_test(testCartGridShiftIndiciesByMetric testCartGridShiftIndiciesByMetric)

# Test sm::config
add_executable(testConfig testConfig.cpp)
add_test(testConfig testConfig)

add_executable(test_zernike test_zernike.cpp)
add_test(test_zernike test_zernike)

add_executable(mat_2x2_general mat_2x2_general.cpp)
add_test(mat_2x2_general mat_2x2_general)

add_executable(mat_3x3_basic mat_3x3_basic.cpp)
add_test(mat_3x3_basic mat_3x3_basic)

add_executable(mat_3x3_general mat_3x3_general.cpp)
add_test(mat_3x3_general mat_3x3_general)

# Test sm::mat<T, 4> (4x4 matrix)
add_executable(mat_4x4 mat_4x4.cpp)
add_test(mat_4x4 mat_4x4)

add_executable(mat_4x4_translate mat_4x4_translate.cpp)
add_test(mat_4x4_translate mat_4x4_translate)

add_executable(mat_4x4_basic2 mat_4x4_basic2.cpp)
add_test(mat_4x4_basic2 mat_4x4_basic2)

add_executable(mat_4x4_reflection mat_4x4_reflection.cpp)
add_test(mat_4x4_reflection mat_4x4_reflection)

if(NOT APPLE)
  # constexpr mat44
  add_executable(mat_4x4_constexpr mat_4x4_constexpr.cpp)
  add_test(mat_4x4_constexpr mat_4x4_constexpr)

  if(Eigen3_FOUND)
    add_executable(mat_4x4_vseigen mat_4x4_vseigen.cpp)
    target_include_directories (mat_4x4_vseigen BEFORE PRIVATE ${EIGEN3_INCLUDE_DIR})
    add_test(mat_4x4_vseigen mat_4x4_vseigen)
  endif(Eigen3_FOUND)
endif()

add_executable(algo_1 algo_1.cpp)
add_test(algo_1 algo_1)

add_executable(geometry_2 geometry_2.cpp)
add_test(geometry_2 geometry_2)

add_executable(geometry_3 geometry_3.cpp)
add_test(geometry_3 geometry_3)

add_executable(geometry_4 geometry_4.cpp)
add_test(geometry_4 geometry_4)

add_executable(geometry_5 geometry_5.cpp)
add_test(geometry_5 geometry_5)

add_executable(geometry_6 geometry_6.cpp)
add_test(geometry_6 geometry_6)

add_executable(range_intersects range_intersects.cpp)
add_test(range_intersects range_intersects)

add_executable(range_adding range_adding.cpp)
add_test(range_adding range_adding)

add_executable(xyz_to_latlong xyz_to_latlong.cpp)
add_test(xyz_to_latlong xyz_to_latlong)

add_executable(sph_projections sph_projections.cpp)
add_test(sph_projections sph_projections)

add_executable(graham_scan_1 graham_scan_1.cpp)
add_test(graham_scan_1 graham_scan_1)

add_executable(util1 util1.cpp)
add_test(util1 util1)

#
# mat compile/not compile tests on operators
#

if(NOT WIN32) # exclude Windows CI from doing the compile failure tests for now

  #
  # mat times vec
  #

  # Only 4x4 matrices can be multiplied with an (N-1) D vector
  add_executable(mat_succeed_4x4timesv3 mat_succeed_4x4timesv3.cpp)
  add_test (mat_succeed_4x4timesv3 mat_succeed_4x4timesv3)

  # Matrix times vector is generally ok, as long as vector is right size
  add_executable(mat_succeed_4x4timesv4 mat_succeed_4x4timesv4.cpp)
  add_test (mat_succeed_4x4timesv4 mat_succeed_4x4timesv4)

  # To build add "-test" to the NAME
  icm_add_build_failure_test(
    NAME mat_fail_4x4timesv16
    SOURCES mat_fail_4x4timesv16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat *= vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv3
    SOURCES mat_fail_4x4timeseqlsv3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv4
    SOURCES mat_fail_4x4timeseqlsv4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4timeseqlsv16
    SOURCES mat_fail_4x4timeseqlsv16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat plus vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec3
    SOURCES mat_fail_4x4plusvec3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec4
    SOURCES mat_fail_4x4plusvec4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4plusvec16
    SOURCES mat_fail_4x4plusvec16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat += vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec3
    SOURCES mat_fail_4x4pluseqlsvec3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec4
    SOURCES mat_fail_4x4pluseqlsvec4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4pluseqlsvec16
    SOURCES mat_fail_4x4pluseqlsvec16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat minus vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec3
    SOURCES mat_fail_4x4minusvec3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec4
    SOURCES mat_fail_4x4minusvec4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minusvec16
    SOURCES mat_fail_4x4minusvec16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat -= vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec3
    SOURCES mat_fail_4x4minuseqlsvec3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec4
    SOURCES mat_fail_4x4minuseqlsvec4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4minuseqlsvec16
    SOURCES mat_fail_4x4minuseqlsvec16.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # Init with vec
  #

  icm_add_build_failure_test(
    NAME mat_fail_4x4initwithv3
    SOURCES mat_fail_4x4initwithv3.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_4x4initwithv4
    SOURCES mat_fail_4x4initwithv4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  add_executable(mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16.cpp)
  add_test (mat_succeed_4x4initwithv16 mat_succeed_4x4initwithv16)

  #
  # mat = vec (allow for same-size vec)
  #
  add_executable(mat_succeed_assignvec mat_succeed_assignvec.cpp)
  add_test (mat_succeed_assignvec mat_succeed_assignvec)

  icm_add_build_failure_test(
    NAME mat_fail_assignvec_diffsize
    SOURCES mat_fail_assignvec_diffsize.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat == vec (disallow)
  #

  icm_add_build_failure_test(
    NAME mat_fail_equalityvec
    SOURCES mat_fail_equalityvec.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  icm_add_build_failure_test(
    NAME mat_fail_equalityvec4
    SOURCES mat_fail_equalityvec4.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

  #
  # mat != vec (disallow)
  #

  icm_add_build_failure_test(
    NAME mat_fail_nonequalityvec
    SOURCES mat_fail_nonequalityvec.cpp
    LABELS build-failure
    ERROR_MATCHES "Should not compile"
  )

endif(NOT WIN32)

endif(0) # DEVELOPMENT
