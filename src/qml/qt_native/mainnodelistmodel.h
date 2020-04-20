#ifndef MAINNODELISTMODEL_H
#define MAINNODELISTMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <vector>
class MainNodeInfo
{
public:
    MainNodeInfo(const QString& address, const QString& status, const QString& version,
        const QString& findtime,const QString& runtime,const QString& paymenttime):address_(address),
        status_(status),version_(version),findtime_(findtime),
        runtime_(runtime),paymenttime_(paymenttime)
    {

    }
    QString  Address() const { return this->address_; }
    QString  Status() const { return this->status_; }
    QString  Version() const { return this->version_; }
    QString  Findtime() const { return this->findtime_; }
    QString  Runtime() const { return this->runtime_; }
    QString  Paymenttime() const { return this->paymenttime_; }
    QVariant  obj;
private:
    QString  address_;
    QString  status_;
    QString  version_;
    QString  findtime_;
    QString  runtime_;
    QString  paymenttime_;
};


class MainNodeListModel  :public QAbstractListModel
{
    Q_OBJECT
public:
    enum datatype{
        Address,
		Status,
        Version,
		Findtime,
        Runtime,
        Paymenttime,
		Operation
    };
    MainNodeListModel(QObject*  parent=NULL);

    void pushdata(const QString& address, const QString& status, const QString& version,
        const QString& findtime,const QString& runtime,const QString& paymenttime);
    void mremove(int index);
    void Add(MainNodeInfo&  md);
    void clear();
    QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent  = QModelIndex() ) const;
    QHash<int, QByteArray> roleNames()  const;

	Q_INVOKABLE void UpdateMainNodeList();
	MainNodeInfo GetNodeInfo(int row);

    Q_INVOKABLE void setcuritem(int index , QVariant  j)
    {
        nodeList[index].obj = j;
    }

    ~MainNodeListModel() {}


private:
    std::vector<MainNodeInfo>  nodeList;
};

#endif