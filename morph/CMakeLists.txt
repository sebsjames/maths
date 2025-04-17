#
# This file controls the installation of the header files.
#

# Header installation (list from ls *.h* -1, then re-ordered into functionality groups)
install(
  FILES
  bootstrap.h
  constexpr_math.h
#  flags.h
#  geometry.h
#  Gridct.h
  gridfeatures.h
  grid.h
  HdfData.h
#  HexGrid.h
#  Hex.h
#  hexyhisto.h
  histo.h
  mat22.h
  mat33.h
  mat44.h
  MathAlgo.h
  mathconst.h
  algo.h
  boxfilter.h
  MathImpl.h
#  NM_Simplex.h
  quaternion.h
  random.h
  range.h
#  ReadCurves.h
#  Rect.h
  scale.h
#  ShapeAnalysis.h
#  tools.h
  trait_tests.h
  vec.h
  vvec.h
#  Winder.h

  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/morph
)

# There are also headers in sub directories
#add_subdirectory(nn) # 'nn' for neural network code
#add_subdirectory(bn) # 'bn' for boolean net code

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
