#include "WidgeMulti.h"
#include "ui_VhfVoice.h"
#include "UI/UIManager.h"
#include <QDebug>

WidgeMulti::WidgeMulti()
{

    ui = new Ui::VhfVoice();
    ui->setupUi(this);
    // 去标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);


    icon1_outActSelf = new QMovie("images/voice_green.gif");
    icon1_outActOthr = new QMovie("images/voice.gif");
    icon1_outRes     = new QMovie("images/voice.gif");

    icon2_outActSelf = new QMovie("images/voice_green.gif");
    icon2_outActOthr = new QMovie("images/voice.gif");
    icon2_outRes     = new QMovie("images/voice.gif");

    icon1_inAct  = new QMovie("images/voice_green.gif");
    icon1_inRes  = new QMovie("images/voice.gif");

    icon2_inAct  = new QMovie("images/voice_green.gif");
    icon2_inRes  = new QMovie("images/voice.gif");

    QSize iconSize1(100,100);
    QSize iconSize2(60,60);
    icon1_outActSelf->setScaledSize(iconSize1);
    icon1_outActOthr->setScaledSize(iconSize1);
    icon1_outRes->setScaledSize(iconSize1);

    icon2_outActSelf->setScaledSize(iconSize1);
    icon2_outActOthr->setScaledSize(iconSize1);
    icon2_outRes->setScaledSize(iconSize1);

    icon1_inAct->setScaledSize(iconSize2);
    icon1_inRes->setScaledSize(iconSize2);

    icon2_inAct->setScaledSize(iconSize2);
    icon2_inRes->setScaledSize(iconSize2);

    icon1_outActSelf->start();
    icon1_outActOthr->start();
    icon1_outRes->start();

    icon2_outActSelf->start();
    icon2_outActOthr->start();
    icon2_outRes->start();

    icon1_inAct->start();
    icon1_inRes->start();

    icon2_inAct->start();
    icon2_inRes->start();

    index1 = 0;
    index2 = 0;

    curTim = "";
    curLon = "";
    curLat = "";

    icon1_inAct->stop();
    icon2_inAct->stop();

}

void WidgeMulti::init()
{

    WidgeBase * widget1 = UIManager::getInstance()->getWidge1();
    WidgeBase * widget2 = UIManager::getInstance()->getWidge2();
    WidgeBase * widget3 = UIManager::getInstance()->getWidge3();
    WidgeBase * widget4 = UIManager::getInstance()->getWidge4();

    if(widget1 != NULL){
        if(index1 == 0){
            index1 = 1;
        } else {
            index2 = 1;
        }
    }

    if(widget2 != NULL){
        if(index1 == 0){
            index1 = 2;
        } else {
            index2 = 2;
        }
    }

    if(widget3 != NULL){
        if(index1 == 0){
            index1 = 3;
        } else {
            index2 = 3;
        }
    }

    if(widget4 != NULL){
        if(index1 == 0){
            index1 = 4;
        } else {
            index2 = 4;
        }
    }

    ui->lblVoice1_1->setMovie(icon1_outActSelf);
    ui->lblVoice1_2->setMovie(icon1_inAct);

    ui->lblVoice2_1->setMovie(icon2_outActSelf);
    ui->lblVoice2_2->setMovie(icon2_inAct);

    WidgeBase * widIndex1 = UIManager::getInstance()->getWidge(index1);
    WidgeBase * widIndex2 = UIManager::getInstance()->getWidge(index2);

    if(widIndex1 != NULL){
        QString radio1;
        radio1.append(QString::fromUtf8("电台：")).append(widIndex1->getRadioTyp());
        ui->lblRadioName1->setText(radio1);
    }

    if(widIndex2 != NULL){
        QString radio2;
        radio2.append(QString::fromUtf8("电台：")).append(widIndex2->getRadioTyp());
        ui->lblRadioName2->setText(radio2);
    }

    this->setBkLight(lightLev);

    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

}


void WidgeMulti::onTimer()
{
//    icon1_outActSelf->stop();
//    icon1_outRes->stop();

//    ui->lblVoice1_1->setMovie(icon1_outRes);

//    qDebug()<< "Index1 =" << index1;
//    qDebug()<< "Index2 =" << index2;

    WidgeBase * widget1 = UIManager::getInstance()->getWidge(index1);
    WidgeBase * widget2 = UIManager::getInstance()->getWidge(index2);

    if(widget1 != NULL){
        int pttACK = widget1->getPttACK();
        if(pttACK == 0){
            icon1_outActSelf->stop();
            ui->lblVoice1_1->setMovie(icon1_outActSelf);
        }
        if(pttACK == 1){
            icon1_outActSelf->start();
            ui->lblVoice1_1->setMovie(icon1_outActSelf);
        }
        if(pttACK == 2){
            icon1_outActOthr->start();
            ui->lblVoice1_1->setMovie(icon1_outActOthr);
        }
    }

    if(widget2 != NULL){
        int pttACK = widget2->getPttACK();
        if(pttACK == 0){
            icon2_outActSelf->stop();
            ui->lblVoice2_1->setMovie(icon2_outActSelf);
        }
        if(pttACK == 1){
            icon2_outActSelf->start();
            ui->lblVoice2_1->setMovie(icon2_outActSelf);
        }
        if(pttACK == 2){
            icon2_outActOthr->start();
            ui->lblVoice2_1->setMovie(icon2_outActOthr);
        }

    }

}


void WidgeMulti::setMute()
{
    ui->lblMute->setText(QString::fromUtf8("静音"));
    ui->barSound->setValue(0);
    ui->barSound->setVisible(false);
}

void WidgeMulti::setVolume(int volumnLev)
{
    int volBar  = this->getVolumnBar(volumnLev);

    ui->lblMute->setText(QString::fromUtf8("音量"));
    ui->barSound->setVisible(true);
    ui->barSound->setValue(volBar);
}

void WidgeMulti::setBkLight(int bkLightLev)
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

    ui->lblMute->setStyleSheet(lblStyle);
    ui->lblRadioName1->setStyleSheet(lblStyle);
    ui->lblRadioName2->setStyleSheet(lblStyle);
}


void WidgeMulti::onKey(int key)
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

}


void WidgeMulti::onKeyA()
{

}

void WidgeMulti::onKeyB()
{

}

void WidgeMulti::onKeyC()
{


}

void WidgeMulti::onKeyD()
{

}


//////////////////////////////////////////////////////////////////////////////////

void WidgeMulti::onKeyCancel()
{

}

void WidgeMulti::onKeyConfirm()
{

}

void WidgeMulti::onKey0()
{

}

void WidgeMulti::onKey1()
{

}

void WidgeMulti::onKey2()
{

}

void WidgeMulti::onKey3()
{

}

void WidgeMulti::onKey4()
{

}

void WidgeMulti::onKey5()
{

}

void WidgeMulti::onKey6()
{

}

void WidgeMulti::onKey7()
{

}

void WidgeMulti::onKey8()
{

}

void WidgeMulti::onKey9()
{

}

void WidgeMulti::onKeyUp()
{

}

void WidgeMulti::onKeyDw()
{

}

void WidgeMulti::onKeyLeft()
{

}

void WidgeMulti::onKeyRight()
{

}