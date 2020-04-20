#ifndef BITCOINAPPLICATION_H
#define BITCOINAPPLICATION_H

#include "bitcoinheaders.h"
#include "bitcoincore.h"



//#include "walletmanager.h"
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class Notificator;


class BitcoinApplication : public QApplication
{
    Q_OBJECT
public:
    explicit BitcoinApplication(int& argc, char** argv);
    ~BitcoinApplication();

    void createPaymentServer();
    void createOptionsModel();
    void createSplashScreen(const NetworkStyle* networkStyle);
    void requestInitialize();
    void requestShutdown();
    int getReturnValue() { return returnValue; }
    QQmlContext *root_context;
   // WalletManager *walletManager;
    void startToolsAndMessageCore();

public slots:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    void handleRunawayException(const QString& message);

    void message(const QString& title, const QString& message, unsigned int style, bool* ret = NULL);
    void processNewTransaction(const QModelIndex& parent, int start, int /*end*/);
    void incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address);


signals:
    void stopThread();
    void requestedInitialize();
    void requestedRestart(QStringList args);
    void requestedShutdown();
    void splashFinished();
    void balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount);
    void pollBalanceChanged_sig();
    void numBlocksChanged(int progress,int progress_max,QString title,QString msg,QString tooltip);
 //   void incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address);


private slots:

    void pollBalanceChanged_sig_slot();
    void balanceChanged_slot();
    void numBlocksChanged_slot(int count);
    void prepareToRestart(QStringList args);

    void emitTraySignal(QAction* action);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:



    PaymentServer* paymentServer = NULL;


    //These items are in coreThread thread, different from Main UI.
    //TODO: Any UI binding and invokeble functions should not be started from here. Remove them all!!!!
    WalletModel* walletModel = NULL;
    ClientModel* clientModel = NULL;
    QThread* coreThread = NULL;
    BlockExplorer* blockExplorer = NULL;
    BitcoinCore* executor = NULL;
    //--------------------------------




    Utility*    utility = NULL;

    RPCConsole* rpcConsole = NULL;
    Notificator* notificator = NULL;
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;

    QAction* toggleHideAction = NULL;
    QAction* sendCoinsAction = NULL;
    QAction* receiveCoinsAction = NULL;
    QAction* privacyAction = NULL;
    QAction* signMessageAction = NULL;
    QAction* verifyMessageAction = NULL;
    QAction* bip38ToolAction = NULL;
    QAction* optionsAction = NULL;
    QAction* openInfoAction = NULL;
    QAction* openRPCConsoleAction = NULL;
    QAction* openNetworkAction = NULL;
    QAction* openPeersAction = NULL;
    QAction* openRepairAction = NULL;
    QAction* openConfEditorAction = NULL;
    QAction* openMNConfEditorAction = NULL;
    QAction* openBlockExplorerAction = NULL;
    QAction* quitAction = NULL;
    QAction* showBackupsAction = NULL;


    OptionsModel* optionsModel = NULL;
    int returnValue;
    int prevBlocks = 0;
    bool enableWallet = true;

    void startThread();
    void startUtilityCore();
    void createTrayIconMenu();
    void createTrayIcon();

};

#endif // BITCOINAPPLICATION_H
