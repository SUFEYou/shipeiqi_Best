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


    icon1_outActSelf = new QMovie("images/mic-green-lt.gif");
    icon1_outActOthr = new QMovie("images/mic-orange-lt.gif");
    icon1_outRes     = new QMovie("images/mic-disabled-lt.jpg");

    icon2_outActSelf = new QMovie("images/mic-green-lt.gif");
    icon2_outActOthr = new QMovie("images/mic-orange-lt.jpg");
    icon2_outRes     = new QMovie("images/mic-disabled-lt.jpg");

    icon1_inAct  = new QMovie("images/vol-green-lt.jpg");
    icon1_inRes  = new QMovie("images/vol-disabled-lt.jpg");

    icon2_inAct  = new QMovie("images/vol-green-lt.jpg");
    icon2_inRes  = new QMovie("images/vol-disabled-lt.jpg");

    QSize iconSize1(64,64);
    QSize iconSize2(50,50);
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

    //Dark
    icon1_outActSelf_Dark = new QMovie("images/mic-green-dk.gif");
    icon1_outActOthr_Dark = new QMovie("images/mic-orange-dk.gif");
    icon1_outRes_Dark     = new QMovie("images/mic-disabled-dk.jpg");

    icon2_outActSelf_Dark = new QMovie("images/mic-green-dk.gif");
    icon2_outActOthr_Dark = new QMovie("images/mic-orange-dk.gif");
    icon2_outRes_Dark     = new QMovie("images/mic-disabled-dk.jpg");

    icon1_inAct_Dark  = new QMovie("images/vol-green-dk.jpg");
    icon1_inRes_Dark  = new QMovie("images/vol-disabled-dk.jpg");

    icon2_inAct_Dark  = new QMovie("images/vol-green-dk.jpg");
    icon2_inRes_Dark  = new QMovie("images/vol-disabled-dk.jpg");



    icon1_outActSelf_Dark->setScaledSize(iconSize1);
    icon1_outActOthr_Dark->setScaledSize(iconSize1);
    icon1_outRes_Dark->setScaledSize(iconSize1);

    icon2_outActSelf_Dark->setScaledSize(iconSize1);
    icon2_outActOthr_Dark->setScaledSize(iconSize1);
    icon2_outRes_Dark->setScaledSize(iconSize1);

    icon1_inAct_Dark->setScaledSize(iconSize2);
    icon1_inRes_Dark->setScaledSize(iconSize2);

    icon2_inAct_Dark->setScaledSize(iconSize2);
    icon2_inRes_Dark->setScaledSize(iconSize2);

    icon1_outActSelf_Dark->start();
    icon1_outActOthr_Dark->start();
    icon1_outRes_Dark->start();

    icon2_outActSelf_Dark->start();
    icon2_outActOthr_Dark->start();
    icon2_outRes_Dark->start();

    icon1_inAct_Dark->start();
    icon1_inRes_Dark->start();

    icon2_inAct_Dark->start();
    icon2_inRes_Dark->start();

    index1 = 0;
    index2 = 0;

    curTim = "";
    curLon = "";
    curLat = "";

    icon1_inAct->stop();
    icon2_inAct->stop();

    icon1_inAct_Dark->stop();
    icon2_inAct_Dark->stop();

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

      updateRadioState();

      resetIconMovie();

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
    QString processBarStyle = "background-color:#B93B00;";

    resetIconMovie();

    if(bkLightLev == 0){

        p.setColor(QPalette::Window,Qt::black);
        this->setPalette(p);

    } else {

        p.setColor(QPalette::Window,Qt::white);
        this->setPalette(p);

        btnStyle  = "color:#000000;";
        lineStyle = "border-top:1px solid #000000;";
        lblStyle  = "color:#000000;";
        processBarStyle = "background-color:#ffffff;";

    }

    ui->lblMute->setStyleSheet(lblStyle);
    ui->lblRadioName1->setStyleSheet(lblStyle);
    ui->lblRadioName2->setStyleSheet(lblStyle);

    ui->barSound->setStyleSheet(processBarStyle);
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

void WidgeMulti::updateRadioState()
{
    WidgeBase * widget1 = UIManager::getInstance()->getWidge(index1);
    WidgeBase * widget2 = UIManager::getInstance()->getWidge(index2);
//    qDebug()<< "widget1->getState()"<< widget1->getState();
//    qDebug()<< "widget2->getState()"<< widget2->getState();

    //断连状态显示
    if(widget1->getState() == -1) {
        ui->lblRadioState1->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState1->setText(QString::fromUtf8("无网络"));
    } else if(widget1->getState() == 1) {
        ui->lblRadioState1->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState1->setText(QString::fromUtf8("电台控制异常"));
    } else {
        ui->lblRadioState1->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState1->setText(QString::fromUtf8(""));
    }

    //注册状态显示
    if(widget1->getRegsitACK() == 1){
        ui->lblRadioState1->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState1->setText(QString::fromUtf8("通道注册已上限"));
    } else if(widget1->getRegsitACK() == 2){
        ui->lblRadioState1->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState1->setText(QString::fromUtf8("控制电台不匹配"));
    }


    //断连状态显示
    if(widget2->getState() == -1) {
        ui->lblRadioState2->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState2->setText(QString::fromUtf8("无网络"));
    } else if(widget2->getState() == 1) {
        ui->lblRadioState2->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState2->setText(QString::fromUtf8("电台控制异常"));
    } else {
        ui->lblRadioState2->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState2->setText(QString::fromUtf8(""));
    }

    //注册状态显示
    if(widget2->getRegsitACK() == 1){
        ui->lblRadioState2->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState2->setText(QString::fromUtf8("通道注册已上限"));
    } else if(widget2->getRegsitACK() == 2){
        ui->lblRadioState2->setStyleSheet("QLabel{color:red;}");
        ui->lblRadioState2->setText(QString::fromUtf8("控制电台不匹配"));
    }

}

void WidgeMulti::resetIconMovie()
{
    WidgeBase * widget1 = UIManager::getInstance()->getWidge(index1);
    WidgeBase * widget2 = UIManager::getInstance()->getWidge(index2);

    if(lightLev == 0) {
        if(widget1 != NULL){
            int pttACK = widget1->getPttACK();
            if(pttACK == 0){
                icon1_outActSelf_Dark->stop();
                ui->lblVoice1_1->setMovie(icon1_outActSelf_Dark);
            }
            if(pttACK == 1){
                icon1_outActSelf_Dark->start();
                ui->lblVoice1_1->setMovie(icon1_outActSelf_Dark);
            }
            if(pttACK == 2){
                icon1_outActOthr_Dark->start();
                ui->lblVoice1_1->setMovie(icon1_outActOthr_Dark);
            }
        }

        if(widget2 != NULL){
            int pttACK = widget2->getPttACK();
            if(pttACK == 0){
                icon2_outActSelf_Dark->stop();
                ui->lblVoice2_1->setMovie(icon2_outActSelf_Dark);
            }
            if(pttACK == 1){
                icon2_outActSelf_Dark->start();
                ui->lblVoice2_1->setMovie(icon2_outActSelf_Dark);
            }
            if(pttACK == 2){
                icon2_outActOthr_Dark->start();
                ui->lblVoice2_1->setMovie(icon2_outActOthr_Dark);
            }

        }

        ui->lblVoice1_2->setMovie(icon1_inAct_Dark);
        ui->lblVoice2_2->setMovie(icon2_inAct_Dark);
    } else {
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

        ui->lblVoice1_2->setMovie(icon1_inAct);
        ui->lblVoice2_2->setMovie(icon2_inAct);
    }


}
