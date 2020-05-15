#ifndef WIDGESSB_H
#define WIDGESSB_H

#include "UI/WidgeBase.h"
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QMovie>
#include <QLabel>

namespace Ui {
class SsbCtrl;
}

class WidgeSSB : public WidgeBase
{
    Q_OBJECT
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

private:
    void setTmpChannel(QString param);
    void cancelTmpChannel();
    void showChnlErr(bool isErr);          // 设置信道后异常信息提示与显示信息切换

    void setTmpFreq(QString param);
    void cancelTmpFreq();
    void showFreqErr(bool isErr);          // 设置频率后异常信息提示与显示信息切换

    void keyInputParam(QString param);

    void setRxFreq(int param);
    void setTxFreq(int param);
    int  getFreqBar(int param);

    void resetIconMovie();

signals:

private slots:
    void onTimer();

private:
    Ui::SsbCtrl             *ui;

    //////////////////////////////////////////////////////

    int                     tmpPower;
    int                     tmpSquelch;
    int                     tmpWorkTyp;


    int                     minFreq;
    int                     maxFreq;

    bool                    isShowFreqErr;
    QTime                   showFreqErrTim;         // 显示错误信息时间
    QString                 DEF_FREQ_STYLE;         // default frequency style
    QString                 SET_FREQ_STYLE;         // set frequency style

    bool                    isSettingFreq;          //frequency
    QTime                   setFreqTim;
    QString                 curRxFreq;
    QString                 curTxFreq;
    QString                 freqTyp;       // RX,TX,ALL
    QString                 tmpFreq;       //准备设置的频点
    QLabel                  *lblRxFreq;
    QLabel                  *lblTxFreq;

    //////////////////////////////////////////////////////

    int                     minChnl;
    int                     maxChnl;

    bool                    isShowChnlErr;
    QTime                   showChnlErrTim;             // 显示错误信息时间

    QString                 DEF_CNANNEL_STYLE;          // default channel style
    QString                 SET_CNANNEL_STYLE;          // set channel value style
    QString                 DEF_CNANNEL_LBL_STYLE;      // set channel label style

    bool                    isSettingChnl;
    QTime                   setChnlTim;
    QString                 curChannel;
    QString                 tmpChannel;    //准备设置的信道

    //////////////////////////////////////////////////////
    QTimer*                 timer;
    int                     tCount;         // 注册计数器

    QMovie                  *iconSelf_Day;
    QMovie                  *iconOthr_Day;
    QMovie                  *iconSelf_Dark;
    QMovie                  *iconOthr_Dark;

};

#endif // WIDGESSB_H
