#ifndef ADDRESSFILTERPROXY_H
#define ADDRESSFILTERPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class AddressFilterProxy :public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AddressFilterProxy(QObject *parent = nullptr);


    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);
    Q_INVOKABLE QString saveAddress(int mode, QString label, QString address_, int row = -1);
    Q_INVOKABLE void deleteAddress(QVariantList rows);
    Q_INVOKABLE QVariant getData(QString roleName, int sourceRow);
    Q_INVOKABLE QString updateLabel(const QString& address);
    Q_INVOKABLE void exportClicked();

signals:
    void message(const QString& title, const QString& message, unsigned int style);

public slots:

private:
    QString GetAddressStatus();

};

#endif // ADDRESSFILTERPROXY_H
