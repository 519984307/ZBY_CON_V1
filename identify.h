#ifndef IDENTIFY_H
#define IDENTIFY_H

#include <QObject>

class Identify : public QObject
{
    Q_OBJECT
public:
    explicit Identify(QObject *parent = nullptr);

signals:

};

#endif // IDENTIFY_H
