#ifndef NODELISTMODEL_H
#define NODELISTMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <vector>
class NodeInfo
{
public:
    NodeInfo(const QString& id, const QString& name,const double& entrust, const int &userNum,const double &totalDealDivide,
			const double &totalDividend):nodeID(id),nodeName(name),userNum(userNum),totalDealDivide(totalDealDivide),
                                                    entrust(entrust),totalDividend(totalDividend)
    {

    }
    QString  NodeID() const { return this->nodeID; }
	QString NodeName() const {return this->nodeName;}
    double  Entrust() const { return this->entrust; }    
    double  TotalDividend() const { return this->totalDividend; }
	int UserNum() const {return this->userNum;}
	double TotalDealDivide() const {return this->totalDealDivide;}
    QVariant  obj;
private:
    QString  nodeID;
	QString nodeName;
	int userNum;
    double entrust;
	double totalDealDivide;
    double totalDividend;
};


class EntrustNodeListModel  :public QAbstractListModel
{
    Q_OBJECT
public:
    enum datatype{
        NodeID,
		NodeName,
		UserNum,
        Entrust,
		TotalDealDivide,
        TotalDividend,
		Operation
    };
    EntrustNodeListModel(QObject*  parent=NULL);

    void pushdata(const QString& id,const QString& name, const double& entrust,const int &userNum,const double &totalDealDivide,const double& totalDividend);
    void mremove(int index);
    void Add(NodeInfo&  md);
    void clear();
    QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent  = QModelIndex() ) const;
    QHash<int, QByteArray> roleNames()  const;

	Q_INVOKABLE void UpdateNodeList();
	NodeInfo GetNodeInfo(int row);

    Q_INVOKABLE void setcuritem(int index , QVariant  j)
    {
        nodeList[index].obj = j;
    }

    ~EntrustNodeListModel() {}


private:
    std::vector<NodeInfo>  nodeList;
};

#endif