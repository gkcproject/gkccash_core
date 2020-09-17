// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "walletmodel.h"

#include "addresstablemodel.h"
#include "guiconstants.h"
#include "recentrequeststablemodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "transactionrecord.h"

#include "rpcserver.h"
#include "coincontrolmodel.h"
#include "masternodetablemodel.h"

#include "tokenitemmodel.h"
#include "tokentransactiontablemodel.h"

#include "optionsmodel.h"
#include "base58.h"
#include "db.h"
#include "keystore.h"
#include "main.h"
#include "spork.h"
#include "sync.h"
#include "ui_interface.h"
#include "wallet.h"
#include "walletdb.h" // for BackupWallet
#include <stdint.h>
#include "guiutil.h"
#include "csvmodelwriter.h"

#include "clientmodel.h"

#include <QSet>
#include <QTimer>
#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

#include "contracttablemodel.h"
#include "createcontractpage.h"
#include "masternodeman.h"

using namespace std;

WalletModel::WalletModel(CWallet* wallet, OptionsModel* optionsModel, QObject* parent) : QObject(parent), wallet(wallet), optionsModel(optionsModel), addressTableModel(0),
    transactionTableModel(0),
    recentRequestsTableModel(0),
    cachedBalance(0), cachedUnconfirmedBalance(0), cachedImmatureBalance(0),
    cachedZerocoinBalance(0), cachedUnconfirmedZerocoinBalance(0), cachedImmatureZerocoinBalance(0),
    cachedEncryptionStatus(Unencrypted),
    cachedNumBlocks(0),
    cachedEntrusts_(0),cachedProfits_(0)
{
    fHaveWatchOnly = wallet->HaveWatchOnly();
    fHaveMultiSig = wallet->HaveMultiSig();
    fForceCheckBalanceChanged = false;

    addressTableModel = new AddressTableModel(wallet, this);

    contractTableModel = new ContractTableModel(wallet, this);
    contractfilterproxy_ = new ContractFilterProxy(this);

    contractfilterproxy_->setSourceModel(contractTableModel);
    contractfilterproxy_->setSortRole(Qt::EditRole);
    contractfilterproxy_->setDynamicSortFilter(true);
    contractfilterproxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    contractfilterproxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    contractfilterproxy_->sort(ContractTableModel::Label,Qt::DescendingOrder);
    contractfilterproxy_->setAddressTableModel(addressTableModel);

    qRegisterMetaType<CreateContractPage*>("CreateContractPage*");

    contractPage_ = new CreateContractPage(this);

    receivingAddressProxyModel_ = new AddressFilterProxy(this);
    sendingAddressProxyModel_ = new AddressFilterProxy(this);

    receivingAddressProxyModel_->setSourceModel(addressTableModel);
    sendingAddressProxyModel_->setSourceModel(addressTableModel);

    sendingAddressProxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    receivingAddressProxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);

    sendingAddressProxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    receivingAddressProxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);

    sendingAddressProxyModel_->setDynamicSortFilter(true);
    receivingAddressProxyModel_->setDynamicSortFilter(true);

    receivingAddressProxyModel_->setFilterRole(AddressTableModel::TypeRole);
    receivingAddressProxyModel_->setFilterFixedString(AddressTableModel::Receive);

    sendingAddressProxyModel_->setFilterRole(AddressTableModel::TypeRole);
    sendingAddressProxyModel_->setFilterFixedString(AddressTableModel::Send);

    sendingAddressProxyModel_->setSortRole(Qt::EditRole);
    receivingAddressProxyModel_->setSortRole(Qt::EditRole);

    mainNodesModel = new MainNodeListModel(this);
    mainNodesModel->UpdateMainNodeList();
    
    mainNodeListProxy = new MainNodeListProxy(this);
    mainNodeListProxy->setSourceModel(mainNodesModel);
    mainNodeListProxy->setSortRole(Qt::EditRole);
    mainNodeListProxy->setDynamicSortFilter(true);
    mainNodeListProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    mainNodeListProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

	entrustNodeListModel = new EntrustNodeListModel(this);
    entrustNodeListModel->UpdateNodeList();
		
	entrustNodeListProxy = new EntrustNodeListProxy(this);
	entrustNodeListProxy->setSourceModel(entrustNodeListModel);
    entrustNodeListProxy->setSortRole(Qt::EditRole);
    entrustNodeListProxy->setDynamicSortFilter(true);
    entrustNodeListProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    entrustNodeListProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
		
	entrustRecordsModel = new EntrustRecordListModel(wallet,this);
	entrustRecordsModel->UpdateEntrustRecordList();
		
	entrustRecordsProxy = new EntrustRecordListProxy(this);
	entrustRecordsProxy->setSourceModel(entrustRecordsModel);
	entrustRecordsProxy->setSortRole(Qt::EditRole);
	entrustRecordsProxy->setDynamicSortFilter(true);
	entrustRecordsProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	entrustRecordsProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
		
	redeemRecordsModel = new EntrustRecordListModel(wallet,this);
	redeemRecordsModel->UpdateRedeemRecordList();
		
	redeemRecordsProxy = new EntrustRecordListProxy(this);
	redeemRecordsProxy->setSourceModel(redeemRecordsModel);
	redeemRecordsProxy->setSortRole(Qt::EditRole);
	redeemRecordsProxy->setDynamicSortFilter(true);
	redeemRecordsProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	redeemRecordsProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

	notificationRecordsModel = new NotificationListModel(wallet,this);
	notificationRecordsModel->UpdateNotificationRecordList(0, 0, 5);

	notificationRecordsProxy = new NotificationListProxy(this);
	notificationRecordsProxy->setSourceModel(notificationRecordsModel);
	notificationRecordsProxy->setSortRole(Qt::EditRole);
	notificationRecordsProxy->setDynamicSortFilter(true);
	notificationRecordsProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	notificationRecordsProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    transactionTableModel = new TransactionTableModel(wallet, this);

    transactionProxyModel = new TransactionFilterProxy(this);
    transactionProxyModel->setSourceModel(transactionTableModel);
    transactionProxyModel->setSortRole(Qt::EditRole);
    transactionProxyModel->setDynamicSortFilter(true);
    transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    transactionProxyModel->sort(TransactionTableModel::Date,Qt::DescendingOrder);

    transactionProxyModelOverView = new TransactionFilterProxy(this);
    transactionProxyModelOverView->setSourceModel(transactionTableModel);
    transactionProxyModelOverView->setSortRole(Qt::EditRole);

    transactionProxyModelOverView->setLimit(NUM_ITEMS);
    transactionProxyModelOverView->setDynamicSortFilter(true);



    transactionProxyModelOverView->setShowInactive(false);
    transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    transactionProxyModelOverView->sort(TransactionTableModel::Date,Qt::DescendingOrder);


    typeList.push_back( 0xFFFFFFFF);
    typeList.push_back( 0x000003FFF);
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) | TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) | TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::Obfuscated));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationMakeCollaterals));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationCreateDenominations));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationDenominate));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationCollateralPayment));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    //typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ContractRecv));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ContractSend));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::StakeMint));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::MNReward));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::RecvFromZerocoinSpend));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinMint));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend_Change_zTsr));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend_FromMe));
	typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::EntrustReward));
	typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::Entrust));
	typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::Deprive));
	typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::GasRefund));
	typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::FundReward));
    typeList.push_back( TransactionFilterProxy::TYPE(TransactionRecord::Other));


    dateList.push_back( All);
    dateList.push_back( Today);
    dateList.push_back( ThisWeek);
    dateList.push_back( ThisMonth);
    dateList.push_back( LastMonth);
    dateList.push_back( ThisYear);
    dateList.push_back( Range);

    recentRequestsTableModel = new RecentRequestsTableModel(wallet, this);
    recentRequestsFilterProxy_ = new RecentRequestsFilterProxy(this);
    recentRequestsFilterProxy_->setSourceModel(recentRequestsTableModel);
    recentRequestsFilterProxy_->setSortRole(Qt::EditRole);
    recentRequestsFilterProxy_->setDynamicSortFilter(true);
    recentRequestsFilterProxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    recentRequestsFilterProxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    recentRequestsFilterProxy_->sort(RecentRequestsTableModel::Date,Qt::DescendingOrder);

    tokenItemModel_ = new TokenItemModel(wallet, this);
    tokenTransactionTableModel = new TokenTransactionTableModel(wallet, this);


    tokenfilterproxy_ = new TokenFilterProxy(this);
    tokenfilterproxy_->setSourceModel(tokenTransactionTableModel);
    tokenfilterproxy_->setSortRole(Qt::EditRole);
    tokenfilterproxy_->setDynamicSortFilter(true);
    tokenfilterproxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    tokenfilterproxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tokenfilterproxy_->sort(TokenTransactionTableModel::Date,Qt::DescendingOrder);
    tokenfilterproxy_->setWalletModel(this);
    tokenfilterproxy_->setWallet(wallet);



    coinControlModel = new CoinControlModel(this);


    coinControlProxy_ = new CoinControlProxy(this);
    coinControlProxy_->setSourceModel(coinControlModel);
    coinControlProxy_->setSortRole(Qt::EditRole);
    coinControlProxy_->setDynamicSortFilter(true);
    coinControlProxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    coinControlProxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    coinControlProxy_->sort(CoinControlModel::Date,Qt::DescendingOrder);
    connect(coinControlModel,SIGNAL(updateLabelBlockSize(QString)),coinControlProxy_,SIGNAL(updateLabelBlockSize(QString)));
    connect(coinControlModel,SIGNAL(updateCoinControlLabelsSig()),coinControlProxy_,SIGNAL(updateCoinControlLabelsSig()));
    connect(coinControlModel,SIGNAL(updateSmartFeeLabels(QVariantList)),coinControlProxy_,SIGNAL(updateSmartFeeLabels(QVariantList)));
    connect(coinControlModel,SIGNAL(notifySendingResult(int,QString,QString)),coinControlProxy_,SIGNAL(notifySendingResult(int,QString,QString)));


    masterNodeTableModel = new MasterNodeTableModel(this);
    masternodetableproxy_ = new MasterNodeTableProxy(this);
    masternodetableproxy_->setSourceModel(masterNodeTableModel);
    masternodetableproxy_->setSortRole(Qt::EditRole);
    masternodetableproxy_->setDynamicSortFilter(true);
    masternodetableproxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    masternodetableproxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    masternodetableproxy_->sort(MasterNodeTableModel::Alias,Qt::DescendingOrder);
    connect(masterNodeTableModel,SIGNAL(message(QString,QString)),masternodetableproxy_,SIGNAL(message(QString,QString)));
    connect(masterNodeTableModel,SIGNAL(setTimer(int)),masternodetableproxy_,SIGNAL(setTimer(int)));


    // This timer will be fired repeatedly to update the balance
    pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(pollBalanceChanged()));
    pollTimer->start(MODEL_UPDATE_DELAY);

    // This timer will be fired repeatedly to notify wallet backup
    backupTimer = new QTimer(this);
    connect(backupTimer, SIGNAL(timeout()), this, SLOT(backupCheck()));
    backupTimer->start(WALLET_BACKUP_DELAY);

    connect(optionsModel,SIGNAL(displayUnitChanged(int)),this,SLOT(emitBalanceChanged()));



    QTimer::singleShot(500, this, SLOT(checkForInvalidTokens()));

    subscribeToCoreSignals();
}

