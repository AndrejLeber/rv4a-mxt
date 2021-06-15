QT -= gui
QT += serialport

#TEMPLATE = app
CONFIG += c++1z console
CONFIG -= app_bundle
#CONFIG -= qt

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mxt.cpp \
    rt.cpp \
    bahnplanung.cpp \
    r3_protocol.cpp \
    GCode.cpp \
    serial.cpp \
    serial_receiver.cpp

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
    serial.h \
    serial_receiver.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
