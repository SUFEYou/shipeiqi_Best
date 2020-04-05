#ifndef RADIOLINK_H
#define RADIOLINK_H

#include <QByteArray>
#include <QList>

class ObjMsg;

class RadioLink
{
public:
    RadioLink();
    virtual ~RadioLink();

    enum{					// Layer State Information
        LAYSTA_ONLINE = 1,		// Stage is on Line
        LAYSTA_OFFLINE,			// Stage is Off Line
        LAYSTA_SUCCESS,			// Receive the Stage's Data Success
        LAYSTA_LOST,			// Lost the Stage's Data
    };

    enum{						// Layer Moment Information
        MOMENT_LISTEN = 1,			// Listen Moment, Not in the Form
        MOMENT_BEGIN,				// Begin Moment
        MOMENT_CIRCLE,				// Circle Moment
        MOMENT_APPLY,				// Apply Moment
        MOMENT_DRIFT,				// Drift Moment
    };

    enum{						// Message Type Of the Layer
        LAYMSG_CONTROL = 1,			// Message to Control the Layer's Information
        LAYMSG_STATE,				// Layer State Control Message
        LAYMSG_MSGONCE,				// Layer's transmit Information
        LAYMSG_MSGCAST,				// Broadcast Information
        LAYMSG_MSGCALL,				// Message's Recall
    };

    enum						// Apply State
    {
        LAYAPP_ONLINE = 1,			// Apply to Join the Chain
        LAYAPP_OFFLINE,				// Apply to Leave the Chain
        LAYAPP_ONCE,				// Once Send Message
        LAYAPP_STILL,				// Call that online
    };

public:
// Operate
    //////////////////////////////////////////////////////////////////////////
    // Set the class is or not Available
    void SetAvailable(bool available);
    bool GetAvailable();

    void recvData(const char* pchar,const int nlength);

protected:
    virtual bool recvDataAnalyze(ObjMsg &msg);

private:
    void recvDataSubpackage();
    void recvDataParse();

    // Encrypt the Message
    // 创建密钥，生成加密报文
    void ActEncrypt_KeyCreate(char* pchar,const int nlength);
    // 解密数据
    bool ActEncrypt_KeyOpen(char* pchar,const int nlength);
    // 生成加密码
    void ActEncrypt_CodeCreate(char A,char B,char C);
    // 去处数据中指定的字符fix
    void ActEncrypt_CharClear(char* pchar,const int nlength,char& nEmp,const char fix);
    // 恢复数据中的指定字符fix
    void ActEncrypt_CharAdd(char* pchar,const int nlength,const char nEmp);

protected:
    //////////////////////////////////////////////////////////////////////////
    // Base Layer Information
    int                                 m_nCodeMe;		// My Stage Code
    int                                 m_nCodeVer;		// Version Code
    bool                                m_bAvailable;	// Available

    // Save the Encrypt Code
    char                                EncryptKey[100];		// Encrypt Code

private:
    QByteArray                          m_recvDataArray;
    QList<QByteArray>                   m_recvDataList;

};

#endif // RADIOLINK_H