WalletModel::~WalletModel()
{
    unsubscribeFromCoreSignals();
}


void WalletModel::backupCheck()
{
    QSettings settings;
    uint lastBackupTime = settings.value("lastBackupTime").toUInt();

    uint now = QDateTime::currentDateTime().toTime_t();

    if(now - lastBackupTime > 86400)
    {
        emit notifyBackup();
    }




}




bool WalletModel::alreadyShowed(const QString &version)
{
    QSettings settings;

    if(settings.value("alreadyShowed").toString() != version)
    {
        settings.setValue("alreadyShowed", version);
        return false;
    }

    return true;

}

void WalletModel::checkForInvalidTokens()
{
    LOCK2(cs_main, wallet->cs_wallet);

    for(auto& info : wallet->mapToken)
    {
        std::string strAddress = info.second.strSenderAddress;
        CTxDestination address = CBitcoinAddress(strAddress).Get();
        if(!IsMine(*wallet, address))
        {
            removeTokenEntry(info.first.ToString());
        }
    }
}


QString WalletModel::getReadMe()
{
    QString fileName = "";
    if(QLocale::system().name() == "zh_CN")
        fileName = ":/README.md";
    else
        fileName = ":/README_EN.md";


    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    QByteArray array = f.readAll();
    f.close();

    return QString(array);
}


bool WalletModel::isMineAddress(const std::string &strAddress)
{
    LOCK2(cs_main, wallet->cs_wallet);

    CTxDestination address = CBitcoinAddress(strAddress).Get();
    if((address.which() == 0) || !IsMine(*wallet, address))
    {
        return false;
    }
    return true;
}

bool WalletModel::existTokenEntry(const CTokenInfo &token)
{
    LOCK2(cs_main, wallet->cs_wallet);

    uint256 hash = token.GetHash();
    std::map<uint256, CTokenInfo>::iterator it = wallet->mapToken.find(hash);

    return it != wallet->mapToken.end();
}

bool WalletModel::addTokenEntry(const CTokenInfo &token)
{
	emit addTokenEntryEmit();
    return wallet->AddTokenEntry(token, true);
}

bool WalletModel::removeTokenEntry(const std::string &sHash)
{
    return wallet->RemoveTokenEntry(uint256S(sHash), true);
}


bool WalletModel::addTokenTxEntry(const CTokenTx& tokenTx, bool fFlushOnClose)
{
	LogPrintf("BBBBBBBBBBBBBB  addTokenTxEntry | tokentx:%s \n",tokenTx.transactionHash.GetHex());
    return wallet->AddTokenTxEntry(tokenTx, fFlushOnClose);
}

void WalletModel::setClientModel(ClientModel* clientModel)
{
    connect(clientModel, SIGNAL(numBlocksChanged(int)), coinControlModel, SLOT(updateSmartFeeLabel()));
    masterNodeTableModel->setClientModel(clientModel);
}


void WalletModel::emitTraySignal(int index)
{
    emit traySignal(index);
}


bool WalletModel::isUnspentAddress(const std::string &qtumAddress) const
{
    LOCK2(cs_main, wallet->cs_wallet);

    std::vector<COutput> vecOutputs;
    wallet->AvailableCoins(vecOutputs);
    for (const COutput& out : vecOutputs)
    {
        CTxDestination address;
        const CScript& scriptPubKey = out.tx->vout[out.i].scriptPubKey;
        bool fValidAddress = ExtractDestination(scriptPubKey, address);

        if(fValidAddress && CBitcoinAddress(address).ToString() == qtumAddress && out.tx->vout[out.i].nValue)
        {
            return true;
        }
    }
    return false;
}

QString WalletModel::getNodeID(int row)
{
	if(row<0)
	{
		return "";
	}
	NodeInfo node = entrustNodeListModel->GetNodeInfo(row);
	QString id = node.NodeID();
	return id;
}

QString WalletModel::GetRedeemDescription(int row)
{
    if(row<0)
    {
        return "";
    }
	QString des = entrustRecordsModel->GetDescription(row);
    return des;
}

QString WalletModel::GetRedeemTXID(int row)
{
   if(row < 0)
   {
       return "";
   }  
	QString txid = entrustRecordsModel->GetTXID(row);
   return txid;
}

bool WalletModel::RedeemOperation(QString txid)
{
	if(txid.isNull())
	{
		message_ = "error: txid can not be empty";
		return false;
	}
	message_ = "";
    UniValue params(UniValue::VARR);
    params.push_back(txid.toStdString());
    try{
		UniValue result = tableRPC.execute("deprive",params);
	    std::string strPrint;
    	if (result.isNull())
        	strPrint = "";
	    else if (result.isStr())
    	    strPrint = result.get_str();
    	else
        	strPrint = result.write();
    	message_ = QString::fromStdString(strPrint);
		emitBalanceChanged();
	}
	catch(UniValue u)
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,"Redeem failure",QString::fromStdString(u["message"].write()),QMessageBox::Ok);	 
		msgBox->show();

    		//QMessageBox::critical(NULL,"Redeem failure",QString::fromStdString(u["message"].write()),QMessageBox::Ok);
		message_ = QString::fromStdString(u["message"].write());
		return false;
	}
	catch(std::exception const& e)
	{
		message_ = e.what();
		return false;
	}
	catch(...)
	{
		return false;
	}
    return true;
}

CAmount WalletModel::GetAllProfit()
{
	UniValue result = tableRPC.execute("listentrustrewards","");
	CAmount amount = 0;
	if(!result.isNull())
	{
		double a = result["amount"].get_real();
		amount = a*1.0e+08;
	}
	return amount;
}

void WalletModel::InitEntrust()
{
	entrustNodeListModel->UpdateNodeList();    
    entrustRecordsModel->UpdateEntrustRecordList();   
    redeemRecordsModel->UpdateRedeemRecordList();     
	cachedEntrusts_ = entrustRecordsModel->GetTotalAmount();
    cachedProfits_ = GetAllProfit();
    emit entrustChanged(cachedBalance,cachedEntrusts_,cachedProfits_);
}

