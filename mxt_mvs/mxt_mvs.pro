QT += serialport

TEMPLATE = app
CONFIG += c++1z console
#CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += \
    main.cpp \
    mxt.cpp \
    rt.cpp \
    bahnplanung.cpp \
    r3_protocol.cpp \
    GCode.cpp \
    serial.cpp

LIBS += \
    -lpthread

LIBS += -L/usr/include/python2.7 -lpython2.7

HEADERS += \
    strdef.h \
    includes.h \
    mxt.h \
    rt.h \
    bahnplanung.h \
    r3_protocol.h \
    matplotlibcpp.h \
    GCode.h \
    serial.h
