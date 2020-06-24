#include "UIManager.h"
#include "Config/ConfigLoader.h"
#include "Config/BoxConf.h"
#include "Util/RadioUtil.h"

UIManager* UIManager::m_instance = NULL;
QMutex UIManager::m_Mutex;

UIManager::UIManager()
{
    curWidgetID = -1;

    widge1 = NULL;
    widge2 = NULL;
    widge3 = NULL;
    widge4 = NULL;
    widgeMulti = NULL;
}

UIManager::~UIManager()
{

    delete widge1;
    delete widge2;
    delete widge3;
    delete widge4;
    delete widgeMulti;
}

UIManager* UIManager::getInstance()
{

    QMutexLocker locker(&m_Mutex);
    if (m_instance == NULL)
    {
        m_instance = new UIManager();
    }

    return m_instance;
}

bool UIManager::init()
{

    BoxConf *boxConf1 = ConfigLoader::getInstance()->getBoxConf1();
    BoxConf *boxConf2 = ConfigLoader::getInstance()->getBoxConf2();
    BoxConf *boxConf3 = ConfigLoader::getInstance()->getBoxConf3();
    BoxConf *boxConf4 = ConfigLoader::getInstance()->getBoxConf4();

    qDebug() << "//////////////////////////////////////////////////////////////";

    int widgetCount = 0;
    if(boxConf1->getBoxEnable()){

        int radioTyp = boxConf1->getRadioTyp();
        qDebug() <<"boxConf1" << "radioTyp---------------"       << radioTyp;

        if(radioTyp == 1814){
            widge1 = new WidgeVHF181d();
        }

        if(radioTyp == 7814){
            widge1 = new WidgeVHF781tcp();
        }

        if(radioTyp == 1714){
            widge1 = new WidgeVHF171d();
        }

        if(radioTyp == 1716){
            widge1 = new WidgeVHF171al();
        }

        if(radioTyp == 2124 || radioTyp == 2204 || radioTyp == 2304){
            widge1 = new WidgeSSB();
        }

        if (widge1 != NULL)
        {
            widgetCount = widgetCount + 1;
            widge1->setIndex(widgetCount);
            widge1->setRadioTypInt(radioTyp);
            widge1->setRadioTyp(RadioUtil::getRadioTypStr(radioTyp));
            widge1->setBoxID(QString::number(boxConf1->getBoxID()));
            widge1->init();
        }

        if(curWidgetID <0){
            curWidgetID = 1;
        }



    } else {

        widge1   = NULL;
    }

    if(boxConf2->getBoxEnable()){

        int radioTyp = boxConf2->getRadioTyp();
        qDebug() <<"boxConf2" << "radioTyp---------------"       << radioTyp;

        if(radioTyp == 1814){
            widge2 = new WidgeVHF181d();
        }

        if(radioTyp == 7814){
            widge2 = new WidgeVHF781tcp();
        }

        if(radioTyp == 1714){
            widge2 = new WidgeVHF171d();
        }

        if(radioTyp == 1716){
            widge2 = new WidgeVHF171al();
        }

        if(radioTyp == 2124 || radioTyp == 2204 || radioTyp == 2304){
            widge2 = new WidgeSSB();
        }

        if (widge2 != NULL)
        {
            widgetCount = widgetCount + 1;
            widge2->setIndex(widgetCount);
            widge2->setRadioTypInt(radioTyp);
            widge2->setRadioTyp(RadioUtil::getRadioTypStr(radioTyp));
            widge2->setBoxID(QString::number(boxConf2->getBoxID()));
            widge2->init();
        }

        if(curWidgetID <0){
            curWidgetID = 2;
        }

    } else {

        widge2   = NULL;
    }

    if(boxConf3->getBoxEnable()){

        int radioTyp = boxConf3->getRadioTyp();
        qDebug() <<"boxConf3" << "radioTyp---------------"       << radioTyp;

        if(radioTyp == 1814){
            widge3 = new WidgeVHF181d();
        }

        if(radioTyp == 7814){
            widge3 = new WidgeVHF781tcp();
        }

        if(radioTyp == 1714){
            widge3 = new WidgeVHF171d();
        }

        if(radioTyp == 1716){
            widge3 = new WidgeVHF171al();
        }

        if(radioTyp == 2124 || radioTyp == 2204 || radioTyp == 2304){
            widge3 = new WidgeSSB();
        }

        if (widge3 != NULL)
        {
            widgetCount = widgetCount + 1;
            widge3->setIndex(widgetCount);
            widge3->setRadioTypInt(radioTyp);
            widge3->setRadioTyp(RadioUtil::getRadioTypStr(radioTyp));
            widge3->setBoxID(QString::number(boxConf3->getBoxID()));
            widge3->init();
        }

        if(curWidgetID <0){
            curWidgetID = 3;
        }


    } else {

        widge3   = NULL;
    }

    if(boxConf4->getBoxEnable()){

        int radioTyp = boxConf4->getRadioTyp();
        qDebug() <<"boxConf4" << "radioTyp---------------"       << radioTyp;

        if(radioTyp == 1814){
            widge4 = new WidgeVHF181d();
        }

        if(radioTyp == 7814){
            widge4 = new WidgeVHF781tcp();
        }

        if(radioTyp == 1714){
            widge4 = new WidgeVHF171d();
        }

        if(radioTyp == 1716){
            widge4 = new WidgeVHF171al();
        }

        if(radioTyp == 2124 || radioTyp == 2204 || radioTyp == 2304){
            widge4 = new WidgeSSB();
        }

        if (widge4 != NULL)
        {
            widgetCount = widgetCount + 1;
            widge4->setIndex(widgetCount);
            widge4->setRadioTypInt(radioTyp);
            widge4->setRadioTyp(RadioUtil::getRadioTypStr(radioTyp));
            widge4->setBoxID(QString::number(boxConf4->getBoxID()));
            widge4->init();
        }

        if(curWidgetID <0){
            curWidgetID = 4;
        }


    } else {

        widge4   = NULL;
    }


    if(widgetCount > 1) {

        widgeMulti = new WidgeMulti();
        widgeMulti->setIndex(5);
        widgeMulti->init();

    } else {

        widgeMulti = NULL;
    }

    if (widge1 == NULL || widge2 == NULL)
        return false;
    else
        return true;
}


