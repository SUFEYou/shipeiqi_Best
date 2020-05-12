#include "AudioPtt.h"
#include "Audio/AudioControl.h"
#include "Audio/AudioPlayer.h"
#include "socket/SocketManage.h"
#include <QDebug>

AudioPtt::AudioPtt()
{
    PttSet pttSet0;
    pttSet0.Priority = -1;
    pttSet0.PttOn    = 0;
    pttSetArr[0] = pttSet0;

    PttSet pttSet1;
    pttSet1.Priority = -1;
    pttSet1.PttOn    = 0;
    pttSetArr[1] = pttSet1;

    PttSet pttSet2;
    pttSet2.Priority = -1;
    pttSet2.PttOn    = 0;
    pttSetArr[2] = pttSet2;

    PttSet pttSet3;
    pttSet3.Priority = -1;
    pttSet3.PttOn    = 0;
    pttSetArr[3] = pttSet3;
}

bool AudioPtt::init()
{
    pttCom = new QextSerialPort("/dev/ttymxc4");

    connect(pttCom, SIGNAL(readyRead()), this, SLOT(readCom()));
    pttCom->setBaudRate(BAUD9600);    //设置波特率
    pttCom->setDataBits(DATA_8);       //设置数据位
    pttCom->setParity(PAR_NONE);       //设置校验
    pttCom->setStopBits(STOP_1);       //设置停止位
    pttCom->setFlowControl(FLOW_OFF);  //设置数据流控制
    pttCom->setTimeout(10);            //设置延时
    //
    if (false == pttCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "PTT Serail Open Err!";
    } else {
        qDebug() << "PTT Serail Open Success!";
    }

    pttOnTim = -1;
    pttOnLim = 180;     //PTTOn 最大时间3分钟
    return true;
}

void AudioPtt::run()
{

    while (!m_stop)
    {
        bool pttOn = false;
        int  pttProri  = -1;
        int  currIndex = -1;
        for(int i=0; i<4; i++)
        {
            PttSet pttSet = pttSetArr[i];
            if(pttSet.PttOn)
            {
                pttOn = true;
                int prori = pttSet.Priority;
                if(prori > pttProri){
                    pttProri = prori;
                    currIndex   = i;
                }
            }
        }

        if(!pttOn)
        {
            //Serial Ctrl-PttOff
//            qDebug()<<"Set PTT OFF --------------------------!!!";
            sendPTTOff();

            pttOnTim = -1;
            AudioControl::getInstance()->getPlayer1()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer2()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer3()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer4()->setPlayEn(false);
        }

        if(pttOn)
        {
            bool timoutStop = false;
            if(pttOnTim < 0){
                pttOnTim = QDateTime::currentDateTimeUtc().toTime_t();
            } else {
                int currTime = QDateTime::currentDateTimeUtc().toTime_t();
                int diffTime = currTime - pttOnTim;
                if(diffTime > pttOnLim){
                    pttOnTim = -1;
                    timoutStop = true;
                }
            }

            if(!timoutStop){
                //serial Ctrl-PttOn
                sendPTTOn(currIndex + 1);

                for(int i=0; i<4; i++)
                {
                    if(i == currIndex){
                        AudioControl::getInstance()->getPlayer(i + 1)->setPlayEn(true);

                    } else {
                        AudioControl::getInstance()->getPlayer(i + 1)->setPlayEn(false);

                    }
                }


            } else {

                //Serial Ctrl-PttOff
                qDebug()<<"Set PTT OFF <TimeOut>--------------------------!!!";
                sendPTTOff();

                for(int i=0; i<4; i++)
                {
                    pttSetArr[i].PttOn = 0;
                }

            }

        }

        msleep(10);

    }
    m_stop = false;
}

void AudioPtt::sendPTTOn(int playID)
{
//    qDebug()<<"Set PTT ON--------------------------!!!";
    if(!pttONSended){

        char pttOnData[3];
        pttOnData[0] = 0xF0;
        pttOnData[1] = 0x31;
        pttOnData[2] = 0x0A;

        pttCom->write(pttOnData, 3);
        pttONSended = 1;
        pttSendTim  = QDateTime::currentDateTimeUtc().toTime_t();

        SocketManage::getInstance()->getVoicUdp()->sendPTTState(playID, 0x01);
    } else {

        int currTim = QDateTime::currentDateTimeUtc().toTime_t();
        int diffTim = currTim - pttSendTim;

        if(diffTim > 2){

            char pttOnData[3];
            pttOnData[0] = 0xF0;
            pttOnData[1] = 0x31;
            pttOnData[2] = 0x0A;

            pttCom->write(pttOnData, 3);
            pttONSended = 1;
            pttSendTim  = QDateTime::currentDateTimeUtc().toTime_t();

            SocketManage::getInstance()->getVoicUdp()->sendPTTState(playID, 0x01);
        }
    }

}

void AudioPtt::sendPTTOff()
{
//    qDebug()<<"Set PTT OFF--------------------------!!!";
    if(pttONSended){

        char pttOffData[3];
        pttOffData[0] = 0xF0;
        pttOffData[1] = 0x30;
        pttOffData[2] = 0x0A;

        pttCom->write(pttOffData, 3);
        pttONSended = 0;
        pttSendTim  = QDateTime::currentDateTimeUtc().toTime_t();

        SocketManage::getInstance()->getVoicUdp()->sendPTTState(0, 0x00);
    } else {

        int currTim = QDateTime::currentDateTimeUtc().toTime_t();
        int diffTim = currTim - pttSendTim;

        if(diffTim > 2){

            char pttOffData[3];
            pttOffData[0] = 0xF0;
            pttOffData[1] = 0x30;
            pttOffData[2] = 0x0A;

            pttCom->write(pttOffData, 3);
            pttONSended = 0;
            pttSendTim  = QDateTime::currentDateTimeUtc().toTime_t();

            SocketManage::getInstance()->getVoicUdp()->sendPTTState(0, 0x00);
        }
    }
}

void AudioPtt::readCom()
{

}


void AudioPtt::setPriority_PttOn(int pID, int priority, int pttOn)
{
    PttSet pttSet;
    pttSet.Priority = priority;
    pttSet.PttOn    = pttOn;

    pttSetArr[pID-1] = pttSet;

}


int AudioPtt::getPttOn(int pID)
{
    if(pID >=0 && pID < 4){
        return pttSetArr[pID-1].PttOn;
    }
    return -1;
}


int AudioPtt::getPriority(int pID)
{
    if(pID >=0 && pID < 4){
        return pttSetArr[pID-1].Priority;
    }
    return -1;
}
