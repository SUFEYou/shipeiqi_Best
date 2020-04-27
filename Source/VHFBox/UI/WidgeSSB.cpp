#include "WidgeSSB.h"
#include "ui_SsbCtrl.h"
#include <QDebug>

WidgeSSB::WidgeSSB()
{
    ui = new Ui::SsbCtrl;
    ui->setupUi(this);
    // 去标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);

    index = 0;
    workModel = -1;
    channel   = -1;
    revFreq   = -1;
    sndFreq   = -1;
    power     = -1;
    squelch   = -1;
    state     = -1;

    regAckTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
    registACK   = 0;
    pttAck      = 0;
    ctrlOutRestrict = 0;             // 遥控(0：未受限 1：受限)
    ctrlInRestrict  = 0;             // 状态(0：未受限 1：受限)
    voicOutRestrict = 0;             // 喊话(0：未受限 1：受限)
    voicInRestrict  = 0;             // 监听(0：未受限 1：受限)

    curTim = "";
    curLon = "";
    curLat = "";

    tCount = 0;                      // 注册计数器

    iconSelf  = new QMovie("images/voice_green.gif");
    iconOthr  = new QMovie("images/voice.gif");

    QSize iconSize(100,100);
    iconSelf->setScaledSize(iconSize);
    iconOthr->setScaledSize(iconSize);

    iconSelf->start();
    iconOthr->start();
}

void WidgeSSB::init()
{

    curChannel = "";
    tmpChannel = "0";

//    DEF_CNANNEL_STYLE = "color:black;font-size:100px;line-height:150px;";
//    SET_CNANNEL_STYLE = "color:#00FF00;font-size:100px;line-height:150px;";//font-family:Microsoft YaHei;

    ui->lblAddrVal->setText(radioTyp);
    ui->lblTime   ->setText("");
    ui->lblLatVal ->setText("");
    ui->lblLonVal ->setText("");

    ui->btnA->setFlat(true);
    ui->btnB->setFlat(true);
    ui->btnC->setFlat(true);
    ui->btnD->setFlat(true);

    this->setBkLight(lightLev);

//    // Update Ui Style
//    QString defaultFontStyle = "color:black;font-weight:520;";

//    ui->lblAddr->setStyleSheet(defaultFontStyle);
//    ui->lblAddrVal->setStyleSheet(defaultFontStyle);
//    ui->lblMode->setStyleSheet(defaultFontStyle);
//    ui->lblModeMsg->setStyleSheet("color:red;");

//    QString smallStyle = "color:black;font-weight:420;font-size:16px;";
//    ui->lblModeMsg->setStyleSheet("color:red;");
//    ui->lblLat->setStyleSheet(smallStyle);
//    ui->lblLon->setStyleSheet(smallStyle);
//    ui->lblTime->setStyleSheet(smallStyle);
//    ui->lblBD->setStyleSheet(smallStyle);
//    ui->lblNoise->setStyleSheet(smallStyle);
//    ui->lblPower->setStyleSheet(smallStyle);
//    ui->lblTx->setStyleSheet(smallStyle);
//    ui->lblRx->setStyleSheet(smallStyle);

//    ui->lblRadioName->setText(radioNm);

//    QString style = NULL;
//    style.append("QProgressBar{border-top: 1px solid black;border-left: 1px solid black;background-color:transparent;}");
//    style.append("QProgressBar::chunk {background:url(\"images/processBar.png\");}");
//    ui->barSound->setStyleSheet(style);
//    ui->barRx->setStyleSheet(style);
//    ui->barTx->setStyleSheet(style);

    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

}

void WidgeSSB::onTimer()
{

}


void WidgeSSB::setMute()
{

    ui->lblMute->setText(QString::fromUtf8("静音"));
    ui->barSound->setValue(0);
    ui->barSound->setVisible(false);
}

void WidgeSSB::setVolume(int volumnLev)
{
    int volBar  = this->getVolumnBar(volumnLev);

    ui->lblMute->setText(QString::fromUtf8("音量"));
    ui->barSound->setVisible(true);
    ui->barSound->setValue(volBar);
}