WidgeBase* UIManager::getWidge(int ID)
{
    if(ID == 1){
        return widge1;
    }
    if(ID == 2){
        return widge2;
    }
    if(ID == 3){
        return widge3;
    }
    if(ID == 4){
        return widge4;
    }
    if(ID == 5){
        return widgeMulti;
    }

    return NULL;
}


WidgeBase* UIManager::getCurrWidge()
{
    if(curWidgetID == 1){
        return widge1;
    }
    if(curWidgetID == 2){
        return widge2;
    }
    if(curWidgetID == 3){
        return widge3;
    }
    if(curWidgetID == 4){
        return widge4;
    }
    if(curWidgetID == 5){
        return widgeMulti;
    }

    return NULL;
}

void UIManager::changeCurrWidge()
{
    int changeToID = curWidgetID;

    if(curWidgetID == 1){

        if(widge2 != NULL){
            changeToID = 2;

        } else if(widge3 != NULL) {
            changeToID = 3;

        } else if(widge4 != NULL) {
            changeToID = 4;

        } else if(widgeMulti != NULL) {
            changeToID = 5;
        }
    }

    if(curWidgetID == 2){

        if(widge3 != NULL){
            changeToID = 3;

        } else if(widge4 != NULL) {
            changeToID = 4;

        } else if(widgeMulti != NULL) {
            changeToID = 5;

        } else if(widge1 != NULL) {
            changeToID = 1;
        }
    }

    if(curWidgetID == 3){

        if(widge4 != NULL){
            changeToID = 4;

        } else if(widgeMulti != NULL) {
            changeToID = 5;

        } else if(widge1 != NULL) {
            changeToID = 1;

        } else if(widge2 != NULL) {
            changeToID = 2;
        }
    }

    if(curWidgetID == 4){

        if(widgeMulti != NULL) {
            changeToID = 5;

        } else if(widge1 != NULL){
            changeToID = 1;

        } else if(widge2 != NULL) {
            changeToID = 2;

        } else if(widge3 != NULL) {
            changeToID = 3;
        }
    }

    if(curWidgetID == 5){

        if(widge1 != NULL){
            changeToID = 1;

        } else if(widge2 != NULL) {
            changeToID = 2;

        } else if(widge3 != NULL) {
            changeToID = 3;

        } else if(widge4 != NULL) {
            changeToID = 4;
        }
    }

    if(curWidgetID != changeToID){

        if (getCurrWidge() != NULL)
        {
            getCurrWidge()->hide();

            curWidgetID = changeToID;
            getCurrWidge()->show();
        }
    }
}


