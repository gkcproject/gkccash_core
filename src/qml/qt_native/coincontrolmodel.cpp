#include "coincontrolmodel.h"
#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "guiutil.h"
#include "init.h"
#include "optionsmodel.h"

#include "coincontrol.h"
#include "obfuscation.h"
#include "wallet.h"
#include "utilmoneystr.h"

#include <QSettings>


QList<CAmount> CoinControlModel::payAmounts;
int CoinControlModel::nSplitBlockDummy;
CCoinControl* CoinControlModel::coinControl = new CCoinControl();

CoinControlModel::CoinControlModel(WalletModel* parent, bool MultisigEnabled) :
    QAbstractListModel(parent),
    model(parent),
    fMultisigEnabled(MultisigEnabled)
{
    QSettings settings;
    if (!settings.contains("bUseObfuScation"))
        settings.setValue("bUseObfuScation", false);
    if (!settings.contains("bUseSwiftTX"))
        settings.setValue("bUseSwiftTX", false);
    if (!settings.contains("fFeeSectionMinimized"))
        settings.setValue("fFeeSectionMinimized", true);
    if (!settings.contains("nFeeRadio") && settings.contains("nTransactionFee") && settings.value("nTransactionFee").toLongLong() > 0) // compatibility
        settings.setValue("nFeeRadio", 1);     //custom                                                                                        // custom
    if (!settings.contains("nFeeRadio"))
        settings.setValue("nFeeRadio", 0);                                                                                                   // recommended
    if (!settings.contains("nCustomFeeRadio") && settings.contains("nTransactionFee") && settings.value("nTransactionFee").toLongLong() > 0) // compatibility
        settings.setValue("nCustomFeeRadio", 1);                                                                                             // total at least
    if (!settings.contains("nCustomFeeRadio"))
        settings.setValue("nCustomFeeRadio", 0); // per kilobyte
    if (!settings.contains("nSmartFeeSliderPosition"))
        settings.setValue("nSmartFeeSliderPosition", 0);
    if (!settings.contains("nTransactionFee"))
        settings.setValue("nTransactionFee", (qint64)DEFAULT_TRANSACTION_FEE);
    if (!settings.contains("fPayOnlyMinFee"))
        settings.setValue("fPayOnlyMinFee", false);
    if (!settings.contains("fSendFreeTransactions"))
        settings.setValue("fSendFreeTransactions", false);


}

QVariant CoinControlModel::getValue(int index)
{
    QSettings settings;
    switch(index)
    {
    case 0:
        return settings.value("fFeeSectionMinimized").toBool();
    case 1:
        return settings.value("nFeeRadio").toInt();
    case 2:
        return settings.value("nCustomFeeRadio").toInt();
    case 3:
        return settings.value("nSmartFeeSliderPosition").toInt();
    case 4:
        return settings.value("nTransactionFee").toLongLong();
    case 5:
        return settings.value("fPayOnlyMinFee").toBool();
    case 6:
        return  settings.value("fSendFreeTransactions").toBool();
    }
}

void CoinControlModel::setEnabled(bool enable)
{
    if (!enable && model) // coin control features disabled
        CoinControlModel::coinControl->SetNull();

    if (enable)
        emit updateCoinControlLabelsSig();

}



