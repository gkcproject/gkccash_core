#include "coincontrolproxy.h"
#include "coincontrol.h"
#include "coincontrolmodel.h"

CoinControlProxy::CoinControlProxy(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}


QVariantList CoinControlProxy::updateView(QVariantList payAmountList)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    return sourceModel_->updateView(payAmountList);
}

void CoinControlProxy::setData_(int sourceRow, QString value,int mode)
{
    QModelIndex index_ = mapToSource(this->index(sourceRow,0));
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());


    if(mode == 0)
        sourceModel_->setData(index_, value, CoinControlModel::StatusRole);
    else
        sourceModel_->setData(index_, value, CoinControlModel::SelectionRole);
}


void CoinControlProxy::selectAll()
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->selectAll();
}

void CoinControlProxy::toggle()
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->toggle();
}


void CoinControlProxy::updateSendingPage()
{
    if(CoinControlModel::coinControl->HasSelected())
        emit showCoinControl(true);
    else
        emit showCoinControl(false);
}

void CoinControlProxy::updateSplitUtxo(bool checked, const QString &utxo, const QString &afterFee)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->updateSplitUtxo(checked,utxo,afterFee);
}

QString CoinControlProxy::updatecustomChangeAddress(bool checked,const QString &address)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    return sourceModel_->updatecustomChangeAddress(checked,address);
}


void CoinControlProxy::setValue(int index, QVariant value)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->setValue(index,value);
}

void CoinControlProxy::setEnabled(bool enable)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->setEnabled(enable);
}


void CoinControlProxy::sendButtonClicked(QVariantList recipientsArray,
                                         bool splitBlockCheckBoxChecked,
                                         int splitBlockSize,
                                         bool swiftTXChecked)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    sourceModel_->sendButtonClicked(recipientsArray,
                                    splitBlockCheckBoxChecked,
                                    splitBlockSize,
                                    swiftTXChecked);
}


void CoinControlProxy::confirmSending()
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    return sourceModel_->confirmSending();
}

QVariantList CoinControlProxy::updateCoinControlLabels(QVariantList payAmountList)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    return sourceModel_->updateCoinControlLabels(payAmountList);
}

QVariant CoinControlProxy::getValue(int index)
{
    CoinControlModel  *sourceModel_ = static_cast<CoinControlModel *> (sourceModel());
    return sourceModel_->getValue(index);
}


QVariant CoinControlProxy::getData(QString roleName, int sourceRow)
{
    if(roleName == "txid"){
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::TxHashRole).toString();
    }
    else if(roleName == "label")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::LabelRole).toString();
    }
    else if(roleName == "amount")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::AmountRole).toString();
    }

    else if(roleName == "address")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(CoinControlModel::AddressRole).toString();
    }
}


void CoinControlProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    CoinControlModel::ColumnIndex ci;

    if(roleName == "type")
        ci = CoinControlModel::Type;
    else if(roleName == "date")
        ci = CoinControlModel::Date;
    else if(roleName == "address")
        ci = CoinControlModel::Address;
    else if(roleName == "amount")
        ci = CoinControlModel::Amount;
    else if(roleName == "status")
        return;
    else if(roleName == "priority")
        ci = CoinControlModel::Priority;
    else if(roleName == "label")
        ci = CoinControlModel::Label;
    else if(roleName == "confirmations")
        ci = CoinControlModel::Confirmations;
    sort(ci,order);
}
