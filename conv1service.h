#ifndef CONV1SERVICE_H
#define CONV1SERVICE_H

#include <QObject>
#include <QCoreApplication>
#include <QPointer>

#include "qtservice.h"
#include "tcpserver.h"

class CONV1Service:public QObject, public QtService<QCoreApplication>
{
    Q_OBJECT
public:
    CONV1Service(int argc, char **argv);
    ~CONV1Service()override;

private:
    TcpServer* server;

protected:
    void start()override;
    void stop()override;
    void pause()override{}
    void resume()override{}
};

#endif // CONV1SERVICE_H