void CoinControlModel::setValue(int index, QVariant value)
{
    QSettings settings;
    bool needUpdateSmartFeeLabel = false;
    bool needUpdateGlobalFeeVariables = false;
    bool needCoinControlUpdateLabels = false;



    switch(index)
    {
    case 0:
        settings.setValue("fFeeSectionMinimized", value.toBool());
        if(value.toBool())
            updateSmartFeeLabel();
        break;
    case 1:
        settings.setValue("nFeeRadio", value.toInt());
        needCoinControlUpdateLabels = true;
        needUpdateGlobalFeeVariables = true;
        break;
    case 2:
        settings.setValue("nCustomFeeRadio", value.toInt());
        needCoinControlUpdateLabels = true;
        needUpdateGlobalFeeVariables = true;
        break;
    case 3:
        settings.setValue("nSmartFeeSliderPosition", value.toInt());
        needUpdateSmartFeeLabel = true;
        needCoinControlUpdateLabels = true;
        needUpdateGlobalFeeVariables = true;
        break;
    case 4:
        settings.setValue("nTransactionFee", value.toLongLong());
        needCoinControlUpdateLabels = true;
        needUpdateGlobalFeeVariables = true;
        break;
    case 5:
        settings.setValue("fPayOnlyMinFee", value.toBool());
        needCoinControlUpdateLabels = true;
        needUpdateGlobalFeeVariables = true;
        break;
    case 6:
        settings.setValue("fSendFreeTransactions", value.toBool());
        break;
    }


    if(needUpdateSmartFeeLabel)
        updateSmartFeeLabel();

    if(needCoinControlUpdateLabels)
        emit updateCoinControlLabelsSig();

    if(needUpdateGlobalFeeVariables)
        updateGlobalFeeVariables();

}

void CoinControlModel::updateGlobalFeeVariables()
{
    QSettings settings;

    if (settings.value("nFeeRadio").toInt() == 0) {
        nTxConfirmTarget = (int)25 - (int)std::max(0, std::min(24, settings.value("nSmartFeeSliderPosition").toInt()));
        payTxFee = CFeeRate(0);
    } else {
        nTxConfirmTarget = 25;
        payTxFee = CFeeRate((CAmount)(settings.value("nTransactionFee").toLongLong()));
        fPayAtLeastCustomFee = (settings.value("nCustomFeeRadio").toInt() == 1);
    }

    fSendFreeTransactions = settings.value("fSendFreeTransactions").toBool();
}


void CoinControlModel::updateSmartFeeLabel()
{
    QVariantList labelList;
    QSettings settings;


    int nBlocksToConfirm = (int)25 - (int)std::max(0, std::min(24, settings.value("nSmartFeeSliderPosition").toInt()));
    CFeeRate feeRate = mempool.estimateFee(nBlocksToConfirm);
    if (feeRate <= CFeeRate(0)) // not enough data => minfee
    {
        labelList.append(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), CWallet::minTxFee.GetFeePerK()) + "/kB");
        labelList.append(tr("Smart fee not initialized yet. This usually takes a few blocks..."));
        labelList.append("");
    } else {
        labelList.append(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), feeRate.GetFeePerK()) + "/kB");
        labelList.append("");
        labelList.append(tr("Estimated to begin confirmation within %n block(s).", "", nBlocksToConfirm));
    }


    if (settings.value("nFeeRadio").toInt() == 0)
        labelList.append(labelList.at(0));
    else {
        labelList.append(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), settings.value("nTransactionFee").toLongLong()) +
                         ((settings.value("nCustomFeeRadio").toInt() == 0) ? "/kB" : ""));
    }

    emit updateSmartFeeLabels(labelList);
}

QVariantList CoinControlModel::updateCoinControlLabels(QVariantList payAmountList)
{
    CoinControlModel::payAmounts.clear();

    for(int i = 0;i<payAmountList.size();i++)
        CoinControlModel::payAmounts.append(payAmountList[i].toLongLong());

    if (CoinControlModel::coinControl->HasSelected())
    {
        // actual coin control calculation
        return CoinControlModel::updateLabelsFunc(model,mapSelection);
    }

    return QVariantList();
}


int CoinControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return vecOuts.size();
}

int CoinControlModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QModelIndex CoinControlModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(vecOuts.size() <= row)
        return QModelIndex();

    return createIndex(row, column, nullptr);

}

