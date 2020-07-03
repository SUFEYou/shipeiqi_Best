#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QMutex>
#include "UI/WidgeBase.h"
#include "UI/WidgeVHF181d.h"
#include "UI/WidgeVHF781tcp.h"
#include "UI/WidgeVHF171d.h"
#include "UI/WidgeVHF171al.h"
#include "UI/WidgeSSB212.h"
#include "UI/WidgeSSB220.h"
#include "UI/WidgeSSB230.h"
#include "UI/WidgeMulti.h"

class UIManager
{
public:
    static UIManager* getInstance();
    bool init();


    WidgeBase *getWidge(int ID);
    WidgeBase *getCurrWidge();
    void   changeCurrWidge();

    inline WidgeBase *getWidge1() const { return widge1; }
    inline WidgeBase *getWidge2() const { return widge2; }
    inline WidgeBase *getWidge3() const { return widge3; }
    inline WidgeBase *getWidge4() const { return widge4; }
    inline WidgeBase *getWidge5() const { return widgeMulti; }

    void updateAllMute();
    void updateAllVolume(int volumeLev);
    void updateAllBkLight(int bkLightLev);
    void updateAllCurTim(QString &param);
    void updateAllCurLon(QString &param);
    void updateAllCurLat(QString &param);

private:
    UIManager();
    ~UIManager();


private:
    static UIManager      *m_instance;
    static QMutex          m_Mutex;

    int                   curWidgetID;
    WidgeBase             *widge1;
    WidgeBase             *widge2;
    WidgeBase             *widge3;
    WidgeBase             *widge4;

    WidgeBase             *widgeMulti;
};

#endif // UIMANAGER_H
