QT -= gui
QT += network

#CONFIG += console
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
    Radio/Radio212.cpp \
    Radio/Radio171al.cpp \
    socket/UdpRctrl.cpp \
    socket/UdpVoice.cpp \
    socket/TcpClient.cpp \
    socket/TCPDataProcess.cpp\
    RadioLink/RadioLink.cpp \
    RadioLink/LinkCommon.cpp \
    RadioLink/RadioLinkManage.cpp \
    RadioLink/RadioLinkClient.cpp \
    RadioLink/RadioLinkMaster.cpp

HEADERS += \
    socket/SocketManage.h \
    config/ConfigLoader.h \
    Uart/qextserialport.h \
    Uart/qextserialport_global.h \
    Uart/qextserialport_p.h \
    Radio/Radio181d.h \
    Radio/RadioManage.h \
    Radio/Radio212.h \
    Radio/Radio171al.h \
    socket/UdpRctrl.h \
    socket/UdpVoice.h \
    socket/TcpClient.h \
    socket/socketcommon.h \
    socket/TCPDataProcess.h \
    RadioLink/RadioLink.h \
    RadioLink/LinkCommon.h \
    RadioLink/RadioLinkManage.h \
    RadioLink/RadioLinkClient.h \
    RadioLink/RadioLinkMaster.h

win32:{
    SOURCES += Uart/qextserialport_win.cpp

#    HEADERS +=
}

unix:{
    SOURCES += Uart/qextserialport_unix.cpp
}
