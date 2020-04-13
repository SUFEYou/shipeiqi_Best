#include "Radio171d.h"
#include <QDebug>

Radio171D::Radio171D()
    : m_timer(new QTimer(this))
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

Radio171D::~Radio171D()
{

}

void Radio171D::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM1");
#else
    dataCom = new QextSerialPort("/dev/ttymxc1");
#endif
    connect(dataCom, SIGNAL(readyRead()), this, SLOT(readDataCom()));
    dataCom->setBaudRate(BAUD38400);    //设置波特率
    dataCom->setDataBits(DATA_8);       //设置数据位
    dataCom->setParity(PAR_NONE);       //设置校验
    dataCom->setStopBits(STOP_1);       //设置停止位
    dataCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    dataCom->setTimeout(10);            //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "171d Serail Open Err!";
    } else {
        qDebug() << "171d Serail Open Success!";
        m_timer->start(1000);
    }
}

void Radio171D::readCom()
{
    dataArray.push_back(dataCom->readAll());
    recvDataSubpackage();
    recvDataParse();
}

int Radio171D::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{
    QMutexLocker locker(&m_ctrlMutex);

    switch (ctrlTyp)
    {
    case MessageTyp_VHF_Set_WorkMod://设置工作模式
        {

        }
    break;
    case MessageTyp_VHF_Set_Channel://设置信道
        {
            int ctrlDataLen = sizeof(VHF_SET_CHANNEL);
            if(len == ctrlDataLen)
            {
                VHF_SET_CHANNEL setChannel;
                memcpy(&setChannel, data, ctrlDataLen);
                sendDataPackage(0x0100, data, len);
            }
        }
    break;
    case MessageTyp_VHF_Ask_State://状态问询
        {

        }
    break;
    case MessageTyp_VHF_Set_Freq://频点设置
        {

        }
    break;
    default:
        {

        }
    break;
    }

    return 0;
}

int Radio171D::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);
    sendDataPackage(0x5500, data, len);

    return 0;
}

void Radio171D::sendDataPackage(uint16_t type, const char* data, const int len)
{
    char tmp[MAXDATALENGTH];
    memset(tmp, 0, MAXDATALENGTH);
    int tmpLen = 0;
    //类型ID
    tmp[0] = (type>>8) & 0xFF;
    tmp[1] = type & 0xFF;
    tmpLen += 2;
    //信息长度
    tmp[2] = (len>>8) & 0xFF;
    tmp[3] = len & 0xFF;
    tmpLen += 2;
    //数据
    memcpy(tmp+4, data, len);
    tmpLen += len;
    //CRC校验
    uint16_t t_crc = getCRC((unsigned char*)tmp, tmpLen);
    tmp[tmpLen] = (t_crc>>8) & 0xFF;
    tmp[tmpLen+1] = t_crc & 0xFF;
    tmpLen += 2;

    char dstData[MAXDATALENGTH];
    memset(dstData, 0, MAXDATALENGTH);
    int dstLen = 0;
    //添加转义
    wConverte(tmp, tmpLen, dstData+1, dstLen);
    //包头包尾
    dstData[0] = 0xC0;
    dstData[dstLen+1] = 0xC0;
    dstLen += 2;

    dataCom->write(dstData, dstLen);
}

void Radio171D::wConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen)
{
    for (int i = 0; i < srcLen; ++i)
    {
        if (srcData[i] == 0xC0)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDC;
            ++dstLen;
        }
        else if (srcData[i] == 0xDB)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDD;
            ++dstLen;
        }
        else
        {
            dstData[dstLen] = srcData[i];
            ++dstLen;
        }
    }
}

void Radio171D::onTimer()
{
    //心跳机制
    static bool flag = false;
    if (!flag)
    {
        flag = true;
        static char str[] = {0xC0, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x0E, 0x03, 0xC0};
        dataCom->write(str, sizeof(str));
    }
    static char str[] = {0xC0, 0x03, 0x02, 0x00, 0x04, 0x12, 0x34, 0x56, 0x78, 0x2D, 0x49, 0xC0};
    dataCom->write(str, sizeof(str));

}

