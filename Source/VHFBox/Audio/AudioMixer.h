#ifndef SOUNDMIXER_H
#define SOUNDMIXER_H

#include <QObject>
#include "alsa/asoundlib.h"

class AudioMixer
{
public:
    AudioMixer();
    bool init(QString& status);
    void up();
    void down();
    void mute();
    void recover();
    void volumn(long val);

private:
    snd_mixer_t         *m_handle;
    snd_mixer_elem_t    *m_elem;
    long                m_volMin;
    long                m_volMax;
    long                m_val;

};

#endif // SOUNDMIXER_H
