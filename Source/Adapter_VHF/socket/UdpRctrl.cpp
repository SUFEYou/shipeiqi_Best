#include "UdpRctrl.h"
#include <QTime>
#include <QDebug>
#include <QLatin1String>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include <socket/socketcommon.h>
#include <time.h>



UDPRctrl::UDPRctrl()
{

}

void UDPRctrl::init(int port)
{
    this->m_Port = port;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(QHostAddress::Any, m_Port, QUdpSocket::ShareAddress);

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

                CTRL_REGIST_VO regPipe;
                regPipe.RadioID   = netRegist.RadioID;                                          //
                regPipe.NetIPAddr = QString(QLatin1String(netRegist.NetIPAddr));                //注册IP
                regPipe.NetPort   = netRegist.NetPort;                                          //注册Port
                regPipe.uptTime   = timestamp;                                                  //跟新时间戳(秒级)


                registCtrl(regPipe);

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

    // 注册通道发送
    int curTime = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    regMutex.lock();
    foreach(CTRL_REGIST_VO regVO,regList)
    {
        int diffS = curTime - regVO.uptTime;
        if(diffS <= 30){
           m_udpSocket->writeDatagram(pData, nLen, QHostAddress(regVO.NetIPAddr), regVO.NetPort);

        } else {

//           qDebug()<<"Ctrl Regist Delete --------------Radio ID"  << regVO.RadioID;
//           qDebug()<<"Ctrl Regist Delete --------------IP"        << regVO.NetIPAddr;
//           qDebug()<<"Ctrl Regist Delete --------------Port"      << regVO.NetPort;
//           qDebug()<<"Ctrl Regist Delete --------------UptTime"   << regVO.uptTime;

           regList.removeOne(regVO);
        }
    }
    regMutex.unlock();
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

        sendData(sndData, sndLen);

    }

}

void UDPRctrl::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}


void UDPRctrl::registCtrl(CTRL_REGIST_VO registVO){

    /////////////////////////////////////////////////////////////////////////
    bool haveReg = false;
    for(int i=0; i<regList.size(); i++){
        CTRL_REGIST_VO regVO = regList.at(i);

        if(regVO == registVO){

            regVO.uptTime = registVO.uptTime;
            regList.replace(i, regVO);

//            qDebug()<<"Ctrl Regist Update --------------Radio ID"  << regVO.RadioID;
//            qDebug()<<"Ctrl Regist Update --------------IP"        << regVO.NetIPAddr;
//            qDebug()<<"Ctrl Regist Update --------------Port"      << regVO.NetPort;
//            qDebug()<<"Ctrl Regist Update --------------UptTime"   << regVO.uptTime;

            haveReg = true;
            break;
        }
    }

    regMutex.lock();
    if(!haveReg){

        regList.append(registVO);

//        qDebug()<<"Ctrl Regist Add --------------Radio ID"  << registVO.RadioID;
//        qDebug()<<"Ctrl Regist Add --------------IP"        << registVO.NetIPAddr;
//        qDebug()<<"Ctrl Regist Add --------------Port"      << registVO.NetPort;
//        qDebug()<<"Ctrl Regist Add --------------UptTime"   << registVO.uptTime;

    }
    regMutex.unlock();
    /////////////////////////////////////////////////////////////////////////
}


