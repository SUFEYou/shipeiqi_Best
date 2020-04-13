#include "Radio220tcr.h"
#include <QDateTime>
#include <QDebug>

Radio220tcr::Radio220tcr()
{

}

Radio220tcr::~Radio220tcr()
{

}

void Radio220tcr::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM1");
#else
    dataCom = new QextSerialPort("/dev/ttymxc1");
#endif
    connect(dataCom, SIGNAL(readyRead()), this, SLOT(readCom()));
    dataCom->setBaudRate(BAUD38400);    //设置波特率
    dataCom->setDataBits(DATA_8);       //设置数据位
    dataCom->setParity(PAR_NONE);       //设置校验
    dataCom->setStopBits(STOP_1);       //设置停止位
    dataCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    dataCom->setTimeout(10);            //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "220TCR Data Serail Open Err!";
    } else {
        qDebug() << "220TCR Data Serail Open Success!";
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    //timer->start(2000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    memset(&radioState, 0, sizeof(VHF_ACK_STATE));
}

void Radio220tcr::readCom()
{
    dataArray.push_back(dataCom->readAll());
    recvDataSubpackage();
    recvDataParse();
}

void Radio220tcr::recvDataSubpackage()
{
    if (dataArray.length() < 3)//3 == 包头（1字节）+ 校验（2字节） + 包尾（1字节）
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
                    else if (contLen < 2)//包内数据长度小于校验字节长度2字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        m_recvDataList.push_back(dataArray.mid(contBegin, contLen));
                        //更新已解码成功的数据长度
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

void Radio220tcr::recvDataParse()
{
    //解析包内容
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        char nCRC = CRCVerify(tmp, tmpArray.length()-2);
        char nCRCGet = (tmp[tmpArray.length()-2] << 4) + tmp[tmpArray.length()-1];

        if (nCRC != nCRCGet)
        {
            qDebug() << "In Radio220tcr::recvDataParse, Decoder Recv Data CRC Err!";
        }
        else
        {
            char  state[MAXDATALENGTH];
            memset(state, 0, MAXDATALENGTH);
            int   stateLen = 0;
            rConverte(tmp, tmpArray.length()-2, state, stateLen);
            //解包到正确数据，判断该数据为数传还是控制信息，分别进行处理
            if (stateLen > 0)
                messageSeparate(state, stateLen);
        }
    }
}

