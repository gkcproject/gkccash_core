#include "bitcoinapplication.h"
#include "qt_native/notificator.cpp"

#include "qt_native/transactiontablemodel.h"

static bool ThreadSafeMessageBox(BitcoinApplication* gui, const std::string& message, const std::string& caption, unsigned int style)
{
    bool modal = (style & CClientUIInterface::MODAL);
    // The SECURE flag has no effect in the Qt GUI.
    // bool secure = (style & CClientUIInterface::SECURE);
    style &= ~CClientUIInterface::SECURE;
    bool ret = false;
    // In case of modal message, use blocking connection to wait for user to click a button
    QMetaObject::invokeMethod(gui, "message",
                              modal ? GUIUtil::blockingGUIThreadConnection() : Qt::QueuedConnection,
                              Q_ARG(QString, QString::fromStdString(caption)),
                              Q_ARG(QString, QString::fromStdString(message)),
                              Q_ARG(unsigned int, style),
                              Q_ARG(bool*, &ret));
    return ret;
}




BitcoinApplication::BitcoinApplication(int& argc, char** argv): QApplication(argc, argv)
{
    setQuitOnLastWindowClosed(false);
}

void BitcoinApplication::startThread()
{
    if (coreThread)
        return;
    coreThread = new QThread(this);
    executor = new BitcoinCore();
    executor->moveToThread(coreThread);

    /*  communication to and from thread */
    connect(executor, SIGNAL(initializeResult(int)), this, SLOT(initializeResult(int)));
    connect(executor, SIGNAL(shutdownResult(int)), this, SLOT(shutdownResult(int)));
    connect(executor, SIGNAL(runawayException(QString)), this, SLOT(handleRunawayException(QString)));


    connect(this, SIGNAL(requestedInitialize()), executor, SLOT(initialize()));
    connect(this, SIGNAL(requestedShutdown()), executor, SLOT(shutdown()));

    /*  make sure executor object is deleted in its own thread */
    connect(this, SIGNAL(stopThread()), executor, SLOT(deleteLater()));
    connect(this, SIGNAL(stopThread()), coreThread, SLOT(quit()));

    connect(this, SIGNAL(requestedRestart(QStringList)), executor, SLOT(restart(QStringList)));

    coreThread->start();


    qDebug()<<"BitcoinApplication.thread:"<<QThread::currentThread();

}




BitcoinApplication::~BitcoinApplication()
{
    uiInterface.ThreadSafeMessageBox.disconnect(boost::bind(ThreadSafeMessageBox, this, _1, _2, _3));

    if(trayIconMenu){
        delete trayIconMenu;
        trayIconMenu = 0;
    }

    if (coreThread) {
        qDebug() << __func__ << ": Stopping thread";
        emit stopThread();
        coreThread->wait();
        qDebug() << __func__ << ": Stopped thread";
    }

    if(paymentServer){
        delete paymentServer;
        paymentServer = 0;
    }

    // Delete Qt-settings if user clicked on "Reset Options"
    QSettings settings;
    if (optionsModel && optionsModel->resetSettings) {
        settings.clear();
        settings.sync();
    }

    if(optionsModel){
        delete optionsModel;
        optionsModel = 0;
    }

}



void BitcoinApplication::createTrayIcon()
{
#ifndef Q_OS_MAC
    QIcon appIcon(":/images/icons/app.png");
    trayIcon = new QSystemTrayIcon(this);
    QString toolTip = tr("GKC Core client");
    trayIcon->setToolTip(toolTip);
    trayIcon->setIcon(appIcon);
    trayIcon->hide();
#endif

    notificator = new Notificator(QApplication::applicationName(), trayIcon, NULL);
}

#ifndef Q_OS_MAC
void BitcoinApplication::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        // Click on system tray icon triggers show/hide of the main window
        // toggleHidden(); //TODO: Show normal or minimize
    }
}
#endif

