#ifndef RPCRAWTRANSACTION_H
#define RPCRAWTRANSACTION_H

#include <string>
#include "uint256.h"

std::string CreateContractAddress(const uint256& txid, uint32_t voutNumber);

#endif // ~RPCRAWTRANSACTION_H



