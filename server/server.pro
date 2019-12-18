
QMAKE_CXXFLAGS += -std=c++11

QT          += network
QT          += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = server
TEMPLATE = app


SOURCES += main.cpp \
    datamanager.cpp \
    serverwindow.cpp

HEADERS += \
    consts.h \
    datamanager.h \
    serverwindow.h

FORMS   += \
    serverwindow.ui
