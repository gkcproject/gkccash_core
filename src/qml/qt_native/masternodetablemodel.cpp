#include "masternodetablemodel.h"

#include "activemasternode.h"
#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "masternode-sync.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "sync.h"
#include "wallet.h"
#include "walletmodel.h"

MasterNodeTableModel::MasterNodeTableModel(WalletModel* parent):model(parent)
{

}



void MasterNodeTableModel::setClientModel(ClientModel* clientmodel_)
{
    clientModel = clientmodel_;
}



int MasterNodeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return nodes.size();
}

int MasterNodeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QModelIndex MasterNodeTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(nodes.size() <= row)
        return QModelIndex();

    return createIndex(row, column, nullptr);

}

void MasterNodeTableModel::updateMyMasternodeInfo(QString strAlias, QString strAddr, CMasternode* pmn)
{
    LOCK(cs_mnlistupdate);

    nodeItem thisNode;

    thisNode.aliasItem = strAlias;
    thisNode.addrItem = pmn ? QString::fromStdString(pmn->addr.ToString()) : strAddr;
    thisNode.protocolItem = QString::number(pmn ? pmn->protocolVersion : -1);
    thisNode.statusItem = QString::fromStdString(pmn ? pmn->GetStatus() : "MISSING");
    thisNode.activeSecondsItem = QString::fromStdString(DurationToDHMS(pmn ? (pmn->lastPing.sigTime - pmn->sigTime) : 0));
    thisNode.lastSeenItem = QString::fromStdString(DateTimeStrFormat("%Y-%m-%d %H:%M", pmn ? pmn->lastPing.sigTime : 0));
    thisNode.pubkeyItem = QString::fromStdString(pmn ? CBitcoinAddress(pmn->pubKeyCollateralAddress.GetID()).ToString() : "");

    nodes.push_back(thisNode);
}

void MasterNodeTableModel::updateMyNodeList(bool fForce)
{
    static int64_t nTimeMyListUpdated = 0;

    beginRemoveRows(QModelIndex(), 0, nodes.size() - 1);
    nodes.clear();
    endRemoveRows();

    // automatically update my masternode list only once in MY_MASTERNODELIST_UPDATE_SECONDS seconds,
    // this update still can be triggered manually at any time via button click
    int64_t nSecondsTillUpdate = nTimeMyListUpdated + MY_MASTERNODELIST_UPDATE_SECONDS - GetTime();
    //Emit signal to  notufy timer change

    emit setTimer(nSecondsTillUpdate);

    if (nSecondsTillUpdate > 0 && !fForce) return;
    nTimeMyListUpdated = GetTime();

    BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        int nIndex;
        if(!mne.castOutputIndex(nIndex))
            continue;

        CTxIn txin = CTxIn(uint256S(mne.getTxHash()), uint32_t(nIndex));
        CMasternode* pmn = mnodeman.Find(txin);
        updateMyMasternodeInfo(QString::fromStdString(mne.getAlias()), QString::fromStdString(mne.getIp()), pmn);
    }

    beginInsertRows(QModelIndex(), 0, nodes.size() - 1);

    endInsertRows();

    // reset "timer"
    emit setTimer(60);


}




QVariant MasterNodeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    nodeItem thisItem = nodes.at(index.row());

    switch (role) {
    case Qt::EditRole:
        // Edit role is used for sorting, so return the unformatted values
        switch (index.column()) {
        case Alias:
            return thisItem.aliasItem;
        case Address:
            return thisItem.addrItem;
        case Protocol:
            return thisItem.protocolItem;
        case Status:
            return thisItem.statusItem;
        case Activated:
            return thisItem.activeSecondsItem;
        case LastSeen:
            return thisItem.lastSeenItem;
        case Pubkey:
            return thisItem.pubkeyItem;

        }
        break;
    case AliasRole:
        return thisItem.aliasItem;
    case AddressRole:
        return thisItem.addrItem;
    case ProtocolRole:
        return thisItem.protocolItem;
    case StatusRole:
        return thisItem.statusItem;
    case ActivatedRole:
        return thisItem.activeSecondsItem;
    case LastSeenRole:
        return thisItem.lastSeenItem;
    case PubkeyRole:
        return thisItem.pubkeyItem;

    }
    return QVariant();

}





