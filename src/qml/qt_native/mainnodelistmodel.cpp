#include "mainnodelistmodel.h"
#include <QDebug>
#include "main.h"
#include <QDateTime>
#include "utilmoneystr.h"
#include "masternode.h"
#include "masternodeman.h"

MainNodeListModel::MainNodeListModel(QObject* parent) :QAbstractListModel(parent)
{
	
}
void MainNodeListModel::Add(MainNodeInfo& node)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    nodeList.push_back(node);
    endInsertRows();
}
void MainNodeListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, nodeList.size());
    nodeList.clear();
    endRemoveRows();
}

void  MainNodeListModel::mremove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    nodeList.erase(nodeList.begin() + index);
    endRemoveRows();
}
void MainNodeListModel::pushdata(const QString& address, const QString& status, const QString& version,
        const QString& findtime,const QString& runtime,const QString& paymenttime)
{
    MainNodeInfo node(address, status,version,findtime,runtime,paymenttime);
    Add(node);
}
int MainNodeListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodeList.size();
}
QVariant MainNodeListModel::data(const QModelIndex &index, int role)  const
{
    if (index.row() < 0 || index.row() >= nodeList.size())
    {
        return QVariant();
    }
    const MainNodeInfo& node = nodeList[index.row()];
	if (role == Address)
    {
        return node.Address();
    }
    else if (role == Status)
    {
        return node.Status();
    }
	else if (role == Version)
	{
		return node.Version();
	}
	else if (role == Findtime)
	{
		return node.Findtime();
	}
    else if (role == Runtime)
    {
        return node.Runtime();
    }
    else if (role == Paymenttime)
    {
        return node.Paymenttime();
    }
    else if (role == Operation)
    {
        return node.obj;
    }
    return QVariant();
}

void MainNodeListModel::UpdateMainNodeList()
{
	clear();
	int nHeight;
    {
        LOCK(cs_main);
        CBlockIndex* pindex = chainActive.Tip();
        if(!pindex) return;
        nHeight = pindex->nHeight;
    }
    QDateTime dateTime;
    std::vector<pair<int, CMasternode> > vMasternodeRanks = mnodeman.GetMasternodeRanks(nHeight);
    BOOST_FOREACH (PAIRTYPE(int, CMasternode) & s, vMasternodeRanks) {
		CMasternode* mn = mnodeman.Find(s.second.vin);
        if (mn != NULL) {
			QString  address = QString(CBitcoinAddress(mn->pubKeyCollateralAddress.GetID()).ToString().c_str());
			QString  status = QString(mn->Status().c_str());
            QString  version = QString::number(mn->protocolVersion);
            QString  findtime = dateTime.fromSecsSinceEpoch(mn->sigTime).toString("yyyy-MM-dd hh:mm:ss");
            QString  runtime;
            long time = mn->lastPing.sigTime - mn->sigTime;
            if(time<0) 
            {
                time = abs(time);
                runtime.append("-");
            }
            int day = (int)(time/86400);
            runtime.append(QString::number(day));
            runtime.append("days,");
            time = time % 86400;
            int hours = (int)(time/3600);
            runtime.append(QString::number(hours));
            runtime.append("hours,");
            time = time % 3600;
            int fen = (int)(time/60);
            runtime.append(QString::number(fen));
            runtime.append("min");
            
            QString  paymenttime;
            if(mn->GetLastPaid()==0)
            {
                paymenttime = "-";
            }
            else
            {
                paymenttime = dateTime.fromSecsSinceEpoch(mn->GetLastPaid()).toString("yyyy-MM-dd hh:mm:ss");
            }
            pushdata(address,status,version,findtime,runtime,paymenttime);
		}
	}
}

MainNodeInfo MainNodeListModel::GetNodeInfo(int row)
{
    MainNodeInfo node("","","","","","");
    if(row<0||row>=nodeList.size())
    {
        return node;
    }
	return nodeList[row];
}

QHash<int, QByteArray> MainNodeListModel::roleNames() const
{
    QHash<int, QByteArray>  node;
    node[Address] = "address";
	node[Status] = "status";
    node[Version] = "version";
	node[Findtime] = "findtime";
    node[Runtime] = "runtime";
    node[Paymenttime] = "paymenttime";
    node[Operation] = "operation";
    return  node;
}