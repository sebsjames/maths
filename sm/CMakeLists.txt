#
# This file controls the installation of the header files.
#

# Header installation (list from ls * -1, then re-ordered into functionality groups)
install(
  FILES
  algo.cppm
  anneal.cppm
  base64.cppm
  bessel_i0.cppm
  bezcurvepath.cppm
  bezcoord.cppm
  bezcurve.cppm
  binomial.cppm
  bootstrap.cppm
  boxfilter.cppm
  cartgrid.cppm
  centroid.cppm
  config.cppm
  constexpr_math.cppm
  crc32.cppm
  edgeconv.cppm
  flags.cppm
  geometry.cppm
  geometry_polyhedra.cppm
  grid.cppm
  hdfdata.cppm
  hex.cppm
  hexgrid.cppm
  hexgrid_hdf.cppm
  hexyhisto.cppm
  histo.cppm
  interval.cppm
  mat.cppm
  mathconst.cppm
  nm_simplex.cppm
  onoff.cppm
  pca.cppm
  polysolve.cppm
  quaternion.cppm
  random.cppm
  random_walk.cppm
  rect.cppm
  scale.cppm
  spline.cppm
  trait_tests.cppm
  util.cppm
  vec.cppm
  vvec.cppm
  winder.cppm

  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sm
)

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
