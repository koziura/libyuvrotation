cmake_minimum_required(VERSION 2.8)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

message( "${Cyan}\
	\t-DCMAKE_BUILD_TYPE={Release,Debug}, default: RELEASE\n \
	\t-DLIBYUV_TEST={OFF,ON}, default: ON\n \
	${ColourReset}"
)

option( BUILD_CLANG "build application with clang" OFF ) # OFF is the default
option( LIBYUV_TEST "Enable building test" ON)

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

project(libyuvrotation)

set(CMAKE_CXX_STANDARD 11)

if ( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE RELEASE )
endif (NOT CMAKE_BUILD_TYPE)

set( CMAKE_CXX_FLAGS_DEBUG "-g" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3" )

set( LIBYUV_DIRECTORY "${PROJECT_SOURCE_DIR}" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin" )
set( CMAKE_DEBUG_POSTFIX "_d")

add_definitions(-DYUVROTATE_LIBRARY_BUILD)

if (WIN32)
	add_definitions(-D_WIN32 -D_WIN32_WINDOWS -D_CRT_SECURE_NO_WARNING -DWIN_OS)
endif(WIN32)

# Common libraries

find_package( Threads REQUIRED )

#set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

find_package ( Boost COMPONENTS	system filesystem timer REQUIRED )

include_directories(${Boost_INCLUDE_DIRS})

# Unpack googletest at configure time
configure_file(3rdparty/gtest/CMakeLists.txt.in googletest-pack/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-pack )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-pack )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
				 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
				 EXCLUDE_FROM_ALL)

include_directories("${gtest_SOURCE_DIR}/include")

# Unpack Chromium YUV library at configure time
configure_file(3rdparty/libyuv/CMakeLists.txt.in libyuv-pack/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libyuv-pack )
if(result)
  message(FATAL_ERROR "CMake step for libyuv failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libyuv-pack )
if(result)
  message(FATAL_ERROR "Build step for libyuv failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libyuv-src
                 ${CMAKE_CURRENT_BINARY_DIR}/libyuv-build
                 EXCLUDE_FROM_ALL)

include_directories("${YUV_SOURCE_DIR}/include")

# project source
include_directories(include)
include_directories(src/inc)

# share
include_directories(share/filewatcher/include)

# library
add_library( ${PROJECT_NAME} SHARED
	include/libyuvrotation.h
	share/filewatcher/include/FileWatcher/FileWatcher.h
	share/filewatcher/include/FileWatcher/FileWatcherImpl.h
	share/filewatcher/include/FileWatcher/FileWatcherLinux.h
	share/filewatcher/include/FileWatcher/FileWatcherOSX.h
	share/filewatcher/include/FileWatcher/FileWatcherWin32.h
	share/filewatcher/source/FileWatcher.cpp
	share/filewatcher/source/FileWatcherLinux.cpp
	share/filewatcher/source/FileWatcherOSX.cpp
	share/filewatcher/source/FileWatcherWin32.cpp
	src/inc/filemonitor.h
	src/inc/libyuvcore.h
	src/inc/singleton.h
	src/inc/yuvrotate.h
	src/filemonitor.cpp
	src/libyuvcore.cpp
	src/libyuvrotation.cpp
	src/yuvrotate.cpp
)

set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )
set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX} )

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

set( LIBYUV_VERSION_MAJOR 0 )
set( LIBYUV_VERSION_MINOR 1 )
set( LIBYUV_VERSION_PATCH 1 )
set( LIBYUV_VERSION_STRING ${LIBYUV_VERSION_MAJOR}.${LIBYUV_VERSION_MINOR}.${LIBYUV_VERSION_PATCH})

target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} yuv)

if( LIBYUV_TEST )
	message( STATUS "*** BUILD YuvRotation LIBRARY ${BoldGreen}WITH${ColourReset} TEST ***" )
	add_subdirectory(test)
else( LIBYUV_TEST )
	message( STATUS "*** BUILD YuvRotation LIBRARY ${BoldRed}WITHOUT${ColourReset} TEST ***" )
endif( LIBYUV_TEST )

