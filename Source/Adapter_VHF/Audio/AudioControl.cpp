#include "AudioControl.h"
#include "AudioRecoder.h"
#include "AudioPlayer.h"
#include "AudioMixer.h"
#include "AudioPtt.h"
#include <QMutexLocker>
#include <QDebug>
#include <QTime>

//0:Opus 1:G729
#define ENCODER_DECODER_TYPE 0

AudioControl* AudioControl::m_instance = NULL;
QMutex AudioControl::m_getMutex;

AudioControl::AudioControl()
             : m_recoder(new AudioRecoder)
             , m_player1(new AudioPlayer)
             , m_player2(new AudioPlayer)
             , m_player3(new AudioPlayer)
             , m_player4(new AudioPlayer)
             , m_ptt(new AudioPtt)
             , m_mixer(new AudioMixer)
{
    m_player1->setPlayID(1);
    m_player2->setPlayID(2);
    m_player3->setPlayID(3);
    m_player4->setPlayID(4);

    m_mixMap.clear();

}

AudioControl::~AudioControl()
{

}


AudioControl* AudioControl::getInstance()
{

    QMutexLocker locker(&m_getMutex);
    if (m_instance == NULL)
    {
        m_instance = new AudioControl;
    }

    return m_instance;
}

bool AudioControl::init(QString& status)
{
    /*---------- Audio Recoder Init ---------*/
    if (!m_recoder->init(status))
    {
        qDebug() << "Audio Capture Init err";
        return false;
    }
    m_recoder->start();

    /*----------- Audio Player Init ----------*/
    if (!m_player1->init(status))
    {
        qDebug() << "Audio Playback1 Init err";
        return false;
    }
    m_player1->start();


    if (!m_player2->init(status))
    {
        qDebug() << "Audio Playback2 Init err";
        return false;
    }
    m_player2->start();


    if (!m_player3->init(status))
    {
        qDebug() << "Audio Playback3 Init err";
        return false;
    }
    m_player3->start();


    if (!m_player4->init(status))
    {
        qDebug() << "Audio Playback4 Init err";
        return false;
    }
    m_player4->start();

    /*----------- Audio Mixer Init ----------*/
    if (!m_mixer->init(status))
    {
        qDebug() << "Sound Mixer Init err";
        //return false;
    }

    if (!m_ptt->init(status))
    {
        qDebug() << "PTT Init err";
    }
    m_ptt->start();

    return true;
}


int AudioControl::bindPlayID(){

    if(!m_player1->isBind()){
        m_player1->setBind(true);
        return m_player1->getPlayID();
    }

    if(!m_player2->isBind()){
        m_player2->setBind(true);
        return m_player2->getPlayID();
    }

    if(!m_player3->isBind()){
        m_player3->setBind(true);
        return m_player3->getPlayID();
    }

    if(!m_player4->isBind()){
        m_player4->setBind(true);
        return m_player4->getPlayID();
    }

   return -1;
}


void AudioControl::unbindPlayID(int playID){

    AudioPlayer* player = getPlayer(playID);

    if(player != NULL){
        player->setBind(false);
    }

}


AudioPlayer* AudioControl::getPlayer(int playID){

    if(playID == 1){
        return m_player1;
    }

    if(playID == 2){
        return m_player2;
    }

    if(playID == 3){
        return m_player3;
    }

    if(playID == 4){
        return m_player4;
    }

    return NULL;
}


void AudioControl::addMixerData(const uint8_t id, uint8_t *data, const unsigned int len)
{
    QMutexLocker locker(&m_mixMutex);
    MixerMap::const_iterator iter = m_mixMap.find(id);
    if (iter == m_mixMap.constEnd())
    {
        pAudioDataList tmp(new AudioDataList);
        m_mixMap.insert(id, tmp);
    }
    pAudioDataList list = m_mixMap.value(id);
    if (len > 0)
    {
        pAudioData d(new AudioData);
        memcpy(d->data, data, len);
        d->dataLen = len;
        //qDebug() << "List size = " << list->length();
        if (list->length() > 100)
        {
            //list->pop_front();
            AudioDataList::iterator iter = list->begin();
            list->erase(iter, iter+20);
        }
        list->append(d);
    }
}