QVariantList CoinControlModel::updateLabelsFunc(WalletModel* model, std::map<QString,bool> &mapSelection)
{
    if (!model)
        return QVariantList();

    // nPayAmount
    CAmount nPayAmount = 0;
    bool fDust = false;
    CMutableTransaction txDummy;
    foreach (const CAmount& amount, CoinControlModel::payAmounts) {
        nPayAmount += amount;

        if (amount > 0) {
            CTxOut txout(amount, (CScript)vector<unsigned char>(24, 0));
            txDummy.vout.push_back(txout);
            if (txout.IsDust(::minRelayTxFee))
                fDust = true;
        }
    }

    QString sPriorityLabel = tr("none");
    CAmount nAmount = 0;
    CAmount nPayFee = 0;
    CAmount nAfterFee = 0;
    CAmount nChange = 0;
    unsigned int nBytes = 0;
    unsigned int nBytesInputs = 0;
    double dPriority = 0;
    double dPriorityInputs = 0;
    unsigned int nQuantity = 0;
    int nQuantityUncompressed = 0;
    bool fAllowFree = false;

    vector<COutPoint> vCoinControl;
    vector<COutput> vOutputs;
    coinControl->ListSelected(vCoinControl);
    model->getOutputs(vCoinControl, vOutputs);

    BOOST_FOREACH (const COutput& out, vOutputs) {
        // unselect already spent, very unlikely scenario, this could happen
        // when selected are spent elsewhere, like rpc or another computer
        uint256 txhash = out.tx->GetHash();
        COutPoint outpt(txhash, out.i);
        if (model->isSpent(outpt)) {
            coinControl->UnSelect(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
            continue;
        }

        // Quantity
        nQuantity++;

        // Amount
        nAmount += out.tx->vout[out.i].nValue;

        // Priority
        dPriorityInputs += (double)out.tx->vout[out.i].nValue * (out.nDepth + 1);

        // Bytes
        CTxDestination address;
        if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, address)) {
            CPubKey pubkey;
            CKeyID* keyid = boost::get<CKeyID>(&address);
            if (keyid && model->getPubKey(*keyid, pubkey)) {
                nBytesInputs += (pubkey.IsCompressed() ? 148 : 180);
                if (!pubkey.IsCompressed())
                    nQuantityUncompressed++;
            } else
                nBytesInputs += 148; // in all error cases, simply assume 148 here
        } else
            nBytesInputs += 148;
    }

    // calculation
    if (nQuantity > 0) {
        // Bytes
        nBytes = nBytesInputs + ((CoinControlModel::payAmounts.size() > 0 ? CoinControlModel::payAmounts.size() + max(1, CoinControlModel::nSplitBlockDummy) : 2) * 34) + 10; // always assume +1 output for change here

        // Priority
        double mempoolEstimatePriority = mempool.estimatePriority(nTxConfirmTarget);
        dPriority = dPriorityInputs / (nBytes - nBytesInputs + (nQuantityUncompressed * 29)); // 29 = 180 - 151 (uncompressed public keys are over the limit. max 151 bytes of the input are ignored for priority)
        sPriorityLabel = CoinControlModel::getPriorityLabel(dPriority, mempoolEstimatePriority);

        // Fee
        nPayFee = CWallet::GetMinimumFee(nBytes, nTxConfirmTarget, mempool);

        // IX Fee
        if (coinControl->useSwiftTX) nPayFee = max(nPayFee, CENT);
        // Allow free?
        double dPriorityNeeded = mempoolEstimatePriority;
        if (dPriorityNeeded <= 0)
            dPriorityNeeded = AllowFreeThreshold(); // not enough data, back to hard-coded
        fAllowFree = (dPriority >= dPriorityNeeded);

        if (fSendFreeTransactions)
            if (fAllowFree && nBytes <= MAX_FREE_TRANSACTION_CREATE_SIZE)
                nPayFee = 0;

        if (nPayAmount > 0) {
            nChange = nAmount - nPayFee - nPayAmount;

            // Never create dust outputs; if we would, just add the dust to the fee.
            if (nChange > 0 && nChange < CENT) {
                CTxOut txout(nChange, (CScript)vector<unsigned char>(24, 0));
                if (txout.IsDust(::minRelayTxFee)) {
                    nPayFee += nChange;
                    nChange = 0;
                }
            }

            if (nChange == 0)
                nBytes -= 34;
        }

        // after fee
        nAfterFee = nAmount - nPayFee;
        if (nAfterFee < 0)
            nAfterFee = 0;
    }

    // actually update labels
    int nDisplayUnit = BitcoinUnits::GKC;
    if (model && model->getOptionsModel())
        nDisplayUnit = model->getOptionsModel()->getDisplayUnit();

    QVariantList returnList;
    bool needReverse = nPayFee > 0 && !(payTxFee.GetFeePerK() > 0 && fPayAtLeastCustomFee && nBytes < 1000);
    // stats
    returnList.append(QString::number(nQuantity));// Quantity
    returnList.append(BitcoinUnits::formatWithUnit(nDisplayUnit, nAmount));// Amount
    returnList.append(needReverse?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nPayFee)):BitcoinUnits::formatWithUnit(nDisplayUnit, nPayFee));// Fee
    returnList.append(needReverse?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nAfterFee)):BitcoinUnits::formatWithUnit(nDisplayUnit, nAfterFee));// After Fee
    returnList.append(((nBytes > 0) ? "~" : "") + QString::number(nBytes));// Bytes
    returnList.append(sPriorityLabel);// Priority
    returnList.append(fDust ? tr("yes") : tr("no"));// Dust
    returnList.append((needReverse && (nChange > 0))?("~" + BitcoinUnits::formatWithUnit(nDisplayUnit, nChange)):BitcoinUnits::formatWithUnit(nDisplayUnit, nChange));// Change


    return returnList;


}


