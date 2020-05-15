#include <QApplication>
#include <stdlib.h>
#include "Audio/AudioControl.h"
#include "Config/ConfigLoader.h"
#include "Socket/SocketManage.h"
#include <QFontDatabase>
#include "Keyboard/KeyUart.h"
#include "UI/UIManager.h"
#include "UI/WidgeBase.h"
#include "UI/WidgeVHF181d.h"
#include "UI/WidgeSSB.h"

#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<AudioData>("AudioPeriodData");

    system("amixer sset 'MIXINR IN3R' on");
    system("amixer sset 'INPGAR IN3R' on");
    //system("amixer cset name='Headphone Volume' 127,127");
    //使用amixer control命令查询播放、录音的numID，具体硬件需要相应修改
    system("amixer cset numID='1' 23");
    system("amixer cset numID='9' 127");

    // Set Gloal Font Lib
    int id = QFontDatabase::addApplicationFont("/opt/qt4.8.5/lib/fonts/msyh.ttf");//msyhbd
    if(id != -1) {
        QString simsun = QFontDatabase::applicationFontFamilies(id).at(0);
        qDebug()<<endl<<"The Gloal Font is:"+simsun<<endl;
        QFont m_font(simsun,13);
        int pointsize = m_font.pointSize();
        m_font.setPixelSize(pointsize*90/72); //设置字体大小单位为像素大小，而不是以磅为单位
        a.setFont(m_font);
    } else {
        qDebug()<<"Doesn't set Gloal Font";
    }


//    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));    //tr 使用的编码
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    QString status;
    ConfigLoader::getInstance()->load();
    AudioControl::getInstance()->init(status);

    UIManager::getInstance()->init();
    KeyUart::getInstance()->serialInit();
    SocketManage::getInstance()->init();

    WidgeBase *widget = UIManager::getInstance()->getCurrWidge();
    widget->show();

    return a.exec();
}
