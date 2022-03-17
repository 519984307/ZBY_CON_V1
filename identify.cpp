#include "identify.h"

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string.h>
#include <algorithm>

Identify::Identify(QObject *parent, const QString &infer_cls, const QString &infer_rec, const QString &infer_det)
{
    this->setParent(parent);

    loadDictParment();
    initCheckMap();

    OCRParameter parameter;
    parameter.use_gpu = false;
    parameter.gpu_id = 0;
    parameter.gpu_mem = 4000;
    parameter.numThread = 8;
    parameter.enable_mkldnn = true;

    //检测模型相关
    parameter.Padding = 50;
    parameter.MaxSideLen = 960;
    parameter.BoxThresh = 0.35f;
    parameter.BoxScoreThresh = 0.45f;
    parameter.UnClipRatio = 1.6f;
    parameter.use_polygon_score = true;
    parameter.visualize = true;

    parameter.DoAngle = true;
    parameter.MostAngle = true;

    //方向分类器相关
    parameter.use_angle_cls = false;
    parameter.cls_thresh = 0.9f;

    QString rootPath=QCoreApplication::applicationDirPath()+"/inference/";
    QString cls_infer=QString("%1ch_ppocr_mobile_v2.0_cls_infer").arg(rootPath);
    if("NUL" != infer_cls){
        cls_infer=QString("%1%2").arg(rootPath,infer_cls);
    }
    QString rec_infer=QString("%1ch_PP-OCRv2_rec_infer").arg(rootPath);
    if("NUL" != infer_rec){
        rec_infer=QString("%1%2").arg(rootPath,infer_rec);
    }
    QString det_infer=QString("%1ch_PP-OCRv2_det_infer").arg(rootPath);
    if("NUL" != infer_det){
        det_infer=QString("%1%2").arg(rootPath,infer_det);
    }
    QString keys=QString("%1ppocr_keys.txt").arg(rootPath);

    pEngine=Initialize(const_cast<char*>(det_infer.toStdString().c_str()),
                       const_cast<char*>(cls_infer.toStdString().c_str()),
                       const_cast<char*>(rec_infer.toStdString().c_str()),
                       const_cast<char*>(keys.toStdString().c_str()),
                       parameter);

}

Identify::~Identify()
{
    FreeEngine(pEngine);
}

void Identify::loadDictParment()
{
    QString rootPath=QCoreApplication::applicationDirPath();

    QFile file(rootPath+"/V1CON.cc");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()) {
            QByteArray arr=file.readLine().trimmed();
            if(arr.isEmpty()){
                continue;
            }
            CONDICT.append(arr);
        }
    }
    file.close();

    file.setFileName(rootPath+"/CON1.cc");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()) {
            QByteArray arr=file.readLine().trimmed();
            QList<QByteArray> tmp=arr.split('-');
            if(tmp.size()==2){
                CONMAP.insert(tmp.at(0),tmp.at(1));
            }
        }
    }
    file.close();

    file.setFileName(rootPath+"/_CON.cc");
    if(file.open(QIODevice::ReadOnly |QIODevice::Text)){
        while (!file.atEnd()) {
            QByteArray arr=file.readLine().trimmed();
            if(arr.isEmpty()){
                continue;
            }
            _CONDICT.append(arr);
        }
    }
    file.close();

    file.setFileName(rootPath+"/ISO.cc");
    if(file.open(QIODevice::ReadOnly |QIODevice::Text)){
        bool size=true;
        while (!file.atEnd()) {
            QByteArray arr=file.readLine().trimmed();
            if(arr.isEmpty()){
                continue;
            }
            else if("[SIZE]" == arr){
                continue;
            }
            else if ("[COUNTRY]" == arr) {
                size=false;
                continue;
            }
            else {
                if(size){
                    ISODICT.append(arr);
                }
                else {
                    COUNTRYDICT.append(arr);
                }
            }
        }
    }
    file.close();

    file.setFileName(rootPath+"/ISO1.cc");
    if(file.open(QIODevice::ReadOnly |QIODevice::Text)){
        bool size=true;
        while (!file.atEnd()) {
            QByteArray arr=file.readLine().trimmed();
            if(arr.isEmpty()){
                continue;
            }
            else if("[SIZE]" == arr){
                continue;
            }
            else if ("[COUNTRY]" == arr) {
                size=false;
                continue;
            }
            else {
                QList<QByteArray> tmp=arr.split('-');
                if(tmp.size()==2){
                    if(size){
                        ISOMAP.insert(tmp.at(0),tmp.at(1));
                    }
                    else {
                        COUNTRYMAP.insert(tmp.at(0),tmp.at(1));
                    }
                }
            }
        }
    }
    file.close();
}

