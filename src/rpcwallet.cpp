// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "amount.h"
#include "base58.h"
#include "core_io.h"
#include "init.h"
#include "net.h"
#include "netbase.h"
#include "rpcserver.h"
#include "timedata.h"
#include "util.h"
#include "utilmoneystr.h"
#include "wallet.h"
#include "walletdb.h"
#include "main.h"
#include "contractconfig.h"
#include "coincontrol.h"

#include <stdint.h>

#include "libzerocoin/Coin.h"
#include "extendeddata.h"
#include "spork.h"
#include <boost/assign/list_of.hpp>

#include <univalue.h>
#include "entrustment.h"
#include "scriptex.h"
#include "forkheights.h"

using namespace std;
using namespace boost;
using namespace boost::assign;

int64_t nWalletUnlockTime;
static CCriticalSection cs_nWalletUnlockTime;

std::string HelpRequiringPassphrase()
{
    return pwalletMain && pwalletMain->IsCrypted() ? "\nRequires wallet passphrase to be set with walletpassphrase call." : "";
}

void EnsureWalletIsUnlocked()
{
    if (pwalletMain->IsLocked() || pwalletMain->fWalletUnlockAnonymizeOnly)
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");
}

void WalletTxToJSON(const CWalletTx& wtx, UniValue& entry)
{
    int confirms = wtx.GetDepthInMainChain(false);
    int confirmsTotal = GetIXConfirmations(wtx.GetHash()) + confirms;
    entry.push_back(Pair("confirmations", confirmsTotal));
    entry.push_back(Pair("bcconfirmations", confirms));
    if (wtx.IsCoinBase() || wtx.IsCoinStake())
        entry.push_back(Pair("generated", true));
    if (confirms > 0) {
        entry.push_back(Pair("blockhash", wtx.hashBlock.GetHex()));
        entry.push_back(Pair("blockindex", wtx.nIndex));
        entry.push_back(Pair("blocktime", mapBlockIndex[wtx.hashBlock]->GetBlockTime()));
    }
    uint256 hash = wtx.GetHash();
    entry.push_back(Pair("txid", hash.GetHex()));
    UniValue conflicts(UniValue::VARR);
    BOOST_FOREACH (const uint256& conflict, wtx.GetConflicts())
        conflicts.push_back(conflict.GetHex());
    entry.push_back(Pair("walletconflicts", conflicts));
    entry.push_back(Pair("time", wtx.GetTxTime()));
    entry.push_back(Pair("timereceived", (int64_t)wtx.nTimeReceived));
    BOOST_FOREACH (const PAIRTYPE(string, string) & item, wtx.mapValue)
        entry.push_back(Pair(item.first, item.second));
}

string AccountFromValue(const UniValue& value)
{
    string strAccount = value.get_str();
    if (strAccount == "*")
        throw JSONRPCError(RPC_WALLET_INVALID_ACCOUNT_NAME, "Invalid account name");
    return strAccount;
}

UniValue getnewaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getnewaddress ( \"account\" )\n"
            "\nReturns a new GKC address for receiving payments.\n"
            "If 'account' is specified (recommended), it is added to the address book \n"
            "so payments received with the address will be credited to 'account'.\n"
            "\nArguments:\n"
            "1. \"account\"        (string, optional) The account name for the address to be linked to. if not provided, the default account \"\" is used. It can also be set to the empty string \"\" to represent the default account. The account does not need to exist, it will be created if there is no account by the given name.\n"
            "\nResult:\n"
            "\"gkcaddress\"    (string) The new gkc address\n"
            "\nExamples:\n" +
            HelpExampleCli("getnewaddress", "") + HelpExampleCli("getnewaddress", "\"\"") + HelpExampleCli("getnewaddress", "\"myaccount\"") + HelpExampleRpc("getnewaddress", "\"myaccount\""));

    // Parse the account first so we don't generate a key if there's an error
    string strAccount;
    if (params.size() > 0)
        strAccount = AccountFromValue(params[0]);

    if (!pwalletMain->IsLocked())
        pwalletMain->TopUpKeyPool();

    // Generate a new key that is added to wallet
    CPubKey newKey;
    if (!pwalletMain->GetKeyFromPool(newKey))
        throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, "Error: Keypool ran out, please call keypoolrefill first");
    CKeyID keyID = newKey.GetID();

    pwalletMain->SetAddressBook(keyID, strAccount, "receive");

    return CBitcoinAddress(keyID).ToString();
}


CBitcoinAddress GetAccountAddress(string strAccount, bool bForceNew = false)
{
    CWalletDB walletdb(pwalletMain->strWalletFile);

    CAccount account;
    walletdb.ReadAccount(strAccount, account);

    bool bKeyUsed = false;

    // Check if the current key has been used
    if (account.vchPubKey.IsValid()) {
        CScript scriptPubKey = GetScriptForDestination(account.vchPubKey.GetID());
        for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin();
             it != pwalletMain->mapWallet.end() && account.vchPubKey.IsValid();
             ++it) {
            const CWalletTx& wtx = (*it).second;
            BOOST_FOREACH (const CTxOut& txout, wtx.vout)
                if (txout.scriptPubKey == scriptPubKey)
                    bKeyUsed = true;
        }
    }

    // Generate a new key
    if (!account.vchPubKey.IsValid() || bForceNew || bKeyUsed) {
        if (!pwalletMain->GetKeyFromPool(account.vchPubKey))
            throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, "Error: Keypool ran out, please call keypoolrefill first");

        pwalletMain->SetAddressBook(account.vchPubKey.GetID(), strAccount, "receive");
        walletdb.WriteAccount(strAccount, account);
    }

    return CBitcoinAddress(account.vchPubKey.GetID());
}

UniValue getaccountaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaccountaddress \"account\"\n"
            "\nReturns the current GKC address for receiving payments to this account.\n"
            "\nArguments:\n"
            "1. \"account\"       (string, required) The account name for the address. It can also be set to the empty string \"\" to represent the default account. The account does not need to exist, it will be created and a new address created  if there is no account by the given name.\n"
            "\nResult:\n"
            "\"gkcaddress\"   (string) The account gkc address\n"
            "\nExamples:\n" +
            HelpExampleCli("getaccountaddress", "") + HelpExampleCli("getaccountaddress", "\"\"") + HelpExampleCli("getaccountaddress", "\"myaccount\"") + HelpExampleRpc("getaccountaddress", "\"myaccount\""));

    // Parse the account first so we don't generate a key if there's an error
    string strAccount = AccountFromValue(params[0]);

    UniValue ret(UniValue::VSTR);

    ret = GetAccountAddress(strAccount).ToString();

    return ret;
}


UniValue getrawchangeaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getrawchangeaddress\n"
            "\nReturns a new GKC address, for receiving change.\n"
            "This is for use with raw transactions, NOT normal use.\n"
            "\nResult:\n"
            "\"address\"    (string) The address\n"
            "\nExamples:\n" +
            HelpExampleCli("getrawchangeaddress", "") + HelpExampleRpc("getrawchangeaddress", ""));

    if (!pwalletMain->IsLocked())
        pwalletMain->TopUpKeyPool();

    CReserveKey reservekey(pwalletMain);
    CPubKey vchPubKey;
    if (!reservekey.GetReservedKey(vchPubKey))
        throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, "Error: Keypool ran out, please call keypoolrefill first");

    reservekey.KeepKey();

    CKeyID keyID = vchPubKey.GetID();

    return CBitcoinAddress(keyID).ToString();
}


UniValue setaccount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "setaccount \"gkcaddress\" \"account\"\n"
            "\nSets the account associated with the given address.\n"
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address to be associated with an account.\n"
            "2. \"account\"         (string, required) The account to assign the address to.\n"
            "\nExamples:\n" +
            HelpExampleCli("setaccount", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" \"tabby\"") + HelpExampleRpc("setaccount", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", \"tabby\""));

    CBitcoinAddress address(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");


    string strAccount;
    if (params.size() > 1)
        strAccount = AccountFromValue(params[1]);

    // Only add the account if the address is yours.
    if (IsMine(*pwalletMain, address.Get())) {
        // Detect when changing the account of an address that is the 'unused current key' of another account:
        if (pwalletMain->mapAddressBook.count(address.Get())) {
            string strOldAccount = pwalletMain->mapAddressBook[address.Get()].name;
            if (address == GetAccountAddress(strOldAccount))
                GetAccountAddress(strOldAccount, true);
        }
        pwalletMain->SetAddressBook(address.Get(), strAccount, "receive");
    } else
        throw JSONRPCError(RPC_MISC_ERROR, "setaccount can only be used with own address");

    return NullUniValue;
}


UniValue getaccount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaccount \"gkcaddress\"\n"
            "\nReturns the account associated with the given address.\n"
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address for account lookup.\n"
            "\nResult:\n"
            "\"accountname\"        (string) the account address\n"
            "\nExamples:\n" +
            HelpExampleCli("getaccount", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\"") + HelpExampleRpc("getaccount", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\""));

    CBitcoinAddress address(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");

    string strAccount;
    map<CTxDestination, CAddressBookData>::iterator mi = pwalletMain->mapAddressBook.find(address.Get());
    if (mi != pwalletMain->mapAddressBook.end() && !(*mi).second.name.empty())
        strAccount = (*mi).second.name;
    return strAccount;
}


UniValue getaddressesbyaccount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getaddressesbyaccount \"account\"\n"
            "\nReturns the list of addresses for the given account.\n"
            "\nArguments:\n"
            "1. \"account\"  (string, required) The account name.\n"
            "\nResult:\n"
            "[                     (json array of string)\n"
            "  \"gkcaddress\"  (string) a gkc address associated with the given account\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n" +
            HelpExampleCli("getaddressesbyaccount", "\"tabby\"") + HelpExampleRpc("getaddressesbyaccount", "\"tabby\""));

    string strAccount = AccountFromValue(params[0]);

    // Find all addresses that have the given account
    UniValue ret(UniValue::VARR);
    BOOST_FOREACH (const PAIRTYPE(CBitcoinAddress, CAddressBookData) & item, pwalletMain->mapAddressBook) {
        const CBitcoinAddress& address = item.first;
        const string& strName = item.second.name;
        if (strName == strAccount)
            ret.push_back(address.ToString());
    }
    return ret;
}

void SendMoneyEx(const CTxDestination& address, CAmount nValue, CWalletTx& wtxNew, bool fUseIX, CAmount nMinUtxoValue);

void SendMoney(const CTxDestination& address, CAmount nValue, CWalletTx& wtxNew, bool fUseIX = false)
{
	SendMoneyEx(address,nValue,wtxNew,fUseIX,0);
}

void SendMoneyEx(const CTxDestination& address, CAmount nValue, CWalletTx& wtxNew, bool fUseIX, CAmount nMinUtxoValue)
{
	// Check amount
	if (nValue <= 0)
		throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");

#ifdef DPOS
	//check nValue with non-deprive tx only.
	if(wtxNew.GetType() != CTransaction::Type::DEPRIVE)
#endif
	if (nValue > pwalletMain->GetBalance())
		throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

	string strError;
	if (pwalletMain->IsLocked()) {
		strError = "Error: Wallet locked, unable to create transaction!";
		LogPrintf("SendMoney() : %s", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}

	// Parse GKC address
	CScript scriptPubKey = GetScriptForDestination(address);
	
	CCoinControl* pCoinCtrl = nullptr;
	CCoinControl coinCtrl;
#ifdef DPOS
	if(!wtxNew.agentid.IsNull()){
		coinCtrl.agentid = wtxNew.GetAgentId();
		coinCtrl.txType = wtxNew.GetType();
		pCoinCtrl = &coinCtrl;
	}
#endif
	if(nMinUtxoValue > 0){
		coinCtrl.nMinUtxoValue = nMinUtxoValue;
		pCoinCtrl = &coinCtrl;
	}

	// Create and send the transaction
	CReserveKey reservekey(pwalletMain);
	CAmount nFeeRequired;
	if (!pwalletMain->CreateTransaction(scriptPubKey, nValue, wtxNew, reservekey, nFeeRequired, strError, pCoinCtrl, ALL_COINS, fUseIX, (CAmount)0)) {
		if (nValue + nFeeRequired > pwalletMain->GetBalance())
			strError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
		LogPrintf("SendMoney() : %s\n", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, (!fUseIX ? "tx" : "ix")))
		throw JSONRPCError(RPC_WALLET_ERROR, "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
}

UniValue sendadvertise(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 3 || params.size() > 5)
        throw runtime_error(
            "sendadvertise \"title\" \"author\" \"text\" (\"link\" \"extend\")\n"
            "\nSend an advertise message\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"title\"      (string, required) \n"
			"2. \"author\"     (string, required) \n"
			"3. \"text\"       (string, required) \n"
			"4. \"link\"	   (string, optional) \n"
			"5. \"extend\"	   (string, optional) \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n");

	CAmount nValue = advertisement::GetAdPrice(chainActive.Height() + 1);
	
	if (nValue > pwalletMain->GetBalance())
		throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

	string strError;
	if (pwalletMain->IsLocked()) {
		strError = "Error: Wallet locked, unable to create transaction!";
		LogPrintf("SendMoney() : %s", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}

	advertisement::Advertise ad;
	if(params.size() >= 3){
		ad.title = params[0].get_str();
		ad.author = params[1].get_str();
		ad.text = params[2].get_str();
	}
	if(params.size() >= 4){
		ad.link = params[3].get_str();
	}
	if(params.size() >= 5){
		ad.extend = params[4].get_str();
	}

	if(advertisement::FilterAd(ad))
		throw JSONRPCError(RPC_INVALID_PARAMETER, "content of ad is filtered");

	CScript script;
	if(!ad.EncodeTo(script))
		throw JSONRPCError(RPC_INTERNAL_ERROR, "Encode() failed");
	
	if(script.size() > MAX_OP_RETURN_RELAY)
		throw JSONRPCError(RPC_INVALID_PARAMETER, "content too large");

	// Create and send the transaction
	CReserveKey reservekey(pwalletMain);
	CAmount nFeeRequired;
	CWalletTx wtxNew;
	if (!pwalletMain->CreateTransaction(script, nValue, wtxNew, reservekey, nFeeRequired, strError, NULL, ALL_COINS, false, (CAmount)0)) {
		if (nValue + nFeeRequired > pwalletMain->GetBalance())
			strError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
		LogPrintf("SendMoney() : %s\n", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, "tx"))
		throw JSONRPCError(RPC_WALLET_ERROR, "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");

	return wtxNew.GetHash().GetHex();
}

UniValue getadadmin(const UniValue& params, bool fHelp)
{
	if (fHelp || params.size() != 1)
		throw runtime_error(
			"getadadmin \"gkcaddress\"\n"
			"\nGet the adadmin info corresponding to 'gkcaddress'.\n"
			"\nArguments:\n"
			"1. \"gkcaddress\"	 (string, required) The gkc address for the public key\n"
			"\nResult:\n"
			"{"
			"\"address\" 			(string)\n"
			"\"pubkey\" 			(string)\n"
			"\"pubkeyhash\" 		(string)\n"
			"}");
	
	EnsureWalletIsUnlocked();
	
	string strAddress = params[0].get_str();
	CBitcoinAddress address;
	if (!address.SetString(strAddress))
		throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");
	CKeyID keyID;
	if (!address.GetKeyID(keyID))
		throw JSONRPCError(RPC_TYPE_ERROR, "Address does not refer to a key");
	CPubKey pubKey;
	if(!pwalletMain->GetPubKey(keyID, pubKey))
		throw JSONRPCError(RPC_WALLET_ERROR, "Public key for address " + strAddress + " is not known");

	UniValue result(UniValue::VOBJ);
	result.push_back(Pair("address",address.ToString()));
	result.push_back(Pair("pubkey",util::BytesToHexString(pubKey.Raw())));
	result.push_back(Pair("pubkeyhash",keyID.ToString()));
	return result;
}

UniValue sendsysmessage(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "sendsysmessage \"message\"\n"
            "\nSend an system message\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"message\"      (string, required) \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n");

	CAmount nValue = 0;
	
	if (nValue > pwalletMain->GetBalance())
		throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

	string strError;
	if (pwalletMain->IsLocked()) {
		strError = "Error: Wallet locked, unable to create transaction!";
		LogPrintf("SendMoney() : %s", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}

	advertisement::SysMessage sysMsg;
	sysMsg.text = params[0].get_str();
	if(!sysMsg.AdminSign(*pwalletMain))
	{
		throw JSONRPCError(RPC_WALLET_ERROR, "AdminSign() failed");
	}
	
	CScript script;
	if(!sysMsg.EncodeTo(script))
		throw JSONRPCError(RPC_INTERNAL_ERROR, "Encode() failed");

	if(script.size() > MAX_OP_RETURN_RELAY)
		throw JSONRPCError(RPC_INVALID_PARAMETER, "content too large");
	
	// Create and send the transaction
	CWalletTx wtxNew;
	CReserveKey reservekey(pwalletMain);
	CAmount nFeeRequired;
	if (!pwalletMain->CreateTransaction(script, nValue, wtxNew, reservekey, nFeeRequired, strError, NULL, ALL_COINS, false, (CAmount)0)) {
		if (nValue + nFeeRequired > pwalletMain->GetBalance())
			strError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
		LogPrintf("SendMoney() : %s\n", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, "tx"))
		throw JSONRPCError(RPC_WALLET_ERROR, "Error: The transaction was rejected!");

	return wtxNew.GetHash().GetHex();
}

UniValue filteradtx(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "filteradtx \"txid\"\n"
            "\nSend an tx to filter the advertise\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"txid\"      (string, required) \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n");

	CAmount nValue = 0;
	
	if (nValue > pwalletMain->GetBalance())
		throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

	string strError;
	if (pwalletMain->IsLocked()) {
		strError = "Error: Wallet locked, unable to create transaction!";
		LogPrintf("SendMoney() : %s", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}

	advertisement::AdFilter filter;
    filter.txid = ParseHashV(params[0], "parameter 1");
	if(!filter.AdminSign(*pwalletMain))
	{
		throw JSONRPCError(RPC_WALLET_ERROR, "AdminSign() failed");
	}

	CScript script;
	if(!filter.EncodeTo(script))
		throw JSONRPCError(RPC_INTERNAL_ERROR, "Encode() failed");

	if(script.size() > MAX_OP_RETURN_RELAY)
		throw JSONRPCError(RPC_INVALID_PARAMETER, "content too large");
	
	CWalletTx wtxNew;
	// Create and send the transaction
	CReserveKey reservekey(pwalletMain);
	CAmount nFeeRequired;
	if (!pwalletMain->CreateTransaction(script, nValue, wtxNew, reservekey, nFeeRequired, strError, NULL, ALL_COINS, false, (CAmount)0)) {
		if (nValue + nFeeRequired > pwalletMain->GetBalance())
			strError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
		LogPrintf("SendMoney() : %s\n", strError);
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	}
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, "tx"))
		throw JSONRPCError(RPC_WALLET_ERROR, "Error: The transaction was rejected!");

	return wtxNew.GetHash().GetHex();
}

UniValue sendtoaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 4)
        throw runtime_error(
            "sendtoaddress \"gkcaddress\" amount ( \"comment\" \"comment-to\" )\n"
            "\nSend an amount to a given address. The amount is a real and is rounded to the nearest 0.00000001\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address to send to.\n"
            "2. \"amount\"      (numeric, required) The amount in btc to send. eg 0.1\n"
            "3. \"comment\"     (string, optional) A comment used to store what the transaction is for. \n"
            "                             This is not part of the transaction, just kept in your wallet.\n"
            "4. \"comment-to\"  (string, optional) A comment to store the name of the person or organization \n"
            "                             to which you're sending the transaction. This is not part of the \n"
            "                             transaction, just kept in your wallet.\n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n"
            "\nExamples:\n" +
            HelpExampleCli("sendtoaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1") + HelpExampleCli("sendtoaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1 \"donation\" \"seans outpost\"") + HelpExampleRpc("sendtoaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", 0.1, \"donation\", \"seans outpost\""));

    CBitcoinAddress address(params[0].get_str());
	if(!address.IsGKC())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");

    // Amount
    CAmount nAmount = AmountFromValue(params[1]);

    // Wallet comments
    CWalletTx wtx;
    if (params.size() > 2 && !params[2].isNull() && !params[2].get_str().empty())
        wtx.mapValue["comment"] = params[2].get_str();
    if (params.size() > 3 && !params[3].isNull() && !params[3].get_str().empty())
        wtx.mapValue["to"] = params[3].get_str();

    EnsureWalletIsUnlocked();

    SendMoney(address.Get(), nAmount, wtx);

    return wtx.GetHash().GetHex();
}

UniValue sendtoaddressix(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 4)
        throw runtime_error(
            "sendtoaddressix \"gkcaddress\" amount ( \"comment\" \"comment-to\" )\n"
            "\nSend an amount to a given address. The amount is a real and is rounded to the nearest 0.00000001\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address to send to.\n"
            "2. \"amount\"      (numeric, required) The amount in btc to send. eg 0.1\n"
            "3. \"comment\"     (string, optional) A comment used to store what the transaction is for. \n"
            "                             This is not part of the transaction, just kept in your wallet.\n"
            "4. \"comment-to\"  (string, optional) A comment to store the name of the person or organization \n"
            "                             to which you're sending the transaction. This is not part of the \n"
            "                             transaction, just kept in your wallet.\n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n"
            "\nExamples:\n" +
            HelpExampleCli("sendtoaddressix", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1") + HelpExampleCli("sendtoaddressix", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1 \"donation\" \"seans outpost\"") + HelpExampleRpc("sendtoaddressix", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", 0.1, \"donation\", \"seans outpost\""));

    CBitcoinAddress address(params[0].get_str());
    if (!address.IsGKC())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");

    // Amount
    CAmount nAmount = AmountFromValue(params[1]);

    // Wallet comments
    CWalletTx wtx;
    if (params.size() > 2 && !params[2].isNull() && !params[2].get_str().empty())
        wtx.mapValue["comment"] = params[2].get_str();
    if (params.size() > 3 && !params[3].isNull() && !params[3].get_str().empty())
        wtx.mapValue["to"] = params[3].get_str();

    EnsureWalletIsUnlocked();

    SendMoney(address.Get(), nAmount, wtx, true);

    return wtx.GetHash().GetHex();
}

UniValue selectutxotosend(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 4)
        throw runtime_error(
            "selectutxotosend address amount minutxo\n"
            "\nSend an amount to a given address. All value of utxo >= minutxo.\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address to send to.\n"
            "2. \"amount\"      (numeric, required) The amount in btc to send. eg 0.1\n"
            "3. \"minutxo\"     (numeric, required) The amount in btc to send. eg 0.01. \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n"
            "\nExamples:\n" +
            HelpExampleCli("selectutxotosend", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1 0.01") + HelpExampleCli("selectutxotosend", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.1 0.01 \"donation\" \"seans outpost\"") + HelpExampleRpc("selectutxotosend", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", 0.1, 0.01, \"donation\", \"seans outpost\""));

    CBitcoinAddress address(params[0].get_str());
	if(!address.IsGKC())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");

    // Amount
    CAmount nAmount = AmountFromValue(params[1]);
	CAmount nMinUtxo = AmountFromValue(params[2]);

    // Wallet comments
    CWalletTx wtx;

    EnsureWalletIsUnlocked();

    SendMoneyEx(address.Get(), nAmount, wtx, false, nMinUtxo);

    return wtx.GetHash().GetHex();
}

UniValue listaddressgroupings(const UniValue& params, bool fHelp)
{
    if (fHelp)
        throw runtime_error(
            "listaddressgroupings\n"
            "\nLists groups of addresses which have had their common ownership\n"
            "made public by common use as inputs or as the resulting change\n"
            "in past transactions\n"
            "\nResult:\n"
            "[\n"
            "  [\n"
            "    [\n"
            "      \"gkcaddress\",     (string) The gkc address\n"
            "      amount,                 (numeric) The amount in btc\n"
            "      \"account\"             (string, optional) The account\n"
            "    ]\n"
            "    ,...\n"
            "  ]\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n" +
            HelpExampleCli("listaddressgroupings", "") + HelpExampleRpc("listaddressgroupings", ""));

    UniValue jsonGroupings(UniValue::VARR);
    map<CTxDestination, CAmount> balances = pwalletMain->GetAddressBalances();
    BOOST_FOREACH (set<CTxDestination> grouping, pwalletMain->GetAddressGroupings()) {
        UniValue jsonGrouping(UniValue::VARR);
        BOOST_FOREACH (CTxDestination address, grouping) {
            UniValue addressInfo(UniValue::VARR);
            addressInfo.push_back(CBitcoinAddress(address).ToString());
            addressInfo.push_back(ValueFromAmount(balances[address]));
            {
                LOCK(pwalletMain->cs_wallet);
                if (pwalletMain->mapAddressBook.find(CBitcoinAddress(address).Get()) != pwalletMain->mapAddressBook.end())
                    addressInfo.push_back(pwalletMain->mapAddressBook.find(CBitcoinAddress(address).Get())->second.name);
            }
            jsonGrouping.push_back(addressInfo);
        }
        jsonGroupings.push_back(jsonGrouping);
    }
    return jsonGroupings;
}

