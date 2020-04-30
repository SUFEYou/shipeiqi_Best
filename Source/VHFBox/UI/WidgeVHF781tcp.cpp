#include "WidgeVHF781tcp.h"
#include "ui_VhfCtrl.h"
#include <QDebug>
#include "Audio/AudioControl.h"
#include "Audio/AudioMixer.h"
#include "Socket/SocketManage.h"
#include "Socket/UdpRctrl.h"
#include "Util/RadioUtil.h"
#include "UI/UIManager.h"


WidgeVHF781tcp::WidgeVHF781tcp()
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

    iconSelf  = new QMovie("images/voice_green.gif");
    iconOthr  = new QMovie("images/voice.gif");

    QSize iconSize(100,100);
    iconSelf->setScaledSize(iconSize);
    iconOthr->setScaledSize(iconSize);

    iconSelf->start();
    iconOthr->start();

}

void WidgeVHF781tcp::init()
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


void WidgeVHF781tcp::onTimer()
{
//    qDebug()<<"Widget"<< index << " " << radioTyp << "----------" <<pttAck;
    //////////////////////////////////////////////////////////////////////////////////

    if(pttAck == 1) {
        ui->voiceIcon->setMovie(iconSelf);
        ui->voiceIcon->setVisible(true);

    } else if(pttAck == 2) {
        ui->voiceIcon->setMovie(iconOthr);
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

    //电台工作状态显示
    if(workModel >= 0) {
       //工作模式,取值0：语音； 1：数传；
        QString workModTxt;
       if(workModel == 0) {
           workModTxt = QString::fromUtf8("语音");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       } else if(workModel == 1) {
           workModTxt = QString::fromUtf8("数传");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       }
//       else if(workModel == 2) {
//           workModTxt = QString::fromUtf8("集群");
//           ui->lblModeMsg->setStyleSheet("color:red;");
//       }

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

void WidgeVHF781tcp::setMute()
{

    ui->lblMute->setText(QString::fromUtf8("静音"));
    ui->barSound->setValue(0);
    ui->barSound->setVisible(false);
}

void WidgeVHF781tcp::setVolume(int volumnLev)
{
    int volBar  = this->getVolumnBar(volumnLev);

    ui->lblMute->setText(QString::fromUtf8("音量"));
    ui->barSound->setVisible(true);
    ui->barSound->setValue(volBar);
}

void WidgeVHF781tcp::setBkLight(int bkLightLev)
{

    // 面板背景
    this->setAutoFillBackground(true);
    QPalette p = this ->palette();

    QString btnStyle  = "color:#FF5809;";
    QString lineStyle = "border-top:1px solid #FF5809;";
    QString lblStyle  = "color:#FF5809;";
    DEF_CNANNEL_STYLE = "QLabel{color:#FF5809;font-weight:bold;font-size:90px;line-height:150px;}";

    if(bkLightLev == 0){

        p.setColor(QPalette::Window,Qt::black);
        this->setPalette(p);

    } else {

        p.setColor(QPalette::Window,Qt::white);
        this->setPalette(p);

        btnStyle  = "color:#000000;";
        lineStyle = "border-top:1px solid #000000;";
        lblStyle  = "color:#000000;";
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
}


void WidgeVHF781tcp::setTmpChannel(QString param)
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


void WidgeVHF781tcp::cancelTmpChannel()
{
    ui->lblChannel->setText(curChannel);
    ui->lblChannel->setStyleSheet(DEF_CNANNEL_STYLE);
    tmpChannel = "0";
    isSettingChnl = false;
}


//////////////////////////////////////////////////////////////////////////////////


void WidgeVHF781tcp::onKey(int key)
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

void WidgeVHF781tcp::onKeyA()
{
     qDebug()<<"A";
     UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
     if(udpRctrl != NULL) {
         workModel = workModel+1;
         if(workModel > 1)  {
             workModel = 0;
         }
//         udpRctrl->sendSetChannel(workModel);
         udpRctrl->sendRadioCtrl(Set_WorkMod, workModel);
     }
}

void WidgeVHF781tcp::onKeyB()
{
    qDebug()<<"B";
}

void WidgeVHF781tcp::onKeyC()
{
//    qDebug()<<radioTypInt;
//    if(RadioUtil::isVHF(radioTypInt)){
//        UIManager::getInstance()->changeCurrWidge();
//    }

}

void WidgeVHF781tcp::onKeyD()
{

    WidgeBase::volumnLev  = 0;

    int volALSA = this->getVolumnALSA(WidgeBase::volumnLev);
    AudioControl::getInstance()->getMixer()->volumn(volALSA);
    UIManager::getInstance()->updateAllMute();
}

//////////////////////////////////////////////////////////////////////////////////

void WidgeVHF781tcp::onKeyChannelUp()
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

void WidgeVHF781tcp::onKeyChannelDw()
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

void WidgeVHF781tcp::onKeyCancel()
{
    cancelTmpChannel();
}

void WidgeVHF781tcp::onKeyConfirm()
{

    qDebug()<<"待设置信道号为："<<tmpChannel;
    UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
    if(udpRctrl != NULL && tmpChannel != NULL) {
//        udpRctrl->sendSetChannel(tmpChannel.toInt());
        udpRctrl->sendRadioCtrl(Set_Channel, tmpChannel.toInt());
    }

    cancelTmpChannel();
}

void WidgeVHF781tcp::onKey0()
{
    setTmpChannel("0");
}

void WidgeVHF781tcp::onKey1()
{
    setTmpChannel("1");
}

void WidgeVHF781tcp::onKey2()
{
    setTmpChannel("2");
}

void WidgeVHF781tcp::onKey3()
{
    setTmpChannel("3");
}

void WidgeVHF781tcp::onKey4()
{
    setTmpChannel("4");
}

void WidgeVHF781tcp::onKey5()
{
    setTmpChannel("5");
}

void WidgeVHF781tcp::onKey6()
{
    setTmpChannel("6");
}

void WidgeVHF781tcp::onKey7()
{
    setTmpChannel("7");
}

void WidgeVHF781tcp::onKey8()
{
    setTmpChannel("8");
}

void WidgeVHF781tcp::onKey9()
{
    setTmpChannel("9");
}

void WidgeVHF781tcp::onKeyUp()
{
    qDebug()<<"上";
}

void WidgeVHF781tcp::onKeyDw()
{
    qDebug()<<"下";
}

void WidgeVHF781tcp::onKeyLeft()
{
    qDebug()<<"左";
}

void WidgeVHF781tcp::onKeyRight()
{
    qDebug()<<"右";
}
