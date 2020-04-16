#include <QCoreApplication>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include "RadioLink/RadioLinkManage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if !WIN32
    system("amixer sset 'MIXINR IN3R' on");
    system("amixer sset 'INPGAR IN3R' on");
//    system("amixer cset name='Headphone Volume' 127,127");
    system("amixer cset numID='9' 127");
#endif

    ConfigLoader::getInstance()->load();
    SocketManage::getInstance()->init();
    RadioManage ::getInstance()->init();
#if !WIN32
    QString status;
    AudioControl::getInstance()->init(status);
#endif
	RadioLinkManage::getInstance()->init();
    return a.exec();
}