void Radio220tcr::messageSeparate(const char* data, const int len)
{
    unsigned char CMDType = (unsigned char)(*(data+4));
    switch(CMDType)
    {
    case 0x12:		// 2.7.2 电台报告信道信息
        {
            ;
        }
        break;
    case 0x15:		// 2.7.3 电台报告信道组信息
        {
            ;
        }
        break;
    case 0x17:		// 2.5 电台报告定频当前工作状态信息
        {
            // 解析定频工作状态信息
        }
        break;
    case 0x21:		// 2.4.3 电台报告功率等级
        {

        }
        break;
    case 0x22:
        {

        }
        break;
    case 0x23:		// 2.4.4 电台报告业务类型
        {
            if (*(data+5) == 0x03)
            {
                unsigned char m_nCurWorkType = (unsigned char)(*(data+6));
                unsigned char m_nMyRadioType = 212;
                if (m_nMyRadioType == 212)
                {
                }
                else if (m_nMyRadioType == 221)
                {
                }
            }
        }
        break;
    case 0x61:		// 2.8.1 电台报告自动网参数
        {
            ;
        }
        break;
    case 0x65:		// 2.8.2 电台报告自动网址
        {
            ;
        }
        break;
    case 0x70:		// 2.8.3 电台报告自动信道表
        {
            ;
        }
        break;
    case 0x76:		// 上报自动建链情况
        {

        }
        break;
    case 0x83:		// 2.3 报告数据通信状态
        {
            switch(*(data+5))
            {
            case 0x03:
                {
                    qDebug() << "电台正在接收数据";
                }
                break;
            case 0x04:
                {
                    qDebug() << "电台接收数据完成";
                }
                break;
            case 0x05:
                {
                    qDebug() << "电台开始发送数据";
                }
                break;
            case 0x06:
                {
                    qDebug() << "电台发送数据结束";
                }

                break;
            case 0x0B:
                {

                }
                break;
            default:
                break;
            }
            // 报告MODEM状态
        }
        break;
    case 0x84:		// 2.3 接收非保密数据
        {
            qDebug() << "接收非保密数据";
        }
        break;
    case 0x87:		// 2.3 电台发送流控状态
        {
            switch(*(data+5))
            {
            case 0x00:

                break;
            case 0x01:

                break;
            default:
                break;
            }
        }
        break;
    case 0x97:		// 2.3 接收保密数据
        {

        }
        break;
    case 0xA2:		// 2.4 电台报告工作模式
        {
            if (*(data+5) == 0x03)
            {
                if (*(data+6) == 0x01)			// 定频模式
                {
                    //m_nWorkMode	= RADIOMODE_RCU;
                    switch((unsigned char)(*(data+7)))
                    {
                    case 0x10:		// 定频操作模式
                        {
                            //m_nRadioState	= RADIORCU_STATE;
                        }
                        break;
                    case 0xA1:		//定频正在调谐
                        {
                            //m_nRadioState	= RADIORCU_TURNNING;
                        }
                        break;
                    case 0xA2:		// 定频调谐成功
                        {
                            //m_nRadioState	= RADIORCU_TURNSUCCESS;
                        }
                        break;
                    case 0xA3:		// 定频调谐失败
                        {
                            //m_nRadioState	= RADIORCU_TURNFAILURE;
                        }
                        break;
                    case 0x35:		// 定频发送低速报
                        {
                            //m_nRadioState	= RADIORCU_LSMSG_SEND;
                        }
                        break;
                    case 0x3D:		// 定频低速报发送结束
                        {
                            //m_nRadioState	= RADIORCU_LSMSG_SENDOVER;
                        }
                        break;
                    case 0x40:		// 定频接收低速报
                        {
                            //m_nRadioState	= RADIORCU_LSMSG_RECEIVE;
                        }
                        break;
                    case 0x45:		// 定频正确收到低速报
                        {
                            //m_nRadioState	= RADIORCU_LSMSG_RECEIVEOVER;
                        }
                        break;
                    case 0xA0:		// 定频已恢复出厂设置
                        {
                            //m_nRadioState	= RADIORCU_DEFAULT;
                        }
                        break;
                    default:
                        break;
                    }
                }
                else if ((unsigned char)(*(data+6)) == 0x04)	// 自动模式
                {
                    //m_nWorkMode	= RADIOMODE_AUTO;
                    switch((unsigned char)(*(data+7)))
                    {
                    case 0x10:		// 自动扫描状态
                        {
                            //m_nRadioState	= RADIOAUTO_SCANNING;
                        }
                        break;
                    case 0x13:		// 自动收到扫描呼叫
                        {
                            //m_nRadioState	= RADIOAUTO_RECVCALL;
                        }
                        break;
                    case 0x14:		// 自动发送扫描呼叫
                        {
                            //m_nRadioState	= RADIOAUTO_CALLING;
                        }
                        break;
                    case 0x27:		// 自动退出链路
                        {
                            //m_nRadioState	= RADIOAUTO_TERLINK;
                        }
                        break;
                    case 0x2B:		// 自动业务 建立主叫
                        {
                            //m_nRadioState	= RADIOAUTO_LINK_S;
                        }
                        break;
                    case 0x2C:		// 自动业务 建立被叫
                        {
                            //m_nRadioState	= RADIOAUTO_LINK_C;
                        }
                        break;
                    case 0xA0:		// 自动已恢复出厂设置
                        {
                            //m_nRadioState	= RADIOAUTO_DEFAULT;
                        }
                        break;
                    case 0xA1:		// 自动正在调谐状态
                        {
                            //m_nRadioState	= RADIOAUTO_TURNNING;
                        }
                        break;
                    case 0xA2:		// 自动调谐成功
                        {
                            //m_nRadioState	= RADIOAUTO_TURNSUCCESS;
                        }
                        break;
                    case 0xA3:		// 自动调谐失败
                        {
                            //m_nRadioState	= RADIOAUTO_TURNFAILURE;
                        }
                        break;
                    case 0x16:		// 自动等待探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_LISTEN;
                        }
                        break;
                    case 0x17:		// 自动响应探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_RECALL;
                        }
                        break;
                    case 0x18:		// 自动发送探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_SEND;
                        }
                        break;
                    case 0x19:		// 自动收到探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_RECEIVE;
                        }
                        break;
                    case 0x20:		// 自动应答探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_RECALL;
                        }
                        break;
                    case 0x21:		// 自动等待探测
                        {
                            //m_nRadioState	= RADIOAUTO_DET_LISTEN;
                        }
                        break;
                    case 0x22:		// 自动ALE建链
                        {
                            //m_nRadioState	= RADIOAUTO_LINK_S;
                        }
                        break;
                    case 0x25:		// 自动等待应答
                        {
                            //m_nRadioState	= RADIOAUTO_LINK_S;
                        }
                        break;
                    case 0x28:		// 自动已发送确认
                        {
                            //m_nRadioState	= RADIOAUTO_ACKSEND;			// 确认发送

                        }
                        break;
                    case 0x29:		// 自动已收到确认
                        {
                            //m_nRadioState	= RADIOAUTO_ACKRECEIVE;		// 收到确认
                        }
                        break;
                    case 0x2A:		// 自动业务握手
                        {
                            //m_nRadioState	= RADIOAUTO_SHACKE;			// 业务握手
                        }
                        break;
                    case 0x2D:		// 自动ARQ发送开始
                        {
                            //m_nRadioState	= RADIOAUTO_ARQ_SENDBEGIN;	// ARQ开始发送
                        }
                        break;
                    case 0x2E:		// 自动ARQ开始接收
                        {
                            //m_nRadioState	= RADIOAUTO_ARQ_RECEIVEBEGIN;	// ARQ开始接收
                        }
                        break;
                    case 0x2F:		// 自动ARQ接收完成
                        {
                            //m_nRadioState	= RADIOAUTO_ARQ_RECEIVEEND;	// ARQ接收结束
                        }
                        break;
                    case 0x30:		// 自动ARQ发送结束
                        {
                            //m_nRadioState	= RADIOAUTO_ARQ_SENDEND;		// ARQ开始结束
                        }
                        break;
                    case 0x33:		// 自动发送定频报 ？
                        {
                        }
                        break;
                    case 0x34:		// 自动发送猝发报
                        {
                        }
                        break;
                    case 0x50:		// 自动ARQ发送拆链
                        {
                        }
                        break;
                    case 0x51:		// 自动ARQ发送数据
                        {
                        }
                        break;
                    case 0x52:		// 自动ARQ等待应答
                        {
                            //m_nRadioState	= RADIOAUTO_ARQ_WAITRECALL;	// ARQ等待应答
                        }
                        break;
                    case 0x55:		// 自动ARQ等待数据
                        {
                        }
                        break;
                    case 0x36:		// 自动等待收方应答
                        {
                        }
                        break;
                    case 0x37:		// 自动定频报发送成功
                        {
                        }
                        break;
                    case 0x38:		// 自动猝发报发送成功
                        {
                        }
                        break;
                    case 0x39:		// 自动定频报发送失败
                        {
                        }
                        break;
                    case 0x3A:		// 自动猝发报发送失败
                        {
                        }
                        break;
                    case 0x3B:		// 自动定频报发送结束
                        {
                        }
                        break;
                    case 0x3C:		// 自动猝发报发送结束
                        {
                        }
                        break;
                    case 0x3D:		// 自动低速报发送结束
                        {
                        }
                        break;
                    case 0x3E:		// 自动接收定频报
                        {
                        }
                        break;
                    case 0x3F:		// 自动接收猝发报
                        {
                        }
                        break;
                    case 0x40:		// 自动接收低速报
                        {
                        }
                        break;
                    case 0x41:		// 自动发送消息应答
                        {
                        }
                        break;
                    case 0x47:		// 自动正确接收到猝发报
                        {
                        }
                        break;
                    case 0x49:		// 自动正确接收到定频报
                        {
                        }
                        break;
                    default:
                        break;
                    }

                }
            }
        }
        break;

    case 0xA4:		// 2.7.1 电台报告日期时间
        {
            // 当前电台的时间信息
            if (*(data+5) == 0x03)
            {
                // Report Current Time Get
            }

        }
        break;
    case 0xA5:		// 2.8.2 电台数据速率
        {
            if(*(data+5) == 0x03)
            {
                //m_nMDMSpeed	= *(pChar+6);

                // Report Current Modem Speed
            }
        }
        break;

    case 0xAB:		// 2.1 握手应答
        {
            if (*(data+5) == 0xB0)
            {
                //m_nWorkMode	= RADIOMODE_CONNECT;
                //m_RadioStateMsgQueryCount = 0;
                qDebug() << "与短波电台握手成功";

            }
        }
        break;

    default:
        break;
    }
}

