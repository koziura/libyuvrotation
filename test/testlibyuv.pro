TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle qml qml_debug
CONFIG -= qt

APP_NAME = testlibyuvrotation

win32:DEFINES += _CRT_SECURE_NO_WARNINGS WINOS _WIN32_WINDOWS

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
DESTDIR = $$PWD/../bin

# Moc output directory
MOC_DIR = tmp/$$OUTPUT_DIR

# Resource compiler directory
RCC_DIR = tmp

# Temp objects output directory
OBJECTS_DIR = obj/$$OUTPUT_DIR

UI_DIR = tmp/ui

LIBYUV_PATH = $$PWD/../../3rd_party/libyuv

INCLUDEPATH += \
	$$PWD/../include \

HEADERS += \

SOURCES += \
	main.cpp \

LIBS += \
	-L$$PWD/../bin

LIBS += -L$$LIBYUV_PATH/lib/dynamic
LIBS += \
	-lyuv

LIBS += \
	-lboost_system	\
	-lboost_filesystem	\
	-lpthread	\
	-gtest.so		\
	-lyuvrotation$$TARGET_POSTFIX
