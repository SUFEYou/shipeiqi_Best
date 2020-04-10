#include "AudioPlayer.h"
#include <QDebug>
#include <QTime>



AudioPlayer::AudioPlayer()
              : m_stop(false)
              , m_handle(NULL)
              , m_params(NULL)
              , m_frameSize(80)
{
    m_PlayEn = false;
    m_bind   = false;

    time = QTime::currentTime();
    time.start();
}

bool AudioPlayer::init(QString& status)
{
    //打开设备
    int r = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if(r < 0)
    {
        status = QString("snd pcm open fail");
        return false;
    }
    //设置参数
    //初始化pcm属性
    snd_pcm_hw_params_alloca(&m_params);
    snd_pcm_hw_params_any(m_handle, m_params);
    //交错模式
    snd_pcm_hw_params_set_access(m_handle, m_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    //设置双声道，小端格式，16位
    snd_pcm_hw_params_set_format(m_handle, m_params, SND_PCM_FORMAT_S16_LE);
    //snd_pcm_hw_params_set_channels(m_handle, m_params, 2);
    snd_pcm_hw_params_set_channels(m_handle, m_params, 1);
    //snd_pcm_hw_params_set_format(m_handle, m_params, SND_PCM_FORMAT_S8);
    //snd_pcm_hw_params_set_channels(m_handle, m_params, 1);
    //设置采样率（44100标准MP3采样频率）
    //unsigned int val = 44100;
    unsigned int val = 8000;
    //snd_pcm_hw_params_set_rate_near(m_handle, m_params, &val, 0);
    snd_pcm_hw_params_set_rate(m_handle, m_params, val, 0);
    //设在采样周期,（最好是让系统自动设置，这一步可以省略）
    //snd_pcm_hw_params_set_period_size_near(m_handle, m_params, (snd_pcm_uframes_t*)&m_frameSize, 0);
    snd_pcm_hw_params_set_period_size(m_handle, m_params, m_frameSize, 0);

    int size = 0;
    snd_pcm_uframes_t periodsize;

    periodsize = 480;
    snd_pcm_hw_params_set_buffer_size_near(m_handle, m_params, &periodsize);

    periodsize /= 2;
    snd_pcm_hw_params_set_period_size_near(m_handle, m_params, &periodsize, 0);

    snd_pcm_hw_params_get_buffer_size(m_params, &size);
    qDebug() << "buffer size=" << size;

    //设置好的参数回写设备
    r = snd_pcm_hw_params(m_handle, m_params);
    if(r < 0)
    {
        status = QString("snd pcm open fail");
        return false;
    }
    //16--2--（一帧数据4个字节）
    //获取一个周期有多少帧数据，一个周期一个周期方式处理音频数据。
    //snd_pcm_hw_params_get_period_size(m_params, (snd_pcm_uframes_t*)&m_frameSize,0);
    //unsigned char *buffer = malloc(4*frames);//由于双通道，16bit，每个通道2个字节，一个周期所需要的空间为4个字节*帧数
    status = QString("snd pcm open succeed");
    return true;
}

void AudioPlayer::run()
{

    int rc = 0;
//    sleep(2);
//    qDebug() << "Wait 2 s";

    while (!m_stop)
    {

        getPlayData(data, rc);
        if (rc <= 0)
        {
//            qDebug() << "Do Not Have Enough Data To Play!";
            msleep(2);
            continue;

        }
        else if ((rc%m_frameSize) != 0)
        {
            qDebug() << "Play Frame Size Err!";
            continue;

        }

        rc = 0;
        rc = snd_pcm_writei(m_handle, data, m_frameSize);

        if (rc == -EPIPE)
        {
              /* EPIPE means underrun */
              qDebug() << "underrun occurred";
              snd_pcm_prepare(m_handle);

              msleep(200);

        } else if (rc < 0) {

            qDebug() << "error from writei: " << snd_strerror(rc);

        } else if (rc != (int)m_frameSize) {

            qDebug() << "short write, write " << rc << "frames";

        } else {

            msleep(5);

        }
//        qDebug() << QTime::currentTime().msec() << "audio play";

    }
    m_stop = false;
}


void AudioPlayer::addPlayData(const uint8_t *data, const unsigned int len)
{
    QMutexLocker locker(&m_playMutex);
    if (len > 0)
    {
        pAudioData tmp(new AudioData);
        memcpy(tmp->data, data, len);
        tmp->dataLen = len;

        m_playList.append(tmp);
    }
}


int AudioPlayer::getPlayData(uint8_t *data, int &len)
{
    QMutexLocker locker(&m_playMutex);
//c    qDebug() << "PlayDataList length = " << m_playList.length();
    if (!m_playList.empty())
    {

        if (m_playList.length() > 20)
        {
            int diff = m_playList.length() - 20;
            for(int i=0; i<diff; i++){
                m_playList.pop_front();
            }
        }

        pAudioData tmp;
        tmp = m_playList.front();
        m_playList.pop_front();
        len = tmp->dataLen;
        memcpy(data, tmp->data, len);

        return m_playList.length();
    }
    else
    {
        len = -1;
    }
}

uint8_t AudioPlayer::getPlayID() const
{
    return m_PlayID;
}


void AudioPlayer::setPlayID(const uint8_t PlayID)
{
    m_PlayID = PlayID;
}


bool AudioPlayer::isPlayEn() const
{
    return m_PlayEn;
}


void AudioPlayer::setPlayEn(bool PlayEn)
{
    m_PlayEn = PlayEn;
}


bool AudioPlayer::isBind() const
{
    return m_bind;
}


void AudioPlayer::setBind(bool bind)
{
    m_bind = bind;
}


void AudioPlayer::write(uint8_t* data)
{
    int rc = 0;
    rc = snd_pcm_writei(m_handle, data, m_frameSize);

    if (rc == -EPIPE)
    {
          /* EPIPE means underrun */
          qDebug() << "underrun occurred";
          snd_pcm_prepare(m_handle);
    }
    else if (rc < 0)
    {
        qDebug() << "error from writei: " << snd_strerror(rc);
    }
    else if (rc != (int)m_frameSize)
    {
        qDebug() << "short write, write " << rc << "frames";
    }
}
