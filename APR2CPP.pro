#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T09:46:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = APR2CPP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG += no_keywords # Python redefines some qt keywords

INCLUDEPATH += C:\Python27\include

LIBS     += C:\Python27\libs\python27.lib

DISTFILES += \
    main.py
