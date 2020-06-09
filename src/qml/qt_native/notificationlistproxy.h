#ifndef NOTIFICATIONLISTPROXYPROXY_H
#define NOTIFICATIONLISTPROXYPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class NotificationListProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit NotificationListProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);


private:
    
};

#endif 