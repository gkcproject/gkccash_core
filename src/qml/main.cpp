#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include <QQuickWindow>
#include <QQuickView>
#include <QQmlContext>
#include "cursorposprovider.h"
#include <QQuickStyle>
#include <QTextCodec>
#include <QFontDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QLibraryInfo>

#include <QTranslator>
#include <QDebug>

#include <QScreen>
#include "lightwallet.h"
#include "qt_native/imageprovider.h"

#include "bitcoinheaders.h"
#include "bitcoinapplication.h"
#include "qt_native/trafficgraphwidget.h"

#include "qt_native/tokenitemmodel.h"

#include "qt_native/intro.h"

//#include "walletmanager.h"

#ifdef Q_OS_WIN32
#include "nativeeventfilter.h"
#endif



ImageProvider *imageProvider = NULL;

static bool fDaemon;

Q_DECLARE_METATYPE(bool*)
Q_DECLARE_METATYPE(CAmount)


void DetectShutdownThread(boost::thread_group* threadGroup)
{
    bool fShutdown = ShutdownRequested();
    // Tell the main threads to shutdown.
    while (!fShutdown) {
        MilliSleep(200);
        fShutdown = ShutdownRequested();
    }
    if (threadGroup) {
        threadGroup->interrupt_all();
        threadGroup->join_all();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Start
//

bool pickDataDirectory(int argc, char *argv[])
{
	namespace fs = boost::filesystem;
    QSettings settings;
	if (!GetArg("-datadir", "").empty())
        return true;

	QString dataDir = Intro::getDefaultDataDirectory();
	dataDir = settings.value("strDataDir", dataDir).toString();
	SoftSetArg("-datadir",GUIUtil::qstringToBoostPath(dataDir).string());
	QString datafile = dataDir + "/wallet.dat";
	QFile file(datafile);
	if (!fs::exists(GUIUtil::qstringToBoostPath(dataDir)) || GetBoolArg("-choosedatadir", false) || !file.exists()) {
		MnemonicManager *mne = new MnemonicManager();

		QGuiApplication a(argc, argv);

		QQmlApplicationEngine engine;
		engine.rootContext()->setContextProperty("mnemonicManager", mne);
		engine.load(QUrl(QStringLiteral("qrc:/qml/app_pages/CreateWalletPage.qml")));
		QObject *root = engine.rootObjects().at(0);

		a.exec();
		

		QString mnemonicWords = mne->GetMnemonicWords();
		std::cout << "mnemonicWords:" << mnemonicWords.toStdString() << "\n";
		if(mnemonicWords!=""){				
			SoftSetArg("-mnemonic",mnemonicWords.toStdString());
		}else{
			std::cout << "mnemonic words is empty!\n";
			return false;
		}

		try {
            TryCreateDirectory(GUIUtil::qstringToBoostPath(dataDir));
        } catch (fs::filesystem_error& e) {
             QMessageBox::critical(0, QString("GKC Core"),
                    QString("Error: Specified data directory \"%1\" cannot be created.").arg(dataDir));
        
        }
	}
	return true;
}


double getScaleRate()
{
    double rate = 0;

    double ratio_ = 0;
    QList<QScreen*> screens = QApplication::screens();

    if (screens.size() > 0) {


        QScreen* screen = screens[0];



        double dpi = screen->logicalDotsPerInch();
        ratio_ = screen->devicePixelRatio();


        rate = dpi / screen->physicalDotsPerInch();

        if (rate < 1.1) {
            rate = 1.0;
        } else if (rate < 1.4) {
            rate = 1.25;
        } else if (rate < 1.6) {
            rate = 1.5;
        } else if (rate < 1.8) {
            rate = 1.75;
        } else {
            rate = 2.0;
        }
    }

    return ratio_;
}




bool AppInit(int argc, char* argv[])
{
    boost::thread_group threadGroup;
    boost::thread* detectShutdownThread = NULL;

    bool fRet = false;

    //
    // Parameters
    //
    // If Qt is used, parameters/gkc.conf are parsed in qt/gkc.cpp's main()
    ParseParameters(argc, argv);

    // Process help and version before taking care about datadir
    if (mapArgs.count("-?") || mapArgs.count("-help") || mapArgs.count("-version")) {
        std::string strUsage = _("Gkc Core Daemon") + " " + _("version") + " " + FormatFullVersion() + "\n";

        if (mapArgs.count("-version")) {
            strUsage += LicenseInfo();
        } else {
            strUsage += "\n" + _("Usage:") + "\n" +
                    "  gkcd [options]                     " + _("Start Gkc Core Daemon") + "\n";

            strUsage += "\n" + HelpMessage(HMM_BITCOIND);
        }

        fprintf(stdout, "%s", strUsage.c_str());
        return false;
    }

    try {
        if (!boost::filesystem::is_directory(GetDataDir(false))) {
            fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", mapArgs["-datadir"].c_str());
            return false;
        }
        try {
            ReadConfigFile(mapArgs, mapMultiArgs);
        } catch (std::exception& e) {
            fprintf(stderr, "Error reading configuration file: %s\n", e.what());
            return false;
        }
        // Check for -testnet or -regtest parameter (Params() calls are only valid after this clause)
        if (!SelectParamsFromCommandLine()) {
            fprintf(stderr, "Error: Invalid combination of -regtest and -testnet.\n");
            return false;
        }

        // parse masternode.conf
        std::string strErr;
        if (!masternodeConfig.read(strErr)) {
            fprintf(stderr, "Error reading masternode configuration file: %s\n", strErr.c_str());
            return false;
        }

        // Command-line RPC
        bool fCommandLine = false;
        for (int i = 1; i < argc; i++)
            if (!IsSwitchChar(argv[i][0]) && !boost::algorithm::istarts_with(argv[i], "gkc:"))
                fCommandLine = true;

        if (fCommandLine) {
            fprintf(stderr, "Error: There is no RPC client functionality in gkcd anymore. Use the gkc-cli utility instead.\n");
            exit(1);
        }
#ifndef WIN32
        fDaemon = GetBoolArg("-daemon", false);
        if (fDaemon) {
            fprintf(stdout, "GKC server starting\n");

            // Daemonize
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error: fork() returned %d errno %d\n", pid, errno);
                return false;
            }
            if (pid > 0) // Parent process, pid is child process id
            {
                return true;
            }
            // Child process falls through to rest of initialization

            pid_t sid = setsid();
            if (sid < 0)
                fprintf(stderr, "Error: setsid() returned %d errno %d\n", sid, errno);
        }
#endif
        SoftSetBoolArg("-server", true);

        detectShutdownThread = new boost::thread(boost::bind(&DetectShutdownThread, &threadGroup));
        fRet = AppInit2(threadGroup);
    } catch (std::exception& e) {
        PrintExceptionContinue(&e, "AppInit()");
    } catch (...) {
        PrintExceptionContinue(NULL, "AppInit()");
    }

    if (!fRet) {
        if (detectShutdownThread)
            detectShutdownThread->interrupt();

        threadGroup.interrupt_all();
        // threadGroup.join_all(); was left out intentionally here, because we didn't re-test all of
        // the startup-failure cases to make sure they don't result in a hang due to some
        // thread-blocking-waiting-for-another-thread-during-startup case
    }

    if (detectShutdownThread) {
        detectShutdownThread->join();
        delete detectShutdownThread;
        detectShutdownThread = NULL;
    }
    Shutdown();

    return fRet;
}

void DebugMessageHandler2(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);
    const char* category = (type == QtDebugMsg) ? "qt" : NULL;
    LogPrint(category, "GUI: %s\n", msg.toStdString());
}

