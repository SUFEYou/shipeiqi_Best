#ifndef AUDIOPTT_H
#define AUDIOPTT_H

#include <QObject>
#include <QThread>

#pragma pack(1)
typedef struct _PTTSET
{
    int    	Priority;			//优先级:1-255 优先级越大越高
    int	    PttOn;              //Ptt(0:PTT-Off 1:PTT-On)
}PttSet;


class AudioPtt : public QThread
{
    Q_OBJECT
public:
    AudioPtt();
    bool init(QString& status);

    void setPriority_PttOn(int pID, int priority, int pttOn);
    int getPttOn(int pID);
    int getPriority(int pID);
protected:
    virtual void run();

private:
    bool      m_stop;

    PttSet    pttSetArr[4];
    int       pttOnTim;
    int       pttOnLim;

};

#endif // AUDIOPTT_H
