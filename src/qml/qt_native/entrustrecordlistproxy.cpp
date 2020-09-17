#include "entrustrecordlistproxy.h"
#include "entrustrecordlistmodel.h"
#include "bitcoinunits.h"
#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "guiutil.h"

#include <qrencode.h>

EntrustRecordListProxy::EntrustRecordListProxy(QObject *parent):QSortFilterProxyModel(parent)
{
    
}


void EntrustRecordListProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    EntrustRecordListModel::DataType ci;

    if(roleName == "txid")
        ci = EntrustRecordListModel::TxID;
    else if(roleName == "vout")
        ci = EntrustRecordListModel::Vout;
    else if(roleName == "agent")
        ci = EntrustRecordListModel::AgentHash;
    else if(roleName == "amount")
        ci = EntrustRecordListModel::Amount;
	else if(roleName == "agentname")
        ci = EntrustRecordListModel::AgentName;

    sort(ci,order);
}