void Identify::initCheckMap()
{
    char ch = 'A';
    for (int i = 10; i <= 38; i++)
    {
        if ((i == 11) || (i == 22) || (i == 33))
        {
            i++;
        }
        checkMap.insert(ch,i);
        ch++;
    }
}

void Identify::slotDetectImage(const QString &image)
{
    //DecIMG(image,pEngine);
    QFileInfo info(image);
    if(info.isFile()){

        OCRResult* ocrResult=new OCRResult() ;
        int cout= Detect(pEngine, const_cast<char*>(image.toStdString().c_str()), &ocrResult);

        if(cout==0){
            emit signalDetectRst(image,"RESULT:||0|0");

            /*****************************
            * @brief:释放识别结果对象
            ******************************/
            FreeDetectResult(ocrResult);

            return;
        }

        QString ConS="";
        QString Iso="";
        /*****************************
        * @brief:箱型原始数据
        ******************************/
        QString Iso_="";

        double CON_source=0;
        double ISO_source=0;

        int sX1=-1;
        int sY1=-1;
        int sX2=-1;
        int sY2=-1;
        int sX3=-1;
        int sY3=-1;
        int sX4=-1;
        int sY4=-1;

        bool con_number=false;

        for (int ind = 0; ind < cout; ind++)
        {
            QString rst= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext)).toUpper();

//            QFile f("c:\\test.txt");
//            if(f.open(QIODevice::WriteOnly | QIODevice::Append))
//            {
//                QTextStream out(&f);
//                out<<"Identify:";
//                out<<rst;
//                out<<"-------------------\r";
//            }
//             f.close();

            QString number="";

            for(int i=0;i<rst.size();i++){
                if((rst.at(i)>='0' && rst.at(i)<='9') || (rst.at(i)>='A' && rst.at(i)<='Z')){
                    number.append(rst.at(i));
                }
            }

            number.remove(QRegExp("\\s"));
            number.remove(QRegExp("\\."));

            if(number.size()>=4){
                if(!CONMAP.value(number.mid(0,4),"").isEmpty()){
                   number.replace(0,4,CONMAP.value(number.mid(0,4)));
                }
                else if(number.at(3)<='9' && !CONMAP.value(number.mid(0,3),"").isEmpty()){
                   number.replace(0,3,CONMAP.value(number.mid(0,3)));
                }
            }
            if(number.size()==3){
                if(!CONMAP.value(number,"").isEmpty()){
                   number.replace(0,3,CONMAP.value(number));
                }
            }

            if(number.simplified().size()>=4 && CONDICT.indexOf(number.mid(0,4)) != -1 && _CONDICT.indexOf(number.mid(0,4))==-1){
                if(ConS.isEmpty()){
                    /*****************************
                    * @brief:找到箱号字头
                    ******************************/
                    ConS.append(number);
                    sX1=ocrResult->pOCRText[ind].points[0].x;
                    sY1=ocrResult->pOCRText[ind].points[0].y;
                    sX2=ocrResult->pOCRText[ind].points[1].x;
                    sY2=ocrResult->pOCRText[ind].points[1].y;
                    sX3=ocrResult->pOCRText[ind].points[2].x;
                    sY3=ocrResult->pOCRText[ind].points[2].y;
                    sX4=ocrResult->pOCRText[ind].points[3].x;
                    sY4=ocrResult->pOCRText[ind].points[3].y;

                    if(ConS.size()>=10){
                        con_number=true;
                    }
                    /*****************************
                    * @brief:置信度
                    ******************************/
                    CON_source=ocrResult->pOCRText[ind].score;
                    //break;
                }
            }

            /*****************************
            * @brief:箱型
            ******************************/
            if((ISODICT.indexOf(number)!=-1 || !ISOMAP.value(number,"").isEmpty()) && Iso.isEmpty()){
                if(!ISOMAP.value(number,"").isEmpty()){
                    Iso=ISOMAP.value(number);
                }
                else {
                    Iso=number;
                }
                if(!Iso.isEmpty()){
                    Iso_=number;
                    /*****************************
                    * @brief:置信度
                    ******************************/
                    ISO_source=ocrResult->pOCRText[ind].score;
                    //break;
                }
            }
            else if(number.size()>=4 && Iso.isEmpty() && !number.endsWith("S")){
                if((ISODICT.indexOf(number.mid(0,4))!=-1 || !ISOMAP.value(number.mid(0,4),"").isEmpty())){
                    if(!ISOMAP.value(number.mid(0,4),"").isEmpty() ){
                        Iso=ISOMAP.value(number.mid(0,4));
                    }
                    else {
                        Iso=number.mid(0,4);
                    }
                    if(!Iso.isEmpty()){
                        Iso_=number;
                        /*****************************
                        * @brief:置信度
                        ******************************/
                        ISO_source=ocrResult->pOCRText[ind].score;
                        //break;
                    }
                }
            }
        }


        if(!ConS.isEmpty() && !con_number){
            for (int ind = 0; ind < cout; ind++)
            {
                if((ocrResult->pOCRText[ind].points[3].y-ocrResult->pOCRText[ind].points[0].y)<(sY4-sY1)/2){
                    continue;
                }

                /*****************************
                * @brief:水平箱号，（单行）
                * WDFU 123456 7
                ******************************/
                QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                if(number==Iso_){
                    continue;
                }

                if(ConS != number){
                    if(qAbs(sX2-ocrResult->pOCRText[ind].points[0].x)<(sX2-sX1) && qAbs(sY2-ocrResult->pOCRText[ind].points[0].y)<((sY3-sY2)/2)){
                        ConS.append(number);
                        con_number=true;
                        if(ConS.size()>=10){
                            break;
                        }
                    }
                }
            }
            if(!con_number){
                for (int ind = 0; ind < cout; ind++)
                {
                    /*****************************
                    * @brief:水平箱号，（多行）
                    * WDFU
                    * 123456 7
                    ******************************/
                    QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                    if(number==Iso_){
                        continue;
                    }

                    if(ConS != number){
                        if(qAbs(sX4-ocrResult->pOCRText[ind].points[0].x)<((sX3-sX4)/2) && qAbs(sY4-ocrResult->pOCRText[ind].points[0].y)<(sY4-sY1)){
                            ConS.append(number);

                            if(ConS.size()>=10){
                                con_number=true;
                                break;
                            }
                            else {
                                sX1=ocrResult->pOCRText[ind].points[0].x;
                                sY1=ocrResult->pOCRText[ind].points[0].y;
                                sX2=ocrResult->pOCRText[ind].points[1].x;
                                sY2=ocrResult->pOCRText[ind].points[1].y;
                                sX3=ocrResult->pOCRText[ind].points[2].x;
                                sY3=ocrResult->pOCRText[ind].points[2].y;
                                sX4=ocrResult->pOCRText[ind].points[3].x;
                                sY4=ocrResult->pOCRText[ind].points[3].y;
                            }
                        }
                    }
                }
            }
            if(!con_number){
                for (int ind = 0; ind < cout; ind++)
                {
                    /*****************************
                    * @brief:水平箱号，（多行）
                    * WDFU
                    * 123     456 7
                    *-----------------------------
                    * WDFU
                    * 123
                    * 1234
                    ******************************/
                    QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                    if(number==Iso_){
                        continue;
                    }

                    if(ConS != number){
                        if(qAbs(sX2-ocrResult->pOCRText[ind].points[0].x)<(sX2-sX1) && qAbs(sY2-ocrResult->pOCRText[ind].points[0].y)<((sY3-sY2)/2)){
                            ConS.append(number);
                            con_number=true;
                            if(ConS.size()>=10){
                                break;
                            }
                        }
                    }
                }
            }
        }

        ConS.remove(QRegExp("\\s"));
        ConS.remove(QRegExp("\\."));

        QString container="";
        for(int i=0;i<ConS.size();i++){
            if((ConS.at(i)>='0' && ConS.at(i)<='9') || (ConS.at(i)>='A' && ConS.at(i)<='Z')){
                container.append(ConS.at(i));
            }
        }

        uint64_t source=4000000;
        if(Iso.isEmpty()){
            source=0;
        }

        uint64_t sourceC=11000000;
        if(container.size()>11 || container.size()<10){
            sourceC=CON_source*10000000;
        }

        if(container.size()>11){
            container=ConS.mid(0,11);
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(numberCheck(container),Iso,QString::number(sourceC,10),QString::number(source,10)));

        }
        else if(container.size()>=10){
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(numberCheck(container),Iso,QString::number(sourceC,10),QString::number(source,10)));

        }
        else {
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(container,Iso,QString::number(sourceC,10),QString::number(source,10)));
        }

        /*****************************
        * @brief:释放识别结果对象
        ******************************/
        FreeDetectResult(ocrResult);
    }
    else {
        qWarning().noquote()<<QString("File does not exist, identification failed!");
        emit signalDetectRst(image,"RESULT:||0|0");
    }
}



