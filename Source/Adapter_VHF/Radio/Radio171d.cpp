#include "Radio171d.h"
#include <QDebug>

Radio171D::Radio171D()
    : m_timer(new QTimer(this))
    , m_connected(false)
    , m_disconnectCnt(0)
    , m_workModFlag(false)
    , m_workMod(0)
    , m_freqFlag(false)
    , m_freq(0)
    , m_squelchFlag(false)
    , m_squelch(0)
{
    memset(&radioState, 0, sizeof(RADIO_STATE));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

Radio171D::~Radio171D()
{
    if(dataCom != NULL){
        delete dataCom;
        dataCom = NULL;
    }
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
    packageData();
    parseData();
}

int Radio171D::writeCtrlData(uint16_t funCode, char* data, int len)
{
    int ctrlDataLen = sizeof(RADIO_SET);
    if(len == ctrlDataLen)
    {
        RADIO_SET m_set;
        memcpy(&m_set, data, ctrlDataLen);
        switch (funCode)
        {
        case Set_WorkMod://设置工作模式
        {
            //由于工作模式包含在信道参数中，设置工作模式需要先查询信道参数，然后获取到信道参数后，只更改信道参数中的工作模式，然后下发
            m_workModFlag = true;
            m_workMod = m_set.workMod;
            writeData(0x0141, NULL, 0);
        }
            break;
        case Set_Channel://设置信道
        {
            writeData(0x0100, (char*)(&m_set.channel), 1);
        }
        break;
        case Set_TxFreq:
        {
            //同设置工作模式
            m_freqFlag = true;
            //设置端显示为KHZ，传输时扩大了10000倍，转换为HZ需要除以10
            m_freq= m_set.txFreq/10;
            writeData(0x0141, NULL, 0);
        }
            break;
        case Set_RxFreq:
        {
            //同设置工作模式
            m_freqFlag = true;
            m_freq= m_set.rxFreq;
            writeData(0x0141, NULL, 0);
        }
            break;
        case Set_Power://设置发射功率
        {
            writeData(0x0005, (char*)(&m_set.power), 1);
        }
            break;
        case Set_Squelch://设置静噪
        {
            m_squelchFlag = true;
            m_squelch = m_set.squelch;
            writeData(0x0048, NULL, 0);
        }
            break;
        case Ask_State://状态问询
        {
            //
        }
            break;
        default:
            break;
        }
    }

    return 0;
}

int Radio171D::writeLinkData(char* data, int len)
{

    writeData(0x5500, data, len);

    return 0;
}

void Radio171D::writeData(uint16_t type, const char* data, const int len)
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
    enCode(tmp, tmpLen, dstData+1, dstLen);
    //包头包尾
    dstData[0] = 0xC0;
    dstData[dstLen+1] = 0xC0;
    dstLen += 2;

    QMutexLocker locker(&m_dataMutex);
    dataCom->write(dstData, dstLen);
}



void Radio171D::onTimer()
{
    QMutexLocker locker(&m_dataMutex);
    //心跳机制
    if (!m_connected)
    {
        static unsigned char str[] = {0xC0, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x0E, 0x03, 0xC0};
        dataCom->write((char*)str, sizeof(str));
    }
    else
    {
        static unsigned char str[] = {0xC0, 0x03, 0x02, 0x00, 0x04, 0x12, 0x34, 0x56, 0x78, 0x2D, 0x49, 0xC0};
        dataCom->write((char*)str, sizeof(str));
        //定时查询状态
        static uint8_t cnt = 0;
        ++cnt;
        if (cnt > 5)
        {
            cnt = 0;
            writeData(0x0048, NULL, 0);
            writeData(0x0141, NULL, 0);
        }
        //断连计数，接收到电台消息时清零，否则每秒递增1，超时后，电台状态为断连
        ++m_disconnectCnt;
        if (m_disconnectCnt > 10)
        {
            radioState.errState = 0;
            m_connected = false;
        }
    }
}

void Radio171D::packageData()
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

