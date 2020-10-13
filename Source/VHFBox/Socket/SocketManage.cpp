#include "SocketManage.h"
#include "Config/ConfigLoader.h"
#include "Config/BoxConf.h"
#include "UI/UIManager.h"
#include "UI/WidgeBase.h"


SocketManage* SocketManage::m_instance = NULL;
QMutex SocketManage::m_Mutex;

SocketManage::SocketManage()
{
    rctrlUdp1 = NULL;
    rctrlUdp2 = NULL;
    rctrlUdp3 = NULL;
    rctrlUdp4 = NULL;

    voiceUdp1 = NULL;
    voiceUdp2 = NULL;
    voiceUdp3 = NULL;
    voiceUdp4 = NULL;

    posRevUdp = NULL;
}

SocketManage::~SocketManage()
{

    delete rctrlUdp1;
    delete rctrlUdp2;
    delete rctrlUdp3;
    delete rctrlUdp4;

    delete voiceUdp1;
    delete voiceUdp2;
    delete voiceUdp3;
    delete voiceUdp4;

    delete posRevUdp;
}

SocketManage* SocketManage::getInstance()
{

    QMutexLocker locker(&m_Mutex);
    if (m_instance == NULL)
    {
        m_instance = new SocketManage();
    }

    return m_instance;
}

