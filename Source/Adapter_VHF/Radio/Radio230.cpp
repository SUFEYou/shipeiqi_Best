#include "Radio230.h"
#include <QDateTime>
#include <QDebug>
#include "log/log4z.h"

using namespace zsummer::log4z;

#define DATA_MAX_SIZE 4096

Radio230::Radio230()
            : updTim(0)
            , m_nModemSend(false)
            , m_nModemState(0)
            , m_SendingTimesCount(0)
            , m_DycArrayLen(0)
            , m_RequestCount(0)
{
    m_DycArrayData = new char[ARRAY_DATA_LEN];
    radioState.errState = 1;

    m_pReceiveBuf = new char[DATA_MAX_SIZE];
    m_dataSndLen = 0;
}

Radio230::~Radio230()
{
    if(dataCom != NULL)
    {
        delete dataCom;
        dataCom = NULL;
    }
    if (NULL != m_DycArrayData)
    {
        delete[] m_DycArrayData;
        m_DycArrayData = NULL;
    }
    if (NULL != m_pReceiveBuf)
    {
        delete[] m_pReceiveBuf;
        m_pReceiveBuf = NULL;
    }
}

void Radio230::serialInit()
{
#if WIN32
    dataCom = new QextSerialPort("COM12");
#else
    dataCom = new QextSerialPort("/dev/ttymxc2");
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
        LOGE("230 Data Serail Open Err!");
    } else {
        LOGI("230 Data Serail Open Success!");
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000);

    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    memset(&radioState, 0, sizeof(RADIO_STATE));
}

void Radio230::readCom()
{
    dataArray.push_back(dataCom->readAll());
    packageData();
    parseData();
}

void Radio230::packageData()
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

void Radio230::parseData()
{
    //解析包内容
    while (!m_recvDataList.isEmpty())
    {
        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        char nCRC = getCRC(tmp+5, tmpArray.length()-7);
        char nCRCGet = (tmp[tmpArray.length()-2] << 4) + tmp[tmpArray.length()-1];

        if (nCRC != nCRCGet)
        {
            qDebug() << "In Radio230::recvDataParse, Decoder Recv Data CRC Err!";
        }
        else
        {
            char  state[MAXDATALENGTH];
            memset(state, 0, MAXDATALENGTH);
            int   stateLen = 0;
            decode(tmp, tmpArray.length()-2, state, stateLen);
            //解包到正确数据，判断该数据为数传还是控制信息，分别进行处理
            updateRadioState(state, stateLen);
        }
    }
}

