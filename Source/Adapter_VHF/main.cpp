#include <QCoreApplication>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include "RadioLink/RadioLinkManage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConfigLoader::getInstance()->load();
    SocketManage::getInstance()->init();
    RadioManage ::getInstance()->init();
    RadioLinkManage::getInstance()->init();
    return a.exec();
}
