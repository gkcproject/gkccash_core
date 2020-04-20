#include "entrustrecordlistmodel.h"
#include "main.h"
#include "utilmoneystr.h"
#include <QDebug>

EntrustRecordListModel::EntrustRecordListModel(CWallet* pwallet,QObject* parent) :QAbstractListModel(parent)
{
	pwalletMain = pwallet;
}
void EntrustRecordListModel::Add(EntrustRecord& er)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    entrustRecords.push_back(er);
    endInsertRows();
}
void EntrustRecordListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, entrustRecords.size());
    entrustRecords.clear();
    endRemoveRows();
}

void  EntrustRecordListModel::minsert(int index, const QString &txid,const double &vout,
                const QString &agent, const double &amount)
{
    EntrustRecord  er(txid, vout,agent,amount);
    beginInsertRows(QModelIndex(), index, index);
    entrustRecords.insert(entrustRecords.begin()+index, er);
    endInsertRows();
}
void  EntrustRecordListModel::mremove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    entrustRecords.erase(entrustRecords.begin() + index);
    endRemoveRows();
}
void EntrustRecordListModel::pushdata(const QString &txid,const double &vout,
                const QString &agent, const double &amount)
{
    EntrustRecord  er(txid, vout,agent,amount);
    Add(er);
}
int EntrustRecordListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return entrustRecords.size();
}
QVariant EntrustRecordListModel::data(const QModelIndex &index, int role)  const
{
    if (index.row() < 0 || index.row() >= entrustRecords.size())
    {
        return QVariant();
    }
    const EntrustRecord& er = entrustRecords[index.row()];
	if (role == TxID)
    {
        return er.TXID();
    }
    else if (role == Vout)
    {
        return er.VOUT();
    }
    else if (role == Agent)
    {
        return er.AGENT();
    }
    else if (role == Amount)
    {
        return er.AMOUNT();
    }
    else if (role == Operation)
    {
        return er.obj;
    }
    return QVariant();
}

void EntrustRecordListModel::UpdateEntrustRecordList()
{
	clear();
    std::vector<COutPoint> vOutpts;
	pwalletMain->ListLockedCoins(vOutpts);
    totalAmount_ = 0;
    for (COutPoint& output : vOutpts)
	{
		CTransaction tx;
		uint256 hashBlock;
		if(GetTransaction(output.hash, tx, hashBlock, true))
		{
			if(pwalletMain->IsSpent(output.hash,output.n))
				continue;
			const CTxOut& txout = tx.vout[output.n];
			if(pwalletMain->IsMine(txout) && txout.IsEntrusted())
			{
				EntrustRecord er(QString::fromStdString(output.hash.GetHex()),
                                output.n,
                                QString::fromStdString(tx.agentid.GetHex()),
                                txout.nValue/1.0e+08);
				totalAmount_ += txout.nValue;
				Add(er);
			}
		}
	}	
}

void EntrustRecordListModel::UpdateRedeemRecordList()
{
	clear();
    std::vector<COutPoint> vOutpts;
	pwalletMain->GetDepriveTxOut(vOutpts);
    totalAmount_ = 0;
    for (COutPoint& output : vOutpts)
	{
		CTransaction tx;
		uint256 hashBlock;
		if(GetTransaction(output.hash, tx, hashBlock, true))
		{
			const CTxOut& txout = tx.vout[output.n];
			if(tx.IsDeprive() && pwalletMain->IsMine(txout) && !txout.IsEntrusted())
			{
				EntrustRecord er(QString::fromStdString(output.hash.GetHex()),
                                output.n,
                                QString::fromStdString(tx.agentid.GetHex()),
                                txout.nValue/1.0e+08);
                totalAmount_ += txout.nValue;
				Add(er);
			}
		}
	}	
}

EntrustRecord EntrustRecordListModel::GetEntrustRecord(int row)
{
    EntrustRecord er("",0,"",0);
    if(row<0||row>=entrustRecords.size())
    {
        return er;
    }
	return entrustRecords[row];
}

CAmount EntrustRecordListModel::GetTotalAmount()
{
    return totalAmount_;
}

QString EntrustRecordListModel::GetTXID(int row)
{
    if(row<0||row>=entrustRecords.size())
    {
        return "";
    }
    EntrustRecord er = entrustRecords[row];
    return er.TXID();
}

QString EntrustRecordListModel::GetDescription(int row)
{
    if(row<0||row>=entrustRecords.size())
    {
        return "";
    }
    EntrustRecord er = entrustRecords[row];
    QString tips = "You can use address signature messages to prove that you have them.Be careful not to sign anything you do not understand,because phishing attacks may try to trick you into signing.Only sign what you know and understand and agree with.You can use address signature messages to prove that you have them.";
    QString html = "";
    html += "<html><font face='verdana, arial, helvetica, sans-serif'>";
    html += "<b>" + tr("NodeID") + ":</b> " + er.AGENT();
    html += "<br>";
    html += "<b>" + tr("EntrustNumber") + ":</b> " + QString::number(er.AMOUNT()) + "GKC";
    html += "<br>";
    html += "<b style=\"color:#FDA205;\"> Tips:</b>" + tips;
    return html;
}

QHash<int, QByteArray> EntrustRecordListModel::roleNames() const
{
    QHash<int, QByteArray>  er;
    er[Agent] = "agent";
    er[Amount] = "amount";
    er[TxID] = "txid";
    er[Vout] = "vout";
    er[Operation] = "operation";
    return  er;
}
