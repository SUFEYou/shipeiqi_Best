#include "UdpRctrl.h"
#include <QTime>
#include <QDebug>
#include <QLatin1String>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include <socket/socketcommon.h>
#include <time.h>

QMutex               pipeMutex;

UDPRctrl::UDPRctrl()
{

}

void UDPRctrl::init(int recivPort, QString sndToIP, int sndToPort)
{
    this->m_sndToIP   = sndToIP;
    this->m_recivPort = recivPort;
    this->m_sndToPort = sndToPort;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(QHostAddress::Any, m_recivPort, QUdpSocket::ShareAddress);

}


void UDPRctrl::onRev()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
       QByteArray datagram;
       QHostAddress sndAdd;                                     //发送IP地址
       quint16 sndPort;                                         //发送端口

       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size(), &sndAdd, &sndPort);


       char* data = datagram.data();
       int nLen   = datagram.size();

//       qDebug()<<"Recv Snder IP---------------:"<< sndAdd.toString();
//       qDebug()<<"Recv Snder Port-------------:"<< sndPort;


       NET_MSG_HEADER netHeader;
       memcpy(&netHeader,data,sizeof(NET_MSG_HEADER));
       int currLen = sizeof(NET_MSG_HEADER);

       if (netHeader.MessageModel == MessageModel_VHF_Ctrl)
       {
            if(netHeader.MessageType == MessageTyp_VHF_regist){                                 //通道注冊

                qDebug()<<"Recv Regist Cmd--------------!!!";

                NET_REGIST netRegist;
                memcpy(&netRegist,data + currLen, sizeof(NET_REGIST));

                int timestamp = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级
//                qDebug()<<"Recv Regist Cmd--------------timestamp"<< timestamp;

                REGIST_PIPE regPipe;
                regPipe.RadioID   = netRegist.RadioID;                                          //
                regPipe.NetIPAddr = QString(QLatin1String(netRegist.NetIPAddr));                //注册IP
                regPipe.NetPort   = netRegist.NetPort;                                          //注册Port
                regPipe.uptTime   = timestamp;                                                  //跟新时间戳(秒级)


                registPipe(regPipe);

            }

            if(netHeader.MessageType == MessageTyp_VHF_Set_WorkMod){                        //设置工作模式

                int ctrlDataLen = sizeof(VHF_SET_WORKMOD);
                if(nLen == sizeof(NET_MSG_HEADER) + ctrlDataLen){

                    char ctrlData[ctrlDataLen];
                    memcpy(ctrlData, data + currLen, ctrlDataLen);


                    RadioManage::getInstance()->writeCtrlData(netHeader.MessageType, ctrlData, ctrlDataLen);
                }

            }

            if(netHeader.MessageType == MessageTyp_VHF_Set_Channel){                        //设置信道

                int ctrlDataLen = sizeof(VHF_SET_CHANNEL);
                if(nLen == sizeof(NET_MSG_HEADER) + ctrlDataLen){

                    char ctrlData[ctrlDataLen];
                    memcpy(ctrlData, data + currLen, ctrlDataLen);


                    RadioManage::getInstance()->writeCtrlData(netHeader.MessageType, ctrlData, ctrlDataLen);
                }
            }

            if(netHeader.MessageType == MessageTyp_VHF_Set_Channel){                        //设置信道

                int ctrlDataLen = sizeof(VHF_SET_CHANNEL);
                if(nLen == sizeof(NET_MSG_HEADER) + ctrlDataLen){

                    char ctrlData[ctrlDataLen];
                    memcpy(ctrlData, data + currLen, ctrlDataLen);


                    RadioManage::getInstance()->writeCtrlData(netHeader.MessageType, ctrlData, ctrlDataLen);
                }
            }

//            if(netHeader.MessageType == MessageTyp_VHF_Ask_State){                          //状态问询

//                int ctrlDataLen = sizeof(VHF_ASK_STATE);
//                if(nLen == sizeof(NET_MSG_HEADER) + ctrlDataLen){

//                    char ctrlData[ctrlDataLen];
//                    memcpy(ctrlData, data + currLen, ctrlDataLen);


//                    RadioManage::getInstance()->writeCtrlData(netHeader.MessageType, ctrlData, ctrlDataLen);
//                }
//            }
       }
    }
}


