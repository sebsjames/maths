// -*- C++ -*-
#
# This file controls the installation of the header files.
#

# Header installation (list from ls * -1, then re-ordered into functionality groups)
install(
  FILES
  base64
  bootstrap
  constexpr_math
  crc32
  flags
  geometry
  gridfeatures
  grid
  hdfdata
  hexgrid
  hex
  hexyhisto
  histo
  mat22
  mat33
  mat44
  MathAlgo
  mathconst
  algo
  boxfilter
  MathImpl
  nm_simplex
  quaternion
  random
  range
  rect
  scale
  trait_tests
  vec
  vvec
  winder

  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/sj
)

# There are also headers in sub directories
#add_subdirectory(nn) # 'nn' for neural network code
#add_subdirectory(bn) # 'bn' for boolean net code

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
