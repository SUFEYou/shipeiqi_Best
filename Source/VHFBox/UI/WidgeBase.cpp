#include "WidgeBase.h"
#include "UI/UIManager.h"
#include "Audio/AudioControl.h"
#include "Audio/AudioMixer.h"

int WidgeBase::volumnLev = 2;
int WidgeBase::lightLev  = 4;

WidgeBase::WidgeBase(QWidget *parent) : QWidget(parent)
{

}


void WidgeBase::upVolumn()
{
    volumnLev  = volumnLev  + 1;
    if(volumnLev > 4){
        volumnLev = 4;
    }
    if(volumnLev < 0){
        volumnLev = 0;
    }

    int volALSA = getVolumnALSA(volumnLev);
    AudioControl::getInstance()->getMixer()->volumn(volALSA);
    UIManager::getInstance()->updateAllVolume(volumnLev);
}


void WidgeBase::dwVolumn()
{
    volumnLev  = volumnLev  - 1;
    if(volumnLev > 4){
        volumnLev = 4;
    }
    if(volumnLev < 0){
        volumnLev = 0;
    }

    int volALSA = getVolumnALSA(volumnLev);
    AudioControl::getInstance()->getMixer()->volumn(volALSA);
    UIManager::getInstance()->updateAllVolume(volumnLev);

}

void WidgeBase::upBkLight()
{
    lightLev  = lightLev  + 1;
    if(lightLev > 4){
        lightLev = 4;
    }
    if(lightLev < 0){
        lightLev = 0;
    }

    UIManager::getInstance()->updateAllBkLight(lightLev);
}

void WidgeBase::dwBkLight()
{
    lightLev  = lightLev  - 1;
    if(lightLev > 4){
        lightLev = 4;
    }
    if(lightLev < 0){
        lightLev = 0;
    }

    UIManager::getInstance()->updateAllBkLight(lightLev);
}

int WidgeBase::getVolumnALSA(int volumnLev)
{
    if(volumnLev == 0){
        return 0;
    }
    if(volumnLev == 1){
        return 97;
    }
    if(volumnLev == 2){
        return 107;
    }
    if(volumnLev == 3){
        return 117;
    }
    if(volumnLev == 4){
        return 127;
    }

    return 127;
}

int WidgeBase::getVolumnBar(int volumnLev)
{
    if(volumnLev == 0){
        return 0;
    }
    if(volumnLev == 1){
        return 40;
    }
    if(volumnLev == 2){
        return 60;
    }
    if(volumnLev == 3){
        return 80;
    }
    if(volumnLev == 4){
        return 100;
    }

    return 100;
}

void WidgeBase::setRegistAck(int ack)
{
    registACK   = ack;
    regAckTim = QDateTime::currentDateTimeUtc().toTime_t();   //秒级

}

void WidgeBase::setPttAck(int ack)
{
    pttAck = ack;
}

void WidgeBase::setCtrlOutRestrict(int value)
{
    ctrlOutRestrict = value;
}

void WidgeBase::setCtrlInRestrict(int value)
{
    ctrlInRestrict = value;
}

void WidgeBase::setVoicOutRestrict(int value)
{
    voicOutRestrict = value;
}

void WidgeBase::setVoicInRestrict(int value)
{
    voicInRestrict = value;
}

void WidgeBase::uptWorkTyp(int param)
{
    workTyp = param;
}

void WidgeBase::uptChannel(int param)
{
    channel = param;
}

void WidgeBase::uptWorkMode(int param)
{
    workModel = param;
}

void WidgeBase::uptRevFreq(int param)
{
    revFreq = param;
}

void WidgeBase::uptSndFreq(int param)
{
    sndFreq = param;
}

void WidgeBase::uptPower(int param)
{
    power = param;
}

void WidgeBase::uptSquelch(int param)
{
    squelch = param;
}

void WidgeBase::uptStatus(int param)
{
    state = param;
}

void WidgeBase::uptCurrTim(QString &param)
{
    curTim = param;
}

void WidgeBase::uptCurrLat(QString &param)
{
    curLat = param;
}

void WidgeBase::uptCurrLon(QString &param)
{
    curLon = param;
}

int WidgeBase::getIndex() const
{
    return index;
}

void WidgeBase::setIndex(int value)
{
    index = value;
}


