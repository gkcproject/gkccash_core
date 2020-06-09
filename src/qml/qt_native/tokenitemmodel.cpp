#include "tokenitemmodel.h"
#include "token.h"
#include "walletmodel.h"
#include "wallet.h"

#include "bitcoinunits.h"
#include <algorithm>

#include <QDateTime>
#include <QFont>
#include <QThread>
#include <QMessageBox>
#include <qrencode.h>

#include "imageprovider.h"

#include "optionsmodel.h"

extern ImageProvider *imageProvider;

class TokenItemEntry
{
public:
    TokenItemEntry()
    {}

    TokenItemEntry(uint256 tokenHash, CTokenInfo tokenInfo)
    {
        hash = tokenHash;
        createTime.setTime_t(tokenInfo.nCreateTime);
        contractAddress = QString::fromStdString(tokenInfo.strContractAddress);
        tokenName = QString::fromStdString(tokenInfo.strTokenName);
        tokenSymbol = QString::fromStdString(tokenInfo.strTokenSymbol);
        decimals = tokenInfo.nDecimals;
        senderAddress = QString::fromStdString(tokenInfo.strSenderAddress);
    }

    TokenItemEntry( const TokenItemEntry &obj)
    {
        hash = obj.hash;
        createTime = obj.createTime;
        contractAddress = obj.contractAddress;
        tokenName = obj.tokenName;
        tokenSymbol = obj.tokenSymbol;
        decimals = obj.decimals;
        senderAddress = obj.senderAddress;
        balance = obj.balance;
    }

    bool update(Token* tokenAbi)
    {
        bool modified;
        return update(tokenAbi, modified);
    }

    bool update(Token* tokenAbi, bool& modified)
    {
        modified = false;

        if(!tokenAbi)
            return false;

        bool ret = true;
        tokenAbi->setAddress(contractAddress.toStdString());
        tokenAbi->setSender(senderAddress.toStdString());
        std::string strBalance;
        ret &= tokenAbi->balanceOf(strBalance);
        if(ret)
        {
            int256_t val(strBalance);
            if(val != balance)
            {
                modified = true;
            }
            balance = val;
        }

        return ret;
    }

    ~TokenItemEntry()
    {}

    uint256 hash;
    QDateTime createTime;
    QString contractAddress;
    QString tokenName;
    QString tokenSymbol;
    quint8 decimals;
    QString senderAddress;
    int256_t balance;
};

class TokenTxWorker : public QObject
{
    Q_OBJECT
public:
    CWallet *wallet;
    bool first;
    Token tokenTxAbi;
    TokenTxWorker(CWallet *_wallet):
        wallet(_wallet), first(true) {}
    
private Q_SLOTS:
    void updateTokenTx(const QString &hash)
    {
        // Initialize variables
        uint256 tokenHash = uint256S(hash.toStdString());
        int64_t fromBlock = 0;
        int64_t toBlock = -1;
        CTokenInfo tokenInfo;
        uint256 blockHash;
        bool found = false;

        LOCK2(cs_main, wallet->cs_wallet);

        int64_t backInPast = first ? COINBASE_MATURITY : 10;
        first = false;


        CBlockIndex* tip = chainActive.Tip();
        if(tip)
        {

            // Get current block hash and height
            blockHash = tip->GetBlockHash();
            toBlock = chainActive.Height();

            // Find the token tx in the wallet
            std::map<uint256, CTokenInfo>::iterator mi = wallet->mapToken.find(tokenHash);
            found = mi != wallet->mapToken.end();
            if(found)
            {


                // Get the start location for search the event log
                tokenInfo = mi->second;
                CBlockIndex* index = chainActive[tokenInfo.blockNumber];
                if(tokenInfo.blockNumber < toBlock)
                {

                    if(index && index->GetBlockHash() == tokenInfo.blockHash)
                    {
                        fromBlock = tokenInfo.blockNumber;

                    }
                    else
                    {
                        fromBlock = tokenInfo.blockNumber - backInPast;

                    }
                }
                else
                {

                    fromBlock = toBlock - backInPast;
                }
                if(fromBlock < 0)
                    fromBlock = 0;

                tokenInfo.blockHash = blockHash;
                tokenInfo.blockNumber = toBlock;


            }
        }

        if(found)
        {
            // List the events and update the token tx
            std::vector<TokenEvent> tokenEvents;
            tokenTxAbi.setAddress(tokenInfo.strContractAddress);
            tokenTxAbi.setSender(tokenInfo.strSenderAddress);
            tokenTxAbi.transferEvents(tokenEvents, fromBlock, toBlock);



            for(size_t i = 0; i < tokenEvents.size(); i++)
            {
                TokenEvent event = tokenEvents[i];
                CTokenTx tokenTx;
                tokenTx.strContractAddress = event.address;
                tokenTx.strSenderAddress = event.sender;
                tokenTx.strReceiverAddress = event.receiver;
                tokenTx.nValue = event.value;
                tokenTx.transactionHash = event.transactionHash;
                tokenTx.blockHash = event.blockHash;
                tokenTx.blockNumber = event.blockNumber;
                wallet->AddTokenTxEntry(tokenTx, false);

            }

            wallet->AddTokenEntry(tokenInfo);
        }
    }

