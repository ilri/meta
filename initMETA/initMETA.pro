#-------------------------------------------------
#
# Project created by QtCreator 2014-04-29T10:59:55
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = initmeta
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:INCLUDEPATH += ../3rdparty

SOURCES += main.cpp

RESOURCES += \
    model.qrc