bool CoinControlModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    QString status = value.toString();
    COutput  out =  vecOuts.at(index.row()).first;
    COutPoint outpt(out.tx->GetHash(), out.i);

    if (role == CoinControlModel::StatusRole)
    {
        if(status == "1")
        {
            model->lockCoin(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
            coinControl->UnSelect(outpt);
        }
        else
        {
            model->unlockCoin(outpt);
        }



    }
    else if (role == CoinControlModel::SelectionRole)
    {
        if(status == "1")
        {
            mapSelection[QString::fromStdString(out.ToString())] = true;
            coinControl->Select(outpt);

        }
        else
        {
            mapSelection[QString::fromStdString(out.ToString())] = false;
            coinControl->UnSelect(outpt);
        }

    }




    emit dataChanged(index, index);
    emit updateCoinControlLabelsSig();

}

QVariant CoinControlModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QString sWalletAddress = vecOuts.at(index.row()).second;
    COutput  out =  vecOuts.at(index.row()).first;
    isminetype mine = pwalletMain->IsMine(out.tx->vout[out.i]);
    bool fMultiSigUTXO = (mine & ISMINE_MULTISIG);
    int nInputSize = 0;

    int nDisplayUnit = model->getOptionsModel()->getDisplayUnit();
    double mempoolEstimatePriority = mempool.estimatePriority(nTxConfirmTarget);


    CTxDestination outputAddress;
    QString sAddress = "";
    if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, outputAddress)) {
        sAddress = QString::fromStdString(CBitcoinAddress(outputAddress).ToString());

        CPubKey pubkey;
        CKeyID* keyid = boost::get<CKeyID>(&outputAddress);
        if (keyid && model->getPubKey(*keyid, pubkey) && !pubkey.IsCompressed())
            nInputSize = 29; // 29 = 180 - 151 (public key is 180 bytes, priority free area is 151 bytes)
    }

    double dPriority = ((double)out.tx->vout[out.i].nValue / (nInputSize + 78)) * (out.nDepth + 1);

    QString sLabel;
    if (!(sAddress == sWalletAddress))
    {
        sLabel = tr("(change)");
    }
    else
    {
        sLabel = model->getAddressTableModel()->labelForAddress(sAddress);
        if (sLabel.isEmpty())
            sLabel = tr("(no label)");
    }

    bool selected;


    if(mapSelection.find(QString::fromStdString(out.ToString())) != mapSelection.end())
        selected = mapSelection.at(QString::fromStdString(out.ToString()));
    else
        selected = false;

    bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);
    QString confirmationStr = strPad(QString::number(out.nDepth), 8, QString(" "));

    QString statusStr;

    if(selected)
        statusStr = "1";
    else
        statusStr = "0";

    if(locked)
        statusStr += "|1";
    else
        statusStr += "|0";



    switch (role) {

    case Qt::EditRole:
        // Edit role is used for sorting, so return the unformatted values
        switch (index.column()) {
        case Amount:
            return (qint64)(out.tx->vout[out.i].nValue);
        case Date:
            return (qint64)(out.tx->GetTxTime());
        case Type:
            return fMultiSigUTXO ? "MultiSig" : "Personal";
        case Label:
            return sLabel;
        case Address:
            return sAddress;
        case Confirmations:
            return QString::number(out.nDepth);
        case Priority:
            return dPriority;

        }
        break;

    case AmountRole:
        return BitcoinUnits::format(nDisplayUnit, out.tx->vout[out.i].nValue);
    case TxHashRole:
        return QString::fromStdString(out.tx->GetHash().GetHex());
    case LabelRole:
        return sLabel;
    case AddressRole:
        return sAddress;
    case TypeRole:
        return fMultiSigUTXO ? "MultiSig" : "Personal";
    case DateRole:
        return GUIUtil::dateTimeStr(out.tx->GetTxTime());
    case ConfirmedRole:
        return confirmationStr;
    case PriorityRole:
        return CoinControlModel::getPriorityLabel(dPriority, mempoolEstimatePriority);
    case StatusRole:
        return statusStr;

    }
    return QVariant();

}