void Radio171D::recvDataSubpackage()
{
    if (dataArray.length() < 8)//8 == 包头(1字节)+类型ID(2字节)+信息长度(2字节)+校验(2字节)+包尾(1字节)
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        for (int i = 0; i < dataArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)dataArray[i] == 0xC0)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)dataArray[i] == 0xC0)
                {
                    if (contLen == 0)//包头丢失，包尾与下一包包头粘连情况（XX XX C0 C0 XX XX XX XX C0）
                    {
                        contBegin = i+1;
                        contLen = 0;
                        continue;
                    }
                    else if (contLen < 6)//包内数据长度小于8字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        m_recvDataList.push_back(dataArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
                        //removeLen = contBegin + contLen + 1;
                        removeLen = contBegin + contLen;
                        step = 0;
                        continue;
                    }
                }
                else
                {
                    ++contLen;
                }
            }

        }
        dataArray.remove(0, removeLen);
    }
}

void Radio171D::recvDataParse()
{
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        //去除转义字符
        char dstData[MAXDATALENGTH];
        memset(dstData, 0, MAXDATALENGTH);
        int dstLen = 0;
        rConverte(tmpArray.data(), tmpArray.length(), dstData, dstLen);
        //CRC校验
        uint16_t t_crc = getCRC((unsigned char*)dstData, dstLen-2);
        uint16_t recv_crc = (dstData[dstLen-2]<<8) | (dstData[dstLen-1]);
        if ( t_crc != recv_crc)
        {
            qDebug() << "In Radio171AL::recvDataParse(), CRC Err";
            return;
        }
        //数据长度校验
        uint16_t msgLen = (dstData[2]<<8) | (dstData[3]);
        //dstLen-6 == 数据域长度 == dstLen - 类型ID(2字节) - 信息长度(2字节) - 校验(2字节)
        if (msgLen != (dstLen-6))
        {
            qDebug() << "In Radio171D::recvDataParse(), Message Len Err";
            return;
        }
        //消息类型
        uint16_t msgType = (dstData[0]<<8) | (dstData[1]);
        //数传
        if (msgType == 0X5500)
        {
            RadioManage::getInstance()->onRecvLinkData(dstData+4, msgLen);
        }
        else
        {
            updateRadioState(msgType, dstData+4, msgLen);
        }
    }
}

void Radio171D::updateRadioState(uint16_t type, const char* data, const int len)
{
    switch (type) {
    case 0X0081://上报时间（0x0081）
        {

        }
    break;
    case 0X0382://上报工作状态信息（0x0382）
        {

        }
    break;
    case 0X0086://上报功率大小（0x0086）
        {

        }
    break;
    case 0X0087://上报音量大小（0x0087）
        {

        }
    break;
    case 0X0180://上报当前信道号（0x0180）
        {

        }
    break;
    default:
        {
            qDebug() << "In Radio171D::updateRadioState, Recv unknown msg type!";
        }
    break;
    }
}

void Radio171D::rConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen)
{
    for (int m = 0;  m < srcLen; ++m)
    {
        if ((unsigned char)srcData[m] != 0xDB)
        {
            dstData[dstLen] = srcData[m];
            ++dstLen;
        }
        else
        {
            if ((unsigned char)srcData[m+1] == 0xDC)
            {
                dstData[dstLen] = 0xC0;
                ++dstLen;
                ++m;
            }
            else if ((unsigned char)srcData[m+1] == 0xDD)
            {
                dstData[dstLen] = 0xDB;
                ++dstLen;
                ++m;
            }
            else
            {
                dstData[dstLen] = srcData[m];
                ++dstLen;
            }
        }
    }
}

uint16_t Radio171D::getCRC(unsigned char* buf, unsigned int len)
{
    unsigned int i, j;
    uint16_t crc, flag;
    bool escapeCharacter = false;
    crc = 0x0000;

    for (i = 0; i < len; i++)
    {
        if ((buf[i] == 0xDB) && !escapeCharacter)
        {
            escapeCharacter = true;
            continue;
        }
        else
        {
            if ((buf[i] == 0xDC) && escapeCharacter)
            {
                buf[i] = 0xC0;
                escapeCharacter =false;
            }
            else if ((buf[i] == 0xDD) && escapeCharacter)
            {
                buf[i] = 0xDB;
                escapeCharacter =false;
            }
            else if (escapeCharacter)
            {
                return 0;
            }
        }
        crc ^= ((static_cast<unsigned short>(buf[i]))<<8);
        for (j = 0; j < 8; j++)
        {
            flag = crc&0x8000;
            crc <<= 1;
            if (flag)
            {
                crc &= 0xfffe;
                crc ^= 0x8005;
            }
        }
    }
    return crc;
}
