# All #includes in test programs have to be #include <morph/header.h>
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

#if(HDF5_FOUND)
#  add_executable(hdfdata hdfdata.cpp)
#  target_link_libraries(hdfdata ${HDF5_C_LIBRARIES})
#endif(HDF5_FOUND)

#add_executable(randvec randvec.cpp)
#target_link_libraries(randvec OpenGL::GL glfw Freetype::Freetype)

# Demo of morph's bootstrap implementation
#add_executable(bootstrap bootstrap.cpp)
#target_link_libraries(bootstrap OpenGL::GL glfw Freetype::Freetype)

#add_executable(scale scale.cpp)

add_executable(vvec vvec.cpp)