QString CoinControlModel::getPriorityLabel(double dPriority, double mempoolEstimatePriority)
{
    double dPriorityMedium = mempoolEstimatePriority;

    if (dPriorityMedium <= 0)
        dPriorityMedium = AllowFreeThreshold(); // not enough data, back to hard-coded

    if (dPriority / 1000000 > dPriorityMedium)
        return tr("highest");
    else if (dPriority / 100000 > dPriorityMedium)
        return tr("higher");
    else if (dPriority / 10000 > dPriorityMedium)
        return tr("high");
    else if (dPriority / 1000 > dPriorityMedium)
        return tr("medium-high");
    else if (dPriority > dPriorityMedium)
        return tr("medium");
    else if (dPriority * 10 > dPriorityMedium)
        return tr("low-medium");
    else if (dPriority * 100 > dPriorityMedium)
        return tr("low");
    else if (dPriority * 1000 > dPriorityMedium)
        return tr("lower");
    else
        return tr("lowest");
}

QString CoinControlModel::strPad(QString s, int nPadLength, QString sPadding) const
{
    while (s.length() < nPadLength)
        s = sPadding + s;

    return s;
}

QHash<int, QByteArray> CoinControlModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[StatusRole] = "status";
    roles[AmountRole] = "amount";
    roles[LabelRole] = "label";
    roles[AddressRole] = "address";
    roles[TypeRole] = "type";
    roles[DateRole] = "date";
    roles[ConfirmedRole] = "confirmations";
    roles[PriorityRole] = "priority";
    roles[TxHashRole] = "txid";

    return roles;
}


void CoinControlModel::selectAll()
{
    bool hasSelected = false;

    for(int row = 0 ;row< vecOuts.size();row++)
    {
        COutput  out =  vecOuts.at(row).first;

        if(mapSelection[QString::fromStdString(out.ToString())])
        {
            hasSelected = true;
            break;
        }

    }

    for(int row = 0 ;row< vecOuts.size();row++){

        COutput  out =  vecOuts.at(row).first;
        COutPoint outpt(out.tx->GetHash(), out.i);

        bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);

        if(!locked)
        {
            if(hasSelected)
            {
                mapSelection[QString::fromStdString(out.ToString())] = false;
                coinControl->UnSelect(outpt);
            }
            else
            {
                mapSelection[QString::fromStdString(out.ToString())] = true;
                coinControl->Select(outpt);

            }

        }


    }


    emit dataChanged(index(0,0), index(vecOuts.size()-1,0));
    emit updateCoinControlLabelsSig();
}