int Identify::computeQuadraticPower(int variable)
{
    int result;
    if (variable == 0)
    {
        return 1;
    }
    result = 2 * computeQuadraticPower(variable - 1);
    return result;
}

QString Identify::numberCheck(QString number)
{
    if(number.length()<10)/* 最后一位可以计算出来 */
    {
        return number;
    }

    int sum=0;
    int i = 1;
    foreach (QChar ch , number)
    {/* 计算校验总值 */
        if (i == 11)
        {
            break;
        }
        if (i > 4)
        {
            sum += (ch.toLatin1()-'0') * (computeQuadraticPower(i-1));
        }
        else
        {
            if (checkMap.value(ch)!=0)
            {
                sum += checkMap.value(ch) * computeQuadraticPower(i - 1);
            }
            else
            {/* 箱号前4位必须是英文,否则箱号格式错误 */
                return number;
            }
        }
        i++;
    }

    int die=sum % 11;
    if(die==10){
        die=0;
    }

    if(number.count()==10){
        number=number.append(QString::number(die));
        return number;
    }
    if (number.count()==11 && die!=number[10].toLatin1()-'0')
    {
        if(0<=die && die <=9)
        {
            number[10]=die+'0';/* 矫正结果 */
            return number;
        }
        return number;
    }
    return number;
}