bool WalletModel::EntrustOperation(QString nodeid,QString amount,int uint)
{
	message_ = "";
	try{
    	UniValue params(UniValue::VARR);
    	params.push_back(nodeid.toStdString());
		qint64 tAmount = getFieldAmount(uint,amount);
		double nAmount = tAmount*1.0/1.0e+08;
		UniValue tmpVal(nAmount);
   		params.push_back(tmpVal);
    	UniValue result = tableRPC.execute("entrust",params);
    	std::string strPrint;
    	if (result.isNull())
    	    strPrint = "";
    	else if (result.isStr())
    	    strPrint = result.get_str();
    	else
    	    strPrint = result.write();
    	message_ = QString::fromStdString(strPrint);
		emitBalanceChanged();
	}
	catch(UniValue u)
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,"Entrust failure",QString::fromStdString(u["message"].write()),QMessageBox::Ok);	 
		msgBox->show();
		
		message_ = QString::fromStdString(u["message"].write());
		return false;
	}
	catch(std::exception const& e)
	{
		message_ = e.what();
		return false;
	}
	catch(...)
	{
		return false;
	}
    return true;
}

bool WalletModel::SendAdvertiseOperation(QString title,QString link,QString text)
{
	message_ = "";
	QString author = getAddressTableModel()->addRow(AddressTableModel::Receive, "", "");
	try{
		UniValue params(UniValue::VARR);
		params.push_back(title.toStdString());
		params.push_back(author.toStdString());
		params.push_back(text.toStdString());
		params.push_back(link.toStdString());
		UniValue result = tableRPC.execute("sendadvertise",params);
		std::string strPrint;
    	if (result.isNull())
    	    strPrint = "";
    	else if (result.isStr())
    	    strPrint = result.get_str();
    	else
    	    strPrint = result.write();
		message_ = "hash:\n" + QString::fromStdString(strPrint);
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Information,"send succes",message_,QMessageBox::Ok);	 
		msgBox->show();
	}
	catch(UniValue& uv)
	{
		std::string msg = uv["message"].get_str();
		message_ = QString::fromStdString(msg);
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,"send error",message_,QMessageBox::Ok);	 
		msgBox->show();
		return false;
	}
	catch(...)
	{
		message_ = "Send advertise error";
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,"send erro",message_,QMessageBox::Ok);	 
		msgBox->show();
		return false;
	}
	return true;
}


QString WalletModel::GetMessage()
{	
	return message_;
}

int WalletModel::GetMyAgentUserNum()
{
	Agent* agent = Entrustment::GetInstance().GetMyAgent(chainActive.Height());
	return agent->userStatistics.size();
}

double WalletModel::GetMyAgentEntrust()
{
	Agent* agent = Entrustment::GetInstance().GetMyAgent(chainActive.Height());
	return (agent->receivedTotalAmount)/1.0e+08;
}

int WalletModel::GetMyUTXOCount()
{
	vector<COutput> vecOutputs;
    assert(pwalletMain != NULL);
    pwalletMain->AvailableCoins(vecOutputs, false);
	return vecOutputs.size();
}

QString WalletModel::GetDopsStateString()
{
	QString str = "utxo number: ";
	str.append(QString::number(GetMyUTXOCount()));
	str.append("  user number: ");
	str.append(QString::number(GetMyAgentUserNum()));
	str.append("  entrust: ");
	str.append(QString::number(GetMyAgentEntrust()));
	str.append(" GKC");
	return str;
}

bool WalletModel::GetMyMasternodeStatus()
{
	bool status = false;
	CMasternode* pmn = mnodeman.Find(activeMasternode.vin);
	if(pmn)
		status = true;
	return status;
}

bool WalletModel::GetStakingStat()
{
	bool nStaking = false;
    if (mapHashedBlocks.count(chainActive.Tip()->nHeight))
        nStaking = true;
    else if (mapHashedBlocks.count(chainActive.Tip()->nHeight - 1) && nLastCoinStakeSearchInterval)
        nStaking = true;
	return nStaking;
}

void WalletModel::UpdateMainNodeList()
{
	entrustNodeListModel->UpdateNodeList();
}

QString WalletModel::GetEntrustSure(QString nodeID,QString number,QString uint)
{
	//qint64 amount = getFieldAmount(uint,number);
    QString tips = "You can use address signature messages to prove that you have them.Be careful not to sign anything you do not understand,because phishing attacks may try to trick you into signing.Only sign what you know and understand and agree with.You can use address signature messages to prove that you have them.";
    QString html = "";
    html += "<html><font face='verdana, arial, helvetica, sans-serif'>";
    html += "<b>NodeID:</b> " + nodeID;
    html += "<br>";
    html += "<b>EntrustNumber:</b> " + number + uint;
    html += "<br>";
    html += "<b style=\"color:#FDA205;\"> Tips:</b>" + tips;
    return html;
}

QString WalletModel::getTxDescription(int row)
{
    QModelIndex idx = transactionProxyModel->index(row,0);

    return transactionProxyModel->mapToSource(idx).data(TransactionTableModel::LongDescriptionRole).toString();

}

QString WalletModel::formatAmount(qint64 amount,int uint)
{
    if(uint  == -1)
        return  BitcoinUnits::format(getOptionsModel()->getDisplayUnit(), amount, false, BitcoinUnits::separatorAlways);
    else
        return  BitcoinUnits::format(uint, amount, false, BitcoinUnits::separatorAlways);

}


QVariant WalletModel::getClipBoard(const QString &type)
{
    if(type == "string")
        return QApplication::clipboard()->text();

    return QVariant();
}

void WalletModel::setClipBoard(QVariant variant)
{
    switch (variant.type()) {
    case QVariant::String:
        QApplication::clipboard()->setText(variant.value<QString>());
        break;
    case QVariant::Image:
        QApplication::clipboard()->setImage(variant.value<QImage>());
        break;
    default:
        break;
    }
}


qint64 WalletModel::getFeePerkilo()
{
    return CWallet::minTxFee.GetFeePerK();
}


qint64 WalletModel::getFieldAmount(int uint,QString amountText)
{
    CAmount amount = 0;
    BitcoinUnits::parse(uint, amountText, &amount);

    return (qint64)amount;
}


#include "bip39.h"
#include "bip39words.h"




QString WalletModel::getBIPAddressFromSeeds(const char *key64, int chainType)
{
    uint8_t I[64] = {0};
    uint8_t secret[32] = {0};
    uint8_t chainCode[32] = {0};
    uint8_t s[32] = {0};
    uint8_t c[32] = {0};

    QByteArray seedsArray;
    seedsArray = QByteArray::fromRawData(key64,64);

    BIP39HMAC(I, BIP39SHA512, 64, BIP32_SEED_KEY, strlen(BIP32_SEED_KEY), key64, 64);
    memcpy(secret, I, 32);
    memcpy(chainCode, I + 32 , 32);
    CKDpriv(secret, chainCode, 0 | BIP32_HARD); // path m/0H
    CKDpriv(secret, chainCode, chainType); // path m/0H/chain
    memcpy(s, secret,32);
    memcpy(c, chainCode,32);
    CKDpriv(s, c, 0); // index'th key in chain

    CKey key;
    key.Set((const unsigned char *)s,(const unsigned char *)(s+32),true);

    CPubKey pubkey = key.GetPubKey();
    assert(key.VerifyPubKey(pubkey));

    CKeyID vchAddress;
    vchAddress = pubkey.GetID();

    CBitcoinAddress address;
    address.Set(vchAddress);
    CTxDestination dest = address.Get();
	
    //-------------
    {
        wallet->MarkDirty();
        wallet->SetAddressBook(dest, "BIP39", "receive",seedsArray.toHex().toUpper().toStdString(),chainType);

        // Don't throw error in case a key is already there
        if (wallet->HaveKey(vchAddress))
        {
            emit existingAddress();
            return "";
        }

        wallet->mapKeyMetadata[vchAddress].nCreateTime = 1;

        if (!wallet->AddKeyPubKey(key, pubkey))
            return "";

        // whenever a key is imported, we need to scan the whole chain
        wallet->nTimeFirstKey = 1; // 0 would be considered 'no value'
    }


    return QString::fromStdString(address.ToString());


}

