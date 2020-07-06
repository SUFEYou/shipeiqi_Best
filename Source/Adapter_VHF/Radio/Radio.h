#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <stdint.h>
#include "RadioManage.h"
#include "config/ConfigLoader.h"
#include "Uart/qextserialport.h"
#include "socket/SocketCommu.h"

#define MAXDATALENGTH 4096

#define RADIO221DATASTORE	20480
#define RADIORTCCMDLENGTH	512
#define ARRAY_DATA_LEN 10240
#define HFSENDDATA_MAXLIMITLEN 100
#define HFSENDDATA_LIMITLEN 100

#define DITAL_TALKING 1  //数话
#define MODE_TALKING 2   //模话
#define DITAL_DATA 3     //数据
#define DITAL_DATA_221 7 //221 数据

typedef enum {		// Radio Work Mode
    RADIOMODE_RCU = 1,			// RCU Model
    RADIOMODE_ALE,				// ALE Model
    RADIOMODE_HOP,				// HOP Model
    RADIOMODE_MDM,				// MODEM Speed
    RADIOMODE_CLOSE,			// Radio Close, No Recall Information
    RADIOMODE_ERROR,			// CMD Error
    RADIOMODE_CONNECT,			// Radio is Connected
    RADIOMODE_AUTO,				// TCR221 Auto Mode
} SSBRADIO_WORKMODE;

typedef enum {		// Radio ALE State
    RADIOALE_SCANNING	= 1,	// 扫描中
    RADIOALE_INITIALIZE,		// 初始化
    RADIOALE_SCANSTOP,			// 停扫状态
    RADIOALE_LQA,				// LQA(链路质量分析)
    RADIOALE_CALL,				// 连接呼叫
    RADIOALE_LINK,				// 建链
    RADIOALE_TERLINK,			// 断链
    RADIOALE_ERROR,				// Command Error
} SSBRADIO_ALESTATE;

typedef enum {		// Radio RCU State
    RADIORCU_STATE	= 1,		 // 定频操作状态
    RADIORCU_TURNNING,			 // 正在调谐
    RADIORCU_TURNSUCCESS,		 // 调谐成功
    RADIORCU_TURNFAILURE,		 // 调谐失败
    RADIORCU_LSMSG_SEND,		 // 定频发送低速报
    RADIORCU_LSMSG_SENDOVER,	 // 定频发送低速报结束
    RADIORCU_LSMSG_RECEIVE,		 // 定频接收低速报
    RADIORCU_LSMSG_RECEIVEOVER,	 // 定频正确接收低速报
    RADIORCU_LSMSG_RECEIVEFAULT, // 定频接收低速报失败
    RADIORCU_DEFAULT,			 // 恢复出厂设置
} SSBRADIO_RCUSTATE;

typedef enum {		// Radio AUTO State
    RADIOAUTO_SCANNING	= 1,	// 自动扫描状态
    RADIOAUTO_RECVCALL,			// 收到扫描呼叫
    RADIOAUTO_CALLING,			// 发送扫描呼叫
    RADIOAUTO_TERLINK,			// 自动退出链路
    RADIOAUTO_LINK_S,			// 自动业务建立,主叫
    RADIOAUTO_LINK_C,			// 自动业务建立,被叫
    RADIOAUTO_TURNNING,			// 自动正在调谐
    RADIOAUTO_TURNSUCCESS,		// 自动调谐成功
    RADIOAUTO_TURNFAILURE,		// 自动调谐失败
    RADIOAUTO_DET_LISTEN,		// 自动等待探测
    RADIOAUTO_DET_RECEIVE,		// 自动收到探测
    RADIOAUTO_DET_SEND,			// 自动发送探测
    RADIOAUTO_DET_RECALL,		// 自动响应探测
    RADIOAUTO_ACKSEND,			// 自动确认发送
    RADIOAUTO_ACKRECEIVE,		// 自动收到确认
    RADIOAUTO_SHACKE,			// 自动业务握手
    RADIOAUTO_ARQ_SENDBEGIN,	// ARQ开始发送
    RADIOAUTO_ARQ_SENDEND,		// ARQ发送结束
    RADIOAUTO_ARQ_RECEIVEEND,	// ARQ接收完成
    RADIOAUTO_ARQ_RECEIVEBEGIN,	// ARQ开始接收
    RADIOAUTO_ARQ_WAITRECALL,	// ARQ等待应答
    RADIOAUTO_DEFAULT,			// 恢复出厂设置
    RADIOAUTO_ARQ_SENDDATA,     // 自动ARQ发送数据
    RADIOAUTO_LAST_MESSAGE		// 数据的结尾
} SSBRADIO_AUTOSTATE;

