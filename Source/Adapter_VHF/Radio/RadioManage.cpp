#include "RadioManage.h"
#include "config/ConfigLoader.h"
#include <QDebug>
#include <VHFLayer/CE_VHFNodeManage.h>
#include "socket/SocketManage.h"

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

        radio181D = new Radio181D();
        radio181D->serialInit();
    }

    if(curRadioTyp == RADIO_171AL){

        radio171AL = new Radio171AL();
        radio171AL->serialInit();

    }
}

void RadioManage::writeLinkData(char* pChar,int nLen)
{
    qDebug() << "Com Send size: " << nLen;

    if(curRadioTyp == RADIO_181D){

        radio181D->writeLinkData(pChar, nLen);
    }

    if(curRadioTyp == RADIO_171AL){

        radio171AL->writeLinkData(pChar, nLen);

    }
}

void RadioManage::writeCtrlData(uint16_t ctrlTyp, char* pChar,int nLen)
{


    if(curRadioTyp == RADIO_181D){

        radio181D->writeCtrlData(ctrlTyp, pChar, nLen);
    }

    if(curRadioTyp == RADIO_171AL){

        radio171AL->writeCtrlData(ctrlTyp, pChar, nLen);

    };
}

void RadioManage::onCtrlAck(uint16_t ackTyp, char* pChar,int nLen){
    SocketManage::getInstance()->getCtrlUdp()->sendCtrlAck(ackTyp, pChar, nLen);
}

void RadioManage::onRecvLinkData(QByteArray data)
{

    CE_VHFNodeManage::getInstance()->OnCommRecData(data);

}