void Identify::DecIMG(const QString &image, int *pEngine)
{
    //QString tmpImg=QDir::toNativeSeparators(image);
    QFileInfo info(image);
    if(info.isFile()){

        LpOCRResult ocrResult;
        int cout= Detect(pEngine, const_cast<char*>(image.toStdString().c_str()), &ocrResult);

        QString ConS="";
        QString Iso="";
        /*****************************
        * @brief:箱型原始数据
        ******************************/
        QString Iso_="";

        double CON_source=0;
        double ISO_source=0;

        int sX1=-1;
        int sY1=-1;
        int sX2=-1;
        int sY2=-1;
        int sX3=-1;
        int sY3=-1;
        int sX4=-1;
        int sY4=-1;

        bool con_number=false;

        for (int ind = 0; ind < cout; ind++)
        {
            QString rst= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext)).toUpper();
            qDebug()<<"Identify:"<<rst;

            QString number="";

            for(int i=0;i<rst.size();i++){
                if((rst.at(i)>='0' && rst.at(i)<='9') || (rst.at(i)>='A' && rst.at(i)<='Z')){
                    number.append(rst.at(i));
                }
            }

            number.remove(QRegExp("\\s"));
            number.remove(QRegExp("\\."));

            if(number.size()>=4){
                if(!CONMAP.value(number.mid(0,4),"").isEmpty()){
                   number.replace(0,4,CONMAP.value(number.mid(0,4)));
                }
                else if(number.at(3)<='9' && !CONMAP.value(number.mid(0,3),"").isEmpty()){
                   number.replace(0,3,CONMAP.value(number.mid(0,3)));
                }
            }
            if(number.size()==3){
                if(!CONMAP.value(number,"").isEmpty()){
                   number.replace(0,3,CONMAP.value(number));
                }
            }

            if(number.simplified().size()>=4 && CONDICT.indexOf(number.mid(0,4)) != -1){
                if(ConS.isEmpty()){
                    /*****************************
                    * @brief:找到箱号字头
                    ******************************/
                    ConS.append(number);
                    sX1=ocrResult->pOCRText[ind].points[0].x;
                    sY1=ocrResult->pOCRText[ind].points[0].y;
                    sX2=ocrResult->pOCRText[ind].points[1].x;
                    sY2=ocrResult->pOCRText[ind].points[1].y;
                    sX3=ocrResult->pOCRText[ind].points[2].x;
                    sY3=ocrResult->pOCRText[ind].points[2].y;
                    sX4=ocrResult->pOCRText[ind].points[3].x;
                    sY4=ocrResult->pOCRText[ind].points[3].y;

                    if(ConS.size()>=10){
                        con_number=true;
                    }
                    /*****************************
                    * @brief:置信度
                    ******************************/
                    CON_source=ocrResult->pOCRText[ind].score;
                    //break;
                }
            }

            /*****************************
            * @brief:箱型
            ******************************/
            if((ISODICT.indexOf(number)!=-1 || !ISOMAP.value(number,"").isEmpty()) && Iso.isEmpty()){
                if(!ISOMAP.value(number,"").isEmpty()){
                    Iso=ISOMAP.value(number);
                }
                else {
                    Iso=number;
                }
                if(!Iso.isEmpty()){
                    Iso_=number;
                    /*****************************
                    * @brief:置信度
                    ******************************/
                    ISO_source=ocrResult->pOCRText[ind].score;
                    //break;
                }
            }
            else if(number.size()>=4 && Iso.isEmpty() && !number.endsWith("S")){
                if((ISODICT.indexOf(number.mid(0,4))!=-1 || !ISOMAP.value(number.mid(0,4),"").isEmpty())){
                    if(!ISOMAP.value(number.mid(0,4),"").isEmpty() ){
                        Iso=ISOMAP.value(number.mid(0,4));
                    }
                    else {
                        Iso=number.mid(0,4);
                    }
                    if(!Iso.isEmpty()){
                        Iso_=number;
                        /*****************************
                        * @brief:置信度
                        ******************************/
                        ISO_source=ocrResult->pOCRText[ind].score;
                        //break;
                    }
                }
            }
        }

        if(!ConS.isEmpty() && !con_number){
            for (int ind = 0; ind < cout; ind++)
            {
                if((ocrResult->pOCRText[ind].points[3].y-ocrResult->pOCRText[ind].points[0].y)<(sY4-sY1)/2){
                    continue;
                }

                /*****************************
                * @brief:水平箱号，（单行）
                * WDFU 123456 7
                ******************************/
                QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                if(number==Iso_){
                    continue;
                }

                if(ConS != number){
                    if(qAbs(sX2-ocrResult->pOCRText[ind].points[0].x)<(sX2-sX1) && qAbs(sY2-ocrResult->pOCRText[ind].points[0].y)<((sY3-sY2)/2)){
                        ConS.append(number);
                        con_number=true;
                        if(ConS.size()>=10){
                            break;
                        }
                    }
                }
            }
            if(!con_number){
                for (int ind = 0; ind < cout; ind++)
                {
                    /*****************************
                    * @brief:水平箱号，（多行）
                    * WDFU
                    * 123456 7
                    ******************************/
                    QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                    if(number==Iso_){
                        continue;
                    }

                    if(ConS != number){
                        if(qAbs(sX4-ocrResult->pOCRText[ind].points[0].x)<((sX3-sX4)/2) && qAbs(sY4-ocrResult->pOCRText[ind].points[0].y)<(sY4-sY1)){
                            ConS.append(number);

                            if(ConS.size()>=10){
                                con_number=true;
                                break;
                            }
                            else {
                                sX1=ocrResult->pOCRText[ind].points[0].x;
                                sY1=ocrResult->pOCRText[ind].points[0].y;
                                sX2=ocrResult->pOCRText[ind].points[1].x;
                                sY2=ocrResult->pOCRText[ind].points[1].y;
                                sX3=ocrResult->pOCRText[ind].points[2].x;
                                sY3=ocrResult->pOCRText[ind].points[2].y;
                                sX4=ocrResult->pOCRText[ind].points[3].x;
                                sY4=ocrResult->pOCRText[ind].points[3].y;
                            }
                        }
                    }
                }
            }
            if(!con_number){
                for (int ind = 0; ind < cout; ind++)
                {
                    /*****************************
                    * @brief:水平箱号，（多行）
                    * WDFU
                    * 123     456 7
                    *-----------------------------
                    * WDFU
                    * 123
                    * 1234
                    ******************************/
                    QString number= QString::fromStdWString(reinterpret_cast<WCHAR*>(ocrResult->pOCRText[ind].ptext));
                    if(number==Iso_){
                        continue;
                    }

                    if(ConS != number){
                        if(qAbs(sX2-ocrResult->pOCRText[ind].points[0].x)<(sX2-sX1) && qAbs(sY2-ocrResult->pOCRText[ind].points[0].y)<((sY3-sY2)/2)){
                            ConS.append(number);
                            con_number=true;
                            if(ConS.size()>=10){
                                break;
                            }
                        }
                    }
                }
            }
        }

        ConS.remove(QRegExp("\\s"));
        ConS.remove(QRegExp("\\."));

        QString container="";
        for(int i=0;i<ConS.size();i++){
            if(i<4 && (ConS.at(i)>='A' && ConS.at(i)<='Z')){
                container.append(ConS.at(i));
            }
            if(i>=4 && (ConS.at(i)>='0' && ConS.at(i)<='9')){
                container.append(ConS.at(i));
            }
        }

        if(container.size()>=11){
            container=ConS.mid(0,11);
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(numberCheck(container),Iso,QString::number(CON_source*1000000),QString::number(ISO_source*1000000)));

        }
        else if(container.size()>=10){
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(numberCheck(container),Iso,QString::number(11000000),QString::number(11000000)));

        }
        else {
            emit signalDetectRst(image,QString("RESULT:%1|%2|%3|%4").arg(container,Iso,QString::number(CON_source*1000000),QString::number(ISO_source*1000000)));
        }

        /*****************************
        * @brief:释放识别结果对象
        ******************************/
        FreeDetectResult(ocrResult);
    }
    else {
        qWarning().noquote()<<QString("File does not exist, identification failed!");
        emit signalDetectRst(image,"RESULT:||0|0");
    }
}
