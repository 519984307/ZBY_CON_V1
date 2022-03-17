#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
{
    this->setParent(parent);
}

void TcpClient::receiveDataSlot()
{
    QByteArray buf=readAll();
    QString imgs=QString::fromLatin1(buf);

//    QFile f("c:\\test.txt");
//    if(f.open(QIODevice::WriteOnly | QIODevice::Append))
//    {
//        QTextStream out(&f);
//        out<<"buf";
//        out<<buf;
//        out<<"-------------------\r";
//    }
//     f.close();

        if(imgs.startsWith("-i")){
            QStringList imgList=imgs.simplified().split(' ');
            foreach (auto img, imgList) {
                if(!img.trimmed().startsWith("-i") && !img.isEmpty()){
//                    if(f.open(QIODevice::WriteOnly | QIODevice::Append))
//                    {
//                        QTextStream out(&f);
//                        out<<"img";
//                                            out<<img;
//                        out<<"-------------------\r";
//                    }
//                    f.close();
                    emit setClientImageSignal(QDir::fromNativeSeparators(img.trimmed()),this->socketDescriptor());
                                        //QThread::msleep(100);

                }
            }
        }






    buf.clear();
    //imgs.clear();
}
