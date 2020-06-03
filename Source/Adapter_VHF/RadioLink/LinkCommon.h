#ifndef LINKCOMMON_H
#define LINKCOMMON_H

#include <QSharedPointer>
#include <QList>
#include <QTime>
#include <stdint.h>

//#define     PACK_LENGTHLIMIT 1020
//171电台发送报文长度大于1000字节时会产生阻塞，导致断链
#define     PACK_LENGTHLIMIT 900
#define     BROADCAST_ID	 0
#define		NETMSGPACK_LEN	 15000
#define		RADIORTCCMDLEN	 1500            //RADIORTCCMDLEN为指令长
#define		NETMSG_PACKLEN	 4096			// A break was detected on input.
#define		MESSAGE_VERSION  16


class ObjStage;
class ObjRecall;

typedef QSharedPointer<ObjStage> pObjStage;
typedef QSharedPointer<ObjRecall> pObjRecall;

/************************************************************************/
/*
    VHF Link Network 中网络传输部分的相关定义参量
    作者： Diyss
    时间：2012.12.23
    版本：Version 1.0.0.0
*/
/************************************************************************/

#define A01VLN_MSGMODEL_STATION_EX		0x0A01

typedef enum VLN_Messaage	{
    VLNMSG_ACC_STATE	= 1,			// 区域中心（ACC）状态
    VLNMSG_ACC_BASEINFO,				// 区域中心（ACC）基本信息
    VLNMSG_RSC_STATE,					// 基站控制台（RSC）状态
    VLNMSG_RSC_BASEINFO,				// 基站控制台（ACC）基本信息
    VLNMSG_MRT_STATE,					// 移动终端（MRT）状态
    VLNMSG_MRT_BASEINFO,				// 移动终端（MRT）基本信息
    VLNMSG_APT_STATE,					// 应用终端（APT）状态
    VLNMSG_APT_BASEINFO,				// 应用终端（APT）基本信息
    VLNMSG_MRT_POSITION,				// 移动台位置报文
    VLNMSG_MSGEX_TEXT,					// 二进制短报文信息
    VLNMSG_MSGEX_RECALLCODE,			// 二进制短报文回馈序号
    VLNMSG_RSC_UPDATEMRT,				// 基站控制台上报移动终端状态
    //------------------- 2012.12.24 V1.0.0.0  End Line ------------------------//
    VLNMSG_MSGEX_DELETED,				// 删除短报文信息
    VLNMSG_MSGEX_SETLINKMODE,           // 设置链路模式 对应设置项中，0:自动，链路自动切换主副台 1:强制切换链路为主台
}VLN_Messaage;

// Network Message Header
#pragma pack(1)
typedef struct _NET_MSG_HEADER
{
    uint16_t	ProgramType;		// Program Type
    uint16_t	ProgramID;			// Program ID
    uint8_t	    MessageVer;			// Message Version
    uint16_t	MessageModel;		// Message Model
    uint16_t	MessageType;		// Message Type
    uint32_t	MessageSerial;		// Message Serial
    uint16_t	MessageLen;			// Message Length
}NET_MSG_HEADER;	//15 bytes
#pragma pack()

// 区域中心（ACC）状态
#pragma pack(1)
typedef struct _NET_ACC_STATE
{
    uint32_t		ACCID;				// ACC ID
    uint8_t         State;				// ACC State
    uint8_t         Going;				// 运行状态
}NET_ACC_STATE;
#pragma pack()

// 区域中心（ACC）基本信息
#pragma pack(1)
typedef struct _NET_ACC_BASEINFO
{
    uint32_t			ACCID;				// ACC ID
    int8_t              Name[50];			// Name
    int8_t              Describe[200];		// 描述
}NET_ACC_BASEINFO;
#pragma pack()

// 基站控制台（RSC）状态
#pragma pack(1)
typedef struct _NET_RSC_STATE
{
    uint32_t			RSCID;				// RSC ID
    uint8_t             State;				// RSC State
    uint8_t             Going;				// 运行状态
}NET_RSC_STATE;
#pragma pack()

// 基站控制台（RSC）基本信息
#pragma pack(1)
typedef struct _NET_RSC_BASEINFO
{
    uint32_t			RSCID;				// RSC ID
    int8_t              Name[50];			// Name
    int8_t              Describe[200];		// 描述
}NET_RSC_BASEINFO;
#pragma pack()

// 移动终端（MRT）状态
#pragma pack(1)
typedef struct _NET_MRT_STATE
{
    uint32_t			MRTID;				// MRT ID
    uint32_t			inRSCID;			// 所在的RSC的区域
    uint8_t         	State;				// MRT State, 0:离线;1:在线;2:休眠;3.基站无响应
    uint32_t        	LastTime;			// 最后一次在线时间
    uint8_t         	Going;				// 运行状态
    uint8_t         	SignalLevel;		// 信号质量
}NET_MRT_STATE;
#pragma pack()

// 移动终端（MRT）基本信息
#pragma pack(1)
typedef struct _NET_MRT_BASEINFO
{
    uint32_t			MRTID;				// MRT ID
    int8_t  			Name[50];			// Name
    int8_t  			Describe[200];		// 描述
    uint32_t			BelongACC;			// 类属ACC编号
    uint8_t         	TerminalType;		// 设备终端类型
}NET_MRT_BASEINFO;
#pragma pack()

