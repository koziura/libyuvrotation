cmake_minimum_required(VERSION 3.13)
project(filewatcher)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/FileWatcher)

add_executable(filewatcher
	include/FileWatcher/FileWatcher.h
	include/FileWatcher/FileWatcherImpl.h
	include/FileWatcher/FileWatcherLinux.h
	include/FileWatcher/FileWatcherOSX.h
	include/FileWatcher/FileWatcherWin32.h
	source/FileWatcher.cpp
	source/FileWatcherLinux.cpp
	source/FileWatcherOSX.cpp
	source/FileWatcherWin32.cpp)
