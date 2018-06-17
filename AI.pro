#-------------------------------------------------
#
# Project created by QtCreator 2018-03-04T08:40:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SSAIReader.cpp \
    Object/Path/SSBezier.cpp \
    Object/Path/SSBezierC.cpp \
    Object/Path/SSBezierV.cpp \
    Object/Path/SSBezierY.cpp \
    Object/Path/SSLineSegment.cpp \
    Object/Path/SSPath.cpp \
    Object/Path/SSPathOperator.cpp \
    Object/Text/SSText.cpp \
    Object/Text/SSTextOperator.cpp \
    Object/SSObject.cpp \
    Object/Image/SSImage.cpp \
    SSObjectInfo.cpp

HEADERS  += mainwindow.h \
    SSAIReader.h \
    Object/Path/SSBezier.h \
    Object/Path/SSBezierC.h \
    Object/Path/SSBezierV.h \
    Object/Path/SSBezierY.h \
    Object/Path/SSLineSegment.h \
    Object/Path/SSPath.h \
    Object/Path/SSPathOperator.h \
    Object/Text/SSText.h \
    Object/Text/SSTextOperator.h \
    Object/SSObject.h \
    Object/Image/SSImage.h \
    SSObjectInfo.h

FORMS    += mainwindow.ui
