#include "notificationlistmodel.h"
#include <QDateTime>
#include "scriptex.h"

using namespace advertisement;


NotificationListModel::NotificationListModel(CWallet* wallet,QObject* parent)
{
	wallet_ = wallet;
}

void NotificationListModel::pushdata(const QString &title,const QString &time,const QString &text,
			const QString &block,const QString &link,const QString &author,const QString &hash)
{
	NotificationRecord nr(title,time,text,block,link,author,hash);
	Add(nr);
}


void NotificationListModel::minsert(int index,const QString &title,const QString &time,const QString &text,
			const QString &block,const QString &link,const QString &author,const QString &hash)
{
	NotificationRecord nr(title,time,text,block,link,author,hash);
    beginInsertRows(QModelIndex(), index, index);
    notificationRecords_.insert(notificationRecords_.begin()+index, nr);
    endInsertRows();
}


void NotificationListModel::mremove(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
    notificationRecords_.erase(notificationRecords_.begin() + index);
    endRemoveRows();
}


void NotificationListModel::Add(NotificationRecord &nr)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
    notificationRecords_.push_back(nr);
    endInsertRows();
}


void NotificationListModel::Clear()
{
	beginRemoveRows(QModelIndex(), 0, notificationRecords_.size());
    notificationRecords_.clear();
    endRemoveRows();
}


int NotificationListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return notificationRecords_.size();
}

void NotificationListModel::UpdateNotificationRecordList(int type,int from,int count)
{
	Clear();
	if(type == 0)
	{
		GetAdveriseList(from,count);
	}
	else
	{
		GetSysMessageList(from,count);
	}
}

bool NotificationListModel::haveNextPage(int type,int from,int count)
{
	if(type==0)
	{
		return vecAdverise_.size() > (from+count);
	}
	else
	{
		return vecSysmsg_.size() > (from+count);
	}
}


void NotificationListModel::SyncAdveriseList()
{
	vecAdverise_.clear();
	std::vector<PAIR> vecAdverise(wallet_->mapAdvertise.begin(),wallet_->mapAdvertise.end());
	std::sort(vecAdverise.begin(), vecAdverise.end(), cmp_by_CWalletTx);
	int i=0;
	while(i<vecAdverise.size())
	{
		if(IsFilterAdv(vecAdverise[i].first)){
			i++;
			continue;
		}
		vecAdverise_.push_back(vecAdverise[i]);
		i++;
	}
}

void NotificationListModel::GetAdveriseList(int from,int count)
{
	if (from == 0)
	{
		SyncAdveriseList();
	}
	
	//std::cout << "GetAdveriseList from:" << from << "  count: " << count 
	//	<< " size:" << vecAdverise_.size() << "\n";
	for (int i=0;i<count;++i)
	{
		if(vecAdverise_.size()>from+i)
		{
			NotificationRecord nr = GetNotificationDetails(vecAdverise_[from+i].second);
			//std::cout << "title:" << nr.TITLE().toStdString() << " time:" << nr.TIME().toStdString()
			//	<< " text:" << nr.NTEXT().toStdString() << " block:" << nr.BLOCK().toStdString()
			//	<< " link:" << nr.LINK().toStdString() << " author:" << nr.AUTHOR().toStdString() 
			//	<< " hash:" << nr.HASH().toStdString()  << "\n";
			Add(nr);
		}
	}
}

bool NotificationListModel::IsFilterAdv(uint256 nhash)
{
	std::map<uint256, CWalletTx>::iterator iter = wallet_->mapFilterAd.begin();
	for(;iter!=wallet_->mapFilterAd.end();iter++)
	{
		if(nhash == iter->first){
			return true;
		}
	}
	return false;
}

void NotificationListModel::SyncSysmessageList()
{
	vecSysmsg_.clear();
	std::vector<PAIR> vecSysmsg(wallet_->mapSysMessage.begin(),wallet_->mapSysMessage.end());
	std::sort(vecSysmsg.begin(), vecSysmsg.end(), cmp_by_CWalletTx);
	int i=0;
	while(i<vecSysmsg.size())
	{
		if(IsFilterAdv(vecSysmsg[i].first)){
			i++;
			continue;
		}
		vecSysmsg_.push_back(vecSysmsg[i]);
		i++;
	}
}