void Radio171D::parseData()
{
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        //去除转义字符
        char dstData[MAXDATALENGTH];
        memset(dstData, 0, MAXDATALENGTH);
        int dstLen = 0;
        decode(tmpArray.data(), tmpArray.length(), dstData, dstLen);
        //CRC校验
        uint16_t t_crc = getCRC((unsigned char*)dstData, dstLen-2);
        uint16_t recv_crc = (dstData[dstLen-2]<<8) | (dstData[dstLen-1]);
        if ( t_crc != recv_crc)
        {
            qDebug() << "In Radio171D::recvDataParse(), CRC Err";
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

void Radio171D::updateRadioState(uint16_t type, char* data, const int len)
{
    //接收到电台数据，断连计数清零
    m_disconnectCnt = 0;
    switch (type) {
    case 0X0081://上报时间（0x0081）
    {
    }
        break;
    case 0X0382://上报工作状态信息（0x0382）
    {
        if (!m_connected)
            m_connected = true;
    }
        break;
    case 0X0086://上报功率大小（0x0086）
    {
        if (len > 0)
        radioState.power = data[0];
    }
        break;
    case 0X0087://上报音量大小（0x0087）
    {
    }
        break;
    case 0X0088://上报开关状态（0x0088）
    {
        if (len != 1)
        {
            qDebug() << "Recv on-off state Len Err";
        }
        else
        {
            //更新静噪状态
            if (data[0]&0x01)
                radioState.squelch = 1;
            else
                radioState.squelch = 0;
            //设置开关状态
            if (m_squelchFlag)
            {
                m_squelchFlag = false;
                if (m_squelch)
                    data[0] = data[0] | 0x01;
                else
                    data[0] = data[0] & 0xFE;
                writeData(0x0007, data, len);
            }
        }
    }
        break;
    case 0X0180://上报当前信道号（0x0180）
    {
        if (len > 0)//BCD码转换为整形
            bcd2uint8(data[0], (uint8_t*)(&radioState.channel));
    }
        break;
    case 0X0181://上报当前信道参数（0x0181）
    {
        if (len != 13)
        {
            qDebug() << "Recv Channel Parameters Len Err";
        }
        else
        {
            setChannelParam(data, len);
        }
    }
        break;
    default:
    {
        qDebug() << "In Radio171D::updateRadioState, Recv unknown msg type!";
    }
        break;
    }
}


void Radio171D::enCode(const char* srcData, const int srcLen, char* dstData, int& dstLen)
{
    for (int i = 0; i < srcLen; ++i)
    {
        if ((unsigned char)srcData[i] == 0XC0)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDC;
            ++dstLen;
        }
        else if ((unsigned char)srcData[i] == 0XDB)
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


void Radio171D::decode(const char* srcData, const int srcLen, char* dstData, int& dstLen)
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

void Radio171D::bcd2uint8(uint8_t src, uint8_t* dst)
{
    *dst = (src>>4)*10 + (src&0x0F);
}

void Radio171D::uint82bcd(uint8_t src, uint8_t* dst)
{
    if (src > 99)
        *dst = 0;
    *dst = (src/10)<<4 | (src%10);
}

void Radio171D::setChannelParam(char* data, const int len)
{
    //更新状态
    radioState.workMod = data[2];
    uint8_t m = 0;
    bcd2uint8(data[3], &m);
    quint64 freq = m*100000000;
    bcd2uint8(data[4], &m);
    freq += m*10000000;
    bcd2uint8(data[5], &m);
    freq += (m/10)*100000 + (m%10)*25000;
    radioState.rxFreq = freq*10;
    radioState.txFreq = freq*10;

    //设置状态
    if (m_workModFlag || m_freqFlag)
    {
        if (m_workModFlag)
        {
            m_workModFlag = false;
            data[2] = m_workMod;
        }
        if (m_freqFlag)
        {
            m_freqFlag = false;
            //获取100M位
            uint8_t tmp = m_freq/100000000;
            uint82bcd(tmp, (uint8_t*)(&data[3]));
            //获取10M,1M位
            tmp = (m_freq%100000000)/1000000;
            uint82bcd(tmp, (uint8_t*)(&data[4]));
            //获取100K,25K位
            tmp = (m_freq%1000000)/100000 + (m_freq%100000)/25000;
            uint82bcd(tmp, (uint8_t*)(&data[5]));
        }
        writeData(0x0101, data, len);
    }
}
