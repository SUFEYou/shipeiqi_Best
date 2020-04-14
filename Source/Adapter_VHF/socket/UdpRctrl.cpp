#include "UdpRctrl.h"
#include <QTime>
#include <QDebug>
#include <QLatin1String>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include <socket/SocketCommu.h>
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


       MSG_HEADER msgHeader;
       memcpy(&msgHeader,data,sizeof(MSG_HEADER));
       int currLen = sizeof(MSG_HEADER);

       if (msgHeader.msgTyp == MSG_TYP_CTRL)
       {
            if(msgHeader.funCod == Dev_regist){                                                 //通道注冊

                qDebug()<<"Recv Regist Cmd--------------!!!";

                DEV_REGIST devRegist;
                memcpy(&devRegist,data + currLen, sizeof(DEV_REGIST));

                int timestamp = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级
//                qDebug()<<"Recv Regist Cmd--------------timestamp"<< timestamp;

                CTRL_REGIST_VO regPipe;
                regPipe.DevID   = msgHeader.DevID;                                            //
                regPipe.NetIPAddr = QString(QLatin1String(devRegist.NetIPAddr));                //注册IP
                regPipe.NetPort   = devRegist.NetPort;                                          //注册Port
                regPipe.uptTime   = timestamp;                                                  //跟新时间戳(秒级)


                registCtrl(regPipe);

            }

            if(msgHeader.funCod == Set_WorkTyp
             ||msgHeader.funCod == Set_WorkMod
             ||msgHeader.funCod == Set_Channel
             ||msgHeader.funCod == Set_TxFreq
             ||msgHeader.funCod == Set_RxFreq
             ||msgHeader.funCod == Set_Power
             ||msgHeader.funCod == Set_Squelch
             ||msgHeader.funCod == Ask_State){

                int ctrlDataLen = sizeof(RADIO_SET);
                if(nLen == sizeof(MSG_HEADER) + ctrlDataLen){

                    char ctrlData[ctrlDataLen];
                    memcpy(ctrlData, data + currLen, ctrlDataLen);


                    RadioManage::getInstance()->writeCtrlData(msgHeader.funCod, ctrlData, ctrlDataLen);
                }

            }
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

    if(ackTyp == Ack_State){                          //工作转台

        time_t ltime;
        time( &ltime );
        int  sndLen = sizeof(MSG_HEADER) + sizeof(RADIO_STATE);
        char sndData[sndLen];

        MSG_HEADER msgHeader;
        msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
        msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
        msgHeader.msgTyp      = MSG_TYP_CTRL;
        msgHeader.DevID       = ConfigLoader::getInstance()->getRadioID();
        msgHeader.RadioTyp    = ConfigLoader::getInstance()->getRadioTyp();
        msgHeader.funCod      = Ack_State;

        memcpy(sndData, &msgHeader, sizeof(MSG_HEADER));
        memcpy(sndData + sizeof(MSG_HEADER), pData, sizeof(RADIO_STATE));

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


