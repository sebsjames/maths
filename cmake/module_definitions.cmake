#
# Define variables of module groups for use by the sebsjames/maths
# build process itself, and by client projects.
#
macro(setup_module_variables_for_maths base_directory json_directory)

  set(SM_MATHCONST_MODULES
    ${base_directory}/sm/mathconst.cppm
  )
  list(REMOVE_DUPLICATES SM_MATHCONST_MODULES)

  set(SM_CONSTEXPR_MATH_MODULES
    ${SM_MATHCONST_MODULES}
    ${base_directory}/sm/constexpr_math.cppm
  )
  list(REMOVE_DUPLICATES SM_CONSTEXPR_MATH_MODULES)

  set(SM_POLYSOLVE_MODULES
    ${SM_CONSTEXPR_MATH_MODULES}
    ${base_directory}/sm/polysolve.cppm
  )
  list(REMOVE_DUPLICATES SM__MODULES)

  set(SM_BESSEL_I0_MODULES
    ${SM_POLYSOLVE_MODULES}
    ${base_directory}/sm/bessel_i0.cppm
  )
  list(REMOVE_DUPLICATES SM__MODULES)

  set(SM_INTERVAL_MODULES
    ${SM_CONSTEXPR_MATH_MODULES}
    ${base_directory}/sm/trait_tests.cppm
    ${base_directory}/sm/interval.cppm
  )
  list(REMOVE_DUPLICATES SM_INTERVAL_MODULES)

  set(SM_RANDOM_MODULES
    ${SM_BESSEL_I0_MODULES}
    ${SM_INTERVAL_MODULES}
    ${base_directory}/sm/random.cppm
  )
  list(REMOVE_DUPLICATES SM_RANDOM_MODULES)

  set(SM_VEC_MODULES
    ${SM_INTERVAL_MODULES}
    ${SM_RANDOM_MODULES}
    ${base_directory}/sm/vec.cppm
  )
  list(REMOVE_DUPLICATES SM_VEC_MODULES)

  set(SM_VVEC_MODULES
    ${SM_INTERVAL_MODULES}
    ${SM_RANDOM_MODULES}
    ${base_directory}/sm/vvec.cppm
  )
  list(REMOVE_DUPLICATES SM_VVEC_MODULES)

  set(SM_EVENSPACING_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/evenspacing.cppm
  )
  list(REMOVE_DUPLICATES SM_EVENSPACING_MODULES)

  set(SM_SCALE_MODULES
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/scale.cppm
  )
  list(REMOVE_DUPLICATES SM_SCALE_MODULES)

  set(SM_UTIL_MODULES
    ${base_directory}/sm/trait_tests.cppm
    ${base_directory}/sm/util.cppm
  )
  list(REMOVE_DUPLICATES SM_UTIL_MODULES)

  set(SM_HDFDATA_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${SM_UTIL_MODULES}
    ${base_directory}/sm/hdfdata.cppm
  )
  list(REMOVE_DUPLICATES SM_HDFDATA_MODULES)

  set(SM_QUATERNION_MODULES
    ${SM_VEC_MODULES}
    ${base_directory}/sm/quaternion.cppm
  )
  list(REMOVE_DUPLICATES SM_QUATERNION_MODULES)

  set(SM_MAT_MODULES
    ${SM_QUATERNION_MODULES}
    ${base_directory}/sm/mat.cppm
  )
  list(REMOVE_DUPLICATES SM_MAT_MODULES)

  set(SM_SPLINE_MODULES
    ${SM_VVEC_MODULES}
    ${SM_MAT_MODULES}
    ${base_directory}/sm/spline.cppm
  )
  list(REMOVE_DUPLICATES SM_SPLINE_MODULES)

  set(SM_RANDOM_WALK_MODULES
    ${SM_SPLINE_MODULES}
    ${base_directory}/sm/random_walk.cppm
  )
  list(REMOVE_DUPLICATES SM_RANDOM_WALK_MODULES)

  set(SM_RUNGEKUTTA4_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/rungekutta4.cppm
  )
  list(REMOVE_DUPLICATES SM_RUNGEKUTTA4_MODULES)

  set(SM_BEZCOORD_MODULES
    ${SM_VEC_MODULES}
    ${base_directory}/sm/bezcoord.cppm
  )
  list(REMOVE_DUPLICATES SM_BEZCOORD_MODULES)

  set(SM_HEX_MODULES
    ${SM_BEZCOORD_MODULES}
    ${base_directory}/sm/hex.cppm
  )
  list(REMOVE_DUPLICATES SM_HEX_MODULES)

  set(SM_RECT_MODULES
    ${SM_BEZCOORD_MODULES}
    ${base_directory}/sm/rect.cppm
  )
  list(REMOVE_DUPLICATES SM_RECT_MODULES)

  set(SM_WINDER_MODULES
    ${SM_VEC_MODULES}
    ${base_directory}/sm/winder.cppm
  )
  list(REMOVE_DUPLICATES SM_WINDER_MODULES)

  set(SM_BOOTSTRAP_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/bootstrap.cppm
  )
  list(REMOVE_DUPLICATES SM_BOOTSTRAP_MODULES)

  set(SM_GRID_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/grid.cppm
  )
  list(REMOVE_DUPLICATES SM_GRID_MODULES)

  set(SM_ALGO_MODULES
    ${SM_VEC_MODULES}
    ${base_directory}/sm/algo.cppm
  )
  list(REMOVE_DUPLICATES SM_ALGO_MODULES)

  set(SM_NM_SIMPLEX_MODULES
    ${SM_ALGO_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/nm_simplex.cppm
  )
  list(REMOVE_DUPLICATES SM_NM_SIMPLEX_MODULES)

  set(SM_ANNEAL_MODULES
    ${SM_HDFDATA_MODULES}
    ${base_directory}/sm/anneal.cppm
  )
  list(REMOVE_DUPLICATES SM_ANNEAL_MODULES)

  set(SM_HISTO_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/histo.cppm
  )
  list(REMOVE_DUPLICATES SM_HISTO_MODULES)

  set(SM_BOXFILTER_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/boxfilter.cppm
  )
  list(REMOVE_DUPLICATES SM_BOXFILTER_MODULES)

  set(SM_GEOMETRY_MODULES
    ${SM_ALGO_MODULES}
    ${SM_VVEC_MODULES}
    ${base_directory}/sm/geometry.cppm
  )
  list(REMOVE_DUPLICATES SM_GEOMETRY_MODULES)

  set(SM_JCV_MODULES
    ${SM_MATHCONST_MODULES}
    ${SM_GEOMETRY_MODULES}
    ${SM_WINDER_MODULES}
    ${base_directory}/sm/jc_voronoi.cppm
  )
  list(REMOVE_DUPLICATES SM_JCV_MODULES)

  set(SM_BEZCURVE_MODULES
    ${SM_BEZCOORD_MODULES}
    ${SM_MAT_MODULES}
    ${SM_NM_SIMPLEX_MODULES}
    ${base_directory}/sm/binomial.cppm
    ${base_directory}/sm/bezcurve.cppm
  )
  list(REMOVE_DUPLICATES SM_BEZCURVE_MODULES)

  set(SM_BEZCURVEPATH_MODULES
    ${SM_BEZCURVE_MODULES}
    ${base_directory}/sm/bezcurvepath.cppm
  )
  list(REMOVE_DUPLICATES SM_BEZCURVEPATH_MODULES)

  set(SM_HEXGRID_MODULES
    ${SM_BEZCURVEPATH_MODULES}
    ${SM_HEX_MODULES}
    ${base_directory}/sm/hexgrid.cppm
  )
  list(REMOVE_DUPLICATES SM_HEXGRID_MODULES)

  set(SM_HEXGRID_HDF_MODULES
    ${SM_HEXGRID_MODULES}
    ${SM_HDFDATA_MODULES}
    ${base_directory}/sm/hexgrid_hdf.cppm
  )
  list(REMOVE_DUPLICATES SM_HEXGRID_HDF_MODULES)

  set(SM_HEXYHISTO_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${SM_HEXGRID_MODULES}
    ${base_directory}/sm/hexyhisto.cppm
  )
  list(REMOVE_DUPLICATES SM_HEXYHISTO_MODULES)

  set(SM_CARTGRID_MODULES
    ${SM_BEZCURVEPATH_MODULES}
    ${SM_RECT_MODULES}
    ${SM_GRID_MODULES}
    ${base_directory}/sm/cartgrid.cppm
  )
  list(REMOVE_DUPLICATES SM_CARGRID_MODULES)

  set(SM_CONFIG_MODULES
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${SM_UTIL_MODULES}
    ${base_directory}/sm/config.cppm
    ${json_directory}/src/modules/json.cppm
  )
  list(REMOVE_DUPLICATES SM_CONFIG_MODULES)

  set(SM_BASE64_MODULES
    ${base_directory}/sm/base64.cppm
  )
  list(REMOVE_DUPLICATES SM_BASE64_MODULES)

  set(SM_CRC32_MODULES
    ${base_directory}/sm/crc32.cppm
  )
  list(REMOVE_DUPLICATES SM_CRC32_MODULES)

  set(SM_FLAGS_MODULES
    ${base_directory}/sm/flags.cppm
  )
  list(REMOVE_DUPLICATES SM_FLAGS_MODULES)

  # All except SM_CONFIG_MODULES, SM_HDFDATA_MODULES, SM_ANNEAL_MODULES (avoiding need for library linking)
  set(SM_ALL_MODULES
    ${SM_MATHCONST_MODULES}
    ${SM_CONSTEXPR_MATH_MODULES}
    ${SM_POLYSOLVE_MODULES}
    ${SM_BESSEL_I0_MODULES}
    ${SM_INTERVAL_MODULES}
    ${SM_RANDOM_MODULES}
    ${SM_VEC_MODULES}
    ${SM_VVEC_MODULES}
    ${SM_EVENSPACING_MODULES}
    ${SM_SCALE_MODULES}
    ${SM_UTIL_MODULES}
    ${SM_QUATERNION_MODULES}
    ${SM_MAT_MODULES}
    ${SM_SPLINE_MODULES}
    ${SM_RANDOM_WALK_MODULES}
    ${SM_RUNGEKUTTA4_MODULES}
    ${SM_BEZCOORD_MODULES}
    ${SM_HEX_MODULES}
    ${SM_RECT_MODULES}
    ${SM_WINDER_MODULES}
    ${SM_BOOTSTRAP_MODULES}
    ${SM_GRID_MODULES}
    ${SM_ALGO_MODULES}
    ${SM_NM_SIMPLEX_MODULES}
    ${SM_HISTO_MODULES}
    ${SM_BOXFILTER_MODULES}
    ${SM_GEOMETRY_MODULES}
    ${SM_BEZCURVE_MODULES}
    ${SM_BEZCURVEPATH_MODULES}
    ${SM_HEXGRID_MODULES}
    ${SM_HEXYHISTO_MODULES}
    ${SM_CARTGRID_MODULES}
    ${SM_BASE64_MODULES}
    ${SM_CRC32_MODULES}
    ${SM_FLAGS_MODULES}
  )
  list(REMOVE_DUPLICATES SM_ALL_MODULES)

endmacro()