void BitcoinApplication::createTrayIconMenu()
{
#ifndef Q_OS_MAC
    // return if trayIcon is unset (only on non-Mac OSes)
    if (!trayIcon)
        return;

    trayIconMenu = new QMenu(NULL);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler* dockIconHandler = MacDockIconHandler::instance();
    dockIconHandler->setMainWindow((QMainWindow*)this);
    trayIconMenu = dockIconHandler->dockMenu();
#endif

    // Configuration of the tray icon (or dock icon) icon menu
    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(sendCoinsAction);
    trayIconMenu->addAction(receiveCoinsAction);
    trayIconMenu->addAction(privacyAction);
//    trayIconMenu->addSeparator();
//    trayIconMenu->addAction(signMessageAction);
//    trayIconMenu->addAction(verifyMessageAction);
//    trayIconMenu->addAction(bip38ToolAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(optionsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openInfoAction);
    trayIconMenu->addAction(openRPCConsoleAction);
    trayIconMenu->addAction(openNetworkAction);
    trayIconMenu->addAction(openPeersAction);
    trayIconMenu->addAction(openRepairAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openConfEditorAction);
    trayIconMenu->addAction(openMNConfEditorAction);
    trayIconMenu->addAction(showBackupsAction);
    trayIconMenu->addAction(openBlockExplorerAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif

    connect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(emitTraySignal(QAction*)));

}


void BitcoinApplication::emitTraySignal(QAction* action)
{

    int index = 0;
    if(action == toggleHideAction)
        index = 0;
    else if(action == sendCoinsAction)
         index = 1;
    else if(action == receiveCoinsAction)
        index = 2;
    else if(action == privacyAction)
        index = 3;
//    else if(action == signMessageAction)
//        index = 4;
//    else if(action == verifyMessageAction)
//        index = 5;
//    else if(action == bip38ToolAction)
//        index = 6;
    else if(action == optionsAction)
        index = 7;
    else if(action == openInfoAction)
        index = 8;
    else if(action == openRPCConsoleAction)
        index = 9;
    else if(action == openNetworkAction)
        index = 10;
    else if(action == openPeersAction)
        index = 11;
    else if(action == openRepairAction)
        index = 12;
    else if(action == openConfEditorAction)
        index = 13;
    else if(action == openMNConfEditorAction)
        index = 14;
    else if(action == showBackupsAction)
        index = 15;
    else if(action == openBlockExplorerAction)
        index = 16;
#ifndef Q_OS_MAC // This is built-in on Mac
    else if(action == quitAction)
        index = 17;
#endif

    walletModel->emitTraySignal(index);

}



void BitcoinApplication::createPaymentServer()
{
    paymentServer = new PaymentServer(this);
}

void BitcoinApplication::createOptionsModel()
{
    optionsModel = new OptionsModel();

}

void BitcoinApplication::createSplashScreen(const NetworkStyle* networkStyle)
{
    SplashScreen* splash = new SplashScreen(0, networkStyle);
    // We don't hold a direct pointer to the splash screen after creation, so use
    // Qt::WA_DeleteOnClose to make sure that the window will be deleted eventually.
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();
    connect(this, SIGNAL(splashFinished()), splash, SLOT(slotFinish()));
}


void BitcoinApplication::requestInitialize()
{
    qDebug() << __func__ << ": Requesting initialize";
    startThread();
    emit requestedInitialize();
}



void BitcoinApplication::requestShutdown()
{
    qDebug() << __func__ << ": Requesting shutdown";
    startThread();

    // window->hide();
    // window->setClientModel(0);
    //pollShutdownTimer->stop();

    // window->removeAllWallets();
    emit walletModel->hideWindow();


    delete walletModel;
    walletModel = 0;

    if(clientModel)
    {
        delete clientModel;
        clientModel = 0;
    }

    // Show a simple window indicating shutdown status
    // ShutdownWindow::showShutdownWindow(window);

    //ShutdownWindow::showShutdownWindow();


    // Request shutdown from core thread
    emit requestedShutdown();
}

void BitcoinApplication::pollBalanceChanged_sig_slot()
{

}

