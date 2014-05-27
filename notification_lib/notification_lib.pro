QT -= gui
QT += network

TARGET = NotificationLib
TEMPLATE = lib
CONFIG += shared

QMAKE_LFLAGS += -static

SOURCES += NotificationLib.cpp

HEADERS += NotificationLib.h

# FIXME for correct building, need to change order of libs in file "Makefile.Release" like this:
#   LIBS = ... -lQt5Network -lQt5Core -lz -lole32 -luuid -lwsock32
win32 {
    LIBS += "-Le:\zlib-1.2.8" -lz
    LIBS += "-Le:\mingw32\i686-w64-mingw32\lib" -lole32 -luuid -lwsock32
}
