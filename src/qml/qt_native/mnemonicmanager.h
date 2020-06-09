#ifndef MNEMONICMANAGER_H
#define MNEMONICMANAGER_H

#include "wallet/bip39.h"
#include <iostream>
#include <QString>
#include <vector>
#include <QObject>

class MnemonicManager : public QObject
{
	Q_OBJECT
public:
	MnemonicManager();
	~MnemonicManager();
	static MnemonicManager* GetIntance();

	void InitVecMnemonicWords();
	Q_INVOKABLE void RandomInit();

	Q_INVOKABLE QString GetRandomIndex(int index);
	Q_INVOKABLE QString GetRandomMnemonicWord(int index);
	
	Q_INVOKABLE bool CheckMnemonicWords(QString words);
	Q_INVOKABLE bool CheckMnemonicWord(QString index,QString word);

	Q_INVOKABLE void SetMnemonicWords();
	Q_INVOKABLE QString GetMnemonicWords();
	Q_INVOKABLE QString GetMnemonicWord(int index);

	Q_INVOKABLE void CheckMnemonicFinished();

signals:
	void mnemonicFinished();

private:
	QString walletMnemonic;
	QString mnemonicWord_;
	std::vector<QString> vecMnemonicWords_;
	std::vector<QString> randomMnemonicWords_;
	std::vector<int> randomIndex_;
};

#endif

