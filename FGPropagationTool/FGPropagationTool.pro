#-------------------------------------------------
#
# Project created by QtCreator 2012-03-03T10:51:15
#
#-------------------------------------------------

QT       += core gui network sql widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = qRadioPredict
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
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
    stationsignalform.cpp \
    flightgearprefs.cpp \
    aprs.cpp \
    aprsstation.cpp \
    rawmessagesform.cpp \
    aprsicon.cpp \
    aprspixmapitem.cpp \
    srtmreader.cpp \
    shpreader.cpp \
    sg/SGGeodesy.cxx \
    radio/antenna.cxx \
    radio/radiosystem.cxx \
    radio/itm.cpp \
    radio/scenerymanager.cpp \
    plotpolygon.cpp \
    plotvalue.cpp \
    savedialog.cpp \
    loaddialog.cpp

HEADERS  += MainWindow.h \
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
    stationsignalform.h \
    flightgearprefs.h \
    aprs.h \
    aprsstation.h \
    rawmessagesform.h \
    aprsicon.h \
    aprspixmapitem.h \
    srtmreader.h \
    shpreader.h \
    sg/SGGeodesy.hxx \
    sg/SGMath.hxx \
    sg/SGCMath.hxx \
    sg/SGQuat.hxx \
    sg/SGLocation.hxx \
    sg/SGMatrix.hxx \
    sg/SGMathFwd.hxx \
    sg/sg_geodesy.hxx \
    sg/SGGeoc.hxx \
    sg/SGGeod.hxx \
    sg/SGVec2.hxx \
    sg/SGVec3.hxx \
    sg/SGMisc.hxx \
    sg/SGLimits.hxx \
    sg/compiler.h \
    sg/constants.h \
    sg/sg_inlines.h \
    radio/antenna.hxx \
    radio/radiosystem.hxx \
    radio/scenerymanager.h \
    plotpolygon.h \
    plotvalue.h \
    savedialog.h \
    loaddialog.h

FORMS    += MainWindow.ui \
    toolbox.ui \
    connectionsuccessdialog.ui \
    mobileform.ui \
    groundstationform.ui \
    flightplanform.ui \
    settingsdialog.ui \
    stationsignalform.ui \
    rawmessagesform.ui \
    savedialog.ui \
    loaddialog.ui

#Linkage for MapGraphics shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:unix:!symbian: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics
INCLUDEPATH += $$PWD/../MapGraphics
INCLUDEPATH += /home/adrian/c++/gdal/include/
DEPENDPATH += $$PWD/../MapGraphics
unix:!symbian: LIBS += -lgdal

RESOURCES += \
    resources.qrc
