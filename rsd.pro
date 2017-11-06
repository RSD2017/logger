#-------------------------------------------------
#
# Project created by QtCreator 2017-10-31T09:46:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rsd
TEMPLATE = app

QMAKE_CXXFLAGS += -Wextra -Wno-unused-variable -Wno-unused-parameter
QMAKE_CXXFLAGS_RELEASE += -o3

CONFIG += c++11

SOURCES += main.cpp\
            mainwindow.cpp\
            logger.cpp

HEADERS  += mainwindow.h \
            logger.h

FORMS    += mainwindow.ui


#unix: RCC_DIR = ./BUILD/rcc/
#unix: OBJECTS_DIR = ./BUILD/obj/
#unix: MOC_DIR = ./BUILD/moc/
#unix: UI_DIR = ./BUILD/ui/

#unix: LIBS += -largtable2
