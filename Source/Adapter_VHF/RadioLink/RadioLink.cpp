#include "RadioLink.h"
#include "LinkCommon.h"
#include <QDebug>

#define MAXDATALENGTH 300

RadioLink::RadioLink()
{

}

RadioLink::~RadioLink()
{

}

//////////////////////////////////////////////////////////////////////////
// Set the class is or not Available
void RadioLink::SetAvailable(bool available)
{
    m_bAvailable = available;
}

bool RadioLink::GetAvailable()
{
    return m_bAvailable;
}

void RadioLink::recvData(const char* pchar,const int nlength)
{
    m_recvDataArray.append(pchar, nlength);
    recvDataSubpackage();
    recvDataParse();
}

void RadioLink::recvDataSubpackage()
{
    if (m_recvDataArray.length() < 10)//10 == 包头(1字节)+替换(1字节)+校验(1字节)+包长(1字节)+版本加密(1字节)+接收ID(2字节)+源ID(2字节)+包尾(1字节)
        return;
    else//数据分包
    {
        int  step = 0;
        int  contBegin = 0;
        int  contLen = 0;
        //删除数据长度
        int  removeLen = 0;
        for (int i = 0; i < m_recvDataArray.length(); ++i)
        {
            if (step == 0)//定位包头
            {
                if ((unsigned char)m_recvDataArray[i] == 0xAA)
                {
                    step = 1;
                    contBegin = i+1;
                    contLen = 0;
                }
            }
            else if (step == 1)//定位包尾
            {
                if ((unsigned char)m_recvDataArray[i] == 0xAA)
                {
                    if (contLen == 0)//包头丢失，包尾与下一包包头粘连情况（XX XX AA AA XX XX XX XX AA）
                    {
                        contBegin = i+1;
                        contLen = 0;
                        continue;
                    }
                    else if (contLen < 8)//包内数据长度小于8字节，数据长度错误
                    {
                        step = 0;
                        continue;
                    }
                    else
                    {
                        //将头和尾的0XAA都加入队列，解析时会用到
                        m_recvDataList.push_back(m_recvDataArray.mid(contBegin-1, contLen+2));
                        //更新已解码成功的数据长度
                        removeLen = contBegin + contLen + 1;
                        step = 0;
                        continue;
                    }
                }
                else
                {
                    ++contLen;
                }
            }

        }
        m_recvDataArray.remove(0, removeLen);
    }
}

void RadioLink::recvDataParse()
{
    ObjMsg	recvMsg;
    while (!m_recvDataList.isEmpty())
    {
        recvMsg.Clear();
        int len = 0;

        QByteArray tmpArray = m_recvDataList.first();
        m_recvDataList.pop_front();

        char tmp[MAXDATALENGTH];
        memset(tmp, 0, MAXDATALENGTH);
        memcpy(tmp, tmpArray.data(), tmpArray.length());

        // Clear the 0xAA Empty Num
        ActEncrypt_CharAdd(tmp+2, tmpArray.length()-3, tmp[1]);
        // Check Number
        char a = tmp[3];
        for (int i = 4; i < tmpArray.length(); ++i)
        {
            a ^= tmp[i];
        }

        if (tmp[2] != a)
        {
            qDebug() << "In RadioLink::recvDataParse() CRC Err";
            continue;
        }
        // Message Length
        if (((unsigned char)tmp[3]) != tmpArray.length())
        {
            qDebug() << "In RadioLink::recvDataParse() Length Err";
            continue;
        }
        // Version & Encrypt
        recvMsg.nVersion = tmp[4] >> 4;
        recvMsg.bEncrypt = tmp[4] & 0x0F;

        if (recvMsg.bEncrypt)
        {
            len = 7;
            recvMsg.nDataLen = tmpArray.length()-12;
        }
        else
        {
            len = 5;
            recvMsg.nDataLen = tmpArray.length()-10;
        }

        // Decrypt
        if (recvMsg.bEncrypt)
        {
            ActEncrypt_KeyOpen(tmp+4, tmpArray.length()-6);
        }

        // Receive ID
        recvMsg.nReceive = (((unsigned char)tmp[len])<<8) | ((unsigned char)tmp[len+1]);
        len += 2;

        // Source ID
        recvMsg.nSource	= (((unsigned char)tmp[len])<<8) | ((unsigned char)tmp[len+1]);
        len += 2;

        // Data
        memcpy(recvMsg.pData, tmp+len, tmpArray.length()-1-len);
    }
}

bool RadioLink::recvDataAnalyze(ObjMsg &msg)
{
    Q_UNUSED(msg);
    return false;
}






