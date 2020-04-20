#ifndef CONTRACTFILTERPROXY_H
#define CONTRACTFILTERPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>

class CWallet;
class AddressTableModel;

extern CWallet* pwalletMain;

class ContractFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QStringList addressList MEMBER m_stringList NOTIFY addressListChanged)

public:
    explicit ContractFilterProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);
    Q_INVOKABLE QString saveContract(int mode,const QString &label, const QString &contract,const QString &abiStr,int row = -1);
    Q_INVOKABLE void deleteContract(int row);
    Q_INVOKABLE QVariant getData(QString roleName, int sourceRow);
    Q_INVOKABLE void exportClicked();
    Q_INVOKABLE QString updateABI(const QString& address);
    Q_INVOKABLE int lookupAddress(const QString &address) const;


    void SetUseZeroBalance(bool useOrNot)
    {
        useZeroBalance = useOrNot;
    }

    void setAddressTableModel(AddressTableModel *addreddTableModel);


signals:
    void message(const QString& title, const QString& message, unsigned int style);
    void addressListChanged();

private slots:
    void on_refresh();

private:


    QStringList m_stringList;

    bool useZeroBalance = false;

    QString GetAddressStatus();
    void appendAddress(const QString &strAddress);
    AddressTableModel* m_addressTableModel;
};

#endif // CONTRACTFILTERPROXY_H
