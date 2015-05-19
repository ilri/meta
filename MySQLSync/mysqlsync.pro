#-------------------------------------------------
#
# Project created by QtCreator 2013-08-19T09:41:32
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = mysqlsync
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

unix:INCLUDEPATH += ../3rdparty

SOURCES += main.cpp
