#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T18:26:14
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = faceCount
TEMPLATE = app


SOURCES += main.cpp\
        facescount.cpp \
    cvfaces.cpp

INCLUDEPATH += /usr/local/include   \
        /usr/local/include/opencv    \
        /usr/local/include/opencv2

LIBS += -L /usr/local/lib/ -lopencv_world

HEADERS  += facescount.h \
    cvfaces.h

FORMS    += facescount.ui
