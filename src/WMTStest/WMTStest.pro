TARGET = WMTStest
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += console c++11
CONFIG += qt
CONFIG -= app_bundle
QT       += core
QT       += network
QT       += gui
QT       += xml
QT       += widgets

SOURCES += \
        main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-WMTSlib/release/ -lWMTSlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-WMTSlib/debug/ -lWMTSlib
else:unix: LIBS += -L$$PWD/../build-WMTSlib/ -lWMTSlib

INCLUDEPATH += $$PWD/../WMTSlib
DEPENDPATH += $$PWD/../WMTSlib