    void cleanTokenTxEntries()
    {
        if(wallet) wallet->CleanTokenTxEntries();
    }
};

#include "tokenitemmodel.moc"

struct TokenItemEntryLessThan
{
    bool operator()(const TokenItemEntry &a, const TokenItemEntry &b) const
    {
        return a.hash < b.hash;
    }
    bool operator()(const TokenItemEntry &a, const uint256 &b) const
    {
        return a.hash < b;
    }
    bool operator()(const uint256 &a, const TokenItemEntry &b) const
    {
        return a < b.hash;
    }
};

class TokenItemPriv
{
public:
    CWallet *wallet;
    QList<TokenItemEntry> cachedTokenItem;
    TokenItemModel *parent;

    TokenItemPriv(CWallet *_wallet, TokenItemModel *_parent):
        wallet(_wallet), parent(_parent) {}

    void refreshTokenItem()
    {
        cachedTokenItem.clear();
        {
            LOCK2(cs_main, wallet->cs_wallet);

            for(std::map<uint256, CTokenInfo>::iterator it = wallet->mapToken.begin(); it != wallet->mapToken.end(); ++it)
            {
                TokenItemEntry tokenItem(it->first, it->second);
                if(parent)
                {
                    tokenItem.update(parent->getTokenAbi());


                }
                cachedTokenItem.append(tokenItem);
            }
        }
        std::sort(cachedTokenItem.begin(), cachedTokenItem.end(), TokenItemEntryLessThan());
    }

    void updateEntry(const TokenItemEntry &item, int status)
    {
        // Find address / label in model
        QList<TokenItemEntry>::iterator lower = qLowerBound(
                    cachedTokenItem.begin(), cachedTokenItem.end(), item, TokenItemEntryLessThan());
        QList<TokenItemEntry>::iterator upper = qUpperBound(
                    cachedTokenItem.begin(), cachedTokenItem.end(), item, TokenItemEntryLessThan());
        int lowerIndex = (lower - cachedTokenItem.begin());
        int upperIndex = (upper - cachedTokenItem.begin());
        bool inModel = (lower != upper);

        switch(status)
        {
        case CT_NEW:
            if(inModel)
            {
                qWarning() << "TokenItemPriv::updateEntry: Warning: Got CT_NEW, but entry is already in model";
                break;
            }
            parent->beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
            cachedTokenItem.insert(lowerIndex, item);
            parent->endInsertRows();
            break;
        case CT_UPDATED:
            if(!inModel)
            {
                qWarning() << "TokenItemPriv::updateEntry: Warning: Got CT_UPDATED, but entry is not in model";
                break;
            }
            cachedTokenItem[lowerIndex] = item;
            parent->emitDataChanged(lowerIndex);
            break;
        case CT_DELETED:
            if(!inModel)
            {
                qWarning() << "TokenItemPriv::updateEntry: Warning: Got CT_DELETED, but entry is not in model";
                break;
            }
            parent->beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
            cachedTokenItem.erase(lower, upper);
            parent->endRemoveRows();
            break;
        }
    }

    int size()
    {
        return cachedTokenItem.size();
    }

    TokenItemEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedTokenItem.size())
        {
            return &cachedTokenItem[idx];
        }
        else
        {
            return new TokenItemEntry();
        }
    }
};