void BitcoinApplication::numBlocksChanged_slot(int count)
{
    if (!clientModel || !walletModel)
        return;

    QString title;
    QString msg;
    QString tooltip;

    int progress = 0;
    int progress_max = 0;

    // Acquire current block source
    enum BlockSource blockSource = clientModel->getBlockSource();
    switch (blockSource) {
    case BLOCK_SOURCE_NETWORK:
        title = tr("Synchronizing with network...");
        break;
    case BLOCK_SOURCE_DISK:
        title = tr("Importing blocks from disk...");
        break;
    case BLOCK_SOURCE_REINDEX:
        title = tr("Reindexing blocks on disk...");
        break;
    case BLOCK_SOURCE_NONE:
        // Case: not Importing, not Reindexing and no network connection
        title = tr("No block source available...");
        break;
    }


    QDateTime lastBlockDate = clientModel->getLastBlockDate();
    QDateTime currentDate = QDateTime::currentDateTime();
    int secs = lastBlockDate.secsTo(currentDate);

    tooltip = tr("Processed %n blocks of transaction history.", "", count);

    // Set icon state: spinning if catching up, tick otherwise
    //    if(secs < 25*60) // 90*60 for bitcoin but we are 4x times faster
    if (masternodeSync.IsBlockchainSynced()) {
        QString strSyncStatus;
        tooltip = tr("Up to date") + QString(".<br>") + tooltip;

        if (masternodeSync.IsSynced()) {
            //TODO:
        } else {
            int nAttempt;



            nAttempt = masternodeSync.RequestedMasternodeAttempt < MASTERNODE_SYNC_THRESHOLD ?
                        masternodeSync.RequestedMasternodeAttempt + 1 :
                        MASTERNODE_SYNC_THRESHOLD;
            progress = nAttempt + (masternodeSync.RequestedMasternodeAssets - 1) * MASTERNODE_SYNC_THRESHOLD;
            progress_max = 4 * MASTERNODE_SYNC_THRESHOLD;
            msg = tr("Synchronizing additional data: ");
        }

        walletModel->setSyncStatus(true);

        strSyncStatus = QString(masternodeSync.GetSyncStatus().c_str());
        title = strSyncStatus;
        tooltip = strSyncStatus + QString("<br>") + tooltip;
    } else {
        // Represent time from last generated block in human readable text
        QString timeBehindText;
        const int HOUR_IN_SECONDS = 60 * 60;
        const int DAY_IN_SECONDS = 24 * 60 * 60;
        const int WEEK_IN_SECONDS = 7 * 24 * 60 * 60;
        const int YEAR_IN_SECONDS = 31556952; // Average length of year in Gregorian calendar
        if (secs < 2 * DAY_IN_SECONDS) {
            timeBehindText = tr("%n hour(s)", "", secs / HOUR_IN_SECONDS);
        } else if (secs < 2 * WEEK_IN_SECONDS) {
            timeBehindText = tr("%n day(s)", "", secs / DAY_IN_SECONDS);
        } else if (secs < YEAR_IN_SECONDS) {
            timeBehindText = tr("%n week(s)", "", secs / WEEK_IN_SECONDS);
        } else {
            int years = secs / YEAR_IN_SECONDS;
            int remainder = secs % YEAR_IN_SECONDS;
            timeBehindText = tr("%1 and %2").arg(tr("%n year(s)", "", years)).arg(tr("%n week(s)", "", remainder / WEEK_IN_SECONDS));
        }

        walletModel->setSyncStatus(false);

        msg = tr("%1 behind. Scanning block %2").arg(timeBehindText).arg(count);
        progress_max = 1000000000;
        progress = clientModel->getVerificationProgress() * 1000000000.0 + 0.5;


        tooltip = tr("Catching up...") + QString("<br>") + tooltip;
        if (count != prevBlocks) {
            //TODO:
        }
        prevBlocks = count;


        tooltip += QString("<br>");
        tooltip += tr("Last received block was generated %1 ago.").arg(timeBehindText);
        tooltip += QString("<br>");
        tooltip += tr("Transactions after this will not yet be visible.");
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");

    emit numBlocksChanged(progress,progress_max,title,msg,tooltip);

}

void BitcoinApplication::balanceChanged_slot()
{

   // qDebug()<<"balanceChanging!";
}

void BitcoinApplication::initializeResult(int retval)
{
    qDebug() << __func__ << ": Initialization result: " << retval;
    // Set exit result: 0 if successful, 1 if failure
    returnValue = retval ? 0 : 1;
    if (retval) {
#ifdef ENABLE_WALLET
        PaymentServer::LoadRootCAs();
        paymentServer->setOptionsModel(optionsModel);
#endif

        clientModel = new ClientModel(optionsModel);
        rpcConsole->setClientModel(clientModel);


        startUtilityCore();
        blockExplorer = new BlockExplorer();
        root_context->setContextProperty("blockExplorer", blockExplorer);

#ifdef ENABLE_WALLET
        if (pwalletMain) {
            walletModel = new WalletModel(pwalletMain, optionsModel);


            root_context->setContextProperty("walletModel", walletModel);
            //root_context->setContextProperty("clientModel", clientModel);
            root_context->setContextProperty("optionsModel", optionsModel);
            walletModel->setClientModel(clientModel);
            //window->addWallet(BitcoinApplication::DEFAULT_WALLET, walletModel);



            // window->setCurrentWallet(BitcoinApplication::DEFAULT_WALLET);

            connect(walletModel, SIGNAL(message(QString, QString, unsigned int)), this, SLOT(message(QString, QString, unsigned int)));



            connect(walletModel->getTransactionTableModel(), SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(processNewTransaction(QModelIndex, int, int)));


            connect(walletModel, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)),
                    this, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)));

            connect(walletModel, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)),
                    this, SLOT(balanceChanged_slot()));


            //TODO: Handle changes in encryption status and unlock
