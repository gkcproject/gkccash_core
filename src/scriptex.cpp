
#include "scriptex.h"
#include "util.h"
#include "hash.h"
#include "wallet.h"
#include "pubkey.h"
#include "init.h"
#include "sensitivewords.h"

using namespace op_return_type;
using namespace advertisement;

static bool GetScriptDataVec(const CScript& script, vector<valtype>& dataVec);

const uint32_t op_return_type::ADVERTISE = 0x41445645;	
const uint32_t op_return_type::SYSTEM    = 0x53595354;	
const uint32_t op_return_type::ADFILTER  = 0x41444649;	
const uint32_t op_return_type::COMMENT   = 0x434f4d4d;	
const uint32_t op_return_type::BLACKAGENT = 0x424c4143; 

static void _encode_header(uint32_t typeLabel, uint16_t version, std::vector<unsigned char>& header)
{
	header.push_back((typeLabel&0xff000000) >> 24);
	header.push_back((typeLabel&0x00ff0000) >> 16);
	header.push_back((typeLabel&0x0000ff00) >> 8);
	header.push_back(typeLabel&0x000000ff);

	header.push_back(version&0x00ff);
	header.push_back((version&0xff00) >> 8);
}

static uint32_t _decode_type(const unsigned char* p)
{
	uint32_t type = 0;
	type = (type | p[0]) << 8;
	type = (type | p[1]) << 8;
	type = (type | p[2]) << 8;
	type = (type | p[3]);
	return type;
}

static uint16_t _decode_version(const unsigned char* p)
{
	uint16_t ver = 0;
	ver = (ver | p[1]) << 8;
	ver = (ver | p[0]);
	return ver;
}

static bool _decode_header(const valtype& header, uint32_t& type, uint16_t& ver)
{
	const unsigned char* p = header.data();
	if(header.size() < 6)
		return false;
	
	type = _decode_type(p);
	ver = _decode_version(p+4);

	return true;
}

static uint32_t _check_type(const unsigned char* p, size_t size)
{
	uint32_t n = 0;
	if(size >= 6 && p[0]==OP_RETURN && p[1]==0x06)
	{
		n = _decode_type(p+2);
	}
	return n;
}

bool Advertise::EncodeTo(CScript& script) const
{
	std::vector<unsigned char> header;
	_encode_header(op_return_type::ADVERTISE, 1, header);
	script = CScript() 
		<< OP_RETURN
		<< header
		<< ToByteVector(title)
		<< ToByteVector(author) 
		<< ToByteVector(text) 
		<< ToByteVector(link)
		<< ToByteVector(extend);
	return true;
}

static std::string _decode_string(const std::vector<unsigned char>& vch)
{
	std::string s;
	for(const unsigned char& ch : vch)
		s.push_back((char)ch);
	return s;
}

bool Advertise::DecodeFrom(const CScript& script)
{
	vector<valtype> dataVec;
	if(!GetScriptDataVec(script,dataVec))
		return false;
	
	if(dataVec.size() < 6)
	{
		LogPrint("ad","Advertise::DecodeFrom | dataVec.size() too low: %d\n",(int)dataVec.size());
		return false;
	}

	_decode_header(dataVec[0],type,version);

	title = _decode_string(dataVec[1]);
	author = _decode_string(dataVec[2]);
	text = _decode_string(dataVec[3]);
	link = _decode_string(dataVec[4]);
	extend = _decode_string(dataVec[5]);

	return true;
}

bool AdminMessage::AdminSign(const CKeyStore& keystore, uint256 hash)
{
	return advertisement::AdminSign(keystore,hash,vchSig);
}

bool SysMessage::AdminSign(const CKeyStore& keystore)
{
    CHashWriter ss(SER_GETHASH, 0);
    ss << text;
	const uint256 textHash = ss.GetHash();
	return AdminMessage::AdminSign(keystore,textHash);
}

bool AdFilter::AdminSign(const CKeyStore& keystore)
{
	return AdminMessage::AdminSign(keystore,txid);
}

bool SysMessage::EncodeTo(CScript& script) const
{
	std::vector<unsigned char> header;
	_encode_header(op_return_type::SYSTEM, 1, header);
	script = CScript() 
		<< OP_RETURN
		<< header
		<< ToByteVector(text)
		<< vchSig
		<< extend;
	return true;
}