void WalletModel::addAddressByWords(const QString &addressStr)
{
    if(addressStr.split(',').length()!=12)
    {
        emit badWords();
        return;
    }

    QStringList wordList  = addressStr.split(',');
    BIP39Words words222;

    for(int i = 0;i<wordList.length();i++)
    {

        if(words222.BIP39WordsEn.indexOf(wordList.at(i)) == -1)
        {
            emit badWords();
            return;
        }
    }

    QString words = wordList.join(' ');
    char *passphrase = "MyWallet";
    char key64[64] = {0};
    char salt[strlen("mnemonic") + (passphrase ? strlen(passphrase) : 0) + 1];
    strcpy(salt, "mnemonic");
    if (passphrase) strcpy(salt + strlen("mnemonic"), passphrase);
    PBKDF2(key64, 64, BRSHA512, 512/8, words.toStdString().data(), strlen(words.toStdString().data()), salt, strlen(salt), 2048);
    mem_clean(salt, sizeof(salt));
	
    QString externalAddress = getBIPAddressFromSeeds(key64,0);
    if(externalAddress == "") return;
    QString internalAddress = getBIPAddressFromSeeds(key64,1);
    if(internalAddress == "") return;


    emit addAddressSuccessful( externalAddress + "\n" + internalAddress);
}




void WalletModel::rescanWallet()
{

    wallet->ScanForWalletTransactions(chainActive.Genesis(), true);

}


QString WalletModel::getAmount(int currentUnit,int unit,QString text,int direction,int factor)
{
    bool valid = false;
    CAmount amount = 0;


    valid = BitcoinUnits::parse(currentUnit, text, &amount);
    if(currentUnit != unit)
        return BitcoinUnits::format(unit, amount, false, BitcoinUnits::separatorAlways);

    if (valid) {
        if(amount < 0 || amount > BitcoinUnits::maxMoney())
            amount = 0;
    }


    switch (direction) {
    case 1:
        amount += factor;
        if(amount > BitcoinUnits::maxMoney())
            amount = BitcoinUnits::maxMoney();
        break;
    case -1:
        amount -= factor;
        if(amount < 0)
            amount = 0;
        break;
    case 0:
        break;
    default:
        break;
    }



    return BitcoinUnits::format(unit, amount, false, BitcoinUnits::separatorAlways);
}





QString WalletModel::caculateSum(QVariantList rows)
{

    qint64 amount = 0;
    int nDisplayUnit = this->getOptionsModel()->getDisplayUnit();
    if (rows.size() == 0)
        return "<b>Selected amount: 0</b>";

    for(int i =0 ;i<rows.size();i++)
    {
        QModelIndex index = transactionProxyModel->mapToSource(transactionProxyModel->index(rows.at(i).toInt(),0));
        amount += index.data(TransactionTableModel::AmountRole).toLongLong();
    }

    QString strAmount("<b>Selected amount: </b>"+ BitcoinUnits::formatWithUnit(nDisplayUnit, amount, true, BitcoinUnits::separatorAlways));
    if (amount < 0) strAmount = "<span style='color:red;'>" + strAmount + "</span>";

    return strAmount;
}


void WalletModel::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(NULL,
                                                tr("Export Transaction History"), QString(),
                                                tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);
    bool fExport = false;


    // name, column, role
    writer.setModel(transactionProxyModel);
    writer.addColumn(tr("Confirmed"), 0, TransactionTableModel::ConfirmedRole);
    if (this->haveWatchOnly())
        writer.addColumn(tr("Watch-only"), TransactionTableModel::Watchonly);
    writer.addColumn(tr("Date"), 0, TransactionTableModel::DateRole);
    writer.addColumn(tr("Type"), TransactionTableModel::Type, Qt::EditRole);
    writer.addColumn(tr("Label"), 0, TransactionTableModel::LabelRole);
    writer.addColumn(tr("Address"), 0, TransactionTableModel::AddressRole);
    writer.addColumn(BitcoinUnits::getAmountColumnTitle(this->getOptionsModel()->getDisplayUnit()), 0, TransactionTableModel::FormattedAmountRole);
    writer.addColumn(tr("ID"), 0, TransactionTableModel::TxIDRole);

    fExport = writer.write();


    if (fExport) {
        emit message(tr("Exporting Successful"), tr("The transaction history was successfully saved to %1.").arg(filename),
                     CClientUIInterface::MSG_INFORMATION);
    }
    else {
        emit message(tr("Exporting Failed"), tr("There was an error trying to save the transaction history to %1.").arg(filename),
                     CClientUIInterface::MSG_ERROR);
    }
}


void WalletModel::chooseType(int idx)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(
                typeList.at(idx));
    // Persist settings
    QSettings settings;
    settings.setValue("transactionType", idx);
}


void WalletModel::chooseDate(int idx)
{
    if (!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    // dateRangeWidget->setVisible(false);

    switch (dateList.at(idx)) {
    case All:
        transactionProxyModel->setDateRange(
                    TransactionFilterProxy::MIN_DATE,
                    TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange(
                    QDateTime(current),
                    TransactionFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek() - 1));
        transactionProxyModel->setDateRange(
                    QDateTime(startOfWeek),
                    TransactionFilterProxy::MAX_DATE);

    } break;
    case ThisMonth:
        transactionProxyModel->setDateRange(
                    QDateTime(QDate(current.year(), current.month(), 1)),
                    TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange(
                    QDateTime(QDate(current.year(), current.month() - 1, 1)),
                    QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange(
                    QDateTime(QDate(current.year(), 1, 1)),
                    TransactionFilterProxy::MAX_DATE);
        break;
        //    case Range:
        //        dateRangeWidget->setVisible(true);
        //        dateRangeChanged();
        //        break;
    }
    // Persist settings
    if (dateList.at(idx) != Range) {
        QSettings settings;
        settings.setValue("transactionDate", idx);
    }
}

void WalletModel::dateRangeChanged(QString fromDate,QString toDate)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setDateRange(
                QDateTime::fromString(fromDate,"yyyy-MM-dd"),
                QDateTime::fromString(toDate,"yyyy-MM-dd").addDays(1));
}

void WalletModel::changedPrefix(QString prefix)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
}


qint64 WalletModel::getlockedCoins()
{
    return pwalletMain->GetLockedCoins();
}

void WalletModel::changedAmount(QString amount)
{
    if (!transactionProxyModel)
        return;
    CAmount amount_parsed = 0;


    // Replace "," by "." so BitcoinUnits::parse will not fail for users entering "," as decimal separator
    QString newAmount = amount;
    newAmount.replace(QString(","), QString("."));

    if (BitcoinUnits::parse(this->getOptionsModel()->getDisplayUnit(), newAmount, &amount_parsed)) {
        transactionProxyModel->setMinAmount(amount_parsed);
    } else {
        transactionProxyModel->setMinAmount(0);
    }

}

CAmount WalletModel::getBalance(const CCoinControl* coinControl) const
{
    if (coinControl) {
        CAmount nBalance = 0;
        std::vector<COutput> vCoins;
        wallet->AvailableCoins(vCoins, true, coinControl);
        BOOST_FOREACH (const COutput& out, vCoins)
                if (out.fSpendable)
                nBalance += out.tx->vout[out.i].nValue;

        return nBalance;
    }
	LogPrintf("------>getBalance   Balance(): %d MasternodeLockedBalance:%d",wallet->GetBalance(),wallet->GetMasternodeLockedBalance());
    return wallet->GetBalance()-wallet->GetMasternodeLockedBalance();
}

CAmount WalletModel::getUnconfirmedBalance() const
{
    return wallet->GetUnconfirmedBalance();
}

CAmount WalletModel::getImmatureBalance() const
{
    return wallet->GetImmatureBalance();
}

CAmount WalletModel::getLockedBalance() const
{
    return wallet->GetLockedCoins();
}

CAmount WalletModel::getZerocoinBalance() const
{
    return wallet->GetZerocoinBalance(false);
}

CAmount WalletModel::getUnconfirmedZerocoinBalance() const
{
    return wallet->GetUnconfirmedZerocoinBalance();
}

CAmount WalletModel::getImmatureZerocoinBalance() const
{
    return wallet->GetImmatureZerocoinBalance();
}


bool WalletModel::haveWatchOnly() const
{
    return fHaveWatchOnly;
}

CAmount WalletModel::getWatchBalance() const
{
    return wallet->GetWatchOnlyBalance();
}

CAmount WalletModel::getWatchUnconfirmedBalance() const
{
    return wallet->GetUnconfirmedWatchOnlyBalance();
}

CAmount WalletModel::getWatchImmatureBalance() const
{
    return wallet->GetImmatureWatchOnlyBalance();
}

void WalletModel::updateStatus()
{
    EncryptionStatus newEncryptionStatus = (EncryptionStatus)(getEncryptionStatus());

    if (cachedEncryptionStatus != newEncryptionStatus)
        emit encryptionStatusChanged(newEncryptionStatus);
}

