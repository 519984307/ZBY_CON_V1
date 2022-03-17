#include "conv1service.h"

CONV1Service::CONV1Service(int argc, char **argv): QtService<QCoreApplication>(argc, argv, "ZBY_CONV1")
{
    setServiceDescription("Shenzhen Zhongbaiyuan container number identification system");
    setServiceFlags(QtServiceBase::CanBeSuspended);
}

CONV1Service::~CONV1Service()
{
    delete server;
    server=nullptr;
}

void CONV1Service::start()
{
    server = new TcpServer();
}

void CONV1Service::stop()
{
    delete server;
    server=nullptr;
}