UniValue signmessage(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "signmessage \"gkcaddress\" \"message\"\n"
            "\nSign a message with the private key of an address" +
            HelpRequiringPassphrase() + "\n"
                                        "\nArguments:\n"
                                        "1. \"gkcaddress\"  (string, required) The gkc address to use for the private key.\n"
                                        "2. \"message\"         (string, required) The message to create a signature of.\n"
                                        "\nResult:\n"
                                        "\"signature\"          (string) The signature of the message encoded in base 64\n"
                                        "\nExamples:\n"
                                        "\nUnlock the wallet for 30 seconds\n" +
            HelpExampleCli("walletpassphrase", "\"mypassphrase\" 30") +
            "\nCreate the signature\n" + HelpExampleCli("signmessage", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" \"my message\"") +
            "\nVerify the signature\n" + HelpExampleCli("verifymessage", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" \"signature\" \"my message\"") +
            "\nAs json rpc\n" + HelpExampleRpc("signmessage", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", \"my message\""));

    EnsureWalletIsUnlocked();

    string strAddress = params[0].get_str();
    string strMessage = params[1].get_str();

    CBitcoinAddress addr(strAddress);
    if (!addr.IsValid())
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid address");

    CKeyID keyID;
    if (!addr.GetKeyID(keyID))
        throw JSONRPCError(RPC_TYPE_ERROR, "Address does not refer to key");

    CKey key;
    if (!pwalletMain->GetKey(keyID, key))
        throw JSONRPCError(RPC_WALLET_ERROR, "Private key not available");

    CHashWriter ss(SER_GETHASH, 0);
    ss << strMessageMagic;
    ss << strMessage;

    vector<unsigned char> vchSig;
    if (!key.SignCompact(ss.GetHash(), vchSig))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Sign failed");

    return EncodeBase64(&vchSig[0], vchSig.size());
}

UniValue getreceivedbyaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getreceivedbyaddress \"gkcaddress\" ( minconf )\n"
            "\nReturns the total amount received by the given gkcaddress in transactions with at least minconf confirmations.\n"
            "\nArguments:\n"
            "1. \"gkcaddress\"  (string, required) The gkc address for transactions.\n"
            "2. minconf             (numeric, optional, default=1) Only include transactions confirmed at least this many times.\n"
            "\nResult:\n"
            "amount   (numeric) The total amount in btc received at this address.\n"
            "\nExamples:\n"
            "\nThe amount from transactions with at least 1 confirmation\n" +
            HelpExampleCli("getreceivedbyaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\"") +
            "\nThe amount including unconfirmed transactions, zero confirmations\n" + HelpExampleCli("getreceivedbyaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0") +
            "\nThe amount with at least 6 confirmation, very safe\n" + HelpExampleCli("getreceivedbyaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 6") +
            "\nAs a json rpc call\n" + HelpExampleRpc("getreceivedbyaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", 6"));

    // gkc address
    CBitcoinAddress address = CBitcoinAddress(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");
    CScript scriptPubKey = GetScriptForDestination(address.Get());
    if (!IsMine(*pwalletMain, scriptPubKey))
        return (double)0.0;

    // Minimum confirmations
    int nMinDepth = 1;
    if (params.size() > 1)
        nMinDepth = params[1].get_int();

    // Tally
    CAmount nAmount = 0;
    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
        const CWalletTx& wtx = (*it).second;
        if (wtx.IsCoinBase() || !IsFinalTx(wtx))
            continue;

        BOOST_FOREACH (const CTxOut& txout, wtx.vout)
            if (txout.scriptPubKey == scriptPubKey)
                if (wtx.GetDepthInMainChain() >= nMinDepth)
                    nAmount += txout.nValue;
    }

    return ValueFromAmount(nAmount);
}


UniValue getreceivedbyaccount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getreceivedbyaccount \"account\" ( minconf )\n"
            "\nReturns the total amount received by addresses with <account> in transactions with at least [minconf] confirmations.\n"
            "\nArguments:\n"
            "1. \"account\"      (string, required) The selected account, may be the default account using \"\".\n"
            "2. minconf          (numeric, optional, default=1) Only include transactions confirmed at least this many times.\n"
            "\nResult:\n"
            "amount              (numeric) The total amount in btc received for this account.\n"
            "\nExamples:\n"
            "\nAmount received by the default account with at least 1 confirmation\n" +
            HelpExampleCli("getreceivedbyaccount", "\"\"") +
            "\nAmount received at the tabby account including unconfirmed amounts with zero confirmations\n" + HelpExampleCli("getreceivedbyaccount", "\"tabby\" 0") +
            "\nThe amount with at least 6 confirmation, very safe\n" + HelpExampleCli("getreceivedbyaccount", "\"tabby\" 6") +
            "\nAs a json rpc call\n" + HelpExampleRpc("getreceivedbyaccount", "\"tabby\", 6"));

    // Minimum confirmations
    int nMinDepth = 1;
    if (params.size() > 1)
        nMinDepth = params[1].get_int();

    // Get the set of pub keys assigned to account
    string strAccount = AccountFromValue(params[0]);
    set<CTxDestination> setAddress = pwalletMain->GetAccountAddresses(strAccount);

    // Tally
    CAmount nAmount = 0;
    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
        const CWalletTx& wtx = (*it).second;
        if (wtx.IsCoinBase() || !IsFinalTx(wtx))
            continue;

        BOOST_FOREACH (const CTxOut& txout, wtx.vout) {
            CTxDestination address;
            if (ExtractDestination(txout.scriptPubKey, address) && IsMine(*pwalletMain, address) && setAddress.count(address))
                if (wtx.GetDepthInMainChain() >= nMinDepth)
                    nAmount += txout.nValue;
        }
    }

    return (double)nAmount / (double)COIN;
}


CAmount GetAccountBalance(CWalletDB&, const string& strAccount, int nMinDepth, const isminefilter& filter)
{
	LogPrint("wallet","GetAccountBalance | strAccount=%s, nMinDepth=%d, filter=%d\n",strAccount,nMinDepth,(int)filter);
    CAmount nBalance = 0;

    // Tally wallet transactions
    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
		const uint256& txid = it->first;
		LogPrint("wallet","GetAccountBalance | txid=%s\n",txid.ToString());
        const CWalletTx& wtx = (*it).second;
        if (!IsFinalTx(wtx) || wtx.GetBlocksToMaturity() > 0 || wtx.GetDepthInMainChain() < 0) {
			LogPrint("wallet","GetAccountBalance | IsFinalTx(wtx)=%d,wtx.GetBlocksToMaturity()=%d,wtx.GetDepthInMainChain()=%d\n",IsFinalTx(wtx),wtx.GetBlocksToMaturity(),wtx.GetDepthInMainChain());
            continue;
        }

		CAmount txSend = 0;
        for(const CTxIn& txin : wtx.vin) {
			LogPrint("wallet","GetAccountBalance | txin.prevout=%s\n",txin.prevout.ToString());
			CTxOut prevout;
			if(!dpos::GetConfirmedTxOut(txin.prevout,prevout)){
				LogPrint("wallet","GetAccountBalance | prevout not found\n");
				continue;
			}
			if(!pwalletMain->IsMine(prevout)){
				LogPrint("wallet","GetAccountBalance | prevout is not mine\n");
				continue;
			}
			CTxDestination address;
			if (!ExtractDestination(prevout.scriptPubKey, address)){
				LogPrint("wallet","GetAccountBalance | prevout.scriptPubKey invalid | prevout.scriptPubKey=%s\n",prevout.scriptPubKey.ToString());
				continue;
			}
			LogPrint("wallet","GetAccountBalance | address=%s\n",CBitcoinAddress(address).ToString());
			if ((pwalletMain->mapAddressBook.count(address)>0 && pwalletMain->mapAddressBook[address].name == strAccount) || 
				(pwalletMain->mapAddressBook.count(address)==0 && strAccount==string(""))){
				txSend += prevout.nValue;
				LogPrint("wallet","GetAccountBalance | %s send %lld, txSend=%lld\n",CBitcoinAddress(address).ToString(),prevout.nValue,txSend);
			}
        }

		CAmount txReceive = 0;
		if(wtx.GetDepthInMainChain() >= nMinDepth){
			for(const CTxOut& txout : wtx.vout) {
				LogPrint("wallet","GetAccountBalance | txout=%s\n",txout.ToString());
				isminetype type = pwalletMain->IsMine(txout);
				if((type & filter)==0){
					LogPrint("wallet","GetAccountBalance | txout's isminetype=%d, is not match with filter(%d)\n",(int)type,filter);
					continue;
				}
				CTxDestination address;
				if (!ExtractDestination(txout.scriptPubKey, address)){
					LogPrint("wallet","GetAccountBalance | txout.scriptPubKey invalid | txout.scriptPubKey=%s\n",txout.scriptPubKey.ToString());
					continue;
				}
				LogPrint("wallet","GetAccountBalance | address=%s\n",CBitcoinAddress(address).ToString());
				if ((pwalletMain->mapAddressBook.count(address)>0 && pwalletMain->mapAddressBook[address].name == strAccount) || 
					(pwalletMain->mapAddressBook.count(address)==0 && strAccount==string(""))){
					txReceive += txout.nValue;
					LogPrint("wallet","GetAccountBalance | %s receive %lld, txReceive=%lld\n",CBitcoinAddress(address).ToString(),txout.nValue,txReceive);
				}
			}
		}

		nBalance += (txReceive - txSend);
		LogPrint("wallet","GetAccountBalance | send(-) %lld, receive(+) %lld, nBalance=%lld\n",txSend,txReceive,nBalance);
    }

    return nBalance;
}

CAmount GetAccountBalance(const string& strAccount, int nMinDepth, const isminefilter& filter)
{
    CWalletDB walletdb(pwalletMain->strWalletFile);
    return GetAccountBalance(walletdb, strAccount, nMinDepth, filter);
}


UniValue getbalance(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 3)
        throw runtime_error(
            "getbalance ( \"account\" minconf includeWatchonly )\n"
            "\nIf account is not specified, returns the server's total available balance (excluding zerocoins).\n"
            "If account is specified, returns the balance in the account.\n"
            "Note that the account \"\" is not the same as leaving the parameter out.\n"
            "The server total may be different to the balance in the default \"\" account.\n"
            "\nArguments:\n"
            "1. \"account\"      (string, optional) The selected account, or \"*\" for entire wallet. It may be the default account using \"\".\n"
            "2. minconf          (numeric, optional, default=1) Only include transactions confirmed at least this many times.\n"
            "3. includeWatchonly (bool, optional, default=false) Also include balance in watchonly addresses (see 'importaddress')\n"
            "\nResult:\n"
            "amount              (numeric) The total amount in btc received for this account.\n"
            "\nExamples:\n"
            "\nThe total amount in the server across all accounts\n" +
            HelpExampleCli("getbalance", "") +
            "\nThe total amount in the server across all accounts, with at least 5 confirmations\n" + HelpExampleCli("getbalance", "\"*\" 6") +
            "\nThe total amount in the default account with at least 1 confirmation\n" + HelpExampleCli("getbalance", "\"\"") +
            "\nThe total amount in the account named tabby with at least 6 confirmations\n" + HelpExampleCli("getbalance", "\"tabby\" 6") +
            "\nAs a json rpc call\n" + HelpExampleRpc("getbalance", "\"tabby\", 6"));

    if (params.size() == 0)
        return ValueFromAmount(pwalletMain->GetBalance());

    int nMinDepth = 1;
    if (params.size() > 1)
        nMinDepth = params[1].get_int();
    isminefilter filter = ISMINE_SPENDABLE;
    if (params.size() > 2)
        if (params[2].get_bool())
            filter = filter | ISMINE_WATCH_ONLY;

    if (params[0].get_str() == "*") {
        // Calculate total balance a different way from GetBalance()
        // (GetBalance() sums up all unspent TxOuts)
        // getbalance and "getbalance * 1 true" should return the same number
        CAmount nBalance = 0;
        for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
            const CWalletTx& wtx = (*it).second;
            if (!IsFinalTx(wtx) || wtx.GetBlocksToMaturity() > 0 || wtx.GetDepthInMainChain() < 0)
                continue;

            CAmount allFee;
            string strSentAccount;
            list<COutputEntry> listReceived;
            list<COutputEntry> listSent;
            wtx.GetAmounts(listReceived, listSent, allFee, strSentAccount, filter);
            if (wtx.GetDepthInMainChain() >= nMinDepth) {
                BOOST_FOREACH (const COutputEntry& r, listReceived)
                    nBalance += r.amount;
            }
            BOOST_FOREACH (const COutputEntry& s, listSent)
                nBalance -= s.amount;
            nBalance -= allFee;
        }
        return ValueFromAmount(nBalance);
    }

    string strAccount = AccountFromValue(params[0]);

    CAmount nBalance = GetAccountBalance(strAccount, nMinDepth, filter);

    return ValueFromAmount(nBalance);
}

UniValue getunconfirmedbalance(const UniValue &params, bool fHelp)
{
    if (fHelp || params.size() > 0)
        throw runtime_error(
            "getunconfirmedbalance\n"
            "Returns the server's total unconfirmed balance\n");
    return ValueFromAmount(pwalletMain->GetUnconfirmedBalance());
}


UniValue movecmd(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 3 || params.size() > 5)
        throw runtime_error(
            "move \"fromaccount\" \"toaccount\" amount ( minconf \"comment\" )\n"
            "\nMove a specified amount from one account in your wallet to another.\n"
            "\nArguments:\n"
            "1. \"fromaccount\"   (string, required) The name of the account to move funds from. May be the default account using \"\".\n"
            "2. \"toaccount\"     (string, required) The name of the account to move funds to. May be the default account using \"\".\n"
            "3. minconf           (numeric, optional, default=1) Only use funds with at least this many confirmations.\n"
            "4. \"comment\"       (string, optional) An optional comment, stored in the wallet only.\n"
            "\nResult:\n"
            "true|false           (boolean) true if successfull.\n"
            "\nExamples:\n"
            "\nMove 0.01 btc from the default account to the account named tabby\n" +
            HelpExampleCli("move", "\"\" \"tabby\" 0.01") +
            "\nMove 0.01 btc timotei to akiko with a comment and funds have 6 confirmations\n" + HelpExampleCli("move", "\"timotei\" \"akiko\" 0.01 6 \"happy birthday!\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("move", "\"timotei\", \"akiko\", 0.01, 6, \"happy birthday!\""));

    string strFrom = AccountFromValue(params[0]);
    string strTo = AccountFromValue(params[1]);
    CAmount nAmount = AmountFromValue(params[2]);
    if (params.size() > 3)
        // unused parameter, used to be nMinDepth, keep type-checking it though
        (void)params[3].get_int();
    string strComment;
    if (params.size() > 4)
        strComment = params[4].get_str();

    CWalletDB walletdb(pwalletMain->strWalletFile);
    if (!walletdb.TxnBegin())
        throw JSONRPCError(RPC_DATABASE_ERROR, "database error");

    int64_t nNow = GetAdjustedTime();

    // Debit
    CAccountingEntry debit;
    debit.nOrderPos = pwalletMain->IncOrderPosNext(&walletdb);
    debit.strAccount = strFrom;
    debit.nCreditDebit = -nAmount;
    debit.nTime = nNow;
    debit.strOtherAccount = strTo;
    debit.strComment = strComment;
    walletdb.WriteAccountingEntry(debit);

    // Credit
    CAccountingEntry credit;
    credit.nOrderPos = pwalletMain->IncOrderPosNext(&walletdb);
    credit.strAccount = strTo;
    credit.nCreditDebit = nAmount;
    credit.nTime = nNow;
    credit.strOtherAccount = strFrom;
    credit.strComment = strComment;
    walletdb.WriteAccountingEntry(credit);

    if (!walletdb.TxnCommit())
        throw JSONRPCError(RPC_DATABASE_ERROR, "database error");

    return true;
}


UniValue sendfrom(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 3 || params.size() > 6)
        throw runtime_error(
            "sendfrom \"fromaccount\" \"togkcaddress\" amount ( minconf \"comment\" \"comment-to\" )\n"
            "\nSent an amount from an account to a gkc address.\n"
            "The amount is a real and is rounded to the nearest 0.00000001." +
            HelpRequiringPassphrase() + "\n"
                                        "\nArguments:\n"
                                        "1. \"fromaccount\"       (string, required) The name of the account to send funds from. May be the default account using \"\".\n"
                                        "2. \"togkcaddress\"  (string, required) The gkc address to send funds to.\n"
                                        "3. amount                (numeric, required) The amount in btc. (transaction fee is added on top).\n"
                                        "4. minconf               (numeric, optional, default=1) Only use funds with at least this many confirmations.\n"
                                        "5. \"comment\"           (string, optional) A comment used to store what the transaction is for. \n"
                                        "                                     This is not part of the transaction, just kept in your wallet.\n"
                                        "6. \"comment-to\"        (string, optional) An optional comment to store the name of the person or organization \n"
                                        "                                     to which you're sending the transaction. This is not part of the transaction, \n"
                                        "                                     it is just kept in your wallet.\n"
                                        "\nResult:\n"
                                        "\"transactionid\"        (string) The transaction id.\n"
                                        "\nExamples:\n"
                                        "\nSend 0.01 btc from the default account to the address, must have at least 1 confirmation\n" +
            HelpExampleCli("sendfrom", "\"\" \"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.01") +
            "\nSend 0.01 from the tabby account to the given address, funds must have at least 6 confirmations\n" + HelpExampleCli("sendfrom", "\"tabby\" \"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 0.01 6 \"donation\" \"seans outpost\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("sendfrom", "\"tabby\", \"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\", 0.01, 6, \"donation\", \"seans outpost\""));

    string strAccount = AccountFromValue(params[0]);
    CBitcoinAddress address(params[1].get_str());
    if (!address.IsGKC())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");
    CAmount nAmount = AmountFromValue(params[2]);
    int nMinDepth = 1;
    if (params.size() > 3)
        nMinDepth = params[3].get_int();

    CWalletTx wtx;
    wtx.strFromAccount = strAccount;
    if (params.size() > 4 && !params[4].isNull() && !params[4].get_str().empty())
        wtx.mapValue["comment"] = params[4].get_str();
    if (params.size() > 5 && !params[5].isNull() && !params[5].get_str().empty())
        wtx.mapValue["to"] = params[5].get_str();

    EnsureWalletIsUnlocked();

    // Check funds
    CAmount nBalance = GetAccountBalance(strAccount, nMinDepth, ISMINE_SPENDABLE);
    if (nAmount > nBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Account has insufficient funds");

    SendMoney(address.Get(), nAmount, wtx);

    return wtx.GetHash().GetHex();
}


UniValue sendmany(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 4)
        throw runtime_error(
            "sendmany \"fromaccount\" {\"address\":amount,...} ( minconf \"comment\" )\n"
            "\nSend multiple times. Amounts are double-precision floating point numbers." +
            HelpRequiringPassphrase() + "\n"
                                        "\nArguments:\n"
                                        "1. \"fromaccount\"         (string, required) The account to send the funds from, can be \"\" for the default account\n"
                                        "2. \"amounts\"             (string, required) A json object with addresses and amounts\n"
                                        "    {\n"
                                        "      \"address\":amount   (numeric) The gkc address is the key, the numeric amount in btc is the value\n"
                                        "      ,...\n"
                                        "    }\n"
                                        "3. minconf                 (numeric, optional, default=1) Only use the balance confirmed at least this many times.\n"
                                        "4. \"comment\"             (string, optional) A comment\n"
                                        "\nResult:\n"
                                        "\"transactionid\"          (string) The transaction id for the send. Only 1 transaction is created regardless of \n"
                                        "                                    the number of addresses.\n"
                                        "\nExamples:\n"
                                        "\nSend two amounts to two different addresses:\n" +
            HelpExampleCli("sendmany", "\"tabby\" \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\"") +
            "\nSend two amounts to two different addresses setting the confirmation and comment:\n" + HelpExampleCli("sendmany", "\"tabby\" \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\" 6 \"testing\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("sendmany", "\"tabby\", \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\", 6, \"testing\""));

    string strAccount = AccountFromValue(params[0]);
    UniValue sendTo = params[1].get_obj();
    int nMinDepth = 1;
    if (params.size() > 2)
        nMinDepth = params[2].get_int();

    CWalletTx wtx;
    wtx.strFromAccount = strAccount;
    if (params.size() > 3 && !params[3].isNull() && !params[3].get_str().empty())
        wtx.mapValue["comment"] = params[3].get_str();

    set<CBitcoinAddress> setAddress;
    vector<pair<CScript, CAmount> > vecSend;

    CAmount totalAmount = 0;
    vector<string> keys = sendTo.getKeys();
    BOOST_FOREACH(const string& name_, keys) {
        CBitcoinAddress address(name_);
        if (!address.IsGKC())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, string("Invalid GKC address: ") + name_);

        if (setAddress.count(address))
            throw JSONRPCError(RPC_INVALID_PARAMETER, string("Invalid parameter, duplicated address: ") + name_);
        setAddress.insert(address);

        CScript scriptPubKey = GetScriptForDestination(address.Get());
        CAmount nAmount = AmountFromValue(sendTo[name_]);
        totalAmount += nAmount;

        vecSend.push_back(make_pair(scriptPubKey, nAmount));
    }

    EnsureWalletIsUnlocked();

    // Check funds
    CAmount nBalance = GetAccountBalance(strAccount, nMinDepth, ISMINE_SPENDABLE);
    if (totalAmount > nBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Account has insufficient funds");

    // Send
    CReserveKey keyChange(pwalletMain);
    CAmount nFeeRequired = 0;
    string strFailReason;
    bool fCreated = pwalletMain->CreateTransaction(vecSend, wtx, keyChange, nFeeRequired, strFailReason);
    if (!fCreated)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, strFailReason);
    if (!pwalletMain->CommitTransaction(wtx, keyChange))
        throw JSONRPCError(RPC_WALLET_ERROR, "Transaction commit failed");

    return wtx.GetHash().GetHex();
}

UniValue selectutxotosendmany(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 5)
        throw runtime_error(
            "selectutxotosendmany minutxo {\"address\":amount,...} ( minconf \"comment\" )\n"
            "\nSend multiple times. Amounts are double-precision floating point numbers." +
            HelpRequiringPassphrase() + "\n"
                                        "\nArguments:\n"
                                        "1. minutxo         		(numeric, required) The min value of utxo to select as vin\n"
                                        "2. \"amounts\"             (string, required) A json object with addresses and amounts\n"
                                        "    {\n"
                                        "      \"address\":amount   (numeric) The gkc address is the key, the numeric amount in btc is the value\n"
                                        "      ,...\n"
                                        "    }\n"
                                        "3. minconf                 (numeric, optional, default=1) Only use the balance confirmed at least this many times.\n"
                                        "4. \"comment\"             (string, optional) A comment\n"
										"5. \"change_address\"		(string, optional) A gkc address\n"
                                        "\nResult:\n"
                                        "\"transactionid\"          (string) The transaction id for the send. Only 1 transaction is created regardless of \n"
                                        "                                    the number of addresses.\n"
                                        "\nExamples:\n"
                                        "\nSend two amounts to two different addresses:\n" +
            HelpExampleCli("selectutxotosendmany", "10 \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\"") +
            "\nSend two amounts to two different addresses setting the confirmation and comment:\n" + HelpExampleCli("selectutxotosendmany", "10 \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\" 6 \"testing\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("selectutxotosendmany", "10, \"{\\\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\\\":0.01,\\\"XuQQkwA4FYkq2XERzMY2CiAZhJTEDAbtcg\\\":0.02}\", 6, \"testing\""));

    int minutxo = params[0].get_int();
    UniValue sendTo = params[1].get_obj();
    int nMinDepth = 1;
    if (params.size() > 2)
        nMinDepth = params[2].get_int();

    CWalletTx wtx;
    if (params.size() > 3 && !params[3].isNull() && !params[3].get_str().empty())
        wtx.mapValue["comment"] = params[3].get_str();

    set<CBitcoinAddress> setAddress;
    vector<pair<CScript, CAmount> > vecSend;

    CAmount totalAmount = 0;
    vector<string> keys = sendTo.getKeys();
    BOOST_FOREACH(const string& name_, keys) {
        CBitcoinAddress address(name_);
        if (!address.IsGKC())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, string("Invalid GKC address: ") + name_);

        if (setAddress.count(address))
            throw JSONRPCError(RPC_INVALID_PARAMETER, string("Invalid parameter, duplicated address: ") + name_);
        setAddress.insert(address);

        CScript scriptPubKey = GetScriptForDestination(address.Get());
        CAmount nAmount = AmountFromValue(sendTo[name_]);
        totalAmount += nAmount;

        vecSend.push_back(make_pair(scriptPubKey, nAmount));
    }

    EnsureWalletIsUnlocked();

    // Check funds
    if (totalAmount > pwalletMain->GetBalance())
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Wallet has insufficient funds");

	CCoinControl coinCtrl;
	coinCtrl.nMinUtxoValue = minutxo;
    if (params.size() > 4 && !params[4].isNull() && !params[4].get_str().empty()){
        CBitcoinAddress changeAddress(params[4].get_str());
		coinCtrl.destChange = changeAddress.Get();
	}

    // Send
    CReserveKey keyChange(pwalletMain);
    CAmount nFeeRequired = 0;
    string strFailReason;
    bool fCreated = pwalletMain->CreateTransaction(vecSend, wtx, keyChange, nFeeRequired, strFailReason, &coinCtrl);
    if (!fCreated)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, strFailReason);
    if (!pwalletMain->CommitTransaction(wtx, keyChange))
        throw JSONRPCError(RPC_WALLET_ERROR, "Transaction commit failed");

    return wtx.GetHash().GetHex();
}
// Defined in rpcmisc.cpp
extern CScript _createmultisig_redeemScript(const UniValue& params);

