#include "masternodetableproxy.h"
#include "masternodetablemodel.h"

MasterNodeTableProxy::MasterNodeTableProxy(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

void MasterNodeTableProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    MasterNodeTableModel::ColumnIndex ci;

    if(roleName == "alias")
        ci = MasterNodeTableModel::Alias;
    else if(roleName == "address")
        ci = MasterNodeTableModel::Address;
    else if(roleName == "protocol")
        ci = MasterNodeTableModel::Protocol;
    else if(roleName == "status")
        ci = MasterNodeTableModel::Status;
    else if(roleName == "activated")
        ci = MasterNodeTableModel::Activated;
    else if(roleName == "lastseen")
        ci = MasterNodeTableModel::LastSeen;
    else if(roleName == "pubkey")
        ci = MasterNodeTableModel::Pubkey;

    sort(ci,order);
}

void MasterNodeTableProxy::updateMyNodeList(bool fForce)
{
    MasterNodeTableModel  *sourceModel_ = static_cast<MasterNodeTableModel *> (sourceModel());
    sourceModel_->updateMyNodeList(fForce);
}

void MasterNodeTableProxy::startButtonClicked(int sourceRow)
{
    QModelIndex index_ = mapToSource(this->index(sourceRow,0));
    MasterNodeTableModel  *sourceModel_ = static_cast<MasterNodeTableModel *> (sourceModel());
    sourceModel_->startButtonClicked(index_.row());
}

void MasterNodeTableProxy::startAll()
{
    MasterNodeTableModel  *sourceModel_ = static_cast<MasterNodeTableModel *> (sourceModel());
    sourceModel_->startAll();
}

void MasterNodeTableProxy::startMissing()
{
    MasterNodeTableModel  *sourceModel_ = static_cast<MasterNodeTableModel *> (sourceModel());
    sourceModel_->startMissing();
}

void MasterNodeTableProxy::updateMyNodeList()
{
    MasterNodeTableModel  *sourceModel_ = static_cast<MasterNodeTableModel *> (sourceModel());
    sourceModel_->updateMyNodeList(true);
}
