# All #includes in test programs have to be #include <sm/header>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

#if(HDF5_FOUND)
#  add_executable(hdfdata hdfdata.cpp)
#  target_link_libraries(hdfdata ${HDF5_C_LIBRARIES})
#endif(HDF5_FOUND)

#add_executable(randvec randvec.cpp)
#target_link_libraries(randvec OpenGL::GL glfw Freetype::Freetype)

# Demo of bootstrap implementation
#add_executable(bootstrap bootstrap.cpp)
#target_link_libraries(bootstrap OpenGL::GL glfw Freetype::Freetype)

#add_executable(scale scale.cpp)

add_executable(readme readme.cpp)

add_executable(vvec vvec.cpp)

add_executable(scale scale.cpp)

add_executable(mat33 mat33.cpp)

add_executable(mat44 mat44.cpp)
add_executable(mat44_frombasis mat44_frombasis.cpp)

add_executable(mat_nonsquare mat_nonsquare.cpp)

if(ARMADILLO_FOUND)
  add_executable(hexgrid hexgrid.cpp)
endif()

add_executable(vonmises vonmises.cpp)

add_executable(nelder_mead nelder_mead.cpp)
