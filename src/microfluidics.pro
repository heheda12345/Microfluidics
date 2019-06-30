#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T20:30:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = microfluidics
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    chipshower.cpp \
    pipeinsertdialog.cpp \
    newchipdialog.cpp \
    chip.cpp \
    designdialog.cpp \
    pipewidget.cpp \
    interwidget.cpp \
    calculator.cpp \
    mycolor.cpp \
    chipdesigner.cpp \
    colorbar.cpp

HEADERS += \
        mainwindow.h \
    pipeinsertdialog.h \
    newchipdialog.h \
    chip.h \
    designdialog.h \
    myundocommand.h \
    pipewidget.h \
    interwidget.h \
    chipshower.h \
    calculator.h \
    mycolor.h \
    chipdesigner.h \
    colorbar.h

FORMS += \
        mainwindow.ui \
    newchipdialog.ui \
    pipeinsertdialog.ui \
    designdialog.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    icon.qrc

RC_ICONS = icon.ico
