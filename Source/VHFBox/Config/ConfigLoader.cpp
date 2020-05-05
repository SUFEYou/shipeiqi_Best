#include "ConfigLoader.h"
#include <QFile>

ConfigLoader* ConfigLoader::m_instance = NULL;
QMutex ConfigLoader::m_Mutex;

ConfigLoader::ConfigLoader()
{

}

ConfigLoader::~ConfigLoader()
{

    delete boxConf1;
    delete boxConf2;
    delete boxConf3;
    delete boxConf4;
}

ConfigLoader* ConfigLoader::getInstance()
{

    QMutexLocker locker(&m_Mutex);
    if (m_instance == NULL)
    {
        m_instance = new ConfigLoader();
    }

    return m_instance;
}

bool ConfigLoader::load()
{
    if(QFile::exists("Config.ini")){
        qDebug() << "---------Loadin Config.ini---------";
        loadConfig();

    } else {

        qDebug() << "---------Create Config.ini---------";
        createConfig();
        loadConfig();
    }

    qDebug() << "-----------------------------------";

    return true;
}

void ConfigLoader::loadConfig()
{


    QSettings settings("Config.ini", QSettings::IniFormat);

    ////////////////////////////////////////////////////////////////////////////////
    this->programType    = settings.value("Box-Static/A1-ProgramType").toInt();
    this->programID      = settings.value("Box-Static/A2-ProgramID").toInt();

    this->posRevPort     = settings.value("Position-Info/A1-posPort").toInt();
    this->posFromIP      = settings.value("Position-Info/A2-posFromIP").toString();
    this->posFromPort    = settings.value("Position-Info/A3-posFromPort").toInt();

    qDebug() << "ProgramInfo-ProgramType:" << programType;
    qDebug() << "ProgramInfo-ProgramID:"   << programID;
    qDebug() << "Position-posPort:"        << posRevPort;
    qDebug() << "Position-posFromIP:"      << posFromIP;
    qDebug() << "Position-posFromPort:"    << posFromPort;


    for(int i=1; i<5; i++){

        QString secPro = "Radio-";
        secPro.append(QString::number(i));

        QString enableStr;
        enableStr.append(secPro).append("/A1-boxEnable");

        QString boxIDStr;
        boxIDStr.append(secPro).append("/A2-boxID");

        QString boxNameStr;
        boxNameStr.append(secPro).append("/A3-boxName");

        QString boxPriorityStr;
        boxPriorityStr.append(secPro).append("/A4-boxPriority");

        QString boxCtrlPortStr;
        boxCtrlPortStr.append(secPro).append("/A5-boxCtrlPort");

        QString boxVoicPortStr;
        boxVoicPortStr.append(secPro).append("/A6-boxVoicPort");

        QString radioIDStr;
        radioIDStr.append(secPro).append("/B1-radioID");

        QString radioTypStr;
        radioTypStr.append(secPro).append("/B2-radioTyp");

        QString radioIPStr;
        radioIPStr.append(secPro).append("/B3-radioIP");

        QString radioCtrlPortStr;
        radioCtrlPortStr.append(secPro).append("/B4-radioCtrlPort");

        QString radioVoicPortStr;
        radioVoicPortStr.append(secPro).append("/B5-radioVoicPort");

        QString boxEnable = settings.value(enableStr).toString();
        int boxID         = settings.value(boxIDStr).toInt();
        QString boxName   = settings.value(boxNameStr).toString();
        int boxPriority   = settings.value(boxPriorityStr).toInt();
        int boxCtrlPort   = settings.value(boxCtrlPortStr).toInt();
        int boxVoicPort   = settings.value(boxVoicPortStr).toInt();
        int radioID       = settings.value(radioIDStr).toInt();
        int radioTyp      = settings.value(radioTypStr).toInt();
        QString radioIP   = settings.value(radioIPStr).toString();
        int radioCtrlPort = settings.value(radioCtrlPortStr).toInt();
        int radioVoicPort = settings.value(radioVoicPortStr).toInt();

        if(i == 1){
            boxConf1 = new BoxConf();
            if(boxEnable == "true"){
                boxConf1->setBoxEnable(true);
            } else {
                boxConf1->setBoxEnable(false);
            }

            boxConf1->setKey(i);
            boxConf1->setBoxID(boxID);
            boxConf1->setBoxName(boxName);
            boxConf1->setBoxPriority(boxPriority);
            boxConf1->setBoxCtrlPort(boxCtrlPort);
            boxConf1->setBoxVoicPort(boxVoicPort);
            boxConf1->setRadioID(radioID);
            boxConf1->setRadioTyp(radioTyp);
            boxConf1->setRadioIP(radioIP);
            boxConf1->setRadioCtrlPort(radioCtrlPort);
            boxConf1->setRadioVoicPort(radioVoicPort);
        }

        if(i == 2){
            boxConf2 = new BoxConf();
            if(boxEnable == "true"){
                boxConf2->setBoxEnable(true);
            } else {
                boxConf2->setBoxEnable(false);
            }

            boxConf2->setKey(i);
            boxConf2->setBoxID(boxID);
            boxConf2->setBoxName(boxName);
            boxConf2->setBoxPriority(boxPriority);
            boxConf2->setBoxCtrlPort(boxCtrlPort);
            boxConf2->setBoxVoicPort(boxVoicPort);
            boxConf2->setRadioID(radioID);
            boxConf2->setRadioTyp(radioTyp);
            boxConf2->setRadioIP(radioIP);
            boxConf2->setRadioCtrlPort(radioCtrlPort);
            boxConf2->setRadioVoicPort(radioVoicPort);
        }

        if(i == 3){
            boxConf3 = new BoxConf();
            if(boxEnable == "true"){
                boxConf3->setBoxEnable(true);
            } else {
                boxConf3->setBoxEnable(false);
            }

            boxConf3->setKey(i);
            boxConf3->setBoxID(boxID);
            boxConf3->setBoxName(boxName);
            boxConf3->setBoxPriority(boxPriority);
            boxConf3->setBoxCtrlPort(boxCtrlPort);
            boxConf3->setBoxVoicPort(boxVoicPort);
            boxConf3->setRadioID(radioID);
            boxConf3->setRadioTyp(radioTyp);
            boxConf3->setRadioIP(radioIP);
            boxConf3->setRadioCtrlPort(radioCtrlPort);
            boxConf3->setRadioVoicPort(radioVoicPort);
        }

        if(i == 4){
            boxConf4 = new BoxConf();
            if(boxEnable == "true"){
                boxConf4->setBoxEnable(true);
            } else {
                boxConf4->setBoxEnable(false);
            }

            boxConf4->setKey(i);
            boxConf4->setBoxID(boxID);
            boxConf4->setBoxName(boxName);
            boxConf4->setBoxPriority(boxPriority);
            boxConf4->setBoxCtrlPort(boxCtrlPort);
            boxConf4->setBoxVoicPort(boxVoicPort);
            boxConf4->setRadioID(radioID);
            boxConf4->setRadioTyp(radioTyp);
            boxConf4->setRadioIP(radioIP);
            boxConf4->setRadioCtrlPort(radioCtrlPort);
            boxConf4->setRadioVoicPort(radioVoicPort);
        }

    }
}





