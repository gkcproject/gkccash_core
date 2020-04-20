#ifndef WALLETMANAGER_H
#define WALLETMANAGER_H

#include <QObject>

class WalletManager : public QObject
{
    Q_OBJECT
public:
    explicit WalletManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WALLETMANAGER_H