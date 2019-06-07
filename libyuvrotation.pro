TEMPLATE = lib
CONFIG += c++11
CONFIG -= lib_bundle qml qml_debug
CONFIG -= qt
CONFIG += dll
CONFIG += no_plugin_name_prefix
CONFIG += plugin

APP_NAME = libyuvrotation

VERSION = 0.1.1

DEFINES += YUVROTATE_LIBRARY_BUILD
win32:DEFINES += _CRT_SECURE_NO_WARNINGS WINOS _WIN32_WINDOWS

# Check if the filewatcher.pri file exists
! include($$PWD/share/filewatcher/filewatcher.pri) {
	error("Couldn't find the filewatcher.pri file!")
}

# Debug configuration
CONFIG(debug, debug|release) {
	# Configuration name
	OUTPUT_DIR = debug
	TARGET_POSTFIX = _d
	DEFINES += _DEBUG

	QMAKE_CFLAGS_DEBUG ~= s/-O[0123s]//g
	QMAKE_CFLAGS_DEBUG += -O0
	CONFIG += debug declarative_debug
}else{
	# Configuration name
	OUTPUT_DIR = release
	DEFINES += NDEBUG
}

QMAKE_LFLAGS -= static
QMAKE_CXXFLAGS_RELEASE = -Wall -pedantic -fPIC
QMAKE_CFLAGS_RELEASE = -Wall -pedantic -fPIC

# Target name
TARGET = $$APP_NAME$$TARGET_POSTFIX

# Destination directories
DESTDIR = bin

# Moc output directory
MOC_DIR = tmp/$$OUTPUT_DIR

# Resource compiler directory
RCC_DIR = tmp

# Temp objects output directory
OBJECTS_DIR = obj/$$OUTPUT_DIR

UI_DIR = tmp/ui

LIBYUV_PATH = $$PWD/../3rd_party/libyuv

INCLUDEPATH += \
	$$PWD/include \
	$$PWD/src/inc \
	$$LIBYUV_PATH/include

HEADERS += \
	include/libyuvrotation.h \
	src/inc/filemonitor.h \
	src/inc/singleton.h \
    src/inc/libyuvcore.h \
    src/inc/yuvrotate.h

SOURCES += \
	src/libyuvrotation.cpp \
	src/filemonitor.cpp \
    src/libyuvcore.cpp \
    src/yuvrotate.cpp

LIBS += -L$$LIBYUV_PATH/lib/dynamic
LIBS += \
	-lyuv

LIBS += \
	-lboost_system	\
	-lboost_filesystem	\
	-lboost_timer	\
	-lpthread