//////////////////////////////////////////////////////////////////////////
// Encrypt the Message
// 创建密钥，生成加密报文
void RadioLink::ActEncrypt_KeyCreate(char* pchar,const int nlength)
{
    //生成密钥
    char t1,t2,t3,t4;
    t1=t2=t3=t4=0;

    t1=rand()%16;
    t2=rand()%16;
    t3=rand()%16;
    t4=t2^0x01;

    char j1=0,j2=0;
    j1=t1*16+t2;
    j2=t3*16+t4;
    j1=j1^'B';
    j2=j2^'S';
    //报头加密
    pchar[1]=j1;
    pchar[2]=j2;
    ////////////生成加密字符串/////////////
    char A1,A2,A3;
    A1=A2=A3=0;
    A1=(t1*16+t2)^'S';
    A2=(t3*16+t4)^'U';
    A3=(t3*16+t2)^'N';
    ActEncrypt_CodeCreate(A1,A2,A3);//得到加密码
    //加密报文
    for(int i=3;i<nlength;i++)
    {
        if(i<103)
            pchar[i]^=EncryptKey[i-3];
        else if(i>102&&i<203)
            pchar[i]^=EncryptKey[i-103];
        else if(i>202)
            pchar[i]^=EncryptKey[i-203];
    }
}

//解密数据
bool RadioLink::ActEncrypt_KeyOpen(char* pchar,const int nlength)
{
    char t1,t2,t3,t4;
    t1=t2=t3=t4=0;

    int j1,j2;
    j1=j2=0;
    j1=pchar[1]^'B';
    j2=pchar[2]^'S';

    t1=int(j1/16);
    t2=j1-t1*16;
    t3=int(j2/16);
    t4=j2-t3*16;


    if(t4 != (t2^0x01))
        return false;
    ////////////生成加密字符串/////////////
    char A1,A2,A3;
    A1=A2=A3=0;
    A1=(t1*16+t2)^'S';
    A2=(t3*16+t4)^'U';
    A3=(t3*16+t2)^'N';
    ActEncrypt_CodeCreate(A1,A2,A3);//得到加密码
    //解密数据
    for(int i=3;i<nlength;i++)
    {
        if(i<103)
            pchar[i]^=EncryptKey[i-3];
        else if(i>102&&i<203)
            pchar[i]^=EncryptKey[i-103];
        else if(i>202)
            pchar[i]^=EncryptKey[i-203];
    }
    return true;
}

//生成加密码
void RadioLink::ActEncrypt_CodeCreate(char A,char B,char C)
{
    memset(EncryptKey,0,100);
    bool b1,b2,b7,Bins,A8,B8,C8;
    b1=b2=b7=Bins=A8=B8=C8=0;
    int j=0;
    quint8 tmp=0;
    while(1)
    {
        EncryptKey[j]=A;
        if(j>98)
            break;
        EncryptKey[j+1]=B;
        EncryptKey[j+2]=C;
        j+=3;
        for(int i=0;i<24;i++)
        {
            tmp=A&128;
            if(tmp==128) b1=1;
            else b1=0;
            tmp=A&64;
            if(tmp==64)	b2=1;
            else b2=0;
            tmp=A&2;
            if(tmp==2) b7=1;
            else b7=0;
        /////取Bins值/////////
            C8=C&1;
            Bins=C8^b7;
            Bins^=b2;
            Bins^=b1;
        ///////移位A/B/C////////////
            A8=A&1;
            B8=B&1;
            A>>=1;
            if(Bins==1) A|=128;
            B>>=1;
            if(A8==1)	B|=128;
            C>>=1;
            if(B8==1)	C|=128;
        }
    }
}

// 去处数据中指定的字符fix
void RadioLink::ActEncrypt_CharClear(char* pchar,const int nlength,char& nEmp,const char fix)
{
    //求取nEmp,去0x0A标记
    quint8 tmp[256];
    unsigned char d=0;
    memset(tmp,0,256);
    for(int i=0;i<nlength;i++)
    {
        d=pchar[i];
        tmp[d] = 1;
    }

    for(int i=255;i>=0;i--)
    {
        if(tmp[i]==0)
        {
            if (i != fix)
            {
                nEmp=i;
                break;
            }
        }
    }
    nEmp=256+fix-nEmp;

    //去除fix
    int a=0;
    for(int i=0;i<nlength;i++)
    {
        a=pchar[i]+nEmp;
        if(a>255)
            a-=256;
        pchar[i]=a;
    }
}

// 恢复数据中的指定字符fix
void RadioLink::ActEncrypt_CharAdd(char* pchar,const int nlength,const char nEmp)
{
    for(int i=0;i<nlength;i++)
    {
        pchar[i]-=nEmp;
        if(pchar[i]<0)
            pchar[i] += char(256);
    }
}
