// Copyright (c) 2011-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_INTRO_H
#define BITCOIN_QT_INTRO_H

#include <QDialog>
#include <QMutex>
#include <QThread>
#include "mnemonicmanager.h"


class FreespaceChecker;

namespace Ui
{
class Intro;
}

/** Introduction screen (pre-GUI startup).
  Allows the user to choose a data directory,
  in which the wallet and block chain will be stored.
 */
class Intro : public QDialog
{
    Q_OBJECT

public:
    explicit Intro(QWidget* parent = 0);
    ~Intro();

    QString getDataDirectory();
    void setDataDirectory(const QString& dataDir);

    /**
     * Determine data directory. Let the user choose if the current one doesn't exist.
     *
     * @returns true if a data directory was selected, false if the user cancelled the selection
     * dialog.
     *
     * @note do NOT call global GetDataDir() before calling this function, this
     * will cause the wrong path to be cached.
     */
    static bool pickDataDirectory();

    /**
     * Determine default data directory for operating system.
     */
    static QString getDefaultDataDirectory();

signals:
    void requestCheck();
    void stopThread();

public slots:
    void setStatus(int status, const QString& message, quint64 bytesAvailable);

private slots:
    void on_dataDirectory_textChanged(const QString& arg1);
    void on_ellipsisButton_clicked();
    void on_dataDirDefault_clicked();
    void on_dataDirCustom_clicked();

private:
    Ui::Intro* ui;
    QThread* thread;
    QMutex mutex;
    bool signalled;
    QString pathToCheck;
	static MnemonicManager* mne;

    void startThread();
    void checkPath(const QString& dataDir);
    QString getPathToCheck();

    friend class FreespaceChecker;
};


#include <QThread>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>

class MnemonicThread : public QThread
{
	Q_OBJECT
public:
	MnemonicThread(MnemonicManager *mne){
		mne_ = mne;	
		isStop = false;
	}

public slots:
	void closeThread(){
		isStop = true;
	}
	
protected:
	virtual void run(){
		QQmlApplicationEngine engine;
		engine.rootContext()->setContextProperty("mnemonicManager", mne_);
		engine.load(QUrl(QStringLiteral("qrc:/qml/app_pages/CreateWalletPage.qml")));
		QObject *root = engine.rootObjects()[0];

		std::cout << "bbbbbbbbbbbbb\n";

    	QQuickWindow *window = qobject_cast<QQuickWindow *>(root);
    	window->setFlags(Qt::FramelessWindowHint
					| Qt::WindowSystemMenuHint
                 	| Qt::WindowMinimizeButtonHint
                 	| Qt::Window);
		window->show();
		while(1){
			if(isStop)
				return;
			sleep(1);
		}
	}
	

private:
	bool isStop;
	MnemonicManager *mne_;
};


#endif // BITCOIN_QT_INTRO_H
