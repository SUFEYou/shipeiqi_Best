#include "UdpVoice.h"
#include <QDebug>
#include <QTime>
#include <QWaitCondition>
#include "socket/SocketManage.h"
#include "config/ConfigLoader.h"
#include "Audio/AudioPlayer.h"
#include "Audio/AudioPtt.h"
#include <time.h>



UDPVoice::UDPVoice()
{

}


void UDPVoice::init(int port)
{

    this->m_Port = port;

    for(int i=0; i<4; i++){
        VOICE_REGIST_VO regVO = regArray[i];
        regVO.regKey    = "";
        regVO.DevID     = -1;
        regVO.NetIPAddr = "";
        regVO.NetPort   = -1;
        regVO.PlayID    = -1;
        regArray[i]     = regVO;
    }

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    qDebug()<<"Voice RevPort   =" << m_Port;

    m_udpSocket->bind(QHostAddress::Any, m_Port, QUdpSocket::ShareAddress);

}

void UDPVoice::onRev()
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

       int radioTyp = ConfigLoader::getInstance()->getRadioTyp();
       QString sessionKey = sndAdd.toString().append(":").append(QString::number(sndPort));

       MSG_HEADER msgHeader;
       memcpy(&msgHeader,data,sizeof(MSG_HEADER));
       int currLen = sizeof(MSG_HEADER);

//       qDebug()<<"Recv Voice Data   1111111111111111111111111111--------------!!!" << nLen;


       if (msgHeader.msgTyp == MSG_TYP_VOIC && msgHeader.RadioTyp == radioTyp)
       {
            if(msgHeader.funCod == Dev_regist){

//                qDebug()<<"Recv Voice Apply--------------!!!";

                DEV_REGIST devRegist;
                memcpy(&devRegist,data + currLen, sizeof(DEV_REGIST));

                int timestamp = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级
//                qDebug()<<"Voice Regist --------------timestamp"   << timestamp;
//                qDebug()<<"Voice Regist --------------Session Key" << sessionKey;

                VOICE_REGIST_VO registVO;
                registVO.DevID     = msgHeader.DevID;                                           //
                registVO.NetIPAddr = QString(QLatin1String(devRegist.NetIPAddr));               //注册IP
                registVO.NetPort   = devRegist.NetPort;                                         //注册Port
                registVO.uptTime   = timestamp;                                                 //跟新时间戳(秒级)
                registVO.PlayID    = 0;

                registVoice(sessionKey, registVO);
            }

            if(msgHeader.funCod == PTT_set){            

                PTT_SET pttSet;
                memcpy(&pttSet,data + currLen, sizeof(PTT_SET));

                uint8_t priority = pttSet.Priority;
                uint8_t pttOn    = pttSet.PttON;

                qDebug()<<"Rev PTT-SET-------------------------" << priority << "_" <<pttOn;

                int playID = getRegistedID(sessionKey);
                if(playID != -1){
#if !WIN32
                    AudioPlayer* player = AudioControl::getInstance()->getPlayer(playID);
                    if(player != NULL && player->isBind()){
  //                      qDebug()<<"-------------------------priority_pttOn" << priority << "_" <<pttOn;
                        AudioControl::getInstance()->getPtt()->setPriority_PttOn(playID, priority, pttOn);
                    }
#endif
                }

            }


            if(nLen == 175) {

     //           qDebug()<<"Recv Voice Data Package--------------!!!";

                VOICE_HEAD voiceHead;
                memcpy(&voiceHead,data + currLen, sizeof(VOICE_HEAD));
                currLen += sizeof(VOICE_HEAD);

                uint8_t priority = voiceHead.Priority;          //优先级:1-255 优先级越大越高
                uint8_t pttOn    = voiceHead.PttON;             //0:PTT-Off 1:PTT-On

                int playID = getRegistedID(sessionKey);
                if(playID != -1){

                    char voiceData[160];
                    memcpy(voiceData, data + currLen, 160);
#if !WIN32
                    AudioPlayer* player = AudioControl::getInstance()->getPlayer(playID);
                    if(player != NULL && player->isBind()){
    //                      qDebug()<<"-------------------------priority_pttOn" << priority << "_" <<pttOn;
                        AudioControl::getInstance()->getPtt()->setPriority_PttOn(playID, priority, pttOn);
                        player->addPlayData(voiceData, 160);
                    }
#endif
                }
            }

       } else if(msgHeader.msgTyp == MSG_TYP_VOIC && msgHeader.RadioTyp != radioTyp){

           if(msgHeader.funCod == Dev_regist){
               DEV_REGIST devRegist;
               memcpy(&devRegist,data + currLen, sizeof(DEV_REGIST));

               QHostAddress netAddr(QString(QLatin1String(devRegist.NetIPAddr)));
               sendRegistState(RegistNG_ErrRTyp,  netAddr, devRegist.NetPort);
           }
       }
    }
}


