#ifndef ENTRUSTRECORDLISTPROXY_H
#define ENTRUSTRECORDLISTPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class EntrustRecordListProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit EntrustRecordListProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);


private:
    
};

#endif 