void UIManager::updateAllCurTim(QString &param)
{
    if(widge1 != NULL){
        widge1->uptCurrTim(param);
    }

    if(widge2 != NULL){
        widge2->uptCurrTim(param);
    }

    if(widge3 != NULL){
        widge3->uptCurrTim(param);
    }

    if(widge4 != NULL){
        widge4->uptCurrTim(param);
    }

    if(widgeMulti != NULL){
        widgeMulti->uptCurrTim(param);
    }
}


void UIManager::updateAllCurLon(QString &param)
{
    if(widge1 != NULL){
        widge1->uptCurrLon(param);
    }

    if(widge2 != NULL){
        widge2->uptCurrLon(param);
    }

    if(widge3 != NULL){
        widge3->uptCurrLon(param);
    }

    if(widge4 != NULL){
        widge4->uptCurrLon(param);
    }

    if(widgeMulti != NULL){
        widgeMulti->uptCurrLon(param);
    }
}


void UIManager::updateAllCurLat(QString &param)
{
    if(widge1 != NULL){
        widge1->uptCurrLat(param);
    }

    if(widge2 != NULL){
        widge2->uptCurrLat(param);
    }

    if(widge3 != NULL){
        widge3->uptCurrLat(param);
    }

    if(widge4 != NULL){
        widge4->uptCurrLat(param);
    }

    if(widgeMulti != NULL){
        widgeMulti->uptCurrLat(param);
    }
}


void UIManager::updateAllMute()
{
    if(widge1 != NULL){
        widge1->setMute();
    }

    if(widge2 != NULL){
        widge2->setMute();
    }

    if(widge3 != NULL){
        widge3->setMute();
    }

    if(widge4 != NULL){
        widge4->setMute();
    }

    if(widgeMulti != NULL){
        widgeMulti->setMute();
    }
}

void UIManager::updateAllVolume(int volumeLev)
{
    if(widge1 != NULL){
        widge1->setVolume(volumeLev);
    }

    if(widge2 != NULL){
        widge2->setVolume(volumeLev);
    }

    if(widge3 != NULL){
        widge3->setVolume(volumeLev);
    }

    if(widge4 != NULL){
        widge4->setVolume(volumeLev);
    }

    if(widgeMulti != NULL){
        widgeMulti->setVolume(volumeLev);
    }
}

void UIManager::updateAllBkLight(int bkLightLev)
{
    if(widge1 != NULL){
        widge1->setBkLight(bkLightLev);
    }

    if(widge2 != NULL){
        widge2->setBkLight(bkLightLev);
    }

    if(widge3 != NULL){
        widge3->setBkLight(bkLightLev);
    }

    if(widge4 != NULL){
        widge4->setBkLight(bkLightLev);
    }

    if(widgeMulti != NULL){
        widgeMulti->setBkLight(bkLightLev);
    }
}