void WalletModel::pollBalanceChanged()
{
    // Get required locks upfront. This avoids the GUI from getting stuck on
    // periodical polls if the core is holding the locks for a longer time -
    // for example, during a wallet rescan.
    emit pollBalanceChanged_sig();

    TRY_LOCK(cs_main, lockMain);
    if (!lockMain)
        return;
    TRY_LOCK(wallet->cs_wallet, lockWallet);
    if (!lockWallet)
        return;

    if (fForceCheckBalanceChanged || chainActive.Height() != cachedNumBlocks || nZeromintPercentage != cachedZeromintPercentage || cachedTxLocks != nCompleteTXLocks) {
        fForceCheckBalanceChanged = false;

        // Balance and number of transactions might have changed
        cachedZeromintPercentage = nZeromintPercentage;

        checkBalanceChanged();
        if (transactionTableModel) {
            transactionTableModel->updateConfirmations();
        }


        if(chainActive.Height() != cachedNumBlocks)
        {
            checkTokenBalanceChanged();
        }

        cachedNumBlocks = chainActive.Height();

    }


}

void WalletModel::checkTokenBalanceChanged()
{
    if(tokenItemModel_)
    {
        tokenItemModel_->checkTokenBalanceChanged();
    }
}

void WalletModel::emitBalanceChanged()
{
    //update Entrust data
    entrustNodeListModel->UpdateNodeList();    
    entrustRecordsModel->UpdateEntrustRecordList();   
    redeemRecordsModel->UpdateRedeemRecordList();     
    cachedEntrusts_ = entrustRecordsModel->GetTotalAmount();
    cachedProfits_ = GetAllProfit();
    emit entrustChanged(cachedBalance,cachedEntrusts_,cachedProfits_);
    // Force update of UI elements even when no values have changed
    emit balanceChanged(cachedBalance, cachedUnconfirmedBalance, cachedImmatureBalance,cachedTotelBalance,
                        cachedZerocoinBalance, cachedUnconfirmedZerocoinBalance, cachedImmatureZerocoinBalance,
                        cachedWatchOnlyBalance, cachedWatchUnconfBalance, cachedWatchImmatureBalance);
}

void WalletModel::updateContractBook(const QString &address, const QString &label, const QString &abi, int status)
{
    if(contractTableModel)
        contractTableModel->updateEntry(address, label, abi, status);
}

void WalletModel::checkBalanceChanged()
{
    TRY_LOCK(cs_main, lockMain);
    if (!lockMain) return;

    CAmount newBalance = getBalance();
    CAmount newUnconfirmedBalance = getUnconfirmedBalance();
    CAmount newImmatureBalance = getImmatureBalance();
    CAmount newZerocoinBalance = getZerocoinBalance();
    CAmount newUnconfirmedZerocoinBalance = getUnconfirmedZerocoinBalance();
    CAmount newImmatureZerocoinBalance = getImmatureZerocoinBalance();
    CAmount newWatchOnlyBalance = 0;
	CAmount newWatchLockBalance = getlockedCoins();
    CAmount newWatchUnconfBalance = 0;
    CAmount newWatchImmatureBalance = 0;
    if (haveWatchOnly()) {
        newWatchOnlyBalance = getWatchBalance();
        newWatchUnconfBalance = getWatchUnconfirmedBalance();
        newWatchImmatureBalance = getWatchImmatureBalance();
    }

    if (cachedBalance != newBalance || cachedUnconfirmedBalance != newUnconfirmedBalance || cachedImmatureBalance != newImmatureBalance ||
            cachedZerocoinBalance != newZerocoinBalance || cachedUnconfirmedZerocoinBalance != newUnconfirmedZerocoinBalance || cachedImmatureZerocoinBalance != newImmatureZerocoinBalance ||
            cachedWatchOnlyBalance != newWatchOnlyBalance || cachedWatchUnconfBalance != newWatchUnconfBalance || cachedWatchImmatureBalance != newWatchImmatureBalance ||
            cachedTxLocks != nCompleteTXLocks ) {
        cachedBalance = newBalance;
        cachedUnconfirmedBalance = newUnconfirmedBalance;
        cachedImmatureBalance = newImmatureBalance;
		cachedTotelBalance = cachedBalance+cachedUnconfirmedBalance+cachedImmatureBalance;
        cachedZerocoinBalance = newZerocoinBalance;
        cachedUnconfirmedZerocoinBalance = newUnconfirmedZerocoinBalance;
        cachedImmatureZerocoinBalance = newImmatureZerocoinBalance;
        cachedTxLocks = nCompleteTXLocks;
        cachedWatchOnlyBalance = newWatchOnlyBalance;
        cachedWatchUnconfBalance = cachedBalance+cachedUnconfirmedBalance;
        cachedWatchImmatureBalance = cachedBalance+cachedUnconfirmedBalance+cachedZerocoinBalance+newWatchLockBalance;
		
    	entrustNodeListModel->UpdateNodeList();   
    	entrustRecordsModel->UpdateEntrustRecordList();   
    	redeemRecordsModel->UpdateRedeemRecordList();     
    	cachedEntrusts_ = entrustRecordsModel->GetTotalAmount();
    	cachedProfits_ = GetAllProfit();
    	emit entrustChanged(cachedBalance,cachedEntrusts_,cachedProfits_);
        emit balanceChanged(newBalance, newUnconfirmedBalance, newImmatureBalance,cachedTotelBalance,
                            newZerocoinBalance, newUnconfirmedZerocoinBalance, newImmatureZerocoinBalance,
                            newWatchOnlyBalance, cachedWatchUnconfBalance, cachedWatchImmatureBalance);
    }
}

void WalletModel::updateTransaction()
{
    // Balance and number of transactions might have changed
    fForceCheckBalanceChanged = true;
}

void WalletModel::updateAddressBook(const QString& address, const QString& label, bool isMine, const QString& purpose, int status)
{
    if (addressTableModel)
        addressTableModel->updateEntry(address, label, isMine, purpose, status);
}
void WalletModel::updateAddressBook(const QString &pubCoin, const QString &isUsed, int status)
{
    if(addressTableModel)
        addressTableModel->updateEntry(pubCoin, isUsed, status);
}






void WalletModel::updateWatchOnlyFlag(bool fHaveWatchonly)
{
    fHaveWatchOnly = fHaveWatchonly;
    emit notifyWatchonlyChanged(fHaveWatchonly);
}

void WalletModel::updateMultiSigFlag(bool fHaveMultiSig)
{
    this->fHaveMultiSig = fHaveMultiSig;
    emit notifyMultiSigChanged(fHaveMultiSig);
}

bool WalletModel::validateAddress(const QString& address)
{
    CBitcoinAddress addressParsed(address.toStdString());
    return addressParsed.IsValid();
}

qint64 WalletModel::validateAmount(int currentUnit,const QString& text)
{
    CAmount amount = 0;
    BitcoinUnits::parse(currentUnit, text, &amount);

    return amount;
}

