#-------------------------------------------------
#
# Project created by QtCreator 2020-02-19T13:08:54
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VHF_Box
TEMPLATE = app

QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS += -Wno-psabi

CONFIG += debug

#INCLUDEPATH += /root/test/OpenSource/install/include
#INCLUDEPATH += /usr/local/include
#LIBS += -L/lib/x86_64-linux-gnu -lasound
#LIBS += -L/root/test/OpenSource/install/lib -lbcg729
#LIBS += -L/usr/local/lib -lopus

#INCLUDEPATH += /usr/local/arm/alsa/include
#INCLUDEPATH += /usr/local/arm/bcg729/include
#INCLUDEPATH += /root/test/OpenSource/Opus/install/include
#LIBS += -L/usr/local/arm/alsa/lib -lasound
#LIBS += -L/usr/local/arm/bcg729/lib -lbcg729
#LIBS += -L/root/test/OpenSource/Opus/install/lib -lopus

INCLUDEPATH += /usr/local/arm/alsa_4.6.2/include
INCLUDEPATH += /usr/local/arm/bcg729_4.6.2/include
INCLUDEPATH += /root/test/OpenSource/Opus/opus_4.6.2/include
LIBS += -L/usr/local/arm/alsa_4.6.2/lib -lasound
LIBS += -L/usr/local/arm/bcg729_4.6.2/lib -lbcg729
LIBS += -L/root/test/OpenSource/Opus/opus_4.6.2/lib -lopus


SOURCES += main.cpp\
    Audio/AudioControl.cpp \
    Audio/AudioPlayer.cpp \
    Audio/AudioRecoder.cpp \
    Audio/AudioMixer.cpp \
    Audio/AudioUtil.cpp \
    Socket/SocketManage.cpp \
    Socket/UdpVoice.cpp \
    Socket/UdpRctrl.cpp \
    Uart/UartManage_bk.cpp \
    Uart/qextserialport.cpp \
    Uart/qextserialport_unix.cpp \
    Keyboard/KeyUart.cpp \
    Config/ConfigLoader.cpp \
    Config/BoxConf.cpp \
    UI/UIManager.cpp \
    UI/WidgeBase.cpp \
    UI/WidgeSSB.cpp \
    Util/RadioUtil.cpp \
    UI/WidgeVHF181d.cpp \
    UI/WidgeMulti.cpp \
    Socket/UdpRevCast.cpp \
    UI/WidgeVHF781tcp.cpp


HEADERS  += \
    Audio/AudioControl.h \
    Audio/AudioPlayer.h \
    Audio/AudioRecoder.h \
    Audio/AudioMixer.h \
    Audio/AudioUtil.h \
    Socket/SocketManage.h \
    Socket/UdpVoice.h \
    Socket/UdpRctrl.h \
    Uart/UartManage_bk.h \
    Uart/qextserialport.h \
    Uart/qextserialport_global.h \
    Uart/qextserialport_p.h \
    Keyboard/KeyUart.h \
    Config/ConfigLoader.h \
    Config/BoxConf.h \
    UI/UIManager.h \
    UI/WidgeBase.h \
    UI/WidgeSSB.h \
    Util/RadioUtil.h \
    UI/WidgeVHF181d.h \
    Socket/SocketCommu.h \
    UI/WidgeMulti.h \
    Socket/UdpRevCast.h \
    UI/WidgeVHF781tcp.h


FORMS    += \
    UI/VhfCtrl.ui \
    UI/VhfVoice.ui \
    UI/SsbCtrl.ui \
    UI/SsbVoice.ui

DISTFILES += \
    Uart/qextserial/LICENSE \
    images/voice.gif \
    images/processBar.png \
    images/voice_green.gif
