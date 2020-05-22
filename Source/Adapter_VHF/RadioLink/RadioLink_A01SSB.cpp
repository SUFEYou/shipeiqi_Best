#include "RadioLink_A01SSB.h"
#include "RadioLinkManage.h"
#include <QDebug>

RadioLink_A01SSB::RadioLink_A01SSB()
                 : m_bAvailable(false)
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

void RadioLink_A01SSB::recvData(const char* pchar,const int nlength)
{
    //发送ID(4字节)+接收ID(4字节)+序号(4字节)+数据类型(1字节)
    if (nlength < 13)
        return;
    qint32 sendID = pchar[0]<<24 | pchar[1]<<16 | pchar[2]<<8 | pchar[3];
    qint32 recvID = pchar[4]<<24 | pchar[5]<<16 | pchar[6]<<8 | pchar[7];
    qint32 serial = pchar[8]<<24 | pchar[9]<<16 | pchar[10]<<8 | pchar[11];
    char type = pchar[12];
    qDebug() << "sendID: " << sendID << ", recvID: " << recvID << ", serial: " << serial << ", type: " << type;
    //报文 文件
    if (type == 0 || type == 1)
    {
        RadioLinkManage::getInstance()->PackToSendRMTtoRSCMessageData(sendID, recvID, (char*)(&pchar[12]), nlength-12, false);
        //接收到报文后，发送回复报
        packageData(2, sendID, recvID, serial, NULL, 0);
    }
    else if (type == 2)//回复报
    {
        RadioLinkManage::getInstance()->RMTtoRSCMessageSerial(sendID, serial);
    }
}

void RadioLink_A01SSB::timerProcess()
{
    static uint8_t sendCnt = 0;
    //定时器一个周期为200ms,sendCnt控制发送频率，暂定为3s
    if (sendCnt < 15)
    {
        pVHFMsg msg = RadioLinkManage::getInstance()->sendDataFromListWait_A01SSB();
        ++sendCnt;
        if (!msg.isNull())
        {
            packageData(msg->pData[0], msg->nSource, msg->nReceive, msg->nSerial, &(msg->pData[1]), msg->nDataLen-1);
            msg->nSendtimes += 1;
        }

    }
    else
        sendCnt = 0;
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
    tmp[offset++] = type;
    //数据
    memcpy(&tmp[offset], data, datalen);
    offset += datalen;
    RadioLinkManage::getInstance()->PortCommSendOutData(tmp, offset);
}
