#ifndef RPC_TOKEN_H
#define RPC_TOKEN_H

#include <univalue.h>

// get token balance of
UniValue gettokenbalance(const UniValue& params, bool fHelp);

// transfer token
UniValue transfertoken(const UniValue& params, bool fHelp);

// list token transactions of
UniValue listtokentransactions(const UniValue& params, bool fHelp);

#endif // ~RPC_TOKEN_H