void WidgeSSB::setBkLight(int bkLightLev)
{
    // 面板背景
    this->setAutoFillBackground(true);
    QPalette p = this ->palette();
    QString btnStyle  = "color:#FF5809;";
    QString lineStyle = "border-top:1px solid #FF5809;";
    QString lblStyle  = "color:#FF5809;";

    if(bkLightLev == 0){

        p.setColor(QPalette::Window,Qt::black);
        this->setPalette(p);

    } else {

        p.setColor(QPalette::Window,Qt::white);
        this->setPalette(p);

        btnStyle  = "color:#000000;";
        lineStyle = "border-top:1px solid #000000;";
        lblStyle  = "color:#000000;";
    }

    ui->lblAddr->setStyleSheet(lblStyle);
    ui->lblAddrVal->setStyleSheet(lblStyle);

    ui->lblMode->setStyleSheet(lblStyle);
    ui->lblModeMsg->setStyleSheet(lblStyle);
    ui->lblNoise->setStyleSheet(lblStyle);
    ui->lblPower->setStyleSheet(lblStyle);
    ui->lblTx->setStyleSheet(lblStyle);
    ui->lblRx->setStyleSheet(lblStyle);

    ui->lblBD->setStyleSheet(lblStyle);
    ui->lblTime->setStyleSheet(lblStyle);
    ui->lblLat->setStyleSheet(lblStyle);
    ui->lblLon->setStyleSheet(lblStyle);

}


//////////////////////////////////////////////////////////////////////////////////

void WidgeSSB::onKey(int key)
{
    if(key == 0x41){
        onKeyA();
    }
    if(key == 0x42){
        onKeyB();
    }
    if(key == 0x43){
        onKeyC();
    }
    if(key == 0x44){
        onKeyD();
    }
    if(key == 0x30){
        onKey0();
    }
    if(key == 0x31){
        onKey1();
    }
    if(key == 0x32){
        onKey2();
    }
    if(key == 0x33){
        onKey3();
    }
    if(key == 0x34){
        onKey4();
    }
    if(key == 0x35){
        onKey5();
    }
    if(key == 0x36){
        onKey6();
    }
    if(key == 0x37){
        onKey7();
    }
    if(key == 0x38){
        onKey8();
    }
    if(key == 0x39){
        onKey9();
    }
    if(key == 0x40){
        onKeyConfirm();
    }
    if(key == 0x21){
        onKeyCancel();
    }

    if(key == 0x5E){
        onKeyUp();
    }
    if(key == 0x76){
        onKeyDw();
    }
    if(key == 0x3C){
        onKeyLeft();
    }
    if(key == 0x3E){
        onKeyRight();
    }

    if(key == 0x5B){
        onKeyChannelUp();
    }
    if(key == 0x5D){
        onKeyChannelDw();
    }

}


void WidgeSSB::onKeyA()
{
     qDebug()<<"A";
}

void WidgeSSB::onKeyB()
{
    qDebug()<<"B";
}

void WidgeSSB::onKeyC()
{

}

void WidgeSSB::onKeyD()
{

}

void WidgeSSB::onKeyChannelUp()
{

}

void WidgeSSB::onKeyChannelDw()
{

}


//////////////////////////////////////////////////////////////////////////////////

void WidgeSSB::onKeyCancel()
{

}

void WidgeSSB::onKeyConfirm()
{

}

void WidgeSSB::onKey0()
{

}

void WidgeSSB::onKey1()
{

}

void WidgeSSB::onKey2()
{

}

void WidgeSSB::onKey3()
{

}

void WidgeSSB::onKey4()
{

}

void WidgeSSB::onKey5()
{

}

void WidgeSSB::onKey6()
{

}

void WidgeSSB::onKey7()
{

}

void WidgeSSB::onKey8()
{

}

void WidgeSSB::onKey9()
{

}

void WidgeSSB::onKeyUp()
{

}

void WidgeSSB::onKeyDw()
{

}

void WidgeSSB::onKeyLeft()
{

}

void WidgeSSB::onKeyRight()
{

}
