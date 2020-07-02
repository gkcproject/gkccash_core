
#ifndef BITCOIN_SCRIPTEX_H
#define BITCOIN_SCRIPTEX_H

#include <string>
#include <cstdint>
#include <vector>
#include "script/script.h"
#include "base58.h"
#include "keystore.h"

namespace scriptex
{
	typedef std::vector<unsigned char> bytes_t;
	
	CScript EncodeBlackAgentToScript(const AgentID&);
	void DecodeBlackAgentFromScript(const CScript&, AgentID&, bytes_t& signature);
	bool IsBlackAgent(const CScript&);
}

namespace op_return_type
{
	extern const uint32_t ADVERTISE;
	extern const uint32_t SYSTEM;
	extern const uint32_t ADFILTER;
	extern const uint32_t COMMENT;
	extern const uint32_t BLACKAGENT;
}

class ScriptEx
{
	const CScript& _script;
public:
	ScriptEx(const CScript& s):_script(s){}
	bool IsAdvertise() const;
	bool IsSysMessage() const;
	bool IsFilterAd() const;
	bool IsComment() const;
};

namespace advertisement
{
	class OpReturnMessage
	{
	public:
		uint32_t type;
		uint16_t version;
	};
	class Advertise : public OpReturnMessage
	{
	public:
		std::string title,author,text,link,extend;
		bool EncodeTo(CScript&) const;
		bool DecodeFrom(const CScript&);
	};
	
	class AdminMessage : public OpReturnMessage
	{
	public:
		std::vector<unsigned char> vchSig;
		bool AdminSign(const CKeyStore&, uint256);
	};

	class SysMessage : public AdminMessage
	{
	public:
		std::string text;
		std::vector<unsigned char> extend;
		bool AdminSign(const CKeyStore&);
		bool EncodeTo(CScript&) const;
		bool DecodeFrom(const CScript&);
	};

	class AdFilter : public AdminMessage
	{
	public:
		uint256 txid;
		std::vector<unsigned char> extend;
		bool AdminSign(const CKeyStore&);
		bool EncodeTo(CScript&) const;
		bool DecodeFrom(const CScript&);
	};

	class TxComment : public OpReturnMessage
	{
	public:
		std::string content;
		bool EncodeTo(CScript&) const;
		bool DecodeFrom(const CScript&);
	};

	class AdAdmin
	{
	public:
		CBitcoinAddress address;
		CPubKey pubkey;
		uint160 pubkeyhash;
		static const AdAdmin& GetInstance();
	protected:
		AdAdmin();
	};

	CPubKey GetAdminPubKey();
	uint160 GetAdminPubKeyHash();
	bool IsSignByAdministrator(const CScript&);
	bool AdminSign(const CKeyStore&, uint256 content, vector<unsigned char>&);
	
	bool FilterAd(const Advertise&);
}

namespace util
{
	std::string BytesToHexString(const std::vector<unsigned char>& bytes);
	std::vector<unsigned char> HexStringToBytes(const std::string& s);
}

#endif // BITCOIN_SCRIPTEX_H
