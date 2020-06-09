#include "tokenfilterproxy.h"

#include <cstdlib>

#include <QDateTime>
#include "token.h"
#include "wallet.h"
#include "walletmodel.h"
#include "tokenitemmodel.h"
#include "tokentransactionrecord.h"


// Earliest date that can be represented (far in the past)
const QDateTime TokenFilterProxy::MIN_DATE = QDateTime::fromTime_t(0);
// Last date that can be represented (far in the future)
const QDateTime TokenFilterProxy::MAX_DATE = QDateTime::fromTime_t(0xFFFFFFFF);

int256_t abs_int256(const int256_t& value)
{
    return value > 0 ? value : -value;
}

TokenFilterProxy::TokenFilterProxy(QObject *parent) :
    QSortFilterProxyModel(parent),
    dateFrom(MIN_DATE),
    dateTo(MAX_DATE),
    addrPrefix(),
    name(),
    typeFilter(ALL_TYPES),
    minAmount(0),
    limitRows(-1)
{

    m_tokenABI = new Token();

    dateList.push_back( All);
    dateList.push_back( Today);
    dateList.push_back( ThisWeek);
    dateList.push_back( ThisMonth);
    dateList.push_back( LastMonth);
    dateList.push_back( ThisYear);
    dateList.push_back( Range);

    typeList.push_back(ALL_TYPES);
    typeList.push_back(TokenFilterProxy::TYPE(TokenTransactionRecord::RecvWithAddress));
    typeList.push_back(TokenFilterProxy::TYPE(TokenTransactionRecord::SendToAddress));
    typeList.push_back(TokenFilterProxy::TYPE(TokenTransactionRecord::SendToSelf));


}


TokenFilterProxy::~TokenFilterProxy()
{

    if(m_tokenABI)
        delete m_tokenABI;
    m_tokenABI = 0;

}

void TokenFilterProxy::setWalletModel(WalletModel *model)
{
    walletModel = model;
    m_tokenNameList.append("All");
    refreshNameList();

    connect(model->getTokenItemModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),this, SLOT(addToNameList(QModelIndex,int,int)));
    connect(model->getTokenItemModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this, SLOT(removeFromNameList(QModelIndex,int,int)));

}

void TokenFilterProxy::addToNameList(const QModelIndex &parent, int start, int /*end*/)
{
    if(walletModel)
    {
        TokenItemModel *tim = walletModel->getTokenItemModel();
        QString name = tim->index(start, TokenItemModel::Symbol, parent).data().toString();
        if(m_tokenNameList.indexOf(name) == -1)
            m_tokenNameList.append(name);

        emit tokenListChanged();

    }
}

void TokenFilterProxy::removeFromNameList(const QModelIndex &parent, int start, int /*end*/)
{
    if(walletModel)
    {
        TokenItemModel *tim = walletModel->getTokenItemModel();
        QString name = tim->index(start, TokenItemModel::Symbol, parent).data().toString();
        int nameCount = 0;

        for(int i = 0; i < tim->rowCount(); i++)
        {
            QString checkName = tim->index(i, TokenItemModel::Symbol, parent).data().toString();
            if(name == checkName)
            {
                nameCount++;
            }
        }

        int nameIndex = m_tokenNameList.indexOf(name);
        if(nameCount == 1 && nameIndex != -1)
            m_tokenNameList.removeAll(name);

        emit tokenListChanged();
    }
}


void TokenFilterProxy::chooseDate(int idx)
{
    if (!walletModel)
        return;

    QDate current = QDate::currentDate();
    // dateRangeWidget->setVisible(false);

    switch (dateList.at(idx)) {
    case All:
        setDateRange(
                    TokenFilterProxy::MIN_DATE,
                    TokenFilterProxy::MAX_DATE);
        break;
    case Today:
        setDateRange(
                    QDateTime(current),
                    TokenFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek() - 1));
        setDateRange(
                    QDateTime(startOfWeek),
                    TokenFilterProxy::MAX_DATE);

    } break;
    case ThisMonth:
        setDateRange(
                    QDateTime(QDate(current.year(), current.month(), 1)),
                    TokenFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        setDateRange(
                    QDateTime(QDate(current.year(), current.month() - 1, 1)),
                    QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        setDateRange(
                    QDateTime(QDate(current.year(), 1, 1)),
                    TokenFilterProxy::MAX_DATE);
        break;
        //    case Range:
        //        dateRangeWidget->setVisible(true);
        //        dateRangeChanged();
        //        break;
    }
    //    // Persist settings
    //    if (dateList.at(idx) != Range) {
    //        QSettings settings;
    //        settings.setValue("transactionDate", idx);
    //    }
}

