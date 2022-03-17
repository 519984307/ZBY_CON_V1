#include <QCoreApplication>
#include <iostream>
#include <QPointer>

#include "identify.h"
#include "conv1service.h"
//#include "LogController/logcontroller.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    Identify ide;

//    while (true) {

//        string cmd;
//        std::cin>>cmd;

//        if(cmd=="cc"){
//            break;
//        }
//        else {
//            ide.slotDetectImage(QString::fromStdString(cmd));
//        }
//    }

//    return a.exec();

    //QPointer<LogController> pLog=QPointer<LogController>(new LogController("ZBY_CON_V1"));

    CONV1Service service(argc, argv);
    return service.exec();
}
