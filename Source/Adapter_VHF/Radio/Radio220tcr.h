#ifndef RADIO220TCR_H
#define RADIO220TCR_H

#include "Radio.h"

#define RADIO221DATASTORE	20480
#define RADIORTCCMDLENGTH	512
#define ARRAY_DATA_LEN 10240
#define HFSENDDATA_MAXLIMITLEN 100
#define HFSENDDATA_LIMITLEN 100

#define DITAL_TALKING 1  //����
#define MODE_TALKING 2   //ģ��
#define DITAL_DATA 3     //����
#define DITAL_DATA_221 7 //221 ����

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
    RADIOALE_SCANNING	= 1,	// ɨ����
    RADIOALE_INITIALIZE,		// ��ʼ��
    RADIOALE_SCANSTOP,			// ͣɨ״̬
    RADIOALE_LQA,				// LQA(��·��������)
    RADIOALE_CALL,				// ���Ӻ���
    RADIOALE_LINK,				// ����
    RADIOALE_TERLINK,			// ����
    RADIOALE_ERROR,				// Command Error
} SSBRADIO_ALESTATE;

typedef enum {		// Radio RCU State
    RADIORCU_STATE	= 1,		 // ��Ƶ����״̬
    RADIORCU_TURNNING,			 // ���ڵ�г
    RADIORCU_TURNSUCCESS,		 // ��г�ɹ�
    RADIORCU_TURNFAILURE,		 // ��гʧ��
    RADIORCU_LSMSG_SEND,		 // ��Ƶ���͵��ٱ�
    RADIORCU_LSMSG_SENDOVER,	 // ��Ƶ���͵��ٱ�����
    RADIORCU_LSMSG_RECEIVE,		 // ��Ƶ���յ��ٱ�
    RADIORCU_LSMSG_RECEIVEOVER,	 // ��Ƶ��ȷ���յ��ٱ�
    RADIORCU_LSMSG_RECEIVEFAULT, // ��Ƶ���յ��ٱ�ʧ��
    RADIORCU_DEFAULT,			 // �ָ���������
} SSBRADIO_RCUSTATE;

typedef enum {		// Radio AUTO State
    RADIOAUTO_SCANNING	= 1,	// �Զ�ɨ��״̬
    RADIOAUTO_RECVCALL,			// �յ�ɨ�����
    RADIOAUTO_CALLING,			// ����ɨ�����
    RADIOAUTO_TERLINK,			// �Զ��˳���·
    RADIOAUTO_LINK_S,			// �Զ�ҵ����,����
    RADIOAUTO_LINK_C,			// �Զ�ҵ����,����
    RADIOAUTO_TURNNING,			// �Զ����ڵ�г
    RADIOAUTO_TURNSUCCESS,		// �Զ���г�ɹ�
    RADIOAUTO_TURNFAILURE,		// �Զ���гʧ��
    RADIOAUTO_DET_LISTEN,		// �Զ��ȴ�̽��
    RADIOAUTO_DET_RECEIVE,		// �Զ��յ�̽��
    RADIOAUTO_DET_SEND,			// �Զ�����̽��
    RADIOAUTO_DET_RECALL,		// �Զ���Ӧ̽��
    RADIOAUTO_ACKSEND,			// �Զ�ȷ�Ϸ���
    RADIOAUTO_ACKRECEIVE,		// �Զ��յ�ȷ��
    RADIOAUTO_SHACKE,			// �Զ�ҵ������
    RADIOAUTO_ARQ_SENDBEGIN,	// ARQ��ʼ����
    RADIOAUTO_ARQ_SENDEND,		// ARQ���ͽ���
    RADIOAUTO_ARQ_RECEIVEEND,	// ARQ�������
    RADIOAUTO_ARQ_RECEIVEBEGIN,	// ARQ��ʼ����
    RADIOAUTO_ARQ_WAITRECALL,	// ARQ�ȴ�Ӧ��
    RADIOAUTO_DEFAULT,			// �ָ���������
    RADIOAUTO_ARQ_SENDDATA,     // �Զ�ARQ��������
    RADIOAUTO_LAST_MESSAGE		// ���ݵĽ�β
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

    RADIOCMD_SHAKE,			// �����ź�
    RADIOCMD_CHANSEE,		// �鿴�ŵ�������Ϣ
    RADIOCMD_RANK,			// �鿴�ŵ������Ϣ
    RADIOCMD_INDEX,			// �鿴��·����Ϣ
    RADIOCMD_LINE_SEE = 30,		// ----- See Line ------

    RADIOCMD_PGM_IND,		// ��ַ��Ϣ���
    RADIOCMD_PGM_CH,		// �ŵ���Ϣ���
    RADIOCMD_PGM_MDM,		// MODEM��Ϣ���
    RADIOCMD_LINE_PGM = 40,		// ----- Program Line ------
} SSBRADIO_CMDTYPE;

class Radio220tcr : public Radio
{
    Q_OBJECT
public:
    Radio220tcr();
    virtual ~Radio220tcr();
    virtual void serialInit();
    virtual int writeCtrlData(uint16_t funCode, char* data, int len);
    virtual int writeLinkData(char* data, int len);

private:
    void packageData();
    void parseData();
    void messageSeparate(const char* data, const int len);
    void decode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void writeData(char nDimID, char type, const char* data, const int len);
    void enCode(const char* srcData, const int srcLen, char* dstData, int& dstLen);
    void updateRadioState(uint16_t type, const char* data, const int len);
    char getCRC(const char* data, const quint16 len);

    void setModemNoticeSendEnd();
    void setModemNoticeCancel();
    void radioModemCanSendData(bool isSend);

private slots:
    void readCom();
    void onTimer();

private:
    QMutex                              m_dataMutex;
    QextSerialPort                      *dataCom;

    QList<QByteArray>                   m_recvDataList;
    QTimer                              *timer;
    long                                updTim;

    uint16_t                            m_nRadioState;                  // Radio State

    bool                                m_nModemSend;
    uint16_t                            m_nModemState;
    uint16_t                            m_SendingTimesCount;

    char*                               m_DycArrayData;
    uint16_t                            m_DycArrayLen;

    uint8_t                             m_RequestCount;
};

#endif // RADIO220TCR_H
