#ifndef MASTERNODETABLEMODEL_H
#define MASTERNODETABLEMODEL_H

#include "masternode.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QAbstractTableModel>

class ClientModel;
class WalletModel;
#define MY_MASTERNODELIST_UPDATE_SECONDS 60
#define MASTERNODELIST_UPDATE_SECONDS 15
#define MASTERNODELIST_FILTER_COOLDOWN_SECONDS 3


class MasterNodeTableModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MasterNodeTableModel(WalletModel* parent);


    enum ColumnIndex {
        Alias = 0,
        Address = 1,
        Protocol = 2,
        Status = 3,
        Activated = 4,
        LastSeen = 5,
        Pubkey = 6
    };

    enum RoleIndex {
        /** Type of transaction */
        AliasRole = Qt::UserRole,
        AddressRole,
        ProtocolRole,
        StatusRole,
        ActivatedRole,
        LastSeenRole,
        PubkeyRole,
    };


    void setClientModel(ClientModel* clientmodel_);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;


    void updateMyNodeList(bool fForce = false);
    void startButtonClicked(int row);
    void startAll(QString strCommand = "start-all");
    void startMissing();

protected:
    QHash<int, QByteArray> roleNames() const ;


private:
    WalletModel *model = NULL;
    ClientModel *clientModel = NULL;

    CCriticalSection cs_mnlistupdate;

    typedef struct nodeItem_
    {
        QString aliasItem;
        QString addrItem;
        QString protocolItem;
        QString statusItem;
        QString activeSecondsItem;
        QString lastSeenItem;
        QString pubkeyItem;
    }nodeItem;


    std::vector<nodeItem> nodes;

    void updateMyMasternodeInfo(QString strAlias, QString strAddr, CMasternode* pmn);
    void startAlias(std::string strAlias);

signals:
    void message(QString title, QString msg);
    void setTimer(int time);

};

#endif // MASTERNODETABLEMODEL_H
