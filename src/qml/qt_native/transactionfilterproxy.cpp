// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactionfilterproxy.h"

#include "transactionrecord.h"
#include "transactiontablemodel.h"

#include <cstdlib>
#include <QDebug>
#include <QDateTime>

// Earliest date that can be represented (far in the past)
const QDateTime TransactionFilterProxy::MIN_DATE = QDateTime::fromTime_t(0);
// Last date that can be represented (far in the future)
const QDateTime TransactionFilterProxy::MAX_DATE = QDateTime::fromTime_t(0xFFFFFFFF);

TransactionFilterProxy::TransactionFilterProxy(QObject* parent) : QSortFilterProxyModel(parent),
                                                                  dateFrom(MIN_DATE),
                                                                  dateTo(MAX_DATE),
                                                                  addrPrefix(),
                                                                  typeFilter(COMMON_TYPES),
                                                                  watchOnlyFilter(WatchOnlyFilter_All),
                                                                  minAmount(0),
                                                                  limitRows(-1),
                                                                  showInactive(true)
{
}

bool TransactionFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    int type = index.data(TransactionTableModel::TypeRole).toInt();
    QDateTime datetime = index.data(TransactionTableModel::DateRole).toDateTime();
    bool involvesWatchAddress = index.data(TransactionTableModel::WatchonlyRole).toBool();
    QString address = index.data(TransactionTableModel::AddressRole).toString();
    QString label = index.data(TransactionTableModel::LabelRole).toString();
    qint64 amount = llabs(index.data(TransactionTableModel::AmountRole).toLongLong());
    int status = index.data(TransactionTableModel::StatusRole).toInt();


    if (!showInactive && status == TransactionStatus::Conflicted)
        return false;
    if (!(TYPE(type) & typeFilter))
        return false;
    if (involvesWatchAddress && watchOnlyFilter == WatchOnlyFilter_No)
        return false;
    if (!involvesWatchAddress && watchOnlyFilter == WatchOnlyFilter_Yes)
        return false;
    if (datetime < dateFrom || datetime > dateTo)
        return false;
    if (!address.contains(addrPrefix, Qt::CaseInsensitive) && !label.contains(addrPrefix, Qt::CaseInsensitive))
        return false;
    if (amount < minAmount)
        return false;

    return true;
}


int TransactionFilterProxy::getRow(QModelIndex index)
{
    return index.row();
}

void TransactionFilterProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    TransactionTableModel::ColumnIndex ci;

    if(roleName == "type")
        ci = TransactionTableModel::Type;
    else if(roleName == "date")
        ci = TransactionTableModel::Date;
    else if(roleName == "address")
        ci = TransactionTableModel::ToAddress;
    else if(roleName == "amount")
        ci = TransactionTableModel::Amount;
    else if(roleName == "status")
        ci = TransactionTableModel::Status;

    sort(ci,order);
}


QVariant TransactionFilterProxy::getData(int sourceRow,QString roleName)
{
    QModelIndex index_ = mapToSource(this->index(sourceRow,0));

    if(roleName == "type")
            return index_.data(TransactionTableModel::TypeRole).toInt();
    else if(roleName == "status_")
            return index_.data(TransactionTableModel::StatusRole_).toString();
    else if(roleName == "date_")
            return index_.data(TransactionTableModel::DateRole_).toString();
    else if(roleName == "amount__")
            return index_.data(TransactionTableModel::AmountRole__).toString();
    else if(roleName == "address__")
            return index_.data(TransactionTableModel::AddressRole__).toString();
    else if(roleName == "address___")
            return index_.data(TransactionTableModel::AddressRole___).toString();
    else if(roleName == "confirmed")
            return index_.data(TransactionTableModel::ConfirmedRole).toBool();
}

void TransactionFilterProxy::setDateRange(const QDateTime& from, const QDateTime& to)
{
    this->dateFrom = from;
    this->dateTo = to;
    invalidateFilter();
}

void TransactionFilterProxy::setAddressPrefix(const QString& addrPrefix)
{
    this->addrPrefix = addrPrefix;
    invalidateFilter();
}

void TransactionFilterProxy::setTypeFilter(quint32 modes)
{
    this->typeFilter = modes;
    invalidateFilter();
}

void TransactionFilterProxy::setMinAmount(const CAmount& minimum)
{
    this->minAmount = minimum;
    invalidateFilter();
}

void TransactionFilterProxy::setWatchOnlyFilter(WatchOnlyFilter filter)
{
    this->watchOnlyFilter = filter;
    invalidateFilter();
}

void TransactionFilterProxy::setLimit(int limit)
{
    this->limitRows = limit;
}

void TransactionFilterProxy::setShowInactive(bool showInactive)
{
    this->showInactive = showInactive;
    invalidateFilter();
}

int TransactionFilterProxy::rowCount(const QModelIndex& parent) const
{
    if (limitRows != -1)
    {
        return std::min(QSortFilterProxyModel::rowCount(parent), limitRows);
    }
    else
    {
        return QSortFilterProxyModel::rowCount(parent);
    }
}
