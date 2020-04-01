#include <QCoreApplication>
#include "config/ConfigLoader.h"
#include "socket/SocketManage.h"
#include "Radio/RadioManage.h"
#include "VHFLayer/CE_VHFNodeManage.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConfigLoader::getInstance()->load();
    SocketManage::getInstance()->init();
    RadioManage ::getInstance()->init();
//    UartManage::getInstance()->init();
    CE_VHFNodeManage::getInstance()->init();
    return a.exec();
}
