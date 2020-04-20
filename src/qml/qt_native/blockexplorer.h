#ifndef BLOCKEXPLORER_H
#define BLOCKEXPLORER_H

#include <QMainWindow>

#include "base58.h"
#include "uint256.h"
#undef loop

namespace Ui
{
class BlockExplorer;
}


class CBlockIndex;
class CTransaction;
class CBlockTreeDB;

std::string getexplorerBlockHash(int64_t);
const CBlockIndex* getexplorerBlockIndex(int64_t);
CTxOut getPrevOut(const COutPoint& out);

class BlockExplorer : public QObject
{
    Q_OBJECT

public:
   Q_INVOKABLE bool isTxindexSet();


   Q_INVOKABLE void onSearch(const QString& query);
   Q_INVOKABLE void goTo(const QString& query);
   Q_INVOKABLE void back();
   Q_INVOKABLE void forward();
   Q_INVOKABLE void startUpPage();


    explicit BlockExplorer(QObject *parent = nullptr);
    ~BlockExplorer();

signals:
    void newContent(QString newContent);
    void newSearch(QString query);
    void setForwardButton(bool enable);
    void setBackButton(bool enable);

protected:

private Q_SLOTS:

private:
    bool m_NeverShown;
    int m_HistoryIndex;
    QStringList m_History;

    void setBlock(CBlockIndex* pBlock);
    bool switchTo(const QString& query);
    void setContent(const std::string& content);
    void updateNavButtons();
};

#endif // BLOCKEXPLORER_H
