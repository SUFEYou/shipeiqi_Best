#include "RadioLink_A01SSB.h"
#include "RadioLinkManage.h"
#include <QDebug>
#include "log/log4z.h"

using namespace zsummer::log4z;


#define A01SSB_MSG 100
#define A01SSB_FILE 101

RadioLink_A01SSB::RadioLink_A01SSB()
                 : m_bAvailable(false)
                 , m_nCodeMe(0)
{

}

RadioLink_A01SSB::~RadioLink_A01SSB()
{

}

void RadioLink_A01SSB::SetAvailable(bool available)
{
    m_bAvailable = available;
}

bool RadioLink_A01SSB::GetAvailable()
{
    return m_bAvailable;
}

void RadioLink_A01SSB::setCodeMe(int x)
{
    m_nCodeMe = x;
}

void RadioLink_A01SSB::recvData(char* pchar,const int nlength)
{
    //发送ID(4字节)+接收ID(4字节)+序号(4字节)+数据类型(1字节)
    if (nlength < 13)
        return;
    qint32 sendID = (pchar[0]<<24&0xFF000000) | (pchar[1]<<16&0xFF0000) | (pchar[2]<<8 &0xFF00) | (pchar[3] &0xFF);
    qint32 recvID = (pchar[4]<<24&0xFF000000) | (pchar[5]<<16&0xFF0000) | (pchar[6]<<8 &0xFF00) | (pchar[7] &0xFF);
    qint32 serial = (pchar[8]<<24&0xFF000000) | (pchar[9]<<16&0xFF0000) | (pchar[10]<<8&0xFF00) | (pchar[11]&0xFF);
    char type = pchar[12];
    if (recvID != m_nCodeMe && recvID != BROADCAST_ID)
    {
        return ;
    }
    LOGD(QString("A01 SSB Recv Data, sendID: %1, recvID: %2, serial: %3, type: %4").arg(sendID).arg(recvID).arg(serial).arg(QString::number(type)).toStdString().c_str());
    if (type == 0 || type == 1)
    {
        if (type == 0)
            pchar[12] = A01SSB_MSG;
        else
            pchar[12] = A01SSB_FILE;
        RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(sendID, recvID, (char*)(&pchar[12]), nlength-12, false);
        //接收到报文后，发送回复报
        packageData(2, recvID, sendID, serial, NULL, 0);
    }
    else if (type == 2)//回复报
    {
        LOGD(QString("A01 SSB Recv Recall").toStdString().c_str());
        RadioLinkManage::getInstance()->RMTtoRSCMessageSerial(sendID, serial);
    }
}

void RadioLink_A01SSB::timerProcess()
{
    static uint8_t sendCnt = 0;
    //定时器一个周期为200ms,sendCnt控制发送频率，暂定为3s
    if (sendCnt > 15)
    {
        sendCnt = 0;
        pVHFMsg msg = RadioLinkManage::getInstance()->sendDataFromListWait_A01SSB();
        if (!msg.isNull())
        {
            packageData(msg->pData[0], msg->nSource, msg->nReceive, msg->nSerial, &(msg->pData[1]), msg->nDataLen-1);
            msg->nSendtimes += 1;
            LOGD(QString("A01 SSB Packing Data, Source: %1, Receive: %2, nSerial: %3").arg(msg->nSource).arg(msg->nReceive).arg(msg->nSerial).toStdString().c_str());
        }

    }
    else
        ++sendCnt;
}

void RadioLink_A01SSB::packageData(const char type, const int sendid, const int recvid, const int serial, const char* data, const int datalen)
{
    char tmp[512];
    memset(tmp, 0, sizeof(tmp));
    uint16_t offset = 0;
    //发送ID(4字节)
    tmp[offset++] = (sendid>>24) & 0xFF;
    tmp[offset++] = (sendid>>16) & 0xFF;
    tmp[offset++] = (sendid>>8)  & 0xFF;
    tmp[offset++] =  sendid      & 0xFF;
    //接收ID(4字节)
    tmp[offset++] = (recvid>>24) & 0xFF;
    tmp[offset++] = (recvid>>16) & 0xFF;
    tmp[offset++] = (recvid>>8)  & 0xFF;
    tmp[offset++] =  recvid      & 0xFF;
    //序号(4字节)
    tmp[offset++] = (serial>>24) & 0xFF;
    tmp[offset++] = (serial>>16) & 0xFF;
    tmp[offset++] = (serial>>8)  & 0xFF;
    tmp[offset++] =  serial      & 0xFF;
    //数据类型(1字节)
    if (type == A01SSB_MSG)
        tmp[offset++] = 0;
    else if (type == A01SSB_FILE)
        tmp[offset++] = 1;
    else
        tmp[offset++] = type;
    //数据
    memcpy(&tmp[offset], data, datalen);
    offset += datalen;
    RadioLinkManage::getInstance()->PortCommSendOutData(tmp, offset);
}
