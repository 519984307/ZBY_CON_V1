#ifndef IDENTIFY_H
#define IDENTIFY_H

#include <QObject>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QSharedPointer>

#include "paddleocr/include/Parameter.h"
#pragma comment (lib,"PaddleOCR.lib")

extern "C" {
    /// <summary>
    /// PaddleOCREngine引擎初始化
    /// </summary>
    /// <param name="det_infer"></param>
    /// <param name="cls_infer"></param>
    /// <param name="rec_infer"></param>
    /// <param name="keys"></param>
    /// <param name="parameter"></param>
    /// <returns></returns>
    __declspec(dllimport) int* Initialize(char* det_infer, char* cls_infer, char* rec_infer, char* keys, OCRParameter  parameter);

    /// <summary>
    /// 文本检测
    /// </summary>
    /// <param name="engine"></param>
    /// <param name="imagefile"></param>
    /// <param name="pOCRResult">返回结果</param>
    /// <returns></returns>
    __declspec(dllimport) int  Detect(int* engine, char* imagefile, LpOCRResult* pOCRResult);

    /// <summary>
    /// 释放引擎对象
    /// </summary>
    /// <param name="engine"></param>
    __declspec(dllimport) void FreeEngine(int* engine);

    /// <summary>
    /// 释放文本识别结果对象
    /// </summary>
    /// <param name="pOCRResult"></param>
    __declspec(dllimport) void FreeDetectResult(LpOCRResult pOCRResult);

    /// <summary>
    /// PaddleOCR检测
    /// </summary>
    /// <param name="det_infer"></param>
    /// <param name="imagefile"></param>
    /// <param name="parameter"></param>
    /// <returns></returns>
    __declspec(dllimport) void DetectImage(char* modelPath_det_infer, char* imagefile, OCRParameter parameter);
};

class Identify : public QObject
{
    Q_OBJECT
public:
    explicit Identify(QObject *parent = nullptr,const QString& infer_cls="NUL",const QString& infer_rec="NUL",const QString& infer_det="NUL");
    ~Identify();

public slots:

    ///
    /// \brief slotDetectImage 识别图片
    /// \param image
    ///
    void slotDetectImage(const QString& image);

signals:

    ///
    /// \brief signalDetectRst 识别结果
    /// \param rst
    ///
    void signalDetectRst(const QString& img, const QString& rst);

private:

    ///
    /// \brief pEngine 识别器初始化对象
    ///
    int* pEngine;

    ///
    /// \brief loadDictParment 加载匹配字典
    ///
    void loadDictParment();

    ///
    /// \brief initCheckMap 校验表
    ///
    void initCheckMap();

    ///
    /// \brief DecIMG 是被图片
    /// \param image
    /// \param pEngine
    ///
    void DecIMG(const QString& image,int* pEngine);

    ///
    /// \brief numberCheck 校验箱号
    /// \param number
    /// \return
    ///
    QString numberCheck(QString number);

    ///
    /// \brief computeQuadraticPower 计算平方
    /// \param variable
    /// \return
    ///
    int computeQuadraticPower(int variable);

protected:

    ///
    /// \brief CONDICT 箱主包含表
    ///
    QVector<QString> CONDICT;

    ///
    /// \brief _CONDICT 箱主排除表
    ///
    QVector<QString> _CONDICT;

    ///
    /// \brief ISODICT 箱型包含表
    ///
    QVector<QString> ISODICT;

    ///
    /// \brief COUNTRYDICT 国家代码包含表
    ///
    QVector<QString> COUNTRYDICT;

    ///
    /// \brief ISOMAP 箱型替换表
    ///
    QMap<QString,QString> ISOMAP;

    ///
    /// \brief COUNTRYMAP 国家代码替换表
    ///
    QMap<QString,QString> COUNTRYMAP;

    ///
    /// \brief CONMAP 箱号替换表
    ///
    QMap<QString,QString> CONMAP;

    ///
    /// \brief checkMap 校验列表
    ///
    QMap<QChar,int> checkMap;
};
#endif // IDENTIFY_H
