#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QDir>

class TcpClient:public QTcpSocket
{
    Q_OBJECT

public:
    TcpClient(QObject *parent=nullptr);

public slots:

    ///
    /// \brief receiveClientDataSlot 接收数据
    ///
    void receiveDataSlot();

signals:

    ///
    /// \brief setClientImageSignal 绑定socketID和图片
    /// \param img
    /// \param socketId
    ///
    void setClientImageSignal(const QString& img,qintptr socketId);

    ///
    /// \brief setDectModelSignal 设置
    /// \param model
    ///
    void setDectModelSignal(int model);
};

#endif // TCPCLIENT_H
