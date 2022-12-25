#-------------------------------------------------
#
# Project created by QtCreator 2019-10-01T09:25:31
#
#-------------------------------------------------

QT       += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adaptorApp
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

lala.files = ../shared/summa.xml
lala.header_flags = -i ../shared/mydata.h
#DBUS_ADAPTORS += ../shared/summa.xml
DBUS_ADAPTORS += lala

SOURCES += \
        ../shared/mydata.cpp \
        main.cpp \
        widgetwithadaptor.cpp

HEADERS += \
    ../shared/mydata.h \
    widgetwithadaptor.h