void WalletModel::sendtoAddresses()
{
    //    QList<SendCoinsRecipient> recipients;
    //    bool valid = true;

    //    for (int i = 0; i < ui->entries->count(); ++i) {
    //        SendCoinsEntry* entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());

    //        //UTXO splitter - address should be our own
    //        CBitcoinAddress address = entry->getValue().address.toStdString();
    //        if (!model->isMine(address) && ui->splitBlockCheckBox->checkState() == Qt::Checked) {
    //            CoinControlDialog::coinControl->fSplitBlock = false;
    //            ui->splitBlockCheckBox->setCheckState(Qt::Unchecked);
    //            QMessageBox::warning(this, tr("Send Coins"),
    //                                 tr("The split block tool does not work when sending to outside addresses. Try again."),
    //                                 QMessageBox::Ok, QMessageBox::Ok);
    //            return;
    //        }

    //        if (entry) {
    //            if (entry->validate()) {
    //                recipients.append(entry->getValue());
    //            } else {
    //                valid = false;
    //            }
    //        }
    //    }

    //    if (!valid || recipients.isEmpty()) {
    //        return;
    //    }

    //    //set split block in model
    //    CoinControlDialog::coinControl->fSplitBlock = ui->splitBlockCheckBox->checkState() == Qt::Checked;

    //    if (ui->entries->count() > 1 && ui->splitBlockCheckBox->checkState() == Qt::Checked) {
    //        CoinControlDialog::coinControl->fSplitBlock = false;
    //        ui->splitBlockCheckBox->setCheckState(Qt::Unchecked);
    //        QMessageBox::warning(this, tr("Send Coins"),
    //                             tr("The split block tool does not work with multiple addresses. Try again."),
    //                             QMessageBox::Ok, QMessageBox::Ok);
    //        return;
    //    }

    //    if (CoinControlDialog::coinControl->fSplitBlock)
    //        CoinControlDialog::coinControl->nSplitBlock = int(ui->splitBlockLineEdit->text().toInt());

    //    QString strFunds = tr("using") + " <b>" + tr("anonymous funds") + "</b>";
    //    QString strFee = "";
    //    recipients[0].inputType = ALL_COINS;
    //    strFunds = tr("using") + " <b>" + tr("any available funds (not recommended)") + "</b>";

    //    if (ui->checkSwiftTX->isChecked()) {
    //        recipients[0].useSwiftTX = true;
    //        strFunds += " ";
    //        strFunds += tr("and SwiftX");
    //    } else {
    //        recipients[0].useSwiftTX = false;
    //    }


    //    // Format confirmation message
    //    QStringList formatted;
    //    foreach (const SendCoinsRecipient& rcp, recipients) {
    //        // generate bold amount string
    //        QString amount = "<b>" + BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), rcp.amount);
    //        amount.append("</b> ").append(strFunds);

    //        // generate monospace address string
    //        QString address = "<span style='font-family: monospace;'>" + rcp.address;
    //        address.append("</span>");

    //        QString recipientElement;

    //        if (!rcp.paymentRequest.IsInitialized()) // normal payment
    //        {
    //            if (rcp.label.length() > 0) // label with address
    //            {
    //                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
    //                recipientElement.append(QString(" (%1)").arg(address));
    //            } else // just address
    //            {
    //                recipientElement = tr("%1 to %2").arg(amount, address);
    //            }
    //        } else if (!rcp.authenticatedMerchant.isEmpty()) // secure payment request
    //        {
    //            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
    //        } else // insecure payment request
    //        {
    //            recipientElement = tr("%1 to %2").arg(amount, address);
    //        }

    //        if (fSplitBlock) {
    //            recipientElement.append(tr(" split into %1 outputs using the UTXO splitter.").arg(CoinControlDialog::coinControl->nSplitBlock));
    //        }

    //        formatted.append(recipientElement);
    //    }

    //    fNewRecipientAllowed = false;

    //    // request unlock only if was locked or unlocked for mixing:
    //    // this way we let users unlock by walletpassphrase or by menu
    //    // and make many transactions while unlocking through this dialog
    //    // will call relock
    //    WalletModel::EncryptionStatus encStatus = model->getEncryptionStatus();
    //    if (encStatus == model->Locked || encStatus == model->UnlockedForAnonymizationOnly) {
    //        WalletModel::UnlockContext ctx(model->requestUnlock(true));
    //        if (!ctx.isValid()) {
    //            // Unlock wallet was cancelled
    //            fNewRecipientAllowed = true;
    //            return;
    //        }
    //        send(recipients, strFee, formatted);
    //        return;
    //    }
    //    // already unlocked or not encrypted at all
    //    send(recipients, strFee, formatted);


}


WalletModel::SendCoinsReturn WalletModel::prepareTransaction(WalletModelTransaction& transaction, const CCoinControl* coinControl)
{
    CAmount total = 0;
    QList<SendCoinsRecipient> recipients = transaction.getRecipients();
    std::vector<std::pair<CScript, CAmount> > vecSend;

    if (recipients.empty()) {
        return OK;
    }

    if (isAnonymizeOnlyUnlocked()) {
        return AnonymizeOnlyUnlocked;
    }

    QSet<QString> setAddress; // Used to detect duplicates
    int nAddresses = 0;

    // Pre-check input data for validity
    foreach (const SendCoinsRecipient& rcp, recipients) {
        if (rcp.paymentRequest.IsInitialized()) { // PaymentRequest...
            CAmount subtotal = 0;
            const payments::PaymentDetails& details = rcp.paymentRequest.getDetails();
            for (int i = 0; i < details.outputs_size(); i++) {
                const payments::Output& out = details.outputs(i);
                if (out.amount() <= 0) continue;
                subtotal += out.amount();
                const unsigned char* scriptStr = (const unsigned char*)out.script().data();
                CScript scriptPubKey(scriptStr, scriptStr + out.script().size());
                vecSend.push_back(std::pair<CScript, CAmount>(scriptPubKey, out.amount()));
            }
            if (subtotal <= 0) {
                return InvalidAmount;
            }
            total += subtotal;
        } else { // User-entered gkc address / amount:
            if (!validateAddress(rcp.address)) {
                return InvalidAddress;
            }
            if (rcp.amount <= 0) {
                return InvalidAmount;
            }
            setAddress.insert(rcp.address);
            ++nAddresses;

            CScript scriptPubKey = GetScriptForDestination(CBitcoinAddress(rcp.address.toStdString()).Get());
            vecSend.push_back(std::pair<CScript, CAmount>(scriptPubKey, rcp.amount));

            total += rcp.amount;
        }
    }
    if (setAddress.size() != nAddresses) {
        return DuplicateAddress;
    }

    CAmount nBalance = getBalance(coinControl);

    if (total > nBalance) {
        return AmountExceedsBalance;
    }

    {
        LOCK2(cs_main, wallet->cs_wallet);

        transaction.newPossibleKeyChange(wallet);
        CAmount nFeeRequired = 0;
        std::string strFailReason;

        CWalletTx* newTx = transaction.getTransaction();
        CReserveKey* keyChange = transaction.getPossibleKeyChange();


        if (recipients[0].useSwiftTX && total > GetSporkValue(SPORK_5_MAX_VALUE) * COIN) {
            emit message(tr("Send Coins"), tr("SwiftX doesn't support sending values that high yet. Transactions are currently limited to %1 GKC.").arg(GetSporkValue(SPORK_5_MAX_VALUE)),
                         CClientUIInterface::MSG_ERROR);
            return TransactionCreationFailed;
        }

        bool fCreated = wallet->CreateTransaction(vecSend, *newTx, *keyChange, nFeeRequired, strFailReason, coinControl, recipients[0].inputType, recipients[0].useSwiftTX);
        transaction.setTransactionFee(nFeeRequired);

        if (recipients[0].useSwiftTX && newTx->GetValueOut() > GetSporkValue(SPORK_5_MAX_VALUE) * COIN) {
            emit message(tr("Send Coins"), tr("SwiftX doesn't support sending values that high yet. Transactions are currently limited to %1 GKC.").arg(GetSporkValue(SPORK_5_MAX_VALUE)),
                         CClientUIInterface::MSG_ERROR);
            return TransactionCreationFailed;
        }

        if (!fCreated) {
            if ((total + nFeeRequired) > nBalance) {
                return SendCoinsReturn(AmountWithFeeExceedsBalance);
            }
            emit message(tr("Send Coins"), QString::fromStdString(strFailReason),
                         CClientUIInterface::MSG_ERROR);
            return TransactionCreationFailed;
        }

        // reject insane fee
        if (nFeeRequired > ::minRelayTxFee.GetFee(transaction.getTransactionSize()) * 10000)
            return InsaneFee;
    }

    return SendCoinsReturn(OK);
}

WalletModel::SendCoinsReturn WalletModel::sendCoins(WalletModelTransaction& transaction)
{
    QByteArray transaction_array; /* store serialized transaction */

    if (isAnonymizeOnlyUnlocked()) {
        return AnonymizeOnlyUnlocked;
    }

    {
        LOCK2(cs_main, wallet->cs_wallet);
        CWalletTx* newTx = transaction.getTransaction();
        QList<SendCoinsRecipient> recipients = transaction.getRecipients();

        // Store PaymentRequests in wtx.vOrderForm in wallet.
        foreach (const SendCoinsRecipient& rcp, recipients) {
            if (rcp.paymentRequest.IsInitialized()) {
                std::string key("PaymentRequest");
                std::string value;
                rcp.paymentRequest.SerializeToString(&value);
                newTx->vOrderForm.push_back(make_pair(key, value));
            } else if (!rcp.message.isEmpty()) // Message from normal gkc:URI (gkc:XyZ...?message=example)
            {
                newTx->vOrderForm.push_back(make_pair("Message", rcp.message.toStdString()));
            }
        }

        CReserveKey* keyChange = transaction.getPossibleKeyChange();

        transaction.getRecipients();

        if (!wallet->CommitTransaction(*newTx, *keyChange, (recipients[0].useSwiftTX) ? "ix" : "tx"))
            return TransactionCommitFailed;

        CTransaction* t = (CTransaction*)newTx;
        CDataStream ssTx(SER_NETWORK, PROTOCOL_VERSION);
        ssTx << *t;
        transaction_array.append(&(ssTx[0]), ssTx.size());
    }

    // Add addresses / update labels that we've sent to to the address book,
    // and emit coinsSent signal for each recipient
    foreach (const SendCoinsRecipient& rcp, transaction.getRecipients()) {
        // Don't touch the address book when we have a payment request
        if (!rcp.paymentRequest.IsInitialized()) {
            std::string strAddress = rcp.address.toStdString();
            CTxDestination dest = CBitcoinAddress(strAddress).Get();
            std::string strLabel = rcp.label.toStdString();
            {
                LOCK(wallet->cs_wallet);

                std::map<CTxDestination, CAddressBookData>::iterator mi = wallet->mapAddressBook.find(dest);

                // Check if we have a new address or an updated label
                if (mi == wallet->mapAddressBook.end()) {
                    wallet->SetAddressBook(dest, strLabel, "send");
                } else if (mi->second.name != strLabel) {
                    wallet->SetAddressBook(dest, strLabel, ""); // "" means don't change purpose
                }
            }
        }
        emit coinsSent(wallet, rcp, transaction_array);
    }
    checkBalanceChanged(); // update balance immediately, otherwise there could be a short noticeable delay until pollBalanceChanged hits

    return SendCoinsReturn(OK);
}