void UDPVoice::sendData(char* pData,int nLen)
{
//    // 固定发送
//    m_udpSocket->writeDatagram(pData, nLen, m_sndToIP, m_sndToPort);

    // 注册通道发送
    int curTime = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级
    for(int i=0; i<4; i++){
        VOICE_REGIST_VO regVO = regArray[i];

        if(regVO.DevID >= 0){

            int diffS = curTime - regVO.uptTime;
            if(diffS <= 30){

              m_udpSocket->writeDatagram(pData, nLen, QHostAddress(regVO.NetIPAddr), regVO.NetPort);
//              qDebug()<<"Voice Data Send -----------------------------" << regVO.NetIPAddr << ":" << regVO.NetPort;

            } else {
#if !WIN32
                AudioControl::getInstance()->unbindPlayID(regVO.PlayID);
#endif
                regVO.regKey = "";
                regVO.DevID  = -1;
                regVO.NetIPAddr = "";
                regVO.NetPort   = -1;
                regVO.PlayID    = -1;
                regArray[i] = regVO;

            }
        }
    }
}


void UDPVoice::sendVoiceData(AudioData audioData)
{
    int dataLen = sizeof(MSG_HEADER) + sizeof(VOICE_HEAD) + audioData.dataLen;

    time_t ltime;
    time( &ltime );

    MSG_HEADER msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_VOIC;
    msgHeader.DevID       = ConfigLoader::getInstance()->getRadioID();
    msgHeader.RadioTyp    = ConfigLoader::getInstance()->getRadioTyp();
    msgHeader.funCod      = Voice_Data;

    FrameSN ++;

    VOICE_HEAD voiceHead;
    voiceHead.Priority   = 0;
    voiceHead.codec      = 0;
    voiceHead.FrameSN    = FrameSN;
    voiceHead.PttON      = 0;

    char voicData[dataLen];
    memcpy(voicData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(voicData + sizeof(MSG_HEADER), &voiceHead, sizeof(VOICE_HEAD));
    memcpy(voicData + sizeof(MSG_HEADER) + sizeof(VOICE_HEAD), audioData.data, audioData.dataLen);

    sendData(voicData, dataLen);

}

void UDPVoice::sendRegistState(uint8_t regState, QHostAddress netAddr, uint32_t netPort)
{
    int  sndLen = sizeof(MSG_HEADER) + sizeof(DEV_REGIST_ACK);
    char sndData[sndLen];

    MSG_HEADER msgHeader;
    msgHeader.ProgramType = ConfigLoader::getInstance()->getProgramType();
    msgHeader.ProgramID   = ConfigLoader::getInstance()->getProgramID();
    msgHeader.msgTyp      = MSG_TYP_VOIC;
    msgHeader.DevID       = ConfigLoader::getInstance()->getRadioID();
    msgHeader.RadioTyp    = ConfigLoader::getInstance()->getRadioTyp();
    msgHeader.funCod      = Dev_regist_Ack;

    DEV_REGIST_ACK registACK;
    registACK.regState = regState;

    memcpy(sndData, &msgHeader, sizeof(MSG_HEADER));
    memcpy(sndData + sizeof(MSG_HEADER), &registACK, sizeof(DEV_REGIST_ACK));

    m_udpSocket->writeDatagram(sndData, sndLen, netAddr, netPort);

}


void UDPVoice::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}


int UDPVoice::getRegistedID(QString sessionKey)
{
    int playID = -1;
    for(int i=0; i<4; i++){
        VOICE_REGIST_VO regVO = regArray[i];

        if(regVO.regKey == sessionKey){

            playID = regVO.PlayID;
            break;
        }
    }

    return playID;
}


void UDPVoice::registVoice(QString sessionKey, VOICE_REGIST_VO registVO){

    /////////////////////////////////////////////////////////////////////////

    bool occupyOK = false;
    bool haveReg  = false;
    for(int i=0; i<4; i++){
        VOICE_REGIST_VO regVO = regArray[i];

        if(regVO.regKey == sessionKey){
            regVO.uptTime = registVO.uptTime;
            regArray[i] = regVO;
            haveReg  = true;
            occupyOK = true;

//            qDebug()<<"Voice Regist Update--------------ID"     << regArray[i].DevID;
//            qDebug()<<"Voice Regist Update--------------IP"     << regArray[i].NetIPAddr;
//            qDebug()<<"Voice Regist Update--------------Port"   << regArray[i].NetPort;
//            qDebug()<<"Voice Regist Update--------------UptTim" << regArray[i].uptTime;

            break;
        }
    }

#if !WIN32
    if(!haveReg){

        for(int i=0; i<4; i++){
            VOICE_REGIST_VO regVO = regArray[i];

            if(regVO.DevID < 0){
                int playID = AudioControl::getInstance()->bindPlayID();

                if(playID > 0){
                    registVO.regKey = sessionKey;
                    registVO.PlayID = playID;
                    regArray[i] = registVO;

                    qDebug()<<"Voice Regist Add--------------PlayID" << regArray[i].PlayID;
                    qDebug()<<"Voice Regist Add--------------ID"     << regArray[i].DevID;
                    qDebug()<<"Voice Regist Add--------------IP"     << regArray[i].NetIPAddr;
                    qDebug()<<"Voice Regist Add--------------Port"   << regArray[i].NetPort;
                    qDebug()<<"Voice Regist Add--------------UptTim" << regArray[i].uptTime;

                    occupyOK = true;
                    break;
                }
            }
        }


        if(occupyOK){

            QHostAddress netAddr(registVO.NetIPAddr);
            sendRegistState(RegistOK,  netAddr, registVO.NetPort);
        } else {

            QHostAddress netAddr(registVO.NetIPAddr);
            sendRegistState(RegistNG_NoMore,  netAddr, registVO.NetPort);
        }
    }

#endif

    /////////////////////////////////////////////////////////////////////////

}




