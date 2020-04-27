#ifndef BOXCONF_H
#define BOXCONF_H

#include <QString>

class BoxConf
{
public:
    BoxConf();
    ~BoxConf();


    int getKey() const;
    void setKey(int value);

    bool getBoxEnable() const;
    void setBoxEnable(bool value);

    int getBoxID() const;
    void setBoxID(int value);

    QString getBoxName() const;
    void setBoxName(const QString &value);

    int getBoxPriority() const;
    void setBoxPriority(int value);

    int getBoxCtrlPort() const;
    void setBoxCtrlPort(int value);

    int getBoxVoicPort() const;
    void setBoxVoicPort(int value);

    int getRadioID() const;
    void setRadioID(int value);

    int getRadioTyp() const;
    void setRadioTyp(int value);

    QString getRadioIP() const;
    void setRadioIP(const QString &value);

    int getRadioCtrlPort() const;
    void setRadioCtrlPort(int value);

    int getRadioVoicPort() const;
    void setRadioVoicPort(int value);

private:
    int         key;
    bool        boxEnable;
    int         boxID;
    QString     boxName;
    int         boxPriority;
    int         boxCtrlPort;
    int         boxVoicPort;
    int         radioID;
    int         radioTyp;
    QString     radioIP;
    int         radioCtrlPort;
    int         radioVoicPort;

};

#endif // BOXCONF_H
