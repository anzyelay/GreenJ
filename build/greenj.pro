# ----------------
# Qt Project file
# ----------------

TEMPLATE = app
TARGET = GreenJ
QT += core network multimedia widgets webenginewidgets
#QT += webkitwidgets
win32 {
	DESTDIR = ../bin/win32
	LIBDIR = ../lib/win32
	BUILDDIR = ../build/win32
	QT += qtmain phonon
	PJSIP_DIR = ../lib/win32/pjsip
	PJSIP_TARGET = i386-Win32-vc8-Release
}
unix {
	DESTDIR = ../bin/linux
	BUILDDIR = ../build/linux
        PJSIP_DIR = /home/anzye/Workspace/3rdparty/audio_video/pjproject/out
        LIBDIR = $$PJSIP_DIR/lib
        PJSIP_TARGET = x86_64-linux-gnu
}

SOURCEDIR = ../src
VENDORDIR = ../vendor
RESOURCEDIR = ../res

#CONFIG += debug
CONFIG += ordered

#DEFINES += QT_LARGEFILE_SUPPORT
DEFINES += DEBUG

INCLUDEPATH += $$SOURCEDIR/GeneratedFiles \
    $$SOURCEDIR/GeneratedFiles/Debug \
    $$SOURCEDIR \
    $$VENDORDIR/qt-json \
    $$PJSIP_DIR/include \

LIBS +=  -L$$LIBDIR/
	
unix: LIBS +=  \
        -lpjsua \
        -lpjsip-ua \
        -lpjsip-simple \
        -lpjsip \
        -lwebrtc \
        -lpjmedia-codec \
        -lpjmedia \
        -lpjmedia-audiodev \
        -lpjnath \
        -lpjlib-util \
        -lresample \
        -lsrtp \
        -lgsmcodec \
        -lspeex \
        -lilbccodec \
        -lg7221codec \
        -lpj \
	-lm \
	-lpthread \
	-lssl \
#	-lasound \
#        -lmilenage \
#        -lportaudio  \
        #-luuid \
	#-lcrypto \

unix:!mac: LIBS += -lnsl -lrt

win32: LIBS += -lIphlpapi \
    -ldsound \
    -ldxguid \
    -lnetapi32 \
    -lmswsock \
    -lws2_32 \
    -lodbc32 \
    -lodbccp32 \
    -lole32 \
    -luser32 \
    -lgdi32 \
    -ladvapi32 \
    -lpjlib-$$PJSIP_TARGET \
    -lpjlib-util-$$PJSIP_TARGET \
    -lpjmedia-$$PJSIP_TARGET \
    -lpjmedia-codec-$$PJSIP_TARGET \
    -lpjmedia-audiodev-$$PJSIP_TARGET \
    -lpjnath-$$PJSIP_TARGET \
    -lpjsua-lib-$$PJSIP_TARGET \
    -lpjsip-ua-$$PJSIP_TARGET \
    -lpjsip-simple-$$PJSIP_TARGET \
    -lpjsip-core-$$PJSIP_TARGET \
    -llibilbccodec-$$PJSIP_TARGET \
    -llibgsmcodec-$$PJSIP_TARGET \
    -llibg7221codec-$$PJSIP_TARGET \
    -llibmilenage-$$PJSIP_TARGET \
    -llibportaudio-$$PJSIP_TARGET \
    -llibresample-$$PJSIP_TARGET \
    -llibspeex-$$PJSIP_TARGET \
    -llibsrtp-$$PJSIP_TARGET

mac: LIBS += -lssl -lcrypto -framework CoreAudio -framework AudioUnit -framework AudioToolbox -framework Cocoa

DEPENDPATH += $$SOURCEDIR
MOC_DIR += $$SOURCEDIR/GeneratedFiles/debug
OBJECTS_DIR += $$BUILDDIR
UI_DIR += $$SOURCEDIR/GeneratedFiles
RCC_DIR += $$SOURCEDIR/GeneratedFiles

HEADERS += $$SOURCEDIR/phone/api/Interface.h \
    $$SOURCEDIR/phone/api/Sip.h \
    $$SOURCEDIR/phone/Account.h \
    $$SOURCEDIR/phone/Call.h \
    $$SOURCEDIR/phone/Phone.h \
    $$SOURCEDIR/Config.h \
    $$SOURCEDIR/Gui.h \
    $$SOURCEDIR/JavascriptHandler.h \
    $$SOURCEDIR/LogHandler.h \ 
    $$SOURCEDIR/PrintHandler.h \
    $$SOURCEDIR/Sound.h \
    $$SOURCEDIR/WebPage.h \
    $$VENDORDIR/qt-json/json.h
SOURCES += $$SOURCEDIR/main.cpp \
    $$SOURCEDIR/phone/api/Sip.cpp \
    $$SOURCEDIR/phone/Account.cpp \
    $$SOURCEDIR/phone/Call.cpp \
    $$SOURCEDIR/phone/Phone.cpp \
    $$SOURCEDIR/Config.cpp \
    $$SOURCEDIR/Gui.cpp \
    $$SOURCEDIR/JavascriptHandler.cpp \
    $$SOURCEDIR/LogHandler.cpp \
    $$SOURCEDIR/PrintHandler.cpp \
    $$SOURCEDIR/Sound.cpp \
    $$VENDORDIR/qt-json/json.cpp
FORMS += $$SOURCEDIR/gui.ui
RESOURCES += $$RESOURCEDIR/gui.qrc

win32:RC_FILE = $$RESOURCEDIR/greenj.rc
mac:ICON = $$RESOURCEDIR/images/greenj.icns
