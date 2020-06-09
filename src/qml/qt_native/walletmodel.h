// Copyright (c) 2011-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_WALLETMODEL_H
#define BITCOIN_QT_WALLETMODEL_H

#include "paymentrequestplus.h"
#include "walletmodeltransaction.h"

#include "allocators.h" /* for SecureString */
#include "swifttx.h"
#include "wallet.h"
#include "transactionfilterproxy.h"
#include "entrustnodelistproxy.h"
#include "entrustrecordlistproxy.h"
#include "entrustnodelistmodel.h"
#include "entrustrecordlistmodel.h"
#include "addressfilterproxy.h"
#include "recentrequestsfilterproxy.h"
#include "contractfilterproxy.h"
#include "tokenfilterproxy.h"
#include "notificationlistmodel.h"
#include "notificationlistproxy.h"

#include "coincontrolproxy.h"
#include "masternodetableproxy.h"
#include "mainnodelistproxy.h"
#include "mainnodelistmodel.h"

#include <map>
#include <vector>

#include <QObject>

class AddressTableModel;
class OptionsModel;
class RecentRequestsTableModel;
class TransactionTableModel;
class WalletModelTransaction;
class ContractTableModel;
class TokenItemModel;
class TokenTransactionTableModel;
class ClientModel;
class CoinControlModel;


class CreateContractPage;

class MasterNodeTableModel;

class CCoinControl;
class CKeyID;
class COutPoint;
class COutput;
class CPubKey;
class CWallet;
class uint256;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

#define NUM_ITEMS 5

using namespace std;

class SendCoinsRecipient
{
public:
    explicit SendCoinsRecipient() : amount(0), nVersion(SendCoinsRecipient::CURRENT_VERSION) {}
    explicit SendCoinsRecipient(const QString& addr, const QString& label, const CAmount& amount, const QString& message) : address(addr), label(label), amount(amount), message(message), nVersion(SendCoinsRecipient::CURRENT_VERSION) {}

    // If from an insecure payment request, this is used for storing
    // the addresses, e.g. address-A<br />address-B<br />address-C.
    // Info: As we don't need to process addresses in here when using
    // payment requests, we can abuse it for displaying an address list.
    // Todo: This is a hack, should be replaced with a cleaner solution!
    QString address;
    QString label;
    AvailableCoinsType inputType;
    bool useSwiftTX;
    CAmount amount;
    // If from a payment request, this is used for storing the memo
    QString message;

    // If from a payment request, paymentRequest.IsInitialized() will be true
    PaymentRequestPlus paymentRequest;
    // Empty if no authentication or invalid signature/cert/etc.
    QString authenticatedMerchant;

    static const int CURRENT_VERSION = 1;
    int nVersion;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        std::string sAddress = address.toStdString();
        std::string sLabel = label.toStdString();
        std::string sMessage = message.toStdString();
        std::string sPaymentRequest;
        if (!ser_action.ForRead() && paymentRequest.IsInitialized())
            paymentRequest.SerializeToString(&sPaymentRequest);
        std::string sAuthenticatedMerchant = authenticatedMerchant.toStdString();

        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(sAddress);
        READWRITE(sLabel);
        READWRITE(amount);
        READWRITE(sMessage);
        READWRITE(sPaymentRequest);
        READWRITE(sAuthenticatedMerchant);

        if (ser_action.ForRead()) {
            address = QString::fromStdString(sAddress);
            label = QString::fromStdString(sLabel);
            message = QString::fromStdString(sMessage);
            if (!sPaymentRequest.empty())
                paymentRequest.parse(QByteArray::fromRawData(sPaymentRequest.data(), sPaymentRequest.size()));
            authenticatedMerchant = QString::fromStdString(sAuthenticatedMerchant);
        }
    }
};

