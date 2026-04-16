#ifndef APPLOGIC_H
#define APPLOGIC_H

#include <QObject>

class AppLogic : public QObject
{
    Q_OBJECT
public:
    explicit AppLogic(QObject *parent = nullptr);

signals:
};

#endif // APPLOGIC_H
