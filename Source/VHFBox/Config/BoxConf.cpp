#include "BoxConf.h"

BoxConf::BoxConf()
{

    key = -1;
}

BoxConf::~BoxConf()
{

}

int BoxConf::getKey() const
{
    return key;
}

void BoxConf::setKey(int value)
{
    key = value;
}

bool BoxConf::getBoxEnable() const
{
    return boxEnable;
}

void BoxConf::setBoxEnable(bool value)
{
    boxEnable = value;
}

int BoxConf::getBoxID() const
{
    return boxID;
}

void BoxConf::setBoxID(int value)
{
    boxID = value;
}

QString BoxConf::getBoxName() const
{
    return boxName;
}

void BoxConf::setBoxName(const QString &value)
{
    boxName = value;
}

int BoxConf::getBoxPriority() const
{
    return boxPriority;
}

void BoxConf::setBoxPriority(int value)
{
    boxPriority = value;
}

int BoxConf::getBoxCtrlPort() const
{
    return boxCtrlPort;
}

void BoxConf::setBoxCtrlPort(int value)
{
    boxCtrlPort = value;
}

int BoxConf::getBoxVoicPort() const
{
    return boxVoicPort;
}

void BoxConf::setBoxVoicPort(int value)
{
    boxVoicPort = value;
}

int BoxConf::getRadioID() const
{
    return radioID;
}

void BoxConf::setRadioID(int value)
{
    radioID = value;
}

int BoxConf::getRadioTyp() const
{
    return radioTyp;
}

void BoxConf::setRadioTyp(int value)
{
    radioTyp = value;
}

QString BoxConf::getRadioIP() const
{
    return radioIP;
}

void BoxConf::setRadioIP(const QString &value)
{
    radioIP = value;
}

int BoxConf::getRadioCtrlPort() const
{
    return radioCtrlPort;
}

void BoxConf::setRadioCtrlPort(int value)
{
    radioCtrlPort = value;
}

int BoxConf::getRadioVoicPort() const
{
    return radioVoicPort;
}

void BoxConf::setRadioVoicPort(int value)
{
    radioVoicPort = value;
}

