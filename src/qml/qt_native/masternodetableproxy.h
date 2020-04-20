#ifndef MASTERNODETABLEPROXY_H
#define MASTERNODETABLEPROXY_H
#include <QSortFilterProxyModel>


class MasterNodeTableProxy : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MasterNodeTableProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);
    Q_INVOKABLE void updateMyNodeList(bool fForce);
    Q_INVOKABLE void startButtonClicked(int sourceRow);
    Q_INVOKABLE void startAll();
    Q_INVOKABLE void startMissing();
    Q_INVOKABLE void updateMyNodeList();


signals:
    void message(QString title, QString msg);
    void setTimer(int time);
};

#endif // MASTERNODETABLEPROXY_H