bool AdFilter::EncodeTo(CScript& script) const
{
	std::vector<unsigned char> header;
	_encode_header(op_return_type::ADFILTER, 1, header);
	script = CScript() 
		<< OP_RETURN
		<< header
		<< ToByteVector(txid)
		<< vchSig
		<< extend;
	return true;
}

bool TxComment::EncodeTo(CScript& script) const
{
	std::vector<unsigned char> header;
	_encode_header(op_return_type::COMMENT, 1, header);
	script = CScript() 
		<< OP_RETURN
		<< header
		<< ToByteVector(content);
	return true;
}

bool TxComment::DecodeFrom(const CScript& script)
{
	vector<valtype> dataVec;
	if(!GetScriptDataVec(script,dataVec))
		return false;
	
	if(dataVec.size() < 2)
	{
		LogPrint("ad","TxComment::DecodeFrom | dataVec.size() too low: %d\n",(int)dataVec.size());
		return false;
	}

	_decode_header(dataVec[0],type,version);

	valtype vch = dataVec[1];
	for(const unsigned char& ch : vch)
		content.push_back((char)ch);

	return true;
}

bool ScriptEx::IsAdvertise() const
{
	return _check_type(_script.data(),_script.size()) == op_return_type::ADVERTISE;
}

bool ScriptEx::IsSysMessage() const
{
	return _check_type(_script.data(),_script.size()) == op_return_type::SYSTEM;
}

bool ScriptEx::IsFilterAd() const
{
	return _check_type(_script.data(),_script.size()) == op_return_type::ADFILTER;
}

bool ScriptEx::IsComment() const
{
	return _check_type(_script.data(),_script.size()) == op_return_type::COMMENT;
}

static bool GetScriptDataVec(const CScript& script, vector<valtype>& dataVec)
{
	CScript::const_iterator pc = script.begin();
	CScript::const_iterator pend = script.end();
	opcodetype opcode;
	valtype vchPushValue;
	while (pc < pend)
	{
		if (!script.GetOp(pc, opcode, vchPushValue))
		{
			LogPrint("ad","GetScriptDataVec | GetOp() failed\n");
			return false;
		}
		if(opcode==OP_RETURN)
			continue;
		dataVec.push_back(vchPushValue);
		LogPrint("ad","GetScriptDataVec | opcode=0x%02x, vchPushValue.size()=%d, dataVec.size()=%d\n",opcode,(int)vchPushValue.size(),(int)dataVec.size());
	}
	return true;
}
bool SysMessage::DecodeFrom(const CScript& script)
{
	vector<valtype> dataVec;
	if(!GetScriptDataVec(script,dataVec))
	{
		LogPrint("ad","SysMessage::DecodeFrom | GetScriptDataVec() failed, script=%s\n",script.ToString());
		return false;
	}
	
	if(dataVec.size() < 3)
	{
		LogPrint("ad","SysMessage::DecodeFrom | dataVec.size() too low: %d\n",(int)dataVec.size());
		return false;
	}

	_decode_header(dataVec[0],type,version);
	LogPrint("ad","SysMessage::DecodeFrom | dataVec[0].size()=%d, type=0x%08x, version=%d\n",dataVec[0].size(),type,version);

	valtype vch = dataVec[1];
	for(const unsigned char& ch : vch)
		text.push_back((char)ch);
	LogPrint("ad","SysMessage::DecodeFrom | dataVec[1].size()=%d, text=%s\n",dataVec[1].size(),text);

	vchSig = dataVec[2];
	LogPrint("ad","SysMessage::DecodeFrom | dataVec[2].size()=%d\n",dataVec[2].size());

	return true;
}

bool AdFilter::DecodeFrom(const CScript& script)
{
	vector<valtype> dataVec;
	if(!GetScriptDataVec(script,dataVec))
		return false;

	if(dataVec.size() < 3)
	{
		LogPrint("ad","AdFilter::DecodeFrom | dataVec.size() too low: %d\n",(int)dataVec.size());
		return false;
	}

	_decode_header(dataVec[0],type,version);

	txid = uint256(dataVec[1]);

	vchSig = dataVec[2];

	return true;
}

std::string util::BytesToHexString(const std::vector<unsigned char>& bytes)
{
	string s;
	for(auto b : bytes)
	{
		char buf[4] = {0};
		sprintf(buf,"%02x",(int)b);
		s += string(buf);
	}
	return s;
}