void Radio220tcr::rConverte(const char* srcData, const int srcLen, char* dstData, int& dstLen)
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

void Radio220tcr::updateRadioState(char* data, int len)
{

}

int Radio220tcr::writeCtrlData(uint16_t ctrlTyp, char* data, int len)
{

}

int Radio220tcr::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);
    sendDataPackage(0x41, 0x84, data, len);

    return 0;
}

void Radio220tcr::sendDataPackage(char nDimID, char type, const char* data, const int len)
{
    char tmp[MAXDATALENGTH];
    memset(tmp, 0, MAXDATALENGTH);
    int tmpLen = 0;
    //类型ID
    tmp[0] = 0xA0;
    //信息长度
    tmp[1] = len+7;
    //目的设备ID
    tmp[2] = nDimID;
    //源设备ID
    tmp[3] = 0x42;
    //信令ID
    tmp[4] = type;
    tmpLen += 5;
    //数据
    memcpy(tmp+tmpLen, data, len);
    tmpLen += len;
    char dstData[MAXDATALENGTH];
    memset(dstData, 0, MAXDATALENGTH);
    int dstLen = 0;
    //添加转义
    wConverte(tmp, tmpLen, dstData+1, dstLen);
    //CRC校验
    uint16_t t_crc = CRCVerify(dstData+1, dstLen);
    dstData[dstLen] = t_crc >> 4;
    dstData[dstLen+1] = t_crc & 0x0F;
    dstLen += 2;
    dstData[0] = 0xC0;
    dstData[dstLen] = 0xC0;
}

void Radio220tcr::wConverte(char* srcData, int srcLen, char* dstData, int& dstLen)
{
    for (int i = 0; i < srcLen; ++i)
    {
        if ((unsigned char)srcData[i] == 0xC0)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDC;
            ++dstLen;
        }
        else if ((unsigned char)srcData[i] == 0xDB)
        {
            dstData[dstLen] = 0xDB;
            ++dstLen;
            dstData[dstLen] = 0xDD;
            ++dstLen;
        }
        else
        {
            dstData[dstLen] = 0xDD;
            ++dstLen;
        }
    }
}

char Radio220tcr::CRCVerify(const char* data, const quint16 len)
{
    if (len <= 0)
        return 0;
    char crc = data[0];
    for (int i = 1; i < len; ++i)
        crc ^= data[i];
    return crc;
}

void Radio220tcr::onTimer()
{

}
