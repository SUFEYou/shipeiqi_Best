QT -= gui
QT += network

CONFIG += console
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

SOURCES += main.cpp \
    socket/SocketManage.cpp \
    config/ConfigLoader.cpp \
    socket/TCPDataDecoder.cpp \
    VHFLayer/SC_01Layer.cpp \
    Uart/qextserialport.cpp \
    #Uart/qextserialport_unix.cpp \
    Uart/UartManage.cpp \
    Uart/qextserialport_win.cpp \
    VHFLayer/CSC_01LayerClient.cpp \
    VHFLayer/CSC_01LayerHead.cpp

HEADERS += \
    socket/SocketManage.h \
    config/ConfigLoader.h \
    socket/TCPDataDecoder.h \
    common.h \
    VHFLayer/SC_01Layer.h \
    Uart/qextserialport.h \
    Uart/qextserialport_global.h \
    Uart/qextserialport_p.h \
    Uart/UartManage.h \
    VHFLayer/CSC_01LayerClient.h \
    VHFLayer/CSC_01LayerHead.h
