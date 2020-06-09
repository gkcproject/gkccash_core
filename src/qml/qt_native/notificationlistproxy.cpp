#include "notificationlistproxy.h"
#include "notificationlistmodel.h"
#include "bitcoinunits.h"
#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "guiutil.h"

#include <qrencode.h>

NotificationListProxy::NotificationListProxy(QObject *parent):QSortFilterProxyModel(parent)
{
    
}


void NotificationListProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    NotificationListModel::DataType ci;

    if(roleName == "title")
        ci = NotificationListModel::Title;
    else if(roleName == "text")
        ci = NotificationListModel::Text;
    else if(roleName == "time")
        ci = NotificationListModel::Time;
    else if(roleName == "block")
        ci = NotificationListModel::NBlock;
	else if(roleName == "link")
        ci = NotificationListModel::Link;
	else if(roleName == "author")
        ci = NotificationListModel::Author;
	else if(roleName == "hash")
        ci = NotificationListModel::NHash;

    sort(ci,order);
}