void Radio230::updateRadioState(const char* data, const int len)
{
    radioState.errState = 0;
    updTim = QDateTime::currentDateTimeUtc().toTime_t();                     //秒级

    unsigned char CMDType = (unsigned char)(*(data+4));
    switch(CMDType)
    {
    case 0x12:		// 2.7.2 电台报告信道信息
    {
        if (*(data+5) == 0x03)
        {
            // 解析定频工作状态信息
            radioState.channel = (*(data+6)<<8) + (*(data+7));

            uint32_t tmpFreq = 0;
            uint8_t tmp = 0;
            bcd2uint8(*(data+8), &tmp);
            tmpFreq += tmp*1000000;
            bcd2uint8(*(data+9), &tmp);
            tmpFreq += tmp*10000;
            bcd2uint8(*(data+10), &tmp);
            tmpFreq += tmp*100;
            bcd2uint8(*(data+11), &tmp);
            tmpFreq += tmp;
            radioState.rxFreq = tmpFreq*10;

            tmpFreq = 0;
            bcd2uint8(*(data+12), &tmp);
            tmpFreq += tmp*1000000;
            bcd2uint8(*(data+13), &tmp);
            tmpFreq += tmp*10000;
            bcd2uint8(*(data+14), &tmp);
            tmpFreq += tmp*100;
            bcd2uint8(*(data+15), &tmp);
            tmpFreq += tmp;
            radioState.txFreq = tmpFreq*10;

            radioState.workTyp = *(data+16);
            radioState.power = *(data+17);
            //radioState.workMod = *(data+18);
            //协议中标注，最后一个字节上送的为静噪等级
            //radioState.squelch = *(data+18);

        }
    }
        break;
    case 0x15:		// 2.7.3 电台报告信道组信息
    {
    }
        break;
    case 0x17:		// 2.5 电台报告定频当前工作状态信息
    {
        // 解析定频工作状态信息
        radioState.channel = (*(data+6)<<8) + (*(data+7));

        uint32_t tmpFreq = 0;
        uint8_t tmp = 0;
        bcd2uint8(*(data+8), &tmp);
        tmpFreq += tmp*1000000;
        bcd2uint8(*(data+9), &tmp);
        tmpFreq += tmp*10000;
        bcd2uint8(*(data+10), &tmp);
        tmpFreq += tmp*100;
        bcd2uint8(*(data+11), &tmp);
        tmpFreq += tmp;
        radioState.rxFreq = tmpFreq*10;

        tmpFreq = 0;
        bcd2uint8(*(data+12), &tmp);
        tmpFreq += tmp*1000000;
        bcd2uint8(*(data+13), &tmp);
        tmpFreq += tmp*10000;
        bcd2uint8(*(data+14), &tmp);
        tmpFreq += tmp*100;
        bcd2uint8(*(data+15), &tmp);
        tmpFreq += tmp;
        radioState.txFreq = tmpFreq*10;

        radioState.workTyp = *(data+16);
        radioState.power = *(data+17);
        radioState.squelch = *(data+18);
        //radioState.workMod = *(data+19);
//        qDebug() << "Channel: " << radioState.channel << ", rxFreq: " << radioState.rxFreq << ", txFreq: " << radioState.txFreq << ", workTyp: " << radioState.workTyp \
//                 << ", power: " << radioState.power << ", squelch: " << radioState.squelch << ", workMod: " << radioState.workMod;
    }
        break;
    case 0x21:		// 2.4.3 电台报告功率等级
    {
        if (*(data+5) == 0x03)
        {
            radioState.power = *(data+6);
        }
    }
        break;
    case 0x22://静噪等级
    {
        if (*(data+5) == 0x03)
        {
            radioState.squelch = *(data+6);
        }
    }
        break;
    case 0x23:		// 2.4.4 电台报告业务类型
    {
        if (*(data+5) == 0x03)
        {
            radioState.workMod = *(data+6);
        }
    }
        break;
    case 0x61:		// 2.8.1 电台报告自动网参数
    {
    }
        break;
    case 0x65:		// 2.8.2 电台报告自动网址
    {
    }
        break;
    case 0x70:		// 2.8.3 电台报告自动信道表
    {
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
            m_nModemState	= RADIOMODEMSTATE_RECEIVING;
            qDebug() << "Recving Data";
            memset(m_pReceiveBuf,0,DATA_MAX_SIZE);
            m_dataSndLen = 0;
        }
            break;
        case 0x04:
        {
            m_nModemState	= RADIOMODEMSTATE_RECEIVEEND;
            qDebug() << "Recv Data Done";
            if (m_dataSndLen <= 0)
            {
                break;
            }
            else
            {
                RadioManage::getInstance()->onRecvLinkData(m_pReceiveBuf, m_dataSndLen);
            }
        }
            break;
        case 0x05:
        {
            m_nModemState	= RADIOMODEMSTATE_SENDING;
            qDebug() << "Begin Send Data";
        }
            break;
        case 0x06:
        {
            m_nModemState	= RADIOMODEMSTATE_SENDEND;
            m_nModemSend = false;
            qDebug() << "Send Data Done";
        }
            break;
        case 0x0B:
        {
//            if((RADIOMODEMSTATE_SENDING == m_nModemState ||
//                RADIOMODEMSTATE_BUFEMPTY == m_nModemState) &&
//                true == m_nModemSend)
//            {
//                m_SendingTimesCount++;
//                if (m_SendingTimesCount > 30)
//                {
//                    setModemNoticeSendEnd();
//                    qDebug() << "0x0B m_SendingTimesCount > 30  SetModemNoticeSendEnd!";

//                }
//                else if (m_SendingTimesCount > 40)
//                {
//                    setModemNoticeCancel();
//                    m_SendingTimesCount = 0;
//                    m_nModemSend = false;
//                    m_nModemState = 0;
//                    qDebug() << "0x0B m_SendingTimesCount > 40  SetModemNoticeCancel!";
//                }
//            }
            //m_RadioStateMsgQueryCount = 0;
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
        qDebug() << "Recv Unclassified Data";
        //RadioManage::getInstance()->onRecvLinkData(data+5, len-5);
        memcpy(m_pReceiveBuf+m_dataSndLen, data+5, len-5);
        m_dataSndLen += len-5;
    }
        break;
    case 0x87:		// 2.3 电台发送流控状态
    {
        switch(*(data+5))
        {
        case 0x00:
            m_nModemState	= RADIOMODEMSTATE_BUFFULL;
            radioModemCanSendData(false);
            break;
        case 0x01:
            m_nModemState	= RADIOMODEMSTATE_BUFEMPTY;
            radioModemCanSendData(true);
            m_RequestCount = 0;
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
                switch((unsigned char)(*(data+7)))
                {
                case 0x10:		// 定频操作模式
                {
                    m_nRadioState	= RADIORCU_STATE;
                }
                    break;
                case 0xA1:		//定频正在调谐
                {
                    m_nRadioState	= RADIORCU_TURNNING;
                }
                    break;
                case 0xA2:		// 定频调谐成功
                {
                    m_nRadioState	= RADIORCU_TURNSUCCESS;
                }
                    break;
                case 0xA3:		// 定频调谐失败
                {
                    m_nRadioState	= RADIORCU_TURNFAILURE;
                }
                    break;
                case 0x35:		// 定频发送低速报
                {
                    m_nRadioState	= RADIORCU_LSMSG_SEND;
                }
                    break;
                case 0x3D:		// 定频低速报发送结束
                {
                    m_nRadioState	= RADIORCU_LSMSG_SENDOVER;
                }
                    break;
                case 0x40:		// 定频接收低速报
                {
                    m_nRadioState	= RADIORCU_LSMSG_RECEIVE;
                }
                    break;
                case 0x45:		// 定频正确收到低速报
                {
                    m_nRadioState	= RADIORCU_LSMSG_RECEIVEOVER;
                }
                    break;
                case 0xA0:		// 定频已恢复出厂设置
                {
                    m_nRadioState	= RADIORCU_DEFAULT;
                }
                    break;
                default:
                    break;
                }
            }
            else if ((unsigned char)(*(data+6)) == 0x04)	// 自动模式
            {
                switch((unsigned char)(*(data+7)))
                {
                case 0x10:		// 自动扫描状态
                {
                    m_nRadioState	= RADIOAUTO_SCANNING;
                }
                    break;
                case 0x13:		// 自动收到扫描呼叫
                {
                    m_nRadioState	= RADIOAUTO_RECVCALL;
                }
                    break;
                case 0x14:		// 自动发送扫描呼叫
                {
                    m_nRadioState	= RADIOAUTO_CALLING;
                }
                    break;
                case 0x27:		// 自动退出链路
                {
                    m_nRadioState	= RADIOAUTO_TERLINK;
                }
                    break;
                case 0x2B:		// 自动业务 建立主叫
                {
                    m_nRadioState	= RADIOAUTO_LINK_S;
                }
                    break;
                case 0x2C:		// 自动业务 建立被叫
                {
                    m_nRadioState	= RADIOAUTO_LINK_C;
                }
                    break;
                case 0xA0:		// 自动已恢复出厂设置
                {
                    m_nRadioState	= RADIOAUTO_DEFAULT;
                }
                    break;
                case 0xA1:		// 自动正在调谐状态
                {
                    m_nRadioState	= RADIOAUTO_TURNNING;
                }
                    break;
                case 0xA2:		// 自动调谐成功
                {
                    m_nRadioState	= RADIOAUTO_TURNSUCCESS;
                }
                    break;
                case 0xA3:		// 自动调谐失败
                {
                    m_nRadioState	= RADIOAUTO_TURNFAILURE;
                }
                    break;
                case 0x16:		// 自动等待探测
                {
                    m_nRadioState	= RADIOAUTO_DET_LISTEN;
                }
                    break;
                case 0x17:		// 自动响应探测
                {
                    m_nRadioState	= RADIOAUTO_DET_RECALL;
                }
                    break;
                case 0x18:		// 自动发送探测
                {
                    m_nRadioState	= RADIOAUTO_DET_SEND;
                }
                    break;
                case 0x19:		// 自动收到探测
                {
                    m_nRadioState	= RADIOAUTO_DET_RECEIVE;
                }
                    break;
                case 0x20:		// 自动应答探测
                {
                    m_nRadioState	= RADIOAUTO_DET_RECALL;
                }
                    break;
                case 0x21:		// 自动等待探测
                {
                    m_nRadioState	= RADIOAUTO_DET_LISTEN;
                }
                    break;
                case 0x22:		// 自动ALE建链
                {
                    m_nRadioState	= RADIOAUTO_LINK_S;
                }
                    break;
                case 0x25:		// 自动等待应答
                {
                    m_nRadioState	= RADIOAUTO_LINK_S;
                }
                    break;
                case 0x28:		// 自动已发送确认
                {
                    m_nRadioState	= RADIOAUTO_ACKSEND;			// 确认发送

                }
                    break;
                case 0x29:		// 自动已收到确认
                {
                    m_nRadioState	= RADIOAUTO_ACKRECEIVE;		// 收到确认
                }
                    break;
                case 0x2A:		// 自动业务握手
                {
                    m_nRadioState	= RADIOAUTO_SHACKE;			// 业务握手
                }
                    break;
                case 0x2D:		// 自动ARQ发送开始
                {
                    m_nRadioState	= RADIOAUTO_ARQ_SENDBEGIN;	// ARQ开始发送
                }
                    break;
                case 0x2E:		// 自动ARQ开始接收
                {
                    m_nRadioState	= RADIOAUTO_ARQ_RECEIVEBEGIN;	// ARQ开始接收
                }
                    break;
                case 0x2F:		// 自动ARQ接收完成
                {
                    m_nRadioState	= RADIOAUTO_ARQ_RECEIVEEND;	// ARQ接收结束
                }
                    break;
                case 0x30:		// 自动ARQ发送结束
                {
                    m_nRadioState	= RADIOAUTO_ARQ_SENDEND;		// ARQ开始结束
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
                    m_nRadioState	= RADIOAUTO_ARQ_WAITRECALL;	// ARQ等待应答
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
        if ((unsigned char)*(data+5) == 0xB0)
        {
            qDebug() << "Shake Hands Success";
        }
    }
        break;
        default:
            break;
    }
}

void Radio230::setModemNoticeSendEnd()
{
    // 请求MODEM结束发送数据
    char pPara[1];
    pPara[0] = 0x02;
    writeData(0x40, 0x83, pPara, 1);
}

void Radio230::setModemNoticeCancel()
{
    // 请求MODEM取消发送数据
    char pPara[1];
    pPara[0]	= 0x07;
    writeData(0x40, 0x83, pPara, 1);
}

void Radio230::radioModemCanSendData(bool isSend)
{
    if (true == isSend)
    {
        if (m_DycArrayLen < ARRAY_DATA_LEN)
        {
            int len = 0;
            int copyLen = 0;
            len = (m_DycArrayLen / HFSENDDATA_MAXLIMITLEN) + 1;

            char tempData[HFSENDDATA_LIMITLEN];

            for (int i = 0 ; i < len; i++)
            {
                if (m_nModemState == RADIOMODEMSTATE_SENDEND)
                {
                    //TRACE(_T("超过组织机制分配的时间片\n\r"));
                    m_nModemState = 0;
                    break;
                }
                if (((i+1)* HFSENDDATA_LIMITLEN) < m_DycArrayLen)
                {
                    copyLen = HFSENDDATA_LIMITLEN;
                }
                else
                {
                    copyLen = m_DycArrayLen - (i*HFSENDDATA_LIMITLEN);
                }

                memset(tempData, 0, HFSENDDATA_LIMITLEN);
                memcpy(tempData, m_DycArrayData+(HFSENDDATA_LIMITLEN*i), copyLen);
                writeData(0x41, 0x84, tempData,copyLen);
                LOGD(QString("On radioModemCanSendData copyLen %1").arg(copyLen).toStdString().c_str());
            }

            m_nModemState = RADIOMODEMSTATE_SENDING;

            setModemNoticeSendEnd();
        }
        else
        {
            setModemNoticeSendEnd();
            m_nModemState = 0;
            LOGD(QString("else m_DycArrayLen < ARRAY_DATA_LEN").toStdString().c_str());
        }
    }
}

void Radio230::decode(const char* srcData, const int srcLen, char* dstData, int& dstLen)
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

int Radio230::writeCtrlData(uint16_t funCode, char* data, int len)
{
    int ctrlDataLen = sizeof(RADIO_SET);
    if(len == ctrlDataLen)
    {
        RADIO_SET m_set;
        memcpy(&m_set, data, ctrlDataLen);
        switch (funCode)
        {
        case Set_WorkTyp://设置工作方式
        {
            setWorkTyp(m_set.workTyp);
        }
            break;
        case Set_WorkMod://设置工作模式
        {
            setWorkMod(m_set.workMod);
        }
            break;
        case Set_Channel://设置信道
        {
            setChannel(m_set.channel);
        }
        break;
        case Set_TxFreq://设置发送频率
        {
            setTxFreq(m_set.txFreq);
        }
            break;
        case Set_RxFreq://设置接收频率
        {
            setRxFreq(m_set.rxFreq);
        }
            break;
        case Set_Power://设置发射功率
        {
            setPower(m_set.power);
        }
            break;
        case Set_Squelch://设置静噪
        {
            setSquelch(m_set.squelch);
        }
            break;
        case Ask_State://状态问询
        {
            char ackData[sizeof(RADIO_STATE)];
            memcpy(ackData, &radioState, sizeof(RADIO_STATE));
            RadioManage::getInstance()->onCtrlAck(Ack_State, ackData, sizeof(RADIO_STATE));
        }
            break;
        default:
            break;
        }
    }
    return 0;
}

int Radio230::writeLinkData(char* data, int len)
{
    QMutexLocker locker(&m_dataMutex);
    //请求发送数据
    char tmp[1];
    tmp[0] = 0x01;
    writeData(0x41, 0x83, tmp, 1);

    if (len <= ARRAY_DATA_LEN)
    {
        memset(m_DycArrayData, 0, ARRAY_DATA_LEN);
        memcpy(m_DycArrayData, data, len);
        m_DycArrayLen = len;
        m_nModemSend = true;
        m_RequestCount++;
        if (m_RequestCount > 4)
        {
            m_RequestCount = 4;
        }
    }
    return 0;
}

void Radio230::writeData(char nDimID, char type, const char* data, const int len)
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
    enCode(tmp, tmpLen, dstData+1, dstLen);
    //添加包头0xC0长度1
    dstLen += 1;
    //CRC校验,只校验参数
    uint16_t t_crc = 0;
    if (len > 0)
        t_crc = getCRC(data, len);

    dstData[dstLen] = t_crc >> 4;
    dstData[dstLen+1] = t_crc & 0x0F;
    dstLen += 2;
    dstData[0] = 0xC0;
    dstData[dstLen] = 0xC0;
    dstLen += 1;
    dataCom->write(dstData, dstLen);
}

void Radio230::enCode(const char* srcData, int srcLen, char* dstData, int& dstLen)
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
            dstData[dstLen] = srcData[i];
            ++dstLen;
        }
    }
}

