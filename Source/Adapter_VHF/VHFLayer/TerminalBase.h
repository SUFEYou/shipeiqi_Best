#ifndef TERMINALBASE_H
#define TERMINALBASE_H

#include <QString>

#define MEM_LENGTH  1024

class CTerminalBase
{
public:
    CTerminalBase();
    virtual ~CTerminalBase();

    enum		// Terminal Tag
    {
        TERM_GPS = 1,		// Terminal GPS
        TERM_ARPA,			// Terminal ARPA
        TERM_HEAD,			// Terminal Head Sensor
        TERM_TRIO,			// Terminal Triones
        TERM_SHIP,			// Terminal Ship Control Board
        TERM_VHFLAYER,		// VHF Layer
        TERM_VHFCONTROL,		// VHF Control
        TERM_SSBLAYER,			// SSB Layer
        TERM_SSBCONTROL,		// SSB Control
        TERM_EROM,				//EngineRoom Terminal
    };


    enum			// Terminal State
    {
        STATE_CLOSE = 1,	// Terminal Close
        STATE_RUN,			// Terminal Run
        STATE_ERROR,		// Error
        STATE_OUTTIME,		// Out Time
        STATE_PAUSE,		// Pause
    };

public:
//Operate
    //////////////////////////////////////////////////////////////////////////
    // Restore the Receive Bytes to Log Files
    //////////////////////////////////////////////////////////////////////////
    // Restore Receive Bytes
    void UseSaveReceiveBytes(char* pchar, const int nlength);
    // Save Bytes
    bool UseSaveBytes();
    // Set Save Path&File's Name
    void UseSetSavePathFile(QString spath,  QString sfile);
    // Open or Close the Bytes Save
    void UseSetSaveAvailable(bool bsave);
    // Get Save Available
    bool UseGetSaveAvailable();
    // Set Terminal Tag
    void UseSetTerminalTag(quint32 tag);
    // Get Terminal Tag
    quint32 UseGetTerminalTag();

    // Set Terminal State
    void UseSetTerminalState(quint32 state);
    // Get Terminal State
    quint32 UseGetTerminalState();

    //////////////////////////////////////////////////////////////////////////
    // Set the PortNum
    void UseSetPortNum(quint32 portn);
    // Get the PortNum
    quint32 UseGetPortNum();

public:
    char        m_CReadBuf[500];	//读缓冲,存储读取的数据
    int         m_CPos;				//当前报文长度
    int         m_CFlag;			//接收状态	//0:语句开始,判断"$"符号;
                                                //1:判断报文头;
                                                //2:判断报文结尾;
                                                //3:判断报文内容,并发送报文;
    int         m_CLen;				//报文总长度
    int         m_CType;			//报文类别
    quint32     m_CSucess;			//是否成功解析报文

    //////////////////////////////////////////////////////////////////////////
    quint32     m_nTag;				// Terminal Tag
    quint32     m_nPortNum;			// Port Number

    int         m_nRunState;		// Port Run State

private:
    // Restore Receive Bytes
    bool        m_rSave;			// Save Receive Bytes or not
    char*       m_rData;			// Memory to Restore Bytes
    int         m_rLength;			// Memory Bytes Length
    QString     m_rPath;			// Save File's Path
    QString     m_rFile;			// Save File's Name
};

#endif // TERMINALBASE_H
