#include "mnemonicmanager.h"
#include <QStringList>
#include <ctime>
#include <functional>
#include <algorithm>


MnemonicManager::MnemonicManager()
{
	walletMnemonic = "";
	SecureString mn = Mnemonic::mnemonic_generate(256);
	mnemonicWord_ = QString(mn.c_str());
	InitVecMnemonicWords();
}

MnemonicManager::~MnemonicManager()
{
	
}


void MnemonicManager::RandomInit()
{
	randomMnemonicWords_.clear();
	randomIndex_.clear();
	randomMnemonicWords_ = vecMnemonicWords_;
	srand((unsigned int)time(0));
	std::random_shuffle(randomMnemonicWords_.begin(),randomMnemonicWords_.end());

	while(randomIndex_.size()<4)
	{
		int index = rand()%24;
		index++;
		bool repeat = false;
		for(int i=0;i<randomIndex_.size();i++)
		{
			if(index == randomIndex_[i])
			{
				repeat = true;
			}
		}
		if(repeat == false)
		{
			randomIndex_.push_back(index);
		}
	}
}


void MnemonicManager::InitVecMnemonicWords()
{
	vecMnemonicWords_.clear();
	QStringList listWords = mnemonicWord_.split(" ");
	for(int i=0;i<listWords.size();++i)
	{
		vecMnemonicWords_.push_back(listWords[i]);
	}
}

QString MnemonicManager::GetRandomIndex(int index)
{
	return QString::number(randomIndex_[index-1]);
}


QString MnemonicManager::GetRandomMnemonicWord(int index)
{
	if(randomMnemonicWords_.size()<index || index < 1)
		return "";
	return randomMnemonicWords_[index-1];
}

bool MnemonicManager::CheckMnemonicWord(QString index,QString word)
{
	int i = index.toInt();
	if(vecMnemonicWords_.size()<i || i < 1)
		return false;
	return vecMnemonicWords_[i-1] == word;
}

bool MnemonicManager::CheckMnemonicWords(QString words)
{
	//std::cout << "CheckMnemonicWords:" << words.toStdString().c_str() << "\n";
	words = words.trimmed();
	SecureString sstr = SecureString(words.toStdString().c_str());
	if(Mnemonic::mnemonic_check(sstr))
	{
		walletMnemonic = words;
		return true;
	}
	return false;
}

void MnemonicManager::SetMnemonicWords()
{
	walletMnemonic = mnemonicWord_;
}

QString MnemonicManager::GetMnemonicWords()
{
	return walletMnemonic;
}

QString MnemonicManager::GetMnemonicWord(int index)
{
	//std::cout << "index:" << index << " word:" << vecMnemonicWords_[index].toStdString() << "\n";
	if(vecMnemonicWords_.size()<index || index < 1)
		return "";
	return vecMnemonicWords_[index-1];
}

void MnemonicManager::CheckMnemonicFinished()
{
	emit mnemonicFinished();
}