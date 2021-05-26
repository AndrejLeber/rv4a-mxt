TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
     main.cpp \
    mxt.cpp \
    rt.cpp \
    bahnplanung.cpp \
    r3_protocol.cpp

LIBS += \
    -lpthread

HEADERS += \
    strdef.h \
    includes.h \
    mxt.h \
    rt.h \
    bahnplanung.h \
    r3_protocol.h
