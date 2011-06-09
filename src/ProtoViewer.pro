#-------------------------------------------------
#
# Project created by QtCreator 2011-03-25T17:37:21
#
#-------------------------------------------------

QT       += core gui

TARGET = ProtoViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    MessageModel.cpp

HEADERS  += mainwindow.h \
    MessageModel.h

FORMS    += mainwindow.ui

LIBS += -L/home/oneamtu/nbites/build/man/straight/memory/protos -lprotos -lprotobuf