void CoinControlModel::toggle()
{
    for(int row = 0 ;row< vecOuts.size();row++){

        COutput  out =  vecOuts.at(row).first;
        COutPoint outpt(out.tx->GetHash(), out.i);
        bool locked =  model->isLockedCoin(out.tx->GetHash(), out.i);

        if(locked)
        {
            model->unlockCoin(outpt);
        }
        else
        {
            model->lockCoin(outpt);
            mapSelection[QString::fromStdString(out.ToString())] = false;
            coinControl->UnSelect(outpt);

        }

    }

    emit dataChanged(index(0,0), index(vecOuts.size()-1,0));
    emit updateCoinControlLabelsSig();

}

QVariantList CoinControlModel::updateView(QVariantList payAmountList)
{
    beginRemoveRows(QModelIndex(), 0, vecOuts.size() - 1);
    mapCoins.clear();
    vecOuts.clear();
    model->listCoins(mapCoins);
    endRemoveRows();

    BOOST_FOREACH (PAIRTYPE(QString, vector<COutput>) coins, mapCoins)
    {

        QString sWalletAddress = coins.first;


        for(const COutput& out: coins.second)
        {
            isminetype mine = pwalletMain->IsMine(out.tx->vout[out.i]);
            bool fMultiSigUTXO = (mine & ISMINE_MULTISIG);
            // when multisig is enabled, it will only display outputs from multisig addresses            if (fMultisigEnabled && !fMultiSigUTXO)
            if (fMultisigEnabled && !fMultiSigUTXO)
                continue;

            vecOuts.push_back(make_pair(out,sWalletAddress));
        }

    }
    beginInsertRows(QModelIndex(), 0, vecOuts.size() - 1);

    endInsertRows();

    return  updateCoinControlLabels(payAmountList);

}

void CoinControlModel::updateSplitUtxo(bool checked,const QString &utxo,const QString &afterFee)
{

    CoinControlModel::coinControl->fSplitBlock = checked;
    fSplitBlock = checked;

    if(!checked)
    {
        return;
    }

    QString qAfterFee = afterFee.left(afterFee.indexOf(" ")).replace("~", "").simplified().replace(" ", "");

    //convert to CAmount
    CAmount nAfterFee;
    ParseMoney(qAfterFee.toStdString().c_str(), nAfterFee);

    //if greater than 0 then divide after fee by the amount of blocks
    CAmount nSize = nAfterFee;
    int nBlocks = utxo.toInt();
    if (nAfterFee && nBlocks)
        nSize = nAfterFee / nBlocks;

    //assign to split block dummy, which is used to recalculate the fee amount more outputs
    CoinControlModel::nSplitBlockDummy = nBlocks;

    //update labels
    emit updateLabelBlockSize(QString::fromStdString(FormatMoney(nSize)));


}


QString CoinControlModel::updatecustomChangeAddress(bool checked,const QString &address)
{
    CoinControlModel::coinControl->destChange = CNoDestination();

    if(!checked) return "";

    CBitcoinAddress addr = CBitcoinAddress(address.toStdString());

    if (address.isEmpty()) // Nothing entered
    {
        return "empty";
    } else if (!addr.IsValid()) // Invalid address
    {
        return "Invalid address";
    } else // Valid address
    {
        CPubKey pubkey;
        CKeyID keyid;
        addr.GetKeyID(keyid);
        if (!model->getPubKey(keyid, pubkey)) // Unknown change address
        {
            return "Unknown change address";
        } else // Known change address
        {

            // Query label
            QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
            CoinControlModel::coinControl->destChange = addr.Get();

            if (!associatedLabel.isEmpty())
                return associatedLabel;
            else
                return tr("(no label)");


        }
    }

}



