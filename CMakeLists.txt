cmake_minimum_required(VERSION 3.2)

project( realty  VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set ( SOURCES
	rclst_main.cpp 		
)

set ( SOURCES2
	rclss_main.cpp 		
)

add_subdirectory(dlib dlib_build)

add_executable(  rclst  ${SOURCES} )
add_executable(  rclss  ${SOURCES2} )

set_target_properties( rclst  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

set_target_properties( rclss  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries( rclst  dlib::dlib)
target_link_libraries( rclss  dlib::dlib)

install(TARGETS  rclst  RUNTIME DESTINATION bin)
install(TARGETS  rclss  RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT egavrilin90@gmail.com)

include(CPack)