typedef enum {
    RADIOMODEMSTATE_RECEIVING	= 1,	// Begin Receive Data
    RADIOMODEMSTATE_RECEIVEEND,			// Receive Data END
    RADIOMODEMSTATE_SENDING,			// Begin Sending Data
    RADIOMODEMSTATE_SENDEND,			// Send Data END
    RADIOMODEMSTATE_BUFFULL,			// Send Data Buffer Full
    RADIOMODEMSTATE_BUFEMPTY,			// Send Data Buffer Empty
    RADIOMODEMSTATE_CANCELSEND,			// Cancel send data
} SSBRADIO_MODEMSTATE;

typedef enum {			// Radio Model
    RADIOCTLPORT_COMBO = 1,			// Command & Data Transfer together
    RADIOCTLPORT_SINGLE,			// One Command, One Data Transfer
} SSBRADIO_MODEL;

typedef enum {              //Radio Group: 1-9&A
    GROUP_1	= 1,
    GROUP_2,
    GROUP_3,
    GROUP_4,
    GROUP_5,
    GROUP_6,
    GROUP_7,
    GROUP_8,
    GROUP_9,
    GROUP_A,
} SSBRADIO_GROUP;


typedef enum {		// Command Type
    RADIOCMD_ALE = 1,		// Set ALE Work Mode
    RADIOCMD_RCU,			// Set RCU Work Mode
    RADIOCMD_HOP,			// Set HOP Work Mode
    RADIOCMD_MDM,			// Set Modem Work Mode
    RADIOCMD_CALL,			// Call Radio
    RADIOCMD_LQA,			// Set LQA Apply
    RADIOCMD_CHANCHANGE,	// Change Channel
    RADIOCMD_NUMSCAN,		// Set Scan Number
    RADIOCMD_SCAN,			// Set Radio to Scanning
    RADIOCMD_STOP,			// Stop Radio Scanning
    RADIOCMD_SHOW,			// Check Current Radio State
    RADIOCMD_MDMENTER,		// Enter MDM Setting State
    RADIOCMD_MDMEXIT,		// Exit MDM Setting State
    RADIOCMD_DTCENTER,		// Enter Data Transfer State
    RADIOCMD_DTCEXIT,		// Exit Data Transfer State
    RADIOCMD_AUTO,			// TCR221 Auto Mode
    RADIOCMD_LINE_CTL = 20,		// ----- Control Line ------

    RADIOCMD_SHAKE,			// 握手信号
    RADIOCMD_CHANSEE,		// 查看信道基本信息
    RADIOCMD_RANK,			// 查看信道打分信息
    RADIOCMD_INDEX,			// 查看链路网信息
    RADIOCMD_LINE_SEE = 30,		// ----- See Line ------

    RADIOCMD_PGM_IND,		// 网址信息编程
    RADIOCMD_PGM_CH,		// 信道信息编程
    RADIOCMD_PGM_MDM,		// MODEM信息编程
    RADIOCMD_LINE_PGM = 40,		// ----- Program Line ------
} SSBRADIO_CMDTYPE;

class Radio : public QObject
{
    Q_OBJECT
public:
    Radio();
    virtual ~Radio();

    virtual void serialInit() = 0;
    virtual int writeCtrlData(uint16_t funCode, char* data, int len) = 0;
    virtual int writeLinkData(char* data, int len) = 0;

    void bcd2uint8(uint8_t src, uint8_t* dst);
    void uint82bcd(uint8_t src, uint8_t* dst);

    inline RADIO_STATE getRadioState() const { return radioState; }


protected:

    RADIO_STATE             radioState;
    QByteArray              dataArray;

private:

};

#endif // RADIO_H