QHash<int, QByteArray> MasterNodeTableModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[AliasRole] = "alias";
    roles[AddressRole] = "address";
    roles[ProtocolRole] = "protocol";
    roles[StatusRole] = "status";
    roles[ActivatedRole] = "activated";
    roles[LastSeenRole] = "lastseen";
    roles[PubkeyRole] = "pubkey";

    return roles;
}

void MasterNodeTableModel::startMissing()
{
    if (!masternodeSync.IsMasternodeListSynced())
    {
        emit message(tr("Command is not available right now"),  tr("You can't use this command until masternode list is synced"));
        return;
    }

    WalletModel::EncryptionStatus encStatus = (WalletModel::EncryptionStatus)(model->getEncryptionStatus());

    if (encStatus == model->Locked || encStatus == model->UnlockedForAnonymizationOnly) {
        WalletModel::UnlockContext ctx(model->requestUnlock());

        if (!ctx.isValid()) return; // Unlock wallet was cancelled

        startAll("start-missing");
        return;
    }

    startAll("start-missing");
}



void MasterNodeTableModel::startAll(QString strCommand)
{
    int nCountSuccessful = 0;
    int nCountFailed = 0;
    std::string strFailedHtml;

    BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        std::string strError;
        CMasternodeBroadcast mnb;

        int nIndex;
        if(!mne.castOutputIndex(nIndex))
            continue;

        CTxIn txin = CTxIn(uint256S(mne.getTxHash()), uint32_t(nIndex));
        CMasternode* pmn = mnodeman.Find(txin);

        if (strCommand == "start-missing" && pmn) continue;

        bool fSuccess = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb);

        if (fSuccess) {
            nCountSuccessful++;
            mnodeman.UpdateMasternodeList(mnb);
            mnb.Relay();
        } else {
            nCountFailed++;
            strFailedHtml += "\nFailed to start " + mne.getAlias() + ". Error: " + strError;
        }
    }
    //pwalletMain->Lock();

    std::string returnObj;
    returnObj = strprintf("Successfully started %d masternodes, failed to start %d, total %d", nCountSuccessful, nCountFailed, nCountFailed + nCountSuccessful);
    if (nCountFailed > 0) {
        returnObj += strFailedHtml;
    }

    emit message(tr("Result"), QString::fromStdString(returnObj));

    updateMyNodeList(true);

}


void MasterNodeTableModel::startButtonClicked(int row)
{
	if(nodes.size()<=row)
	{
		return;
	}
    std::string strAlias = nodes.at(row).aliasItem.toStdString();
    WalletModel::EncryptionStatus encStatus = (WalletModel::EncryptionStatus)(model->getEncryptionStatus());

    if (encStatus == model->Locked || encStatus == model->UnlockedForAnonymizationOnly) {
        WalletModel::UnlockContext ctx(model->requestUnlock());

        if (!ctx.isValid()) return; // Unlock wallet was cancelled

        startAlias(strAlias);
        return;
    }

    startAlias(strAlias);
}

void MasterNodeTableModel::startAlias(std::string strAlias)
{
    std::string strStatusHtml;
    strStatusHtml += "<center>Alias: " + strAlias;

    BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        if (mne.getAlias() == strAlias) {
            std::string strError;
            CMasternodeBroadcast mnb;


            bool fSuccess = activeMasternode.Register(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError);

           // bool fSuccess = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb);

            if (fSuccess) {
                strStatusHtml += "<br>Successfully started masternode.";
                mnodeman.UpdateMasternodeList(mnb);
                mnb.Relay();
            }         //Emit signal to  notufy timer restart
            else {
                strStatusHtml += "<br>Failed to start masternode.<br>Error: " + strError;
            }
            break;
        }
    }
    strStatusHtml += "</center>";

    emit message(tr("Result"), QString::fromStdString(strStatusHtml));


    updateMyNodeList(true);
}
