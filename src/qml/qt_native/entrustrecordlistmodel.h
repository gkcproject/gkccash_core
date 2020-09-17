#ifndef ENTRUSTRECORFLISTMODEL_H
#define ENTRUSTRECORFLISTMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <vector>
#include "entrustment.h"
#include "wallet.h"

class EntrustRecord
{
public:
    EntrustRecord(const QString &txid,const double &vout,const QString &agent,
                const double &amount,const QString &agentname)
                :txid_(txid),vout_(vout),agent_(agent),amount_(amount),agentname_(agentname)
    {
    }
    
    QString TXID() const {return this->txid_;}
    double VOUT() const {return this->vout_;}
    QString AGENT() const {return this->agent_;}
    double AMOUNT() const {return this->amount_;}
	QString AGENTNAME() const {return this->agentname_;}
    QVariant  obj;
private:
    QString txid_;
    double vout_;
    QString agent_;
    double amount_;
	QString agentname_;
};

class EntrustRecordListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataType{
        TxID,
        Vout,
        AgentHash,
        Amount,
        AgentName,
		Operation
    };
    EntrustRecordListModel(CWallet* pwallet, QObject*  parent=NULL);

    void pushdata(const QString &txid,const double &vout,const QString &agent,
                const double &amount,const QString &agentname);
    void minsert(int index, const QString &txid,const double &vout,
                const QString &agent, const double &amount,const QString &agentname);
    void mremove(int index);
    void Add(EntrustRecord&  er);
    void clear();
    QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent  = QModelIndex() ) const;
    QHash<int, QByteArray> roleNames()  const;

	Agent* GetAgentName(AgentID id);

	Q_INVOKABLE void UpdateEntrustRecordList();
	Q_INVOKABLE void UpdateRedeemRecordList();
	EntrustRecord GetEntrustRecord(int row);
    CAmount GetTotalAmount();
    QString GetDescription(int row);
    QString GetEntructDescription(int row);
	QString GetDepriveDescription(int row);
    QString GetTXID(int row);

    Q_INVOKABLE void setcuritem(int index , QVariant  j)
    {
        entrustRecords[index].obj = j;
    }

    ~EntrustRecordListModel() {}


private:
	CWallet* pwalletMain;
    std::vector<EntrustRecord>  entrustRecords;
    CAmount totalAmount_;
};

#endif