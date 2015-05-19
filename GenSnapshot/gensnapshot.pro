#-------------------------------------------------
#
# Project created by QtCreator 2013-12-24T10:00:33
#
#-------------------------------------------------

QT       += core sql

QT       -= gui

TARGET = gensnapshot
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../3rdparty

SOURCES += main.cpp sqlimport.cpp

HEADERS  += sqlimport.h
