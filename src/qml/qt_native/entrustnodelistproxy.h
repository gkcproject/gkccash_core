#ifndef ENTRUSTNODELISTPROXY_H
#define ENTRUSTNODELISTPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class EntrustNodeListProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit EntrustNodeListProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);


private:
    
};

#endif 
