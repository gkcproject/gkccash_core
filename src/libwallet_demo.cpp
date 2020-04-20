#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <cassert>
#include <unistd.h>
#include "libwallet/libwallet.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// demo.h
namespace demo
{
	void PrintLine(const std::string& msg);

	class UI
	{
	public:
		UI();
		void Open();
		void Close();
		bool Closed() const;
		void Update();

	private:
		enum class Status { DUMMY, RUNNING, CLOSED };
		class Command
		{
		public:
			typedef std::string Name;
			typedef std::vector<std::string> Param;
			Name name;
			Param params;
		};
		typedef void (UI::*CommandHandler)();
		
		Status status;
		libwallet::IWallet* wallet;
		std::map<Command::Name,CommandHandler> cmdMap;
		Command cmd;
		
		void OnCommand();
		void GetCommand();
		
		static bool GetCommandFromInputStream(std::istream& is, UI::Command& cmd);
		
		void CreateAddress();
		void Quit();
		void CreateTx();
		void Encrypt();
		void Unlock();
		void ListAddress();
		void History();
		void Entrust();
		void Deprive();
		void ListAgents();
		void ListUtxos();
		void AllInfo();
		void OpenWallet();
		void StartSync();
		void StopSync();

		libwallet::Node GetSyncNode() const;
	};
}
	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// demo.cpp
using namespace std;
using namespace demo;
using namespace libwallet;
int main(int argc, char* argv[])
{
	PrintLine("main() begin");
	try
	{
		UI ui;
		ui.Open();
		while(!ui.Closed())
		{
			ui.Update();
		};
	}
	catch(std::exception& e)
	{
		PrintLine(string("std::exception:")+e.what());
	}
	PrintLine("main() end");
	return 0;
}

UI::UI(){
	status = Status::DUMMY;
	
	cmdMap["q"] = &UI::Quit;
	cmdMap["c"] = &UI::CreateAddress;
	cmdMap["t"] = &UI::CreateTx;
	cmdMap["e"] = &UI::Encrypt;
	cmdMap["u"] = &UI::Unlock;
	cmdMap["l"] = &UI::ListAddress;
	cmdMap["h"] = &UI::History;
	cmdMap["entrust"] = &UI::Entrust;
	cmdMap["deprive"] = &UI::Deprive;
	cmdMap["listagents"] = &UI::ListAgents;
	cmdMap["listutxos"] = &UI::ListUtxos;
	cmdMap["a"] = &UI::AllInfo;
	cmdMap["ow"] = &UI::OpenWallet;
	cmdMap["ts"] = &UI::StartSync;
	cmdMap["ps"] = &UI::StopSync;

	wallet = nullptr;
}
void UI::Open(){
	status = Status::RUNNING;
	libwallet::OpenLib();
}
void UI::Close(){
	libwallet::CloseLib();
	status = Status::CLOSED;
}
bool UI::Closed() const{
	return status==Status::CLOSED;
}
void UI::Update(){
	GetCommand();
	OnCommand();
}

void UI::OnCommand()
{
	if(!cmdMap.count(cmd.name))
	{
		PrintLine("Undefined command, 'q' to Quit !");
		return;
	}

	CommandHandler handler = cmdMap.at(cmd.name);
	(this->*handler)();
}

void UI::GetCommand()
{
	bool success = false;
	while(!success)
	{
		success = GetCommandFromInputStream(std::cin,cmd);
	}
}

bool UI::GetCommandFromInputStream(std::istream& is, UI::Command& cmd)
{
	std::cout << "command > " << std::flush;

	std::string line;
	std::getline(is,line);
	std::stringstream ss(line);

	Command::Param words;
	std::string word;
	while(ss >> word)
	{
		words.push_back(word);
	}

	if(words.empty())
		return false;
		
	cmd.name = words[0];
	cmd.params.assign(words.begin()+1,words.end());

	return true;
}


void demo::PrintLine(const std::string& msg)
{
	std::cout << msg << std::endl;
}

void UI::CreateAddress()
{
	libwallet::Txaddr addr;
	if(wallet->GetNewAddress(addr))
		std::cout << "addr:" << addr << std::endl;
}