UniValue addmultisigaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3) {
        string msg = "addmultisigaddress nrequired [\"key\",...] ( \"account\" )\n"
                     "\nAdd a nrequired-to-sign multisignature address to the wallet.\n"
                     "Each key is a GKC address or hex-encoded public key.\n"
                     "If 'account' is specified, assign address to that account.\n"

                     "\nArguments:\n"
                     "1. nrequired        (numeric, required) The number of required signatures out of the n keys or addresses.\n"
                     "2. \"keysobject\"   (string, required) A json array of gkc addresses or hex-encoded public keys\n"
                     "     [\n"
                     "       \"address\"  (string) gkc address or hex-encoded public key\n"
                     "       ...,\n"
                     "     ]\n"
                     "3. \"account\"      (string, optional) An account to assign the addresses to.\n"

                     "\nResult:\n"
                     "\"gkcaddress\"  (string) A gkc address associated with the keys.\n"

                     "\nExamples:\n"
                     "\nAdd a multisig address from 2 addresses\n" +
                     HelpExampleCli("addmultisigaddress", "2 \"[\\\"Xt4qk9uKvQYAonVGSZNXqxeDmtjaEWgfrs\\\",\\\"XoSoWQkpgLpppPoyyzbUFh1fq2RBvW6UK1\\\"]\"") +
                     "\nAs json rpc call\n" + HelpExampleRpc("addmultisigaddress", "2, \"[\\\"Xt4qk9uKvQYAonVGSZNXqxeDmtjaEWgfrs\\\",\\\"XoSoWQkpgLpppPoyyzbUFh1fq2RBvW6UK1\\\"]\"");
        throw runtime_error(msg);
    }

    string strAccount;
    if (params.size() > 2)
        strAccount = AccountFromValue(params[2]);

    // Construct using pay-to-script-hash:
    CScript inner = _createmultisig_redeemScript(params);
    CScriptID innerID(inner);
    pwalletMain->AddCScript(inner);

    pwalletMain->SetAddressBook(innerID, strAccount, "send");
    return CBitcoinAddress(innerID).ToString();
}


struct tallyitem {
    CAmount nAmount;
    int nConf;
    int nBCConf;
    vector<uint256> txids;
    bool fIsWatchonly;
    tallyitem()
    {
        nAmount = 0;
        nConf = std::numeric_limits<int>::max();
        nBCConf = std::numeric_limits<int>::max();
        fIsWatchonly = false;
    }
};

UniValue ListReceived(const UniValue& params, bool fByAccounts)
{
    // Minimum confirmations
    int nMinDepth = 1;
    if (params.size() > 0)
        nMinDepth = params[0].get_int();

    // Whether to include empty accounts
    bool fIncludeEmpty = false;
    if (params.size() > 1)
        fIncludeEmpty = params[1].get_bool();

    isminefilter filter = ISMINE_SPENDABLE;
    if (params.size() > 2)
        if (params[2].get_bool())
            filter = filter | ISMINE_WATCH_ONLY;

    // Tally
    map<CBitcoinAddress, tallyitem> mapTally;
    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
        const CWalletTx& wtx = (*it).second;

        if (wtx.IsCoinBase() || !IsFinalTx(wtx))
            continue;

        int nDepth = wtx.GetDepthInMainChain();
        int nBCDepth = wtx.GetDepthInMainChain(false);
        if (nDepth < nMinDepth)
            continue;

        BOOST_FOREACH (const CTxOut& txout, wtx.vout) {
            CTxDestination address;
            if (!ExtractDestination(txout.scriptPubKey, address))
                continue;

            isminefilter mine = IsMine(*pwalletMain, address);
            if (!(mine & filter))
                continue;

            tallyitem& item = mapTally[address];
            item.nAmount += txout.nValue;
            item.nConf = min(item.nConf, nDepth);
            item.nBCConf = min(item.nBCConf, nBCDepth);
            item.txids.push_back(wtx.GetHash());
            if (mine & ISMINE_WATCH_ONLY)
                item.fIsWatchonly = true;
        }
    }

    // Reply
    UniValue ret(UniValue::VARR);
    map<string, tallyitem> mapAccountTally;
    BOOST_FOREACH (const PAIRTYPE(CBitcoinAddress, CAddressBookData) & item, pwalletMain->mapAddressBook) {
        const CBitcoinAddress& address = item.first;
        const string& strAccount = item.second.name;
        map<CBitcoinAddress, tallyitem>::iterator it = mapTally.find(address);
        if (it == mapTally.end() && !fIncludeEmpty)
            continue;

        CAmount nAmount = 0;
        int nConf = std::numeric_limits<int>::max();
        int nBCConf = std::numeric_limits<int>::max();
        bool fIsWatchonly = false;
        if (it != mapTally.end()) {
            nAmount = (*it).second.nAmount;
            nConf = (*it).second.nConf;
            nBCConf = (*it).second.nBCConf;
            fIsWatchonly = (*it).second.fIsWatchonly;
        }

        if (fByAccounts) {
            tallyitem& item = mapAccountTally[strAccount];
            item.nAmount += nAmount;
            item.nConf = min(item.nConf, nConf);
            item.nBCConf = min(item.nBCConf, nBCConf);
            item.fIsWatchonly = fIsWatchonly;
        } else {
            UniValue obj(UniValue::VOBJ);
            if (fIsWatchonly)
                obj.push_back(Pair("involvesWatchonly", true));
            obj.push_back(Pair("address", address.ToString()));
            obj.push_back(Pair("account", strAccount));
            obj.push_back(Pair("amount", ValueFromAmount(nAmount)));
            obj.push_back(Pair("confirmations", (nConf == std::numeric_limits<int>::max() ? 0 : nConf)));
            obj.push_back(Pair("bcconfirmations", (nBCConf == std::numeric_limits<int>::max() ? 0 : nBCConf)));
            UniValue transactions(UniValue::VARR);
            if (it != mapTally.end()) {
                BOOST_FOREACH (const uint256& item, (*it).second.txids) {
                    transactions.push_back(item.GetHex());
                }
            }
            obj.push_back(Pair("txids", transactions));
            ret.push_back(obj);
        }
    }

    if (fByAccounts) {
        for (map<string, tallyitem>::iterator it = mapAccountTally.begin(); it != mapAccountTally.end(); ++it) {
            CAmount nAmount = (*it).second.nAmount;
            int nConf = (*it).second.nConf;
            int nBCConf = (*it).second.nBCConf;
            UniValue obj(UniValue::VOBJ);
            if ((*it).second.fIsWatchonly)
                obj.push_back(Pair("involvesWatchonly", true));
            obj.push_back(Pair("account", (*it).first));
            obj.push_back(Pair("amount", ValueFromAmount(nAmount)));
            obj.push_back(Pair("confirmations", (nConf == std::numeric_limits<int>::max() ? 0 : nConf)));
            obj.push_back(Pair("bcconfirmations", (nBCConf == std::numeric_limits<int>::max() ? 0 : nBCConf)));
            ret.push_back(obj);
        }
    }

    return ret;
}

UniValue listreceivedbyaddress(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 3)
        throw runtime_error(
            "listreceivedbyaddress ( minconf includeempty includeWatchonly)\n"
            "\nList balances by receiving address.\n"
            "\nArguments:\n"
            "1. minconf       (numeric, optional, default=1) The minimum number of confirmations before payments are included.\n"
            "2. includeempty  (numeric, optional, default=false) Whether to include addresses that haven't received any payments.\n"
            "3. includeWatchonly (bool, optional, default=false) Whether to include watchonly addresses (see 'importaddress').\n"

            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"involvesWatchonly\" : \"true\",    (bool) Only returned if imported addresses were involved in transaction\n"
            "    \"address\" : \"receivingaddress\",  (string) The receiving address\n"
            "    \"account\" : \"accountname\",       (string) The account of the receiving address. The default account is \"\".\n"
            "    \"amount\" : x.xxx,                  (numeric) The total amount in btc received by the address\n"
            "    \"confirmations\" : n                (numeric) The number of confirmations of the most recent transaction included\n"
            "    \"bcconfirmations\" : n              (numeric) The number of blockchain confirmations of the most recent transaction included\n"
            "  }\n"
            "  ,...\n"
            "]\n"

            "\nExamples:\n" +
            HelpExampleCli("listreceivedbyaddress", "") + HelpExampleCli("listreceivedbyaddress", "6 true") + HelpExampleRpc("listreceivedbyaddress", "6, true, true"));

    return ListReceived(params, false);
}

UniValue listreceivedbyaccount(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 3)
        throw runtime_error(
            "listreceivedbyaccount ( minconf includeempty includeWatchonly)\n"
            "\nList balances by account.\n"
            "\nArguments:\n"
            "1. minconf      (numeric, optional, default=1) The minimum number of confirmations before payments are included.\n"
            "2. includeempty (boolean, optional, default=false) Whether to include accounts that haven't received any payments.\n"
            "3. includeWatchonly (bool, optional, default=false) Whether to include watchonly addresses (see 'importaddress').\n"

            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"involvesWatchonly\" : \"true\",    (bool) Only returned if imported addresses were involved in transaction\n"
            "    \"account\" : \"accountname\",  (string) The account name of the receiving account\n"
            "    \"amount\" : x.xxx,             (numeric) The total amount received by addresses with this account\n"
            "    \"confirmations\" : n           (numeric) The number of confirmations of the most recent transaction included\n"
            "    \"bcconfirmations\" : n         (numeric) The number of blockchain confirmations of the most recent transaction included\n"
            "  }\n"
            "  ,...\n"
            "]\n"

            "\nExamples:\n" +
            HelpExampleCli("listreceivedbyaccount", "") + HelpExampleCli("listreceivedbyaccount", "6 true") + HelpExampleRpc("listreceivedbyaccount", "6, true, true"));

    return ListReceived(params, true);
}

static void MaybePushAddress(UniValue & entry, const CTxDestination &dest)
{
    CBitcoinAddress addr;
    if (addr.Set(dest))
        entry.push_back(Pair("address", addr.ToString()));
}

void ListTransactions(const CWalletTx& wtx, const string& strAccount, int nMinDepth, bool fLong, UniValue& ret, const isminefilter& filter)
{
    CAmount nFee;
    string strSentAccount;
    list<COutputEntry> listReceived;
    list<COutputEntry> listSent;

    wtx.GetAmounts(listReceived, listSent, nFee, strSentAccount, filter);

    bool fAllAccounts = (strAccount == string("*"));
    bool involvesWatchonly = wtx.IsFromMe(ISMINE_WATCH_ONLY);

    // Sent
    if ((!listSent.empty() || nFee != 0) && (fAllAccounts || strAccount == strSentAccount)) {
        BOOST_FOREACH (const COutputEntry& s, listSent) {
            UniValue entry(UniValue::VOBJ);
            if (involvesWatchonly || (::IsMine(*pwalletMain, s.destination) & ISMINE_WATCH_ONLY))
                entry.push_back(Pair("involvesWatchonly", true));
            entry.push_back(Pair("account", strSentAccount));
            MaybePushAddress(entry, s.destination);
            std::map<std::string, std::string>::const_iterator it = wtx.mapValue.find("DS");
            entry.push_back(Pair("category", (it != wtx.mapValue.end() && it->second == "1") ? "darksent" : "send"));
            entry.push_back(Pair("amount", ValueFromAmount(-s.amount)));
            entry.push_back(Pair("vout", s.vout));
            entry.push_back(Pair("fee", ValueFromAmount(-nFee)));
            if (fLong)
                WalletTxToJSON(wtx, entry);
            ret.push_back(entry);
        }
    }

    // Received
    if (listReceived.size() > 0 && wtx.GetDepthInMainChain() >= nMinDepth) {
        BOOST_FOREACH (const COutputEntry& r, listReceived) {
            string account;
            if (pwalletMain->mapAddressBook.count(r.destination))
                account = pwalletMain->mapAddressBook[r.destination].name;
            if (fAllAccounts || (account == strAccount)) {
                UniValue entry(UniValue::VOBJ);
                if (involvesWatchonly || (::IsMine(*pwalletMain, r.destination) & ISMINE_WATCH_ONLY))
                    entry.push_back(Pair("involvesWatchonly", true));
                entry.push_back(Pair("account", account));
                MaybePushAddress(entry, r.destination);
                if (wtx.IsCoinBase()) {
                    if (wtx.GetDepthInMainChain() < 1)
                        entry.push_back(Pair("category", "orphan"));
                    else if (wtx.GetBlocksToMaturity() > 0)
                        entry.push_back(Pair("category", "immature"));
                    else
                        entry.push_back(Pair("category", "generate"));
                } else {
                    entry.push_back(Pair("category", "receive"));
                }
                entry.push_back(Pair("amount", ValueFromAmount(r.amount)));
                entry.push_back(Pair("vout", r.vout));
                if (fLong)
                    WalletTxToJSON(wtx, entry);
                ret.push_back(entry);
            }
        }
    }
}

void AcentryToJSON(const CAccountingEntry& acentry, const string& strAccount, UniValue& ret)
{
    bool fAllAccounts = (strAccount == string("*"));

    if (fAllAccounts || acentry.strAccount == strAccount) {
        UniValue entry(UniValue::VOBJ);
        entry.push_back(Pair("account", acentry.strAccount));
        entry.push_back(Pair("category", "move"));
        entry.push_back(Pair("time", acentry.nTime));
        entry.push_back(Pair("amount", ValueFromAmount(acentry.nCreditDebit)));
        entry.push_back(Pair("otheraccount", acentry.strOtherAccount));
        entry.push_back(Pair("comment", acentry.strComment));
        ret.push_back(entry);
    }
}

UniValue listtransactions(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 4)
        throw runtime_error(
            "listtransactions ( \"account\" count from includeWatchonly)\n"
            "\nReturns up to 'count' most recent transactions skipping the first 'from' transactions for account 'account'.\n"
            "\nArguments:\n"
            "1. \"account\"    (string, optional) The account name. If not included, it will list all transactions for all accounts.\n"
            "                                     If \"\" is set, it will list transactions for the default account.\n"
            "2. count          (numeric, optional, default=10) The number of transactions to return\n"
            "3. from           (numeric, optional, default=0) The number of transactions to skip\n"
            "4. includeWatchonly (bool, optional, default=false) Include transactions to watchonly addresses (see 'importaddress')\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"account\":\"accountname\",       (string) The account name associated with the transaction. \n"
            "                                                It will be \"\" for the default account.\n"
            "    \"address\":\"gkcaddress\",    (string) The gkc address of the transaction. Not present for \n"
            "                                                move transactions (category = move).\n"
            "    \"category\":\"send|receive|move\", (string) The transaction category. 'move' is a local (off blockchain)\n"
            "                                                transaction between accounts, and not associated with an address,\n"
            "                                                transaction id or block. 'send' and 'receive' transactions are \n"
            "                                                associated with an address, transaction id and block details\n"
            "    \"amount\": x.xxx,          (numeric) The amount in btc. This is negative for the 'send' category, and for the\n"
            "                                         'move' category for moves outbound. It is positive for the 'receive' category,\n"
            "                                         and for the 'move' category for inbound funds.\n"
            "    \"vout\" : n,               (numeric) the vout value\n"
            "    \"fee\": x.xxx,             (numeric) The amount of the fee in btc. This is negative and only available for the \n"
            "                                         'send' category of transactions.\n"
            "    \"confirmations\": n,       (numeric) The number of confirmations for the transaction. Available for 'send' and \n"
            "                                         'receive' category of transactions.\n"
            "    \"bcconfirmations\": n,     (numeric) The number of blockchain confirmations for the transaction. Available for 'send'\n"
            "                                          and 'receive' category of transactions.\n"
            "    \"blockhash\": \"hashvalue\", (string) The block hash containing the transaction. Available for 'send' and 'receive'\n"
            "                                          category of transactions.\n"
            "    \"blockindex\": n,          (numeric) The block index containing the transaction. Available for 'send' and 'receive'\n"
            "                                          category of transactions.\n"
            "    \"txid\": \"transactionid\", (string) The transaction id. Available for 'send' and 'receive' category of transactions.\n"
            "    \"time\": xxx,              (numeric) The transaction time in seconds since epoch (midnight Jan 1 1970 GMT).\n"
            "    \"timereceived\": xxx,      (numeric) The time received in seconds since epoch (midnight Jan 1 1970 GMT). Available \n"
            "                                          for 'send' and 'receive' category of transactions.\n"
            "    \"comment\": \"...\",       (string) If a comment is associated with the transaction.\n"
            "    \"otheraccount\": \"accountname\",  (string) For the 'move' category of transactions, the account the funds came \n"
            "                                          from (for receiving funds, positive amounts), or went to (for sending funds,\n"
            "                                          negative amounts).\n"
            "  }\n"
            "]\n"

            "\nExamples:\n"
            "\nList the most recent 10 transactions in the systems\n" +
            HelpExampleCli("listtransactions", "") +
            "\nList the most recent 10 transactions for the tabby account\n" + HelpExampleCli("listtransactions", "\"tabby\"") +
            "\nList transactions 100 to 120 from the tabby account\n" + HelpExampleCli("listtransactions", "\"tabby\" 20 100") +
            "\nAs a json rpc call\n" + HelpExampleRpc("listtransactions", "\"tabby\", 20, 100"));

    string strAccount = "*";
    if (params.size() > 0)
        strAccount = params[0].get_str();
    int nCount = 10;
    if (params.size() > 1)
        nCount = params[1].get_int();
    int nFrom = 0;
    if (params.size() > 2)
        nFrom = params[2].get_int();
    isminefilter filter = ISMINE_SPENDABLE;
    if (params.size() > 3)
        if (params[3].get_bool())
            filter = filter | ISMINE_WATCH_ONLY;

    if (nCount < 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Negative count");
    if (nFrom < 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Negative from");

    UniValue ret(UniValue::VARR);

    std::list<CAccountingEntry> acentries;
    CWallet::TxItems txOrdered = pwalletMain->OrderedTxItems(acentries, strAccount);

    // iterate backwards until we have nCount items to return:
    for (CWallet::TxItems::reverse_iterator it = txOrdered.rbegin(); it != txOrdered.rend(); ++it) {
        CWalletTx* const pwtx = (*it).second.first;
        if (pwtx != 0)
            ListTransactions(*pwtx, strAccount, 0, true, ret, filter);
        CAccountingEntry* const pacentry = (*it).second.second;
        if (pacentry != 0)
            AcentryToJSON(*pacentry, strAccount, ret);

        if ((int)ret.size() >= (nCount + nFrom)) break;
    }
    // ret is newest to oldest

    if (nFrom > (int)ret.size())
        nFrom = ret.size();
    if ((nFrom + nCount) > (int)ret.size())
        nCount = ret.size() - nFrom;

    vector<UniValue> arrTmp = ret.getValues();

    vector<UniValue>::iterator first = arrTmp.begin();
    std::advance(first, nFrom);
    vector<UniValue>::iterator last = arrTmp.begin();
    std::advance(last, nFrom+nCount);

    if (last != arrTmp.end()) arrTmp.erase(last, arrTmp.end());
    if (first != arrTmp.begin()) arrTmp.erase(arrTmp.begin(), first);

    std::reverse(arrTmp.begin(), arrTmp.end()); // Return oldest to newest

    ret.clear();
    ret.setArray();
    ret.push_backV(arrTmp);

    return ret;
}

UniValue listaccounts(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 2)
        throw runtime_error(
            "listaccounts ( minconf includeWatchonly)\n"
            "\nReturns Object that has account names as keys, account balances as values.\n"
            "\nArguments:\n"
            "1. minconf          (numeric, optional, default=1) Only include transactions with at least this many confirmations\n"
            "2. includeWatchonly (bool, optional, default=false) Include balances in watchonly addresses (see 'importaddress')\n"
            "\nResult:\n"
            "{                      (json object where keys are account names, and values are numeric balances\n"
            "  \"account\": x.xxx,  (numeric) The property name is the account name, and the value is the total balance for the account.\n"
            "  ...\n"
            "}\n"
            "\nExamples:\n"
            "\nList account balances where there at least 1 confirmation\n" +
            HelpExampleCli("listaccounts", "") +
            "\nList account balances including zero confirmation transactions\n" + HelpExampleCli("listaccounts", "0") +
            "\nList account balances for 6 or more confirmations\n" + HelpExampleCli("listaccounts", "6") +
            "\nAs json rpc call\n" + HelpExampleRpc("listaccounts", "6"));

    int nMinDepth = 1;
    if (params.size() > 0)
        nMinDepth = params[0].get_int();
    isminefilter includeWatchonly = ISMINE_SPENDABLE;
    if (params.size() > 1)
        if (params[1].get_bool())
            includeWatchonly = includeWatchonly | ISMINE_WATCH_ONLY;

    map<string, CAmount> mapAccountBalances;
    BOOST_FOREACH (const PAIRTYPE(CTxDestination, CAddressBookData) & entry, pwalletMain->mapAddressBook) {
		LogPrint("wallet","listaccounts | mapAddressBook entry: address=%s, name=%s\n",CBitcoinAddress(entry.first).ToString(),entry.second.name);
        if (IsMine(*pwalletMain, entry.first) & includeWatchonly){ // This address belongs to me
            mapAccountBalances[entry.second.name] = 0;
			LogPrint("wallet","listaccounts | mapAccountBalances[%s]=0\n",entry.second.name);
        }
    }

    //CCoinsViewCache view(pcoinsTip);
    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it) {
		const uint256& txid = it->first;
		LogPrint("wallet","listaccounts | txid=%s\n",txid.ToString());
        const CWalletTx& wtx = (*it).second;
        CAmount nFee;
        string strSentAccount;
        list<COutputEntry> listReceived;
        list<COutputEntry> listSent;
        int nDepth = wtx.GetDepthInMainChain();
        if (wtx.GetBlocksToMaturity() > 0 || nDepth < 0)
            continue;
        wtx.GetAmounts(listReceived, listSent, nFee, strSentAccount, includeWatchonly);
        for(const CTxIn& txin : wtx.vin) {
			//const CTxOut &prevout = view.AccessCoins(txin.prevout.hash)->vout[txin.prevout.n];
			LogPrint("wallet","listaccounts | txin.prevout=%s\n",txin.prevout.ToString());
			CTxOut prevout;
			if(!dpos::GetConfirmedTxOut(txin.prevout,prevout)){
				LogPrint("wallet","listaccounts | prevout not found | prevout=%s\n",txin.prevout.ToString());
				continue;
			}
			if(!pwalletMain->IsMine(prevout))
				continue;
			CTxDestination address;
			if (!ExtractDestination(prevout.scriptPubKey, address)){
				LogPrint("wallet","listaccounts | prevout.scriptPubKey invalid | prevout.scriptPubKey=%s\n",prevout.scriptPubKey.ToString());
				continue;
			}
			LogPrint("wallet","listaccounts | send from address=%s, value=%lld\n",CBitcoinAddress(address).ToString(),prevout.nValue);
			if (pwalletMain->mapAddressBook.count(address)){
				mapAccountBalances[pwalletMain->mapAddressBook[address].name] -= prevout.nValue;
				LogPrint("wallet","listaccounts | mapAccountBalances[%s], -%lld, =%lld\n",pwalletMain->mapAddressBook[address].name,prevout.nValue,mapAccountBalances[pwalletMain->mapAddressBook[address].name]);
			}
			else {
				mapAccountBalances[""] -= prevout.nValue;
				LogPrint("wallet","listaccounts | address not found in mapAddressBook | mapAccountBalances[\"\"], -%lld, =%lld\n",prevout.nValue,mapAccountBalances[""]);
			}
        }
		
        if (nDepth >= nMinDepth) {
	        for(const CTxOut& txout : wtx.vout) {
				LogPrint("wallet","listaccounts | txout=%s\n",txout.ToString());
				if(!pwalletMain->IsMine(txout)){
					LogPrint("wallet","listaccounts | txout is not to me.\n");
					continue;
				}
				CTxDestination address;
				if (!ExtractDestination(txout.scriptPubKey, address)){
					LogPrint("wallet","listaccounts | txout.scriptPubKey invalid | txout.scriptPubKey=%s\n",txout.scriptPubKey.ToString());
					continue;
				}
				LogPrint("wallet","listaccounts | receive at address=%s, value=%lld\n",CBitcoinAddress(address).ToString(),txout.nValue);
				if (pwalletMain->mapAddressBook.count(address)){
					mapAccountBalances[pwalletMain->mapAddressBook[address].name] += txout.nValue;
					LogPrint("wallet","listaccounts | mapAccountBalances[%s], +%lld, =%lld\n",pwalletMain->mapAddressBook[address].name,txout.nValue,mapAccountBalances[pwalletMain->mapAddressBook[address].name]);
				}
				else {
					mapAccountBalances[""] += txout.nValue;
					LogPrint("wallet","listaccounts | address not found in mapAddressBook | mapAccountBalances[\"\"], +%lld, =%lld\n",txout.nValue,mapAccountBalances[""]);
				}
	        }
        }
    }

    UniValue ret(UniValue::VOBJ);
    BOOST_FOREACH (const PAIRTYPE(string, CAmount) & accountBalance, mapAccountBalances) {
        ret.push_back(Pair(accountBalance.first, ValueFromAmount(accountBalance.second)));
    }
    return ret;
}