/** Interface to Bitcoin wallet from Qt view code. */
class WalletModel : public QObject
{
    Q_OBJECT	
	Q_PROPERTY(NotificationListProxy *notificationRecords_Proxy MEMBER notificationRecordsProxy CONSTANT)
	Q_PROPERTY(EntrustRecordListProxy *redeemRecords_Proxy MEMBER redeemRecordsProxy CONSTANT)
	Q_PROPERTY(EntrustRecordListProxy *entrustRecords_Proxy MEMBER entrustRecordsProxy CONSTANT)
	Q_PROPERTY(EntrustNodeListProxy *nodeList_Proxy MEMBER entrustNodeListProxy CONSTANT)
	Q_PROPERTY(MainNodeListProxy *mainNodeList_Proxy MEMBER mainNodeListProxy CONSTANT)
    Q_PROPERTY(TransactionFilterProxy *transaction_ProxyModel MEMBER transactionProxyModel CONSTANT)
    Q_PROPERTY(QString syncStatus MEMBER m_syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(TransactionFilterProxy *transaction_ProxyModelOverView MEMBER transactionProxyModelOverView CONSTANT)
    Q_PROPERTY(AddressFilterProxy *receivingAddressProxyModel MEMBER receivingAddressProxyModel_ CONSTANT)
    Q_PROPERTY(AddressFilterProxy *sendingAddressProxyModel MEMBER sendingAddressProxyModel_ CONSTANT)
    Q_PROPERTY(RecentRequestsFilterProxy *recentRequestsFilterProxy MEMBER recentRequestsFilterProxy_ CONSTANT)

    Q_PROPERTY(ContractFilterProxy *contractfilterproxy MEMBER contractfilterproxy_ CONSTANT)

    Q_PROPERTY(CreateContractPage *contractPage MEMBER contractPage_ CONSTANT)

    Q_PROPERTY(TokenFilterProxy *tokenfilterproxy MEMBER tokenfilterproxy_ CONSTANT)
    Q_PROPERTY(TokenItemModel *tokenItemModel MEMBER tokenItemModel_ CONSTANT)

    Q_PROPERTY(CoinControlProxy *coinControlProxy MEMBER coinControlProxy_ CONSTANT)

    Q_PROPERTY(MasterNodeTableProxy *masternodetableproxy MEMBER masternodetableproxy_ CONSTANT)



public:
    explicit WalletModel(CWallet* wallet, OptionsModel* optionsModel, QObject* parent = 0);
    ~WalletModel();

    Q_INVOKABLE QString getTxDescription(int row);
    Q_INVOKABLE QString caculateSum(QVariantList rows);
    Q_INVOKABLE void exportClicked();
    Q_INVOKABLE void chooseType(int idx);
    Q_INVOKABLE void chooseDate(int idx);
    Q_INVOKABLE void dateRangeChanged(QString fromDate, QString toDate);
    Q_INVOKABLE void changedPrefix(QString prefix);
    Q_INVOKABLE void changedAmount(QString amount);
    Q_INVOKABLE qint64 getlockedCoins();
	
	Q_INVOKABLE CAmount GetAllProfit();
	Q_INVOKABLE QString getNodeID(int row);
    Q_INVOKABLE QString GetRedeemDescription(int row);
    Q_INVOKABLE QString GetRedeemTXID(int row);
    Q_INVOKABLE QString GetEntrustSure(QString nodeID,QString number,QString uint);
    Q_INVOKABLE bool RedeemOperation(QString txid);
    Q_INVOKABLE bool EntrustOperation(QString nodeid,QString amount,int uint);
	Q_INVOKABLE bool SendAdvertiseOperation(QString title,QString link,QString text);
	Q_INVOKABLE QString GetMessage();
	Q_INVOKABLE void InitEntrust();
	Q_INVOKABLE int GetMyAgentUserNum();
	Q_INVOKABLE int GetMyUTXOCount();
	Q_INVOKABLE double GetMyAgentEntrust();
	Q_INVOKABLE QString GetDopsStateString();
	Q_INVOKABLE bool GetStakingStat();
	Q_INVOKABLE bool GetMyMasternodeStatus();

	Q_INVOKABLE void UpdateMainNodeList();

    Q_INVOKABLE QString formatAmount(qint64 amount, int unit = -1);
    Q_INVOKABLE QString getAmount(int currentUnit, int unit, QString text, int direction, int factor=100000);
    Q_INVOKABLE QVariant getClipBoard(const QString &type);
    Q_INVOKABLE void setClipBoard(QVariant variant);
    Q_INVOKABLE qint64 getFeePerkilo();

    Q_INVOKABLE bool alreadyShowed(const QString &version);


    Q_INVOKABLE qint64 getFieldAmount(int uint,QString amountText);

    Q_INVOKABLE void addAddressByWords(const QString &addressStr);

    Q_INVOKABLE void rescanWallet();

    // Check address for validity
    Q_INVOKABLE bool validateAddress(const QString& address);
    Q_INVOKABLE qint64 validateAmount(int currentUnit, const QString& text);

    Q_INVOKABLE void sendtoAddresses();


    Q_INVOKABLE QString getReadMe();

    Q_INVOKABLE bool isTestNet(){return GetBoolArg("-testnet", false);}
    Q_INVOKABLE int getEncryptionStatus() const;

    // Wallet encryption
    Q_INVOKABLE bool setWalletEncrypted(const QString& passStr);
    Q_INVOKABLE bool changePassphrase(const QString &oldPassStr, const QString &newPassStr);
    // Passphrase only needed when unlocking
    Q_INVOKABLE bool setWalletLocked(bool locked, const QString &passStr = QString(), bool anonymizeOnly = false);

    // Wallet backup
    Q_INVOKABLE bool backupWallet(const QString& filename);
    Q_INVOKABLE bool checkBackupStatus();

	Q_INVOKABLE void UpdateNotificationRecordList(int type,int from,int count);
	Q_INVOKABLE bool HaveNotificationNextPage(int type,int from,int count);
	Q_INVOKABLE QString GetNotificationRecord(int index);
	Q_INVOKABLE QString GetNotificationTitle(int index);

	Q_INVOKABLE QString GetEntructDescription(int row);
	Q_INVOKABLE QString GetDepriveDescription(int row);


    enum StatusCode // Returned by sendCoins
    {
        OK,
        InvalidAmount,
        InvalidAddress,
        AmountExceedsBalance,
        AmountWithFeeExceedsBalance,
        DuplicateAddress,
        TransactionCreationFailed, // Error returned when wallet is still locked
        TransactionCommitFailed,
        AnonymizeOnlyUnlocked,
        InsaneFee
    };

    enum DateEnum {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

    enum EncryptionStatus {
        Unencrypted = 0,                 // !wallet->IsCrypted()
        Locked = 1,                      // wallet->IsCrypted() && wallet->IsLocked()
        Unlocked = 2,                    // wallet->IsCrypted() && !wallet->IsLocked()
        UnlockedForAnonymizationOnly = 3 // wallet->IsCrypted() && !wallet->IsLocked() && wallet->fWalletUnlockAnonymizeOnly
    };

	QString message_;

    OptionsModel* getOptionsModel();
    AddressTableModel* getAddressTableModel();
    TransactionTableModel* getTransactionTableModel();
    RecentRequestsTableModel* getRecentRequestsTableModel();
    ContractTableModel *getContractTableModel();
    TokenItemModel *getTokenItemModel();
    TokenTransactionTableModel *getTokenTransactionTableModel();

    bool isMineAddress(const std::string &strAddress);
    bool existTokenEntry(const CTokenInfo &token);
    bool addTokenEntry(const CTokenInfo &token);
    bool removeTokenEntry(const std::string &sHash);
    bool isUnspentAddress(const std::string &qtumAddress) const;
    bool addTokenTxEntry(const CTokenTx& tokenTx, bool fFlushOnClose=true);
    void setClientModel(ClientModel* clientModel);

    void emitTraySignal(int index);

    CAmount getBalance(const CCoinControl* coinControl = NULL) const;
    CAmount getUnconfirmedBalance() const;
    CAmount getImmatureBalance() const;
    CAmount getLockedBalance() const;
    CAmount getZerocoinBalance() const;
    CAmount getUnconfirmedZerocoinBalance() const;
    CAmount getImmatureZerocoinBalance() const;
    bool haveWatchOnly() const;
    CAmount getWatchBalance() const;
    CAmount getWatchUnconfirmedBalance() const;
    CAmount getWatchImmatureBalance() const;
    CKey generateNewKey() const; //for temporary paper wallet key generation
    bool setAddressBook(const CTxDestination& address, const string& strName, const string& strPurpose);
    void encryptKey(const CKey key, const std::string& pwd, const std::string& slt, std::vector<unsigned char>& crypted);
    void decryptKey(const std::vector<unsigned char>& crypted, const std::string& slt, const std::string& pwd, CKey& key);



    // Return status record for SendCoins, contains error id + information
    struct SendCoinsReturn {
        SendCoinsReturn(StatusCode status = OK) : status(status) {}
        StatusCode status;
    };

    // prepare transaction for getting txfee before sending coins
    SendCoinsReturn prepareTransaction(WalletModelTransaction& transaction, const CCoinControl* coinControl = NULL);

    // Send coins to a list of recipients
    SendCoinsReturn sendCoins(WalletModelTransaction& transaction);

    // Is wallet unlocked for anonymization only?
    bool isAnonymizeOnlyUnlocked();

    // RAI object for unlocking wallet, returned by requestUnlock()
    class UnlockContext
    {
    public:
        UnlockContext(bool valid, bool relock);
        ~UnlockContext();

        bool isValid() const { return valid; }

        // Copy operator and constructor transfer the context
        UnlockContext(const UnlockContext& obj) { CopyFrom(obj); }
        UnlockContext& operator=(const UnlockContext& rhs)
        {
            CopyFrom(rhs);
            return *this;
        }

    private:
        bool valid;
        mutable bool relock; // mutable, as it can be set to false by copying

        void CopyFrom(const UnlockContext& rhs);
    };

    UnlockContext requestUnlock(bool relock = false);

    bool getPubKey(const CKeyID& address, CPubKey& vchPubKeyOut) const;
    bool isMine(CBitcoinAddress address);
    void getOutputs(const std::vector<COutPoint>& vOutpoints, std::vector<COutput>& vOutputs);
    bool isSpent(const COutPoint& outpoint) const;
    void listCoins(std::map<QString, std::vector<COutput> >& mapCoins) const;

    bool isLockedCoin(uint256 hash, unsigned int n) const;
    void lockCoin(COutPoint& output);
    void unlockCoin(COutPoint& output);
    void listLockedCoins(std::vector<COutPoint>& vOutpts);

    void listZerocoinMints(std::list<CZerocoinMint>& listMints, bool fUnusedOnly = false, bool fMaturedOnly = false, bool fUpdateStatus = false);

    void loadReceiveRequests(std::vector<std::string>& vReceiveRequests);
    bool saveReceiveRequest(const std::string& sAddress, const int64_t nId, const std::string& sRequest);

    void setSyncStatus(bool status)
    {
            if(status)
                m_syncStatus = "";
            else
                m_syncStatus = "Out of sync";

            emit syncStatusChanged();
    }

private:
    CWallet* wallet;
    bool fHaveWatchOnly;
    bool fHaveMultiSig;
    bool fForceCheckBalanceChanged;

    // Wallet has an options model for wallet-specific options
    // (transaction fee, for example)
    OptionsModel* optionsModel;

    AddressTableModel* addressTableModel;
    ContractTableModel *contractTableModel;

    AddressFilterProxy *receivingAddressProxyModel_;
    AddressFilterProxy *sendingAddressProxyModel_;

    TransactionTableModel* transactionTableModel;

	EntrustNodeListModel* entrustNodeListModel;
	EntrustNodeListProxy* entrustNodeListProxy;
    
	
	EntrustRecordListModel* entrustRecordsModel;
	EntrustRecordListProxy* entrustRecordsProxy;
	
	EntrustRecordListModel* redeemRecordsModel;
	EntrustRecordListProxy* redeemRecordsProxy;

	NotificationListModel* notificationRecordsModel;
	NotificationListProxy* notificationRecordsProxy;
    
    MainNodeListModel* mainNodesModel;
    MainNodeListProxy* mainNodeListProxy;

    TransactionFilterProxy* transactionProxyModel;
    TransactionFilterProxy* transactionProxyModelOverView;


    ContractFilterProxy* contractfilterproxy_;
    CreateContractPage* contractPage_;

    MasterNodeTableModel *masterNodeTableModel;
    MasterNodeTableProxy *masternodetableproxy_;


    RecentRequestsTableModel* recentRequestsTableModel;
    RecentRequestsFilterProxy* recentRequestsFilterProxy_;


    TokenItemModel *tokenItemModel_;
    TokenTransactionTableModel *tokenTransactionTableModel;
    TokenFilterProxy *tokenfilterproxy_;

    CoinControlModel *coinControlModel;
    CoinControlProxy *coinControlProxy_;


    // Cache some values to be able to detect changes
    CAmount cachedBalance;
    CAmount cachedUnconfirmedBalance;
    CAmount cachedImmatureBalance;
	CAmount cachedTotelBalance;
    CAmount cachedZerocoinBalance;
    CAmount cachedUnconfirmedZerocoinBalance;
    CAmount cachedImmatureZerocoinBalance;
    CAmount cachedWatchOnlyBalance;
    CAmount cachedWatchUnconfBalance;
    CAmount cachedWatchImmatureBalance;
    CAmount cachedEntrusts_;
    CAmount cachedProfits_;
    EncryptionStatus cachedEncryptionStatus;
    int cachedNumBlocks;
    int cachedTxLocks;
    int cachedZeromintPercentage;

    QList<quint32> typeList;
    QList<quint32> dateList;

    QTimer* pollTimer;
    QTimer* backupTimer;

    QString m_syncStatus;

	std::vector<PAIR> vecAdverise_;
	std::vector<PAIR> vecSysmsg_;

    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();
    void checkBalanceChanged();
    QString GetAddressStatus();
    void checkTokenBalanceChanged();
    QString getBIPAddressFromSeeds(const char *key64, int chainType);

signals:
    // Signal that balance in wallet changed
    void balanceChanged(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, 
                        const CAmount& totalBalance,
						const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance, 
                        const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);
    
    void entrustChanged(const CAmount& balance,const CAmount& cachedEntrusts,
                        const CAmount& cachedProfits);
    // Encryption status of wallet changed
    void encryptionStatusChanged(int status);

	void notificationChanged(int index,QString title,QString time,QString text,QString nblock,
					QString link,QString author,QString nhash);

    // Signal emitted when wallet needs to be unlocked
    // It is valid behaviour for listeners to keep the wallet locked after this signal;
    // this means that the unlocking failed or was cancelled.
    void requireUnlock();

    void syncStatusChanged();

    void notifyBackup();


    void hideWindow();

    // Fired when a message should be reported to the user
    void message(const QString& title, const QString& message, unsigned int style);

    // Coins sent: from wallet, to recipient, in (serialized) transaction:
    void coinsSent(CWallet* wallet, SendCoinsRecipient recipient, QByteArray transaction);

    // Show progress dialog e.g. for rescan
    void showProgress(const QString& title, int nProgress);

    // Watch-only address added
    void notifyWatchonlyChanged(bool fHaveWatchonly);

    // MultiSig address added
    void notifyMultiSigChanged(bool fHaveMultiSig);

    //For QML: balande change notify;

    void pollBalanceChanged_sig();


    void traySignal(int index);
    void badWords();
    void existingAddress();
    void addAddressSuccessful(QString address);
	void addTokenEntryEmit();

public slots:
    /* Wallet status might have changed */
    void updateStatus();
    /* New transaction, or transaction changed status */
    void updateTransaction();
    /* New, updated or removed address book entry */
    void updateAddressBook(const QString& address, const QString& label, bool isMine, const QString& purpose, int status);
    /* Zerocoin update */
    void updateAddressBook(const QString &pubCoin, const QString &isUsed, int status);
    /* Watch-only added */
    void updateWatchOnlyFlag(bool fHaveWatchonly);
    /* MultiSig added */
    void updateMultiSigFlag(bool fHaveMultiSig);
    /* Current, immature or unconfirmed balance might have changed - emit 'balanceChanged' if so */
    void pollBalanceChanged();

    void emitBalanceChanged(); // Force update of UI-elements even when no values have changed


    void updateContractBook(const QString &address, const QString &label, const QString &abi, int status);

private slots:
    void checkForInvalidTokens();
    void backupCheck();

};

#endif // BITCOIN_QT_WALLETMODEL_H
