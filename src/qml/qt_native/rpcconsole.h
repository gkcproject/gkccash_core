// Copyright (c) 2011-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_RPCCONSOLE_H
#define BITCOIN_QT_RPCCONSOLE_H

#include "guiutil.h"
#include "peertablemodel.h"

#include "peertableproxy.h"

#include "singleitemmodel.h"
#include "net.h"

#include <QDialog>
#include <QCompleter>

class ClientModel;

namespace Ui
{
class RPCConsole;
}

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

/** Local Bitcoin RPC console. */
class RPCConsole : public QObject
{
    Q_OBJECT

    Q_PROPERTY(SingleItemModel *hintModel MEMBER m_hintModel CONSTANT)
    //Q_PROPERTY(QSortFilterProxyModel *peerTableModel_sorted MEMBER peerTableModel CONSTANT)
    Q_PROPERTY(PeerTableProxy *peerTableModel_sorted MEMBER peerTableproxy CONSTANT)


public:
    explicit RPCConsole(QObject *parent = nullptr);
    ~RPCConsole();

    static bool parseCommandLine(std::vector<std::string>& args, const std::string& strCommand);


    void setClientModel(ClientModel* model);

    Q_INVOKABLE int hintRowCount();
    Q_INVOKABLE void findhint(QString text);
    Q_INVOKABLE void on_lineEdit_returnPressed(QString cmd);
    Q_INVOKABLE void startFromQML();
    Q_INVOKABLE QString browseHistory(int offset);
    Q_INVOKABLE QString getStartUpInfo();
    Q_INVOKABLE void openDebugLogFile();
    Q_INVOKABLE QString setTrafficGraphRange(int mins);
    Q_INVOKABLE QString getStartUpTraffic();
    Q_INVOKABLE ClientModel *getClientModel();


    Q_INVOKABLE QString getWalletPath();
    Q_INVOKABLE void changeNetWork();

    /** Wallet repair options */
    Q_INVOKABLE void walletSalvage();
    Q_INVOKABLE void walletRescan();
    Q_INVOKABLE void walletZaptxes1();
    Q_INVOKABLE void walletZaptxes2();
    Q_INVOKABLE void walletUpgrade();
    Q_INVOKABLE void walletReindex();
    Q_INVOKABLE void walletResync();
    /** Open external (default) editor with gkc.conf */
    Q_INVOKABLE void showConfEditor();
    /** Open external (default) editor with masternode.conf */
    Q_INVOKABLE void showMNConfEditor();
    /** Show folder with wallet backups in default browser */
    Q_INVOKABLE void showBackups();




    enum MessageClass {
        MC_ERROR,
        MC_DEBUG,
        CMD_REQUEST,
        CMD_REPLY,
        CMD_ERROR
    };


protected:

private slots:
    void on_tabWidget_currentChanged(int index);
    /** change the time range of the network traffic graph */
    /** update traffic statistics */
    void updateTrafficStats(quint64 totalBytesIn, quint64 totalBytesOut);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

public slots:
    void clear();



    void reject();
    void message(int category, const QString& message, bool html = false);
    /** Set number of connections shown in the UI */
    void setNumConnections(int count);
    /** Set number of blocks shown in the UI */
    void setNumBlocks(int count);
    /** Set number of masternodes shown in the UI */
    void setMasternodeCount(const QString& strMasternodes);
    /** Go forward or back in history */
    /** Scroll console view to end */
    /** Switch to info tab and show */
    /** Switch to console tab and show */
    /** Switch to network tab and show */
    /** Switch to peers tab and show */


    /** Handle selection of peer in peers list */
    void peerSelected(const QItemSelection& selected, const QItemSelection& deselected);
    /** Handle updated peer information */
    void peerLayoutChanged();


signals:
    // For RPC command executor
    void stopExecutor();
    void cmdRequest(const QString& command);
    /** Get restart command-line parameters and handle restart */
    void handleRestart(QStringList args);
    void newMessage(QString message);
    void infoUpdate(QString info);
    void trafficUpdate(QString traffic);
    void requestedRestart(QStringList args);

private:
    static QString FormatBytes(quint64 bytes);
    void startExecutor();
    /** Build parameter list for restart */
    void buildParameterlist(QString arg);
    /** show detailed information on ui about selected node */
    void updateNodeDetail(const CNodeCombinedStats* stats);
    void emittinginfoUpdate();

    enum ColumnWidths {
        ADDRESS_COLUMN_WIDTH = 170,
        SUBVERSION_COLUMN_WIDTH = 140,
        PING_COLUMN_WIDTH = 80
    };

    QString clientName;
    QString clientversion;
    QString openSSLVersion;
    QString berkeleyDBVersion;
    QString buildDate;
    QString startupTime;

    QString networkName;
    QString connections;
    QString MasternodeCount;

    QString NumBlocks;
    QString LastBlockDate;


    ClientModel* clientModel;
    PeerTableModel* peerTableModel = NULL;
    PeerTableProxy* peerTableproxy = NULL;
    QStringList history;
    QStringList wordList;

    int historyPtr;
    NodeId cachedNodeid;
    QCompleter *autoCompleter;
    SingleItemModel *m_hintModel;
};

#endif // BITCOIN_QT_RPCCONSOLE_H
