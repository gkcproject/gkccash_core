// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_TRAFFICGRAPHWIDGET_H
#define BITCOIN_QT_TRAFFICGRAPHWIDGET_H

//![0]
#include <QtQuick/QQuickPaintedItem>
#include <QColor>
#include <QQueue>

#include "bitcoinheaders.h"


#include <cmath>

#define DESIRED_SAMPLES 800

#define XMARGIN 10
#define YMARGIN 10

class TrafficGraphWidget : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(ClientModel* client_model READ getClientModel WRITE setClientModel)
    Q_PROPERTY(int range_min READ getGraphRangeMins WRITE setGraphRangeMins)

public:
    TrafficGraphWidget(QQuickItem *parent = 0);

    ClientModel *getClientModel();
    void setClientModel(ClientModel* model);

    int getGraphRangeMins() const;
    Q_INVOKABLE void setGraphRangeMins(int mins);

    QString name() const;
    void setName(const QString &name);

    QColor color() const;
    void setColor(const QColor &color);

    void paint(QPainter *painter);


    Q_INVOKABLE void clear();
public slots:
    void updateRates();

private:
    QString m_name;
    QColor m_color;

    void paintPath(QPainterPath& path, QQueue<float>& samples);

    QTimer* timer;
    float fMax;
    int nMins;
    QQueue<float> vSamplesIn;
    QQueue<float> vSamplesOut;
    quint64 nLastBytesIn;
    quint64 nLastBytesOut;

    ClientModel* clientModel;

};
//![0]

#endif // BITCOIN_QT_TRAFFICGRAPHWIDGET_H
