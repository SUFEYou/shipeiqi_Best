#include "WidgeVHF171d.h"
#include "ui_VhfCtrl.h"
#include <QDebug>
#include "Audio/AudioControl.h"
#include "Audio/AudioMixer.h"
#include "Socket/SocketManage.h"
#include "Socket/UdpRctrl.h"
#include "Util/RadioUtil.h"
#include "UI/UIManager.h"


WidgeVHF171d::WidgeVHF171d()
{
    ui = new Ui::VhfCtrl;
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

    iconSelf_Day  = new QMovie("images/mic-green-lt.gif");
    iconOthr_Day  = new QMovie("images/mic-orange-lt.gif");

    QSize iconSize(64,64);
    iconSelf_Day->setScaledSize(iconSize);
    iconOthr_Day->setScaledSize(iconSize);

    iconSelf_Day->start();
    iconOthr_Day->start();

    iconSelf_Dark  = new QMovie("images/mic-green-dk.gif");
    iconOthr_Dark  = new QMovie("images/mic-orange-dk.gif");


    iconSelf_Dark->setScaledSize(iconSize);
    iconOthr_Dark->setScaledSize(iconSize);

    iconSelf_Dark->start();
    iconOthr_Dark->start();

}

void WidgeVHF171d::init()
{

    curChannel = "";
    tmpChannel = "0";

    SET_CNANNEL_STYLE = "color:#00FF00;font-weight:bold;font-size:90px;line-height:150px;";      //font-family:Microsoft YaHei;

    ui->lblAddrVal->setText(radioTyp);
    ui->lblTime   ->setText("");
    ui->lblLatVal ->setText("");
    ui->lblLonVal ->setText("");

    ui->btnA->setFlat(true);
    ui->btnB->setFlat(true);
    ui->btnC->setFlat(true);
    ui->btnD->setFlat(true);

//    QString style = NULL;
//    style.append("QProgressBar{border-top: 1px solid black;border-left: 1px solid black;background-color:transparent;}");
//    style.append("QProgressBar::chunk {background:url(\"images/processBar.png\");}");
//    ui->barSound->setStyleSheet(style);

    this->setBkLight(lightLev);

    // 初始化 Timer
    setChnlTim = QTime::currentTime();

    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

//    qDebug()<<"Widget" << index << " " << radioTyp << "----------init";


}


