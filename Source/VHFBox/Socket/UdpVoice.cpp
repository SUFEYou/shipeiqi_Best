#include "UdpVoice.h"
#include <QDebug>
#include <QTime>
#include <QWaitCondition>
#include "Socket/SocketManage.h"
#include "Audio/AudioPlayer.h"
#include "Config/ConfigLoader.h"
#include "UI/UIManager.h"
#include "UI/WidgeVHF181d.h"



UDPVoice::UDPVoice()
{
    priority = 1;
}


void UDPVoice::init(int recivPort, QString sndToIP, int sndToPort)
{
    this->m_sndToIP   = sndToIP;
    this->m_recivPort = recivPort;
    this->m_sndToPort = sndToPort;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(QHostAddress::Any, m_recivPort, QUdpSocket::ShareAddress);

}

void UDPVoice::onRev()
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

//       qDebug() << QTime::currentTime().msec() << " Recv UDP Data :" << datagram.size();

       if (msgHeader.msgTyp == MSG_TYP_VOIC){

           if(msgHeader.funCod == Dev_regist_Ack){

//               qDebug() << " Recv Voice REGIST_STATE ------------!!!";

               int bodyLen = sizeof(DEV_REGIST_ACK);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){

                   DEV_REGIST_ACK regState;
                   memcpy(&regState, data + currLen, bodyLen);

//                   qDebug() << " Recv Voice REGIST_STATE DevID    :" << msgHeader.DevID;
//                   qDebug() << " Recv Voice REGIST_STATE RegState :" << regState.regState;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){

                       widget->setRegistAck(regState.regState);
                   }
               }
           }

           if(msgHeader.funCod == PTT_state){

//               qDebug() << " Recv Voice PTT_STATE ------------!!!";

               int bodyLen = sizeof(PTT_STATE);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){

                   PTT_STATE pttState;
                   memcpy(&pttState, data + currLen, bodyLen);

//                   qDebug() << " Recv Voice PTT_STATE DevID    :" << pttState.DevID;
//                   qDebug() << " Recv Voice PTT_STATE PTTState :" << pttState.PttOccupy;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){

                       if(pttState.PttOccupy == 1){         // 1:PttOn 0:PttOff
                           if(devID == pttState.DevID){
                               widget->setPttAck(1);        // 本设备PttOn
                           } else {
                               widget->setPttAck(2);        // 他设备PttOn
                           }
                       } else {
                           widget->setPttAck(0);            // PttOff
                       }

                   }
               }
           }

           if(msgHeader.funCod == Dev_restrict_Ack){

//               qDebug() << " Recv Dev_restrict_Ack ------------!!!";

               int bodyLen = sizeof(DEV_RESTRICT_ACK);
               if(nLen == sizeof(MSG_HEADER) + bodyLen){

                   DEV_RESTRICT_ACK restrictACK;
                   memcpy(&restrictACK, data + currLen, bodyLen);

                   qDebug() << " Recv Dev_restrict_Ack----ctrlOut:" << index << ":" << restrictACK.ctrlOut;
                   qDebug() << " Recv Dev_restrict_Ack----ctrlIn :" << index << ":" << restrictACK.ctrlIn;
                   qDebug() << " Recv Dev_restrict_Ack----voicOut:" << index << ":" << restrictACK.voicOut;
                   qDebug() << " Recv Dev_restrict_Ack----voicIn :" << index << ":" << restrictACK.voicIn;

                   WidgeBase *widget = UIManager::getInstance()->getWidge(index);
                   if(widget != NULL){
                       widget->setCtrlOutRestrict(restrictACK.ctrlOut);
                       widget->setVoicOutRestrict(restrictACK.voicOut);
                       widget->setCtrlInRestrict(restrictACK.ctrlIn);
                       widget->setVoicInRestrict(restrictACK.voicIn);
                   }
               }
           }

           if(nLen == 175) {


               WidgeBase *curWidget = UIManager::getInstance()->getCurrWidge();
               WidgeBase *ownWidget = UIManager::getInstance()->getWidge(index);

//               WidgeVHF181d *widget181d = dynamic_cast<WidgeVHF181d*>(ownWidget);
               bool playOK = false;
               if(curWidget != NULL && ownWidget != NULL){
                    if(ownWidget->getVoicInRestrict() == 0){
                        if(curWidget->getIndex() == 5 || curWidget->getIndex() == index){
                            playOK = true;
                        }
                    }
               }
//               qDebug() << "To Play-----------------" <<playOK;

               if(playOK){
                   VOICE_HEAD voiceHead;
                   memcpy(&voiceHead,data + currLen, sizeof(VOICE_HEAD));
                   currLen += sizeof(VOICE_HEAD);

                   char voiceData[160];
                   memcpy(voiceData, data + currLen, 160);

                   AudioPlayer* player = AudioControl::getInstance()->getPlayer(index);
                   if(player != NULL && radioTyp == msgHeader.RadioTyp){
                      player->addPlayData(voiceData, 160);
                   }
               }
           }
       }
    }
}