TokenItemModel::TokenItemModel(CWallet *_wallet, WalletModel *parent):
    QAbstractItemModel(parent),
    tokenAbi(0),
    walletModel(parent),
    wallet(_wallet),
    priv(0),
    worker(0),
    tokenTxCleaned(false)
{
    columns << tr("Token Name") << tr("Token Symbol") << tr("Balance");
    tokenAbi = new Token();

    m_selectedToken = new SelectedToken();


    priv = new TokenItemPriv(wallet, this);
    priv->refreshTokenItem();

    worker = new TokenTxWorker(wallet);
    worker->moveToThread(&(t));

    t.start();

    subscribeToCoreSignals();
}

TokenItemModel::~TokenItemModel()
{
    unsubscribeFromCoreSignals();

    t.quit();
    t.wait();

    if(tokenAbi)
    {
        delete tokenAbi;
        tokenAbi = 0;
    }

    if(priv)
    {
        delete priv;
        priv = 0;
    }
}

QModelIndex TokenItemModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    TokenItemEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    return QModelIndex();
}

void TokenItemModel::menuPicked(int modelIndex,int menuIndex)
{
    QModelIndex index_ = index(modelIndex,0);
    switch (menuIndex) {
    case 0:
        walletModel->setClipBoard(data(index_,TokenItemModel::SenderRole));
        break;
    case 1:
        walletModel->setClipBoard(data(index_,TokenItemModel::BalanceRole));
        break;
    case 2:
        walletModel->setClipBoard(data(index_,TokenItemModel::NameRole));
        break;
    case 3:
        walletModel->setClipBoard(data(index_,TokenItemModel::AddressRole));
        break;
    default:
        break;
    }

}

void TokenItemModel::removeTokenItem(int modelIndex)
{
    QModelIndex index_ = index(modelIndex,0);

    std::string sHash = index_.data(TokenItemModel::HashRole).toString().toStdString();
    walletModel->removeTokenEntry(sHash);
}


QVariantList TokenItemModel::updateReceiveImg(int modelIndex)
{
    QVariantList returnList = QVariantList();

    QModelIndex index_ = index(modelIndex,0);

	QString address = index_.data(TokenItemModel::SenderRole).toString();
    QString symbol = index_.data(TokenItemModel::SymbolRole).toString();
    QString address_ = "gkc:" + address;
	
	//int decimalDiff = decimals - m_selectedToken->decimals;
	m_selectedToken->address = index_.data(TokenItemModel::AddressRole).toString().toStdString();
	m_selectedToken->sender = index_.data(TokenItemModel::SenderRole).toString().toStdString();
	m_selectedToken->symbol = symbol.toStdString();
	m_selectedToken->decimals = index_.data(TokenItemModel::DecimalsRole).toUInt();
	m_selectedToken->balance = index_.data(TokenItemModel::BalanceRole).toString().toStdString();

	   
    QRcode* code = QRcode_encodeString(address_.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!code) {
        return returnList;
    }
    QImage myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
    myImage.fill(0xffffff);
    unsigned char* p = code->data;
    for (int y = 0; y < code->width; y++) {
        for (int x = 0; x < code->width; x++) {
            myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
            p++;
        }
    }
    QRcode_free(code);

    myImage = myImage.scaled(300,300);

    imageProvider->setImage(myImage);

    returnList.append(symbol);
    returnList.append(address);

    return returnList;
}


QModelIndex TokenItemModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int TokenItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int TokenItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QHash<int, QByteArray> TokenItemModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[SenderRole] = "address";
    roles[SymbolRole] = "symbol";
    roles[BalanceRole] = "balance";


    return roles;
}

QVariant TokenItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TokenItemEntry *rec = static_cast<TokenItemEntry*>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Name:
            return rec->tokenName;
        case Symbol:
            return rec->tokenSymbol;
        case Balance:
            return BitcoinUnits::formatToken(rec->decimals, rec->balance, false, BitcoinUnits::separatorAlways);
        default:
            break;
        }
        break;
    case TokenItemModel::HashRole:
        return QString::fromStdString(rec->hash.ToString());
        break;
    case TokenItemModel::AddressRole:
        return rec->contractAddress;
        break;
    case TokenItemModel::NameRole:
        return rec->tokenName;
        break;
    case TokenItemModel::SymbolRole:
        return rec->tokenSymbol;
        break;
    case TokenItemModel::DecimalsRole:
        return rec->decimals;
        break;
    case TokenItemModel::SenderRole:
        return rec->senderAddress;
        break;
    case TokenItemModel::BalanceRole:
        return BitcoinUnits::formatToken(rec->decimals, rec->balance, false, BitcoinUnits::separatorAlways);
        break;
    case TokenItemModel::RawBalanceRole:
        return QString::fromStdString(rec->balance.str());
        break;
    default:
        break;
    }

    return QVariant();
}