char Radio230::getCRC(const char* data, const quint16 len)
{
    if (len <= 0)
        return 0;
    char crc = data[0];
    for (int i = 1; i < len; ++i)
        crc ^= data[i];
    return crc;
}

void Radio230::onTimer()
{
    static uint16_t ShakeHeadCount = 0;
    static uint16_t ChangeToDataModeCount = 0;
    static uint8_t QueryStateCount = 0;

    checkDisconnect();

    if (ShakeHeadCount > 120)
    {
        if (m_nModemState != RADIOMODEMSTATE_SENDING)
        {
            ShakeHeadCount = 0;
            //握手
            writeData(0x40, 0xB0, NULL, 0);
        }
    }

    if (ChangeToDataModeCount > 300 && \
        m_nModemState != RADIOMODEMSTATE_SENDING && \
        m_nModemState != RADIOMODEMSTATE_RECEIVING )
    {
        ChangeToDataModeCount = 0;
        char tmp[2];
        tmp[0] = 0x02;
        tmp[1] = DITAL_DATA;
        //更改业务类型为数传
        writeData(0x40, 0x23, tmp, 2);
        //查询业务类型
        tmp[0] = 0x01;
        writeData(0x40, 0x23, tmp, 1);
    }

    if (QueryStateCount > 3)
    {
        QueryStateCount = 0;
        //返回0x17
        char tmp[1];
        tmp[0] = 1;
        writeData(0x40, 0xB1, tmp, 1);
        writeData(0x40, 0x23, tmp, 1);
    }

    if(m_RequestCount > 3)
    {
        char nData[1];
        nData[0] = 0x02;
        writeData(0x40, 0x84, nData, 1);
        setModemNoticeSendEnd();
    }

    ++ShakeHeadCount;
    ++ChangeToDataModeCount;
    ++QueryStateCount;

    char ackData[sizeof(RADIO_STATE)];
    memcpy(ackData, &radioState, sizeof(RADIO_STATE));
    RadioManage::getInstance()->onCtrlAck(Ack_State, ackData, sizeof(RADIO_STATE));
}

