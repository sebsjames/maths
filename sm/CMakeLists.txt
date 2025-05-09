#
# This file controls the installation of the header files.
#

# Header installation (list from ls * -1, then re-ordered into functionality groups)
install(
  FILES
  algo
  anneal
  base64
  bootstrap
  boxfilter
  config
  constexpr_math
  crc32
  flags
  geometry
  gridfeatures
  grid
  hdfdata
  hex
  hexgrid
  hexyhisto
  histo
  mat22
  mat33
  mat44
  mathconst
  nm_simplex
  process
  quaternion
  random
  range
  rect
  scale
  trait_tests
  vec
  vvec
  winder

  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/sm
)

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