Token *TokenItemModel::getTokenAbi()
{
    return tokenAbi;
}

void TokenItemModel::updateToken(const QString &hash, int status, bool showToken)
{
    // Find token in wallet
    LOCK2(cs_main, wallet->cs_wallet);

    uint256 updated;
    updated.SetHex(hash.toStdString());
    std::map<uint256, CTokenInfo>::iterator mi = wallet->mapToken.find(updated);
    showToken &= mi != wallet->mapToken.end();

    TokenItemEntry tokenEntry;
    if(showToken)
    {
        tokenEntry = TokenItemEntry(mi->first, mi->second);
        tokenEntry.update(getTokenAbi());
    }
    else
    {
        tokenEntry.hash = updated;
    }
    priv->updateEntry(tokenEntry, status);
}


QVariantList TokenItemModel::sendToken(const QString &payToAddress,
                                       const QString &gasLimitStr,
                                       int gasPriceUint,
                                       const QString &gasPriceStr,
                                       const QString &amountStr,
                                       const QString &description
                                       )
{

    QVariantList returnList = QVariantList();
    std::string symbol,decimals;

    WalletModel::UnlockContext ctx(walletModel->requestUnlock());
    if(!ctx.isValid())
    {
        return returnList;
    }

    if(walletModel && (walletModel->getBalance() > 0))
    {
        int unit = walletModel->getOptionsModel()->getDisplayUnit();

        uint64_t gasLimit = gasLimitStr.toULongLong();
        CAmount gasPrice = 0;
        BitcoinUnits::parse(gasPriceUint, gasPriceStr, &gasPrice);
		if(gasPrice == 0)
		{
			returnList.append(tr("Error send token"));
            returnList.append("Gas price cannot be 0!");
            return returnList;
		}

		if(gasLimit == 0)
		{
			returnList.append(tr("Error send token"));
            returnList.append("Gas limit cannot be 0!");
            return returnList;
		}

		if(m_selectedToken==NULL)
		{
			returnList.append(tr("Error send token"));
            returnList.append("Please select token first!");
            return returnList;
		}

		if(m_selectedToken->address==""||m_selectedToken->balance=="")
		{
			returnList.append(tr("Error send token"));
            returnList.append("Please select token first!");
            return returnList;
		}

        label = description.toStdString();


        tokenAbi->decimals(decimals);
        tokenAbi->symbol(symbol);


        tokenAbi->setAddress(m_selectedToken->address);
        tokenAbi->setSender(m_selectedToken->sender);
        tokenAbi->setGasLimit(QString::number(gasLimit).toStdString());
        tokenAbi->setGasPrice(BitcoinUnits::format(unit, gasPrice, false, BitcoinUnits::separatorNever).toStdString());



        toAddress = payToAddress.toStdString();


        int256_t amount_;
        if(!BitcoinUnits::parseToken(m_selectedToken->decimals,amountStr,&amount_))
        {
			returnList.append(tr("Error send token"));
            returnList.append("Exceeds max precision");
            return returnList;
		}

        int256_t balance;
		
		std::cout << "m_selectedToken->balance:" << m_selectedToken->balance << "\n";
		std::cout << "m_selectedToken->decimals:" << m_selectedToken->decimals << "\n";
		std::cout << "m_selectedToken->address:" << m_selectedToken->address << "\n";
		std::cout << "m_selectedToken->sender:" << m_selectedToken->sender << "\n";
        BitcoinUnits::parseToken(m_selectedToken->decimals,QString::fromStdString(m_selectedToken->balance),&balance);
		std::cout << "amount:" << amount_ << "  balance:" << balance << "\n";

        if(amount_ > balance)
        {
            returnList.append(tr("Error send token"));
            returnList.append("Balance is not enough for giving amount");
            return returnList;
        }


        amountToSend = BitcoinUnits::formatToken(0, amount_, false, BitcoinUnits::separatorNever).replace(" ","").toStdString();


        amountFormated = BitcoinUnits::formatToken(m_selectedToken->decimals, amount_, false, BitcoinUnits::separatorAlways);



        returnList.append(tr("Confirm send token."));

        QString questionString = tr("Are you sure you want to send? <br /><br />");
        questionString.append(tr("<b>%1 %2 </b> to ")
                              .arg(amountFormated).arg(QString::fromStdString(m_selectedToken->symbol)));
        questionString.append(tr("<br />%3 <br />")
                              .arg(QString::fromStdString(toAddress)));


        returnList.append(questionString);




    }
    else
    {


        QString message = tr("To send %1 you need GKC for the fee charged.")
                .arg(QString::fromStdString(symbol));

        returnList.append(tr("Error send token"));
        returnList.append(message);

    }

    return returnList;
}

