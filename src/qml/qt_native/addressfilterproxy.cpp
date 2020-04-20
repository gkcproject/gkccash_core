#include "addressfilterproxy.h"
#include "addresstablemodel.h"
#include "ui_interface.h"
#include "csvmodelwriter.h"
#include "guiutil.h"

AddressFilterProxy::AddressFilterProxy(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void AddressFilterProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    AddressTableModel::ColumnIndex ci;

    if(roleName == "address")
        ci = AddressTableModel::Address;
    else if(roleName == "label")
        ci = AddressTableModel::Label;

    sort(ci,order);
}

void AddressFilterProxy::exportClicked()
{
    QString filename = GUIUtil::getSaveFileName(NULL,
                                                tr("Export Address List"), QString(),
                                                tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(this);
    writer.addColumn("Label", AddressTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", AddressTableModel::Address, Qt::EditRole);


    if (writer.write()) {
        emit message(tr("Exporting Successful"), tr("File exported."),
                     CClientUIInterface::MSG_INFORMATION);
    }
    else {
        emit message(tr("Exporting Failed"), tr("There was an error trying to save the address list to %1. Please try again.").arg(filename),
                     CClientUIInterface::MSG_ERROR);
    }
}

QString AddressFilterProxy::saveAddress(int mode, QString label, QString address_,int row)
{
    QString address;
    AddressTableModel  *sourceModel_ = static_cast<AddressTableModel *> (sourceModel());

    switch (mode) {
    case 0://new receiving
    case 1://new sending
        address = sourceModel_->addRow(
            mode == 1 ? AddressTableModel::Send : AddressTableModel::Receive,
            label,
            address_);
        break;
    case 2://edit receiving
    case 3://edit sending
        sourceModel_->setData(sourceModel_->index(mapToSource(index(row,0)).row(),(int)AddressTableModel::Label),label,Qt::EditRole);
        sourceModel_->setData(sourceModel_->index(mapToSource(index(row,0)).row(),(int)AddressTableModel::Address),address_,Qt::EditRole);
        break;
    }

    if( (mode == 1 || mode == 0) && address.isEmpty())
    {
        return GetAddressStatus();
    }

    return "ok";
}

QString AddressFilterProxy::GetAddressStatus()
{
   AddressTableModel  *sourceModel_ = static_cast<AddressTableModel *> (sourceModel());

    switch (sourceModel_->getEditStatus()) {
    case AddressTableModel::OK:
        return tr("Failed with unknown reason.");
    case AddressTableModel::NO_CHANGES:
        return tr("No changes were made during edit operation.");
    case AddressTableModel::INVALID_ADDRESS:
        return tr("The entered address is not a valid GKC address.");
    case AddressTableModel::DUPLICATE_ADDRESS:
        return tr("The entered address is already in the address book.");
    case AddressTableModel::WALLET_UNLOCK_FAILURE:
        return tr("Could not unlock wallet.");
    case AddressTableModel::KEY_GENERATION_FAILURE:
        return tr("New key generation failed.");
    }

}

void AddressFilterProxy::deleteAddress(QVariantList rows)
{
    AddressTableModel  *sourceModel_ = static_cast<AddressTableModel *> (sourceModel());

    for(int i =0 ;i<rows.size();i++)
    {
        QModelIndex index_ = mapToSource(index(rows.at(i).toInt(),0));
        sourceModel_->removeRow(index_.row());
    }
}

QVariant AddressFilterProxy::getData(QString roleName, int sourceRow)
{
    if(roleName == "address"){
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(AddressTableModel::AddressRole).toString();
    }
    else
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(AddressTableModel::LabelRole).toString();
    }
}

QString AddressFilterProxy::updateLabel(const QString& address)
{
    AddressTableModel  *sourceModel_ = static_cast<AddressTableModel *>(sourceModel());
    return sourceModel_->labelForAddress(address);
}