void UDPRctrl::sendData(char* pData,int nLen)
{
    // 固定发送
    m_udpSocket->writeDatagram(pData, nLen, m_sndToIP, m_sndToPort);

    // 注册通道发送
    QMutexLocker locker(&pipeMutex);

    for(int i=0; i<regPipeList.size(); i++){

        REGIST_PIPE pipe = regPipeList.at(i);
        m_udpSocket->writeDatagram(pData, nLen, QHostAddress(pipe.NetIPAddr), pipe.NetPort);
    }
}

void UDPRctrl::sendCtrlAck(uint16_t ackTyp, char* pData,int nLen)
{

    if(ackTyp == MessageTyp_VHF_Ack_State){                          //状态回复

        time_t ltime;
        time( &ltime );
        int  sndLen = sizeof(NET_MSG_HEADER) + sizeof(VHF_ACK_STATE);
        char sndData[sndLen];

        NET_MSG_HEADER netHeader;
        netHeader.MessageLen = sndLen;
        netHeader.MessageModel = MessageModel_VHF_Ctrl;
        netHeader.MessageSerial= (unsigned long)ltime;
        netHeader.MessageType  = MessageTyp_VHF_Ack_State;
    //    netHeader.MessageVer;
        netHeader.ProgramID    = ConfigLoader::getInstance()->getProgramID();
        netHeader.ProgramType  = ConfigLoader::getInstance()->getProgramType();

        memcpy(sndData, &netHeader, sizeof(NET_MSG_HEADER));
        memcpy(sndData + sizeof(NET_MSG_HEADER), pData, sizeof(VHF_ACK_STATE));

        // 固定发送
        m_udpSocket->writeDatagram(sndData, sndLen, m_sndToIP, m_sndToPort);

        qDebug() << "Send --------------------------!!!" << m_sndToIP << m_sndToPort;

        // 注册通道发送
        QMutexLocker locker(&pipeMutex);
        for(int i=0; i<regPipeList.size(); i++){

            REGIST_PIPE pipe = regPipeList.at(i);
            m_udpSocket->writeDatagram(sndData, sndLen, QHostAddress(pipe.NetIPAddr), pipe.NetPort);

            qDebug() << "Send --------------------------!!!" << pipe.NetIPAddr << pipe.NetPort;
        }
    }


}

void UDPRctrl::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}


void UDPRctrl::registPipe(REGIST_PIPE regPipe){

    /////////////////////////////////////////////////////////////////////////
    bool havePipe = false;
    for(int i=0; i<regPipeList.size(); i++){
        REGIST_PIPE pipe = regPipeList.at(i);

        if(pipe.RadioID == regPipe.RadioID
           && pipe.NetIPAddr == regPipe.NetIPAddr
           && pipe.NetPort   == regPipe.NetPort){

            qDebug()<<"Update Regist --------------Radio ID"    << regPipe.RadioID;
            qDebug()<<"Update Regist --------------Radio IP"    << regPipe.NetIPAddr;
            qDebug()<<"Update Regist --------------Regist Port" << regPipe.NetPort;

            pipe.uptTime = regPipe.uptTime;
            havePipe = true;
            break;
        }

    }

    QMutexLocker locker(&pipeMutex);
    if(!havePipe){
//        if(m_sndToIP.toString() != regPipe.NetIPAddr && m_sndToPort != regPipe.NetPort){        // 注册通道 != 固定通道
            regPipeList.append(regPipe);

            qDebug()<<"ADD Regist --------------Radio ID"    << regPipe.RadioID;
            qDebug()<<"ADD Regist --------------Radio IP"    << regPipe.NetIPAddr;
            qDebug()<<"ADD Regist --------------Regist Port" << regPipe.NetPort;
//        }
    }
    /////////////////////////////////////////////////////////////////////////
}


