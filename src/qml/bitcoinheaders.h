#ifndef BITCOINHEADERS_H
#define BITCOINHEADERS_H

#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QGuiApplication>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <QEventLoop>
#include <QDir>
#include <QDateTime>
//-----------------System----------------------------
#include "clientversion.h"
#include "init.h"
#include "main.h"
#include "masternodeconfig.h"
#include "noui.h"
#include "rpcserver.h"
#include "ui_interface.h"
#include "util.h"


#include "qt_native/paymentserver.h"
#include "qt_native/optionsmodel.h"
#include "qt_native/clientmodel.h"
#include "qt_native/splashscreen.h"
#include "qt_native/guiutil.h"
#include "qt_native/guiconstants.h"
#include "qt_native/networkstyle.h"
#include "qt_native/rpcconsole.h"
#include "qt_native/blockexplorer.h"
#include "qt_native/utilitydialog.h"




#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>



//-----------------System----------------------------


#endif // BITCOINHEADERS_H
