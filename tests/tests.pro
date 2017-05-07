 ###################################################################
#                                                                   #
#  SpendingControl                                                  #
#  Copyright (C) 2017 Eugene Melnik <jeka7js@gmail.com>             #
#                                                                   #
 ###################################################################

TARGET = spending-control_tests
TEMPLATE = app


QT = core gui widgets sql testlib
CONFIG += qt warn_on thread console testcase

LIBS += -lstdc++ -lpthread

QMAKE_CXXFLAGS += -std=c++11 -fPIC


INCLUDEPATH += $$PWD/../src/


HEADERS += ../src/database/manager.h \
           database/manager_tests.h

SOURCES += ../src/database/manager.cpp \
           database/manager_tests.cpp \
           main.cpp


    # Debug & Release #

CONFIG(debug, debug|release) {
    message( "DEBUG mode" )

    MOC_DIR = ./tmp/debug/moc
    OBJECTS_DIR = ./tmp/debug/obj
    RCC_DIR = ./tmp/debug/rcc
    UI_DIR = ./tmp/debug/ui

} else {
    message( "RELEASE mode" )

    MOC_DIR = ./tmp/release/moc
    OBJECTS_DIR = ./tmp/release/obj
    RCC_DIR = ./tmp/release/rcc
    UI_DIR = ./tmp/release/ui
}

DESTDIR = ./
