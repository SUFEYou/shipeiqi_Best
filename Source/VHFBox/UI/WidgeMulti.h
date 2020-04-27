#ifndef WIDGEMULTI_H
#define WIDGEMULTI_H

#include "UI/WidgeBase.h"
#include <QTimer>
#include <QTime>
#include <QMovie>
#include <QObject>

namespace Ui {
class VhfVoice;
}

class WidgeMulti : public WidgeBase
{
    Q_OBJECT
public:
    WidgeMulti();

    ///////////////////////////////////////////////////////////////////

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



private slots:
    void onTimer();


private:
    Ui::VhfVoice             *ui;

    int                     index1;
    int                     index2;

    QMovie                  *icon1_outActSelf;
    QMovie                  *icon1_outActOthr;
    QMovie                  *icon1_outRes;
    QMovie                  *icon1_inAct;
    QMovie                  *icon1_inRes;

    QMovie                  *icon2_outActSelf;
    QMovie                  *icon2_outActOthr;
    QMovie                  *icon2_outRes;
    QMovie                  *icon2_inAct;
    QMovie                  *icon2_inRes;

    QTimer*                 timer;

};

#endif // WIDGEMULTI_H
