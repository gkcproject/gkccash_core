#include "entrustnodelistproxy.h"
#include "entrustnodelistmodel.h"
#include "bitcoinunits.h"
#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "guiutil.h"

#include <qrencode.h>

EntrustNodeListProxy::EntrustNodeListProxy(QObject *parent):QSortFilterProxyModel(parent)
{
    
}


void EntrustNodeListProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    EntrustNodeListModel::datatype ci;

    if(roleName == "nodeid")
        ci = EntrustNodeListModel::NodeID;
    else if(roleName == "entrust")
        ci = EntrustNodeListModel::Entrust;
    else if(roleName == "usernum")
        ci = EntrustNodeListModel::UserNum;
	else if(roleName == "totaldealdivide")
        ci = EntrustNodeListModel::TotalDealDivide;
	else if(roleName == "totaldividend")
        ci = EntrustNodeListModel::TotalDividend;

    sort(ci,order);
}