void TokenItemModel::clearSendToken()
{
	toAddress = "";
}


bool TokenItemModel::sendTokenConfirmed()
{
    WalletModel::UnlockContext ctx(walletModel->requestUnlock());

    if(!ctx.isValid())
    {
        return false;
    }

	if(toAddress=="")
	{
		return false;
	}

    QString errorMessage;



    if(tokenAbi->transfer(toAddress, amountToSend, errorMessage,true ))
    {
        CTokenTx tokenTx;
        tokenTx.strContractAddress = m_selectedToken->address;
        tokenTx.strSenderAddress = m_selectedToken->sender;
        tokenTx.strReceiverAddress = toAddress;
        dev::u256 nValue(amountToSend);
        tokenTx.nValue = u256Touint(nValue);
        tokenTx.transactionHash = uint256S(tokenAbi->getTxId());
        tokenTx.strLabel = label;
        walletModel->addTokenTxEntry(tokenTx);
        return true;
    }



    //emit execError(errorMessage);
	QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical,"Error",errorMessage,QMessageBox::Ok);	 
	msgBox->show();


    return false;
}



void TokenItemModel::checkTokenBalanceChanged()
{
    if(!priv && !tokenAbi)
        return;


    for(int i = 0; i < priv->cachedTokenItem.size(); i++)
    {
        TokenItemEntry tokenEntry = priv->cachedTokenItem[i];
        bool modified = false;
        tokenEntry.update(tokenAbi, modified);
        if(modified)
        {
            priv->cachedTokenItem[i] = tokenEntry;
            emitDataChanged(i);

        }



        // Search for token transactions
        if(fLogEvents)
        {

            QString hash = QString::fromStdString(tokenEntry.hash.ToString());
            QMetaObject::invokeMethod(worker, "updateTokenTx", Qt::QueuedConnection,
                                      Q_ARG(QString, hash));
        }
    }

    // Clean token transactions
    if(fLogEvents && !tokenTxCleaned)
    {
        tokenTxCleaned = true;
        QMetaObject::invokeMethod(worker, "cleanTokenTxEntries", Qt::QueuedConnection);
    }
}

void TokenItemModel::emitDataChanged(int idx)
{
    Q_EMIT dataChanged(index(idx, 0, QModelIndex()), index(idx, columns.length()-1, QModelIndex()));
}

struct TokenNotification
{
public:
    TokenNotification() {}
    TokenNotification(uint256 _hash, ChangeType _status, bool _showToken):
        hash(_hash), status(_status), showToken(_showToken) {}

    void invoke(QObject *tim)
    {
        QString strHash = QString::fromStdString(hash.GetHex());

        QMetaObject::invokeMethod(tim, "updateToken", Qt::QueuedConnection,
                                  Q_ARG(QString, strHash),
                                  Q_ARG(int, status),
                                  Q_ARG(bool, showToken));
    }
private:
    uint256 hash;
    ChangeType status;
    bool showToken;
};

static void NotifyTokenChanged(TokenItemModel *tim, CWallet *wallet, const uint256 &hash, ChangeType status)
{
    // Find token in wallet
    LOCK2(cs_main, wallet->cs_wallet);

    std::map<uint256, CTokenInfo>::iterator mi = wallet->mapToken.find(hash);
    bool showToken = mi != wallet->mapToken.end();

    TokenNotification notification(hash, status, showToken);
    notification.invoke(tim);
}

void TokenItemModel::subscribeToCoreSignals()
{
    // Connect signals to wallet
    wallet->NotifyTokenChanged.connect(boost::bind(NotifyTokenChanged, this, _1, _2, _3));
}

void TokenItemModel::unsubscribeFromCoreSignals()
{
    // Disconnect signals from wallet
    wallet->NotifyTokenChanged.disconnect(boost::bind(NotifyTokenChanged, this, _1, _2, _3));
}