void UI::Quit()
{
	PrintLine("Quit.");
	Close();
}
void UI::CreateTx()
{
	Amount amount = 0;
	if(!libwallet::ParseMoney(cmd.params[1],amount))
		return;
	Txaddr addr = cmd.params[0];
	Tx tx;
	Txid txid;
	if(!wallet->CreateTransaction(addr,amount,tx))
		std::cout << "create transaction failed: " << GetLastError().desc << std::endl;
	else if(!wallet->SendTransaction(tx.rawdata,txid))
		std::cout << "send transaction failed:" << GetLastError().desc << std::endl;
	else
		std::cout << "txid:" << txid << std::endl;
}
void UI::Encrypt()
{
	bool res = wallet->Encrypt("123456");
	std::cout << "encrypt " << res << std::endl;
}
void UI::Unlock()
{
	bool res = wallet->Unlock("123456");
	std::cout << "unlock " << res << std::endl;
}
void UI::ListAddress()
{
	assert(wallet);
	std::vector<Txaddr> addrs;
	wallet->GetMyAddress(addrs);
	int sn = 0;
	for(const Txaddr& a : addrs)
	{
		std::cout << ++sn << ":" << a << std::endl;
	}
}
void UI::History()
{
	std::vector<Tx> hisTx;
	wallet->GetHistoryTransactions(hisTx);
	int sn = 0;
	for(const Tx& tx : hisTx)
	{
		std::cout << ++sn << ":" << tx.id << ":" << tx.type << std::endl;
	}
}
void UI::Entrust()
{
	Agentid agentid(cmd.params[0]);
	Amount amount;
	ParseMoney(cmd.params[1], amount);
	Txid txid;
	if(!wallet->Entrust(agentid,amount,txid))
		std::cout << "error:" << GetLastError().desc << std::endl;
	else
		std::cout << txid << std::endl;
}
void UI::Deprive()
{
	Txid entrustTxid(cmd.params[0]);
	Txid depriveTxid;
	if(!wallet->Deprive(entrustTxid,depriveTxid))
	{
		std::cout << "error:" << GetLastError().desc << std::endl;
	}
	else
		std::cout << "txid:" << depriveTxid << std::endl;
}
void UI::ListAgents()
{
	std::vector<libwallet::Agentid> agents;
	wallet->ListAgents(agents);
	std::cout << "Agent Num:" << agents.size() << std::endl;
	for(int i=0; i<agents.size(); i++)
	{
		std::cout << i+1 << ":" << agents[i] << std::endl;
	}
}
void UI::ListUtxos()
{
	std::ostringstream ss;
	std::vector<Utxo> utxos;
	wallet->GetUtxos(utxos);
	
	ss.str("");
	ss << "UTXO Num:" << utxos.size();
	PrintLine(ss.str());

	if(utxos.size() > 0)
	{
		PrintLine("index,txid,outIndex,amount,type");
		
		for(int i=0; i<utxos.size(); i++)
		{
			const Utxo& utxo = utxos[i];
			ss.str("");
			ss << i << ":" << utxo.txid << ":" << utxo.outIndex << ":" << FormatMoney(utxo.amount) << ":" << utxo.type;
			PrintLine(ss.str());
		}
	}
}

void UI::AllInfo()
{
	if(!wallet)
		throw std::runtime_error("wallet null");
	Balance bal = wallet->GetAllBalance();
	std::cout << "height: " << wallet->GetHeight() << "/" << wallet->GetNodeHeight() << std::endl;
	std::cout << "balance:" << std::endl;
	std::cout << "  .available:" << libwallet::FormatMoney(bal.available) << std::endl;
	std::cout << "  .locked:" << libwallet::FormatMoney(bal.locked) << std::endl;
	std::cout << "  .unconfirmed:" << libwallet::FormatMoney(bal.unconfirmed) << std::endl;
	std::cout << "  .immature:" << libwallet::FormatMoney(bal.immature) << std::endl;
	std::cout << "  .total:" << libwallet::FormatMoney(bal.Total()) << std::endl;

	string statusName;
	WalletStatus status = wallet->Status();
	switch(status){
		case WalletStatus::OFFLINE: statusName = "Offline"; break;
		case WalletStatus::SYNCING: statusName = "Syncing"; break;
		case WalletStatus::STANDBY: statusName = "Standby"; break;
	}
	std::cout << "status: " << statusName << std::endl;
}

void UI::OpenWallet()
{
	std::string name,password;
	const std::string defaultName("appwallet.dat");
	size_t size = cmd.params.size();
	if(size >= 1)
		name = cmd.params[0];
	else
		name = defaultName;
	if(size >= 2)
		password = cmd.params[1];

	if(IsWalletExists(name))
		wallet = libwallet::OpenWallet(name,password);
	else
		wallet = libwallet::CreateWallet(name,password);
	if(wallet==nullptr)
	{
		PrintLine("create or open wallet failed");
		return;
	}
	PrintLine("wallet ready");
}

void UI::StartSync()
{
	if(!wallet)
		throw std::runtime_error("wallet null");
	size_t size = cmd.params.size();
	int startHeight = wallet->GetHeight() + 1;
	if(size >= 1)
		startHeight = atoi(cmd.params[0].c_str());
	Node node = GetSyncNode();
	if(!PingNode(node))
	{
		PrintLine("node isn't available");
		return;
	}
	wallet->StartSync(node,startHeight);
}

Node UI::GetSyncNode() const
{
	Node node;
	node.ip = "192.100.3.219";
	node.rpcport = "38803";
	node.rpcuser = "gkc";
	node.rpcpassword = "123456";
	return node;
}

void UI::StopSync()
{
	if(!wallet)
		throw std::runtime_error("wallet null");
	wallet->StopSync();
}