void TokenFilterProxy::dateRangeChanged(QString fromDate,QString toDate)
{
    setDateRange(QDateTime::fromString(fromDate,"yyyy-MM-dd"),
                 QDateTime::fromString(toDate,"yyyy-MM-dd").addDays(1));
}


void TokenFilterProxy::setWallet(CWallet *wallet)
{
    m_wallet = wallet;
}

void TokenFilterProxy::menuPicked(int tokenModelIndex,int menuIndex)
{
    QModelIndex index_ = mapToSource(this->index(tokenModelIndex,0));

    switch (menuIndex) {
    case 0:
        walletModel->setClipBoard(index_.data(TokenTransactionTableModel::AddressRole_).toString());
        break;
    case 1:
        walletModel->setClipBoard(index_.data(TokenTransactionTableModel::AmountRole_).toString());
        break;
    case 2:
        walletModel->setClipBoard(index_.data(TokenTransactionTableModel::TxHashRole).toString());
        break;
    case 3:
        walletModel->setClipBoard(index_.data(TokenTransactionTableModel::LongDescriptionRole).toString());
        break;
    default:
        break;
    }

}

QString TokenFilterProxy::getTxDescription(int tokenModelIndex)
{
    QModelIndex index_ = mapToSource(this->index(tokenModelIndex,0));
    return index_.data(TokenTransactionTableModel::LongDescriptionRole).toString();
}


void TokenFilterProxy::setDateRange(const QDateTime &from, const QDateTime &to)
{
    this->dateFrom = from;
    this->dateTo = to;
    invalidateFilter();
}

void TokenFilterProxy::chooseType(int idx)
{
    setTypeFilter(
        typeList.at(idx));
}

void TokenFilterProxy::changedPrefix(const QString &prefix)
{
    setAddressPrefix(prefix);
}

void TokenFilterProxy::setAddressPrefix(const QString &_addrPrefix)
{
    this->addrPrefix = _addrPrefix;
    invalidateFilter();
}

void TokenFilterProxy::setTypeFilter(quint32 modes)
{
    this->typeFilter = modes;
    invalidateFilter();
}

void TokenFilterProxy::changedAmount(const QString &amount)
{
    int256_t amount_parsed = 0;
    if(BitcoinUnits::parseToken(18, amount, &amount_parsed))
    {
        setMinAmount(amount_parsed);
    }
    else
    {
        setMinAmount(0);
    }
}

void TokenFilterProxy::setMinAmount(const int256_t &minimum)
{
    this->minAmount = minimum;
    invalidateFilter();
}

void TokenFilterProxy::chooseName(const QString &name)
{
    setName(name);
}

void TokenFilterProxy::setName(const QString _name)
{
    this->name = _name;
	if(_name=="All")
		this->name = "";
    invalidateFilter();
}

void TokenFilterProxy::setLimit(int limit)
{
    this->limitRows = limit;
}

int TokenFilterProxy::rowCount(const QModelIndex &parent) const
{
    if(limitRows != -1)
    {
        return std::min(QSortFilterProxyModel::rowCount(parent), limitRows);
    }
    else
    {
        return QSortFilterProxyModel::rowCount(parent);
    }
}


void TokenFilterProxy::refreshNameList()
{
    TokenItemModel *tim = walletModel->getTokenItemModel();
    for(int i = 0; i < tim->rowCount(); i++)
    {
        QString name = tim->data(tim->index(i, 0), TokenItemModel::SymbolRole).toString();
        if(m_tokenNameList.indexOf(name) == -1)
            m_tokenNameList.append(name);
    }

    emit tokenListChanged();
}