void WidgeVHF171d::onTimer()
{
//    qDebug()<<"Widget"<< index << " " << radioTyp << "----------" <<pttAck;
    //////////////////////////////////////////////////////////////////////////////////
    resetIconMovie();
    if(pttAck == 1) {
        ui->voiceIcon->setMovie(iconSelf_Day);
        ui->voiceIcon->setVisible(true);

    } else if(pttAck == 2) {
        ui->voiceIcon->setMovie(iconOthr_Day);
        ui->voiceIcon->setVisible(true);

    } else {
        ui->voiceIcon->hide();
    }


    //////////////////////////////////////////////////////////////////////////////////

    //电台信道显示
    if(isSettingChnl){
        if(QTime::currentTime() > setChnlTim)
        {
              cancelTmpChannel();
        }
    } else {

        if(channel > 0){
            ui->lblChannel->setStyleSheet(DEF_CNANNEL_STYLE);
            QString chanlTxt = QString("%1").arg(channel, 2, 10, QLatin1Char('0'));
            ui->lblChannel->setText(chanlTxt);
            curChannel = chanlTxt;
            tmpChannel = "0";
        } else {
            ui->lblChannel->setText("");
        }
    }

    //////////////////////////////////////////////////////////////////////////////////

    //电台信道显示
    if(isSettingChnl){
        if(QTime::currentTime() > setChnlTim)
        {
              cancelTmpChannel();
        }
    } else {

        if(channel > 0){
            ui->lblChannel->setStyleSheet(DEF_CNANNEL_STYLE);
            QString chanlTxt = QString("%1").arg(channel, 2, 10, QLatin1Char('0'));
            ui->lblChannel->setText(chanlTxt);
            curChannel = chanlTxt;
            tmpChannel = "0";
        } else {
            ui->lblChannel->setText("");
        }
    }

    //////////////////////////////////////////////////////////////////////////////////

    //电台工作状态显示
    if(workModel >= 0) {
       //工作模式,取值3：背负DFF明； 4：背负FCS明； 5:背负FH明
        QString workModTxt;
       if(workModel == 3) {
           workModTxt = QString::fromUtf8("定频");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       } else if(workModel == 5) {
           workModTxt = QString::fromUtf8("跳频");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       }
       else if(workModel == 0) {
           workModTxt = QString::fromUtf8("模话");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");
       }
       else
       {
           workModTxt = QString::fromUtf8("未知");

           ui->lblModeMsg->setStyleSheet("color:red;");
       }

       ui->lblMode->setVisible(true);
       ui->lblModeMsg->setVisible(true);
       ui->errMsg->setVisible(false);

       ui->lblMode->setText(QString::fromUtf8("模式"));
       ui->lblModeMsg->setText(workModTxt);
    }

    //////////////////////////////////////////////////////////////////////////////////

    //断连状态显示
    if(state == -1){
        ui->lblMode->setVisible(true);
        ui->lblModeMsg->setVisible(true);
        ui->errMsg->setVisible(false);

        ui->lblMode->setText(QString::fromUtf8("模式"));
        ui->lblModeMsg->setStyleSheet("QLabel{color:red;}");
        ui->lblModeMsg->setText(QString::fromUtf8("无网络"));
    }

    //电台状态显示
    if(state == 1){
        ui->lblMode->setVisible(false);
        ui->lblModeMsg->setVisible(false);
        ui->errMsg->setVisible(true);

        ui->errMsg->setStyleSheet("QLabel{color:red;}");
        ui->errMsg->setText(QString::fromUtf8("电台控制异常"));
    }

    //注册状态显示
    if(registACK == 1){
        ui->lblMode->setVisible(false);
        ui->lblModeMsg->setVisible(false);
        ui->errMsg->setVisible(true);

        ui->errMsg->setStyleSheet("QLabel{color:red;}");
        ui->errMsg->setText(QString::fromUtf8("通道注册已上限"));
    }

    if(registACK == 2){
        ui->lblMode->setVisible(false);
        ui->lblModeMsg->setVisible(false);
        ui->errMsg->setVisible(true);

        ui->errMsg->setStyleSheet("QLabel{color:red;}");
        ui->errMsg->setText(QString::fromUtf8("控制电台不匹配"));
    }

    //////////////////////////////////////////////////////////////////////////////////

    //遥控和语音注册
    tCount ++;
    if(tCount >= 2){
        tCount = 0;

//        qDebug()<<"Widget"<< index << " " << radioTyp << "----------registApply" <<QTime::currentTime();
        SocketManage::getInstance()->registApplySocket(index);
    }

    //判断是否断连
    int currTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
    int diffTim = currTim - regAckTim;
    if(diffTim > 7){
        state = -1;
    }
}


//////////////////////////////////////////////////////////////////////////////////

void WidgeVHF171d::setMute()
{

    ui->lblMute->setText(QString::fromUtf8("静音"));
    ui->barSound->setValue(0);
    ui->barSound->setVisible(false);
}

void WidgeVHF171d::setVolume(int volumnLev)
{
    int volBar  = this->getVolumnBar(volumnLev);

    ui->lblMute->setText(QString::fromUtf8("音量"));
    ui->barSound->setVisible(true);
    ui->barSound->setValue(volBar);
}