void Radio230::setWorkTyp(const uint8_t nWorkTyp)
{
    char pPara[2];
    pPara[0]	= 0x02;
    pPara[1]	= nWorkTyp;

    writeData(0x40, 0x20, pPara, 2);
    //radioState.workTyp = nWorkTyp;
}

void Radio230::setWorkMod(const uint8_t nWorkMod)
{
    char pPara[2];
    pPara[0]	= 0x02;
    pPara[1]	= nWorkMod;

    writeData(0x40, 0x23, pPara, 2);
    //radioState.workMod = nWorkMod;
//    char pPara[14];
//    pPara[0]	= 0x02;
//    pPara[1]	= radioState.channel/256;
//    pPara[2]	= radioState.channel%256;
//    //传递设置的数据为khz*10000,转化为hz需除以10
//    uint64_t tmp = radioState.rxFreq/10;
//    if (tmp < 1600000 || tmp > 30000000)
//        memset(&pPara[3], 0, 4);
//    else
//    {
//        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[3]));
//        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[4]));
//        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[5]));
//        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[6]));
//    }

//    tmp = radioState.txFreq/10;
//    if (tmp < 1600000 || tmp > 30000000)
//        memset(&pPara[7], 0, 4);
//    else
//    {
//        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[7]));
//        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[8]));
//        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[9]));
//        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[10]));
//    }
//    //工作方式
//    pPara[11] = radioState.workTyp;
//    //功率等级
//    pPara[12] = radioState.power;
//    //业务类型
//    pPara[13] = nWorkMod;

//    writeData(0x40, 0x12, pPara, 14);
    //radioState.workMod = nWorkMod;
}

