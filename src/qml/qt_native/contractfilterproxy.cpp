#include "contractfilterproxy.h"
#include "contracttablemodel.h"
#include "guiutil.h"
#include "csvmodelwriter.h"
#include "ui_interface.h"


#include <script/standard.h>
#include <base58.h>
#include <wallet.h>

ContractFilterProxy::ContractFilterProxy(QObject *parent) : QSortFilterProxyModel(parent)
{
}

void ContractFilterProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    ContractTableModel::ColumnIndex ci;

    if(roleName == "label")
        ci = ContractTableModel::Label;
    else if(roleName == "address")
        ci = ContractTableModel::Address;
    else if(roleName == "abi")
        ci = ContractTableModel::ABI;

    sort(ci,order);
}

void ContractFilterProxy::exportClicked()
{
    QString filename = GUIUtil::getSaveFileName(NULL,
                                                tr("Export Contract List"), QString(),
                                                tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(this);
    writer.addColumn("Label", ContractTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", ContractTableModel::Address, Qt::EditRole);
    writer.addColumn("ABI", ContractTableModel::ABI, Qt::EditRole);


    if (writer.write()) {
        emit message(tr("Exporting Successful"), tr("File exported."),
                     CClientUIInterface::MSG_INFORMATION);
    }
    else {
        emit message(tr("Exporting Failed"), tr("There was an error trying to save the contract list to %1. Please try again.").arg(filename),
                     CClientUIInterface::MSG_ERROR);
    }
}

QString ContractFilterProxy::saveContract(int mode, const QString &label, const QString &contract,const QString &abiStr,int row )
{

    QString address;
    ContractTableModel  *sourceModel_ = static_cast<ContractTableModel *> (sourceModel());

    switch (mode) {
    case 0://new
        address = sourceModel_->addRow(
            label,
            contract,
            abiStr);
        break;
    case 1://edit
        sourceModel_->setData(sourceModel_->index(mapToSource(index(row,0)).row(),(int)ContractTableModel::Label),label,Qt::EditRole);
        sourceModel_->setData(sourceModel_->index(mapToSource(index(row,0)).row(),(int)ContractTableModel::Address),contract,Qt::EditRole);
        sourceModel_->setData(sourceModel_->index(mapToSource(index(row,0)).row(),(int)ContractTableModel::ABI),abiStr,Qt::EditRole);
        break;
    }

    if( (mode == 0) && address.isEmpty())
    {
        return GetAddressStatus();
    }

    return "ok";
}


QString ContractFilterProxy::GetAddressStatus()
{
   ContractTableModel  *sourceModel_ = static_cast<ContractTableModel *> (sourceModel());

    switch (sourceModel_->getEditStatus()) {
    case ContractTableModel::OK:
        return tr("Failed with unknown reason.");
    case ContractTableModel::NO_CHANGES:
        return tr("No changes were made during edit operation.");
    case ContractTableModel::DUPLICATE_ADDRESS:
        return tr("The entered address is already in the address book.");
    }

}



void ContractFilterProxy::deleteContract(int row)
{
    removeRows(row,1);
}

QVariant ContractFilterProxy::getData(QString roleName, int sourceRow)
{
    if(roleName == "address"){
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(ContractTableModel::AddressRole).toString();
    }
    else if(roleName == "label")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(ContractTableModel::LabelRole).toString();
    }
    else if(roleName == "abi")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(ContractTableModel::ABIRole).toString();
    }
}

QString ContractFilterProxy::updateABI(const QString& address)
{
    ContractTableModel  *sourceModel_ = static_cast<ContractTableModel *>(sourceModel());
    return sourceModel_->abiForAddress(address);
}


int ContractFilterProxy::lookupAddress(const QString &address) const
{
    ContractTableModel  *sourceModel_ = static_cast<ContractTableModel *>(sourceModel());
    return sourceModel_->lookupAddress(address);
}





//-----------------addressList----------------
void ContractFilterProxy::on_refresh()
{
    m_stringList.clear();
    vector<COutput> vecOutputs;

    assert(pwalletMain != NULL);

    {
        // Fill the list with UTXO
        LOCK2(cs_main, pwalletMain->cs_wallet);

        // Add all available addresses if 0 address ballance for token is enabled
        if(m_addressTableModel)
        {
            // Fill the list with user defined address
            for(int row = 0; row < m_addressTableModel->rowCount(); row++)
            {
                QModelIndex index = m_addressTableModel->index(row, AddressTableModel::Address);
                QString strAddress = m_addressTableModel->data(index,Qt::EditRole).toString();
                QString type = m_addressTableModel->data(index, AddressTableModel::TypeRole).toString();
                if(type == AddressTableModel::Receive)
                {
                    appendAddress(strAddress);
                }
            }

            // Include zero or unconfirmed coins too
            pwalletMain->AvailableCoins(vecOutputs, false, NULL, true);
        }
        else
        {
            // List only the spendable coins
            pwalletMain->AvailableCoins(vecOutputs);
        }

        for(const COutput& out : vecOutputs) {
            CTxDestination address;
            const CScript& scriptPubKey = out.tx->vout[out.i].scriptPubKey;
            bool fValidAddress = ExtractDestination(scriptPubKey, address);

            if (fValidAddress)
            {
                QString strAddress = QString::fromStdString(CBitcoinAddress(address).ToString());
                appendAddress(strAddress);
            }
        }

    }

    //TODO: may need to update the current picked address in qml

   emit addressListChanged();
}

void ContractFilterProxy::appendAddress(const QString &strAddress)
{
    CTxDestination address = CBitcoinAddress(strAddress.toStdString()).Get();

    if(!IsValidContractSenderAddress(address))
        return;

    if(!m_stringList.contains(strAddress) && IsMine(*pwalletMain, address))
    {
        m_stringList.append(strAddress);
    }

}

void ContractFilterProxy::setAddressTableModel(AddressTableModel *addreddTableModel)
{
    ContractTableModel  *sourceModel_ = static_cast<ContractTableModel *>(sourceModel());

    m_addressTableModel = addreddTableModel;

    if(m_addressTableModel)
    {
        disconnect(m_addressTableModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(on_refresh()));
        disconnect(m_addressTableModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(on_refresh()));
    }

    connect(m_addressTableModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(on_refresh()));
    connect(m_addressTableModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(on_refresh()));

    on_refresh();
}
