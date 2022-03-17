#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent):QTcpServer (parent)
{
    this->setParent(parent);
    this->listen(QHostAddress::AnyIPv4,55550);

    IDE=new Identify ();
    connect(this,&TcpServer::detectImageSignal,IDE,&Identify::slotDetectImage);
    connect(IDE,&Identify::signalDetectRst,this,&TcpServer::detectRstSlot);
}

TcpServer::~TcpServer()
{
    foreach (auto client, clientSocketIdMap.values()) {
        client->disconnected();
        client->abort();
        client->close();

        delete client;
        client=nullptr;
    }
    clientSocketIdMap.clear();

    delete  IDE;
    IDE=nullptr;

    this->close();
}

void TcpServer::incomingConnection(qintptr socketID)
{
    TcpClient* pClient=new TcpClient (this);

    pClient->setSocketDescriptor(socketID);
    clientSocketIdMap.insert(socketID,pClient);

    connect(pClient,&TcpClient::setClientImageSignal,this,&TcpServer::setClientImageSlot);
    connect(pClient,&QIODevice::readyRead,pClient,&TcpClient::receiveDataSlot);
    connect(pClient,&TcpClient::disconnected,this,&TcpServer::disconnectedSlot);

    qInfo().noquote()<<QString("[%1] %2-%3:A new client is added").arg(this->metaObject()->className(),pClient->peerAddress().toString(),QString::number(pClient->peerPort()));
}

void TcpServer::disconnectedSlot()
{
    TcpClient* pClient=qobject_cast<TcpClient*>(sender());

    qintptr socketID= clientSocketIdMap.key(pClient);
    clientSocketIdMap.remove(socketID);

    qInfo().noquote()<<QString("[%1] %2-%3:The client is Offline").arg(this->metaObject()->className(),pClient->peerAddress().toString(),QString::number(pClient->peerPort()));
}

void TcpServer::setClientImageSlot(const QString &img, qintptr socketId)
{
    //Q_UNUSED(socketId)
    ImageToClientMap.insert(img,socketId);
    emit detectImageSignal(img);
}

void TcpServer::detectRstSlot(const QString &img, const QString &rst)
{
    qintptr CID=ImageToClientMap.value(img);

    clientSocketIdMap.value(CID)->write(QString("%1-%2 ").arg(img,rst).toLatin1());
//    clientSocketIdMap.value()
//    foreach (auto obj, clientSocketIdMap.values()) {
//        emit obj->write(QString("%1-%2 ").arg(img,rst).toLatin1());
//    }
    ImageToClientMap.remove(img);
}
