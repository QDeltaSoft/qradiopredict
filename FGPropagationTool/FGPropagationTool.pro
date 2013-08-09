#-------------------------------------------------
#
# Project created by QtCreator 2012-03-03T10:51:15
#
#-------------------------------------------------

QT       += core gui network sql widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = FGPropagationTool
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    WeatherImageObject.cpp \
    WeatherManager.cpp \
    databaseapi.cpp \
    fgtelnet.cpp \
    toolbox.cpp \
    connectionsuccessdialog.cpp \
    util.cpp \
    fgremote.cpp \
    mobilestation.cpp \
    groundstation.cpp \
    flightplanpoints.cpp \
    mobileform.cpp \
    groundstationform.cpp \
    flightplanform.cpp \
    signal.cpp \
    updater.cpp \
    settingsdialog.cpp \
    stationsignalform.cpp

HEADERS  += MainWindow.h \
    WeatherImageObject.h \
    WeatherManager.h \
    databaseapi.h \
    fgtelnet.h \
    toolbox.h \
    connectionsuccessdialog.h \
    util.h \
    fgremote.h \
    mobilestation.h \
    groundstation.h \
    flightplanpoints.h \
    mobileform.h \
    groundstationform.h \
    flightplanform.h \
    signal.h \
    updater.h \
    settingsdialog.h \
    stationsignalform.h

FORMS    += MainWindow.ui \
    toolbox.ui \
    connectionsuccessdialog.ui \
    mobileform.ui \
    groundstationform.ui \
    flightplanform.ui \
    settingsdialog.ui \
    stationsignalform.ui

#Linkage for MapGraphics shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:unix:!symbian: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics -lsqlite3

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics

RESOURCES += \
    resources.qrc
