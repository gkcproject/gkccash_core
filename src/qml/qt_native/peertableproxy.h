#ifndef PEERTABLEPROXY_H
#define PEERTABLEPROXY_H

#include <QSortFilterProxyModel>
#include "peertablemodel.h"

class PeerTableProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    PeerTableProxy(QObject *parent);


    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);

    Q_INVOKABLE QString getPeerTitleInfo(int row);
    Q_INVOKABLE QString getPeerDetailedInfo(int row);
    Q_INVOKABLE void startAutoRefresh();
    Q_INVOKABLE void stopAutoRefresh();

private:
    QString FormatBytes(quint64 bytes);

};

#endif // PEERTABLEPROXY_H