//            connect(walletModel, SIGNAL(encryptionStatusChanged(int)), this, SIGNAL(encryptionStatusChanged(int)));
//            updateEncryptionStatus();
//            connect(walletModel, SIGNAL(requireUnlock()), this, SLOT(unlockWallet()));


            connect(clientModel, SIGNAL(numBlocksChanged(int)), this, SLOT(numBlocksChanged_slot(int)));


            connect(walletModel, SIGNAL(pollBalanceChanged_sig()),this,SIGNAL(pollBalanceChanged_sig()));
            connect(walletModel, SIGNAL(pollBalanceChanged_sig()),this,SLOT(pollBalanceChanged_sig_slot()));




            connect(walletModel, SIGNAL(coinsSent(CWallet*, SendCoinsRecipient, QByteArray)),
                    paymentServer, SLOT(fetchPaymentACK(CWallet*, const SendCoinsRecipient&, QByteArray)));
        }
#endif

        // If -min option passed, start window minimized.
        //        if (GetBoolArg("-min", false)) {
        //            window->showMinimized();
        //        } else {
        //            window->show();
        //        }

        emit splashFinished();

        createTrayIconMenu();
        if (trayIcon)
        {
          trayIcon->show();
        }

#ifdef ENABLE_WALLET
        // Now that initialization/startup is done, process any command-line
        // GKC: URIs or payment requests:
        //        connect(paymentServer, SIGNAL(receivedPaymentRequest(SendCoinsRecipient)),
        //            window, SLOT(handlePaymentRequest(SendCoinsRecipient)));
        //        connect(window, SIGNAL(receivedURI(QString)),
        //            paymentServer, SLOT(handleURIOrFile(QString)));
        //        connect(paymentServer, SIGNAL(message(QString, QString, unsigned int)),
        //            window, SLOT(message(QString, QString, unsigned int)));
        QTimer::singleShot(100, paymentServer, SLOT(uiReady()));
#endif
    } else {
        quit(); // Exit main loop
    }
}


void BitcoinApplication::incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address)
{

    // Only send notifications when not disabled
    if(!bdisableSystemnotifications){
        // On new transaction, make an info balloon
        message((amount) < 0 ? (pwalletMain->fMultiSendNotify == true ? tr("Sent MultiSend transaction") : tr("Sent transaction")) : tr("Incoming transaction"),
            tr("Date: %1\n"
               "Amount: %2\n"
               "Type: %3\n"
               "Address: %4\n")
                .arg(date)
                .arg(BitcoinUnits::formatWithUnit(unit, amount, true))
                .arg(type)
                .arg(address),
            CClientUIInterface::MSG_INFORMATION);

        pwalletMain->fMultiSendNotify = false;
    }
}


void BitcoinApplication::processNewTransaction(const QModelIndex& parent, int start, int /*end*/)
{
    // Prevent balloon-spam when initial block download is in progress
    if (!walletModel || !clientModel || clientModel->inInitialBlockDownload())
        return;

    TransactionTableModel* ttm = walletModel->getTransactionTableModel();
    if (!ttm || ttm->processingQueuedTransactions())
        return;

    QString date = ttm->index(start, TransactionTableModel::Date, parent).data().toString();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent).data(Qt::EditRole).toULongLong();
    QString type = ttm->index(start, TransactionTableModel::Type, parent).data().toString();
    QString address = ttm->index(start, TransactionTableModel::ToAddress, parent).data().toString();

    incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address);
   // emit incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address);
}

