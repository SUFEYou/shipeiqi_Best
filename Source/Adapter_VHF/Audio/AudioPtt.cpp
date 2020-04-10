#include "AudioPtt.h"
#include "Audio/AudioControl.h"
#include "Audio/AudioPlayer.h"

AudioPtt::AudioPtt()
{

}

bool AudioPtt::init(QString& status)
{

    pttOnTim = -1;
    pttOnLim = 300;
    return true;
}

void AudioPtt::run()
{

    int rc = 0;

    while (!m_stop)
    {
        bool pttOn = false;
        int  pttProri = -1;
        for(int i=0; i<4; i++)
        {
            PttSet pttSet = pttSetArr[i];
            if(pttSet.PttOn)
            {
                pttOn = true;
                int prori = pttSet.Priority;
                if(prori > pttProri){
                    pttProri = prori;
                }
            }
        }

        if(!pttOn)
        {
            pttOnTim = -1;
            AudioControl::getInstance()->getPlayer1()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer2()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer3()->setPlayEn(false);
            AudioControl::getInstance()->getPlayer4()->setPlayEn(false);

            //TODO serial Ctrl-PttOff
        }

        if(pttOn)
        {
            bool pttStop = false;
            if(pttOnTim < 0){
                pttOnTim = QDateTime::currentDateTimeUtc().toTime_t();
            } else {
                int currTime = QDateTime::currentDateTimeUtc().toTime_t();
                int diffTime = currTime - pttOnTim;
                if(diffTime > pttOnLim){
                    pttOnTim = -1;
                    pttStop = true;
                }
            }

            if(!pttStop){
                for(int i=0; i<4; i++)
                {
                    PttSet pttSet = pttSetArr[i];
                    if(pttSet.PttOn && pttSet.Priority >= pttProri)
                    {
                        AudioControl::getInstance()->getPlayer(i+1)->setPlayEn(true);
                    } else {
                        AudioControl::getInstance()->getPlayer(i+1)->setPlayEn(false);
                    }
                }

                //TODO serial Ctrl-PttOff
            } else {

                for(int i=0; i<4; i++)
                {
                    pttSetArr[i].PttOn = 0;
                }
                //TODO serial Ctrl-PttOn
            }

        }

        msleep(10);

    }
    m_stop = false;
}


void AudioPtt::setPriority_PttOn(int pID, int priority, int pttOn)
{
    PttSet pttSet;
    pttSet.Priority = priority;
    pttSet.PttOn    = pttOn;

    pttSetArr[pID] = pttSet;

}


int AudioPtt::getPttOn(int pID)
{
    if(pID >=0 && pID < 4){
        return pttSetArr[pID].PttOn;
    }
    return -1;
}


int AudioPtt::getPriority(int pID)
{
    if(pID >=0 && pID < 4){
        return pttSetArr[pID].Priority;
    }
    return -1;
}
