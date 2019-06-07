cmake_minimum_required(VERSION 2.8)

project(testlibyuvrotate)

#set( CMAKE_POSITION_INDEPENDENT_CODE OFF )

set(CMAKE_CXX_STANDARD 11)

if ( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE RELEASE )
endif (NOT CMAKE_BUILD_TYPE)

set( CMAKE_CXX_FLAGS_DEBUG "-g" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3" )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBYUV_DIRECTORY}/bin" )
set( CMAKE_DEBUG_POSTFIX "_d")

if (WIN32)
	add_definitions(-D_WIN32 -D_WIN32_WINDOWS -D_CRT_SECURE_NO_WARNING -DWIN_OS)
endif(WIN32)

find_package( Threads REQUIRED )
find_package( GTest )

if (NOT GTest)
	message("spec include gtest")
	include_directories("${gtest_SOURCE_DIR}/include")
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(UUID REQUIRED uuid)

# libyuvrotate source
include_directories(${LIBYUV_DIRECTORY}/include)
link_directories( "${LIBYUV_DIRECTORY}/bin" )
set( LINKYUV_LIBRARY optimized yuvrotation debug "yuvrotation${CMAKE_DEBUG_POSTFIX}" )

add_executable(${PROJECT_NAME}
	${LIBYUV_DIRECTORY}/include/libyuvrotation.h
	main.cpp
)

# Add dependencies
if(UNIX)
  add_dependencies( ${PROJECT_NAME} libyuvrotation)
endif(UNIX)

set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LIBUUID} ${LINKYUV_LIBRARY} ${GTEST_LIBRARIES} )

