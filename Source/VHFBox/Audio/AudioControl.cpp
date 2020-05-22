#include "AudioControl.h"
#include "AudioRecoder.h"
#include "AudioPlayer.h"
#include "AudioMixer.h"
#include "Config/ConfigLoader.h"
#include "Config/BoxConf.h"
#include <QMutexLocker>
#include <QDebug>
#include <QTime>

//0:Opus 1:G729
#define ENCODER_DECODER_TYPE 0

AudioControl* AudioControl::m_instance = NULL;
QMutex AudioControl::m_getMutex;

AudioControl::AudioControl()
             : m_mixer(new AudioMixer)
{
    m_mixMap.clear();

    m_recoder = NULL;
    m_player1 = NULL;
    m_player2 = NULL;
    m_player3 = NULL;
    m_player4 = NULL;
}

AudioControl::~AudioControl()
{
    delete m_recoder;
    delete m_player1;
    delete m_player2;
    delete m_player3;
    delete m_player4;
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
    qDebug() << "//////////////////////////////////////////////////////////////";

    /*---------- Audio Recoder Init ---------*/
    m_recoder = new AudioRecoder();
    if (!m_recoder->init(status))
    {
        qDebug() << "Audio Capture Init err";
        return false;
    }
    m_recoder->start();

    BoxConf *boxConf1 = ConfigLoader::getInstance()->getBoxConf1();
    BoxConf *boxConf2 = ConfigLoader::getInstance()->getBoxConf2();
    BoxConf *boxConf3 = ConfigLoader::getInstance()->getBoxConf3();
    BoxConf *boxConf4 = ConfigLoader::getInstance()->getBoxConf4();

    /*----------- Audio Player Init ----------*/
    if(boxConf1->getBoxEnable()){

        m_player1 = new AudioPlayer();
        m_player1->setPlayID(1);
        m_player1->setPlayEn(true);
        if (!m_player1->init(status))
        {
            qDebug() << "Audio Playback1 Init err";
            return false;
        }
        m_player1->start();

    } else {
        m_player1 = NULL;
    }

    if(boxConf2->getBoxEnable()){

        m_player2 = new AudioPlayer();
        m_player2->setPlayID(2);
        m_player2->setPlayEn(true);
        if (!m_player2->init(status))
        {
            qDebug() << "Audio Playback2 Init err";
            return false;
        }
        m_player2->start();

    } else {
        m_player2 = NULL;
    }

    if(boxConf3->getBoxEnable()){

        m_player3 = new AudioPlayer();
        m_player3->setPlayID(3);
        m_player3->setPlayEn(true);
        if (!m_player3->init(status))
        {
            qDebug() << "Audio Playback3 Init err";
            return false;
        }
        m_player3->start();

    } else {
        m_player3 = NULL;
    }

    if(boxConf4->getBoxEnable()){

        m_player4 = new AudioPlayer();
        m_player4->setPlayID(4);
        m_player4->setPlayEn(true);
        if (!m_player4->init(status))
        {
            qDebug() << "Audio Playback4 Init err";
            return false;
        }
        m_player4->start();

    } else {
        m_player4 = NULL;
    }


    /*----------- Audio Mixer Init ----------*/
    if (!m_mixer->init(status))
    {
        qDebug() << "Sound Mixer Init err";
        //return false;
    }


    return true;
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


AudioPlayer* AudioControl::getPlayer(int ID)
{
    if(ID == 1){
        return m_player1;
    }
    if(ID == 2){
        return m_player2;
    }
    if(ID == 3){
        return m_player3;
    }
    if(ID == 4){
        return m_player4;
    }
    return NULL;
}




