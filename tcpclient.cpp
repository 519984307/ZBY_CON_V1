#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
{
    this->setParent(parent);
}

void TcpClient::receiveDataSlot()
{
    QByteArray buf=readAll();
    QString imgs=QString::fromLatin1(buf);
        if(imgs.startsWith("-i")){
            QStringList imgList=imgs.simplified().split(' ');
            foreach (auto img, imgList) {
                if(!img.trimmed().startsWith("-i") && !img.isEmpty()){
                    emit setClientImageSignal(QDir::fromNativeSeparators(img.trimmed()),this->socketDescriptor());
                }
            }
        }
        if(imgs.startsWith("-m")){
            QStringList par=imgs.simplified().split(' ');
            if(par.size()>=2){
                emit setDectModelSignal(par.at(1).trimmed().toInt());
            }
        }
    buf.clear();
}
