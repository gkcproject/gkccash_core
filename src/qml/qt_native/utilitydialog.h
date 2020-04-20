// Copyright (c) 2011-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_UTILITYDIALOG_H
#define BITCOIN_QT_UTILITYDIALOG_H

#include <QDialog>
#include <QObject>
#include <QApplication>
class BitcoinGUI;
class ClientModel;
#include <QTextDocument>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QTextTable>
#include <QQuickTextDocument>
//namespace Ui
//{
//class HelpMessageDialog;
//}

class Utility : public QObject
{
    Q_OBJECT

public:
    explicit Utility(QObject *parent = nullptr);
    ~Utility();

    Q_INVOKABLE void getHelpMessage(QQuickTextDocument *document);
    Q_INVOKABLE QString getAboutMessage();
    Q_INVOKABLE void aboutQt();
private:
    QString text;

};


/** "Shutdown" window */
class ShutdownWindow : public QWidget
{
    Q_OBJECT

public:
    ShutdownWindow(QWidget* parent = 0, Qt::WindowFlags f = 0);
    static void showShutdownWindow();

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // BITCOIN_QT_UTILITYDIALOG_H
