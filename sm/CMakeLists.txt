#
# This file controls the installation of the header files.
#

# Header installation (list from ls * -1, then re-ordered into functionality groups)
install(
  FILES
  algo.cppm
  algo_hexgrid.cppm
  anneal.cppm
  base64.cppm
  bessel_i0.hpp
  bezcurvepath.cppm
  bezcoord.cppm
  bezcurve.cppm
  binomial.cppm
  bootstrap.cppm
  boxfilter.cppm
  cartgrid.cppm
  centroid.cppm
  constexpr_math.hpp
  crc32.cppm
  edgeconv.cppm
  evenspacing.hpp
  fft.cppm
  flags.cppm
  geometry.cppm
  geometry_polyhedra.cppm
  grid.cppm
  healpix_bare.cppm
  hex.cppm
  hexfft.cppm
  hexgrid.cppm
  hexyhisto.cppm
  histo.cppm
  interval.hpp
  jc_voronoi.cppm
  mat.cppm
  mathconst.hpp
  nm_simplex.cppm
  onoff.cppm
  pca.cppm
  polysolve.hpp
  quaternion.cppm
  random.hpp
  random_walk.cppm
  rect.cppm
  rungekutta4.cppm
  scale.hpp
  spline.cppm
  trait_tests.hpp
  util.cppm
  vec.hpp
  vmat.hpp
  vvec.hpp
  winder.cppm

  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/sm
)

# Install the EXPORT so that maths has its own .cmake file and find_package(maths) should work
install(FILES maths-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/maths)