static void InitMessage(const std::string& message)
{
    LogPrintf("init message: %s\n", message);
}


static std::string Translate(const char* psz)
{
    return QCoreApplication::translate("gkc-core", psz).toStdString();
}

static QString GetLangTerritory()
{
    QSettings settings;
    // Get desired locale (e.g. "de_DE")
    // 1) System default language
    QString lang_territory = QLocale::system().name();
    // 2) Language from QSettings
    QString lang_territory_qsettings = settings.value("language", "").toString();
    if (!lang_territory_qsettings.isEmpty())
        lang_territory = lang_territory_qsettings;
    // 3) -lang command line argument
    lang_territory = QString::fromStdString(GetArg("-lang", lang_territory.toStdString()));
    return lang_territory;
}

/** Set up translations */
static void initTranslations(QTranslator& qtTranslatorBase, QTranslator& qtTranslator, QTranslator& translatorBase, QTranslator& translator)
{
    // Remove old translators
    QApplication::removeTranslator(&qtTranslatorBase);
    QApplication::removeTranslator(&qtTranslator);
    QApplication::removeTranslator(&translatorBase);
    QApplication::removeTranslator(&translator);

    // Get desired locale (e.g. "de_DE")
    // 1) System default language
    QString lang_territory = GetLangTerritory();

    // Convert to "de" only by truncating "_DE"
    QString lang = lang_territory;
    lang.truncate(lang_territory.lastIndexOf('_'));

    // Load language files for configured locale:
    // - First load the translator for the base language, without territory
    // - Then load the more specific locale translator

    // Load e.g. qt_de.qm
    if (qtTranslatorBase.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&qtTranslatorBase);

    // Load e.g. qt_de_DE.qm
    if (qtTranslator.load("qt_" + lang_territory, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        QApplication::installTranslator(&qtTranslator);

    // Load e.g. bitcoin_de.qm (shortcut "de" needs to be defined in gkc.qrc)
    if (translatorBase.load("gkc_" +lang, ":/locale/")){

        QApplication::installTranslator(&translatorBase);
    }


    // Load e.g. bitcoin_de_DE.qm (shortcut "de_DE" needs to be defined in gkc.qrc)
    if (translator.load("gkc_" +lang_territory, ":/locale/")){

        QApplication::installTranslator(&translator);
    }
}


#ifdef Q_OS_WIN32
bool CheckAeroEnabled()
{
    if (QSysInfo::kernelVersion().split('.').at(0).toInt() >= 6)
    {
        int enabled = 0;
        DwmIsCompositionEnabled(&enabled);
        return (enabled == 1) ? true : false;
    }
    return false;
}
#endif

#ifdef Q_OS_WIN32
void SwitchOnDropShadow()
{
   SystemParametersInfo(SPI_SETDROPSHADOW,0, (PVOID)TRUE, 0);
}
#endif



//------------------- Main -----------------------

int main(int argc, char *argv[])
{
    qDebug()<<"---Begin Main---";

    SetupEnvironment();

    ParseParameters(argc, argv);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


 #ifdef Q_OS_WIN
     QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
 #else
     QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
 #endif



    //QML Style
    QQuickStyle::setStyle("Material");


#ifdef  Q_OS_WIN32
    //Font Style Family and Format
    QFont font("Microsoft Yahei");
    QGuiApplication::setFont(font);
#endif

    // WalletManager walletManager;
    if(!pickDataDirectory(argc, argv))
		return 0;

    BitcoinApplication app(argc, argv);

    //app.walletManager = &walletManager;

    qRegisterMetaType<bool*>();
    //   Need to pass name here as CAmount is a typedef (see http://qt-project.org/doc/qt-5/qmetatype.html#qRegisterMetaType)
    //   IMPORTANT if it is no longer a typedef use the normal variant above
    qRegisterMetaType<CAmount>("CAmount");

    qRegisterMetaType<ClientModel*>("ClientModel");
    qRegisterMetaType<TokenItemModel*>("TokenItemModel");


    //qmlRegisterType<TrafficGraphWidget>("TrafficGraphWidget", 1, 0, "TrafficGraphWidget");
    qmlRegisterType<TrafficGraphWidget>("TrafficGraphWidget", 1, 0, "TrafficGraphWidget");


    QApplication::setOrganizationName(QAPP_ORG_NAME);
    QApplication::setOrganizationDomain(QAPP_ORG_DOMAIN);
    QApplication::setApplicationName(QAPP_APP_NAME_DEFAULT);

    GUIUtil::SubstituteFonts(GetLangTerritory());


    // Now that QSettings are accessible, initialize translations
    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;
    initTranslations(qtTranslatorBase, qtTranslator, translatorBase, translator);
    uiInterface.Translate.connect(Translate);


    //if (!Intro::pickDataDirectory())
    //    return 0;


    if (!boost::filesystem::is_directory(GetDataDir(false))) {
        QMessageBox::critical(0, QObject::tr("GKC Core"),
                              QObject::tr("Error: Specified data directory \"%1\" does not exist.").arg(QString::fromStdString(mapArgs["-datadir"])));
        return 1;
    }

    try {
        ReadConfigFile(mapArgs, mapMultiArgs);
    } catch (std::exception& e) {
        QMessageBox::critical(0, QObject::tr("GKC Core"),
                              QObject::tr("Error: Cannot parse configuration file: %1. Only use key=value syntax.").arg(e.what()));
        return 0;
    }


    if (!SelectParamsFromCommandLine()) {
        QMessageBox::critical(0, QObject::tr("GKC Core"), QObject::tr("Error: Invalid combination of -regtest and -testnet."));
        return 1;
    }

    // Parse URIs on command line -- this can affect Params()
    PaymentServer::ipcParseCommandLine(argc, argv);


    NetworkStyle  *networkStyle;
    if(GetBoolArg("-testnet", false))
        networkStyle =  new NetworkStyle("main","GKC-Qt",":/images/icons/bitcoin",":/images/icons/splash_testnet");
    else
        networkStyle =  new NetworkStyle("main","GKC-Qt",":/images/icons/bitcoin",":/images/icons/splash");


    // Allow for separate UI settings for testnets
    QApplication::setApplicationName(networkStyle->getAppName());

    string strErr;
    if (!masternodeConfig.read(strErr)) {
        QMessageBox::critical(0, QObject::tr("GKC Core"),
                              QObject::tr("Error reading masternode configuration file: %1").arg(strErr.c_str()));
        return 0;
    }

    if (PaymentServer::ipcSendCommandLine())
        exit(0);

    // Start up the payment server early, too, so impatient users that click on
    // gkc: links repeatedly have their payment requests routed to this process:
    app.createPaymentServer();

    //qInstallMessageHandler(DebugMessageHandler);

    app.createOptionsModel();
    app.createSplashScreen(networkStyle);

    // Subscribe to global signals from core
    uiInterface.InitMessage.connect(InitMessage);

    //QML Environment
    QQmlApplicationEngine engine;
    CursorPosProvider mousePosProvider;

    engine.rootContext()->setContextProperty("mousePosition", &mousePosProvider);
    engine.rootContext()->setContextProperty("bitcoinapp", &app);
    engine.addImageProvider("ReQuestURI",imageProvider = new ImageProvider());

    app.root_context = engine.rootContext();

    app.startToolsAndMessageCore();

    app.requestInitialize();

    //To do extra setContextProperty for qml .
    QEventLoop loop;
    QObject::connect(&app, SIGNAL(splashFinished()), &loop, SLOT(quit()));
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()){
        return -1;

    }

    //Wallet

    LightWallet lightWallet;

    //Windows Explorer
    QObject *root = engine.rootObjects()[0];




    QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    window->setFlags(Qt::FramelessWindowHint|Qt::Window);


#ifdef Q_OS_WIN32
    HWND hwnd = (HWND)(window->winId());
    bool m_aeroEnabled = CheckAeroEnabled();

    SwitchOnDropShadow();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    if(!m_aeroEnabled)
        style |= CS_DROPSHADOW;

    //::SetClassLong(hwnd, GWL_STYLE, style);

    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX| WS_MINIMIZEBOX|WS_THICKFRAME|WS_CAPTION);

    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    int window_height = rect.bottom - rect.top;
    int window_width = rect.right - rect.left;

    NativeEventFilter *nativeeventfilter = new NativeEventFilter();
    nativeeventfilter->winId = hwnd;
    nativeeventfilter->desktop_height = window_height;

    nativeeventfilter->desktop_width = window_width;
    nativeeventfilter->main_window = window;
    nativeeventfilter->m_aeroEnabled = m_aeroEnabled;

    nativeeventfilter->scale_rate = getScaleRate();


    app.installNativeEventFilter(nativeeventfilter);
#endif


    app.exec();


    app.requestShutdown();
    app.exec();
    //    } catch (std::exception& e) {
    //        PrintExceptionContinue(&e, "Runaway exception");
    //        app.handleRunawayException(QString::fromStdString(strMiscWarning));
    //    } catch (...) {
    //        PrintExceptionContinue(NULL, "Runaway exception");
    //        app.handleRunawayException(QString::fromStdString(strMiscWarning));
    //    }

    return app.getReturnValue();


    //  return (AppInit(argc, argv) ? 0 : 1);

    //
    //  QGuiApplication app(argc, argv);




    //    const MARGINS shadow = { 1, 1, 1, 1 };
    //    DwmExtendFrameIntoClientArea(HWND(window->winId()), &shadow);



}






