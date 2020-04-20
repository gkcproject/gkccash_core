// Copyright (c) 2017 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/zerocoin.h"

void CZerocoinSpendReceipt::AddSpend(const CZerocoinSpend& spend)
{
    vSpends.emplace_back(spend);
}

std::vector<CZerocoinSpend> CZerocoinSpendReceipt::GetSpends()
{
    return vSpends;
}

void CZerocoinSpendReceipt::SetStatus(std::string strStatus, int nStatus, int nNeededSpends)
{
    strStatusMessage = strStatus;
    this->nStatus = nStatus;
    this->nNeededSpends = nNeededSpends;
}

std::string CZerocoinSpendReceipt::GetStatusMessage()
{
    return strStatusMessage;
}

int CZerocoinSpendReceipt::GetStatus()
{
    return nStatus;
}

int CZerocoinSpendReceipt::GetNeededSpends()
{
    return nNeededSpends;
}

std::string CZerocoinMint::ToString() const
{
	return ToUniValue().write();
}

UniValue CZerocoinMint::ToUniValue() const
{
	UniValue result(UniValue::VOBJ);
	result.push_back(Pair("denomination",(int)denomination));
	result.push_back(Pair("nHeight",nHeight));
	result.push_back(Pair("value",value.GetHex()));
	result.push_back(Pair("randomness",randomness.GetHex()));
	result.push_back(Pair("serialNumber",serialNumber.GetHex()));
	result.push_back(Pair("txid",txid.GetHex()));
	result.push_back(Pair("isUsed",isUsed));
	return result;
}