QVariantList TokenFilterProxy::addressChanged(const QString &address)
{

    m_tokenABI->setAddress(address.toStdString());
    std::string name, symbol, decimals;
    bool ret = m_tokenABI->name(name);
    ret &= m_tokenABI->symbol(symbol);
    ret &= m_tokenABI->decimals(decimals);


    if(!ret) return QVariantList();

    QVariantList returnList = {QString::fromStdString(name), QString::fromStdString(symbol), QString::fromStdString(decimals)  };
    return returnList;
}

QVariantList TokenFilterProxy::confirmAdd(const QString &contractAddress,
                                          const QString &tokenName,
                                          const QString &tokenSymbol,
                                          const QString &decimals,
                                          const QString &senderAddress)
{
    QVariantList returnList = QVariantList();

    CTokenInfo tokenInfo;
    tokenInfo.strContractAddress = contractAddress.toStdString();
    tokenInfo.strTokenName = tokenName.toStdString();
    tokenInfo.strTokenSymbol = tokenSymbol.toStdString();
    tokenInfo.nDecimals = decimals.toUInt();
    tokenInfo.strSenderAddress = senderAddress.toStdString();


    if(walletModel)
    {
        if(!walletModel->isMineAddress(tokenInfo.strSenderAddress))
        {
            QString symbol = QString::fromStdString(tokenInfo.strTokenSymbol);
            QString address = QString::fromStdString(tokenInfo.strSenderAddress);
            QString message = tr("The %1 address \"%2\" is not yours, please change it to new one.\n").arg(symbol, address);
            returnList.append("Invalid token address");
            returnList.append(message);
        }
        else if(walletModel->existTokenEntry(tokenInfo))
        {
            returnList.append("Token exist");
            returnList.append("The token already exist with the specified contract and sender addresses.");
        }
        else
        {
            walletModel->addTokenEntry(tokenInfo);

            if(!fLogEvents)
            {
                returnList.append("Log events");
                returnList.append("Enable log events from the option menu in order to receive token transactions.");
            }
        }
    }

    return returnList;
}


void TokenFilterProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    TokenTransactionTableModel::ColumnIndex ci;

    if(roleName == "date")
        ci = TokenTransactionTableModel::Date;
    else if(roleName == "label")
        ci = TokenTransactionTableModel::ToAddress;
    else if(roleName == "type")
        ci = TokenTransactionTableModel::Type;
    else if(roleName == "name")
        ci = TokenTransactionTableModel::Name;
    else if(roleName == "amount")
        ci = TokenTransactionTableModel::Amount;

    sort(ci,order);
}

bool TokenFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    int type = index.data(TokenTransactionTableModel::TypeRole).toInt();
    QDateTime datetime = index.data(TokenTransactionTableModel::DateRole).toDateTime();
    QString address = index.data(TokenTransactionTableModel::AddressRole).toString();
    int256_t amount(index.data(TokenTransactionTableModel::AmountRole).toString().toStdString());
    amount = abs_int256(amount);
    QString tokenName = index.data(TokenTransactionTableModel::NameRole).toString();
	//std::cout << "111111111111111amount:" << amount << "\n";
	//std::cout << "222222222222222name:" << name.toStdString() << "\n";
    if(!(TYPE(type) & typeFilter))
        return false;
    if(datetime < dateFrom || datetime > dateTo)
        return false;
    if (!address.contains(addrPrefix, Qt::CaseInsensitive))
        return false;
    if(amount < minAmount)
        return false;
    if(!name.isEmpty() && name != tokenName)
        return false;

    return true;
}

bool TokenFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if(left.column() == TokenTransactionTableModel::Amount &&
            right.column() == TokenTransactionTableModel::Amount)
    {
        int256_t amountLeft(left.data(TokenTransactionTableModel::AmountRole).toString().toStdString());
        amountLeft = abs_int256(amountLeft);

        int256_t amountRight(right.data(TokenTransactionTableModel::AmountRole).toString().toStdString());
        amountRight = abs_int256(amountRight);

        return amountLeft < amountRight;
    }
    return QSortFilterProxyModel::lessThan(left, right);
}