void WidgeVHF171d::setBkLight(int bkLightLev)
{

    // 面板背景
    this->setAutoFillBackground(true);
    QPalette p = this ->palette();

    QString btnStyle  = "color:#FF5809;font-size:18px;";
    QString lineStyle = "border-top:1px solid #FF5809;";
    QString lblStyle  = "color:#FF5809;";
    QString processBarStyle = "background-color:#B93B00;";
    DEF_CNANNEL_STYLE = "QLabel{color:#FF5809;font-weight:bold;font-size:90px;line-height:150px;}";

    resetIconMovie();

    if(bkLightLev == 0){

        p.setColor(QPalette::Window,Qt::black);
        this->setPalette(p);

    } else {

        p.setColor(QPalette::Window,Qt::white);
        this->setPalette(p);

        btnStyle  = "color:#000000;font-size:18px;";
        lineStyle = "border-top:1px solid #000000;";
        lblStyle  = "color:#000000;";
        processBarStyle = "background-color:#ffffff;";
        DEF_CNANNEL_STYLE = "color:#000000;font-weight:bold;font-size:90px;line-height:150px;";

    }

    cancelTmpChannel();
    ui->btnC   ->setStyleSheet(btnStyle);
    ui->btnB   ->setStyleSheet(btnStyle);
    ui->btnA   ->setStyleSheet(btnStyle);
    ui->btnD   ->setStyleSheet(btnStyle);
    ui->line_1 ->setStyleSheet(lineStyle);
    ui->line_2 ->setStyleSheet(lineStyle);
    ui->line_3 ->setStyleSheet(lineStyle);

    ui->lblChannel ->setStyleSheet(DEF_CNANNEL_STYLE);
    ui->lblMode    ->setStyleSheet(lblStyle);
    ui->lblModeMsg ->setStyleSheet(lblStyle);
    ui->lblMute    ->setStyleSheet(lblStyle);

    ui->lblAddr     ->setStyleSheet(lblStyle);
    ui->lblAddrVal  ->setStyleSheet(lblStyle);

    ui->lblBD     ->setStyleSheet(lblStyle);
    ui->lblTime   ->setStyleSheet(lblStyle);
    ui->lblLat    ->setStyleSheet(lblStyle);
    ui->lblLatVal ->setStyleSheet(lblStyle);
    ui->lblLon    ->setStyleSheet(lblStyle);
    ui->lblLonVal ->setStyleSheet(lblStyle);

    ui->barSound->setStyleSheet(processBarStyle);
}


void WidgeVHF171d::setTmpChannel(QString param)
{
    setChnlTim = QTime::currentTime().addSecs(3);
    isSettingChnl = true;
    ui->lblChannel->setStyleSheet(SET_CNANNEL_STYLE);

    if(!param.isNull() && !param.isEmpty() ) {
        tmpChannel = tmpChannel.append(param);
        tmpChannel = tmpChannel.mid(tmpChannel.length()-2,tmpChannel.length());
        ui->lblChannel->setText(tmpChannel);

    } else {
         QString currChannl = ui->lblChannel->text();
         if(currChannl.isNull() || currChannl.isEmpty()) {
            ui->lblChannel->setText("");
         }
    }

}


void WidgeVHF171d::cancelTmpChannel()
{
    ui->lblChannel->setText(curChannel);
    ui->lblChannel->setStyleSheet(DEF_CNANNEL_STYLE);
    tmpChannel = "0";
    isSettingChnl = false;
}


//////////////////////////////////////////////////////////////////////////////////


void WidgeVHF171d::onKey(int key)
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

void WidgeVHF171d::onKeyA()
{
     //qDebug()<<"A";
     UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
     if(udpRctrl != NULL) {
         const static uint8_t mode[] = {0, 3, 5}; //0:模话 3:定频 5:跳频
         static uint8_t curMode = 0;
         for (int i = 0; i < sizeof(mode); ++i)
         {
             if (workModel == mode[i])
             {
                 curMode = i;
                 break;
             }
         }
         ++curMode;
         if(curMode >= sizeof(mode))
             curMode = 0;
         //workModel = mode[curMode];
         udpRctrl->sendRadioCtrl(Set_WorkMod, mode[curMode]);
     }
}

void WidgeVHF171d::onKeyB()
{
    qDebug()<<"B";
}

