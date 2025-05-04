#
# This file controls the installation of the header files.
#

# Header installation (list from ls *.h* -1, then re-ordered into functionality groups)
install(
  FILES
  base64.h
  bootstrap.h
  constexpr_math.h
  crc32.h
  flags.h
  geometry.h
  gridfeatures.h
  grid.h
  hdfdata.h
  hexgrid.h
  hex.h
  hexyhisto.h
  histo.h
  mat22.h
  mat33.h
  mat44.h
  MathAlgo.h
  mathconst.h
  algo.h
  boxfilter.h
  MathImpl.h
  nm_simplex.h
  quaternion.h
  random.h
  range.h
  rect.h
  scale.h
  trait_tests.h
  vec.h
  vvec.h
  winder.h

  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/morph
)

# There are also headers in sub directories
#add_subdirectory(nn) # 'nn' for neural network code
#add_subdirectory(bn) # 'bn' for boolean net code

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
