#ifndef BITCOINGUI_H
#define BITCOINGUI_H

#include <QObject>

class BitcoinGUI : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinGUI(QObject *parent = nullptr);

signals:

public slots:
};

#endif // BITCOINGUI_H