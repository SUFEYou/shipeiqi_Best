#ifndef WIDGESSB_H
#define WIDGESSB_H

#include "UI/WidgeBase.h"
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QMovie>

namespace Ui {
class SsbCtrl;
}

class WidgeSSB : public WidgeBase
{
public:
    WidgeSSB();

    virtual void init();
    virtual void onKey(int key);

    virtual void setMute();
    virtual void setVolume(int volumnLev);
    virtual void setBkLight(int lightLev);

    void onKeyA();
    void onKeyB();
    void onKeyC();
    void onKeyD();

    void onKey0();
    void onKey1();
    void onKey2();
    void onKey3();
    void onKey4();
    void onKey5();
    void onKey6();
    void onKey7();
    void onKey8();
    void onKey9();
    void onKeyCancel();
    void onKeyConfirm();

    void onKeyUp();
    void onKeyDw();
    void onKeyLeft();
    void onKeyRight();

    void onKeyChannelUp();
    void onKeyChannelDw();

signals:

private slots:
    void onTimer();

private:
    Ui::SsbCtrl             *ui;


    QString                 curChannel;
    QString                 tmpChannel;    //准备设置的信道

    //////////////////////////////////////////////////////
    QTimer*                 timer;
    int                     tCount;         // 注册计数器

    QMovie                  *iconSelf;
    QMovie                  *iconOthr;

};

#endif // WIDGESSB_H
