#include "mainnodelistproxy.h"
#include "mainnodelistmodel.h"
#include "bitcoinunits.h"
#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "guiutil.h"

#include <qrencode.h>

MainNodeListProxy::MainNodeListProxy(QObject *parent):QSortFilterProxyModel(parent)
{
    
}


void MainNodeListProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    MainNodeListModel::datatype ci;

    if(roleName == "address")
        ci = MainNodeListModel::Address;
    else if(roleName == "status")
        ci = MainNodeListModel::Status;
    else if(roleName == "version")
        ci = MainNodeListModel::Version;
	else if(roleName == "findtime")
        ci = MainNodeListModel::Findtime;
	else if(roleName == "runtime")
        ci = MainNodeListModel::Runtime;
    else if(roleName == "paymenttime")
        ci = MainNodeListModel::Paymenttime;

    sort(ci,order);
}