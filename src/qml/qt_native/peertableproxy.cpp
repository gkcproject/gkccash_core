#include "peertableproxy.h"
#include "guiutil.h"
PeerTableProxy::PeerTableProxy(QObject *parent) : QSortFilterProxyModel(parent)
{

}


void PeerTableProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    PeerTableModel::ColumnIndex ci;
    PeerTableModel  *sourceModel_ = static_cast<PeerTableModel *> (sourceModel());

    if(roleName == "address")
        ci = PeerTableModel::Address;
    else if(roleName == "version")
        ci = PeerTableModel::Subversion;
    else if(roleName == "ping")
        ci = PeerTableModel::Ping;


    if(!sourceModel_) return;
        sourceModel_->sort(ci,order);
}

QString PeerTableProxy::getPeerTitleInfo(int row)
{
    PeerTableModel  *sourceModel_ = static_cast<PeerTableModel *> (sourceModel());

    QModelIndex index_ = mapToSource(index(row,0));


    const CNodeCombinedStats* stats = sourceModel_->getNodeStats(index_.row());
    if (stats)
    {
        QString peerAddrTitles(QString::fromStdString(stats->nodeStats.addrName));
        if (!stats->nodeStats.addrLocal.empty())
            return  peerAddrTitles += "<br />" + tr("via %1").arg(QString::fromStdString(stats->nodeStats.addrLocal));

    }

    return "";
}


QString PeerTableProxy::getPeerDetailedInfo(int row)
{
    QModelIndex index_ = mapToSource(index(row,0));

    PeerTableModel  *sourceModel_ = static_cast<PeerTableModel *> (sourceModel());

    const CNodeCombinedStats* stats = sourceModel_->getNodeStats(index_.row());


    if (stats)
    {

        QString peerAddrDetails = "";

        peerAddrDetails +=  stats->nodeStats.fInbound ? tr("Inbound") + "\n" : tr("Outbound") + "\n";
        peerAddrDetails +=  QString("%1").arg(stats->nodeStats.nVersion) + "\n";
        peerAddrDetails += QString::fromStdString(stats->nodeStats.cleanSubVer) + "\n";
        peerAddrDetails +=  GUIUtil::formatServicesStr(stats->nodeStats.nServices) + "\n";
        peerAddrDetails +=  QString("%1").arg(stats->nodeStats.nStartingHeight) + "\n";

        if (stats->fNodeStateStatsAvailable) {

            if (stats->nodeStateStats.nSyncHeight > -1)
                peerAddrDetails +=  QString("%1").arg(stats->nodeStateStats.nSyncHeight) + "\n";
            else
                peerAddrDetails += "Unknown\n";

            peerAddrDetails += QString("%1").arg(stats->nodeStateStats.nMisbehavior) + "\n";

        }
        else {
            peerAddrDetails += "Fetching...\n";
            peerAddrDetails += "Fetching...\n";

        }

        peerAddrDetails += GUIUtil::formatDurationStr(GetTime() - stats->nodeStats.nTimeConnected) + "\n";
        peerAddrDetails += stats->nodeStats.nLastSend ? GUIUtil::formatDurationStr(GetTime() - stats->nodeStats.nLastSend) + "\n" : tr("never") + "\n";
        peerAddrDetails += stats->nodeStats.nLastRecv ? GUIUtil::formatDurationStr(GetTime() - stats->nodeStats.nLastRecv) + "\n" : tr("never") + "\n";
        peerAddrDetails += FormatBytes(stats->nodeStats.nSendBytes) + "\n";
        peerAddrDetails += FormatBytes(stats->nodeStats.nRecvBytes) + "\n";
        peerAddrDetails += GUIUtil::formatPingTime(stats->nodeStats.dPingTime) + "\n";


        return peerAddrDetails;


    }

    return "";

}

QString PeerTableProxy::FormatBytes(quint64 bytes)
{
    if (bytes < 1024)
        return QString(tr("%1 B")).arg(bytes);
    if (bytes < 1024 * 1024)
        return QString(tr("%1 KB")).arg(bytes / 1024);
    if (bytes < 1024 * 1024 * 1024)
        return QString(tr("%1 MB")).arg(bytes / 1024 / 1024);

    return QString(tr("%1 GB")).arg(bytes / 1024 / 1024 / 1024);
}


void PeerTableProxy::startAutoRefresh()
{
    PeerTableModel  *sourceModel_ = static_cast<PeerTableModel *> (sourceModel());
    sourceModel_->startAutoRefresh();
}

void PeerTableProxy::stopAutoRefresh()
{
    PeerTableModel  *sourceModel_ = static_cast<PeerTableModel *> (sourceModel());
    sourceModel_->stopAutoRefresh();
}
