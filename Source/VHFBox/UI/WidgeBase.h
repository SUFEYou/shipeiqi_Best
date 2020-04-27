#ifndef WIDGEBASE_H
#define WIDGEBASE_H

#include <QWidget>
#include <QString>
#include <QTime>
#include "Socket/SocketCommu.h"

class WidgeBase : public QWidget
{
    Q_OBJECT
public:
    explicit WidgeBase(QWidget *parent = 0);

    int getVolumnBar (int volumnLev);
    static int  getVolumnLev()  { return volumnLev; };
    static int  getVolumnALSA(int volumnLev);
    static void upVolumn();
    static void dwVolumn();

    static int  getBkLightLev() { return lightLev; }
    static void upBkLight();
    static void dwBkLight();

//    inline int getIndex() const { return index; }
//    inline void setIndex(int value) { index = value; }


    int getIndex() const;
    void setIndex(int value);

    inline int getRadioTypInt() const { return radioTypInt; }
    inline void setRadioTypInt(int value) { radioTypInt = value; }

    inline QString getRadioTyp() const { return radioTyp; }
    inline void setRadioTyp(const QString &value) { radioTyp = value; }

    inline QString getRadioID() const { return radioID; }
    inline void setRadioID(const QString &value) { radioID = value; }

    inline QString getBoxID() const { return boxID; }
    inline void setBoxID(const QString &value) { boxID = value; }

    inline int getState()     const { return state; }
    inline int getRegsitACK() const { return registACK; }
    inline int getPttACK()    const { return pttAck;}
    inline int getCtrlOutRestrict() const { return ctrlOutRestrict; }
    inline int getCtrlInRestrict()  const { return ctrlInRestrict;  }
    inline int getVoicOutRestrict() const { return voicOutRestrict; }
    inline int getVoicInRestrict()  const { return voicInRestrict;  }

    void setCtrlOutRestrict(int value);
    void setCtrlInRestrict (int value);
    void setVoicOutRestrict(int value);
    void setVoicInRestrict (int value);

    ///////////////////////////////////////////////////////////////////

    virtual void init() = 0;
    virtual void onKey(int key) = 0;

    virtual void setMute() = 0;
    virtual void setVolume(int volumnLev) = 0;
    virtual void setBkLight(int lightLev) = 0;

    virtual void setRegistAck(int regAck);
    virtual void setPttAck(int pttAck);

    virtual void uptChannel(int param);
    virtual void uptWorkMode(int param);
    virtual void uptRevFreq(int param);
    virtual void uptSndFreq(int param);
    virtual void uptPower(int param);
    virtual void uptSquelch(int param);
    virtual void uptStatus(int status);

    virtual void uptCurrTim(QString &param);
    virtual void uptCurrLat(QString &param);
    virtual void uptCurrLon(QString &param);


signals:

public slots:


protected:
    int                  index;
    int                  radioTypInt;
    QString              boxID;         //BoxID
    QString              radioTyp;      //电台型号名称
    QString              radioID;       //RadioID

    int                  workTyp;       //工作方式
    int                  workModel;     //工作模式
    int                  channel;       //工作信道
    int                  sndFreq;       //发射频率
    int                  revFreq;       //接受频率
    int                  power;         //功率
    int                  squelch;       //静噪
    int                  state;         //状态

    QString              curTim;        //当前时间
    QString              curLon;        //当前经度
    QString              curLat;        //当前维度

    static int           volumnLev;     //音量等级 0-4挡 0：静音
    static int           lightLev;      //背光等级 0-4挡

    int                  regAckTim;     //注册反馈时间
    int                  registACK;     //注册状态
    int                  pttAck;        //PTT状态

    int                  ctrlOutRestrict;            // 遥控(0：未受限 1：受限)
    int                  ctrlInRestrict;             // 状态(0：未受限 1：受限)
    int                  voicOutRestrict;            // 喊话(0：未受限 1：受限)
    int                  voicInRestrict;             // 监听(0：未受限 1：受限)

};

#endif // WIDGEBASE_H
