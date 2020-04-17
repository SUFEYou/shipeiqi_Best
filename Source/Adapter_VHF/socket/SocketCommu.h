#ifndef SOCKETCOMMON_H
#define SOCKETCOMMON_H
#include <stdint.h>

// Message Type
#pragma pack(1)
typedef enum _MSG_TYP
{
    MSG_TYP_CTRL   = 1,                 // 电台遥控
    MSG_TYP_VOIC   = 2,                 // 语音数据

}MSG_TYP;


// Function Code
#pragma pack(1)
typedef enum _FUNC_CODE
{
    Set_WorkTyp     = 1,             // 设置工作方式
    Set_WorkMod     = 2,             // 设置工作模式
    Set_Channel     = 3,             // 设置信道
    Set_TxFreq      = 4,             // 设置发射频点
    Set_RxFreq      = 5,             // 设置接受频点
    Set_Power       = 6,             // 设置发射功率
    Set_Squelch     = 7,             // 设置静噪

    Ask_State       = 20,            // 电台工作状态问询
    Ack_State       = 21,            // 电台工作状态反馈

    Dev_regist      = 30,            // 通道注册
    Dev_regist_Ack  = 31,            // 注册状态反馈
    Dev_restrict_Ack= 32,            // 权限受限反馈

    Voice_Data      = 41,            // 语音数据
    PTT_set         = 42,            // PTT设置
    PTT_state       = 43,            // PTT状态

}FUNC_CODE;


#pragma pack(1)
typedef struct _MSG_HEADER
{
    uint16_t        ProgramType;		// Program Type
    uint16_t        ProgramID;			// Program ID
    uint16_t        msgTyp;             //
    uint16_t	    DevID;              // 设备ID
    uint16_t	    RadioTyp;			// 电台类型
    uint8_t         funCod;             // 功能码
}MSG_HEADER;


#pragma pack(1)
typedef struct _RADIO_SET
{

    uint8_t         workTyp;            // 工作方式
    uint8_t         workMod;            // 工作模式
    uint8_t         channel;            // 信道
    uint64_t        txFreq;             // 发射频点
    uint64_t        rxFreq;             // 接受频点
    uint8_t         power;              // 发射功率
    uint8_t         squelch;            // 静噪
    uint64_t        ext1;               // 预留1
    uint64_t        ext2;               // 预留2

}RADIO_SET;


#pragma pack(1)
typedef struct _RADIO_STATE
{
    uint8_t         workTyp;            // 工作方式
    uint8_t         workMod;            // 工作模式
    uint8_t         channel;            // 信道
    uint64_t        txFreq;             // 发射频点
    uint64_t        rxFreq;             // 接受频点
    uint8_t         power;              // 发射功率
    uint8_t         squelch;            // 静噪
    uint8_t         errState;           // 0:电台通信正常 1：电台通信异常
    uint64_t        ext1;               // 预留1
    uint64_t        ext2;               // 预留2

}RADIO_STATE;


#pragma pack(1)
typedef struct _DEV_RESTRICT_STATE
{

    uint8_t         ctrlOut;            // 遥控(0：未受限 1：受限)
    uint8_t         ctrlIn;             // 状态(0：未受限 1：受限)
    uint8_t         voicOut;            // 喊话(0：未受限 1：受限)
    uint8_t         voicIn;             // 监听(0：未受限 1：受限)

}DEV_RESTRICT_ACK;


#pragma pack(1)
typedef struct _DEV_REGIST
{

    unsigned char	NetIPAddr[16];		// 注册IP
    uint32_t	    NetPort;			// 注册Port

}DEV_REGIST;


#pragma pack(1)
typedef struct _DEV_REGIST_ACK
{

    uint8_t         regState;           // 注册状态

}DEV_REGIST_ACK;


#pragma pack(1)
typedef enum _REGIST_STATE
{
    RegistOK         =0,        // 注册成功
    RegistNG_NoMore  =1,		// 注册失败：已达上限
    RegistNG_ErrRTyp =2,        // 注册失败：电台类型错误

}REGIST_STATE;


#pragma pack(1)
typedef struct _VOICE_HEAD
{
    uint8_t         Priority;           // 优先级
    uint8_t         codec;              // 编码方式（G729a/Source/G711）
    uint8_t         FrameSN;			// Frame Serial Number
    uint8_t         PttON;              // 0:PTT-Off 1:PTT-On

}VOICE_HEAD;


#pragma pack(1)
typedef struct _PTT_SET
{
    uint8_t         Priority;           // 优先级:1-255 优先级越大越高
    uint8_t         PttON;              // 0:PTT-Off 1:PTT-On

}PTT_SET;


#pragma pack(1)
typedef struct _PTT_STATE
{
    uint16_t	    DevID;              // 设备ID
    uint8_t         PttOccupy;          // 0:未抢占 1:抢占

}PTT_STATE;


//////////////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
typedef struct _CTRL_REGIST_INFO
{
    QString         regKey;
    uint16_t	    DevID;              // 注册的设备ID
    QString 	    NetIPAddr;		    // 注册IP
    uint32_t	    NetPort;			// 注册Port
    uint32_t        uptTime;            // 更新时间戳

    bool operator==(const _CTRL_REGIST_INFO &other) const {
        return (this->NetIPAddr == other.NetIPAddr && this->NetPort == other.NetPort);
    }

}CTRL_REGIST_VO;


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


//////////////////////////////////////////////////////////////////////////////////////////