UniValue listsinceblock(const UniValue& params, bool fHelp)
{
    if (fHelp)
        throw runtime_error(
            "listsinceblock ( \"blockhash\" target-confirmations includeWatchonly)\n"
            "\nGet all transactions in blocks since block [blockhash], or all transactions if omitted\n"
            "\nArguments:\n"
            "1. \"blockhash\"   (string, optional) The block hash to list transactions since\n"
            "2. target-confirmations:    (numeric, optional) The confirmations required, must be 1 or more\n"
            "3. includeWatchonly:        (bool, optional, default=false) Include transactions to watchonly addresses (see 'importaddress')"
            "\nResult:\n"
            "{\n"
            "  \"transactions\": [\n"
            "    \"account\":\"accountname\",       (string) The account name associated with the transaction. Will be \"\" for the default account.\n"
            "    \"address\":\"gkcaddress\",    (string) The gkc address of the transaction. Not present for move transactions (category = move).\n"
            "    \"category\":\"send|receive\",     (string) The transaction category. 'send' has negative amounts, 'receive' has positive amounts.\n"
            "    \"amount\": x.xxx,          (numeric) The amount in btc. This is negative for the 'send' category, and for the 'move' category for moves \n"
            "                                          outbound. It is positive for the 'receive' category, and for the 'move' category for inbound funds.\n"
            "    \"vout\" : n,               (numeric) the vout value\n"
            "    \"fee\": x.xxx,             (numeric) The amount of the fee in btc. This is negative and only available for the 'send' category of transactions.\n"
            "    \"confirmations\": n,       (numeric) The number of confirmations for the transaction. Available for 'send' and 'receive' category of transactions.\n"
            "    \"bcconfirmations\" : n,    (numeric) The number of blockchain confirmations for the transaction. Available for 'send' and 'receive' category of transactions.\n"
            "    \"blockhash\": \"hashvalue\",     (string) The block hash containing the transaction. Available for 'send' and 'receive' category of transactions.\n"
            "    \"blockindex\": n,          (numeric) The block index containing the transaction. Available for 'send' and 'receive' category of transactions.\n"
            "    \"blocktime\": xxx,         (numeric) The block time in seconds since epoch (1 Jan 1970 GMT).\n"
            "    \"txid\": \"transactionid\",  (string) The transaction id. Available for 'send' and 'receive' category of transactions.\n"
            "    \"time\": xxx,              (numeric) The transaction time in seconds since epoch (Jan 1 1970 GMT).\n"
            "    \"timereceived\": xxx,      (numeric) The time received in seconds since epoch (Jan 1 1970 GMT). Available for 'send' and 'receive' category of transactions.\n"
            "    \"comment\": \"...\",       (string) If a comment is associated with the transaction.\n"
            "    \"to\": \"...\",            (string) If a comment to is associated with the transaction.\n"
            "  ],\n"
            "  \"lastblock\": \"lastblockhash\"     (string) The hash of the last block\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("listsinceblock", "") + HelpExampleCli("listsinceblock", "\"000000000000000bacf66f7497b7dc45ef753ee9a7d38571037cdb1a57f663ad\" 6") + HelpExampleRpc("listsinceblock", "\"000000000000000bacf66f7497b7dc45ef753ee9a7d38571037cdb1a57f663ad\", 6"));

    CBlockIndex* pindex = NULL;
    int target_confirms = 1;
    isminefilter filter = ISMINE_SPENDABLE;

    if (params.size() > 0) {
        uint256 blockId = 0;

        blockId.SetHex(params[0].get_str());
        BlockMap::iterator it = mapBlockIndex.find(blockId);
        if (it != mapBlockIndex.end())
            pindex = it->second;
    }

    if (params.size() > 1) {
        target_confirms = params[1].get_int();

        if (target_confirms < 1)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter");
    }

    if (params.size() > 2)
        if (params[2].get_bool())
            filter = filter | ISMINE_WATCH_ONLY;

    int depth = pindex ? (1 + chainActive.Height() - pindex->nHeight) : -1;

    UniValue transactions(UniValue::VARR);

    for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); it++) {
        CWalletTx tx = (*it).second;

        if (depth == -1 || tx.GetDepthInMainChain(false) < depth)
            ListTransactions(tx, "*", 0, true, transactions, filter);
    }

    CBlockIndex* pblockLast = chainActive[chainActive.Height() + 1 - target_confirms];
    uint256 lastblock = pblockLast ? pblockLast->GetBlockHash() : 0;

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("transactions", transactions));
    ret.push_back(Pair("lastblock", lastblock.GetHex()));

    return ret;
}

UniValue gettransaction(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "gettransaction \"txid\" ( includeWatchonly )\n"
            "\nGet detailed information about in-wallet transaction <txid>\n"
            "\nArguments:\n"
            "1. \"txid\"    (string, required) The transaction id\n"
            "2. \"includeWatchonly\"    (bool, optional, default=false) Whether to include watchonly addresses in balance calculation and details[]\n"
            "\nResult:\n"
            "{\n"
            "  \"amount\" : x.xxx,        (numeric) The transaction amount in btc\n"
            "  \"confirmations\" : n,     (numeric) The number of confirmations\n"
            "  \"bcconfirmations\" : n,   (numeric) The number of blockchain confirmations\n"
            "  \"blockhash\" : \"hash\",  (string) The block hash\n"
            "  \"blockindex\" : xx,       (numeric) The block index\n"
            "  \"blocktime\" : ttt,       (numeric) The time in seconds since epoch (1 Jan 1970 GMT)\n"
            "  \"txid\" : \"transactionid\",   (string) The transaction id.\n"
            "  \"time\" : ttt,            (numeric) The transaction time in seconds since epoch (1 Jan 1970 GMT)\n"
            "  \"timereceived\" : ttt,    (numeric) The time received in seconds since epoch (1 Jan 1970 GMT)\n"
            "  \"details\" : [\n"
            "    {\n"
            "      \"account\" : \"accountname\",  (string) The account name involved in the transaction, can be \"\" for the default account.\n"
            "      \"address\" : \"gkcaddress\",   (string) The gkc address involved in the transaction\n"
            "      \"category\" : \"send|receive\",    (string) The category, either 'send' or 'receive'\n"
            "      \"amount\" : x.xxx                  (numeric) The amount in btc\n"
            "      \"vout\" : n,                       (numeric) the vout value\n"
            "    }\n"
            "    ,...\n"
            "  ],\n"
            "  \"hex\" : \"data\"         (string) Raw data for transaction\n"
            "}\n"

            "\nExamples:\n" +
            HelpExampleCli("gettransaction", "\"1075db55d416d3ca199f55b6084e2115b9345e16c5cf302fc80e9d5fbf5d48d\"") + HelpExampleCli("gettransaction", "\"1075db55d416d3ca199f55b6084e2115b9345e16c5cf302fc80e9d5fbf5d48d\" true") + HelpExampleRpc("gettransaction", "\"1075db55d416d3ca199f55b6084e2115b9345e16c5cf302fc80e9d5fbf5d48d\""));

    uint256 hash;
    hash.SetHex(params[0].get_str());

    isminefilter filter = ISMINE_SPENDABLE;
    if (params.size() > 1)
        if (params[1].get_bool())
            filter = filter | ISMINE_WATCH_ONLY;

    UniValue entry(UniValue::VOBJ);
    if (!pwalletMain->mapWallet.count(hash))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid or non-wallet transaction id");
    const CWalletTx& wtx = pwalletMain->mapWallet[hash];

    CAmount nCredit = wtx.GetCredit(filter);
    CAmount nDebit = wtx.GetDebit(filter);
    CAmount nNet = nCredit - nDebit;
    CAmount nFee = (wtx.IsFromMe(filter) ? wtx.GetValueOut() - nDebit : 0);

    entry.push_back(Pair("amount", ValueFromAmount(nNet - nFee)));
    if (wtx.IsFromMe(filter))
        entry.push_back(Pair("fee", ValueFromAmount(nFee)));

    WalletTxToJSON(wtx, entry);

    UniValue details(UniValue::VARR);
    ListTransactions(wtx, "*", 0, false, details, filter);
    entry.push_back(Pair("details", details));

    string strHex = EncodeHexTx(static_cast<CTransaction>(wtx));
    entry.push_back(Pair("hex", strHex));

    return entry;
}


UniValue backupwallet(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "backupwallet \"destination\"\n"
            "\nSafely copies wallet.dat to destination, which can be a directory or a path with filename.\n"
            "\nArguments:\n"
            "1. \"destination\"   (string) The destination directory or file\n"
            "\nExamples:\n" +
            HelpExampleCli("backupwallet", "\"backup.dat\"") + HelpExampleRpc("backupwallet", "\"backup.dat\""));

    string strDest = params[0].get_str();
    if (!BackupWallet(*pwalletMain, strDest))
        throw JSONRPCError(RPC_WALLET_ERROR, "Error: Wallet backup failed!");

    return NullUniValue;
}


UniValue keypoolrefill(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "keypoolrefill ( newsize )\n"
            "\nFills the keypool." +
            HelpRequiringPassphrase() + "\n"
                                        "\nArguments\n"
                                        "1. newsize     (numeric, optional, default=100) The new keypool size\n"
                                        "\nExamples:\n" +
            HelpExampleCli("keypoolrefill", "") + HelpExampleRpc("keypoolrefill", ""));

    // 0 is interpreted by TopUpKeyPool() as the default keypool size given by -keypool
    unsigned int kpSize = 0;
    if (params.size() > 0) {
        if (params[0].get_int() < 0)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, expected valid size.");
        kpSize = (unsigned int)params[0].get_int();
    }

    EnsureWalletIsUnlocked();
    pwalletMain->TopUpKeyPool(kpSize);

    if (pwalletMain->GetKeyPoolSize() < kpSize)
        throw JSONRPCError(RPC_WALLET_ERROR, "Error refreshing keypool.");

    return NullUniValue;
}


static void LockWallet(CWallet* pWallet)
{
    LOCK(cs_nWalletUnlockTime);
    nWalletUnlockTime = 0;
    pWallet->fWalletUnlockAnonymizeOnly = false;
    pWallet->Lock();
}

UniValue walletpassphrase(const UniValue& params, bool fHelp)
{
    if (pwalletMain->IsCrypted() && (fHelp || params.size() < 2 || params.size() > 3))
        throw runtime_error(
            "walletpassphrase \"passphrase\" timeout ( anonymizeonly )\n"
            "\nStores the wallet decryption key in memory for 'timeout' seconds.\n"
            "This is needed prior to performing transactions related to private keys such as sending GKCs\n"
            "\nArguments:\n"
            "1. \"passphrase\"     (string, required) The wallet passphrase\n"
            "2. timeout            (numeric, required) The time to keep the decryption key in seconds.\n"
            "3. anonymizeonly      (boolean, optional, default=flase) If is true sending functions are disabled."
            "\nNote:\n"
            "Issuing the walletpassphrase command while the wallet is already unlocked will set a new unlock\n"
            "time that overrides the old one. A timeout of \"0\" unlocks until the wallet is closed.\n"
            "\nExamples:\n"
            "\nUnlock the wallet for 60 seconds\n" +
            HelpExampleCli("walletpassphrase", "\"my pass phrase\" 60") +
            "\nUnlock the wallet for 60 seconds but allow Obfuscation only\n" + HelpExampleCli("walletpassphrase", "\"my pass phrase\" 60 true") +
            "\nLock the wallet again (before 60 seconds)\n" + HelpExampleCli("walletlock", "") +
            "\nAs json rpc call\n" + HelpExampleRpc("walletpassphrase", "\"my pass phrase\", 60"));

    if (fHelp)
        return true;
    if (!pwalletMain->IsCrypted())
        throw JSONRPCError(RPC_WALLET_WRONG_ENC_STATE, "Error: running with an unencrypted wallet, but walletpassphrase was called.");

    // Note that the walletpassphrase is stored in params[0] which is not mlock()ed
    SecureString strWalletPass;
    strWalletPass.reserve(100);
    // TODO: get rid of this .c_str() by implementing SecureString::operator=(std::string)
    // Alternately, find a way to make params[0] mlock()'d to begin with.
    strWalletPass = params[0].get_str().c_str();

    bool anonymizeOnly = false;
    if (params.size() == 3)
        anonymizeOnly = params[2].get_bool();

    if (!pwalletMain->IsLocked() && pwalletMain->fWalletUnlockAnonymizeOnly && anonymizeOnly)
        throw JSONRPCError(RPC_WALLET_ALREADY_UNLOCKED, "Error: Wallet is already unlocked.");

    if (!pwalletMain->Unlock(strWalletPass, anonymizeOnly))
        throw JSONRPCError(RPC_WALLET_PASSPHRASE_INCORRECT, "Error: The wallet passphrase entered was incorrect.");

    pwalletMain->TopUpKeyPool();

    int64_t nSleepTime = params[1].get_int64();
    LOCK(cs_nWalletUnlockTime);
    nWalletUnlockTime = GetTime() + nSleepTime;

    if (nSleepTime > 0) {
        nWalletUnlockTime = GetTime () + nSleepTime;
        RPCRunLater ("lockwallet", boost::bind (LockWallet, pwalletMain), nSleepTime);
    }

    return NullUniValue;
}


UniValue walletpassphrasechange(const UniValue& params, bool fHelp)
{
    if (pwalletMain->IsCrypted() && (fHelp || params.size() != 2))
        throw runtime_error(
            "walletpassphrasechange \"oldpassphrase\" \"newpassphrase\"\n"
            "\nChanges the wallet passphrase from 'oldpassphrase' to 'newpassphrase'.\n"
            "\nArguments:\n"
            "1. \"oldpassphrase\"      (string) The current passphrase\n"
            "2. \"newpassphrase\"      (string) The new passphrase\n"
            "\nExamples:\n" +
            HelpExampleCli("walletpassphrasechange", "\"old one\" \"new one\"") + HelpExampleRpc("walletpassphrasechange", "\"old one\", \"new one\""));

    if (fHelp)
        return true;
    if (!pwalletMain->IsCrypted())
        throw JSONRPCError(RPC_WALLET_WRONG_ENC_STATE, "Error: running with an unencrypted wallet, but walletpassphrasechange was called.");

    // TODO: get rid of these .c_str() calls by implementing SecureString::operator=(std::string)
    // Alternately, find a way to make params[0] mlock()'d to begin with.
    SecureString strOldWalletPass;
    strOldWalletPass.reserve(100);
    strOldWalletPass = params[0].get_str().c_str();

    SecureString strNewWalletPass;
    strNewWalletPass.reserve(100);
    strNewWalletPass = params[1].get_str().c_str();

    if (strOldWalletPass.length() < 1 || strNewWalletPass.length() < 1)
        throw runtime_error(
            "walletpassphrasechange <oldpassphrase> <newpassphrase>\n"
            "Changes the wallet passphrase from <oldpassphrase> to <newpassphrase>.");

    if (!pwalletMain->ChangeWalletPassphrase(strOldWalletPass, strNewWalletPass))
        throw JSONRPCError(RPC_WALLET_PASSPHRASE_INCORRECT, "Error: The wallet passphrase entered was incorrect.");

    return NullUniValue;
}


UniValue walletlock(const UniValue& params, bool fHelp)
{
    if (pwalletMain->IsCrypted() && (fHelp || params.size() != 0))
        throw runtime_error(
            "walletlock\n"
            "\nRemoves the wallet encryption key from memory, locking the wallet.\n"
            "After calling this method, you will need to call walletpassphrase again\n"
            "before being able to call any methods which require the wallet to be unlocked.\n"
            "\nExamples:\n"
            "\nSet the passphrase for 2 minutes to perform a transaction\n" +
            HelpExampleCli("walletpassphrase", "\"my pass phrase\" 120") +
            "\nPerform a send (requires passphrase set)\n" + HelpExampleCli("sendtoaddress", "\"XwnLY9Tf7Zsef8gMGL2fhWA9ZmMjt4KPwg\" 1.0") +
            "\nClear the passphrase since we are done before 2 minutes is up\n" + HelpExampleCli("walletlock", "") +
            "\nAs json rpc call\n" + HelpExampleRpc("walletlock", ""));

    if (fHelp)
        return true;
    if (!pwalletMain->IsCrypted())
        throw JSONRPCError(RPC_WALLET_WRONG_ENC_STATE, "Error: running with an unencrypted wallet, but walletlock was called.");

    {
        LOCK(cs_nWalletUnlockTime);
        pwalletMain->Lock();
        nWalletUnlockTime = 0;
    }

    return NullUniValue;
}


UniValue encryptwallet(const UniValue& params, bool fHelp)
{
    if (!pwalletMain->IsCrypted() && (fHelp || params.size() != 1))
        throw runtime_error(
            "encryptwallet \"passphrase\"\n"
            "\nEncrypts the wallet with 'passphrase'. This is for first time encryption.\n"
            "After this, any calls that interact with private keys such as sending or signing \n"
            "will require the passphrase to be set prior the making these calls.\n"
            "Use the walletpassphrase call for this, and then walletlock call.\n"
            "If the wallet is already encrypted, use the walletpassphrasechange call.\n"
            "Note that this will shutdown the server.\n"
            "\nArguments:\n"
            "1. \"passphrase\"    (string) The pass phrase to encrypt the wallet with. It must be at least 1 character, but should be long.\n"
            "\nExamples:\n"
            "\nEncrypt you wallet\n" +
            HelpExampleCli("encryptwallet", "\"my pass phrase\"") +
            "\nNow set the passphrase to use the wallet, such as for signing or sending GKCs\n" + HelpExampleCli("walletpassphrase", "\"my pass phrase\"") +
            "\nNow we can so something like sign\n" + HelpExampleCli("signmessage", "\"gkcaddress\" \"test message\"") +
            "\nNow lock the wallet again by removing the passphrase\n" + HelpExampleCli("walletlock", "") +
            "\nAs a json rpc call\n" + HelpExampleRpc("encryptwallet", "\"my pass phrase\""));

    if (fHelp)
        return true;
    if (pwalletMain->IsCrypted())
        throw JSONRPCError(RPC_WALLET_WRONG_ENC_STATE, "Error: running with an encrypted wallet, but encryptwallet was called.");

    // TODO: get rid of this .c_str() by implementing SecureString::operator=(std::string)
    // Alternately, find a way to make params[0] mlock()'d to begin with.
    SecureString strWalletPass;
    strWalletPass.reserve(100);
    strWalletPass = params[0].get_str().c_str();

    if (strWalletPass.length() < 1)
        throw runtime_error(
            "encryptwallet <passphrase>\n"
            "Encrypts the wallet with <passphrase>.");

    if (!pwalletMain->EncryptWallet(strWalletPass))
        throw JSONRPCError(RPC_WALLET_ENCRYPTION_FAILED, "Error: Failed to encrypt the wallet.");

    // BDB seems to have a bad habit of writing old data into
    // slack space in .dat files; that is bad if the old data is
    // unencrypted private keys. So:
    StartShutdown();
    return "wallet encrypted; gkc server stopping, restart to run with encrypted wallet. The keypool has been flushed, you need to make a new backup.";
}

UniValue lockunspent(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "lockunspent unlock [{\"txid\":\"txid\",\"vout\":n},...]\n"
            "\nUpdates list of temporarily unspendable outputs.\n"
            "Temporarily lock (unlock=false) or unlock (unlock=true) specified transaction outputs.\n"
            "A locked transaction output will not be chosen by automatic coin selection, when spending GKCs.\n"
            "Locks are stored in memory only. Nodes start with zero locked outputs, and the locked output list\n"
            "is always cleared (by virtue of process exit) when a node stops or fails.\n"
            "Also see the listunspent call\n"
            "\nArguments:\n"
            "1. unlock            (boolean, required) Whether to unlock (true) or lock (false) the specified transactions\n"
            "2. \"transactions\"  (string, required) A json array of objects. Each object the txid (string) vout (numeric)\n"
            "     [           (json array of json objects)\n"
            "       {\n"
            "         \"txid\":\"id\",    (string) The transaction id\n"
            "         \"vout\": n         (numeric) The output number\n"
            "       }\n"
            "       ,...\n"
            "     ]\n"

            "\nResult:\n"
            "true|false    (boolean) Whether the command was successful or not\n"

            "\nExamples:\n"
            "\nList the unspent transactions\n" +
            HelpExampleCli("listunspent", "") +
            "\nLock an unspent transaction\n" + HelpExampleCli("lockunspent", "false \"[{\\\"txid\\\":\\\"a08e6907dbbd3d809776dbfc5d82e371b764ed838b5655e72f463568df1aadf0\\\",\\\"vout\\\":1}]\"") +
            "\nList the locked transactions\n" + HelpExampleCli("listlockunspent", "") +
            "\nUnlock the transaction again\n" + HelpExampleCli("lockunspent", "true \"[{\\\"txid\\\":\\\"a08e6907dbbd3d809776dbfc5d82e371b764ed838b5655e72f463568df1aadf0\\\",\\\"vout\\\":1}]\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("lockunspent", "false, \"[{\\\"txid\\\":\\\"a08e6907dbbd3d809776dbfc5d82e371b764ed838b5655e72f463568df1aadf0\\\",\\\"vout\\\":1}]\""));

    if (params.size() == 1)
        RPCTypeCheck(params, boost::assign::list_of(UniValue::VBOOL));
    else
        RPCTypeCheck(params, boost::assign::list_of(UniValue::VBOOL)(UniValue::VARR));

    bool fUnlock = params[0].get_bool();

    if (params.size() == 1) {
        if (fUnlock)
            pwalletMain->UnlockAllCoins();
        return true;
    }

    UniValue outputs = params[1].get_array();
    for (unsigned int idx = 0; idx < outputs.size(); idx++) {
        const UniValue& output = outputs[idx];
        if (!output.isObject())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, expected object");
        const UniValue& o = output.get_obj();

        RPCTypeCheckObj(o, boost::assign::map_list_of("txid", UniValue::VSTR)("vout", UniValue::VNUM));

        string txid = find_value(o, "txid").get_str();
        if (!IsHex(txid))
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, expected hex txid");

        int nOutput = find_value(o, "vout").get_int();
        if (nOutput < 0)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, vout must be positive");

        COutPoint outpt(uint256(txid), nOutput);

        if (fUnlock)
            pwalletMain->UnlockCoin(outpt);
        else
            pwalletMain->LockCoin(outpt);
    }

    return true;
}

UniValue listlockunspent(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 0)
        throw runtime_error(
            "listlockunspent\n"
            "\nReturns list of temporarily unspendable outputs.\n"
            "See the lockunspent call to lock and unlock transactions for spending.\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"txid\" : \"transactionid\",     (string) The transaction id locked\n"
            "    \"vout\" : n                      (numeric) The vout value\n"
            "  }\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n"
            "\nList the unspent transactions\n" +
            HelpExampleCli("listunspent", "") +
            "\nLock an unspent transaction\n" + HelpExampleCli("lockunspent", "false \"[{\\\"txid\\\":\\\"a08e6907dbbd3d809776dbfc5d82e371b764ed838b5655e72f463568df1aadf0\\\",\\\"vout\\\":1}]\"") +
            "\nList the locked transactions\n" + HelpExampleCli("listlockunspent", "") +
            "\nUnlock the transaction again\n" + HelpExampleCli("lockunspent", "true \"[{\\\"txid\\\":\\\"a08e6907dbbd3d809776dbfc5d82e371b764ed838b5655e72f463568df1aadf0\\\",\\\"vout\\\":1}]\"") +
            "\nAs a json rpc call\n" + HelpExampleRpc("listlockunspent", ""));

    vector<COutPoint> vOutpts;
    pwalletMain->ListLockedCoins(vOutpts);

    UniValue ret(UniValue::VARR);

    BOOST_FOREACH (COutPoint& outpt, vOutpts) {
        UniValue o(UniValue::VOBJ);

        o.push_back(Pair("txid", outpt.hash.GetHex()));
        o.push_back(Pair("vout", (int)outpt.n));
        ret.push_back(o);
    }

    return ret;
}

UniValue settxfee(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 1)
        throw runtime_error(
            "settxfee amount\n"
            "\nSet the transaction fee per kB.\n"
            "\nArguments:\n"
            "1. amount         (numeric, required) The transaction fee in GKC/kB rounded to the nearest 0.00000001\n"
            "\nResult\n"
            "true|false        (boolean) Returns true if successful\n"
            "\nExamples:\n" +
            HelpExampleCli("settxfee", "0.00001") + HelpExampleRpc("settxfee", "0.00001"));

    // Amount
    CAmount nAmount = 0;
    if (params[0].get_real() != 0.0)
        nAmount = AmountFromValue(params[0]); // rejects 0.0 amounts

    payTxFee = CFeeRate(nAmount, 1000);
    return true;
}

UniValue getwalletinfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getwalletinfo\n"
            "Returns an object containing various wallet state info.\n"
            "\nResult:\n"
            "{\n"
            "  \"walletversion\": xxxxx,     (numeric) the wallet version\n"
            "  \"balance\": xxxxxxx,         (numeric) the total GKC balance of the wallet\n"
            "  \"txcount\": xxxxxxx,         (numeric) the total number of transactions in the wallet\n"
            "  \"keypoololdest\": xxxxxx,    (numeric) the timestamp (seconds since GMT epoch) of the oldest pre-generated key in the key pool\n"
            "  \"keypoolsize\": xxxx,        (numeric) how many new keys are pre-generated\n"
            "  \"unlocked_until\": ttt,      (numeric) the timestamp in seconds since epoch (midnight Jan 1 1970 GMT) that the wallet is unlocked for transfers, or 0 if the wallet is locked\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("getwalletinfo", "") + HelpExampleRpc("getwalletinfo", ""));

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("walletversion", pwalletMain->GetVersion()));
    obj.push_back(Pair("balance", ValueFromAmount(pwalletMain->GetBalance())));
    obj.push_back(Pair("txcount", (int)pwalletMain->mapWallet.size()));
    obj.push_back(Pair("keypoololdest", pwalletMain->GetOldestKeyPoolTime()));
    obj.push_back(Pair("keypoolsize", (int)pwalletMain->GetKeyPoolSize()));
    if (pwalletMain->IsCrypted())
        obj.push_back(Pair("unlocked_until", nWalletUnlockTime));
    CKeyID masterKeyID = pwalletMain->GetHDChain().masterKeyID;
    if (!masterKeyID.IsNull())
         obj.push_back(Pair("hdmasterkeyid", masterKeyID.GetHex()));
    return obj;
}

// ppcoin: reserve balance from being staked for network protection
UniValue reservebalance(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 2)
        throw runtime_error(
            "reservebalance ( reserve amount )\n"
            "\nShow or set the reserve amount not participating in network protection\n"
            "If no parameters provided current setting is printed.\n"

            "\nArguments:\n"
            "1. reserve     (boolean, optional) is true or false to turn balance reserve on or off.\n"
            "2. amount      (numeric, optional) is a real and rounded to cent.\n"

            "\nResult:\n"
            "{\n"
            "  \"reserve\": true|false,     (boolean) Status of the reserve balance\n"
            "  \"amount\": x.xxxx       (numeric) Amount reserved\n"
            "\nExamples:\n" +
            HelpExampleCli("reservebalance", "true 5000") + HelpExampleRpc("reservebalance", "true 5000"));

    if (params.size() > 0) {
        bool fReserve = params[0].get_bool();
        if (fReserve) {
            if (params.size() == 1)
                throw runtime_error("must provide amount to reserve balance.\n");
            CAmount nAmount = AmountFromValue(params[1]);
            nAmount = (nAmount / CENT) * CENT; // round to cent
            if (nAmount < 0)
                throw runtime_error("amount cannot be negative.\n");
            nReserveBalance = nAmount;
        } else {
            if (params.size() > 1)
                throw runtime_error("cannot specify amount to turn off reserve.\n");
            nReserveBalance = 0;
        }
    }

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("reserve", (nReserveBalance > 0)));
    result.push_back(Pair("amount", ValueFromAmount(nReserveBalance)));
    return result;
}