void BitcoinApplication::shutdownResult(int retval)
{
    qDebug() << __func__ << ": Shutdown result: " << retval;
    quit(); // Exit main loop after shutdown finished
}

void BitcoinApplication::handleRunawayException(const QString& message)
{
    QMessageBox::critical(0, "Runaway exception", ("A fatal error occurred. GKC can no longer continue safely and will quit.") + QString("\n\n") + message);
    ::exit(1);
}



void BitcoinApplication::message(const QString& title, const QString& message, unsigned int style, bool* ret)
{
    QString strTitle = tr("GKC Core"); // default title
    // Default to information icon
    int nMBoxIcon = QMessageBox::Information;
    int nNotifyIcon = Notificator::Information;

    QString msgType;

    // Prefer supplied title over style based title
    if (!title.isEmpty()) {
        msgType = title;
    } else {
        switch (style) {
        case CClientUIInterface::MSG_ERROR:
            msgType = tr("Error");
            break;
        case CClientUIInterface::MSG_WARNING:
            msgType = tr("Warning");
            break;
        case CClientUIInterface::MSG_INFORMATION:
            msgType = tr("Information");
            break;
        default:
            break;
        }
    }
    // Append title to "GKC - "
    if (!msgType.isEmpty())
        strTitle += " - " + msgType;

    // Check for error/warning icon
    if (style & CClientUIInterface::ICON_ERROR) {
        nMBoxIcon = QMessageBox::Critical;
        nNotifyIcon = Notificator::Critical;
    } else if (style & CClientUIInterface::ICON_WARNING) {
        nMBoxIcon = QMessageBox::Warning;
        nNotifyIcon = Notificator::Warning;
    }

    // Display message
    if (style & CClientUIInterface::MODAL) {
        // Check for buttons, use OK as default, if none was supplied
        QMessageBox::StandardButton buttons;
        if (!(buttons = (QMessageBox::StandardButton)(style & CClientUIInterface::BTN_MASK)))
            buttons = QMessageBox::Ok;

        //showNormalIfMinimized(); //TODO: What is the intention?
        QMessageBox mBox((QMessageBox::Icon)nMBoxIcon, strTitle, message, buttons, NULL);
        int r = mBox.exec();
        if (ret != NULL)
            *ret = r == QMessageBox::Ok;
    } else{
        notificator->notify((Notificator::Class)nNotifyIcon, strTitle, message);
    }
}




void BitcoinApplication::startToolsAndMessageCore()
{
    rpcConsole = new RPCConsole(enableWallet ? this : 0);
    root_context->setContextProperty("rpcConsole", rpcConsole);

    connect(rpcConsole, SIGNAL(requestedRestart(QStringList)), this, SLOT(prepareToRestart(QStringList)));


    uiInterface.ThreadSafeMessageBox.connect(boost::bind(ThreadSafeMessageBox, this, _1, _2, _3));


    createTrayIcon();


    //sendCoinsAction = new QAction(QIcon(":/icons/send"), tr("&Send"), this);
    sendCoinsAction = new QAction( tr("&Send"), this);
    sendCoinsAction->setStatusTip(tr("Send coins to a GKC address"));
    sendCoinsAction->setToolTip(sendCoinsAction->statusTip());
    sendCoinsAction->setCheckable(true);
#ifdef Q_OS_MAC
    sendCoinsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
#else
    sendCoinsAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_2));
#endif

   // receiveCoinsAction = new QAction(QIcon(":/icons/receiving_addresses"), tr("&Receive"), this);
    receiveCoinsAction = new QAction(tr("&Receive"), this);
    receiveCoinsAction->setStatusTip(tr("Request payments (generates QR codes and gkc: URIs)"));
    receiveCoinsAction->setToolTip(receiveCoinsAction->statusTip());
    receiveCoinsAction->setCheckable(true);
#ifdef Q_OS_MAC
    receiveCoinsAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
#else
    receiveCoinsAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_3));
#endif


    //privacyAction = new QAction(QIcon(":/icons/privacy"), tr("&Privacy"), this);
    privacyAction = new QAction(tr("&Privacy"), this);
    privacyAction->setStatusTip(tr("Privacy Actions for zGKC"));
    privacyAction->setToolTip(privacyAction->statusTip());
    privacyAction->setCheckable(true);
#ifdef Q_OS_MAC
    privacyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
#else
    privacyAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_5));