void WidgeVHF171d::onKeyC()
{
//    qDebug()<<radioTypInt;
//    if(RadioUtil::isVHF(radioTypInt)){
//        UIManager::getInstance()->changeCurrWidge();
//    }

}

void WidgeVHF171d::onKeyD()
{

    WidgeBase::volumnLev  = 0;

    int volALSA = this->getVolumnALSA(WidgeBase::volumnLev);
    AudioControl::getInstance()->getMixer()->volumn(volALSA);
    UIManager::getInstance()->updateAllMute();
}

//////////////////////////////////////////////////////////////////////////////////

void WidgeVHF171d::onKeyChannelUp()
{

    int currChanl = tmpChannel.toInt();
    if(!isSettingChnl){
        currChanl = curChannel.toInt();
    }

    currChanl = currChanl + 1;
    if(currChanl > 19 ) {
        currChanl = 1;
    }

    qDebug()<<"onBtnChannelUp" << currChanl;

    setTmpChannel(QString("%1").arg(currChanl, 2, 10, QLatin1Char('0')));
}

void WidgeVHF171d::onKeyChannelDw()
{

    int currChanl = tmpChannel.toInt();
    if(!isSettingChnl){
        currChanl = curChannel.toInt();
    }

    currChanl = currChanl - 1;
    if(currChanl < 1 ) {
        currChanl = 1;
    }

    qDebug()<<"onBtnChannelDw" << currChanl;

    setTmpChannel(QString("%1").arg(currChanl, 2, 10, QLatin1Char('0')));
}


//////////////////////////////////////////////////////////////////////////////////

void WidgeVHF171d::onKeyCancel()
{
    cancelTmpChannel();
}

void WidgeVHF171d::onKeyConfirm()
{

    qDebug()<<"待设置信道号为："<<tmpChannel;
    UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
    if(udpRctrl != NULL && tmpChannel != NULL) {
//        udpRctrl->sendSetChannel(tmpChannel.toInt());
        udpRctrl->sendRadioCtrl(Set_Channel, tmpChannel.toInt());
    }

    cancelTmpChannel();
}

void WidgeVHF171d::onKey0()
{
    setTmpChannel("0");
}

void WidgeVHF171d::onKey1()
{
    setTmpChannel("1");
}

void WidgeVHF171d::onKey2()
{
    setTmpChannel("2");
}

void WidgeVHF171d::onKey3()
{
    setTmpChannel("3");
}

void WidgeVHF171d::onKey4()
{
    setTmpChannel("4");
}

void WidgeVHF171d::onKey5()
{
    setTmpChannel("5");
}

void WidgeVHF171d::onKey6()
{
    setTmpChannel("6");
}

void WidgeVHF171d::onKey7()
{
    setTmpChannel("7");
}

void WidgeVHF171d::onKey8()
{
    setTmpChannel("8");
}

void WidgeVHF171d::onKey9()
{
    setTmpChannel("9");
}

void WidgeVHF171d::onKeyUp()
{
    qDebug()<<"上";
}

void WidgeVHF171d::onKeyDw()
{
    qDebug()<<"下";
}

void WidgeVHF171d::onKeyLeft()
{
    qDebug()<<"左";
}

void WidgeVHF171d::onKeyRight()
{
    qDebug()<<"右";
}

void WidgeVHF171d::resetIconMovie()
{
    if(lightLev == 0){
        if(pttAck == 1) {
            ui->voiceIcon->setMovie(iconSelf_Dark);
        } else if(pttAck == 2) {
            ui->voiceIcon->setMovie(iconOthr_Dark);
        } else {
            ui->voiceIcon->setMovie(iconSelf_Dark);
        }
    } else {
        if(pttAck == 1) {
            ui->voiceIcon->setMovie(iconSelf_Day);
        } else if(pttAck == 2) {
            ui->voiceIcon->setMovie(iconOthr_Day);
        }  else {
            ui->voiceIcon->setMovie(iconSelf_Day);
        }
    }

}
