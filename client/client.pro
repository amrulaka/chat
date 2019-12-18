QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

QT += network
SOURCES += \
    main.cpp \
    connectionmanager.cpp \
    chatwindow.cpp \
    connectionwindow.cpp \
    subscriptionwindow.cpp \
    editpasswordwindow.cpp

HEADERS += \
    connectionmanager.h \
    consts.h \
    chatwindow.h \
    connectionwindow.h \
    subscriptionwindow.h \
    editpasswordwindow.h

FORMS += \
    chatwindow.ui

RESOURCES += \
    src.qrc
