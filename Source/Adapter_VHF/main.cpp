#include <QCoreApplication>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include "RadioLink/RadioLinkManage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    system("amixer sset 'MIXINR IN3R' on");
    system("amixer sset 'INPGAR IN3R' on");
    system("amixer cset name='Headphone Volume' 127,127");

    QString status;
    ConfigLoader::getInstance()->load();
    SocketManage::getInstance()->init();
    RadioManage ::getInstance()->init();
    AudioControl::getInstance()->init(status);
	RadioLinkManage::getInstance()->init();
    return a.exec();
}
