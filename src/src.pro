 ###################################################################
#                                                                   #
#  SpendingControl                                                  #
#  Copyright (C) 2017 Eugene Melnik <jeka7js@gmail.com>             #
#                                                                   #
 ###################################################################

    # Main configuration #

TARGET = spending-control
TEMPLATE = app


CONFIG += console

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
    controller/maincontroller.h \
    database/manager.h \
    database/migration.h \
    database/query.h \
    model/accountsmodel.h \
    tool/commandlineparser.h \
    tool/logger.h \
    tool/settings.h \
    window/accountslistdialog.h \
    window/mainwindow.h \
    defines.h \
    types.h \
    version.h

SOURCES += \
    controller/maincontroller.cpp \
    database/manager.cpp \
    database/migration.cpp \
    database/query.cpp \
    model/accountsmodel.cpp \
    tool/commandlineparser.cpp \
    tool/logger.cpp \
    tool/settings.cpp \
    window/accountslistdialog.cpp \
    window/mainwindow.cpp \
    main.cpp

FORMS += \
    window/mainwindow.ui \
    window/accountslistdialog.ui


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
