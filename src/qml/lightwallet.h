#ifndef LIGHTWALLET_H
#define LIGHTWALLET_H

#include <QObject>


#include <QDebug>

class LightWallet : public QObject
{
    Q_OBJECT
public:
    explicit LightWallet(QObject *parent = nullptr);
    //CWallet *wallet = NULL;
    static bool fDaemon;

signals:

public slots:

private:
    int main2();


};

#endif // LIGHTWALLET_H
