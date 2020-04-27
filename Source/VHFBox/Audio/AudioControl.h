#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include <QThread>
#include <QMutex>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QMetaType>
#include <stdint.h>
//#include "bcg729/encoder.h"
//#include "bcg729/decoder.h"

//#include "opus/opus.h"
//#include "opus/opus_types.h"

class AudioRecoder;
class AudioPlayer;
class AudioMixer;

#define DATAMAXSIZE 1024

typedef struct AUDIODATA {
    uint8_t     data[DATAMAXSIZE];
    uint16_t     dataLen;
} AudioData;

Q_DECLARE_METATYPE(AudioData);

typedef QSharedPointer<AudioData>         pAudioData;
typedef QList<pAudioData>                 AudioDataList;
typedef QSharedPointer<AudioDataList>     pAudioDataList;
typedef QMap<uint8_t, pAudioDataList>     MixerMap;



class AudioControl
{

public:
    static AudioControl* getInstance();
    bool init(QString& status);

    void addMixerData(const uint8_t id, uint8_t *data, const unsigned int len);

    AudioPlayer *getPlayer(int ID);
    inline AudioPlayer *getPlayer1() const { return m_player1;}
    inline AudioPlayer *getPlayer2() const { return m_player2;}
    inline AudioPlayer *getPlayer3() const { return m_player3;}
    inline AudioPlayer *getPlayer4() const { return m_player4;}

    inline AudioMixer  *getMixer()  const { return m_mixer; }

private:
    AudioControl();
    ~AudioControl();

private:
    static AudioControl                     *m_instance;
    static QMutex                           m_getMutex;

    AudioRecoder                            *m_recoder;
    AudioPlayer                             *m_player1;
    AudioPlayer                             *m_player2;
    AudioPlayer                             *m_player3;
    AudioPlayer                             *m_player4;

    AudioMixer                              *m_mixer;
    MixerMap                                m_mixMap;
    QMutex                                  m_mixMutex;

};

#endif // AUDIOCONTROL_H