OptionsModel* WalletModel::getOptionsModel()
{
    return optionsModel;
}

AddressTableModel* WalletModel::getAddressTableModel()
{
    return addressTableModel;
}

ContractTableModel *WalletModel::getContractTableModel()
{
    return contractTableModel;
}

TokenItemModel *WalletModel::getTokenItemModel()
{
    return tokenItemModel_;
}

TokenTransactionTableModel *WalletModel::getTokenTransactionTableModel()
{
    return tokenTransactionTableModel;
}

TransactionTableModel* WalletModel::getTransactionTableModel()
{
    return transactionTableModel;
}

RecentRequestsTableModel* WalletModel::getRecentRequestsTableModel()
{
    return recentRequestsTableModel;
}

int WalletModel::getEncryptionStatus() const
{
    if (!wallet->IsCrypted()) {
        return Unencrypted;
    } else if (wallet->fWalletUnlockAnonymizeOnly) {
        return UnlockedForAnonymizationOnly;
    } else if (wallet->IsLocked()) {
        return Locked;
    } else {
        return Unlocked;
    }

}

bool WalletModel::checkBackupStatus()
{
    QSettings settings;
    uint lastBackupTime = settings.value("lastBackupTime").toUInt();

    uint now = QDateTime::currentDateTime().toTime_t();

    return (now-lastBackupTime) < 86400;

}

bool WalletModel::setWalletEncrypted(const QString& passStr)
{
    SecureString newPass;

    newPass.reserve(MAX_PASSPHRASE_SIZE);
    newPass.assign(passStr.toStdString().c_str());

    return wallet->EncryptWallet(newPass);
}


bool WalletModel::changePassphrase(const QString& oldPassStr, const QString& newPassStr)
{
    SecureString oldPass, newPass;

    newPass.reserve(MAX_PASSPHRASE_SIZE);
    newPass.assign(newPassStr.toStdString().c_str());
    oldPass.reserve(MAX_PASSPHRASE_SIZE);
    oldPass.assign(oldPassStr.toStdString().c_str());

    bool retval;
    {
        LOCK(wallet->cs_wallet);
        wallet->Lock(); // Make sure wallet is locked before attempting pass change
        retval = wallet->ChangeWalletPassphrase(oldPass, newPass);
    }
    return retval;
}


bool WalletModel::setWalletLocked(bool locked, const QString& passStr, bool anonymizeOnly)
{
    SecureString pass;

    if (locked) {
        // Lock
        wallet->fWalletUnlockAnonymizeOnly = false;
        return wallet->Lock();
    } else {
        // Unlock
        pass.reserve(MAX_PASSPHRASE_SIZE);
        pass.assign(passStr.toStdString().c_str());
        return wallet->Unlock(pass, anonymizeOnly);
    }
}

bool WalletModel::isAnonymizeOnlyUnlocked()
{
    return wallet->fWalletUnlockAnonymizeOnly;
}

bool WalletModel::backupWallet(const QString& filename)
{
    QUrl  url = QUrl(filename);

    bool res = BackupWallet(*wallet, url.toLocalFile().toLocal8Bit().data());
    if(res)
    {
        QSettings settings;
         settings.setValue("lastBackupTime",QDateTime::currentDateTime().toTime_t());
         settings.sync();
    }

    return res;
}

// Handlers for core signals
static void NotifyKeyStoreStatusChanged(WalletModel* walletmodel, CCryptoKeyStore* wallet)
{
    QMetaObject::invokeMethod(walletmodel, "updateStatus", Qt::QueuedConnection);
}

static void NotifyAddressBookChanged(WalletModel* walletmodel, CWallet* wallet, const CTxDestination& address, const std::string& label, bool isMine, const std::string& purpose, ChangeType status)
{
    QString strAddress = QString::fromStdString(CBitcoinAddress(address).ToString());
    QString strLabel = QString::fromStdString(label);
    QString strPurpose = QString::fromStdString(purpose);

    QMetaObject::invokeMethod(walletmodel, "updateAddressBook", Qt::QueuedConnection,
                              Q_ARG(QString, strAddress),
                              Q_ARG(QString, strLabel),
                              Q_ARG(bool, isMine),
                              Q_ARG(QString, strPurpose),
                              Q_ARG(int, status));
}

// queue notifications to show a non freezing progress dialog e.g. for rescan
static bool fQueueNotifications = false;
static std::vector<std::pair<uint256, ChangeType> > vQueueNotifications;
static void NotifyTransactionChanged(WalletModel* walletmodel, CWallet* wallet, const uint256& hash, ChangeType status)
{
    if (fQueueNotifications) {
        vQueueNotifications.push_back(make_pair(hash, status));
        return;
    }

    QString strHash = QString::fromStdString(hash.GetHex());

    QMetaObject::invokeMethod(walletmodel, "updateTransaction", Qt::QueuedConnection /*,
                                                                                                            Q_ARG(QString, strHash),
                                                                                                            Q_ARG(int, status)*/);
}

static void ShowProgress(WalletModel* walletmodel, const std::string& title, int nProgress)
{
    // emits signal "showProgress"
    QMetaObject::invokeMethod(walletmodel, "showProgress", Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(title)),
                              Q_ARG(int, nProgress));
}

static void NotifyWatchonlyChanged(WalletModel* walletmodel, bool fHaveWatchonly)
{
    QMetaObject::invokeMethod(walletmodel, "updateWatchOnlyFlag", Qt::QueuedConnection,
                              Q_ARG(bool, fHaveWatchonly));
}

static void NotifyContractBookChanged(WalletModel *walletmodel, CWallet *wallet,
                                      const std::string &address, const std::string &label, const std::string &abi, ChangeType status)
{
    QString strAddress = QString::fromStdString(address);
    QString strLabel = QString::fromStdString(label);
    QString strAbi = QString::fromStdString(abi);

    QMetaObject::invokeMethod(walletmodel, "updateContractBook", Qt::QueuedConnection,
                              Q_ARG(QString, strAddress),
                              Q_ARG(QString, strLabel),
                              Q_ARG(QString, strAbi),
                              Q_ARG(int, status));
}

static void NotifyMultiSigChanged(WalletModel* walletmodel, bool fHaveMultiSig)
{
    QMetaObject::invokeMethod(walletmodel, "updateMultiSigFlag", Qt::QueuedConnection,
                              Q_ARG(bool, fHaveMultiSig));
}

static void NotifyZerocoinChanged(WalletModel* walletmodel, CWallet* wallet, const std::string& hexString,
                                  const std::string& isUsed, ChangeType status)
{
    QString HexStr = QString::fromStdString(hexString);
    QString isUsedStr = QString::fromStdString(isUsed);
    QMetaObject::invokeMethod(walletmodel, "updateAddressBook", Qt::QueuedConnection,
                              Q_ARG(QString, HexStr),
                              Q_ARG(QString, isUsedStr),
                              Q_ARG(int, status));
}

void WalletModel::subscribeToCoreSignals()
{
    // Connect signals to wallet
    wallet->NotifyStatusChanged.connect(boost::bind(&NotifyKeyStoreStatusChanged, this, _1));
    wallet->NotifyAddressBookChanged.connect(boost::bind(NotifyAddressBookChanged, this, _1, _2, _3, _4, _5, _6));
    wallet->NotifyTransactionChanged.connect(boost::bind(NotifyTransactionChanged, this, _1, _2, _3));
    wallet->ShowProgress.connect(boost::bind(ShowProgress, this, _1, _2));
    wallet->NotifyWatchonlyChanged.connect(boost::bind(NotifyWatchonlyChanged, this, _1));
    wallet->NotifyMultiSigChanged.connect(boost::bind(NotifyMultiSigChanged, this, _1));
    wallet->NotifyZerocoinChanged.connect(boost::bind(NotifyZerocoinChanged, this, _1, _2, _3, _4));
    wallet->NotifyContractBookChanged.connect(boost::bind(NotifyContractBookChanged, this, _1, _2, _3, _4, _5));

}

