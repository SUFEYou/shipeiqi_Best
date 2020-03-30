#include "TerminalBase.h"
#include <QFile>

CTerminalBase::CTerminalBase()
{
    m_CPos		= 0;
    m_CFlag		= 0;
    m_CLen		= 0;
    m_CType		= 0;
    m_CSucess	= false;

    memset(m_CReadBuf,0,500);

    ///////////////////////////////////////////////////////////////////
    m_rData		= new char[MEM_LENGTH*2];
    m_rLength	= 0;
    m_rSave		= false;
    m_rFile		= "";
    m_rPath		= "";

    m_nTag		= 0;
    m_nPortNum	= 0;

    m_nRunState	= 0;
}

CTerminalBase::~CTerminalBase()
{
    if (m_rData)
    {
        delete []m_rData;
        m_rData = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// Restore Receive Bytes
void CTerminalBase::UseSaveReceiveBytes(char* pchar, const int nlength)
{
    if ( !m_rSave )
    {
        return;
    }

    memcpy(m_rData+m_rLength,pchar,nlength);
    m_rLength += nlength;

    if (m_rLength >= MEM_LENGTH)
    {
        UseSaveBytes();
    }
}

// Save Bytes
bool CTerminalBase::UseSaveBytes()
{
    QFile fp(m_rPath+m_rFile);
    if (!fp.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        return false;
    }

    fp.write(m_rData, m_rLength);
    fp.close();

    memset(m_rData,0,m_rLength);
    m_rLength = 0;
    return true;
}

// Set Save Path&File's Name
void CTerminalBase::UseSetSavePathFile(QString spath,  QString sfile)
{
    m_rFile = sfile;
    m_rPath = spath;
}

// Open or Close the Bytes Save
void CTerminalBase::UseSetSaveAvailable(bool bsave)
{
    m_rSave = bsave;
}

// Get Save Available
bool CTerminalBase::UseGetSaveAvailable()
{
    return m_rSave;
}

// Set Terminal Tag
void CTerminalBase::UseSetTerminalTag(quint32 tag)
{
    m_nTag = tag;
}

// Get Terminal Tag
quint32 CTerminalBase::UseGetTerminalTag()
{
    return m_nTag;
}

//////////////////////////////////////////////////////////////////////////
// Set the PortNum
void CTerminalBase::UseSetPortNum(quint32 portn)
{
    m_nPortNum = portn;
}

// Get the PortNum
quint32 CTerminalBase::UseGetPortNum()
{
    return m_nPortNum;
}