bool SocketManage::init()
{


    BoxConf *boxConf1 = ConfigLoader::getInstance()->getBoxConf1();
    BoxConf *boxConf2 = ConfigLoader::getInstance()->getBoxConf2();
    BoxConf *boxConf3 = ConfigLoader::getInstance()->getBoxConf3();
    BoxConf *boxConf4 = ConfigLoader::getInstance()->getBoxConf4();

    if(boxConf1->getBoxEnable()){

        QString sndToIP = boxConf1->getRadioIP();
        int cRecivPort  = boxConf1->getBoxCtrlPort();
        int cSndToPort  = boxConf1->getRadioCtrlPort();
        int vRecivPort  = boxConf1->getBoxVoicPort();
        int vSndToPort  = boxConf1->getRadioVoicPort();
        int devID       = boxConf1->getBoxID();
        int radioTyp    = boxConf1->getRadioTyp();
        int priority    = boxConf1->getBoxPriority();

        qDebug() <<"boxConf1" << "sndToIP---------------"       << sndToIP;
        qDebug() <<"boxConf1" << "cRecivPort---------------"    << cRecivPort;
        qDebug() <<"boxConf1" << "cSndToPort---------------"    << cSndToPort;
        qDebug() <<"boxConf1" << "vRecivPort---------------"    << vRecivPort;
        qDebug() <<"boxConf1" << "vSndToPort---------------"    << vSndToPort;
        qDebug() <<"boxConf1" << "devID-----------------"       << devID;
        qDebug() <<"boxConf1" << "priority-----------------"    << priority;

        voiceUdp1 = new UDPVoice();
        voiceUdp1->setIndex(1);
        voiceUdp1->setDevID(devID);
        voiceUdp1->setRadioTyp(radioTyp);
        voiceUdp1->setPriority(priority);
        voiceUdp1->init(vRecivPort, sndToIP, vSndToPort);

        rctrlUdp1 = new UDPRctrl();
        rctrlUdp1->setIndex(1);
        rctrlUdp1->setDevID(devID);
        rctrlUdp1->setRadioTyp(radioTyp);
        rctrlUdp1->setPriority(priority);
        rctrlUdp1->init(cRecivPort, sndToIP, cSndToPort);


    } else {
        voiceUdp1 = NULL;
        rctrlUdp1 = NULL;
    }

    if(boxConf2->getBoxEnable()){

        QString sndToIP = boxConf2->getRadioIP();
        int cRecivPort  = boxConf2->getBoxCtrlPort();
        int cSndToPort  = boxConf2->getRadioCtrlPort();
        int vRecivPort  = boxConf2->getBoxVoicPort();
        int vSndToPort  = boxConf2->getRadioVoicPort();
        int devID       = boxConf2->getBoxID();
        int radioTyp    = boxConf2->getRadioTyp();
        int priority    = boxConf2->getBoxPriority();

        qDebug() <<"boxConf2" << "sndToIP---------------"       << sndToIP;
        qDebug() <<"boxConf2" << "cRecivPort---------------"    << cRecivPort;
        qDebug() <<"boxConf2" << "cSndToPort---------------"    << cSndToPort;
        qDebug() <<"boxConf2" << "vRecivPort---------------"    << vRecivPort;
        qDebug() <<"boxConf2" << "vSndToPort---------------"    << vSndToPort;
        qDebug() <<"boxConf1" << "devID-----------------"       << devID;
        qDebug() <<"boxConf2" << "priority-----------------"    << priority;

        voiceUdp2 = new UDPVoice();
        voiceUdp2->setIndex(2);
        voiceUdp2->setDevID(devID);
        voiceUdp2->setRadioTyp(radioTyp);
        voiceUdp2->setPriority(priority);
        voiceUdp2->init(vRecivPort, sndToIP, vSndToPort);

        rctrlUdp2 = new UDPRctrl();
        rctrlUdp2->setIndex(2);
        rctrlUdp2->setDevID(devID);
        rctrlUdp2->setRadioTyp(radioTyp);
        rctrlUdp2->setPriority(priority);
        rctrlUdp2->init(cRecivPort, sndToIP, cSndToPort);

    } else {
        voiceUdp2 = NULL;
        rctrlUdp2 = NULL;
    }

    if(boxConf3->getBoxEnable()){

        QString sndToIP = boxConf3->getRadioIP();
        int cRecivPort  = boxConf3->getBoxCtrlPort();
        int cSndToPort  = boxConf3->getRadioCtrlPort();
        int vRecivPort  = boxConf3->getBoxVoicPort();
        int vSndToPort  = boxConf3->getRadioVoicPort();
        int devID       = boxConf3->getBoxID();
        int radioTyp    = boxConf3->getRadioTyp();
        int priority    = boxConf3->getBoxPriority();

        qDebug() <<"boxConf3" << "sndToIP---------------"       << sndToIP;
        qDebug() <<"boxConf3" << "cRecivPort---------------"    << cRecivPort;
        qDebug() <<"boxConf3" << "cSndToPort---------------"    << cSndToPort;
        qDebug() <<"boxConf3" << "vRecivPort---------------"    << vRecivPort;
        qDebug() <<"boxConf3" << "vSndToPort---------------"    << vSndToPort;
        qDebug() <<"boxConf3" << "priority-----------------"    << priority;

        voiceUdp3 = new UDPVoice();
        voiceUdp3->setIndex(3);
        voiceUdp3->setDevID(devID);
        voiceUdp3->setRadioTyp(radioTyp);
        voiceUdp3->setPriority(priority);
        voiceUdp3->init(vRecivPort, sndToIP, vSndToPort);

        rctrlUdp3 = new UDPRctrl();
        rctrlUdp3->setIndex(3);
        rctrlUdp3->setDevID(devID);
        rctrlUdp3->setRadioTyp(radioTyp);
        rctrlUdp3->setPriority(priority);
        rctrlUdp3->init(cRecivPort, sndToIP, cSndToPort);

    } else {
        voiceUdp3 = NULL;
        rctrlUdp3 = NULL;
    }

    if(boxConf4->getBoxEnable()){

        QString sndToIP = boxConf4->getRadioIP();
        int cRecivPort  = boxConf4->getBoxCtrlPort();
        int cSndToPort  = boxConf4->getRadioCtrlPort();
        int vRecivPort  = boxConf4->getBoxVoicPort();
        int vSndToPort  = boxConf4->getRadioVoicPort();
        int devID       = boxConf4->getBoxID();
        int radioTyp    = boxConf4->getRadioTyp();
        int priority    = boxConf4->getBoxPriority();

        qDebug() <<"boxConf4" << "sndToIP---------------"       << sndToIP;
        qDebug() <<"boxConf4" << "cRecivPort---------------"    << cRecivPort;
        qDebug() <<"boxConf4" << "cSndToPort---------------"    << cSndToPort;
        qDebug() <<"boxConf4" << "vRecivPort---------------"    << vRecivPort;
        qDebug() <<"boxConf4" << "vSndToPort---------------"    << vSndToPort;
        qDebug() <<"boxConf4" << "priority-----------------"    << priority;

        voiceUdp4 = new UDPVoice();
        voiceUdp4->setIndex(4);
        voiceUdp4->setDevID(devID);
        voiceUdp4->setRadioTyp(radioTyp);
        voiceUdp4->setPriority(priority);
        voiceUdp4->init(vRecivPort, sndToIP, vSndToPort);

        rctrlUdp4 = new UDPRctrl();
        rctrlUdp4->setIndex(4);
        rctrlUdp4->setDevID(devID);
        rctrlUdp4->setRadioTyp(radioTyp);
        rctrlUdp4->setPriority(priority);
        rctrlUdp4->init(cRecivPort, sndToIP, cSndToPort);

    } else {
        voiceUdp4 = NULL;
        rctrlUdp4 = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    int posRevPort = ConfigLoader::getInstance()->getPosRevPort();
    posRevUdp = new UdpRevCast();
    posRevUdp->init(posRevPort);

    return true;
}

void SocketManage::sendVoiceData(AudioData audioData)
{
    if(voiceUdp1 != NULL && pttOn){
        voiceUdp1->sendVoiceData(audioData);
    }
    if(voiceUdp2 != NULL && pttOn){
        voiceUdp2->sendVoiceData(audioData);
    }
    if(voiceUdp3 != NULL && pttOn){
        voiceUdp3->sendVoiceData(audioData);
    }
    if(voiceUdp4 != NULL && pttOn){
        voiceUdp4->sendVoiceData(audioData);
    }

}


UDPVoice* SocketManage::getVoicUdp(int ID)
{
    if(ID == 1){
        return voiceUdp1;
    }
    if(ID == 2){
        return voiceUdp2;
    }
    if(ID == 3){
        return voiceUdp3;
    }
    if(ID == 4){
        return voiceUdp4;
    }
    return NULL;
}


UDPRctrl* SocketManage::getCtrlUdp(int ID)
{
    if(ID == 1){
        return rctrlUdp1;
    }
    if(ID == 2){
        return rctrlUdp2;
    }
    if(ID == 3){
        return rctrlUdp3;
    }
    if(ID == 4){
        return rctrlUdp4;
    }
    return NULL;
}


void SocketManage::registApplySocket(int ID)
{

    UDPRctrl *ctrlUdp = getCtrlUdp(ID);
    if(ctrlUdp != NULL){
        ctrlUdp->registSocket();
    }

    UDPVoice *voicUdp = getVoicUdp(ID);
    if(voicUdp != NULL){
        voicUdp->sendVoiceRegist();
    }
}

void SocketManage::setPttOn(bool value)
{
    pttOn = value;
    sendPtt(pttOn);
}


void SocketManage::sendPtt(bool pttOn)
{
    WidgeBase *base = UIManager::getInstance()->getCurrWidge();
    //qDebug()<< "CurrIndex =" << base->getIndex()<<" ptt:"<<pttOn;
    if(base->getIndex() == 5) {
        if(voiceUdp1 != NULL){
            voiceUdp1->sendVoicePTTSet(pttOn);
        }
        if(voiceUdp2 != NULL){
            voiceUdp2->sendVoicePTTSet(pttOn);
        }
        if(voiceUdp3 != NULL){
            voiceUdp3->sendVoicePTTSet(pttOn);
        }
        if(voiceUdp4 != NULL){
            voiceUdp4->sendVoicePTTSet(pttOn);
        }
    } else {
        if(base->getIndex() == 1 && voiceUdp1 != NULL){
            voiceUdp1->sendVoicePTTSet(pttOn);
        }
        if(base->getIndex() == 2 && voiceUdp2 != NULL){
            voiceUdp2->sendVoicePTTSet(pttOn);
        }
        if(base->getIndex() == 3 && voiceUdp3 != NULL){
            voiceUdp3->sendVoicePTTSet(pttOn);
        }
        if(base->getIndex() == 4 && voiceUdp4 != NULL){
            voiceUdp4->sendVoicePTTSet(pttOn);
        }
    }

}


//获取本机IP
QString SocketManage::getLocalIPAddr()
{
    QString ipAddr;

    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, AddressList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address != QHostAddress::Null &&
                address != QHostAddress::LocalHost)
        {
            if (address.toString().contains("127.0.")){
                continue;
            }
            ipAddr = address.toString();
        }
    }
//    qDebug() << "ip:                              " << ipAddr;
    return ipAddr;
}