// presstab HyperStake
UniValue setstakesplitthreshold(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "setstakesplitthreshold value\n"
            "\nThis will set the output size of your stakes to never be below this number\n"

            "\nArguments:\n"
            "1. value   (numeric, required) Threshold value between 1 and 999999\n"
            "\nResult:\n"
            "{\n"
            "  \"threshold\": n,    (numeric) Threshold value set\n"
            "  \"saved\": true|false    (boolean) 'true' if successfully saved to the wallet file\n"
            "}\n"
            "\nExamples:\n" +
            HelpExampleCli("setstakesplitthreshold", "5000") + HelpExampleRpc("setstakesplitthreshold", "5000"));

    uint64_t nStakeSplitThreshold = params[0].get_int();
    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Unlock wallet to use this feature");
    if (nStakeSplitThreshold > 999999)
        throw runtime_error("Value out of range, max allowed is 999999");

    CWalletDB walletdb(pwalletMain->strWalletFile);
    LOCK(pwalletMain->cs_wallet);
    {
        bool fFileBacked = pwalletMain->fFileBacked;

        UniValue result(UniValue::VOBJ);
        pwalletMain->nStakeSplitThreshold = nStakeSplitThreshold;
        result.push_back(Pair("threshold", int(pwalletMain->nStakeSplitThreshold)));
        if (fFileBacked) {
            walletdb.WriteStakeSplitThreshold(nStakeSplitThreshold);
            result.push_back(Pair("saved", "true"));
        } else
            result.push_back(Pair("saved", "false"));

        return result;
    }
}

// presstab HyperStake
UniValue getstakesplitthreshold(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getstakesplitthreshold\n"
            "Returns the threshold for stake splitting\n"
            "\nResult:\n"
            "n      (numeric) Threshold value\n"
            "\nExamples:\n" +
            HelpExampleCli("getstakesplitthreshold", "") + HelpExampleRpc("getstakesplitthreshold", ""));

    return int(pwalletMain->nStakeSplitThreshold);
}

UniValue autocombinerewards(const UniValue& params, bool fHelp)
{
    bool fEnable;
    if (params.size() >= 1)
        fEnable = params[0].get_bool();

    if (fHelp || params.size() < 1 || (fEnable && params.size() != 2) || params.size() > 2)
        throw runtime_error(
            "autocombinerewards true|false ( threshold )\n"
            "\nWallet will automatically monitor for any coins with value below the threshold amount, and combine them if they reside with the same GKC address\n"
            "When autocombinerewards runs it will create a transaction, and therefore will be subject to transaction fees.\n"

            "\nArguments:\n"
            "1. true|false      (boolean, required) Enable auto combine (true) or disable (false)\n"
            "2. threshold       (numeric, optional) Threshold amount (default: 0)\n"
            "\nExamples:\n" +
            HelpExampleCli("autocombinerewards", "true 500") + HelpExampleRpc("autocombinerewards", "true 500"));

    CWalletDB walletdb(pwalletMain->strWalletFile);
    CAmount nThreshold = 0;

    if (fEnable)
        nThreshold = params[1].get_int();

    pwalletMain->fCombineDust = fEnable;
    pwalletMain->nAutoCombineThreshold = nThreshold;

    if (!walletdb.WriteAutoCombineSettings(fEnable, nThreshold))
        throw runtime_error("Changed settings in wallet but failed to save to database\n");

    return NullUniValue;
}

//  gkc-vm
UniValue createcontract(const UniValue& params, bool fHelp)
{
    int height = chainActive.Tip()->nHeight;

    uint64_t blockGasLimit = GetBlockGasLimit(height);
    uint64_t minGasPrice = CAmount(GetMinGasPrice(height));
    CAmount nGasPrice = (minGasPrice > DEFAULT_GAS_PRICE) ? minGasPrice : DEFAULT_GAS_PRICE;
    
    if (fHelp || params.size() < 1 || params.size() > 6)
        throw std::runtime_error(
                "createcontract \"bytecode\" (gaslimit gasprice \"senderaddress\" broadcast)"
                        "\nCreate a contract with bytcode.\n"
                + HelpRequiringPassphrase() +
                "\nArguments:\n"
                        "1. \"bytecode\"  (string, required) contract bytcode.\n"
                        "2. gasLimit  (numeric or string, optional) gasLimit, default: " +
                i64tostr(DEFAULT_GAS_LIMIT_OP_CREATE) + ", max: " + i64tostr(blockGasLimit) + "\n"
                        "3. gasPrice  (numeric or string, optional) gasPrice GKC price per gas unit, default: " +
                FormatMoney(nGasPrice) + ", min:" + FormatMoney(minGasPrice) + "\n"
                        "4. \"senderaddress\" (string, optional) The gkc address that will be used to create the contract.\n"
                        "5. \"broadcast\" (bool, optional, default=true) Whether to broadcast the transaction or not.\n"
                        "6. \"changeToSender\" (bool, optional, default=true) Return the change to the sender.\n"
                        "\nResult:\n"
                        "[\n"
                        "  {\n"
                        "    \"txid\" : (string) The transaction id.\n"
                        "    \"sender\" : (string) GKC address of the sender.\n"
                        "    \"hash160\" : (string) ripemd-160 hash of the sender.\n"
                        "    \"address\" : (string) expected contract address.\n"
                        "  }\n"
                        "]\n"
                        "\nExamples:\n"
                + HelpExampleCli("createcontract",
                                 "\"60606040525b33600060006101000a81548173ffffffffffffffffffffffffffffffffffffffff02191690836c010000000000000000000000009081020402179055506103786001600050819055505b600c80605b6000396000f360606040526008565b600256\"")
                + HelpExampleCli("createcontract",
                                 "\"60606040525b33600060006101000a81548173ffffffffffffffffffffffffffffffffffffffff02191690836c010000000000000000000000009081020402179055506103786001600050819055505b600c80605b6000396000f360606040526008565b600256\" 6000000 " +
                                 FormatMoney(minGasPrice) + " \"QM72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\" true")
        );

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Unlock wallet to use this feature");

    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);

    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    LOCK(pwalletMain->cs_wallet);

    std::string bytecode = params[0].get_str();

    if (!IsHex(bytecode))
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid data (data not hex)");
    }

    uint64_t nGasLimit = DEFAULT_GAS_LIMIT_OP_CREATE;
    if (params.size() > 1)
    {
        if (params[1].isNum())
        {
            nGasLimit = params[1].get_int64();
        } else if (params[1].isStr())
        {
            nGasLimit = atoi64(params[1].get_str());
        } else
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "JSON value for gasLimit is not (numeric or string)");
        }

        if (nGasLimit > blockGasLimit)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasLimit (Maximum is: " + i64tostr(blockGasLimit) + ")");
        if (nGasLimit < MINIMUM_GAS_LIMIT)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasLimit (Minimum is: " + i64tostr(MINIMUM_GAS_LIMIT) + ")");
        if (nGasLimit <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasLimit");
    }

    if (params.size() > 2)
    {
        if (params[2].isNum())
        {
            nGasPrice = params[2].get_real() * COIN;
        } else if (params[2].isStr())
        {
            double retval;
            if (!ParseDouble(params[2].get_str(), &retval))
                throw std::runtime_error("JSON double out of range");

            nGasPrice = retval * COIN;
        } else
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "JSON value for gasPrice is not (numeric or string)");
        }

        CAmount maxRpcGasPrice = GetArg("-rpcmaxgasprice", MAX_RPC_GAS_PRICE);
        if (nGasPrice > (int64_t)maxRpcGasPrice)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasPrice, Maximum allowed in RPC calls is: " +
                                               FormatMoney(maxRpcGasPrice) + " (use -rpcmaxgasprice to change it)");
        if (nGasPrice < (int64_t)minGasPrice)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasPrice (Minimum is: " + FormatMoney(minGasPrice) + ")");
        if (nGasPrice <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasPrice");
    }

    bool fHasSender = false;
    CBitcoinAddress senderAddress;
    if (params.size() > 3)
    {
        senderAddress.SetString(params[3].get_str());
        if (!senderAddress.IsValid())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid gkc address to send from");
        else
            fHasSender = true;
    }
    bool fBroadcast = true;
    if (params.size() > 4)
    {
        fBroadcast = params[4].get_bool();
    }

    bool fChangeToSender = true;
    if (params.size() > 5)
    {
        fChangeToSender = params[5].get_bool();
    }

    CCoinControl coinControl;

    if (fHasSender)
    {
        //find a UTXO with sender address

        UniValue results(UniValue::VARR);
        std::vector<COutput> vecOutputs;

        coinControl.fAllowOtherInputs = true;

        assert(pwalletMain != NULL);
        pwalletMain->AvailableCoins(vecOutputs, true, NULL, false);

        //        BOOST_FOREACH(const COutput& out, vecOutputs) {
        for (const COutput &out : vecOutputs)
        {
            CTxDestination address;
            const CScript &scriptPubKey = out.tx->vout[out.i].scriptPubKey;
            bool fValidAddress = ExtractDestination(scriptPubKey, address);

            CBitcoinAddress destAdress(address);

            if (!fValidAddress || senderAddress.Get() != destAdress.Get())
                continue;

            coinControl.Select(COutPoint(out.tx->GetHash(), out.i));

            break;

        }

        if (!coinControl.HasSelected())
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "Sender address does not have any unspent outputs");
        }
        if (fChangeToSender)
        {
            coinControl.destChange = senderAddress.Get();
        }
    }

    CWalletTx wtx;

    wtx.nTimeSmart = GetAdjustedTime();

    CAmount nGasFee = nGasPrice * nGasLimit;

    CAmount curBalance = pwalletMain->GetBalance();

    // Check amount
    if (nGasFee <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");

    if (nGasFee > curBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

    // Build OP_EXEC script
    CScript scriptPubKey = CScript() << CScriptNum(VersionVM::GetEVMDefault().toRaw()) << CScriptNum(nGasLimit)
                                     << CScriptNum(nGasPrice) << ParseHex(bytecode) << OP_CREATE;

    // Create and send the transaction
    CReserveKey reservekey(pwalletMain);
    CAmount nFeeRequired;
    std::string strError;
    std::vector<pair<CScript, CAmount>> vecSend;
    int nChangePosRet = -1;

    vecSend.push_back(std::make_pair(scriptPubKey, 0));

    if (!pwalletMain->CreateTransaction(vecSend, wtx, reservekey, nFeeRequired, strError, &coinControl, ALL_COINS, false, 0, nGasFee, fHasSender))
    {
        if (nFeeRequired > pwalletMain->GetBalance())
            strError = strprintf(
                    "Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!",
                    FormatMoney(nFeeRequired));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }

    CTxDestination txSenderDest;
    ExtractDestination(pwalletMain->mapWallet[wtx.vin[0].prevout.hash].vout[wtx.vin[0].prevout.n].scriptPubKey,
                       txSenderDest);

    if (fHasSender && !(senderAddress.Get() == txSenderDest))
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender could not be set, transaction was not committed!");
    }

    UniValue result(UniValue::VOBJ);
    CValidationState state;
    if (fBroadcast)
    {
        //////////////////////////////////////////////////////////// //gkc-vm
        // check contract tx
        if (!CheckTransaction(wtx, height >= Params().Zerocoin_StartHeight(), false, state)) // state filled in by CheckTransaction
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "Sender could not be set, transaction was not committed!");
        }

        
        CAmount nMinGasPrice = 0;
        CAmount nValueIn = 0;
        CAmount nValueOut = 0;
        CAmount nFees = 0;

        {
            CCoinsView dummy;
            CCoinsViewCache view(&dummy);

            LOCK(mempool.cs);
            CCoinsViewMemPool viewMemPool(pcoinsTip, mempool);
            view.SetBackend(viewMemPool);

            nMinGasPrice = 0;
            nValueIn = view.GetValueIn(wtx);
            nValueOut = wtx.GetValueOut();
            nFees = nValueIn - nValueOut;

            if (!wtx.CheckSenderScript(view))
            {
                throw JSONRPCError(RPC_TYPE_ERROR, "bad-txns-invalid-sender-script");
            }
        }

        int level = 0;
        string errinfo;

        if (!CheckContractTx(wtx, nFees, nMinGasPrice, level, errinfo))
        {
            throw JSONRPCError(RPC_TYPE_ERROR, errinfo);
        }
        ////////////////////////////////////////////////////////////

        if (!pwalletMain->CommitTransaction(wtx, reservekey, "smart-contract"))
            throw JSONRPCError(RPC_WALLET_ERROR,
                               "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of the wallet and coins were spent in the copy but not marked as spent here.");

        std::string txId = wtx.GetHash().GetHex();
        result.push_back(Pair("txid", txId));

        CBitcoinAddress txSenderAdress(txSenderDest);
        CKeyID keyid;
        txSenderAdress.GetKeyID(keyid);

        result.push_back(Pair("sender", txSenderAdress.ToString()));
        result.push_back(Pair("hash160", HexStr(valtype(keyid.begin(), keyid.end()))));

        std::vector<unsigned char> SHA256TxVout(32);
        std::vector<unsigned char> contractAddress(20);
        std::vector<unsigned char> txIdAndVout(wtx.GetHash().begin(), wtx.GetHash().end());
        uint32_t voutNumber = 0;
        //        BOOST_FOREACH(const CTxOut& txout, wtx.tx->vout) {
        for (const CTxOut &txout : wtx.vout)
        {
            if (txout.scriptPubKey.HasOpCreate())
            {
                std::vector<unsigned char> voutNumberChrs;
                if (voutNumberChrs.size() < sizeof(voutNumber))
                    voutNumberChrs.resize(sizeof(voutNumber));
                std::memcpy(voutNumberChrs.data(), &voutNumber, sizeof(voutNumber));
                txIdAndVout.insert(txIdAndVout.end(), voutNumberChrs.begin(), voutNumberChrs.end());
                break;
            }
            voutNumber++;
        }
        CSHA256().Write(txIdAndVout.data(), txIdAndVout.size()).Finalize(SHA256TxVout.data());
        CRIPEMD160().Write(SHA256TxVout.data(), SHA256TxVout.size()).Finalize(contractAddress.data());
        result.push_back(Pair("address", HexStr(contractAddress)));
    } else
    {
        std::string strHex = EncodeHexTx(wtx);
        result.push_back(Pair("raw transaction", strHex));
    }
    if (!mempool.exists(wtx.GetHash()))
    {
        result.push_back(Pair("state", strprintf("Warning: Transaction cannot be broadcast immediately! %s",
                                                 state.GetRejectReason())));
    }

    return result;
}

//  gkc-vm
UniValue sendtocontract(const UniValue& params, bool fHelp)
{
    int height = chainActive.Tip()->nHeight;

    LogPrintf("Sendto Params: %s",params.write(2));

    uint64_t blockGasLimit = GetBlockGasLimit(height);
    uint64_t minGasPrice = CAmount(GetMinGasPrice(height));
    CAmount nGasPrice = (minGasPrice > DEFAULT_GAS_PRICE) ? minGasPrice : DEFAULT_GAS_PRICE;

    if (fHelp || params.size() < 2 || params.size() > 8)
        throw std::runtime_error(
                "sendtocontract \"contractaddress\" \"data\" (amount gaslimit gasprice senderaddress broadcast)"
                        "\nSend funds and data to a contract.\n"
                + HelpRequiringPassphrase() +
                "\nArguments:\n"
                        "1. \"contractaddress\" (string, required) The contract address that will receive the funds and data.\n"
                        "2. \"datahex\"  (string, required) data to send.\n"
                        "3. \"amount\"      (numeric or string, optional) The amount in GKC to send. eg 0.1, default: 0\n"
                        "4. gasLimit  (numeric or string, optional) gasLimit, default: " +
                i64tostr(DEFAULT_GAS_LIMIT_OP_SEND) + ", max: " + i64tostr(blockGasLimit) + "\n"
                        "5. gasPrice  (numeric or string, optional) gasPrice Tsr price per gas unit, default: " +
                FormatMoney(nGasPrice) + ", min:" + FormatMoney(minGasPrice) + "\n"
                        "6. \"senderaddress\" (string, optional) The gkc address that will be used as sender.\n"
                        "7. \"broadcast\" (bool, optional, default=true) Whether to broadcast the transaction or not.\n"
                        "8. \"changeToSender\" (bool, optional, default=true) Return the change to the sender.\n"
                        "\nResult:\n"
                        "[\n"
                        "  {\n"
                        "    \"txid\" : (string) The transaction id.\n"
                        "    \"sender\" : (string) GKC address of the sender.\n"
                        "    \"hash160\" : (string) ripemd-160 hash of the sender.\n"
                        "  }\n"
                        "]\n"
                        "\nExamples:\n"
                + HelpExampleCli("sendtocontract", "\"c6ca2697719d00446d4ea51f6fac8fd1e9310214\" \"54f6127f\"")
                + HelpExampleCli("sendtocontract",
                                 "\"c6ca2697719d00446d4ea51f6fac8fd1e9310214\" \"54f6127f\" 12.0015 6000000 " +
                                 FormatMoney(minGasPrice) + " \"QM72Sfpbz1BPpXFHz9m3CdqATR44Jvaydd\"")
        );

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Unlock wallet to use this feature");

    bool IsEnabled = (chainActive.Tip()->nVersion > ZEROCOIN_VERSION);

    if (!IsEnabled)
        throw JSONRPCError(RPC_INTERNAL_ERROR, "not arrive to the contract height,disabled");

    LOCK2(cs_main, pwalletMain->cs_wallet);

    std::string contractaddress = params[0].get_str();
    if (contractaddress.size() != 40 || !IsHex(contractaddress))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Incorrect contract address");

    if (!AddressInUse(contractaddress))
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "contract address does not exist");

    std::string datahex = params[1].get_str();
    if (!IsHex(datahex))
        throw JSONRPCError(RPC_TYPE_ERROR, "Invalid data (data not hex)");

    CAmount nAmount = 0;
    if (params.size() > 2)
    {
        nAmount = AmountFromValue(params[2]);
        if (nAmount < 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid amount for send");
    }

    uint64_t nGasLimit = DEFAULT_GAS_LIMIT_OP_SEND;
    if (params.size() > 3)
    {
        if (params[3].isNum())
        {
            nGasLimit = params[3].get_int64();
        } else if (params[3].isStr())
        {
            nGasLimit = atoi64(params[3].get_str());
        } else
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "JSON value for gasLimit is not (numeric or string)");
        }

        if (nGasLimit > blockGasLimit)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasLimit (Maximum is: " + i64tostr(blockGasLimit) + ")");
        if (nGasLimit < MINIMUM_GAS_LIMIT)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasLimit (Minimum is: " + i64tostr(MINIMUM_GAS_LIMIT) + ")");
        if (nGasLimit <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasLimit");
    }

    if (params.size() > 4)
    {
        if (params[4].isNum())
        {
            nGasPrice = params[4].get_real() * COIN;
        } else if (params[4].isStr())
        {
            double retval;
            if (!ParseDouble(params[4].get_str(), &retval))
                throw std::runtime_error("JSON double out of range");

            nGasPrice = retval * COIN;
        } else
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "JSON value for gasPrice is not (numeric or string)");
        }

        CAmount maxRpcGasPrice = GetArg("-rpcmaxgasprice", MAX_RPC_GAS_PRICE);
        if (nGasPrice > (int64_t)maxRpcGasPrice)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasPrice, Maximum allowed in RPC calls is: " +
                                               FormatMoney(maxRpcGasPrice) + " (use -rpcmaxgasprice to change it)");
        if (nGasPrice < (int64_t)minGasPrice)
            throw JSONRPCError(RPC_TYPE_ERROR,
                               "Invalid value for gasPrice (Minimum is: " + FormatMoney(minGasPrice) + ")");
        if (nGasPrice <= 0)
            throw JSONRPCError(RPC_TYPE_ERROR, "Invalid value for gasPrice");
    }

    bool fHasSender = false;
    CBitcoinAddress senderAddress;
    if (params.size() > 5)
    {
        senderAddress.SetString(params[5].get_str());
        if (!senderAddress.IsGKC())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Gkc address to send from");
        else
            fHasSender = true;
    }

    bool fBroadcast = true;
    if (params.size() > 6)
    {
        fBroadcast = params[6].get_bool();
    }

    bool fChangeToSender = true;
    if (params.size() > 7)
    {
        fChangeToSender = params[7].get_bool();
    }

    CCoinControl coinControl;

    if (!fHasSender)
    {
        std::vector<COutput> vecOutputs;
        assert(pwalletMain != NULL);
        pwalletMain->AvailableCoins(vecOutputs, true, NULL, false);
        for (const COutput &out : vecOutputs)
        {
            CTxDestination address;
            const CScript &scriptPubKey = out.tx->vout[out.i].scriptPubKey;
            if(!ExtractDestination(scriptPubKey, address))
				continue;
			senderAddress = CBitcoinAddress(address);
            break;
        }
    }
	if (fChangeToSender)
	{
		coinControl.destChange = senderAddress.Get();
	}

    CWalletTx wtx;

    wtx.nTimeSmart = GetAdjustedTime();

    CAmount nGasFee = nGasPrice * nGasLimit;

    CAmount curBalance = pwalletMain->GetBalance();

    // Check amount
    if (nGasFee <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount for gas fee");

    if (nAmount + nGasFee > curBalance)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

	CKeyID senderKeyid;
	CPubKey senderPubKey;
	if(!senderAddress.GetKeyID(senderKeyid) ||	!pwalletMain->GetPubKey(senderKeyid, senderPubKey))
		throw JSONRPCError(RPC_INVALID_PARAMETER, "Unable to obtain the public key of the sender's address.");	
    // Build OP_EXEC_ASSIGN script
    CScript script = CScript() 
	    << CScriptNum(VersionVM::GetEVMDefault().toRaw()) << CScriptNum(nGasLimit)
		<< CScriptNum(nGasPrice) << ParseHex(datahex) << ParseHex(contractaddress)
		<< valtype(senderPubKey.begin(),senderPubKey.end()) << OP_CALL;
	// VERY CRITICAL !!! The new script in CreateTransaction() will replace the script here

    // Create and send the transaction
    CReserveKey reservekey(pwalletMain);
    CAmount nFeeRequired;
    std::string strError;
    std::vector<std::pair<CScript, CAmount>> vecSend;
    int nChangePosRet = -1;

    vecSend.push_back(std::make_pair(script, nAmount));

    if (!pwalletMain->CreateTransaction(vecSend, wtx, reservekey, nFeeRequired, strError, &coinControl, ALL_COINS, false, 0, nGasFee, fHasSender))
    {
        if (nFeeRequired > pwalletMain->GetBalance())
            strError = strprintf(
                    "Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!",
                    FormatMoney(nFeeRequired));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }

    UniValue result(UniValue::VOBJ);

    CValidationState state;
    if (fBroadcast)
    {
        //////////////////////////////////////////////////////////// //gkc-vm
        // check contract tx
        if (!CheckTransaction(wtx, height >= Params().Zerocoin_StartHeight(), false, state)) // state filled in by CheckTransaction
        {
            throw JSONRPCError(RPC_TYPE_ERROR, "Contract tx check failed, transaction was not committed!");
        }

        CAmount nMinGasPrice = 0;
        CAmount nValueIn = 0;
        CAmount nValueOut = 0;
        CAmount nFees = 0;
        
        {
            CCoinsView dummy;
            CCoinsViewCache view(&dummy);

            LOCK(mempool.cs);
            CCoinsViewMemPool viewMemPool(pcoinsTip, mempool);
            view.SetBackend(viewMemPool);

            nMinGasPrice = 0;
            nValueIn = view.GetValueIn(wtx);
            nValueOut = wtx.GetValueOut();
            nFees = nValueIn - nValueOut;

            if (!wtx.CheckSenderScript(view))
            {
                throw JSONRPCError(RPC_TYPE_ERROR, "bad-txns-invalid-sender-script");
            }
        }

        int level = 0;
        string errinfo;

        if (!CheckContractTx(wtx, nFees, nMinGasPrice, level, errinfo))
        {
            throw JSONRPCError(RPC_TYPE_ERROR, errinfo);
        }
        ////////////////////////////////////////////////////////////

        if (!pwalletMain->CommitTransaction(wtx, reservekey, "smart-contract"))
            throw JSONRPCError(RPC_WALLET_ERROR,
                               "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of the wallet and coins were spent in the copy but not marked as spent here.");

        std::string txId = wtx.GetHash().GetHex();
        result.push_back(Pair("txid", txId));
        result.push_back(Pair("sender", senderAddress.ToString()));
        result.push_back(Pair("hash160", HexStr(valtype(senderKeyid.begin(), senderKeyid.end()))));
    } else
    {
        std::string strHex = EncodeHexTx(wtx);
        result.push_back(Pair("raw transaction", strHex));
    }
    if (!mempool.exists(wtx.GetHash()))
    {
        result.push_back(Pair("state", strprintf("Warning: Transaction cannot be broadcast immediately! %s",
                                                 state.GetRejectReason())));
    }

    return result;
}

static bool parsehexdata(std::string strData, std::vector<uint8_t>& vecData, size_t Bytes)
{
    bool    bSuccess    = false;

    std::vector<unsigned char>  vData;

    if (strData.size() == 2 * Bytes && IsHex(strData)) {
        bSuccess = true;
        vData = ParseHex(strData);
        vecData.insert(vecData.end(), vData.begin(), vData.end());
    } else if (strData.size() == (2 + 2 * Bytes) && (0 == strData.find("0x") || 0 == strData.find("0X")) && 
               IsHex(strData.substr(2)))
    {
        bSuccess = true;
        vData = ParseHex(strData.substr(2));
        vecData.insert(vecData.end(), vData.begin(), vData.end());
    }

    return bSuccess;
}

