#-------------------------------------------------
#
# Project created by QtCreator 2017-06-21T00:22:49
#
#-------------------------------------------------

QT       += core gui network dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QBattMon
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwidget.cpp \
    systemtrayicon.cpp \
    battery.cpp \
    brightnessslider.cpp \
    powermanagement.cpp

HEADERS += \
        mainwidget.h \
    systemtrayicon.h \
    battery.h \
    globalheader.h \
    brightnessslider.h \
    powermanagement.h

FORMS += \
        mainwidget.ui \
    powermanagement.ui

DISTFILES += \
    CHANGELOG.md \
    COPYING \
    README.md

LIBS += -lX11 \
        -lXrandr \
        -lutil

DEFINES += QT_NO_DEBUG_OUTPUT

CONFIG(debug, debug | release) {
    DEFINES -= QT_NO_DEBUG_OUTPUT
}
