#ifndef BITCOINCORE_H
#define BITCOINCORE_H

#include <QObject>
#include "bitcoinheaders.h"

class BitcoinCore : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinCore();

public slots:
    void initialize();
    void shutdown();
    void restart(QStringList args);

signals:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    void runawayException(const QString& message);

private:
    boost::thread_group threadGroup;

    /// Flag indicating a restart
    bool execute_restart;

    /// Pass fatal exception message to UI thread
    void handleRunawayException(std::exception* e);
};

#endif // BITCOINCORE_H


