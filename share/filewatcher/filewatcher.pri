
INCLUDEPATH += \
	$$PWD/include

HEADERS += \
    $$PWD/include/FileWatcher/FileWatcher.h \
    $$PWD/include/FileWatcher/FileWatcherImpl.h \
    $$PWD/include/FileWatcher/FileWatcherLinux.h \
    $$PWD/include/FileWatcher/FileWatcherOSX.h \
    $$PWD/include/FileWatcher/FileWatcherWin32.h

SOURCES += \
    $$PWD/source/FileWatcher.cpp \
    $$PWD/source/FileWatcherLinux.cpp \
    $$PWD/source/FileWatcherOSX.cpp \
    $$PWD/source/FileWatcherWin32.cpp
