#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T18:05:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TilingGenerator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surface.cpp

HEADERS  += mainwindow.h \
    surface.h

FORMS    += mainwindow.ui

linux-g++ {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/bin

    desktop.path = $$PREFIX/share/applications/
    desktop.files += TilingGenerator.desktop

    icon.path = $$PREFIX/share/icons/hicolor/scalable/apps/
    icon.files += TilingGenerator.svg

    INSTALLS += icon
    INSTALLS += desktop
    INSTALLS += target
}