void NotificationListModel::GetSysMessageList(int from,int count)
{
	if (from == 0)
	{
		SyncSysmessageList();
	}
	for (int i=0;i<count;++i)
	{
		if(vecSysmsg_.size()>from+i)
		{
			NotificationRecord nr = GetNotificationDetails(vecSysmsg_[from+i].second);
			Add(nr);
		}
	}
}

NotificationRecord NotificationListModel::GetNotificationDetails(CWalletTx tx)
{
	int64_t n = tx.GetTxTime();
	QDateTime dt = QDateTime::fromSecsSinceEpoch(n);
	uint256 hash = tx.GetHash();
	BOOST_FOREACH (const CTxOut& txout, tx.vout) {
		ScriptEx scriptex(txout.scriptPubKey);
		if(scriptex.IsAdvertise())
		{
			Advertise adv;
			if(adv.DecodeFrom(txout.scriptPubKey))
			{	
				
				//std::cout << "date:" << n << "time:" << dt.toString("yyyy/MM/dd hh:mm").toStdString().c_str() << "\n";
				//std::cout << "hash:" << tx.hashBlock.GetHex() << " text: " << adv.text << "\n";
				return NotificationRecord(QString::fromStdString(adv.title),dt.toString("yyyy/MM/dd hh:mm"),
						QString::fromStdString(adv.text),
						QString::number(wallet_->GetHeightWithHash(tx.hashBlock)),QString::fromStdString(adv.link),
						QString::fromStdString(adv.author),QString::fromStdString(hash.ToString()));
			}			
		}
		else if(scriptex.IsSysMessage())
		{
			SysMessage sys;
			if(sys.DecodeFrom(txout.scriptPubKey))
			{
				return NotificationRecord("System message",dt.toString("yyyy/MM/dd hh:mm"),
						QString::fromStdString(sys.text),
						QString::number(wallet_->GetHeightWithHash(tx.GetHash())),"",
						"system",QString::fromStdString(hash.ToString()));
			}
		}
	}
	return NotificationRecord("","","","","","","");
}

QString NotificationListModel::GetNotificationRecord(int index)
{
	 QString strHTML="";
	if(index >= notificationRecords_.size())
		return strHTML;
	
	NotificationRecord nr = notificationRecords_[index];
	strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";
    strHTML += "<b>" + tr("Title") + ":</b> " + nr.TITLE();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Author") + ":</b> " + nr.AUTHOR();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Time") + ":</b> " + nr.TIME();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Text") + ":</b> " + nr.NTEXT();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Block") + ":</b> " + nr.BLOCK();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Hash") + ":</b> " + nr.HASH();
    strHTML += "<br>";
    strHTML += "<b>" + tr("Link") + ":</b> <a href=\"url\">" +  nr.LINK() + "</a>";
    strHTML += "<br>";
	strHTML += "</font></html>";
	
	return strHTML;
}


QString NotificationListModel::GetNotificationTitle(int index)
{
	if(wallet_==NULL)
	{
		return "";
	}
	if(vecAdverise_.size()<=index)
	{
		return "";
	}
	NotificationRecord nr = GetNotificationDetails(vecAdverise_[index].second);
	return nr.TITLE();
}



QVariant NotificationListModel::data(const QModelIndex &index, int role)  const
{
    if (index.row() < 0 || index.row() >= notificationRecords_.size())
    {
        return QVariant();
    }
    const NotificationRecord& nr = notificationRecords_[index.row()];
	if (role == Title)
    {
        return nr.TITLE();
    }
    else if (role == Time)
    {
        return nr.TIME();
    }
    else if (role == Text)
    {
        return nr.NTEXT();
    }
    else if (role == NBlock)
    {
        return nr.BLOCK();
    }
    else if (role == Link)
    {
        return nr.LINK();
    }
	else if (role == Author)
    {
        return nr.AUTHOR();
    }
	else if (role == NHash)
    {
        return nr.HASH();
    }
    return QVariant();
}


QHash<int, QByteArray> NotificationListModel::roleNames() const
{
    QHash<int, QByteArray>  nr;
    nr[Title] = "title";
    nr[Time] = "time";
    nr[Text] = "text";
    nr[NBlock] = "block";
    nr[Link] = "link";
    nr[Author] = "author";
    nr[NHash] = "hash";
    return  nr;
}
