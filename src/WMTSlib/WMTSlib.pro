
TARGET = WMTSlib
TEMPLATE = lib

QT       += core
QT       += network
QT       += gui
QT       += xml
QT       += widgets

DEFINES += WMTSLIB_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        downloadmanager.cpp \
        tilematrix.cpp \
        wmtslib.cpp \
        xmlprocessor.cpp

HEADERS += \
        downloadmanager.h \
        tilematrix.h \
        wmtslib.h \
        wmtslib_global.h \
        xmlprocessor.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
