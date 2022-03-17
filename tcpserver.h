#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QPointer>

#include "tcpclient.h"
#include "identify.h"

class TcpServer:public QTcpServer
{
    Q_OBJECT

public:
    TcpServer(QObject *parent=nullptr);
    ~TcpServer();

protected:

    ///
    /// \brief incomingConnection 重写客户端接入
    /// \param handle
    ///
    void incomingConnection(qintptr socketID);

private:

    ///
    /// \brief clientSocketIdMap 客户端字典
    ///
    QMap<qintptr,TcpClient*> clientSocketIdMap;

    ///
    /// \brief ImageToClientMap 图片对应的客户端
    ///
    QMap<QString,qintptr> ImageToClientMap;

    ///
    /// \brief imgVec 图片
    ///
    QVector<QString> imgVec;

    ///
    /// \brief IDE 识别器
    ///
    Identify* IDE;

private slots:

    ///
    /// \brief disconnectedSlot 客户端断开信号(从客户端列表删除断开的客户端)
    ///
    void disconnectedSlot();

    ///
    /// \brief setClientImageSlot 绑定socketID和图片
    /// \param img
    /// \param socketId
    ///
    void setClientImageSlot(const QString& img,qintptr socketId);

    ///
    /// \brief detectRstSlot 识别结果
    /// \param rst
    ///
    void detectRstSlot(const QString& img, const QString& rst);

signals:

    ///
    /// \brief detectImageSignal 识别图片
    /// \param image
    ///
    void detectImageSignal(const QString& image);
};

#endif // TCPSERVER_H