void WalletModel::unsubscribeFromCoreSignals()
{
    // Disconnect signals from wallet
    wallet->NotifyStatusChanged.disconnect(boost::bind(&NotifyKeyStoreStatusChanged, this, _1));
    wallet->NotifyAddressBookChanged.disconnect(boost::bind(NotifyAddressBookChanged, this, _1, _2, _3, _4, _5, _6));
    wallet->NotifyTransactionChanged.disconnect(boost::bind(NotifyTransactionChanged, this, _1, _2, _3));
    wallet->ShowProgress.disconnect(boost::bind(ShowProgress, this, _1, _2));
    wallet->NotifyWatchonlyChanged.disconnect(boost::bind(NotifyWatchonlyChanged, this, _1));
    wallet->NotifyMultiSigChanged.disconnect(boost::bind(NotifyMultiSigChanged, this, _1));
    wallet->NotifyZerocoinChanged.disconnect(boost::bind(NotifyZerocoinChanged, this, _1, _2, _3, _4));
}

// WalletModel::UnlockContext implementation
WalletModel::UnlockContext WalletModel::requestUnlock(bool relock)
{
    bool was_locked = getEncryptionStatus() == Locked;

    if (!was_locked && isAnonymizeOnlyUnlocked()) {
        setWalletLocked(true);
        wallet->fWalletUnlockAnonymizeOnly = false;
        was_locked = getEncryptionStatus() == Locked;
    }

    if (was_locked) {
        // Request UI to unlock wallet
        emit requireUnlock();
    }
    // If wallet is still locked, unlock was failed or cancelled, mark context as invalid
    bool valid = getEncryptionStatus() != Locked;

    return UnlockContext(valid, relock);
    //    return UnlockContext(this, valid, was_locked && !isAnonymizeOnlyUnlocked());
}

WalletModel::UnlockContext::UnlockContext(bool valid, bool relock) : valid(valid), relock(relock)
{
}

WalletModel::UnlockContext::~UnlockContext()
{
    /*
    if (valid && relock) {
        wallet->setWalletLocked(true);
    }
*/
}

void WalletModel::UnlockContext::CopyFrom(const UnlockContext& rhs)
{
    // Transfer context; old object no longer relocks wallet
    *this = rhs;
    rhs.relock = false;
}

bool WalletModel::getPubKey(const CKeyID& address, CPubKey& vchPubKeyOut) const
{
    return wallet->GetPubKey(address, vchPubKeyOut);
}

// returns a list of COutputs from COutPoints
void WalletModel::getOutputs(const std::vector<COutPoint>& vOutpoints, std::vector<COutput>& vOutputs)
{
    LOCK2(cs_main, wallet->cs_wallet);
    BOOST_FOREACH (const COutPoint& outpoint, vOutpoints) {
        if (!wallet->mapWallet.count(outpoint.hash)) continue;
        int nDepth = wallet->mapWallet[outpoint.hash].GetDepthInMainChain();
        if (nDepth < 0) continue;
        COutput out(&wallet->mapWallet[outpoint.hash], outpoint.n, nDepth, true);
        vOutputs.push_back(out);
    }
}

bool WalletModel::isSpent(const COutPoint& outpoint) const
{
    LOCK2(cs_main, wallet->cs_wallet);
    return wallet->IsSpent(outpoint.hash, outpoint.n);
}

// AvailableCoins + LockedCoins grouped by wallet address (put change in one group with wallet address)
void WalletModel::listCoins(std::map<QString, std::vector<COutput> >& mapCoins) const
{
    std::vector<COutput> vCoins;
    wallet->AvailableCoins(vCoins);
    LOCK2(cs_main, wallet->cs_wallet); // ListLockedCoins, mapWallet
    std::vector<COutPoint> vLockedCoins;
    wallet->ListLockedCoins(vLockedCoins);

    // add locked coins
    BOOST_FOREACH (const COutPoint& outpoint, vLockedCoins) {
        if (!wallet->mapWallet.count(outpoint.hash)) continue;
        int nDepth = wallet->mapWallet[outpoint.hash].GetDepthInMainChain();
        if (nDepth < 0) continue;
        COutput out(&wallet->mapWallet[outpoint.hash], outpoint.n, nDepth, true);
        if (outpoint.n < out.tx->vout.size() && wallet->IsMine(out.tx->vout[outpoint.n]) == ISMINE_SPENDABLE)
            vCoins.push_back(out);
    }

    BOOST_FOREACH (const COutput& out, vCoins) {
        COutput cout = out;

        while (wallet->IsChange(cout.tx->vout[cout.i]) && cout.tx->vin.size() > 0 && wallet->IsMine(cout.tx->vin[0])) {
            if (!wallet->mapWallet.count(cout.tx->vin[0].prevout.hash)) break;
            cout = COutput(&wallet->mapWallet[cout.tx->vin[0].prevout.hash], cout.tx->vin[0].prevout.n, 0, true);
        }

        CTxDestination address;
        if (!out.fSpendable || !ExtractDestination(cout.tx->vout[cout.i].scriptPubKey, address))
            continue;
        mapCoins[QString::fromStdString(CBitcoinAddress(address).ToString())].push_back(out);
    }
}

bool WalletModel::isLockedCoin(uint256 hash, unsigned int n) const
{
    LOCK2(cs_main, wallet->cs_wallet);
    return wallet->IsLockedCoin(hash, n);
}

void WalletModel::lockCoin(COutPoint& output)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->LockCoin(output);
}

void WalletModel::unlockCoin(COutPoint& output)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->UnlockCoin(output);
}

void WalletModel::listLockedCoins(std::vector<COutPoint>& vOutpts)
{
    LOCK2(cs_main, wallet->cs_wallet);
    wallet->ListLockedCoins(vOutpts);
}


void WalletModel::listZerocoinMints(std::list<CZerocoinMint>& listMints, bool fUnusedOnly, bool fMaturedOnly, bool fUpdateStatus)
{
    listMints.clear();
    CWalletDB walletdb(wallet->strWalletFile);
    listMints = walletdb.ListMintedCoins(fUnusedOnly, fMaturedOnly, fUpdateStatus);
}

void WalletModel::loadReceiveRequests(std::vector<std::string>& vReceiveRequests)
{
    LOCK(wallet->cs_wallet);
    BOOST_FOREACH (const PAIRTYPE(CTxDestination, CAddressBookData) & item, wallet->mapAddressBook)
            BOOST_FOREACH (const PAIRTYPE(std::string, std::string) & item2, item.second.destdata)
            if (item2.first.size() > 2 && item2.first.substr(0, 2) == "rr") // receive request
            vReceiveRequests.push_back(item2.second);
}

bool WalletModel::saveReceiveRequest(const std::string& sAddress, const int64_t nId, const std::string& sRequest)
{
    CTxDestination dest = CBitcoinAddress(sAddress).Get();

    std::stringstream ss;
    ss << nId;
    std::string key = "rr" + ss.str(); // "rr" prefix = "receive request" in destdata

    LOCK(wallet->cs_wallet);
    if (sRequest.empty())
        return wallet->EraseDestData(dest, key);
    else
        return wallet->AddDestData(dest, key, sRequest);
}

bool WalletModel::isMine(CBitcoinAddress address)
{
    return IsMine(*wallet, address.Get());
}


void WalletModel::UpdateNotificationRecordList(int type, int from, int count)
{
	notificationRecordsModel->UpdateNotificationRecordList(type, from, count);
}

bool WalletModel::HaveNotificationNextPage(int type, int from, int count)
{
	return notificationRecordsModel->haveNextPage(type, from, count);
}


QString WalletModel::GetNotificationRecord(int index)
{
	//NotificationRecord nr = notificationRecordsModel->GetNotificationRecord(index);
	//if(nr.TITLE()=="")
	//	return;
	//emit notificationChanged(index,nr.TITLE(),nr.TIME(),nr.TEXT(),nr.BLOCK(),nr.LINK(),nr.AUTHOR(),nr.HASH());
	return notificationRecordsModel->GetNotificationRecord(index);
}

QString WalletModel::GetNotificationTitle(int index)
{
	return notificationRecordsModel->GetNotificationTitle(index);
}

QString WalletModel::GetEntructDescription(int row)
{
	return entrustRecordsModel->GetEntructDescription(row);
}

QString WalletModel::GetDepriveDescription(int row)
{
	return redeemRecordsModel->GetDepriveDescription(row);
}

