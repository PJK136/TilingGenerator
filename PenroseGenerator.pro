#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T18:05:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PenroseGenerator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    triangle.cpp

HEADERS  += mainwindow.h \
    triangle.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra
