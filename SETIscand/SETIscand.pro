QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
        ../Shared/include \
		classes \
		/usr/local/include

SOURCES += \
        classes/config.cc \
        classes/datablock.cc \
        classes/datamgr.cc \
        classes/soapyio.cc \
        classes/soapyworker.cc \
        classes/tester.cc \
        main.cc

LIBS += \
        -L/usr/local/lib \
        -lfftw3 \
        -lSoapySDR

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../Shared/include/constants.h \
    ../Shared/include/properties.h \
    ../Shared/include/singleton.h \
    ../Shared/include/testable.h \
    classes/config.h \
    classes/datablock.h \
    classes/datamgr.h \
    classes/soapyio.h \
    classes/soapyworker.h \
    classes/tester.h