void Radio230::setChannel(const uint16_t nCHN)
{
    char pPara[3];
    pPara[0]	= 0x02;
    pPara[1]	= nCHN/256;
    pPara[2]	= nCHN%256;

    writeData(0x40, 0x14, pPara, 3);
    //radioState.channel = nCHN;
}

void Radio230::setPower(const uint8_t nPower)
{
    char pPara[2];
    pPara[0]	= 0x02;
    pPara[1]	= nPower;

    writeData(0x40, 0x21, pPara, 2);
    //radioState.power = nPower;
}

void Radio230::setSquelch(const uint8_t nSquelch)
{
    char pPara[2];
    pPara[0]	= 0x02;
    pPara[1]	= nSquelch;

    writeData(0x40, 0x22, pPara, 2);
    //radioState.power = nSquelch;
}

void Radio230::setTxFreq(const uint64_t nTxFreq)
{
//    //传递设置的数据为khz*10000,转化为hz需除以10
//    uint64_t tmp = nTxFreq/10;

//    if (tmp < 1600000 || tmp > 30000000)
//        return;
//    char pPara[5];
//    pPara[0]	= 0x02;
//    uint82bcd(tmp/1000000, (uint8_t*)(&pPara[1]));
//    uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[2]));
//    uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[3]));
//    uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[4]));

//    writeData(0x40, 0x18, pPara, 5);
//    radioState.txFreq = nTxFreq;
    char pPara[14];
    pPara[0]	= 0x02;
    pPara[1]	= radioState.channel/256;
    pPara[2]	= radioState.channel%256;
    //传递设置的数据为khz*10000,转化为hz需除以10
    uint64_t tmp = radioState.rxFreq/10;
    if (tmp < 1600000 || tmp > 30000000)
        memset(&pPara[3], 0, 4);
    else
    {
        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[3]));
        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[4]));
        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[5]));
        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[6]));
    }

    tmp = nTxFreq/10;
    if (tmp < 1600000 || tmp > 30000000)
        memset(&pPara[7], 0, 4);
    else
    {
        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[7]));
        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[8]));
        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[9]));
        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[10]));
    }
    //工作方式
    pPara[11] = radioState.workTyp;
    //功率等级
    pPara[12] = radioState.power;
    //业务类型
    pPara[13] = radioState.workMod;

    writeData(0x40, 0x12, pPara, 14);
    //radioState.txFreq = nTxFreq;
}