std::vector<unsigned char> util::HexStringToBytes(const std::string& s)
{
	std::vector<unsigned char> v;
	const char* p = s.c_str();
	size_t size = s.length();
	for(int i=0; i<size; i+=2)
	{
		int n = 0;
		sscanf(p+i,"%02x",&n);
		v.push_back((unsigned char)n);
	}
	return v;
}

AdAdmin::AdAdmin()
{
	address.SetString("GMRVdRo6pq2xoS7abn8yYswQAXj5UT4btj");

	// execute rpc command with admin wallet: getadminpubkey address

	vector<unsigned char> pubKeyData = util::HexStringToBytes("033fd7d68f0c4ec1c3ab3e98162494bf3db7ad3b53f4c6f42768281b93eed06693");
	pubkey = CPubKey(pubKeyData.data(),pubKeyData.data()+pubKeyData.size());

	pubkeyhash = uint160("d86578f9da44c81fcb800bce345e296501d44127");
}

const AdAdmin& AdAdmin::GetInstance()
{
	static AdAdmin admin;
	return admin;
}

CPubKey advertisement::GetAdminPubKey()
{
	return AdAdmin::GetInstance().pubkey;
}

uint160 advertisement::GetAdminPubKeyHash()
{
	return AdAdmin::GetInstance().pubkeyhash;
}

bool advertisement::IsSignByAdministrator(const CScript& script)
{
	using namespace advertisement;
	uint256 sigContent;
	vector<unsigned char> vchSig;
	ScriptEx scriptex(script);
	if(scriptex.IsSysMessage())
	{
		SysMessage sysMsg;
		if(sysMsg.DecodeFrom(script))
		{
			CHashWriter ss(SER_GETHASH, 0);
			ss << sysMsg.text;
			sigContent = ss.GetHash();
			vchSig = sysMsg.vchSig;
		}
	}
	else if(scriptex.IsFilterAd())
	{
		AdFilter adFilter;
		if(adFilter.DecodeFrom(script))
		{
			sigContent = adFilter.txid;
			vchSig = adFilter.vchSig;
		}
	}
	else
	{
		// undefined script type
	}

	CPubKey pubKey = advertisement::GetAdminPubKey();
	//uint160 pubKeyHash = advertisement::GetAdminPubKeyHash();
	//CPubKey pubKey = CPubKey(valtype(pubKeyHash.begin(),pubKeyHash.end()));
    if (!pubKey.Verify(sigContent, vchSig)) {
        LogPrint("ad","IsSignByAdministrator | Verify() failed\n");
        return false;
    }
	return true;
}

bool advertisement::AdminSign(const CKeyStore& keystore, uint256 hash, vector<unsigned char>& vchSig)
{
	CKeyID keyID = advertisement::GetAdminPubKey().GetID();
	//uint160 pubKeyHash = advertisement::GetAdminPubKeyHash();
	//CKeyID keyID = CKeyID(pubKeyHash);

    CKey privKey;
	if(!keystore.GetKey(keyID,privKey))
	{
		LogPrint("ad","advertisement::AdminSign | Couln't get admin key");
		return false;
	}

	LogPrint("ad","advertisement::AdminSign | privKey=%s\n",util::BytesToHexString(std::vector<unsigned char>(privKey.begin(),privKey.end())));
	if (!privKey.Sign(hash, vchSig))
	{
		LogPrint("ad","Sign() failed");
		return false;
	}

	return true;
}

bool advertisement::FilterAd(const Advertise& ad)
{
	return advertisement::SensitiveWordsLib::GetInstance().Match(ad.text);
}

CScript scriptex::EncodeBlackAgentToScript(const AgentID& agentid)
{
	CScript script;
	bytes_t header;
	_encode_header(op_return_type::BLACKAGENT, 1, header);
	script << OP_RETURN
		<< header
		<< ToByteVector(agentid);
	return script;
}

bool scriptex::IsBlackAgent(const CScript& script)
{
	return _check_type(script.data(),script.size()) == op_return_type::BLACKAGENT;
}

void scriptex::DecodeBlackAgentFromScript(const CScript& script, AgentID& agentid, bytes_t& sig)
{
	vector<valtype> dataVec;
	if(GetScriptDataVec(script,dataVec))
	{
		size_t size = dataVec.size();
		if(size >= 2)
		{
			OpReturnMessage msg;
			_decode_header(dataVec[0],msg.type,msg.version);
		
			valtype vch = dataVec[1];
			agentid = AgentID(vch);
		
			if(size >= 3)
				sig = dataVec[2];
		}
	}	
}

