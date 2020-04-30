#include "UdpRctrl.h"
#include <QTime>
#include <QDebug>
#include "Config/ConfigLoader.h"
#include "Socket/SocketManage.h"
#include "UI/UIManager.h"
#include "UI/WidgeBase.h"

UDPRctrl::UDPRctrl()
{
    priority = 1;
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
       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size());


       char* data = datagram.data();
       int nLen   = datagram.size();

       MSG_HEADER msgHeader;
       memcpy(&msgHeader,data,sizeof(MSG_HEADER));
       int currLen = sizeof(MSG_HEADER);

       if (msgHeader.msgTyp == MSG_TYP_CTRL){

           if(msgHeader.funCod == Dev_regist_Ack){

//               qDebug() << " Recv Ctrl REGIST_STATE ------------!!!";

               int bodyLen = sizeof(DEV_REGIST_ACK);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){                

                   DEV_REGIST_ACK regState;
                   memcpy(&regState, data + currLen, bodyLen);

//                   qDebug() << " Recv Ctrl REGIST_STATE DevID    :" << msgHeader.DevID;
//                   qDebug() << " Recv Ctrl REGIST_STATE RegState :" << regState.regState;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){

                       widget->setRegistAck(regState.regState);
                   }
               }
           }

           if(msgHeader.funCod == Ack_State){

               int bodyLen = sizeof(RADIO_STATE);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){

                   RADIO_STATE ackState;
                   memcpy(&ackState, data + currLen, bodyLen);

//                   qDebug() << " Recv ACKWORKMODE radioID   :" << ackState.RadioID;
//                   qDebug() << " Recv ACKWORKMODE cmd       :" << ackState.funCod;
//                   qDebug() << " Recv ACKWORKMODE workMode  :" << ackState.workMod;
//                   qDebug() << " Recv ACKCHAN Channel       :" << ackState.channel;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){
                       widget->uptChannel(ackState.channel);        //工作信道
                       widget->uptWorkMode(ackState.workMod);       //工作模式
                       widget->uptRevFreq(ackState.rxFreq);         //接受频率
                       widget->uptSndFreq(ackState.txFreq);         //发射频率
                       widget->uptPower(ackState.power);            //发射功率
                       widget->uptSquelch(ackState.squelch);        //静噪等级
                       widget->uptStatus(ackState.errState);
                   }
                }
            }

           if(msgHeader.funCod == Dev_restrict_Ack){

//               qDebug() << " Recv Dev_restrict_Ack ------------!!!";

               int bodyLen = sizeof(DEV_RESTRICT_ACK);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){

                   DEV_RESTRICT_ACK restrictACK;
                   memcpy(&restrictACK, data + currLen, bodyLen);

//                   qDebug() << " Recv Dev_restrict_Ack----ctrlOut:" << index << ":" << restrictACK.ctrlOut;
//                   qDebug() << " Recv Dev_restrict_Ack----ctrlIn :" << index << ":" << restrictACK.ctrlIn;
//                   qDebug() << " Recv Dev_restrict_Ack----voicOut:" << index << ":" << restrictACK.voicOut;
//                   qDebug() << " Recv Dev_restrict_Ack----voicIn :" << index << ":" << restrictACK.voicIn;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){
                       widget->setCtrlOutRestrict(restrictACK.ctrlOut);
                       widget->setVoicOutRestrict(restrictACK.voicOut);
                       widget->setCtrlInRestrict(restrictACK.ctrlIn);
                       widget->setVoicInRestrict(restrictACK.voicIn);
                   }
               }
           }
        }
    }
}



void UDPRctrl::registSocket()
{

    int dataLen = sizeof(MSG_HEADER) + sizeof(DEV_REGIST);

    time_t ltime;
    time( &ltime );

    MSG_HEADER	msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_CTRL;
    msgHeader.DevID       = devID;
    msgHeader.RadioTyp    = radioTyp;
    msgHeader.funCod      = Dev_regist;

    QString localIP = SocketManage::getInstance()->getLocalIPAddr();

    DEV_REGIST ctrlRegist;
    memcpy(ctrlRegist.NetIPAddr, localIP.toStdString().c_str(), 16);
    ctrlRegist.NetPort   = m_recivPort;;

    char sendData[dataLen];
    memcpy(sendData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(sendData + sizeof(MSG_HEADER), &ctrlRegist, sizeof(DEV_REGIST));

    m_udpSocket->writeDatagram(sendData, dataLen, m_sndToIP, m_sndToPort);

}


void UDPRctrl::sendRadioCtrl(int setTyp, uint64_t value)
{
    int dataLen = sizeof(MSG_HEADER) + sizeof(RADIO_SET);

    time_t ltime;
    time( &ltime );

    MSG_HEADER	msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_CTRL;
    msgHeader.DevID       = devID;
    msgHeader.RadioTyp    = radioTyp;
    msgHeader.funCod      = setTyp;

    RADIO_SET radioSet;
    memset(&radioSet, 0, sizeof(RADIO_SET));

    bool toSend = false;
    if(setTyp == Set_WorkTyp){
        radioSet.workTyp = value;
        toSend = true;

    } else if(setTyp == Set_WorkMod){
        radioSet.workMod = value;
        toSend = true;

    } else if(setTyp == Set_Channel){
        radioSet.channel = value;
        toSend = true;

    } else if(setTyp == Set_TxFreq){
        radioSet.txFreq = value;
        toSend = true;

    } else if(setTyp == Set_RxFreq){
        radioSet.rxFreq = value;
        toSend = true;

    } else if(setTyp == Set_Power){
        radioSet.power = value;
        toSend = true;

    } else if(setTyp == Set_Squelch){
        radioSet.squelch = value;
        toSend = true;

    } else if(setTyp == Ask_State){
        toSend = true;

    }

    if(toSend){
        char sendData[dataLen];
        memcpy(sendData, &msgHeader, sizeof(MSG_HEADER));
        memcpy(sendData + sizeof(MSG_HEADER), &radioSet, sizeof(RADIO_SET));

        m_udpSocket->writeDatagram(sendData, dataLen, m_sndToIP, m_sndToPort);
    }

}


void UDPRctrl::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}