void UDPVoice::sendVoiceData(AudioData audioData)
{
    int dataLen = sizeof(MSG_HEADER) + sizeof(VOICE_HEAD) + audioData.dataLen;

//    time_t ltime;
//    time( &ltime );

    MSG_HEADER msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_VOIC;
    msgHeader.DevID       = devID;
    msgHeader.RadioTyp    = radioTyp;
    msgHeader.funCod      = Voice_Data;

    FrameSN ++;

    VOICE_HEAD voiceHead;
    memset(&voiceHead, 0, sizeof(VOICE_HEAD));
    voiceHead.Priority   = priority;
    voiceHead.codec      = 0;
    voiceHead.FrameSN    = FrameSN;
    voiceHead.PttON      = 0x01;

    char voicData[dataLen];
    memcpy(voicData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(voicData + sizeof(MSG_HEADER), &voiceHead, sizeof(VOICE_HEAD));
    memcpy(voicData + sizeof(MSG_HEADER) + sizeof(VOICE_HEAD), audioData.data, audioData.dataLen);

    m_udpSocket->writeDatagram(voicData, dataLen, m_sndToIP, m_sndToPort);

}


void UDPVoice::sendVoicePTTSet(bool pttON)
{

    int dataLen = sizeof(MSG_HEADER) + sizeof(PTT_SET);

    MSG_HEADER msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_VOIC;
    msgHeader.DevID       = devID;
    msgHeader.RadioTyp    = radioTyp;
    msgHeader.funCod      = PTT_set;

    PTT_SET pttSet;
    pttSet.Priority = priority;
    pttSet.PttON    = 0x00;
    if(pttON){
        pttSet.PttON= 0x01;
    }

    char pttData[dataLen];
    memcpy(pttData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(pttData + sizeof(MSG_HEADER), &pttSet, sizeof(PTT_SET));

    m_udpSocket->writeDatagram(pttData, dataLen, m_sndToIP, m_sndToPort);

}


void UDPVoice::sendVoiceRegist()
{
    int dataLen = sizeof(MSG_HEADER) + sizeof(DEV_REGIST);

//    time_t ltime;
//    time( &ltime );

    MSG_HEADER msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_VOIC;
    msgHeader.DevID       = devID;
    msgHeader.RadioTyp    = radioTyp;
    msgHeader.funCod      = Dev_regist;

    QString localIP = SocketManage::getInstance()->getLocalIPAddr();

    DEV_REGIST voiceRegist;
    memcpy(voiceRegist.NetIPAddr, localIP.toStdString().c_str(), 16);
    voiceRegist.NetPort = m_recivPort;

    char applyData[dataLen];
    memcpy(applyData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(applyData + sizeof(MSG_HEADER), &voiceRegist, sizeof(DEV_REGIST));

//    qDebug()<< "Send Voice Apply ----------" << m_sndToIP << "|" << m_sndToPort;

    m_udpSocket->writeDatagram(applyData, dataLen, m_sndToIP, m_sndToPort);

}


void UDPVoice::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}