void CoinControlModel::sendButtonClicked(QVariantList recipientsArray,
                                         bool splitBlockCheckBoxChecked,
                                         int splitBlockSize,
                                         bool swiftTXChecked)
{
    QList<SendCoinsRecipient> recipients;

    for (int i = 0; i < recipientsArray.count(); ++i) {

        QVariantList thisRecipient = recipientsArray.at(i).toList();

        //UTXO splitter - address should be our own
        CBitcoinAddress address = thisRecipient.at(0).toString().toStdString();
        if (!model->isMine(address) && splitBlockCheckBoxChecked) {
            CoinControlModel::coinControl->fSplitBlock = false;
            emit notifySendingResult(0,tr("Send Coins"),tr("The split block tool does not work when sending to outside addresses. Try again."));
            return;
        }

        SendCoinsRecipient recipient;
        recipient.address = thisRecipient.at(0).toString();
        recipient.label = thisRecipient.at(1).toString();
        recipient.amount = thisRecipient.at(2).toLongLong();

        recipient.message = thisRecipient.at(3).toString();
        recipients.append(recipient);
    }

    //set split block in model
    CoinControlModel::coinControl->fSplitBlock = splitBlockCheckBoxChecked;

    if (recipientsArray.count() > 1 && splitBlockCheckBoxChecked) {
        CoinControlModel::coinControl->fSplitBlock = false;
        emit notifySendingResult(0,tr("Send Coins"),tr("The split block tool does not work with multiple addresses. Try again."));
        return;
    }

    if (CoinControlModel::coinControl->fSplitBlock)
        CoinControlModel::coinControl->nSplitBlock = int(splitBlockSize);

    QString strFunds = tr("using") + " <b>" + tr("anonymous funds") + "</b>";
    QString strFee = "";
    recipients[0].inputType = ALL_COINS;
    strFunds = tr("using") + " <b>" + tr("any available funds (not recommended)") + "</b>";

    if (swiftTXChecked) {
        recipients[0].useSwiftTX = true;
        strFunds += " ";
        strFunds += tr("and SwiftX");
    } else {
        recipients[0].useSwiftTX = false;
    }


    // Format confirmation message
    QStringList formatted;
    foreach (const SendCoinsRecipient& rcp, recipients) {
        // generate bold amount string
        QString amount = "<b>" + BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), rcp.amount);
        amount.append("</b> ").append(strFunds);

        // generate monospace address string
        QString address = "<span style='font-family: monospace;'>" + rcp.address;
        address.append("</span>");

        QString recipientElement;

        if (!rcp.paymentRequest.IsInitialized()) // normal payment
        {
            if (rcp.label.length() > 0) // label with address
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
                recipientElement.append(QString(" (%1)").arg(address));
            } else // just address
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }
        } else if (!rcp.authenticatedMerchant.isEmpty()) // secure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
        } else // insecure payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, address);
        }

        if (fSplitBlock) {
            recipientElement.append(tr(" split into %1 outputs using the UTXO splitter.").arg(CoinControlModel::coinControl->nSplitBlock));
        }

        formatted.append(recipientElement);
    }

    fNewRecipientAllowed = false;

    // request unlock only if was locked or unlocked for mixing:
    // this way we let users unlock by walletpassphrase or by menu
    // and make many transactions while unlocking through this dialog
    // will call relock
    WalletModel::EncryptionStatus encStatus = (WalletModel::EncryptionStatus)(model->getEncryptionStatus());
    if (encStatus == model->Locked || encStatus == model->UnlockedForAnonymizationOnly) {
        WalletModel::UnlockContext ctx(model->requestUnlock(true));
        if (!ctx.isValid()) {
            // Unlock wallet was cancelled
            fNewRecipientAllowed = true;
            return;
        }
        send(recipients, strFee, formatted);
        return;
    }
    // already unlocked or not encrypted at all
    send(recipients, strFee, formatted);
}


