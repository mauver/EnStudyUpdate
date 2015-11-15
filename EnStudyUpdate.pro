#-------------------------------------------------
#
# Project created by QtCreator 2015-11-12T22:35:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnStudyUpdate
TEMPLATE = app


SOURCES += main.cpp\
        updatedialog.cpp \
    webmanager.cpp \
    filemanager.cpp

HEADERS  += updatedialog.h \
    webmanager.h \
    userdefs.h \
    filemanager.h

FORMS    += updatedialog.ui
