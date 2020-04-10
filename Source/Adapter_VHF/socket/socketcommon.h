#ifndef SOCKETCOMMON_H
#define SOCKETCOMMON_H
#include <stdint.h>

//#pragma pack(1)
//typedef struct _NET_MSG_HEADER
//{
//    unsigned short	ProgramType;		// Program Type
//    unsigned short	ProgramID;			// Program ID
//    unsigned char	MessageVer;			// Message Version
//    unsigned short	MessageModel;		// Message Model
//    unsigned short	MessageType;		// Message Type
//    unsigned long   MessageSerial;		// Message Serial
//    unsigned short	MessageLen;			// Message Length
//}NET_MSG_HEADER;


// HEADER MessageModel
#pragma pack(1)
typedef enum _MESSAGEMODEL
{
    MessageModel_VHF_Ctrl   = 2,		// VHF电台遥控
    MessageModel_VoicData   = 7,        // 语音数据
}MESSAGEMODEL;


// HEADER MessageType
#pragma pack(1)
typedef enum _MESSAGETYPE
{
    MessageTyp_VHF_Set_Signal      = 1,          //类型：signal intensity
    MessageTyp_VHF_Set_WorkMod     = 2,          //类型：设置工作模式
    MessageTyp_VHF_Set_Freq        = 3,          //类型：frequency
    MessageTyp_VHF_Ask_State       = 4,          //类型：状态问询
    MessageTyp_VHF_Set_Lock        = 5,
    MessageTyp_VHF_Set_Channel     = 6,          //类型：设置信道
    /////////////////////////////////////////////////////////////////////////
    MessageTyp_VHF_Ack_Signal      = 7,
    MessageTyp_VHF_Ack_WorkMod     = 8,         //类型：设置工作模式回复
    MessageTyp_VHF_Ack_Freq        = 9,
    MessageTyp_VHF_Ack_State       = 10,        //类型：状态参数回复
    MessageTyp_VHF_Lock_State      = 11,
    MessageTyp_VHF_Ack_Channel     = 12,        //类型：设置信道回复
    MessageTyp_VHF_CARRIER         = 13,        //类型：有载波
    MessageTyp_VHF_NOCARRIER       = 14,        //类型：无载波
    /////////////////////////////////////////////////////////////////////////
    MessageTyp_VHF_regist          = 15,		//类型：注册

}MESSAGETYPE;


#pragma pack(1)
typedef enum _VOICE_MESSAGE {               // Radio Voice Message Type
    Voice_Message_Data        = 1,          // Voice Data
    Voice_Message_DataQuery   = 2,          // Query Data
    Voice_Message_Apply       = 3,          // Apply Voice
    Voice_Message_PttSet      = 4,          // Set Radio PTT Station
    Voice_Message_PlyaState   = 5,          // Radio Play Voice State
    Voice_Message_SourceData  = 6,          // Voice Source Data
}VOICE_MESSAGETYPE;


#pragma pack(1)
typedef struct _NET_REGIST
{
    uint16_t	    RadioID;			// Radio ID
    unsigned char	NetIPAddr[16];		// 注册IP
    uint32_t	    NetPort;			// 注册Port
}NET_REGIST;


#pragma pack(1)
typedef struct _CTRL_REGIST_INFO
{
    uint16_t	    RadioID;			// Radio ID
    QString 	    NetIPAddr;		    // 注册IP
    uint32_t	    NetPort;			// 注册Port
    uint32_t        uptTime;            // 更新时间戳

    bool operator==(const _CTRL_REGIST_INFO &other) const {
        return (this->NetIPAddr == other.NetIPAddr && this->NetPort == other.NetPort);
    }

}CTRL_REGIST_VO;


#pragma pack(1)
typedef struct _VHF_ASK_STATE
{
    int	            RadioID;			// Radio ID
    unsigned char	funCod;             // 命令
    unsigned char	param1;             // 参数
}VHF_ASK_STATE;

#pragma pack(1)
typedef struct _VHF_SET_WORKMOD
{
    int	            RadioID;			// Radio ID
    unsigned char	funCod;             // 命令
    unsigned char	param1;             // 参数
}VHF_SET_WORKMOD;

#pragma pack(1)
typedef struct _VHF_SET_CHANNEL
{
    int	            RadioID;			// Radio ID
    unsigned char	funCod;             // 命令
    unsigned char	param1;             // 参数
}VHF_SET_CHANNEL;



#pragma pack(1)
typedef struct _VHF_ACK_STATE
{
    int	            RadioID;			// Radio ID
    unsigned char   funCod;             //
    unsigned char	sound;              // (为兼容以前)
    unsigned short	sendFreq;           //
    unsigned short	recvFreq;           //
    unsigned char	channel;            //
    unsigned char	workMod;            //
    unsigned char	signalV;            //
    unsigned char	radioPro;           //

}VHF_ACK_STATE;


#pragma pack(1)
typedef struct _VOICE_APPLY
{
    unsigned char	ApplyType;			// None use (为兼容以前)
    uint16_t        RadioID;			// Radio ID
    unsigned char	NetIPAddr[16];		// IP Address
    uint32_t        NetPort;			// Port
}VOICE_APPLY;


#pragma pack(1)
typedef struct _VOICE_REGIST_INFO
{
    QString         regKey;
    int	            DevID;			    // Regist Device ID
    QString 	    NetIPAddr;		    // 注册IP
    int	            NetPort;			// 注册Port
    int             PlayID;             // 播放ID
    uint32_t        uptTime;            // 更新时间戳
}VOICE_REGIST_VO;


#pragma pack(1)
typedef struct _VOICE_PTTSET
{
    uint16_t    	RadioID;			// Radio ID
    unsigned char	PTTClose;			// Radio PTT Close, 0:On; 1:Off;
}VOICE_PTTSET;


#pragma pack(1)
typedef struct _VOICE_DATA_HEAD
{
    uint16_t        RadioID;			// Radio ID
    unsigned char	Attribute;			// 属性：优先级/类型（G729a/Source/G711）
    unsigned char	FrameSN;			// Frame Serial Number
    uint32_t		SignalValue;        // Ptt复用(0:PTT-Off 1:PTT-On)

}VOICE_DATA_HEAD;


#endif // SOCKETCOMMON_H