//#pragma pack(1)
//typedef struct _MSG_HEADER
//{
//    unsigned short	ProgramType;		// Program Type
//    unsigned short	ProgramID;			// Program ID
//    unsigned char	MessageVer;         // Message Version

//    unsigned short	MessageTyp;         // Message Type
//    unsigned short  MessageSeq;         // Message Serial
//    unsigned short	MessageLen;			// Message Length
//}MSG_HEADER;


//// HEADER MessageModel
//#pragma pack(1)
//typedef enum _MSGEMODEL
//{
//    MSG_TYP_CTRL   = 2,            // 电台遥控
//    MSG_TYP_VOIC   = 7,            // 语音数据
//}MSGMOD;


//// HEADER MessageType
//#pragma pack(1)
//typedef enum _MSGTYPE
//{
//    MsgTyp_Set_WorkTyp     = 1,             // 设置工作方式
//    MsgTyp_Set_WorkMod     = 2,             // 设置工作模式
//    MsgTyp_Set_Channel     = 3,             // 设置信道
//    MsgTyp_Set_TxFreq      = 4,             // 设置发射频点
//    MsgTyp_Set_RxFreq      = 5,             // 设置接受频点
//    MsgTyp_Set_Power       = 6,             // 设置发射功率
//    MsgTyp_Set_Squelch     = 7,             // 设置静噪

//    MsgTyp_Ask_State       = 20,            // 状态问询
//    MsgTyp_Ack_State       = 21,            // 状态上报
//    MsgTyp_regist          = 40,            // 通道注册（控制/语音）
//    MsgTyp_Voic_Data       = 41,            // 语音数据

////    /////////////////////////////////////////////////////////////////////////
////    MessageTyp_VHF_Ack_Signal      = 7,
////    MessageTyp_VHF_Ack_WorkMod     = 8,         //类型：设置工作模式回复
////    MessageTyp_VHF_Ack_Freq        = 9,

////    MessageTyp_VHF_Lock_State      = 11,
////    MessageTyp_VHF_Ack_Channel     = 12,        //类型：设置信道回复
////    MessageTyp_VHF_CARRIER         = 13,        //类型：有载波
////    MessageTyp_VHF_NOCARRIER       = 14,        //类型：无载波
////    /////////////////////////////////////////////////////////////////////////
////    MsgTyp_regist          = 15,		//类型：注册

//}MSGTYPE;


//#pragma pack(1)
//typedef enum _VOICE_MESSAGE {               // Radio Voice Message Type
//    MsgTyp_Voic_Data        = 1,          // Voice Data
//    Voice_Message_DataQuery   = 2,          // Query Data
//    Voice_Message_Apply       = 3,          // Apply Voice
//    Voice_Message_PttSet      = 4,          // Set Radio PTT Station
//    Voice_Message_PlyaState   = 5,          // Radio Play Voice State
//    Voice_Message_SourceData  = 6,          // Voice Source Data
//}VOICE_MESSAGETYPE;


//#pragma pack(1)
//typedef struct _NET_REGIST
//{
//    uint16_t	    RegDevID;			// 注册设备ID
//    unsigned char	NetIPAddr[16];		// 注册IP
//    uint32_t	    NetPort;			// 注册Port
//}NET_REGIST;


//#pragma pack(1)
//typedef struct _VHF_ASK_STATE
//{
//    int	            RadioID;			// Radio ID
//    unsigned char	funCod;             // 命令
//    unsigned char	param1;             // 参数
//}VHF_ASK_STATE;

//#pragma pack(1)
//typedef struct _VHF_SET_WORKMOD
//{
//    int	            RadioID;			// Radio ID
//    unsigned char	funCod;             // 命令
//    unsigned char	param1;             // 参数
//}VHF_SET_WORKMOD;

//#pragma pack(1)
//typedef struct _VHF_SET_CHANNEL
//{
//    int	            RadioID;			// Radio ID
//    unsigned char	funCod;             // 命令
//    unsigned char	param1;             // 参数
//}VHF_SET_CHANNEL;



//#pragma pack(1)
//typedef struct _VHF_ACK_STATE
//{
//    int	            RadioID;			// Radio ID
//    unsigned char   funCod;             //
//    unsigned char	sound;              // (为兼容以前)
//    unsigned short	sendFreq;           //
//    unsigned short	recvFreq;           //
//    unsigned char	channel;            //
//    unsigned char	workMod;            //
//    unsigned char	signalV;            //
//    unsigned char	radioPro;           //

//}VHF_ACK_STATE;


//#pragma pack(1)
//typedef struct _VOICE_APPLY
//{
//    unsigned char	ApplyType;			// None use (为兼容以前)
//    uint16_t        RadioID;			// Radio ID
//    unsigned char	NetIPAddr[16];		// IP Address
//    uint32_t        NetPort;			// Port
//}VOICE_APPLY;





//#pragma pack(1)
//typedef struct _VOICE_PTTSET
//{
//    uint16_t    	RadioID;			// Radio ID
//    unsigned char	PTTClose;			// Radio PTT Close, 0:On; 1:Off;
//}VOICE_PTTSET;


//#pragma pack(1)
//typedef struct _VOICE_DATA_HEAD
//{
//    uint16_t        RadioID;			// Radio ID
//    unsigned char	Attribute;			// 属性：优先级/类型（G729a/Source/G711）
//    unsigned char	FrameSN;			// Frame Serial Number
//    uint32_t		SignalValue;        // Ptt复用(0:PTT-Off 1:PTT-On)

//}VOICE_DATA_HEAD;


#endif // SOCKETCOMMON_H
