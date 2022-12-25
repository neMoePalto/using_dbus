#-------------------------------------------------
#
# Project created by QtCreator 2019-10-01T09:27:47
#
#-------------------------------------------------

QT       += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = interfaceApp
TEMPLATE = app

OBJECTS_DIR = temp/obj
MOC_DIR = temp/moc
DESTDIR = ../../bin/
#INCLUDEPATH += src/

# --- Compilation flags:
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14
#CONFIG += C++14
#QMAKE_CXXFLAGS += -std=c++14

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_WARNING_OUTPUT
#DEFINES += QT_NO_DEBUG_OUTPUT

custom_interface.files = ../shared/summa.xml
custom_interface.header_flags = -i ../shared/mydata.h
#DBUS_INTERFACES += ../shared/summa.xml
DBUS_INTERFACES += custom_interface

SOURCES += \
        ../shared/mydata.cpp \
        main.cpp \
        widgetwithinterface.cpp

HEADERS += \
    ../shared/mydata.h \
    widgetwithinterface.h