void Radio230::setRxFreq(const uint64_t nRxFreq)
{
//    //传递设置的数据为khz*10000,转化为hz需除以10
//    uint64_t tmp = nRxFreq/10;

//    if (tmp < 1600000 || tmp > 30000000)
//        return;
//    char pPara[5];
//    pPara[0]	= 0x01;
//    uint82bcd(tmp/1000000, (uint8_t*)(&pPara[1]));
//    uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[2]));
//    uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[3]));
//    uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[4]));

//    writeData(0x40, 0x18, pPara, 5);
//    radioState.rxFreq = nRxFreq;
    char pPara[14];
    pPara[0]	= 0x02;
    pPara[1]	= radioState.channel/256;
    pPara[2]	= radioState.channel%256;
    //传递设置的数据为khz*10000,转化为hz需除以10
    uint64_t tmp = nRxFreq/10;
    if (tmp < 1600000 || tmp > 30000000)
        memset(&pPara[3], 0, 4);
    else
    {
        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[3]));
        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[4]));
        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[5]));
        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[6]));
    }

    tmp = radioState.txFreq/10;
    if (tmp < 1600000 || tmp > 30000000)
        memset(&pPara[7], 0, 4);
    else
    {
        uint82bcd(tmp/1000000, (uint8_t*)(&pPara[7]));
        uint82bcd(tmp/10000%100, (uint8_t*)(&pPara[8]));
        uint82bcd(tmp%10000/100, (uint8_t*)(&pPara[9]));
        uint82bcd(tmp%10000%100, (uint8_t*)(&pPara[10]));
    }
    //工作方式
    pPara[11] = radioState.workTyp;
    //功率等级
    pPara[12] = radioState.power;
    //业务类型
    pPara[13] = radioState.workMod;

    writeData(0x40, 0x12, pPara, 14);
    //radioState.rxFreq = nRxFreq;
}

void Radio230::checkDisconnect()
{
    long curTim = QDateTime::currentDateTimeUtc().toTime_t();         //秒级
    long difTim = curTim - updTim;

    if(difTim > 5)
    {
        updTim = curTim;
        radioState.errState = 1;       // 电台通信异常
    }
}