UniValue sendextenddata(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "sendextenddata [{\"key\":\"name\",\"value\":\"data\"},...] senderaddress\n"
            "Create a transaction with extend data spending the given addresses.\n"
            "\nResult:\n"
            "\"hex\"             (string) The transaction hash in hex\n"
            "\nExamples:\n"
            "\nCreate and send a transaction with extend data\n" +
            HelpExampleCli("sendextenddata", "\"[{\\\"key\\\" : \\\"name1\\\",\\\"type\\\":type1,\\\"value\\\":\"data1\"}]\" \"senderaddress\"\n"));

    if (!params[0].isArray())
        throw runtime_error("extend data is not array\n");

    UniValue jsonData = params[0].get_array();

    uint8_t     u8Temp;
    uint8_t     u8Count;
    
    uint32_t    u32Size;

    std::vector<unsigned char>  bindata;

    CBitcoinAddress senderAddress;

    //  bin data size and count:
    bindata.push_back(0);
    bindata.push_back(0);
    bindata.push_back(0);

    u8Count = 0;
    for (unsigned int index = 0; index < jsonData.size(); index++)
    {
        uint256  u256Name;

        std::string strName;
        std::string strData;
        std::string strType;

        std::vector<uint8_t> vecData;

        eExtendDataType eType;

        const UniValue& keyvalue = jsonData[index];

        const UniValue& key = find_value(keyvalue, "key");
        const UniValue& type = find_value(keyvalue, "type");
        const UniValue& value = find_value(keyvalue, "value");

        if ((key.isStr() || key.isNum()) && 
            (type.isNum() && (0 <= type.get_int()) && (type.get_int() <= UCHAR_MAX)) &&
            (value.isStr() || value.isNum()))
        {
            if (key.isStr())
                strName = key.get_str();
            else
                strName = key.write();

            if (0 == strName.size())
                throw runtime_error("key is null\n");
            else if (strName.size() > 32)
                throw runtime_error("key is too long(Max 32 bytes)\n");

            int s32Type = type.get_int();
            if (s32Type < EXT_DATA_STRING || s32Type >= EXT_DATA_RESERVED)
               throw runtime_error("invalid value type\n");

            eType = eExtendDataType(s32Type);
            switch (eType) {
                case EXT_DATA_STRING:
                    if (value.isStr()) {
                        strData = value.get_str();
                        vecData.insert(vecData.end(), strData.begin(), strData.end());
                    } else {
                        throw runtime_error("invalid value of string\n");
                    }
                    break;
                
                case EXT_DATA_DOUBLE:
                    if (value.isNum()) {
                        strData = value.write();
                        vecData.insert(vecData.end(), strData.begin(), strData.end());
                    } else {
                        throw runtime_error("invalid value of double\n");
                    }
                    break;

                
                case EXT_DATA_BOOL:
                    if (value.isBool()) {
                        if (value.get_bool())
                            vecData.push_back(1);
                        else
                            vecData.push_back(0);
                    } else if (value.isNum()) {
                        if (value.get_int())
                            vecData.push_back(1);
                        else
                            vecData.push_back(0);
                    }else {
                        throw runtime_error("invalid value of bool\n");
                    }
                    break;

                case EXT_DATA_INT8:
                case EXT_DATA_UINT8:
                    strType = (EXT_DATA_INT8 == eType) ? "INT8" : "UINT8";

                    if (value.isNum()) {
                        if ((EXT_DATA_INT8 == eType && SCHAR_MIN <= value.get_int() && value.get_int() <= CHAR_MAX) ||
                            (EXT_DATA_UINT8 == eType && 0 <= value.get_int() && value.get_int() <= UCHAR_MAX)) {
                            vecData.push_back(value.get_int());
                        } else {
                            throw runtime_error("Exceeding the range of " + strType + "\n");
                        }
                    } else if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int8_t)))
                            throw runtime_error("invalid value of "+ strType + "\n");
                    } else {
                        throw runtime_error("invalid value of "+ strType + "\n");
                    }
                    break;

                case EXT_DATA_INT16:
                case EXT_DATA_UINT16:
                    strType = (EXT_DATA_INT16 == eType) ? "INT16" : "UINT16";

                    if (value.isNum()) {
                        if ((EXT_DATA_INT16 == eType && SHRT_MIN <= value.get_int() && value.get_int() <= SHRT_MAX) ||
                            (EXT_DATA_UINT16 == eType && 0 <= value.get_int() && value.get_int() <= USHRT_MAX)) {
                            uint16_t u16Value = value.get_int();
                            
                            vecData.push_back(u16Value >> 8);
                            vecData.push_back(u16Value % 256);
                        } else {
                            throw runtime_error("Exceeding the range of "+ strType + "\n");
                        }
                    } else if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int16_t))) {
                            throw runtime_error("invalid value of "+ strType + "\n");
                        }
                    } else {
                        throw runtime_error("invalid value of "+ strType + "\n");
                    }
                    break;
                
                case EXT_DATA_INT32:
                case EXT_DATA_UINT32:
                    strType = (EXT_DATA_INT32 == eType) ? "INT32" : "UINT32";

                    if (value.isNum()) {
                        if ((EXT_DATA_INT32 == eType && INT_MIN <= value.get_int() && value.get_int() <= INT_MAX) ||
                            (EXT_DATA_UINT32 == eType && 0 <= value.get_int64() && value.get_int64() <= UINT_MAX)) {
                            uint64_t u64Value = value.get_int64();
                            
                            vecData.push_back((u64Value >> 24) & 0xFF);
                            vecData.push_back((u64Value >> 16) & 0xFF);
                            vecData.push_back((u64Value >>  8) & 0xFF);
                            vecData.push_back((u64Value >>  0) & 0xFF);
                        } else {
                            throw runtime_error("Exceeding the range of "+ strType + "\n");
                        }
                    } else if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int32_t))) {
                            throw runtime_error("invalid value of "+ strType + "\n");
                        }
                    } else {
                        throw runtime_error("invalid value of "+ strType + "\n");
                    }
                    break;

                case EXT_DATA_INT64:
                case EXT_DATA_UINT64:
                    strType = (EXT_DATA_INT64 == eType) ? "INT64" : "UINT64";

                    if (value.isNum()) {
                        if ((EXT_DATA_INT64 == eType && LONG_MIN <= value.get_int64() && value.get_int64() <= LONG_MAX) || 
                            (EXT_DATA_UINT64 == eType && 0 <= value.get_int64() && value.get_int64() <= ULONG_MAX)) {
                            uint64_t u64Value = value.get_int64();
                            
                            vecData.push_back((u64Value >> 56) & 0xFF);
                            vecData.push_back((u64Value >> 48) & 0xFF);
                            vecData.push_back((u64Value >> 40) & 0xFF);
                            vecData.push_back((u64Value >> 32) & 0xFF);
                            vecData.push_back((u64Value >> 24) & 0xFF);
                            vecData.push_back((u64Value >> 16) & 0xFF);
                            vecData.push_back((u64Value >>  8) & 0xFF);
                            vecData.push_back((u64Value >>  0) & 0xFF);
                        }
                    } else if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int64_t))) {
                            throw runtime_error("invalid value of "+ strType + "\n");
                        }
                    } else {
                        throw runtime_error("invalid value of "+ strType + "\n");
                    }
                    break;

                case EXT_DATA_INT128:
                case EXT_DATA_UINT128:
                    strType = (EXT_DATA_INT128 == eType) ? "INT128" : "UINT128";

                    if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int64_t))) {
                            throw runtime_error("invalid value of "+ strType + ", only supports hex strings\n");
                        }
                    } else {
                        throw runtime_error("invalid value of "+ strType + ", only supports hex strings\n");
                    }
                    break;

                case EXT_DATA_INT256:
                case EXT_DATA_UINT256:
                    strType = (EXT_DATA_INT64 == eType) ? "INT256" : "UINT256";

                    if (value.isStr()) {
                        strData = value.get_str();
                        if (!parsehexdata(strData, vecData, sizeof(int64_t))) {
                            throw runtime_error("invalid value of "+ strType + ", only supports hex strings\n");
                        }
                    } else {
                        throw runtime_error("invalid value of "+ strType + ", only supports hex strings\n");
                    }
                    break;
            }
            
            if (0 == vecData.size())
                throw runtime_error("value is null\n");
        }
        else
        {
            if (key.isNull())
                throw runtime_error("key is null\n");
            else if (!(key.isStr() || key.isNum()))
                throw runtime_error("key is not string or number\n");
            else if (!type.isNum())
                throw runtime_error("value type is not number\n");
            else if (type.get_int() < 0 || type.get_int() > UCHAR_MAX)
                throw runtime_error("invalid value type\n");
            else if (value.isNull())
                throw runtime_error("value is null\n");
            else if (!(value.isStr() || value.isNum()))
                throw runtime_error("value is not string or number\n");
            else
                throw runtime_error("unknown key/value error\n");
        }

        if (strName.size() && strName.size() <= u256Name.size() && vecData.size() && vecData.size() <= USHRT_MAX)
        {
            u32Size = 3;
            u256Name.SetNull();
            
            memcpy(u256Name.begin(), strName.data(), strName.size());
            u32Size += u256Name.size();
            u32Size += vecData.size();
            
            u8Temp = (u32Size >> 8) & 0xFF;
            bindata.push_back(u8Temp);
            u8Temp = (u32Size & 0xFF);
            bindata.push_back(u8Temp);

            bindata.push_back(eType);

            bindata.insert(bindata.end(), u256Name.begin(), u256Name.end());
            bindata.insert(bindata.end(), vecData.begin(), vecData.end());
            u8Count++;
        }
        else
        {
            if (!strName.size())
                throw runtime_error("min key size is 1\n");
            else if (strName.size() > u256Name.size())
                throw runtime_error("max key size is " + std::to_string(u256Name.size()) + "\n");
            else if (!vecData.size() || vecData.size() > USHRT_MAX)
                throw runtime_error("key " + strName + " data size is " + std::to_string(vecData.size()) + "\n");
            else
                throw runtime_error("unknown name/data error\n");
        }
    }

    if (u8Count != jsonData.size())
        throw runtime_error("name/data pair error\n");

    if(bindata.size() > USHRT_MAX)
        throw runtime_error("Length of extend data(" + std::to_string(bindata.size()) + ") exceeds the limit:" + std::to_string(USHRT_MAX) + "\n");

    u32Size = bindata.size();

    bindata[0] = (u32Size >> 8) & 0xFF;
    bindata[1] = (u32Size & 0xFF);
    bindata[2] = u8Count;

    senderAddress.SetString(params[1].get_str());
    if (!senderAddress.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid gkc address to send from");

    CCoinControl coinControl;

    UniValue results(UniValue::VARR);
    std::vector<COutput> vecOutputs;
    
    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Unlock wallet to use this feature");

    coinControl.fAllowOtherInputs = true;
    assert(pwalletMain != NULL);
    pwalletMain->AvailableCoins(vecOutputs, true, NULL, false);
    
    for (const COutput &out : vecOutputs)
    {
        CTxDestination address;
        const CScript &scriptPubKey = out.tx->vout[out.i].scriptPubKey;
        bool fValidAddress = ExtractDestination(scriptPubKey, address);
    
        CBitcoinAddress destAdress(address);
    
        if (!fValidAddress || senderAddress.Get() != destAdress.Get())
            continue;
    
        coinControl.Select(COutPoint(out.tx->GetHash(), out.i));
    
        break;
    }
    
    if (!coinControl.HasSelected())
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender address does not have any unspent outputs");
    }
    coinControl.destChange = senderAddress.Get();

    CWalletTx wtx;

    wtx.nTimeSmart = GetAdjustedTime();

    int height = chainActive.Tip()->nHeight;

    CAmount curBalance = pwalletMain->GetBalance();
    if (curBalance <= 0)
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

    CScript  script;

    {
        if (bindata.size() < OP_PUSHDATA1) {
            script.insert(script.end(), (unsigned char)bindata.size());
        } else if (bindata.size() <= 0xff) {
            script.insert(script.end(), OP_PUSHDATA1);
            script.insert(script.end(), (unsigned char)bindata.size());
        } else if (bindata.size() <= 0xffff) {
            script.insert(script.end(), OP_PUSHDATA2);
            unsigned short nSize = bindata.size();
            script.insert(script.end(), (unsigned char*)&nSize, (unsigned char*)&nSize + sizeof(nSize));
        } else {
            throw runtime_error("Length of extend data(" + std::to_string(bindata.size()) + ") exceeds the limit:" + std::to_string(USHRT_MAX) + "\n");
        }
        script.insert(script.end(), bindata.begin(), bindata.end());
    }
    script << OP_EXT_DATA;

    // Create and send the transaction
    CReserveKey reservekey(pwalletMain);
    CAmount nFeeRequired;
    std::string strError;
    std::vector<std::pair<CScript, CAmount>> vecSend;
    int nChangePosRet = -1;

    vecSend.push_back(std::make_pair(script, 0));

    if (!pwalletMain->CreateTransaction(vecSend, wtx, reservekey, nFeeRequired, strError, &coinControl, ALL_COINS, false, 0, 0, true))
    {
        if (nFeeRequired > pwalletMain->GetBalance())
            strError = strprintf(
                    "Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!",
                    FormatMoney(nFeeRequired));
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }

    CTxDestination txSenderDest;
    ExtractDestination(pwalletMain->mapWallet[wtx.vin[0].prevout.hash].vout[wtx.vin[0].prevout.n].scriptPubKey, txSenderDest);

    if (!(senderAddress.Get() == txSenderDest))
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender could not be set, transaction was not committed!");
    }

    CValidationState state;

    if (!CheckTransaction(wtx, height >= Params().Zerocoin_StartHeight(), false, state)) // state filled in by CheckTransaction
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "Sender could not be set, transaction was not committed!");
    }
    
    CCoinsView dummy;
    CCoinsViewCache view(&dummy);

    LOCK(mempool.cs);
    CCoinsViewMemPool viewMemPool(pcoinsTip, mempool);
    view.SetBackend(viewMemPool);
    
    if (!wtx.CheckSenderScript(view))
    {
        throw JSONRPCError(RPC_TYPE_ERROR, "bad-txns-invalid-sender-script");
    }

    if (!pwalletMain->CommitTransaction(wtx, reservekey, "sendextenddata"))
        throw JSONRPCError(RPC_WALLET_ERROR,
                           "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of the wallet and coins were spent in the copy but not marked as spent here.");

    return wtx.GetHash().GetHex();
}

UniValue getextenddata(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3)
        throw runtime_error(
            "getextenddata height key\n"
            "\nReturns value of key in best-block-chain at height provided.\n"
            "\nResult:\n"
            "\"value\"          (string) The value of key in string\n"
            "\nExamples:\n"
            "\nGet extended value of key from a transaction with extend data\n" +
            HelpExampleCli("getextenddata", "1000 \"key\" \"SenderAddress\"\n"));

    bool                hasSender;

    int32_t             height;

    std::string         strKey;
    std::string         strValue;

    dev::Address        Sender;
    
    eExtendDataType     eType;

    if (!params[0].isNum())
        throw runtime_error("height is not number\n");

    if (!params[1].isStr())
        throw runtime_error("key is not string\n");

    hasSender = false;
    if (params.size() > 2) {
        hasSender = true;
        if (!params[2].isStr())
            throw runtime_error("address is not string\n");

        CKeyID keyID;
        CBitcoinAddress txSender;

        txSender.SetString(params[2].get_str());
        
        if (!txSender.IsValid()) {
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Gkc address");
        }
        txSender.GetKeyID(keyID);
        memcpy(Sender.data(), keyID.begin(), dev::Address::size);
        std::cout << "keyid " << keyID.ToString() << std::endl;
    }

    height = params[0].get_int();
    if (height < 0 || height > chainActive.Height())
        return strValue;

    strKey = params[1].get_str();

    std::vector<uint8_t> value;
    if (getExtendedData(height, strKey, eType, value, Sender)) {
        strValue = std::string((const char *)value.data(), value.size());
    }

    return strValue;
}


UniValue printMultiSend()
{
    UniValue ret(UniValue::VARR);
    UniValue act(UniValue::VOBJ);
    act.push_back(Pair("MultiSendStake Activated?", pwalletMain->fMultiSendStake));
    act.push_back(Pair("MultiSendMasternode Activated?", pwalletMain->fMultiSendMasternodeReward));
    ret.push_back(act);

    if (pwalletMain->vDisabledAddresses.size() >= 1) {
        UniValue disAdd(UniValue::VOBJ);
        for (unsigned int i = 0; i < pwalletMain->vDisabledAddresses.size(); i++) {
            disAdd.push_back(Pair("Disabled From Sending", pwalletMain->vDisabledAddresses[i]));
        }
        ret.push_back(disAdd);
    }

    ret.push_back("MultiSend Addresses to Send To:");

    UniValue vMS(UniValue::VOBJ);
    for (unsigned int i = 0; i < pwalletMain->vMultiSend.size(); i++) {
        vMS.push_back(Pair("Address " + boost::lexical_cast<std::string>(i), pwalletMain->vMultiSend[i].first));
        vMS.push_back(Pair("Percent", pwalletMain->vMultiSend[i].second));
    }

    ret.push_back(vMS);
    return ret;
}

UniValue printAddresses()
{
    std::vector<COutput> vCoins;
    pwalletMain->AvailableCoins(vCoins);
    std::map<std::string, double> mapAddresses;
    BOOST_FOREACH (const COutput& out, vCoins) {
        CTxDestination utxoAddress;
        ExtractDestination(out.tx->vout[out.i].scriptPubKey, utxoAddress);
        std::string strAdd = CBitcoinAddress(utxoAddress).ToString();

        if (mapAddresses.find(strAdd) == mapAddresses.end()) //if strAdd is not already part of the map
            mapAddresses[strAdd] = (double)out.tx->vout[out.i].nValue / (double)COIN;
        else
            mapAddresses[strAdd] += (double)out.tx->vout[out.i].nValue / (double)COIN;
    }

    UniValue ret(UniValue::VARR);
    for (map<std::string, double>::const_iterator it = mapAddresses.begin(); it != mapAddresses.end(); ++it) {
        UniValue obj(UniValue::VOBJ);
        const std::string* strAdd = &(*it).first;
        const double* nBalance = &(*it).second;
        obj.push_back(Pair("Address ", *strAdd));
        obj.push_back(Pair("Balance ", *nBalance));
        ret.push_back(obj);
    }

    return ret;
}

unsigned int sumMultiSend()
{
    unsigned int sum = 0;
    for (unsigned int i = 0; i < pwalletMain->vMultiSend.size(); i++)
        sum += pwalletMain->vMultiSend[i].second;
    return sum;
}

UniValue multisend(const UniValue& params, bool fHelp)
{
    CWalletDB walletdb(pwalletMain->strWalletFile);
    bool fFileBacked;
    //MultiSend Commands
    if (params.size() == 1) {
        string strCommand = params[0].get_str();
        UniValue ret(UniValue::VOBJ);
        if (strCommand == "print") {
            return printMultiSend();
        } else if (strCommand == "printaddress" || strCommand == "printaddresses") {
            return printAddresses();
        } else if (strCommand == "clear") {
            LOCK(pwalletMain->cs_wallet);
            {
                bool erased = false;
                if (pwalletMain->fFileBacked) {
                    if (walletdb.EraseMultiSend(pwalletMain->vMultiSend))
                        erased = true;
                }

                pwalletMain->vMultiSend.clear();
                pwalletMain->setMultiSendDisabled();

                UniValue obj(UniValue::VOBJ);
                obj.push_back(Pair("Erased from database", erased));
                obj.push_back(Pair("Erased from RAM", true));

                return obj;
            }
        } else if (strCommand == "enablestake" || strCommand == "activatestake") {
            if (pwalletMain->vMultiSend.size() < 1)
                throw JSONRPCError(RPC_INVALID_REQUEST, "Unable to activate MultiSend, check MultiSend vector");

            if (CBitcoinAddress(pwalletMain->vMultiSend[0].first).IsValid()) {
                pwalletMain->fMultiSendStake = true;
                if (!walletdb.WriteMSettings(true, pwalletMain->fMultiSendMasternodeReward, pwalletMain->nLastMultiSendHeight)) {
                    UniValue obj(UniValue::VOBJ);
                    obj.push_back(Pair("error", "MultiSend activated but writing settings to DB failed"));
                    UniValue arr(UniValue::VARR);
                    arr.push_back(obj);
                    arr.push_back(printMultiSend());
                    return arr;
                } else
                    return printMultiSend();
            }

            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unable to activate MultiSend, check MultiSend vector");
        } else if (strCommand == "enablemasternode" || strCommand == "activatemasternode") {
            if (pwalletMain->vMultiSend.size() < 1)
                throw JSONRPCError(RPC_INVALID_REQUEST, "Unable to activate MultiSend, check MultiSend vector");

            if (CBitcoinAddress(pwalletMain->vMultiSend[0].first).IsValid()) {
                pwalletMain->fMultiSendMasternodeReward = true;

                if (!walletdb.WriteMSettings(pwalletMain->fMultiSendStake, true, pwalletMain->nLastMultiSendHeight)) {
                    UniValue obj(UniValue::VOBJ);
                    obj.push_back(Pair("error", "MultiSend activated but writing settings to DB failed"));
                    UniValue arr(UniValue::VARR);
                    arr.push_back(obj);
                    arr.push_back(printMultiSend());
                    return arr;
                } else
                    return printMultiSend();
            }

            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Unable to activate MultiSend, check MultiSend vector");
        } else if (strCommand == "disable" || strCommand == "deactivate") {
            pwalletMain->setMultiSendDisabled();
            if (!walletdb.WriteMSettings(false, false, pwalletMain->nLastMultiSendHeight))
                throw JSONRPCError(RPC_DATABASE_ERROR, "MultiSend deactivated but writing settings to DB failed");

            return printMultiSend();
        } else if (strCommand == "enableall") {
            if (!walletdb.EraseMSDisabledAddresses(pwalletMain->vDisabledAddresses))
                return "failed to clear old vector from walletDB";
            else {
                pwalletMain->vDisabledAddresses.clear();
                return printMultiSend();
            }
        }
    }
    if (params.size() == 2 && params[0].get_str() == "delete") {
        int del = boost::lexical_cast<int>(params[1].get_str());
        if (!walletdb.EraseMultiSend(pwalletMain->vMultiSend))
            throw JSONRPCError(RPC_DATABASE_ERROR, "failed to delete old MultiSend vector from database");

        pwalletMain->vMultiSend.erase(pwalletMain->vMultiSend.begin() + del);
        if (!walletdb.WriteMultiSend(pwalletMain->vMultiSend))
            throw JSONRPCError(RPC_DATABASE_ERROR, "walletdb WriteMultiSend failed!");

        return printMultiSend();
    }
    if (params.size() == 2 && params[0].get_str() == "disable") {
        std::string disAddress = params[1].get_str();
        if (!CBitcoinAddress(disAddress).IsValid())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "address you want to disable is not valid");
        else {
            pwalletMain->vDisabledAddresses.push_back(disAddress);
            if (!walletdb.EraseMSDisabledAddresses(pwalletMain->vDisabledAddresses))
                throw JSONRPCError(RPC_DATABASE_ERROR, "disabled address from sending, but failed to clear old vector from walletDB");

            if (!walletdb.WriteMSDisabledAddresses(pwalletMain->vDisabledAddresses))
                throw JSONRPCError(RPC_DATABASE_ERROR, "disabled address from sending, but failed to store it to walletDB");
            else
                return printMultiSend();
        }
    }

    //if no commands are used
    if (fHelp || params.size() != 2)
        throw runtime_error(
            "multisend <command>\n"
            "****************************************************************\n"
            "WHAT IS MULTISEND?\n"
            "MultiSend allows a user to automatically send a percent of their stake reward to as many addresses as you would like\n"
            "The MultiSend transaction is sent when the staked coins mature (100 confirmations)\n"
            "****************************************************************\n"
            "TO CREATE OR ADD TO THE MULTISEND VECTOR:\n"
            "multisend <GKC Address> <percent>\n"
            "This will add a new address to the MultiSend vector\n"
            "Percent is a whole number 1 to 100.\n"
            "****************************************************************\n"
            "MULTISEND COMMANDS (usage: multisend <command>)\n"
            " print - displays the current MultiSend vector \n"
            " clear - deletes the current MultiSend vector \n"
            " enablestake/activatestake - activates the current MultiSend vector to be activated on stake rewards\n"
            " enablemasternode/activatemasternode - activates the current MultiSend vector to be activated on masternode rewards\n"
            " disable/deactivate - disables the current MultiSend vector \n"
            " delete <Address #> - deletes an address from the MultiSend vector \n"
            " disable <address> - prevents a specific address from sending MultiSend transactions\n"
            " enableall - enables all addresses to be eligible to send MultiSend transactions\n"
            "****************************************************************\n");

    //if the user is entering a new MultiSend item
    string strAddress = params[0].get_str();
    CBitcoinAddress address(strAddress);
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");
    if (boost::lexical_cast<int>(params[1].get_str()) < 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, expected valid percentage");
    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");
    unsigned int nPercent = boost::lexical_cast<unsigned int>(params[1].get_str());

    LOCK(pwalletMain->cs_wallet);
    {
        fFileBacked = pwalletMain->fFileBacked;
        //Error if 0 is entered
        if (nPercent == 0) {
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Sending 0% of stake is not valid");
        }

        //MultiSend can only send 100% of your stake
        if (nPercent + sumMultiSend() > 100)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Failed to add to MultiSend vector, the sum of your MultiSend is greater than 100%");

        for (unsigned int i = 0; i < pwalletMain->vMultiSend.size(); i++) {
            if (pwalletMain->vMultiSend[i].first == strAddress)
                throw JSONRPCError(RPC_INVALID_PARAMETER, "Failed to add to MultiSend vector, cannot use the same address twice");
        }

        if (fFileBacked)
            walletdb.EraseMultiSend(pwalletMain->vMultiSend);

        std::pair<std::string, int> newMultiSend;
        newMultiSend.first = strAddress;
        newMultiSend.second = nPercent;
        pwalletMain->vMultiSend.push_back(newMultiSend);
        if (fFileBacked) {
            if (!walletdb.WriteMultiSend(pwalletMain->vMultiSend))
                throw JSONRPCError(RPC_DATABASE_ERROR, "walletdb WriteMultiSend failed!");
        }
    }
    return printMultiSend();
}
UniValue getzerocoinbalance(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() != 0)
        throw runtime_error(
                            "getzerocoinbalance\n"
                            + HelpRequiringPassphrase());

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    return ValueFromAmount(pwalletMain->GetZerocoinBalance(true));

}
UniValue listmintedzerocoins(const UniValue& params, bool fHelp)
{
    
    if (fHelp || params.size() != 0)
        throw runtime_error(
                            "listmintedzerocoins\n"
                            + HelpRequiringPassphrase());
    
    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");
    
    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CZerocoinMint> listPubCoin = walletdb.ListMintedCoins(true, false, true);
    
    UniValue jsonList(UniValue::VARR);
    for (const CZerocoinMint& pubCoinItem : listPubCoin) {
        jsonList.push_back(pubCoinItem.GetValue().GetHex());
    }
    
    return jsonList;
}

