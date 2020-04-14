#include "RadioManage.h"
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "RadioLink/RadioLinkManage.h"
#include "Radio181d.h"
#include "Radio171al.h"
#include "Radio171d.h"
#include "Radio781tcp.h"
#include "Radio212tcr.h"
#include <QDebug>

RadioManage* RadioManage::m_instance = NULL;
QMutex RadioManage::m_mutex;

RadioManage::RadioManage()
{

}

RadioManage::~RadioManage()
{

}

RadioManage* RadioManage::getInstance()
{

    QMutexLocker locker(&m_mutex);
    if (m_instance == NULL)
    {
        m_instance = new RadioManage();
    }

    return m_instance;
}

void RadioManage::init()
{
    curRadioTyp = ConfigLoader::getInstance()->getRadioTyp();

    if(curRadioTyp == RADIO_181D){

        radio = new Radio181D();
    }

    if(curRadioTyp == RADIO_171AL){

        radio = new Radio171AL();
    }

    if(curRadioTyp == RADIO_171D){

        radio = new Radio171D();
    }

    if(curRadioTyp == RADIO_781TCP){

        radio = new Radio781TCP();
    }

    if(curRadioTyp == RADIO_212TCR){

        radio = new Radio212TCR();
    }

    if (radio != NULL)
        radio->serialInit();
}


void RadioManage::writeLinkData(char* pChar,int nLen)
{
    qDebug() << "Com Send size: " << nLen;

    if (radio != NULL)
        radio->writeLinkData(pChar, nLen);
}


void RadioManage::writeCtrlData(uint16_t funCode, char* pChar,int nLen)
{
    if (radio != NULL)
        radio->writeCtrlData(funCode, pChar, nLen);
}


void RadioManage::onCtrlAck(uint16_t funCode, char* pChar,int nLen){
    SocketManage::getInstance()->getCtrlUdp()->sendCtrlAck(funCode, pChar, nLen);
}


void RadioManage::onRecvLinkData(QByteArray data)
{
    RadioLinkManage::getInstance()->OnCommRecData(data);
}

void RadioManage::onRecvLinkData(const char* data, const uint16_t len)
{
    RadioLinkManage::getInstance()->OnCommRecData(data, len);
}
