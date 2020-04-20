#ifndef TOKENFILTERPROXY_H
#define TOKENFILTERPROXY_H

#include <amount.h>
#include "tokentransactiontablemodel.h"
#include <QDateTime>
#include <QSortFilterProxyModel>
class Token;

#include <boost/multiprecision/cpp_int.hpp>
using namespace boost::multiprecision;

class TokenFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList tokenNameList MEMBER m_tokenNameList NOTIFY tokenListChanged)

public:
    explicit TokenFilterProxy(QObject *parent = 0);
    ~TokenFilterProxy();

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);

    Q_INVOKABLE QVariantList addressChanged(const QString &address);

    Q_INVOKABLE QVariantList confirmAdd(const QString &contractAddress,
                                      const QString &tokenName,
                                      const QString &tokenSymbol,
                                      const QString &decimals,
                                      const QString &senderAddress);


    Q_INVOKABLE void chooseDate(int idx);
    Q_INVOKABLE void dateRangeChanged(QString fromDate,QString toDate);
    Q_INVOKABLE void chooseType(int idx);
    Q_INVOKABLE void chooseName(const QString &name);
    Q_INVOKABLE void changedPrefix(const QString &prefix);
    Q_INVOKABLE void changedAmount(const QString &amount);
    Q_INVOKABLE void menuPicked(int tokenModelIndex,int menuIndex);
    Q_INVOKABLE QString getTxDescription(int tokenModelIndex);



    void setWalletModel(WalletModel *model);



    void setWallet(CWallet *wallet);

    /** Earliest date that can be represented (far in the past) */
    static const QDateTime MIN_DATE;
    /** Last date that can be represented (far in the future) */
    static const QDateTime MAX_DATE;
    /** Type filter bit field (all types) */

    static quint32 TYPE(int type) { return 1<<type; }

    void setDateRange(const QDateTime &from, const QDateTime &to);
    void setAddressPrefix(const QString &addrPrefix);
    /**
      @note Type filter takes a bit field created with TYPE() or ALL_TYPES
     */
    void setTypeFilter(quint32 modes);
    void setMinAmount(const int256_t& minimum);
    void setName(const QString _name);

    /** Set maximum number of rows returned, -1 if unlimited. */
    void setLimit(int limit);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const;

private:
    enum DateEnum {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

    quint32 ALL_TYPES = 0xFFFFFFFF;

    QDateTime dateFrom;
    QDateTime dateTo;
    QString addrPrefix;
    QString name;
    quint32 typeFilter;
    int256_t minAmount;
    int limitRows;
    bool showInactive;
    CWallet *m_wallet;
    WalletModel *walletModel;

    QStringList m_tokenNameList;

    Token *m_tokenABI;

    QList<quint32> typeList;
    QList<quint32> dateList;

    void refreshNameList();

signals:
    void tokenListChanged();

private slots:
    void removeFromNameList(const QModelIndex &parent, int start, int /*end*/);
    void addToNameList(const QModelIndex &parent, int start, int /*end*/);

};

#endif // TOKENFILTERPROXY_H
