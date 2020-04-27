#include "UdpRevCast.h"
#include <QDebug>
#include "UI/UIManager.h"

UdpRevCast::UdpRevCast()
{

}

void UdpRevCast::init(int recivPort)
{
    this->m_recivPort = recivPort;

    m_udpSocket = new QUdpSocket(this);

    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(onRev()));
    connect(m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    m_udpSocket->bind(m_recivPort, QUdpSocket::ShareAddress);

}

void UdpRevCast::onRev()
{
    while (m_udpSocket->hasPendingDatagrams())
    {
       QByteArray datagram;
       datagram.resize(m_udpSocket->pendingDatagramSize());
       m_udpSocket->readDatagram(datagram.data(), datagram.size());

       char* data = datagram.data();
       int nLen   = datagram.size();

       if(nLen == 2 + sizeof(LocationInfo)) {
           if(data[0] == 0x00 && data[1] == 0x7E){

               LocationInfo locaInfo;
               memcpy(&locaInfo, data + 2, sizeof(LocationInfo));

               qDebug("Typ: %d\n",  locaInfo.LocaTyp);
               qDebug("Lat: %f\n",  locaInfo.Lat);
               qDebug("Lon: %f\n",  locaInfo.Lon);
               qDebug("YY:  %d\n",  locaInfo.Year);
               qDebug("MM:  %d\n",  locaInfo.Month);
               qDebug("DD:  %d\n",  locaInfo.Day);
               qDebug("HH:  %d\n",  locaInfo.Hour);
               qDebug("Mi:  %d\n",  locaInfo.Min);
               qDebug("SS:  %d\n",  locaInfo.Sec);

               QString tim;
               tim.append(locaInfo.Hour).append(":").append(locaInfo.Min).append(":").append(locaInfo.Sec);

               UIManager::getInstance()->updateAllCurTim(tim);
           }
       }
    }
}

//void UdpRevCast::separateData(const char* data, const uint16_t len)
//{
//    recvArray.append(data, len);
//    if (recvArray.length() < 2+sizeof(LocationInfo))//完整包数据长度
//        return;
//    else
//    {
//        int  step = 0;  //
//        char sd[2];     // 00H 7EH
//        //初始化为0xFF，防止与包头数据冲突
//        memset(sd, 0xFF, sizeof(sd));
//        int  contBegin = 0;
//        int  contLen = 0;
//        int  removeLen = 0;
//        for(int i=0; i< recvArray.length(); i++)
//        {
//            char currd = recvArray.at(i);
//            if ( step == 0 )//定位包头
//            {
//                sd[0] = sd[1];
//                sd[1] = currd;
//                //  00H 7EH
//                if(sd[0] == 0x00 && sd[1] == 0x7E)
//                {
//                    step = 1;
//                    contBegin = i+1;
//                    contLen = 0;
//                    memset(sd, 0xFF, sizeof(sd));
//                }
//            }
//            else if ( step == 1 )
//            {
//                sd[0] = sd[1];
//                sd[1] = currd;
//                contLen += 1;
//                if (contLen < sizeof(LocationInfo))
//                {
//                    //接收到数据长度不足，则舍弃
//                    if(sd[0] == 0x00 && sd[1] == 0x7E)
//                    {
//                        contBegin = i+1;
//                        contLen = 0;
//                        memset(sd, 0xFF, sizeof(sd));
//                    }
//                }
//                else if (contLen == sizeof(LocationInfo))
//                {
//                    memset(sd, 0, sizeof(sd));
//                    recvList.push_back(recvArray.mid(contBegin, contLen));
//                    removeLen = contBegin + contLen;
//                    step = 0;
//                    contLen = 0;
//                }
//            }
//        }

//        recvArray.remove(0, removeLen);
//    }
//}


//void UdpRevCast::parseData()
//{
//    while(!recvList.isEmpty())
//    {
//        QByteArray tmpArray = recvList.first();
//        recvList.pop_front();
//        memcpy(&locationInfo, tmpArray.data(), tmpArray.length());

//        qDebug("Type: %d\n", locationInfo.LocationType);
//        qDebug("Lat: %f\n",  locationInfo.Lat);
//        qDebug("Lon: %f\n",  locationInfo.Lon);
//        qDebug("Year: %d\n", locationInfo.Year);
//        qDebug("Month: %d\n",locationInfo.Month);
//        qDebug("Day: %d\n",  locationInfo.Day);
//        qDebug("Hour: %d\n", locationInfo.Hour);
//        qDebug("Min: %d\n",  locationInfo.Min);
//        qDebug("Sec: %d\n",  locationInfo.Sec);
////        fflush(stdout);
//    }
//}


void UdpRevCast::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qDebug() << m_udpSocket->errorString();
}
