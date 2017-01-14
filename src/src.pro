 ###################################################################
#                                                                   #
#  SpendingControl                                                  #
#  Copyright (C) 2017 Eugene Melnik <jeka7js@gmail.com>             #
#                                                                   #
 ###################################################################

    # Main configuration #

TARGET = spending-control
TEMPLATE = app


#CONFIG += release
#CONFIG += debug console

QT = core gui widgets sql
CONFIG += qt warn_on thread

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Ofast

LIBS += -lstdc++ -lpthread


    # Options #

#DEFINES += 


    # Source files #

HEADERS += \
    database/manager.h \
    database/migration.h \
    database/query.h \
    commandlineparser.h \
    defines.h \
    logger.h \
    maincontroller.h \
    mainwindow.h \
    settings.h \
    types.h \
    version.h

SOURCES += \
    database/manager.cpp \
    database/migration.cpp \
    database/query.cpp \
    commandlineparser.cpp \
    logger.cpp \
    main.cpp \
    maincontroller.cpp \
    mainwindow.cpp \
    settings.cpp

FORMS += \
    mainwindow.ui


    # Translations #

#TRANSLATIONS += 


    # Resources #

#RESOURCES = 


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


    # Installation setup #

target.path = /usr/bin/

INSTALLS = target
