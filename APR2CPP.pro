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
        mainwindow.cpp \
    brosethread.cpp \
    AudioFile-master/AudioFile.cpp

HEADERS  += mainwindow.h \
    brosethread.h \
    AudioFile-master/AudioFile.h

FORMS    += mainwindow.ui

win32: LIBS += -L$$PWD/../../apr2dll/apr2dll/Release/ -lbroseAPR

INCLUDEPATH += $$PWD/../../apr2dll/apr2dll/apr2dll
DEPENDPATH += $$PWD/../../apr2dll/apr2dll/apr2dll