#endif




    //quitAction = new QAction(QIcon(":/icons/quit"), tr("E&xit"), this);
    quitAction = new QAction( tr("E&xit"), this);
    quitAction->setStatusTip(tr("Quit application"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAction->setMenuRole(QAction::QuitRole);

    //optionsAction = new QAction(QIcon(":/icons/options"), tr("&Options..."), this);
    optionsAction = new QAction(tr("&Options..."), this);
    optionsAction->setStatusTip(tr("Modify configuration options for GKC"));
    optionsAction->setMenuRole(QAction::PreferencesRole);

    toggleHideAction = new QAction( tr("&Show / Hide"), this);
    //toggleHideAction = new QAction(networkStyle->getAppIcon(), tr("&Show / Hide"), this);
    toggleHideAction->setStatusTip(tr("Show or hide the main Window"));


//    signMessageAction = new QAction( tr("Sign &message..."), this);
//    //signMessageAction = new QAction(QIcon(":/icons/edit"), tr("Sign &message..."), this);
//    signMessageAction->setStatusTip(tr("Sign messages with your GKC addresses to prove you own them"));

//    verifyMessageAction = new QAction( tr("&Verify message..."), this);
//    //verifyMessageAction = new QAction(QIcon(":/icons/transaction_0"), tr("&Verify message..."), this);
//    verifyMessageAction->setStatusTip(tr("Verify messages to ensure they were signed with specified GKC addresses"));

//    bip38ToolAction = new QAction( tr("&BIP38 tool"), this);
//    //bip38ToolAction = new QAction(QIcon(":/icons/key"), tr("&BIP38 tool"), this);
//    bip38ToolAction->setToolTip(tr("Encrypt and decrypt private keys using a passphrase"));


    openInfoAction = new QAction(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("&Information"), this);
    openInfoAction->setStatusTip(tr("Show diagnostic information"));

    openRPCConsoleAction = new QAction(tr("&Debug console"), this);
    //openRPCConsoleAction = new QAction(QIcon(":/icons/debugwindow"), tr("&Debug console"), this);
    openRPCConsoleAction->setStatusTip(tr("Open debugging console"));

    openNetworkAction = new QAction(tr("&Network Monitor"), this);
    //openNetworkAction = new QAction(QIcon(":/icons/connect_4"), tr("&Network Monitor"), this);
    openNetworkAction->setStatusTip(tr("Show network monitor"));

    openPeersAction = new QAction( tr("&Peers list"), this);
    //openPeersAction = new QAction(QIcon(":/icons/connect_4"), tr("&Peers list"), this);
    openPeersAction->setStatusTip(tr("Show peers info"));

    openRepairAction = new QAction(tr("Wallet &Repair"), this);
    //openRepairAction = new QAction(QIcon(":/icons/options"), tr("Wallet &Repair"), this);
    openRepairAction->setStatusTip(tr("Show wallet repair options"));

    //openConfEditorAction = new QAction(QIcon(":/icons/edit"), tr("Open Wallet &Configuration File"), this);
    openConfEditorAction = new QAction( tr("Open Wallet &Configuration File"), this);
    openConfEditorAction->setStatusTip(tr("Open configuration file"));

    //openMNConfEditorAction = new QAction(QIcon(":/icons/edit"), tr("Open &Masternode Configuration File"), this);
    openMNConfEditorAction = new QAction(tr("Open &Masternode Configuration File"), this);
    openMNConfEditorAction->setStatusTip(tr("Open Masternode configuration file"));

    //openBlockExplorerAction = new QAction(QIcon(":/icons/explorer"), tr("&Blockchain explorer"), this);
    openBlockExplorerAction = new QAction( tr("&Blockchain explorer"), this);
    openBlockExplorerAction->setStatusTip(tr("Block explorer window"));


    showBackupsAction = new QAction( tr("Show Automatic &Backups"), this);
    //showBackupsAction = new QAction(QIcon(":/icons/browse"), tr("Show Automatic &Backups"), this);
    showBackupsAction->setStatusTip(tr("Show automatically created wallet backups"));


}


void BitcoinApplication::startUtilityCore()
{
    utility = new Utility(enableWallet ? this : 0);
    root_context->setContextProperty("utility", utility);
}


void BitcoinApplication::prepareToRestart(QStringList args)
{
    emit walletModel->hideWindow();
    emit requestedRestart(args);
}