void ConfigLoader::createConfig()
{

    QSettings settings("Config.ini", QSettings::IniFormat);

    settings.beginGroup("Box-Static");
    settings.setValue("A1-ProgramType",2982);
    settings.setValue("A2-ProgramID",  2982);
    settings.endGroup();

    settings.beginGroup("Radio-1");
    settings.setValue("A1-boxEnable", "true");
    settings.setValue("A2-boxID",     1001);
    settings.setValue("A3-boxName",   "VHFBox1");
    settings.setValue("A4-boxPriority", 1);
    settings.setValue("A5-boxCtrlPort", 50101);
    settings.setValue("A6-boxVoicPort", 50201);

    settings.setValue("B1-radioID",  10001);
    settings.setValue("B2-radioTyp", 1714);
    settings.setValue("B3-radioIP",  "192.168.1.171");
    settings.setValue("B4-radioCtrlPort", 50018);
    settings.setValue("B5-radioVoicPort", 50017);
    settings.endGroup();

    settings.beginGroup("Radio-2");
    settings.setValue("A1-boxEnable", "true");
    settings.setValue("A2-boxID",     1002);
    settings.setValue("A3-boxName",   "VHFBox2");
    settings.setValue("A4-boxPriority", 1);
    settings.setValue("A5-boxCtrlPort", 50102);
    settings.setValue("A6-boxVoicPort", 50202);

    settings.setValue("B1-radioID",  10002);
    settings.setValue("B2-radioTyp", 1716);
    settings.setValue("B3-radioIP",  "192.168.1.190");
    settings.setValue("B4-radioCtrlPort", 50018);
    settings.setValue("B5-radioVoicPort", 50017);
    settings.endGroup();

    settings.beginGroup("Radio-3");
    settings.setValue("A1-boxEnable", "false");
    settings.setValue("A2-boxID",     1003);
    settings.setValue("A3-boxName",   "VHFBox3");
    settings.setValue("A4-boxPriority", 1);
    settings.setValue("A5-boxCtrlPort", 50103);
    settings.setValue("A6-boxVoicPort", 50203);

    settings.setValue("B1-radioID",  10003);
    settings.setValue("B2-radioTyp", 1814);
    settings.setValue("B3-radioIP",  "192.168.1.191");
    settings.setValue("B4-radioCtrlPort", 50018);
    settings.setValue("B5-radioVoicPort", 50017);
    settings.endGroup();

    settings.beginGroup("Radio-4");
    settings.setValue("A1-boxEnable", "false");
    settings.setValue("A2-boxID",     1004);
    settings.setValue("A3-boxName",   "VHFBox4");
    settings.setValue("A4-boxPriority", 1);
    settings.setValue("A5-boxCtrlPort", 50104);
    settings.setValue("A6-boxVoicPort", 50204);

    settings.setValue("B1-radioID",  10004);
    settings.setValue("B2-radioTyp", 1814);
    settings.setValue("B3-radioIP",  "192.168.1.192");
    settings.setValue("B4-radioCtrlPort", 50018);
    settings.setValue("B5-radioVoicPort", 50017);
    settings.endGroup();

    settings.beginGroup("Position-Info");
    settings.setValue("A1-posPort",   60000);
    settings.setValue("A2-posFromIP", "192.168.1.15");
    settings.setValue("A3-posFromPort",3030);
    settings.endGroup();

}


int ConfigLoader::getProgramID() const
{
    return programID;
}

int ConfigLoader::getProgramType() const
{
    return programType;
}

QString ConfigLoader::getPosFromIP() const
{
    return posFromIP;
}

int ConfigLoader::getPosFromPort() const
{
    return posFromPort;
}

int ConfigLoader::getPosRevPort() const
{
    return posRevPort;
}

