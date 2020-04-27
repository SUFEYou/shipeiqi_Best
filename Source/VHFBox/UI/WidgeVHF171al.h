#ifndef WIDGEVHF171AL_H
#define WIDGEVHF171AL_H

#include "UI/WidgeBase.h"
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QMovie>

namespace Ui {
class VhfCtrl;
}

class WidgeVHF171al : public WidgeBase
{
    Q_OBJECT
public:
    WidgeVHF171al();

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


private:
    void setTmpChannel(QString param);
    void cancelTmpChannel();

signals:

private slots:
    void onTimer();

private:
    Ui::VhfCtrl             *ui;


    //////////////////////////////////////////////////////

    QString                 DEF_CNANNEL_STYLE;     // default channel style
    QString                 SET_CNANNEL_STYLE;     // set channel style

    bool                    isSettingChnl;
    QTime                   setChnlTim;
    QString                 tmpChannel;     //准备设置的信道
    QString                 curChannel;


    //////////////////////////////////////////////////////
    QTimer*                 timer;
    int                     tCount;         // 注册计数器

    QMovie                  *iconSelf;
    QMovie                  *iconOthr;


};

#endif // WIDGEVHF171AL_H
