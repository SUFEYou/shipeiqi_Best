#ifndef AUDIOPLAYBACK_H
#define AUDIOPLAYBACK_H

#include <QThread>
#include <QTime>
#include <stdint.h>
#include "AudioControl.h"
#if !WIN32
#include "alsa/asoundlib.h"
#endif


class AudioPlayer : public QThread
{
    Q_OBJECT
public:
    AudioPlayer();
    bool init(QString& status);
    void write(uint8_t* data);

    void addPlayData(const uint8_t *data, const unsigned int len);

    uint8_t getPlayID() const;
    void setPlayID(const uint8_t PlayID);

    bool isPlayEn() const;
    void setPlayEn(bool PlayEn);

    bool isBind() const;
    void setBind(bool PlayIdle);

protected:
    virtual void run();

private:
    int  getPlayData(uint8_t *data, int &len);

private:
    bool                    m_stop;
#if !WIN32
    snd_pcm_t               *m_handle;      //pcm句柄
    snd_pcm_hw_params_t     *m_params;      //pcm属性
#endif
    unsigned int            m_frameSize;
    QTime time;

    uint8_t                 m_PlayID;       //播放ID
    bool                    m_PlayEn;       //播放使能
    bool                    m_bind;         //绑定标示

    uint8_t                 data[1024];
    AudioDataList           m_playList;
    QMutex                  m_playMutex;
};

#endif // AUDIOPLAYBACK_H
