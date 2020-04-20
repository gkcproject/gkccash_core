#ifndef MAINNODELISTPROXY_H
#define MAINNODELISTPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class MainNodeListProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit MainNodeListProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);


private:
    
};

#endif 
