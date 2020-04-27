#include "KeyUart.h"
#include "QDebug"
#include "UI/UIManager.h"
#include "UI/WidgeBase.h"
#include "Socket/SocketManage.h"
#include "Audio/AudioControl.h"
#include "Audio/AudioMixer.h"

KeyUart* KeyUart::m_instance = NULL;
QMutex KeyUart::m_getMutex;

KeyUart::KeyUart()
{
    pttOnTim = 0;
    toChangeCount = 0;
    toChangeTime = -1;
}

KeyUart::~KeyUart()
{

}

KeyUart* KeyUart::getInstance()
{

    QMutexLocker locker(&m_getMutex);
    if (m_instance == NULL)
    {
        m_instance = new KeyUart;
    }

    return m_instance;
}


void KeyUart::serialInit()
{

    dataCom = new QextSerialPort("/dev/ttymxc1");

    connect(dataCom, SIGNAL(readyRead()), this, SLOT(readDataCom()));
    dataCom->setBaudRate(BAUD115200);    //设置波特率
    dataCom->setDataBits(DATA_8);        //设置数据位
    dataCom->setParity(PAR_NONE);        //设置校验
    dataCom->setStopBits(STOP_1);        //设置停止位
    dataCom->setFlowControl(FLOW_OFF);   //设置数据流控制
    dataCom->setTimeout(10);             //设置延时
    //
    if (false == dataCom->open(QIODevice::ReadWrite))
    {
        qDebug() << "KeyBoard Serail Open Err!";
    } else {
        qDebug() << "KeyBoard Serail Open Success";
    }

    timer = new QTimer(this);
    timer->setInterval(400);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

}

void KeyUart::readDataCom()
{
    QByteArray data = dataCom->readAll();
    if(!data.isEmpty())
    {

//        qDebug() << "KeyBoard Recv size: " << data.size();
        //emit comRecData(temp);

        for(int i=0; i<data.size(); i++){
            char d = data.at(i);
//            qDebug() << "KeyBoard =: " << d;

            if(d == 0x50){
                pttOnTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
                onPttOn();

            } else if(d == 0x70){
//                qDebug() << "Set PTT Off ------------------------------!!!";
                pttOnTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
                onPttOff();

            } else if(d == 0x76){      // 按键【下】连续按下：复用为切换按键

                toChangeCount = toChangeCount + 1;
                if(toChangeTime == -1){
                    toChangeTime = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
                }

                if(toChangeCount >= 3){

                    int currTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
                    int diffTim = currTim - toChangeTime;

//                    qDebug() << "toChange cost time------------------------------" << diffTim;
                    if(diffTim <= 1){
                        UIManager::getInstance()->changeCurrWidge();
                    }
                    toChangeCount = 0;
                    toChangeTime  = -1;

                } else {

                    onKey(d);
                }


            } else {

                toChangeCount = 0;
                toChangeTime  = -1;

                onKey(d);
            }

        }

    }
}

void KeyUart::onKey(char key)
{

    if(key == 0x2B){
        WidgeBase::upVolumn();

    } else if(key == 0x2D){
        WidgeBase::dwVolumn();

    } else if(key == 0x7B){
        WidgeBase::upBkLight();

    } else if(key == 0x7D){
        WidgeBase::dwBkLight();

    } else {

        WidgeBase* widget = UIManager::getInstance()->getCurrWidge();
        if(widget != NULL){
            widget->onKey(key);
        }
    }
}


void KeyUart::onTimer()
{
    int currTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
    int diffTim = currTim - pttOnTim;
    if(diffTim >= 2){

        qDebug() << "Set PTT Off <timeout>------------------------------!!!";
        SocketManage::getInstance()->setPttOn(false);
    }

}

void KeyUart::onPttOn()
{
    SocketManage::getInstance()->setPttOn(true);
}


void KeyUart::onPttOff()
{
    SocketManage::getInstance()->setPttOn(false);
}



