#include "WidgeSSB220.h"
#include "ui_SsbCtrl.h"
#include <QObject>
#include <QDebug>
#include <QSize>
#include <QPainter>
#include "Audio/AudioControl.h"
#include "Audio/AudioMixer.h"
#include "Socket/SocketManage.h"
#include "Socket/UdpRctrl.h"
#include "Util/RadioUtil.h"
#include "UI/UIManager.h"

WidgeSSB220::WidgeSSB220()
{
    ui = new Ui::SsbCtrl;
    ui->setupUi(this);
    // 去标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);

    index = 0;
    workModel = -1;
    workTyp   = -1;
    channel   = -1;
    revFreq   = -1;
    sndFreq   = -1;
    power     = -1;
    squelch   = -1;
    state     = -1;

    tmpPower     = -1;
    tmpSquelch   = -1;
    tmpWorkTyp   = -1;

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

    minChnl =  0;
    maxChnl = 255;

    minFreq =  0;
    maxFreq = 300000;

    QSize iconSize(64,64);
    iconSelf_Day  = new QMovie("images/mic-green-lt.gif");
    iconOthr_Day  = new QMovie("images/mic-orange-lt.gif");
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

void WidgeSSB220::init()
{

    curChannel = "";
    tmpChannel = "000";

    tmpFreq = "";

    SET_CNANNEL_STYLE = "color:#00FF00;font-weight:bold;font-size:25px;";
    SET_FREQ_STYLE = "color:#00FF00;font-weight:bold;font-size:30px;";

    ui->lblAddrVal->setText(radioTyp);
    ui->lblTime   ->setText("");
    ui->lblLatVal ->setText("");
    ui->lblLonVal ->setText("");

    ui->btnA->setFlat(true);
    ui->btnB->setFlat(true);
    ui->btnC->setFlat(true);
    ui->btnD->setFlat(true);



    ui->lblChnlErr->setText(QString::fromUtf8("信道超出范围"));

    ui->lblFreqErr->setText(QString::fromUtf8("频率超出范围"));
    ui->lblFreqErr->setWordWrap(true);
    ui->lblFreqErr->setAlignment(Qt::AlignTop);

    this->showChnlErr(false);
    this->showFreqErr(false);

    this->setBkLight(lightLev);

    ui->voiceIcon->hide();

    // 初始化 Timer
    setChnlTim = QTime::currentTime();

    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

}

void WidgeSSB220::onTimer()
{

    //////////////////////////////////////////////////////////////////////////////////
    // 经纬度 时间
    if(!curTim.isNull() && !curTim.isEmpty()) {
        ui->lblTime->setText(curTim);
    }

    if(!curLat.isNull() && !curLat.isEmpty()) {
        ui->lblLatVal->setText(curLat);
    }

    if(!curLon.isNull() && !curLon.isEmpty()) {
        ui->lblLonVal->setText(curLon);
    }

    //////////////////////////////////////////////////////////////////////////////////
    resetIconMovie();

    if(pttAck == 1) {
        ui->voiceIcon->setVisible(true);

        ui->lblRx->setVisible(false);
        ui->barRx->setVisible(false);
        ui->lblTx->setVisible(false);
        ui->barTx->setVisible(false);

    } else if(pttAck == 2) {
        ui->voiceIcon->setVisible(true);

        ui->lblRx->setVisible(false);
        ui->barRx->setVisible(false);
        ui->lblTx->setVisible(false);
        ui->barTx->setVisible(false);

    } else {
        ui->voiceIcon->hide();

        ui->lblRx->setVisible(true);
        ui->barRx->setVisible(true);
        ui->lblTx->setVisible(true);
        ui->barTx->setVisible(true);
    }

    //////////////////////////////////////////////////////////////////////////////////

    //电台信道显示
    if(isSettingChnl){
        if(QTime::currentTime() > setChnlTim)
        {
            cancelTmpChannel();
        }
    } else {

        if(channel >= 0){
            ui->lblChnlVal->setStyleSheet(DEF_CNANNEL_STYLE);
            QString chanlTxt = QString("%1").arg(channel, 3, 10, QLatin1Char('0'));
            ui->lblChnlVal->setText(chanlTxt);
            curChannel = chanlTxt;
            tmpChannel = "000";
        } else {
            ui->lblChnlVal->setText("");
        }
    }

    //错误提示信息
    if(isShowChnlErr) {

        if(QTime::currentTime() > showChnlErrTim)
        {
            //电台信道错误提示信息
            showChnlErr(false);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////

    //电台工作状态显示
    if(workModel >= 0) {
       //工作模式,取值1：数话； 2：模话，
        QString workModTxt;
       if(workModel == 1) {
           workModTxt = QString::fromUtf8("数话");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       } else if(workModel == 2) {
           workModTxt = QString::fromUtf8("模话");

           if(lightLev != 0)
           ui->lblModeMsg->setStyleSheet("color:black;");

       } else {
           //qDebug() << "------------------------------------" << workModel;
//           workModTxt = QString::fromUtf8("未知");

//           ui->lblModeMsg->setStyleSheet("color:red;");
       }

       ui->lblMode->setVisible(true);
       ui->lblModeMsg->setVisible(true);
       ui->errMsg->setVisible(false);

       ui->lblMode->setText(QString::fromUtf8("模式"));
       ui->lblModeMsg->setText(workModTxt);
    }

    //////////////////////////////////////////////////////////////////////////////////

    //电台频点显示
    if(isSettingFreq){
        if(QTime::currentTime() > setFreqTim)
        {
            cancelTmpFreq();
        }
    } else {

        //电台传输频率格式为KHZ*10000
        if(sndFreq >= 0){
            ui->lblTxFreq->setStyleSheet(DEF_FREQ_STYLE);
            //QString chanlTxt = QString("%1.%2").arg(sndFreq/10000, 5, 10, QLatin1Char('0')).arg(sndFreq%10000/1000);
            QString chanlTxt = QString("%1.%2").arg(sndFreq/10000, 5, 10, QLatin1Char('0')).arg(sndFreq%10000/1000);
            setTxFreq(sndFreq/10000);
            ui->lblTxFreq->setText(chanlTxt);
            curTxFreq = chanlTxt;
            tmpFreq = "";
        } else {
            ui->lblTxFreq->setText("");
        }

        if(revFreq >= 0){
            ui->lblRxFreq->setStyleSheet(DEF_FREQ_STYLE);
            //QString chanlTxt = QString("%1").arg(revFreq, 6, 10, QLatin1Char('0'));
            //QString chanlTxt = QString("%1").arg(revFreq/10000, 5, 10, QLatin1Char('0')).arg(revFreq%10000/1000);
            QString chanlTxt = QString("%1.%2").arg(revFreq/10000, 5, 10, QLatin1Char('0')).arg(revFreq%10000/1000);
            setRxFreq(revFreq/10000);
            ui->lblRxFreq->setText(chanlTxt);
            curRxFreq = chanlTxt;
            tmpFreq = "";
        } else {
            ui->lblRxFreq->setText("");
        }
    }

    //错误提示信息
    if(isShowFreqErr) {

        if(QTime::currentTime() > showFreqErrTim)
        {
            //电台信道错误提示信息
            showFreqErr(false);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    // 工作方式
    if (workTyp >= 0) {
        if(workTyp == 0) {
             ui->lblWorkTyp->setText(QString::fromUtf8("USB"));    //0:USB(上边带)
        }
        if(workTyp == 1) {
             ui->lblWorkTyp->setText(QString::fromUtf8("LSB"));    //1:LSB(下边带)
        }
        if(workTyp == 2) {
             ui->lblWorkTyp->setText(QString::fromUtf8("AM"));     //2:AM(调幅报)
        }
        if(workTyp == 3) {
             ui->lblWorkTyp->setText(QString::fromUtf8("CW"));     //3:CW(等幅报)
        }
    }

    // 功率
    if (power >= 0) {
        if(power == 0) {
             ui->lblPower->setText(QString::fromUtf8("高"));    //0:高
        }
        if(power == 2) {
             ui->lblPower->setText(QString::fromUtf8("中"));    //2:中
        }
        if(power == 1) {
             ui->lblPower->setText(QString::fromUtf8("低"));    //3:低
        }
    }

    // 静噪
    if (squelch >= 0) {
         ui->lblNoise->setText(QString::fromUtf8("静噪") + QString::number(squelch));    //0~5
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

//        //qDebug()<<"Widget"<< index << " " << radioTyp << "----------registApply" <<QTime::currentTime();
       SocketManage::getInstance()->registApplySocket(index);
   }

   //判断是否断连
   int currTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级
   int diffTim = currTim - regAckTim;
   if(diffTim > 7){
       state = -1;
   }
}


void WidgeSSB220::setMute()
{

    ui->lblMute->setText(QString::fromUtf8("静音"));
    ui->barSound->setValue(0);
    ui->barSound->setVisible(false);
}

void WidgeSSB220::setVolume(int volumnLev)
{
    int volBar  = this->getVolumnBar(volumnLev);

    ui->lblMute->setText(QString::fromUtf8("音量"));
    ui->barSound->setVisible(true);
    ui->barSound->setValue(volBar);
}

void WidgeSSB220::setBkLight(int bkLightLev)
{
    // 面板背景
    this->setAutoFillBackground(true);
    QPalette p = this ->palette();
    QString btnStyle  = "color:#FF5809;font-size:18px";
    QString lineStyle = "border-top:1px solid #FF5809;";
    QString lblStyle  = "color:#FF5809;";
    QString lblSmallStyle = "color:#FF5809;";
    QString processBarStyle = "background-color:#B93B00;";
    QString freqErrMsgStyle = "color:#FF5809;font-weight:bold;font-size:40px;";
    DEF_CNANNEL_LBL_STYLE = "color:#FF5809;";
    DEF_CNANNEL_STYLE = "QLabel{color:#FF5809;font-size:25px;}";
    DEF_FREQ_STYLE = "color:#FF5809;font-size:30px;";

    resetIconMovie();

    if(bkLightLev == 0) {

        p.setColor(QPalette::Window,Qt::black);
        this->setPalette(p);

    } else {

        p.setColor(QPalette::Window,Qt::white);
        this->setPalette(p);

        btnStyle  = "color:#000000;font-size:18px;";
        lineStyle = "border-top:1px solid #000000;";
        lblStyle  = "color:#000000;";
        lblSmallStyle= "color:#000000;";
        processBarStyle = "background-color:#ffffff;";
        freqErrMsgStyle = "color:#000000;font-weight:bold;font-size:40px;";
        DEF_CNANNEL_LBL_STYLE = "color:#000000;";
        DEF_CNANNEL_STYLE = "color:#000000;font-weight:bold;font-size:25px;";
        DEF_FREQ_STYLE = "color:#000000;font-weight:bold;font-size:30px;";
    }

    cancelTmpFreq();
    cancelTmpChannel();

    ui->btnC   ->setStyleSheet(btnStyle);
    ui->btnB   ->setStyleSheet(btnStyle);
    ui->btnA   ->setStyleSheet(btnStyle);
    ui->btnD   ->setStyleSheet(btnStyle);
    ui->line_1 ->setStyleSheet(lineStyle);
    ui->line_2 ->setStyleSheet(lineStyle);
    ui->line_3 ->setStyleSheet(lineStyle);

    ui->lblChnlErr  ->setStyleSheet(lblStyle);
    ui->lblChnl     ->setStyleSheet(lblStyle);
    ui->lblChnlVal  ->setStyleSheet(DEF_CNANNEL_STYLE);
    ui->lblNoise    ->setStyleSheet(lblStyle);
    ui->lblPower    ->setStyleSheet(lblStyle);
    ui->lblMode     ->setStyleSheet(lblStyle);
    ui->lblModeMsg  ->setStyleSheet(lblStyle);
    ui->lblWorkTyp  ->setStyleSheet(lblStyle);
    ui->lblMute     ->setStyleSheet(lblStyle);
    ui->lblRx       ->setStyleSheet(lblSmallStyle);
    ui->lblTx       ->setStyleSheet(lblSmallStyle);
    ui->lblFreqErr  ->setStyleSheet(freqErrMsgStyle);

    ui->lblRxFreq  ->setStyleSheet(DEF_FREQ_STYLE);
    ui->lblTxFreq  ->setStyleSheet(DEF_FREQ_STYLE);

    ui->lblAddr     ->setStyleSheet(lblStyle);
    ui->lblAddrVal  ->setStyleSheet(lblStyle);

    ui->lblBD     ->setStyleSheet(lblSmallStyle);
    ui->lblTime   ->setStyleSheet(lblSmallStyle);
    ui->lblLat    ->setStyleSheet(lblSmallStyle);
    ui->lblLatVal ->setStyleSheet(lblSmallStyle);
    ui->lblLon    ->setStyleSheet(lblSmallStyle);
    ui->lblLonVal ->setStyleSheet(lblSmallStyle);

    ui->barSound->setStyleSheet(processBarStyle);
    ui->barRx   ->setStyleSheet(processBarStyle);
    ui->barTx   ->setStyleSheet(processBarStyle);

}

void WidgeSSB220::setTmpChannel(QString param)
{
    setChnlTim = QTime::currentTime().addSecs(3);
    isSettingChnl = true;
    ui->lblChnl->setStyleSheet("color:#00FF00");
    ui->lblChnlVal->setStyleSheet(SET_CNANNEL_STYLE);

    if(!param.isNull() && !param.isEmpty() ) {
        tmpChannel = tmpChannel.append(param);
        tmpChannel = tmpChannel.mid(tmpChannel.length()-3,tmpChannel.length());
        ui->lblChnlVal->setText(tmpChannel);
    } else {
        QString currChannl = ui->lblChnlVal->text();
        if(currChannl.isNull() || currChannl.isEmpty()) {
            ui->lblChnlVal->setText("");
        }
    }

}

void WidgeSSB220::cancelTmpChannel()
{
    ui->lblChnlVal->setText(curChannel);
    ui->lblChnlVal->setStyleSheet(DEF_CNANNEL_STYLE);

    showChnlErr(false);
}

void WidgeSSB220::showChnlErr(bool isErr) {
    // 恢复临时默认信道
    tmpChannel = "000";
    isSettingChnl = false;
    ui->lblChnl->setStyleSheet(DEF_CNANNEL_LBL_STYLE);

    if(isErr) {
        this->isShowChnlErr = true;
        ui->lblChnl->setVisible(false);
        ui->lblChnlVal->setVisible(false);
        ui->lblChnlErr->setVisible(true);
        showChnlErrTim = QTime::currentTime().addSecs(3);
    } else {
        this->isShowChnlErr = false;
        ui->lblChnl->setVisible(true);
        ui->lblChnlVal->setVisible(true);
        ui->lblChnlErr->setVisible(false);
    }
}

void WidgeSSB220::setTmpFreq(QString param)
{
    setFreqTim = QTime::currentTime().addSecs(3);
    isSettingFreq = true;
    if(lblRxFreq != NULL) {
        lblRxFreq->setStyleSheet(SET_FREQ_STYLE);
    }
    if(lblTxFreq != NULL) {
        lblTxFreq->setStyleSheet(SET_FREQ_STYLE);
    }


    if(!param.isNull() && !param.isEmpty() ) {
        //qDebug()<<"param----"<<param;
        tmpFreq = tmpFreq.append(param);
        if(tmpFreq.length() > 6) {
            tmpFreq = tmpFreq.mid(tmpFreq.length()-6,tmpFreq.length());
        }
        tmpFreq = tmpFreq.remove(".");
        //qDebug()<<"tmpFreq----"<<tmpFreq;
        //QString freqTxt = QString("%1").arg(tmpFreq.toInt(), 6, 10, QLatin1Char('0'));
        //QString pLeft = freqTxt.mid(0, 5);
        //QString pRight = freqTxt.mid(5);
        //QString freqView = pLeft.append(".").append(pRight);
        //qDebug()<<"freqView----"<<freqView;

        QString freqView = QString("%1.%2").arg(tmpFreq.toInt()/10, 5, 10, QLatin1Char('0')).arg(tmpFreq.toInt()%10);

        if(lblRxFreq != NULL) {
             lblRxFreq->setText(freqView);
        }
        if(lblTxFreq != NULL) {
             lblTxFreq->setText(freqView);
        }

    } else {
         QString currRxFreq = curRxFreq;
         if(currRxFreq.isNull() || currRxFreq.isEmpty()) {
             if(lblRxFreq != NULL) {
                  lblRxFreq->setText("");
             }

         }

         QString currTxFreq = curTxFreq;
         if(currTxFreq.isNull() || currTxFreq.isEmpty()) {
             if(lblTxFreq != NULL) {
                  lblTxFreq->setText("");
             }
         }
    }

}

void WidgeSSB220::cancelTmpFreq()
{
    ui->lblRxFreq->setText(curRxFreq);
    ui->lblTxFreq->setText(curTxFreq);
    ui->lblRxFreq->setStyleSheet(DEF_FREQ_STYLE);
    ui->lblTxFreq->setStyleSheet(DEF_FREQ_STYLE);

    showFreqErr(false);
}

void WidgeSSB220::showFreqErr(bool isErr) {
    lblRxFreq = NULL;
    lblTxFreq = NULL;
    // 恢复临时值
    tmpFreq = "";
    isSettingFreq = false;
    freqTyp="";

    if(isErr) {
        isShowFreqErr = true;
        ui->lblRxFreq->setVisible(false);
        ui->lblTxFreq->setVisible(false);
        ui->lblFreqErr->setVisible(true);
        showFreqErrTim = QTime::currentTime().addSecs(3);
    } else {
        isShowFreqErr = false;
        ui->lblRxFreq->setVisible(true);
        ui->lblTxFreq->setVisible(true);
        ui->lblFreqErr->setVisible(false);
    }
}


//////////////////////////////////////////////////////////////////////////////////

void WidgeSSB220::onKey(int key)
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

void WidgeSSB220::onKeyA()
{
     //qDebug()<<"A";
     UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
     if(udpRctrl != NULL) {
         const static uint8_t mode[] = {1, 2}; //1:数话 2:模话
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

void WidgeSSB220::onKeyB()
{
    //qDebug()<<"B";
    // 静噪
    tmpSquelch = squelch;
    tmpSquelch = tmpSquelch + 1;
    if(tmpSquelch > 5 ) {
        tmpSquelch = 0;
    }
    qDebug()<<"静噪:"<<tmpSquelch;
    UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
    if(udpRctrl != NULL)
    udpRctrl->sendRadioCtrl(Set_Squelch, tmpSquelch);
}

void WidgeSSB220::onKeyC()
{
    //qDebug()<<"C";
    // 功率
    tmpPower = power;
    tmpPower = tmpPower + 1;
    if(tmpPower > 2 ) {
        tmpPower = 0;
    }
    qDebug()<<"功率:"<<tmpPower;
    UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
    if(udpRctrl != NULL)
    udpRctrl->sendRadioCtrl(Set_Power, tmpPower);
}

void WidgeSSB220::onKeyD()
{
    //qDebug()<<"D";
    // 方式
    tmpWorkTyp = workTyp;
    tmpWorkTyp = tmpWorkTyp + 1;
    if(tmpWorkTyp > 3 ) {
        tmpWorkTyp = 0;
    }
    qDebug()<<"方式:"<<tmpWorkTyp;
    UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
    if(udpRctrl != NULL)
    udpRctrl->sendRadioCtrl(Set_WorkTyp, tmpWorkTyp);
}

void WidgeSSB220::onKeyChannelUp()
{
    int currChanl = tmpChannel.toInt();
    if(!isSettingChnl){
        currChanl = curChannel.toInt();
    }

    currChanl = currChanl + 1;
    if(currChanl > maxChnl ) {
        currChanl = 1;
    }

    //qDebug()<<"onBtnChannelUp" << currChanl;

    setTmpChannel(QString("%1").arg(currChanl, 3, 10, QLatin1Char('0')));
}

void WidgeSSB220::onKeyChannelDw()
{
    int currChanl = tmpChannel.toInt();
    if(!isSettingChnl){
        currChanl = curChannel.toInt();
    }

    currChanl = currChanl - 1;
    if(currChanl < 0 ) {
        currChanl = 0;
    }

    //qDebug()<<"onBtnChannelDw" << currChanl;

    setTmpChannel(QString("%1").arg(currChanl, 3, 10, QLatin1Char('0')));
}


//////////////////////////////////////////////////////////////////////////////////

void WidgeSSB220::onKeyCancel()
{
    cancelTmpFreq();
    cancelTmpChannel();
}

void WidgeSSB220::onKeyConfirm()
{
    //qDebug()<<"待设置信道号为："<<tmpChannel.toInt();
    if(!tmpChannel.isNull() && !tmpChannel.isEmpty() && isSettingChnl == true) {
        // 信道超出范围
        if(tmpChannel.toInt() > maxChnl) {
            showChnlErr(true);
        } else {
            //TODO::
            UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
            if(udpRctrl != NULL) {
        //        udpRctrl->sendSetChannel(tmpChannel.toInt());
                udpRctrl->sendRadioCtrl(Set_Channel, tmpChannel.toInt());
            }

            cancelTmpChannel();
        }

    } else {
        cancelTmpChannel();
    }

    if(!tmpFreq.isNull() && !tmpFreq.isEmpty() && isSettingFreq == true) {
        if(freqOverRange(tmpFreq.toInt())) {
            showFreqErr(true);
        } else {
            //TODO::
            UDPRctrl *udpRctrl = SocketManage::getInstance()->getCtrlUdp(index);
            //频率发送按照KHZ*10000发送至适配器
            uint64_t freq = tmpFreq.toInt()*1000;
            if(freqTyp == "RX") {
                //qDebug()<<"待设RX频点为："<<tmpFreq;
                udpRctrl->sendRadioCtrl(Set_RxFreq, freq);
            }

            if(freqTyp == "TX") {
                //qDebug()<<"待设TX频点为："<<tmpFreq;
                udpRctrl->sendRadioCtrl(Set_TxFreq, freq);
            }

            if(freqTyp == "ALL") {
                //qDebug()<<"待设RX频点为："<<tmpFreq;
                //qDebug()<<"待设TX频点为："<<tmpFreq;
                udpRctrl->sendRadioCtrl(Set_TxFreq, freq);
                udpRctrl->sendRadioCtrl(Set_RxFreq, freq);
            }

            cancelTmpFreq();
        }


    } else {
        cancelTmpFreq();
    }

}

void WidgeSSB220::onKey0()
{
    keyInputParam("0");
}

void WidgeSSB220::onKey1()
{
    keyInputParam("1");
}

void WidgeSSB220::onKey2()
{
    keyInputParam("2");
}

void WidgeSSB220::onKey3()
{
    keyInputParam("3");
}

void WidgeSSB220::onKey4()
{
    keyInputParam("4");
}

void WidgeSSB220::onKey5()
{
    keyInputParam("5");
}

void WidgeSSB220::onKey6()
{
    keyInputParam("6");
}

void WidgeSSB220::onKey7()
{
    keyInputParam("7");
}

void WidgeSSB220::onKey8()
{
    keyInputParam("8");
}

void WidgeSSB220::onKey9()
{
    keyInputParam("9");
}

void WidgeSSB220::onKeyUp()
{
    cancelTmpChannel();
    cancelTmpFreq();

    lblRxFreq = ui->lblRxFreq;
    freqTyp = "RX";
    setTmpFreq(curRxFreq);
}

void WidgeSSB220::onKeyDw()
{
    cancelTmpChannel();
    cancelTmpFreq();

    lblTxFreq = ui->lblTxFreq;
    freqTyp = "TX";
    setTmpFreq(curTxFreq);
}

void WidgeSSB220::onKeyLeft()
{
    cancelTmpChannel();
    cancelTmpFreq();

    lblRxFreq = ui->lblRxFreq;
    lblTxFreq = ui->lblTxFreq;
    freqTyp = "ALL";
    setTmpFreq(curRxFreq);
    setTmpFreq(curTxFreq);
}

void WidgeSSB220::onKeyRight()
{
   cancelTmpFreq();
   cancelTmpChannel();

   setTmpChannel(curChannel);
}


void WidgeSSB220::keyInputParam(QString param)
{
    if(isSettingChnl) {
        setTmpChannel(param);
    }

    if(isSettingFreq) {
        setTmpFreq(param);
    }

}

void WidgeSSB220::setRxFreq(int param)
{
    int freqBar = this->getFreqBar(param);
    ui->barRx->setValue(freqBar);
}

void WidgeSSB220::setTxFreq(int param)
{
    int freqBar = this->getFreqBar(param);
    ui->barTx->setValue(freqBar);
}

int WidgeSSB220::getFreqBar(int param)
{
    if(param >=100 && param <6000){
        return 20;
    }
    if(param >=6000 && param <12000){
        return 40;
    }
    if(param >=12000 && param <18000){
        return 60;
    }
    if(param >=18000 && param <24000){
        return 80;
    }
    if(param >=24000 && param <30000){
        return 100;
    }

    return 0;
}

void WidgeSSB220::resetIconMovie()
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

bool WidgeSSB220::freqOverRange(const int curFreq)
{
    if (freqTyp == "TX") {
        if ((curFreq < 16000) || (curFreq > 300000))
            return true;
        else
            return false;
    } else if (freqTyp == "RX") {
        if ((curFreq < 1000) || (curFreq > 300000))
            return true;
        else
            return false;
    } else {
        if ((curFreq < 16000) || (curFreq > 300000))
            return true;
        else
            return false;
    }
}