UniValue listzerocoinamounts(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() != 0)
        throw runtime_error(
            "listzerocoinamounts\n"
            + HelpRequiringPassphrase());

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CZerocoinMint> listPubCoin = walletdb.ListMintedCoins(true, true, true);
 
    std::map<libzerocoin::CoinDenomination, CAmount> spread;
    for (const auto& denom : libzerocoin::zerocoinDenomList)
        spread.insert(std::pair<libzerocoin::CoinDenomination, CAmount>(denom, 0));
    for (auto& mint : listPubCoin) spread.at(mint.GetDenomination())++;


    UniValue jsonList(UniValue::VARR);
    UniValue val(UniValue::VOBJ);
    for (const auto& m : libzerocoin::zerocoinDenomList) {
        stringstream s1;
        s1 << "Denomination Value " << libzerocoin::ZerocoinDenominationToInt(m);
        stringstream s2;
        s2 << spread.at(m) << " coins";
        val.push_back(Pair(s1.str(),s2.str()));
    }
    jsonList.push_back(val);
    return jsonList;
}
UniValue listspentzerocoins(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() != 0)
        throw runtime_error(
            "listspentzerocoins\n"
            + HelpRequiringPassphrase());

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CBigNum> listPubCoin = walletdb.ListSpentCoinsSerial();

    UniValue jsonList(UniValue::VARR);
    for (const CBigNum& pubCoinItem : listPubCoin) {
        jsonList.push_back(pubCoinItem.GetHex());
    }

    return jsonList;
}

UniValue mintzerocoin(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "mintzerocoin <amount>\n"
            "Usage: Enter an amount of Tsr to convert to zTsr"
            + HelpRequiringPassphrase());

	throw JSONRPCError(RPC_WALLET_ERROR, "zerocoin is disabled.");

    int64_t nTime = GetTimeMillis();

    if(GetAdjustedTime() > GetSporkValue(SPORK_16_ZEROCOIN_MAINTENANCE_MODE))
        throw JSONRPCError(RPC_WALLET_ERROR, "zGKC is currently disabled due to maintenance.");

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    CAmount nAmount = params[0].get_int() * COIN;

    CWalletTx wtx;
    vector<CZerocoinMint> vMints;
    string strError = pwalletMain->MintZerocoin(nAmount, wtx, vMints);

    if (strError != "")
        throw JSONRPCError(RPC_WALLET_ERROR, strError);

    UniValue arrMints(UniValue::VARR);
    for (CZerocoinMint mint : vMints) {
        UniValue m(UniValue::VOBJ);
        m.push_back(Pair("txid", wtx.GetHash().ToString()));
        m.push_back(Pair("value", ValueFromAmount(libzerocoin::ZerocoinDenominationToAmount(mint.GetDenomination()))));
        m.push_back(Pair("pubcoin", mint.GetValue().GetHex()));
        m.push_back(Pair("randomness", mint.GetRandomness().GetHex()));
        m.push_back(Pair("serial", mint.GetSerialNumber().GetHex()));
        m.push_back(Pair("time", GetTimeMillis() - nTime));
        arrMints.push_back(m);
    }
    
    return arrMints;
}

UniValue spendzerocoin(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 5 || params.size() < 4)
        throw runtime_error(
            "spendzerocoin <amount> <mintchange [true|false]> <minimizechange [true|false]>  <securitylevel [1-100]> <address>\n"
            "Overview: Convert zGKC (zerocoins) into GKC. \n"
            "amount: amount to spend\n"
            "mintchange: if there is left over GKC (change), the wallet can convert it automatically back to zerocoins [true]\n"
            "minimizechange: try to minimize the returning change  [false]\n"
            "security level: the amount of checkpoints to add to the accumulator. A checkpoint contains 10 blocks worth of zerocoinmints."
                    "The more checkpoints that are added, the more untraceable the transaction will be. Use [100] to add the maximum amount"
                    "of checkpoints available. Tip: adding more checkpoints makes the minting process take longer\n"
            "address: Send straight to an address or leave the address blank and the wallet will send to a change address. If there is change then"
                    "an address is required"
            + HelpRequiringPassphrase());

	
	throw JSONRPCError(RPC_WALLET_ERROR, "zerocoin is disabled.");

    if(GetAdjustedTime() > GetSporkValue(SPORK_16_ZEROCOIN_MAINTENANCE_MODE))
        throw JSONRPCError(RPC_WALLET_ERROR, "zGKC is currently disabled due to maintenance.");

    int64_t nTimeStart = GetTimeMillis();
    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    CAmount nAmount = AmountFromValue(params[0]);   // Spending amount
    bool fMintChange = params[1].get_bool();        // Mint change to zGKC
    bool fMinimizeChange = params[2].get_bool();    // Minimize change
    int nSecurityLevel = params[3].get_int();       // Security level

    CBitcoinAddress address = CBitcoinAddress(); // Optional sending address. Dummy initialization here.
    if (params.size() == 5) {
        // Destination address was supplied as params[4]. Optional parameters MUST be at the end
        // to avoid type confusion from the JSON interpreter
        address = CBitcoinAddress(params[4].get_str());
        if(!address.IsValid())
            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid GKC address");
    }

    CWalletTx wtx;
    vector<CZerocoinMint> vMintsSelected;
    CZerocoinSpendReceipt receipt;
    bool fSuccess;

    if(params.size() == 5) // Spend to supplied destination address
        fSuccess = pwalletMain->SpendZerocoin(nAmount, nSecurityLevel, wtx, receipt, vMintsSelected, fMintChange, fMinimizeChange, &address);
    else                   // Spend to newly generated local address
        fSuccess = pwalletMain->SpendZerocoin(nAmount, nSecurityLevel, wtx, receipt, vMintsSelected, fMintChange, fMinimizeChange);

    if (!fSuccess)
        throw JSONRPCError(RPC_WALLET_ERROR, receipt.GetStatusMessage());

    CAmount nValueIn = 0;
    UniValue arrSpends(UniValue::VARR);
    for (CZerocoinSpend spend : receipt.GetSpends()) {
        UniValue obj(UniValue::VOBJ);
        obj.push_back(Pair("denomination", spend.GetDenomination()));
        obj.push_back(Pair("pubcoin", spend.GetPubCoin().GetHex()));
        obj.push_back(Pair("serial", spend.GetSerial().GetHex()));
        uint32_t nChecksum = spend.GetAccumulatorChecksum();
        obj.push_back(Pair("acc_checksum", HexStr(BEGIN(nChecksum), END(nChecksum))));
        arrSpends.push_back(obj);
        nValueIn += libzerocoin::ZerocoinDenominationToAmount(spend.GetDenomination());
    }

    CAmount nValueOut = 0;
    UniValue vout(UniValue::VARR);
    for (unsigned int i = 0; i < wtx.vout.size(); i++) {
        const CTxOut& txout = wtx.vout[i];
        UniValue out(UniValue::VOBJ);
        out.push_back(Pair("value", ValueFromAmount(txout.nValue)));
        nValueOut += txout.nValue;

        CTxDestination dest;
        if(txout.scriptPubKey.IsZerocoinMint())
            out.push_back(Pair("address", "zerocoinmint"));
        else if(ExtractDestination(txout.scriptPubKey, dest))
            out.push_back(Pair("address", CBitcoinAddress(dest).ToString()));
        vout.push_back(out);
    }

    //construct JSON to return
    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("txid", wtx.GetHash().ToString()));
    ret.push_back(Pair("bytes", (int64_t)wtx.GetSerializeSize(SER_NETWORK, CTransaction::CURRENT_VERSION)));
    ret.push_back(Pair("fee", ValueFromAmount(nValueIn - nValueOut)));
    ret.push_back(Pair("duration_millis", (GetTimeMillis() - nTimeStart)));
    ret.push_back(Pair("spends", arrSpends));
    ret.push_back(Pair("outputs", vout));

    return ret;
}

UniValue resetmintzerocoin(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "resetmintzerocoin <extended_search>\n"
            "Scan the blockchain for all of the zerocoins that are held in the wallet.dat. Update any meta-data that is incorrect.\n"
            "Archive any mints that are not able to be found."

            "\nArguments:\n"
            "1. \"extended_search\"      (bool, optional) Rescan each block of the blockchain looking for your mints. WARNING - may take 30+ minutes!\n"

            + HelpRequiringPassphrase());

    bool fExtendedSearch = false;
    if (params.size() == 1)
        fExtendedSearch = params[0].get_bool();

    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CZerocoinMint> listMints = walletdb.ListMintedCoins(false, false, true);
    vector<CZerocoinMint> vMintsToFind{ std::make_move_iterator(std::begin(listMints)), std::make_move_iterator(std::end(listMints)) };
    vector<CZerocoinMint> vMintsMissing;
    vector<CZerocoinMint> vMintsToUpdate;

    // search all of our available data for these mints
    FindMints(vMintsToFind, vMintsToUpdate, vMintsMissing, fExtendedSearch);

    // update the meta data of mints that were marked for updating
    UniValue arrUpdated(UniValue::VARR);
    for (CZerocoinMint mint : vMintsToUpdate) {
        walletdb.WriteZerocoinMint(mint);
        arrUpdated.push_back(mint.GetValue().GetHex());
    }

    // delete any mints that were unable to be located on the blockchain
    UniValue arrDeleted(UniValue::VARR);
    for (CZerocoinMint mint : vMintsMissing) {
        arrDeleted.push_back(mint.GetValue().GetHex());
        walletdb.ArchiveMintOrphan(mint);
    }

    UniValue obj(UniValue::VOBJ);
    obj.push_back(Pair("updated", arrUpdated));
    obj.push_back(Pair("archived", arrDeleted));
    return obj;
}

UniValue resetspentzerocoin(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "resetspentzerocoin\n"
                "Scan the blockchain for all of the zerocoins that are held in the wallet.dat. Reset mints that are considered spent that did not make it into the blockchain."
            + HelpRequiringPassphrase());

    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CZerocoinMint> listMints = walletdb.ListMintedCoins(false, false, false);
    list<CZerocoinSpend> listSpends = walletdb.ListSpentCoins();
    list<CZerocoinSpend> listUnconfirmedSpends;

    for (CZerocoinSpend spend : listSpends) {
        CTransaction tx;
        uint256 hashBlock = 0;
        if (!GetTransaction(spend.GetTxHash(), tx, hashBlock)) {
            listUnconfirmedSpends.push_back(spend);
            continue;
        }

        //no confirmations
        if (hashBlock == 0)
            listUnconfirmedSpends.push_back(spend);
    }

    UniValue objRet(UniValue::VOBJ);
    UniValue arrRestored(UniValue::VARR);
    for (CZerocoinSpend spend : listUnconfirmedSpends) {
        for (CZerocoinMint mint : listMints) {
            if (mint.GetSerialNumber() == spend.GetSerial()) {
                mint.SetUsed(false);
                walletdb.WriteZerocoinMint(mint);
                walletdb.EraseZerocoinSpendSerialEntry(spend.GetSerial());
                RemoveSerialFromDB(spend.GetSerial());
                UniValue obj(UniValue::VOBJ);
                obj.push_back(Pair("serial", spend.GetSerial().GetHex()));
                arrRestored.push_back(obj);
                continue;
            }
        }
    }

    objRet.push_back(Pair("restored", arrRestored));
    return objRet;
}

UniValue getarchivedzerocoin(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() != 0)
        throw runtime_error(
            "getarchivedzerocoin\n"
            "Display zerocoins that were archived because they were believed to be orphans."
            "Provides enough information to recover mint if it was incorrectly archived."
            + HelpRequiringPassphrase());

    CWalletDB walletdb(pwalletMain->strWalletFile);
    list<CZerocoinMint> listMints = walletdb.ListArchivedZerocoins();

    UniValue arrRet(UniValue::VARR);
    for (const CZerocoinMint mint : listMints) {
        UniValue objMint(UniValue::VOBJ);
        objMint.push_back(Pair("txid", mint.GetTxHash().GetHex()));
        objMint.push_back(Pair("denomination", FormatMoney(mint.GetDenominationAsAmount())));
        objMint.push_back(Pair("serial", mint.GetSerialNumber().GetHex()));
        objMint.push_back(Pair("randomness", mint.GetRandomness().GetHex()));
        objMint.push_back(Pair("pubcoin", mint.GetValue().GetHex()));
        arrRet.push_back(objMint);
    }

    return arrRet;
}

UniValue exportzerocoins(const UniValue& params, bool fHelp)
{
    if(fHelp || params.empty() || params.size() > 2)
        throw runtime_error(
            "exportzerocoins include_spent ( denomination )\n"
                "Exports zerocoin mints that are held by this wallet.dat\n"

                "\nArguments:\n"
                "1. \"include_spent\"        (bool, required) Include mints that have already been spent\n"
                "2. \"denomination\"         (integer, optional) Export a specific denomination of zTsr\n"

                "\nResult\n"
                "[                   (array of json object)\n"
                "  {\n"
                "    \"d\" : n,        (numeric) the mint's zerocoin denomination \n"
                "    \"p\" : \"pubcoin\", (string) The public coin\n"
                "    \"s\" : \"serial\",  (string) The secret serial number\n"
                "    \"r\" : \"random\",  (string) The secret random number\n"
                "    \"t\" : \"txid\",    (string) The txid that the coin was minted in\n"
                "    \"h\" : n,         (numeric) The height the tx was added to the blockchain\n"
                "    \"u\" : used       (boolean) Whether the mint has been spent\n"
                "  }\n"
                "  ,...\n"
                "]\n"

                "\nExamples\n" +
            HelpExampleCli("exportzerocoins", "false 5") + HelpExampleRpc("exportzerocoins", "false 5"));

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    CWalletDB walletdb(pwalletMain->strWalletFile);

    bool fIncludeSpent = params[0].get_bool();
    libzerocoin::CoinDenomination denomination = libzerocoin::ZQ_ERROR;
    if (params.size() == 2)
        denomination = libzerocoin::IntToZerocoinDenomination(params[1].get_int());
    list<CZerocoinMint> listMints = walletdb.ListMintedCoins(!fIncludeSpent, false, false);

    UniValue jsonList(UniValue::VARR);
    for (const CZerocoinMint mint : listMints) {
        if (denomination != libzerocoin::ZQ_ERROR && denomination != mint.GetDenomination())
            continue;

        UniValue objMint(UniValue::VOBJ);
        objMint.push_back(Pair("d", mint.GetDenomination()));
        objMint.push_back(Pair("p", mint.GetValue().GetHex()));
        objMint.push_back(Pair("s", mint.GetSerialNumber().GetHex()));
        objMint.push_back(Pair("r", mint.GetRandomness().GetHex()));
        objMint.push_back(Pair("t", mint.GetTxHash().GetHex()));
        objMint.push_back(Pair("h", mint.GetHeight()));
        objMint.push_back(Pair("u", mint.IsUsed()));
        jsonList.push_back(objMint);
    }

    return jsonList;
}

UniValue importzerocoins(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() == 0)
        throw runtime_error(
            "importzerocoins importdata \n"
                "[{\"d\":denomination,\"p\":\"pubcoin_hex\",\"s\":\"serial_hex\",\"r\":\"randomness_hex\",\"t\":\"txid\",\"h\":height, \"u\":used},{\"d\":...}]\n"
                "\nImport zerocoin mints.\n"
                "Adds raw zerocoin mints to the wallet.dat\n"
                "Note it is recommended to use the json export created from the exportzerocoins RPC call\n"

                "\nArguments:\n"
                "1. \"importdata\"    (string, required) A json array of json objects containing zerocoin mints\n"

                "\nResult:\n"
                "\"added\"            (int) the quantity of zerocoin mints that were added\n"
                "\"value\"            (string) the total zTsr value of zerocoin mints that were added\n"

                "\nExamples\n" +
            HelpExampleCli("importzerocoins", "\'[{\"d\":100,\"p\":\"mypubcoin\",\"s\":\"myserial\",\"r\":\"randomness_hex\",\"t\":\"mytxid\",\"h\":104923, \"u\":false},{\"d\":5,...}]\'") +
                HelpExampleRpc("importzerocoins", "[{\"d\":100,\"p\":\"mypubcoin\",\"s\":\"myserial\",\"r\":\"randomness_hex\",\"t\":\"mytxid\",\"h\":104923, \"u\":false},{\"d\":5,...}]"));

    LOCK2(cs_main, pwalletMain->cs_wallet);

    EnsureWalletIsUnlocked();

    RPCTypeCheck(params, list_of(UniValue::VARR)(UniValue::VOBJ));
    UniValue arrMints = params[0].get_array();
    CWalletDB walletdb(pwalletMain->strWalletFile);

    int count = 0;
    CAmount nValue = 0;
    for (unsigned int idx = 0; idx < arrMints.size(); idx++) {
        const UniValue &val = arrMints[idx];
        const UniValue &o = val.get_obj();

        const UniValue& vDenom = find_value(o, "d");
        if (!vDenom.isNum())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, missing d key");
        int d = vDenom.get_int();
        if (d < 0)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, d must be positive");

        libzerocoin::CoinDenomination denom = libzerocoin::IntToZerocoinDenomination(d);
        CBigNum bnValue = 0;
        bnValue.SetHex(find_value(o, "p").get_str());
        CBigNum bnSerial = 0;
        bnSerial.SetHex(find_value(o, "s").get_str());
        CBigNum bnRandom = 0;
        bnRandom.SetHex(find_value(o, "r").get_str());
        uint256 txid(find_value(o, "t").get_str());

        int nHeight = find_value(o, "h").get_int();
        if (nHeight < 0)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, h must be positive");

        bool fUsed = find_value(o, "u").get_bool();
        CZerocoinMint mint(denom, bnValue, bnRandom, bnSerial, fUsed);
        mint.SetTxHash(txid);
        mint.SetHeight(nHeight);
        walletdb.WriteZerocoinMint(mint);
        count++;
        nValue += libzerocoin::ZerocoinDenominationToAmount(denom);
    }

    UniValue ret(UniValue::VOBJ);
    ret.push_back(Pair("added", count));
    ret.push_back(Pair("value", FormatMoney(nValue)));
    return ret;
}

UniValue reconsiderzerocoins(const UniValue& params, bool fHelp)
{
    if(fHelp || !params.empty())
        throw runtime_error(
            "reconsiderzerocoins\n"
                "\nCheck archived zTsr list to see if any mints were added to the blockchain.\n"

                "\nResult\n"
                "[                                 (array of json objects)\n"
                "  {\n"
                "    \"txid\" : txid,              (numeric) the mint's zerocoin denomination \n"
                "    \"denomination\" : \"denom\", (numeric) the mint's zerocoin denomination\n"
                "    \"pubcoin\" : \"pubcoin\",    (string) The mint's public identifier\n"
                "    \"height\" : n,               (numeric) The height the tx was added to the blockchain\n"
                "  }\n"
                "  ,...\n"
                "]\n"

                "\nExamples\n" +
            HelpExampleCli("reconsiderzerocoins", "") + HelpExampleRpc("reconsiderzerocoins", ""));

    if(pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED,
                           "Error: Please enter the wallet passphrase with walletpassphrase first.");

    list<CZerocoinMint> listMints;
    pwalletMain->ReconsiderZerocoins(listMints);

    UniValue arrRet(UniValue::VARR);
    for (const CZerocoinMint mint : listMints) {
        UniValue objMint(UniValue::VOBJ);
        objMint.push_back(Pair("txid", mint.GetTxHash().GetHex()));
        objMint.push_back(Pair("denomination", FormatMoney(mint.GetDenominationAsAmount())));
        objMint.push_back(Pair("pubcoin", mint.GetValue().GetHex()));
        objMint.push_back(Pair("height", mint.GetHeight()));
        arrRet.push_back(objMint);
    }

    return arrRet;
}

#ifdef DPOS

bool IsValidDposName(const std::string& str)
{
	if(str.empty() || str.length() > 64) //limit for length is defined in app
		return false;

	return true;
}

//CreateEntrustAgentID
UniValue createentrustagent(const UniValue& params, bool fHelp)
{
    if(fHelp)
        throw runtime_error(
		"createentrustagent (name) \n"
			"create a dpos agent.\n"
		
			"\nArguments:\n"
			"1. \"name\"	  (string, optional) dpos name that will be write in the transaction\n"
		
			"\nResult:\n"
			"\"agentid\"	(string) this transaction hash or agentid\n"
		
			"\nExamples\n" +
            HelpExampleCli("createentrustagent", "") + HelpExampleRpc("createentrustagent", ""));
    CWalletTx wtx;
	wtx.SetCreateAgent();

    if (params.size() > 0 && !params[0].isNull() && !params[0].get_str().empty()){
		if(!IsValidDposName(params[0].get_str())){
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Name of Dpos isn't valid.");
		};
        wtx.mapValue["comment"] = params[0].get_str();
    }

    EnsureWalletIsUnlocked();

    SendMoney(GetAccountAddress("").Get(), Entrustment::GetInstance().GetCreateAgentAmount(chainActive.Height()+1), wtx);

    return wtx.GetHash().GetHex();
}
UniValue listmyagentid(const UniValue& params, bool fHelp)
{
    if(fHelp)
        throw runtime_error(
            "listmyagentid\n"
            "\nList all of my agentid.\n");

	UniValue txidArray(UniValue::VARR);
	for (const CWalletTx* tx : pwalletMain->GetCreateAgentTx())
		txidArray.push_back(tx->GetHash().ToString());

	return txidArray;
}
UniValue resignagent(const UniValue& params, bool fHelp)
{
    if(fHelp)
        throw runtime_error(
            "resignagent\n"
            "\nResign my identity of agent.\n");

    CWalletTx wtxNew;
	CReserveKey reservekey(pwalletMain);
	std::string strError;
	if (!pwalletMain->CreateResignAgentTx(wtxNew, reservekey, strError))
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, "tx"))
		throw JSONRPCError(RPC_WALLET_ERROR, "CWallet::CommitTransaction() failed in resignagent().");

    return wtxNew.GetHash().GetHex();
}
UniValue entrust(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() < 2 || 4 < params.size())
        throw runtime_error(
            "entrust agentid amount\n"
            "\nEntrust an amount to a given agent.\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"agentid\"  (string, required) The agentid to entrust to.\n"
            "2. \"amount\"   (numeric, required) The amount in gkc to entrust. eg 0.1\n"
			"3. \"minutxo\"  (numeric, optional) The amount in gkc to select as inputs. eg 0.01. \n"
			"4. \"address\"  (string, optional) The gkc address to entrust on. \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n"
            "\nExamples:\n" +
            HelpExampleCli("entrust", "\"0xac4bc2a311f4e49256ae1270e2b561995f509eb22b1e3f409d18840cda4469e1\" 0.1"));

    CBitcoinAddress address(GetAccountAddress(""));
	uint256 agentid;
	agentid.SetHex(params[0].get_str());
	if(!Entrustment::GetInstance().IsAgentIdValid(agentid))
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: agentid is invalid. ");
	
    CAmount nAmount = AmountFromValue(params[1]);
	LogPrintf("DPOS entrust | agentid=%s, nAmount=%lld\n",agentid.GetHex().c_str(),(int64_t)nAmount);

	BlockHeight predictHeight = chainActive.Height() + 1;
	if(nAmount < Entrustment::GetInstance().GetMinEntrustAmount(predictHeight))
	{
        throw JSONRPCError(RPC_INVALID_PARAMETER, std::string("Error: Amount too low. min: ") + FormatMoney(Entrustment::GetInstance().GetMinEntrustAmount(predictHeight)));
	}

    CWalletTx wtx;
	wtx.SetEntrust();
	wtx.SetAgentId(agentid);

    EnsureWalletIsUnlocked();

	CAmount nMinUtxo = 0;
	if(params.size() >= 3)
		nMinUtxo = AmountFromValue(params[2]);
	if(params.size() >= 4) {
		address.SetString(params[3].get_str());
		if(!address.IsGKC()){
			throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: GKC address invalid. ");
		}
	}
	
	SendMoneyEx(address.Get(), nAmount, wtx, false, nMinUtxo);

	LogPrintf("DPOS entrust | return %s\n",wtx.GetHash().GetHex().c_str());
    return wtx.GetHash().GetHex();
}

