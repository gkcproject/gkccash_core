#ifndef NOTIFICATIONLISTMODEL_H
#define NOTIFICATIONLISTMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <vector>
#include "wallet.h"

typedef pair<uint256, CWalletTx> PAIR;   

static bool cmp_by_CWalletTx(PAIR& lhs, PAIR& rhs) {  
  return lhs.second.GetTxTime() > rhs.second.GetTxTime();  
}

class NotificationRecord
{
public:
	NotificationRecord(const QString &title,const QString &time,const QString &text,
			const QString &block,const QString &link,const QString &author,
			const QString &hash):title_(title),time_(time),text_(text),block_(block),link_(link),
			author_(author),hash_(hash)
	{		
	}
	QString TITLE() const {return this->title_;}
	QString TIME() const {return this->time_;}
	QString NTEXT() const {return this->text_;}
	QString BLOCK() const {return this->block_;}
	QString LINK() const {return this->link_;}
	QString AUTHOR() const {return this->author_;}
	QString HASH() const {return this->hash_;}
	QVariant  obj;
private:
	QString title_;
	QString time_;
	QString text_;
	QString block_;
	QString link_;
	QString author_;
	QString hash_;
};

class NotificationListModel:public QAbstractListModel
{
	Q_OBJECT
public:
	enum DataType{
		Title,
		Time,
		Text,
		NBlock,
		Link,
		Author,
		NHash
	};
	NotificationListModel(CWallet* wallet,QObject* parent=NULL);
	
	void pushdata(const QString &title,const QString &time,const QString &text,
			const QString &block,const QString &link,const QString &author,const QString &hash);
	void minsert(int index,const QString &title,const QString &time,const QString &text,
			const QString &block,const QString &link,const QString &author,const QString &hash);
	void mremove(int index);
	void Add(NotificationRecord &nr);
	void Clear();
	QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent  = QModelIndex() ) const;
	QHash<int, QByteArray> roleNames()  const;
	
	Q_INVOKABLE void UpdateNotificationRecordList(int type,int from,int count);
	Q_INVOKABLE bool haveNextPage(int type,int from,int count);
	
	void GetAdveriseList(int from,int count);
	void GetSysMessageList(int from,int count);
	NotificationRecord GetNotificationDetails(CWalletTx tx);

	void SyncAdveriseList();
	bool IsFilterAdv(uint256 hash);
	void SyncSysmessageList();

	QString GetNotificationRecord(int index);
	QString GetNotificationTitle(int index);
	
	Q_INVOKABLE void setcuritem(int index , QVariant  j)
    {
        notificationRecords_[index].obj = j;
    }
	~NotificationListModel(){}
	
private:
	CWallet* wallet_;
	std::vector<NotificationRecord> notificationRecords_;
	
	std::vector<PAIR> vecAdverise_;
	std::vector<PAIR> vecSysmsg_;
};

#endif