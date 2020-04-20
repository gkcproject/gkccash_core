#include "main.h"
#include "extendeddata.h"

#define HDR_LEN_SIZE    (2)
#define HDR_TYPE_SIZE   (1)
#define HDR_KEY_SIZE    (32)

static bool findExtendedKeyData(const std::vector<unsigned char> vExtendData, const std::string strKey, eExtendDataType & type, std::vector<uint8_t>& value)
{
    uint32_t        u32Offset;
    uint32_t        u32SecLens;
    uint32_t        u32TotalLens;
    uint32_t        u32TotalCounts;

    if (vExtendData.empty() || 0 == strKey.size())
    {
        return false;
    }
    uint8_t *pu8Params = (uint8_t *)vExtendData.data();

    //  check script.
    u32TotalLens = (pu8Params[0] << 8) | (pu8Params[1]);
    u32TotalCounts = pu8Params[2];
    u32Offset = 3;

    //  lens not the same.
    if (u32TotalLens > vExtendData.size())
    {
        return false;
    }

    while (u32Offset < u32TotalLens && u32TotalCounts > 0)
    {
        u32SecLens = (pu8Params[u32Offset] << 8) | (pu8Params[u32Offset + 1]);
        u32TotalCounts--;

        //  check type.
        switch (eExtendDataType(pu8Params[u32Offset + 2]))
        {
            case EXT_DATA_STRING:
            case EXT_DATA_DOUBLE:
                break;

            case EXT_DATA_BOOL:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + sizeof(int8_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT8:
            case EXT_DATA_UINT8:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + sizeof(int8_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT16:
            case EXT_DATA_UINT16:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + sizeof(int16_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT32:
            case EXT_DATA_UINT32:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + sizeof(int32_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT64:
            case EXT_DATA_UINT64:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + sizeof(int64_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT128:
            case EXT_DATA_UINT128:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + 16 * sizeof(int8_t)) != u32SecLens) return false;
                break;

            case EXT_DATA_INT256:
            case EXT_DATA_UINT256:
                if ((HDR_LEN_SIZE + HDR_TYPE_SIZE + HDR_KEY_SIZE + 32 * sizeof(int8_t)) != u32SecLens) return false;
                break;

            default:
                return false;
                break;
        }

        u32Offset += u32SecLens;
    }

    if (u32Offset != u32TotalLens || 0 != u32TotalCounts)
    {
        return false;
    }

    u32Offset = 3;
    while (u32Offset < u32TotalLens)
    {
        std::string         strSecKey;

        uint32_t            u32SecKeyLens = 32;

        eExtendDataType     eType;

        u32SecLens = (pu8Params[u32Offset] << 8) | (pu8Params[u32Offset + 1]);
        eType = eExtendDataType(pu8Params[u32Offset + 2]);

        if (strlen((char *)(pu8Params + u32Offset + 3)) <= u32SecKeyLens)
            u32SecKeyLens = strlen((char *)(pu8Params + u32Offset + 3));

        strSecKey = std::string((char *)(pu8Params + u32Offset + 3), u32SecKeyLens);

        if (0 == strKey.compare(strSecKey) && u32SecLens > 35)
        {
            type = eType;
            value.resize(u32SecLens - 35);
            memcpy(value.data(), pu8Params + u32Offset + 35, value.size());
            
            return true;
        }
        u32Offset += u32SecLens;
    }

    return false;
}

bool getExtendedData(uint32_t height, const std::string & strKey, eExtendDataType & type, std::vector<uint8_t>& value, dev::Address const& _owner)
{
    if (height > chainActive.Tip()->nHeight || NULL == chainActive[height])
    {
        return 0;
    }

    bool            find;

    CBlock          block;
    
    dev::Address    invalid;
    
    CBlockIndex *pindex = chainActive[height];

    if (!ReadBlockFromDisk(block, pindex))
        return 0;

    find = false;

    //  Search tx and vout script.
    for (auto tx : block.vtx)
    {
        if (tx.vin.size() > 0)
        {
            CScript     sender;
            CScript     receiver;

            uint256     prevhash;

            CTransaction prevtx;

            if (!GetTransaction(tx.vin[0].prevout.hash, prevtx, prevhash, true))  {
                continue;
            }
            sender = prevtx.vout[tx.vin[0].prevout.n].scriptPubKey;

            CTxDestination txSender;
            txnouttype txType = TX_NONSTANDARD;
            if (ExtractDestination(sender, txSender, &txType))
            {
                if ((txType == TX_PUBKEY || txType == TX_PUBKEYHASH) && txSender.type() == typeid(CKeyID))
                {
                    CKeyID senderAddress(boost::get<CKeyID>(txSender));

                    if ((0 == memcmp(invalid.data(), _owner.data(), dev::Address::size)) ||
                        (0 == memcmp(senderAddress.begin(), _owner.data(), dev::Address::size)))
                    {
                        for (auto txOut : tx.vout)
                        {
                            if (txOut.scriptPubKey.HasOpExtData())
                            {
                                txnouttype whichType = TX_NONSTANDARD;
                                std::vector<std::vector<unsigned char>> vSolutions;

                                receiver = txOut.scriptPubKey;
                                if (Solver(receiver, whichType, vSolutions, true) && (TX_EXT_DATA == whichType) && (vSolutions.size() > 0))
                                {
                                    find = findExtendedKeyData(vSolutions[0], strKey, type, value);
                                    if (find)
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (find)
            break;
    }

    return find;
}
