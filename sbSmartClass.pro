#-------------------------------------------------
#
# Project created by QtCreator 2017-05-25T00:23:28
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += charts
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sbSmartClass
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    ffmpeg.cpp \
    emotion.cpp

HEADERS  += widget.h \
    emotion.h \
    ffmpeg.h

FORMS    += widget.ui
