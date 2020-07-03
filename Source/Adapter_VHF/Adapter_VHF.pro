QT -= gui
QT += network

CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -fpermissive

SOURCES += main.cpp \
    socket/SocketManage.cpp \
    config/ConfigLoader.cpp \
    Uart/qextserialport.cpp \
    Radio/Radio181d.cpp \
    Radio/RadioManage.cpp \
    Radio/Radio171al.cpp \
    Radio/Radio171d.cpp \
    Radio/Radio781tcp.cpp \
    socket/UdpRctrl.cpp \
    socket/UdpVoice.cpp \
    socket/TcpClient.cpp \
    socket/TCPDataProcess.cpp\
    RadioLink/RadioLink.cpp \
    RadioLink/LinkCommon.cpp \
    RadioLink/RadioLinkManage.cpp \
    RadioLink/RadioLinkClient.cpp \
    RadioLink/RadioLinkMaster.cpp \
    Radio/Radio212tcr.cpp \
    Radio/Radio.cpp \
    Radio/Radio220tcr.cpp \
    config/ConfigRadio171al.cpp \
    config/ConfigRadio171d.cpp \
    config/ConfigRadio181d.cpp \
    config/ConfigRadio781tcp.cpp \
    config/ConfigRadio.cpp \
    config/ConfigRadio212tcr.cpp \
    config/ConfigRadio220.cpp \
    config/ConfigRadio230.cpp \
    Radio/Radio230.cpp \
    RadioLink/RadioLink_A01SSB.cpp \
    log/log4z.cpp \
    config/ConfigLog.cpp


HEADERS += \
    socket/SocketManage.h \
    config/ConfigLoader.h \
    Uart/qextserialport.h \
    Uart/qextserialport_global.h \
    Uart/qextserialport_p.h \
    Radio/Radio181d.h \
    Radio/RadioManage.h \
    Radio/Radio171al.h \
    Radio/Radio171d.h \
    Radio/Radio781tcp.h \
    socket/UdpRctrl.h \
    socket/UdpVoice.h \
    socket/TcpClient.h \
    socket/TCPDataProcess.h \
    RadioLink/RadioLink.h \
    RadioLink/LinkCommon.h \
    RadioLink/RadioLinkManage.h \
    RadioLink/RadioLinkClient.h \
    RadioLink/RadioLinkMaster.h \
    Radio/Radio212tcr.h \
    Radio/Radio.h \
    Radio/RadioType.h \
    socket/SocketCommu.h \
    Radio/Radio220tcr.h \
    config/ConfigRadio171al.h \
    config/ConfigRadio171d.h \
    config/ConfigRadio181d.h \
    config/ConfigRadio781tcp.h \
    config/ConfigRadio.h \
    config/ConfigRadio212tcr.h \
    config/ConfigRadio220.h \
    config/ConfigRadio230.h \
    Radio/Radio230.h \
    RadioLink/RadioLink_A01SSB.h \
    log/log4z.h \
    config/ConfigLog.h

win32{
    CONFIG += console

    SOURCES += Uart/qextserialport_win.cpp

#    HEADERS +=

    LIBS += -L$$PWD/log/lib/win -lshlwapi
}

unix{

    INCLUDEPATH += /usr/local/arm/alsa_4.6.2/include
    LIBS += -L/usr/local/arm/alsa_4.6.2/lib -lasound

    SOURCES +=  Uart/qextserialport_unix.cpp \
                Audio/AudioControl.cpp \
                Audio/AudioMixer.cpp \
                Audio/AudioPlayer.cpp \
                Audio/AudioRecoder.cpp \
                Audio/AudioPtt.cpp

    HEADERS +=  Audio/AudioControl.h \
                Audio/AudioMixer.h \
                Audio/AudioPlayer.h \
                Audio/AudioRecoder.h \
                Audio/AudioPtt.h
}
