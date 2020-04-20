#include "entrustnodelistmodel.h"
#include <QDebug>
#include "rpcserver.h"
#include "entrustment.h"
#include "main.h"
#include "utilmoneystr.h"

EntrustNodeListModel::EntrustNodeListModel(QObject* parent) :QAbstractListModel(parent)
{
	
}
void EntrustNodeListModel::Add(NodeInfo& node)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    nodeList.push_back(node);
    endInsertRows();
}
void EntrustNodeListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, nodeList.size());
    nodeList.clear();
    endRemoveRows();
}

void  EntrustNodeListModel::mremove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    nodeList.erase(nodeList.begin() + index);
    endRemoveRows();
}
void EntrustNodeListModel::pushdata(const QString& id, const QString& name,const double& entrust,
									const int &userNum,const double &totalDealDivide,const double& totalDividend)
{
    NodeInfo node(id,name, entrust,userNum,totalDealDivide,totalDividend);
    Add(node);
}
int EntrustNodeListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return nodeList.size();
}
QVariant EntrustNodeListModel::data(const QModelIndex &index, int role)  const
{
    if (index.row() < 0 || index.row() >= nodeList.size())
    {
        return QVariant();
    }
    const NodeInfo& node = nodeList[index.row()];
	if (role == NodeID)
    {
        return node.NodeID();
    }
	else if (role == NodeName)
	{
		return node.NodeName();
	}
    else if (role == Entrust)
    {
        return node.Entrust();
    }
	else if (role == UserNum)
	{
		return node.UserNum();
	}
	else if (role == TotalDealDivide)
	{
		return node.TotalDealDivide();
	}
    else if (role == TotalDividend)
    {
        return node.TotalDividend();
    }
    else if (role == Operation)
    {
        return node.obj;
    }
    return QVariant();
}

void EntrustNodeListModel::UpdateNodeList()
{
	clear();
	std::vector<Agent> agents = Entrustment::GetInstance().GetAgentList(chainActive.Height());
	for (const Agent& agent : agents) {
		CAmount totalDealDivide = 0;
		CAmount totalDividend = 0;
		std::map<CBitcoinAddress,dpos::UserStatistics>::const_iterator it = agent.userStatistics.begin();
		for(;it != agent.userStatistics.end(); it++)
		{
			totalDividend += it->second.totalDividendReward;
			totalDealDivide += it->second.totalDealDivideReward;
		}
		
		pushdata(QString::fromStdString(agent.Id().ToString()),QString::fromStdString(agent.Name()),
				 agent.receivedTotalAmount/1.0e+08,agent.userStatistics.size(),
				 totalDealDivide/1.0e+08,totalDividend/1.0e+08);
		
	}
}

NodeInfo EntrustNodeListModel::GetNodeInfo(int row)
{
    NodeInfo node("","",0,0,0,0);
    if(row<0||row>=nodeList.size())
    {
        return node;
    }
	return nodeList[row];
}

QHash<int, QByteArray> EntrustNodeListModel::roleNames() const
{
    QHash<int, QByteArray>  node;
    node[NodeID] = "nodeid";
	node[NodeName] = "nodename";
	node[UserNum] = "usernum";
    node[Entrust] = "entrust";
	node[TotalDealDivide] = "totaldealdivide";
    node[TotalDividend] = "totaldividend";
    node[Operation] = "operation";
    return  node;
}