// 应用终端（APT）状态
#pragma pack(1)
typedef struct _NET_APT_STATE
{
    uint32_t			APTID;				// APT ID
    uint8_t         	State;				// APTState, 0:离线;1:在线;2:休眠
    uint32_t        	LastTime;			// 最后一次在线时间
    uint8_t         	Going;				// 运行状态
}NET_APT_STATE;
#pragma pack()

// 应用终端（APT）基本信息
#pragma pack(1)
typedef struct _NET_APT_BASEINFO
{
    uint32_t			APTID;				// APT ID
    int8_t  			Name[50];			// Name
    int8_t  			Describe[200];		// 描述
    uint8_t         	TerminalType;		// 终端类型
}NET_APT_BASEINFO;
#pragma pack()

// 移动台位置报文
#pragma pack(1)
typedef struct _NET_MRT_POSITION
{
    uint32_t			MRTID;				// MRT ID
    uint32_t        	LastTime;			// 更新时间
    int32_t				Latitude;			// 纬度，单位：分，数值*100000.0
    int32_t				Longitude;			// 经度，单位：分，数值*100000.0
    int32_t				Speed;				// 速度，单位：节，数值*100.0
    int32_t				Course;				// 航向，单位：分，数值*10.0
    int32_t				Heading;			// 首向，单位：分，数值*10.0
    int32_t				PosSys;				// 定位系统，0:GPS,1:北斗;2:网络;3:其他
}NET_MRT_POSITION;
#pragma pack()

// 二进制短报文信息
#pragma pack(1)
typedef struct _NET_MSGEX_TEXT
{
    uint32_t			RecvID;				// 接收方地址，接收方为0时为广播
    uint32_t			SendID;				// 发送方地址
    uint8_t         	Encrypt;			// 加密，0：非加密，1：加密；
    int32_t				Degree;				// 报文等级
    int32_t				Serial;				// 报文序号
    uint16_t        	TextLength;			// 文本长度
    uint8_t         	Text[1];			// 文本内容
}NET_MSGEX_TEXT;
#pragma pack()

// 基站控制台上报移动终端状态
#pragma pack(1)
typedef struct _NET_RSC_UPDATEMRT
{
    uint32_t			RSCID;				// RSC ID
    uint16_t        	Count;				// RMT个数
    NET_MRT_STATE       MRTState[1];		// RMT状态
}NET_RSC_UPDATEMRT;
#pragma pack()

// 报文序号结构体
#pragma pack(1)
typedef struct _NET_MSGEX_RECALLCODE
{
    uint32_t			SendID;				// 发送方地址
    int32_t				Serial;				// 消息序号
}NET_MSGEX_RECALLCODE;
#pragma pack()

#pragma pack(1)
typedef struct _NET_MSGEX_MSGDELETED
{
    uint32_t			SendID;				// 发送方地址
    //int               Serial;				//
    int32_t				SerialBegin;		// 要删除的一条数据的消息序号 开始号
    int32_t				SerialEnd;			// 要删除的一条数据的消息序号 结束号
}NET_MSGEX_MSGDELETED;
#pragma pack()

class ObjStage	// Stage Information
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjStage();
    void operator =(ObjStage& obj);
    // Clear
    void Clear();

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int		id;			// Stage ID
    int		state;		// Stage State, Online or Offline
    int8_t	signal;		// signal is or Not can be Received
    int		degree;		// Degree,  1 = Main/2 = Assistant
    int		sequence;	// Sequence
    int		errcount;	// Error Count,Master Stage Use it

protected:
private:
};

class ModelChain
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ModelChain();
    void operator =(ModelChain& chain);
    // Clear
    void Clear();
    // Get Member is or not in the List
    bool IsInList(int nid,int& nstate);

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int                 nChainId;			// Chain ID
    int                 nLimitOut;			// Out Time Limit
    int                 nLimitApply;		// Apply Moment's time limit
    int                 nLimitDrift;		// Drift Moment's time limit

    int                 nMemNum;			// Member's Number
    QList<pObjStage> nListMember;        // Member List on the Chain

protected:
private:
};

class ObjMsg
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjMsg();
    void operator =(ObjMsg& msg);
    // Clear
    void Clear();
    // Get Message Type
    int GetDataType();

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int		nReceive;		// Receive Id
    int		nSource;		// Source id
    bool	bEncrypt;		// Encrypt
    int		nVersion;		// Version
    int		nDataLen;		// Message Length
    char	pData[255];		// Message Data
    int		nDegree;		// Degree
    int		nSerial;		// Message Serial
    int		nTimeCount;		// Time Count
    int		nSendtimes;		// Send Times
protected:
private:
};

class ObjRecall
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Operate
    ObjRecall();
    void operator =(ObjRecall& msg);

    //////////////////////////////////////////////////////////////////////////
    // Parameter
    int nSource;		// Message Source
    int	nSerial;		// Message's Serial，VHF链路生成的报文序列号
    int	nState;			// 报文状态
protected:
private:
};



#endif // LINKCOMMON_H