UniValue entrustwithcomment(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() < 3 || 5 < params.size())
        throw runtime_error(
            "entrustwithcomment agentid amount comment\n"
            "\nEntrust an amount to a given agent.\n" +
            HelpRequiringPassphrase() +
            "\nArguments:\n"
            "1. \"agentid\"  (string, required) The agentid to entrust to.\n"
            "2. \"amount\"   (numeric, required) The amount in gkc to entrust. eg 0.1\n"
			"3. \"comment\"	 (string, required) The comment to write to tx output. eg \"sample text\"\n"
			"4. \"minutxo\"  (numeric, optional) The amount in gkc to select as inputs. eg 0.01. \n"
			"5. \"address\"  (string, optional) The gkc address to entrust on. \n"
            "\nResult:\n"
            "\"transactionid\"  (string) The transaction id.\n"
            "\nExamples:\n" +
            HelpExampleCli("entrustwithcomment", "\"0xac4bc2a311f4e49256ae1270e2b561995f509eb22b1e3f409d18840cda4469e1\" 30 \"0x65e55156517D4c83DaC03D90A7c941956f530F30\""));

	BlockHeight predictHeight = chainActive.Height() + 1;
	if (predictHeight < forkheight_release_v_2_7_0) {
		throw JSONRPCError(RPC_INVALID_REQUEST, strprintf("Error: The command is not supported until blockheight %d",forkheight_release_v_2_7_0));
	}

	uint256 agentid; agentid.SetHex(params[0].get_str());
    CAmount nAmount = AmountFromValue(params[1]);
	string comment = params[2].get_str();
	CAmount nMinUtxo = 0;
    CBitcoinAddress address(GetAccountAddress(""));

	if(params.size() >= 4)
		nMinUtxo = AmountFromValue(params[3]);

	if(params.size() >= 5) {
		address.SetString(params[4].get_str());
		if(!address.IsGKC()){
			throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: GKC address invalid. ");
		}
	}

	if(!Entrustment::GetInstance().IsAgentIdValid(agentid))
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: agentid is invalid. ");
	

	if(nAmount < Entrustment::GetInstance().GetMinEntrustAmount(predictHeight))
	{
        throw JSONRPCError(RPC_INVALID_PARAMETER, std::string("Error: Amount too low. min: ") + FormatMoney(Entrustment::GetInstance().GetMinEntrustAmount(predictHeight)));
	}

    CWalletTx wtx;
	wtx.SetEntrust();
	wtx.SetAgentId(agentid);
	wtx.mapValue["comment"] = comment;

    EnsureWalletIsUnlocked();
	
	SendMoneyEx(address.Get(), nAmount, wtx, false, nMinUtxo);

    return wtx.GetHash().GetHex();
}

UniValue deprive(const UniValue& params, bool fHelp)
{
    if(fHelp || params.size() != 1)
        throw runtime_error("deprive txid\n");

	uint256 entrustTxId;
	entrustTxId.SetHex(params[0].get_str());

	const CWalletTx* entrustTx = pwalletMain->GetWalletTx(entrustTxId);
	if(!entrustTx)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: txid invalid.");

	assert(entrustTx);
	if(!entrustTx->IsEntrust())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Error: The txid isn't an enrust transaction.");

    CWalletTx wtxNew;
	CReserveKey reservekey(pwalletMain);
	std::string strError;
	if (!pwalletMain->CreateDepriveTransaction(*entrustTx, wtxNew, reservekey, chainActive.Height(), strError))
		throw JSONRPCError(RPC_WALLET_ERROR, strError);
	
	if (!pwalletMain->CommitTransaction(wtxNew, reservekey, "tx"))
		throw JSONRPCError(RPC_WALLET_ERROR, "CWallet::CommitTransaction() failed in deprive().");

    return wtxNew.GetHash().GetHex();
}
UniValue getentrustment(const UniValue& params, bool fHelp)
{
    if(fHelp)
        throw runtime_error(
            "getentrustment\n"
            "\nGet entrustment.\n");

	UniValue result = Entrustment::GetInstance().ToUniValue();
    return result;
}

UniValue getentrustlockheight(const UniValue& params, bool fHelp)
{
    if(fHelp)
        throw runtime_error(
            "getentrustlockheight\n"
            "\nGet lock heights of entrust tx.\n");

	UniValue result(UniValue::VOBJ);
	result.push_back(Pair("entrustTx",Entrustment::GetInstance().GetEntrustTxLockHeight()));
	result.push_back(Pair("entrustWithCommentTx",Entrustment::GetInstance().GetEntrustWithCommentTxLockHeight(chainActive.Height())));
    return result;
}

UniValue listagents(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"listagents (verbose)\n"
			"\nGet agent list.\n");

	bool verbose = false;
	if(params.size() > 0)
		verbose = true;
			
	UniValue agentArray(UniValue::VARR);
	std::vector<Agent> agents = Entrustment::GetInstance().GetAgentList(chainActive.Height());

	if(verbose)	{
		for (const Agent& agent : agents) {
			UniValue agentObj(UniValue::VOBJ);
			agentObj.push_back(Pair("id",agent.id.ToString()));
			agentObj.push_back(Pair("name",agent.name));
			agentObj.push_back(Pair("lastBlockTime",agent.lastBlockTime));
			agentObj.push_back(Pair("receivedTotalAmount",agent.receivedTotalAmount));
			agentObj.push_back(Pair("Users",agent.GetActiveUserCount()));
			agentObj.push_back(Pair("TotalDealDivideAmount",agent.GetTotalDealDivideAmount()));
			agentObj.push_back(Pair("TotalDividendAmount",agent.GetTotalDividendAmount()));
			agentArray.push_back(agentObj);
		}
		return agentArray;
	}
	
	for (const Agent& agent : agents) {
		agentArray.push_back(agent.Id().ToString());
	}
	
	return agentArray;
}

int GetLatestBlockNum(AgentID agentid, int hour_num)
{
	if(agentid.IsNull())
		return 0;
	
	int count = 0;
	CBlockIndex* blockIndex = chainActive.Tip();
	const time_t currentTime = blockIndex ? time_t(blockIndex->nTime) : time_t(GetAdjustedTime());
	const time_t validTimeRange = hour_num*60*60; // unit: second
	const time_t startTime = currentTime - validTimeRange;
	const time_t stopTime = currentTime;
	while(blockIndex){
		if(blockIndex->nTime < startTime || stopTime < blockIndex->nTime)
			break;
		CBlock block;
		if(ReadBlockFromDisk(block, blockIndex)){
			if(block.IsProofOfStake() && block.GetAgentId()==agentid)
				count++;
		}
		blockIndex = blockIndex->pprev;
	}
	
	return count;
}

UniValue getagent(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() < 1)
		throw runtime_error(
			"getagent agentid [blockheight]\n"
			"\nGet agent decription at an block height.\n");

	AgentID agentid;
	assert(params.size() >= 1);
	agentid.SetHex(params[0].get_str());
	BlockHeight height = chainActive.Height();
	if(params.size() > 1)
		height = params[1].get_int();
	const Agent* agent = Entrustment::GetInstance().GetAgent(height,agentid);
	if(!agent)
		throw JSONRPCError(RPC_INVALID_PARAMETER, std::string("Agent ") + agentid.ToString() + " not found.");

	UniValue result = agent->ToUniValue();
	int num24 = GetLatestBlockNum(agent->id,24);
	int num72 = GetLatestBlockNum(agent->id,72);
	result.push_back(Pair("lastest_24_hour_block_num",num24));
	result.push_back(Pair("lastest_72_hour_block_num",num72));
	return result;
}

UniValue listentrusts(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"listentrusts\n"
			"\nGet list all of my entrustment.\n");

	std::vector<COutPoint> vOutpts;
	pwalletMain->ListLockedCoins(vOutpts);
	LogPrintf("DPOS listentrusts | vOutpts.size=%d\n",vOutpts.size());
	
	UniValue ret(UniValue::VOBJ);
	UniValue outputs(UniValue::VARR);
	CAmount totalAmount = 0;
	CCoinsViewCache view(pcoinsTip);
	for (COutPoint& output : vOutpts)
	{
		CTransaction tx;
		uint256 hashBlock;
		if(GetTransaction(output.hash, tx, hashBlock, true))
		{
			if(pwalletMain->IsSpent(output.hash,output.n))
				continue;
			const CTxOut& txout = tx.vout[output.n];
			LogPrintf("DPOS listentrusts | outpoint=%s, txout=%s, IsMine=%d\n",output.ToString(),txout.ToString(),pwalletMain->IsMine(txout));
			if(pwalletMain->IsMine(txout) && txout.IsEntrusted())
			{
				UniValue o(UniValue::VOBJ);
				o.push_back(Pair("txid", output.hash.GetHex()));
				o.push_back(Pair("vout", (int)output.n));
				o.push_back(Pair("agent", tx.agentid.GetHex()));
				o.push_back(Pair("amount", ValueFromAmount(txout.nValue)));
				int unlockHeight = 0;
				if (tx.IsEntrustWithCommentTx()) {
					for (const CTxOut& out: tx.vout) {
						if (ScriptEx(out.scriptPubKey).IsComment()) {
							advertisement::TxComment comment;
							comment.DecodeFrom(out.scriptPubKey);
							o.push_back(Pair("comment", comment.content));
							break;
						}
					}
				}
				if(mapBlockIndex.count(hashBlock) > 0) {
					int txHeight = mapBlockIndex[hashBlock]->nHeight;
					int lockHeight = Entrustment::GetInstance().GetEntrustTxLockHeight();
					if (tx.IsEntrustWithCommentTx()) {
						lockHeight = Entrustment::GetInstance().GetEntrustWithCommentTxLockHeight(chainActive.Height());
					}
					o.push_back(Pair("txHeight", txHeight));
					o.push_back(Pair("lockHeight", lockHeight));
					o.push_back(Pair("unlockHeight", txHeight+lockHeight));
				}
				outputs.push_back(o);
				totalAmount += txout.nValue;
			}
		}
	}
	ret.push_back(Pair("amount",ValueFromAmount(totalAmount)));
	ret.push_back(Pair("outputs",outputs));

	return ret;
}

UniValue getcurrentseason(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() > 0)
		throw runtime_error(
			"getcurrentseason\n"
			"\nGet top agent nodes in current season.\n");

	UniValue result(UniValue::VOBJ);

	const BlockHeight current_height = chainActive.Height();
	crp::Season season = crp::CoinReleasePlan::GetInstance().GetSeason(current_height+1);

	result.push_back(Pair("first_height",season.first_height));
	result.push_back(Pair("last_height",season.last_height));

	Entrustment::GetInstance().Lock();
	SortRule* sort_rule = nullptr;
	DescBySeasonEntrustAmount descByEntrustAmount;
	DescBySeasonRewardFactor descByRewardFactor;
	if(Entrustment::GetInstance().IsSeasonRewardV2(season.reward_height))
		sort_rule = &descByRewardFactor;
	else
		sort_rule = &descByEntrustAmount;
	assert(sort_rule);
	std::vector<Agent*> node_vec = Entrustment::GetInstance().GetAgentsRank(current_height,*sort_rule);
	UniValue node_arry(UniValue::VARR);
	const size_t size = node_vec.size();
	for(int i=0; i<size; i++){
		const Agent& a = *node_vec[i];
		UniValue agent(UniValue::VOBJ);
		agent.push_back(Pair("rank",i+1));
		agent.push_back(Pair("nodeid",a.id.ToString()));
		agent.push_back(Pair("name",a.name));
		agent.push_back(Pair("blocks",a.monthlyCount.blockAmount));
		agent.push_back(Pair("entrust",a.monthlyCount.entrustAmount));
		agent.push_back(Pair("last_season_reward",a.last_season.expect_reward));
		agent.push_back(Pair("current_season_reward_factor",CBigNum(a.monthlyCount.reward_factor).ToString()));
		agent.push_back(Pair("expected_reward",a.monthlyCount.expect_reward));
		node_arry.push_back(agent);
	}
	result.push_back(Pair("node_arry",node_arry));
	Entrustment::GetInstance().Unlock();

	return result;
}

UniValue listdeprives(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"listdeprives\n"
			"\nGet list all of my deprivement.\n");

	std::vector<COutPoint> vOutpts;
	pwalletMain->GetDepriveTxOut(vOutpts);
	
	UniValue ret(UniValue::VOBJ);
	UniValue outputs(UniValue::VARR);
	CAmount totalAmount = 0;
	for (COutPoint& output : vOutpts)
	{
		CTransaction tx;
		uint256 hashBlock;
		if(GetTransaction(output.hash, tx, hashBlock, true))
		{
			const CTxOut& txout = tx.vout[output.n];
			if(tx.IsDeprive() && pwalletMain->IsMine(txout) && !txout.IsEntrusted())
			{
				UniValue o(UniValue::VOBJ);
				o.push_back(Pair("txid", output.hash.GetHex()));
				o.push_back(Pair("vout", (int)output.n));
				o.push_back(Pair("agent", tx.agentid.GetHex()));
				o.push_back(Pair("amount", ValueFromAmount(txout.nValue)));
				outputs.push_back(o);
				totalAmount += txout.nValue;
			}
		}
	}
	ret.push_back(Pair("amount",ValueFromAmount(totalAmount)));
	ret.push_back(Pair("outputs",outputs));

	return ret;
}

UniValue listentrustrewards(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"listentrustrewards\n"
			"\nGet list all of my reward for entrustment.\n");

	UniValue ret(UniValue::VOBJ);
	UniValue outputs(UniValue::VARR);
	CAmount totalAmount = 0;
	{
		LOCK(pwalletMain->cs_wallet);
		for (map<uint256, CWalletTx>::const_iterator it = pwalletMain->mapWallet.begin(); it != pwalletMain->mapWallet.end(); ++it)
		{
			uint256 txhash = it->first;
			const CWalletTx& tx = it->second;
			if(!tx.IsCoinStake())
				continue;
			
			for (size_t i = 0; i < tx.vout.size(); i++) 
			{
				const CTxOut& txout = tx.vout[i];
				if(txout.type != CTxOut::Type::ENTRUST_REWARD || !pwalletMain->IsMine(txout))
					continue;
				UniValue o(UniValue::VOBJ);
				o.push_back(Pair("txid", txhash.GetHex()));
				o.push_back(Pair("vout", i));
				o.push_back(Pair("agent", tx.agentid.GetHex()));
				o.push_back(Pair("amount", ValueFromAmount(txout.nValue)));
				outputs.push_back(o);
				totalAmount += txout.nValue;
			}
		}
	}
	
	ret.push_back(Pair("amount",ValueFromAmount(totalAmount)));
	ret.push_back(Pair("outputs",outputs));

	return ret;
}

UniValue getrawblock(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"getrawblock blockheight\n"
			"\nGet block object by height.\n");

    int nHeight = params[0].get_int();
    if (nHeight < 0 || nHeight > chainActive.Height())
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Block height out of range");

	CBlock block;
	if(!ReadBlockFromDisk(block,chainActive[nHeight]))
		throw JSONRPCError(RPC_INTERNAL_ERROR, "ReadBlockFromDisk() failed.");

    CDataStream ssData(SER_NETWORK, PROTOCOL_VERSION);
    ssData << block;
    return HexStr(ssData.begin(), ssData.end());
}

UniValue getaddressdpos(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() < 1)
		throw runtime_error(
			"getaddressdpos gkcaddress\n"
			"\nGet dpos info of address.\n");

	CBitcoinAddress addr(params[0].get_str());

	UniValue result(UniValue::VOBJ);

	BlockHeight height = chainActive.Height();
	CAmount totalEntrust = 0, totalReward = 0, totalUnpaid = 0;
	std::vector<Agent> agents = Entrustment::GetInstance().GetAgentList(chainActive.Height());
	for (const Agent& agent : agents) {
		std::map<CBitcoinAddress,dpos::UserStatistics>::const_iterator it = agent.userStatistics.find(addr);
		if(it != agent.userStatistics.end()){
			const dpos::UserStatistics& statistics = it->second;
			totalEntrust += statistics.entrustAmount;
			totalReward += statistics.totalDividendReward;
			totalUnpaid += (statistics.totalDealDivideReward - statistics.totalDividendReward);
		}
	}

	result.push_back(Pair("Height",std::to_string(height)));
	result.push_back(Pair("Entrust",FormatMoney(totalEntrust)));
	result.push_back(Pair("Incomming",FormatMoney(totalReward)));
	result.push_back(Pair("Unpaid",FormatMoney(totalUnpaid)));

	return result;
}

UniValue getcrp(const UniValue& params, bool fHelp)
{
	if(fHelp)
		throw runtime_error(
			"getcrp\n"
			"\nGet coin release plan.\n");

	const crp::CoinReleasePlan& plan = crp::CoinReleasePlan::GetInstance();
	UniValue result(UniValue::VOBJ);
	UniValue powObj(UniValue::VOBJ);
	UniValue posObj(UniValue::VOBJ);
	UniValue otherObj(UniValue::VOBJ);
	
	powObj.push_back(Pair("velocity",plan.pow.velocity));
	powObj.push_back(Pair("begin-height",plan.pow.heightRange.Begin()));
	powObj.push_back(Pair("back-height",plan.pow.heightRange.Back()));
	powObj.push_back(Pair("firstBlockRewardAmount",plan.pow.firstBlockRewardAmount));
	powObj.push_back(Pair("stableBlockNum",plan.pow.stableBlockNum));
	powObj.push_back(Pair("stableBlockReward",plan.pow.stableBlockReward));
	result.push_back(Pair("pow",powObj));

	const crp::PosPlan& posplan = plan.GetPosPlan(chainActive.Height());
	posObj.push_back(Pair("velocity",posplan.velocity));
	posObj.push_back(Pair("begin-height",posplan.heightRange.Begin()));
	posObj.push_back(Pair("back-height",posplan.heightRange.Back()));
	posObj.push_back(Pair("standardMinerReward",posplan.standardMinerReward));
	posObj.push_back(Pair("standardFundReward",posplan.standardFundReward));
	posObj.push_back(Pair("standardEntrustReward",posplan.standardEntrustReward));
	posObj.push_back(Pair("standardMasternodeReward",posplan.standardMasternodeReward));
	posObj.push_back(Pair("monthlyTopFirst",posplan.monthlyTopFirst));
	posObj.push_back(Pair("monthlyTopSecond",posplan.monthlyTopSecond));
	result.push_back(Pair("pos",posObj));

	otherObj.push_back(Pair("zero-height",Params().Zerocoin_StartHeight()));
	otherObj.push_back(Pair("contract-height",Params().Contract_StartHeight()));
	result.push_back(Pair("other",otherObj));
	
	return result;
}

UniValue getdposnameofblock(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() != 1)
		throw runtime_error(
			"getdposnameofblock blockhash\n"
			"\nGet dpos name of block\n");

	uint256 blockhash;
	blockhash.SetHex(params[0].get_str());

    if (mapBlockIndex.count(blockhash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[blockhash];

    if (!ReadBlockFromDisk(block, pblockindex))
        throw JSONRPCError(RPC_INTERNAL_ERROR, "Can't read block from disk");

	string nodeid,name;
	CTransaction* coinstake = block.GetCoinStake();
	if(coinstake){
		AgentID agentid = coinstake->agentid;
		const Agent* agent = Entrustment::GetInstance().GetAgent(chainActive.Height(),agentid);
		if(!agent)
			throw JSONRPCError(RPC_INVALID_PARAMETER, "Agent not found");
		nodeid = agentid.ToString();
		name = agent->name;
	} else {
		// Coinstake not found
		name = "pow";
	}

	UniValue result(UniValue::VOBJ);
	result.push_back(Pair("nodeid",nodeid));
	result.push_back(Pair("name",name));
	return result;
}

template<typename T>
void GetVecPage(const std::vector<T>& vec, int offset, int count, std::vector<T>& output)
{
	int size = vec.size();
	const int begin = offset;
	const int end = begin + count;
	assert(begin <= end);
	for(int i=0; i<size; i++)
	{
		if(i<begin) continue;
		else if(i>=end) break;
		output.push_back(vec[i]);
	}
}

UniValue listad(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() > 2)
		throw runtime_error(
			"listad (offset=0) (count=20)\n");

	int offset = 0;
	int count = 20;
	if(params.size() > 0)
		offset = params[0].get_int();
	if(params.size() > 1)
		count = params[1].get_int();
	
	std::vector<CTransaction> adVec;
	{
		LOCK(pwalletMain->cs_wallet);
		for(map<uint256,CWalletTx>::const_iterator it = pwalletMain->mapAdvertise.begin(); it != pwalletMain->mapAdvertise.end(); it++)
		{
			adVec.push_back(it->second);
		}
	}
	std::vector<CTransaction> page;
	GetVecPage(adVec,offset,count,page);

	UniValue result(UniValue::VARR);
	int size = page.size();
	for(int i=0; i<size; i++)
	{
		const CTransaction& tx = page[i];
		advertisement::Advertise ad;
		for(const CTxOut& txout : page[i].vout)
		{
			ScriptEx scriptex(txout.scriptPubKey);
			if(scriptex.IsAdvertise())
			{
				ad.DecodeFrom(txout.scriptPubKey);
				break;
			}
		}
		UniValue adObj(UniValue::VOBJ);
		adObj.push_back(Pair("tx",tx.GetHash().ToString()));
		adObj.push_back(Pair("title",ad.title));
		adObj.push_back(Pair("author",ad.author));
		adObj.push_back(Pair("text",ad.text));
		adObj.push_back(Pair("link",ad.link));
		result.push_back(adObj);
	}

	return result;
}

UniValue listsysmsg(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() > 2)
		throw runtime_error(
			"listsysmsg (offset=0) (count=20)\n");

	int offset = 0;
	int count = 20;
	if(params.size() > 0)
		offset = params[0].get_int();
	if(params.size() > 1)
		count = params[1].get_int();
	
	std::vector<CTransaction> adVec;
	{
		LOCK(pwalletMain->cs_wallet);
		for(map<uint256,CWalletTx>::const_iterator it = pwalletMain->mapSysMessage.begin(); it != pwalletMain->mapSysMessage.end(); it++)
		{
			adVec.push_back(it->second);
		}
	}
	std::vector<CTransaction> page;
	GetVecPage(adVec,offset,count,page);

	UniValue result(UniValue::VARR);
	int size = page.size();
	for(int i=0; i<size; i++)
	{
		const CTransaction& tx = page[i];
		advertisement::SysMessage msg;
		for(const CTxOut& txout : page[i].vout)
		{
			ScriptEx scriptex(txout.scriptPubKey);
			if(scriptex.IsSysMessage())
			{
				msg.DecodeFrom(txout.scriptPubKey);
				break;
			}
		}
		UniValue adObj(UniValue::VOBJ);
		adObj.push_back(Pair("txid",tx.GetHash().ToString()));
		adObj.push_back(Pair("text",msg.text));
		result.push_back(adObj);
	}

	return result;
}

UniValue listadfilter(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() > 2)
		throw runtime_error(
			"listadfilter (offset=0) (count=20)\n");

	int offset = 0;
	int count = 20;
	if(params.size() > 0)
		offset = params[0].get_int();
	if(params.size() > 1)
		count = params[1].get_int();
	
	std::vector<CTransaction> adVec;
	{
		LOCK(pwalletMain->cs_wallet);
		for(map<uint256,CWalletTx>::const_iterator it = pwalletMain->mapFilterAd.begin(); it != pwalletMain->mapFilterAd.end(); it++)
		{
			adVec.push_back(it->second);
		}
	}
	std::vector<CTransaction> page;
	GetVecPage(adVec,offset,count,page);

	UniValue result(UniValue::VARR);
	int size = page.size();
	for(int i=0; i<size; i++)
	{
		const CTransaction& tx = page[i];
		advertisement::AdFilter filter;
		for(const CTxOut& txout : page[i].vout)
		{
			ScriptEx scriptex(txout.scriptPubKey);
			if(scriptex.IsFilterAd())
			{
				filter.DecodeFrom(txout.scriptPubKey);
				break;
			}
		}
		UniValue adObj(UniValue::VOBJ);
		adObj.push_back(Pair("txid",tx.GetHash().ToString()));
		adObj.push_back(Pair("filter",filter.txid.ToString()));
		result.push_back(adObj);
	}

	return result;
}

UniValue getmnemoniccode(const UniValue& params, bool fHelp)
{
    if (fHelp)
        throw runtime_error(
            "getmnemoniccode\n"
            "\nDumps mnemonic code in wallet.\n");
    LOCK2(cs_main, pwalletMain->cs_wallet);

    EnsureWalletIsUnlocked();

	std::string result;
	
    MnemonicContainer mContainer = pwalletMain->GetMnemonicContainer();
    const CHDChain& chain = pwalletMain->GetHDChain();
    if(!mContainer.IsNull() && chain.nVersion >= CHDChain::VERSION_WITH_BIP39)
    {
        if(mContainer.IsCrypted())
        {
            if(!pwalletMain->DecryptMnemonicContainer(mContainer))
                throw JSONRPCError(RPC_INTERNAL_ERROR, "Cannot decrypt hd chain");
        }

        SecureString mnemonic;
        if(mContainer.GetMnemonic(mnemonic))
            result = mnemonic.c_str();
    }
	return result;
}

UniValue listblackagents(const UniValue& params, bool fHelp)
{
	if(fHelp)
        throw runtime_error("listblackagents");
		
	dpos::Blacklist blist = Entrustment::GetInstance().GetSeasonRewardBlacklist(chainActive.Height());
	std::vector<AgentID> agentlist;
	blist.ListAgents(agentlist);
	UniValue result(UniValue::VARR);
	for(const auto& id : agentlist)
		result.push_back(id.ToString());
	return result;
}

UniValue addblackagent(const UniValue& params, bool fHelp)
{
	if(fHelp || params.size() < 1)
        throw runtime_error("addblackagent <agentid>");

	try
	{
		if(!Entrustment::GetInstance().BlackAgentListAvailable(chainActive.Height()))
			throw std::runtime_error("Feature not available");

		AgentID agentid;
		agentid.SetHex(params[0].get_str());

		if(!Entrustment::GetInstance().IsAgentIdValid(agentid))
			throw std::runtime_error("Agent not found");

		if(!pwalletMain)
			throw std::runtime_error("invalid wallet");
		CWalletTx tx = pwalletMain->CreateAddBlackAgentTx(agentid);
		
		CReserveKey reservekey(pwalletMain);
		if(!pwalletMain->CommitTransaction(tx,reservekey,"tx"))
			throw std::runtime_error("CommitTransaction() failed");
		
		return tx.GetHash().ToString();
	}
	catch(std::exception& e)
	{
		throw JSONRPCError(RPC_WALLET_ERROR, e.what());
	}
}

#endif

