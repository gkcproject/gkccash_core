#ifndef COINCONTROLMODEL_H
#define COINCONTROLMODEL_H
#include "walletmodel.h"

#include <QAbstractTableModel>
#include <QStringList>


static const int MAX_SEND_POPUP_ENTRIES = 10;


class CCoinControl;


class CoinControlModel : public QAbstractListModel
{
    Q_OBJECT


public:
   explicit CoinControlModel( WalletModel *parent = 0,  bool MultisigEnabled = false);


    enum ColumnIndex {
        Status = 0,
        Date = 1,
        Type = 2,
        Address = 3,
        Label = 4,
        Confirmations = 5,
        Priority = 6,
        Amount = 7
    };

    enum RoleIndex {
        /** Type of transaction */
        TypeRole = Qt::UserRole,
        DateRole,
        AddressRole,
        LabelRole,
        AmountRole,
        StatusRole,
        SelectionRole,
        ConfirmedRole,
        PriorityRole,
        TxHashRole,
        };



    bool fSplitBlock;

    QVariantList updateView(QVariantList payAmountList);
    void toggle();
    void selectAll();
    void updateSplitUtxo(bool checked,const QString &utxo, const QString &afterFee);
    QString updatecustomChangeAddress(bool checked, const QString &address);
    void setValue(int index, QVariant value);
    void sendButtonClicked(QVariantList recipientsArray,
                           bool splitBlockCheckBoxChecked,
                           int splitBlockSize,
                           bool swiftTXChecked);

    void setEnabled(bool enable);


    void confirmSending();
    QVariant getValue(int index);
    QVariantList updateCoinControlLabels(QVariantList payAmountList);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role);

    static QList<CAmount> payAmounts;
    static CCoinControl* coinControl;
    static int nSplitBlockDummy;

    static QString getPriorityLabel(double dPriority, double mempoolEstimatePriority);
    static QVariantList updateLabelsFunc(WalletModel* model, std::map<QString, bool> &mapSelection);

protected:
    QHash<int, QByteArray> roleNames() const ;

private slots:
    void updateSmartFeeLabel();


private:
    WalletModel* model;
    bool fMultisigEnabled;
    bool fNewRecipientAllowed;
    WalletModelTransaction *currentTransaction = NULL;
    std::map<QString, std::vector<COutput>> mapCoins;
    std::vector<std::pair<COutput,QString>> vecOuts;

    std::map<QString,bool> mapSelection;

    QString strPad(QString s, int nPadLength, QString sPadding) const;
    void updateFeeMinimizedLabel(int value);
    void updateGlobalFeeVariables();
    void send(QList<SendCoinsRecipient> recipients, QString strFee, QStringList formatted);
    void processSendCoinsReturn(const WalletModel::SendCoinsReturn& sendCoinsReturn, const QString& msgArg = QString(), bool fPrepare = false);


signals:
    void updateLabelBlockSize(QString size);
    void updateCoinControlLabelsSig();
    void updateSmartFeeLabels(QVariantList returnList);
    void notifySendingResult(int type,QString title,QString msg);

};

#endif // COINCONTROLMODEL_H
