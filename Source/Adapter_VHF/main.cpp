#include <QCoreApplication>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConfigLoader::getInstance()->load();
    SocketManage::getInstance()->init();
    return a.exec();
}