void CoinControlModel::processSendCoinsReturn(const WalletModel::SendCoinsReturn& sendCoinsReturn, const QString& msgArg, bool fPrepare)
{
    bool fAskForUnlock = false;

    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendCoinsDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch (sendCoinsReturn.status) {
    case WalletModel::InvalidAddress:
        msgParams.first = tr("The recipient address is not valid, please recheck.");
        break;
    case WalletModel::InvalidAmount:
        msgParams.first = tr("The amount to pay must be larger than 0.");
        break;
    case WalletModel::AmountExceedsBalance:
        msgParams.first = tr("The amount exceeds your balance.");
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
        break;
    case WalletModel::DuplicateAddress:
        msgParams.first = tr("Duplicate address found, can only send to each address once per send operation.");
        break;
    case WalletModel::TransactionCreationFailed:
        msgParams.first = tr("Transaction creation failed!");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::TransactionCommitFailed:
        msgParams.first = tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::AnonymizeOnlyUnlocked:
        // Unlock is only need when the coins are send
        if(!fPrepare)
            fAskForUnlock = true;
        else
            msgParams.first = tr("Error: The wallet was unlocked only to anonymize coins.");
        break;

    case WalletModel::InsaneFee:
        msgParams.first = tr("A fee %1 times higher than %2 per kB is considered an insanely high fee.").arg(10000).arg(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), ::minRelayTxFee.GetFeePerK()));
        break;
        // included to prevent a compiler warning.
    case WalletModel::OK:
    default:
        return;
    }

    // Unlock wallet if it wasn't fully unlocked already
    if(fAskForUnlock) {
        model->requestUnlock(false);
        if(model->getEncryptionStatus () != WalletModel::Unlocked) {
            msgParams.first = tr("Error: The wallet was unlocked only to anonymize coins. Unlock canceled.");
        }
        else {
            // Wallet unlocked
            return;
        }
    }

    emit notifySendingResult(-1,tr("Send Coins"), msgParams.first);
}

void CoinControlModel::send(QList<SendCoinsRecipient> recipients, QString strFee, QStringList formatted)
{
    // prepare transaction for getting txFee earlier
    if(currentTransaction) delete currentTransaction;

    currentTransaction = new WalletModelTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;
    if (model->getOptionsModel()->getCoinControlFeatures()) // coin control enabled
        prepareStatus = model->prepareTransaction(*currentTransaction, CoinControlModel::coinControl);
    else
        prepareStatus = model->prepareTransaction(*currentTransaction);

    // process prepareStatus and on error generate message shown to user
    processSendCoinsReturn(prepareStatus,
                           BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction->getTransactionFee()), true);

    if (prepareStatus.status != WalletModel::OK) {
        fNewRecipientAllowed = true;
        return;
    }

    CAmount txFee = currentTransaction->getTransactionFee();
    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if (txFee > 0) {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("are added as transaction fee"));
        questionString.append(" ");
        questionString.append(strFee);

        // append transaction size
        questionString.append(" (" + QString::number((double)(currentTransaction->getTransactionSize()) / 1000) + " kB)");
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    CAmount totalAmount = currentTransaction->getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    foreach (BitcoinUnits::Unit u, BitcoinUnits::availableUnits()) {
        if (u != model->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcoinUnits::formatHtmlWithUnit(u, totalAmount));
    }

    // Show total amount + all alternative units
    questionString.append(tr("Total Amount = <b>%1</b><br />= %2")
                          .arg(BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), totalAmount))
                          .arg(alternativeUnits.join("<br />= ")));

    // Limit number of displayed entries
    int messageEntries = formatted.size();
    int displayedEntries = 0;
    for (int i = 0; i < formatted.size(); i++) {
        if (i >= MAX_SEND_POPUP_ENTRIES) {
            formatted.removeLast();
            i--;
        } else {
            displayedEntries = i + 1;
        }
    }
    questionString.append("<hr />");
    questionString.append(tr("<b>(%1 of %2 entries displayed)</b>").arg(displayedEntries).arg(messageEntries));

    emit notifySendingResult(1,tr("Confirm send coins"), questionString.arg(formatted.join("<br />")));

}


void CoinControlModel::confirmSending()
{
    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(*currentTransaction);
    // process sendStatus and on error generate message shown to user
    processSendCoinsReturn(sendStatus);

    if (sendStatus.status == WalletModel::OK)
    {
        for(int row = 0 ;row< vecOuts.size();row++)
        {
            COutput  out =  vecOuts.at(row).first;
            mapSelection[QString::fromStdString(out.ToString())] = false;
        }

        CoinControlModel::coinControl->UnSelectAll();
        emit updateCoinControlLabelsSig();
    }
    fNewRecipientAllowed = true;

}

