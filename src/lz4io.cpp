#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lz4.h>
#include <lz4hc.h>
#define LZ4F_STATIC_LINKING_ONLY
#include <lz4frame.h>

#include <vector>
#include <string>

#define MAGICNUMBER_SIZE                4
#define LZ4IO_MAGICNUMBER               0x184D2204
#define LZ4IO_SKIPPABLE0                0x184D2A50
#define LZ4IO_SKIPPABLEMASK             0xFFFFFFF0
#define LEGACY_MAGICNUMBER              0x184C2102

#define LZ4IO_BLOCKSIZEID_DEFAULT       7
#define LZ4IO_BLOCKCHECKSUM_DEFAULT     0
#define LZ4IO_FAVORDECSPEED_DEFAULT     0
#define LZ4IO_STREAMCHECKSUM_DEFAULT    1
#define LZ4IO_BLOCKINDEPENDENCE_DEFAULT 1


static int LZ4IO_GetBlockSizeFromBlockId (int id) { return (1 << (8 + (2 * id))); }

int LZ4IO_Compress(const std::string &strDecompress, unsigned int u32Size, std::vector<unsigned char> &vCompress)
{
    unsigned char               *pu8DstBuffer       = NULL;
    
    int                         s32CompSize         = 0;
    
    size_t                      u32BlockSize        = 0;
    size_t                      u32DstBufferSize    = 0;
    
    LZ4F_errorCode_t            u32ErrorCode        = 0;
    
    LZ4F_preferences_t          tPreferences;
    LZ4F_compressionContext_t   tLz4Context;
    
    vCompress.clear();
    if (0 == u32Size)
    {
        return vCompress.size();
    }
    
    u32BlockSize = LZ4IO_GetBlockSizeFromBlockId (LZ4IO_BLOCKSIZEID_DEFAULT);
    
    u32ErrorCode = LZ4F_createCompressionContext(&tLz4Context, LZ4F_VERSION);
    if (LZ4F_isError(u32ErrorCode))
    {
        return vCompress.size();
    }

    u32DstBufferSize = LZ4F_compressFrameBound(u32BlockSize, NULL);
    pu8DstBuffer = new unsigned char[u32DstBufferSize];
    
    memset(&tPreferences, 0, sizeof(LZ4F_preferences_t));
    
    tPreferences.autoFlush = 1;
    tPreferences.compressionLevel = 1;
    tPreferences.frameInfo.blockMode = (LZ4F_blockMode_t)LZ4IO_BLOCKINDEPENDENCE_DEFAULT;
    tPreferences.frameInfo.blockSizeID = (LZ4F_blockSizeID_t)LZ4IO_BLOCKSIZEID_DEFAULT;
    tPreferences.frameInfo.blockChecksumFlag = (LZ4F_blockChecksum_t)LZ4IO_BLOCKCHECKSUM_DEFAULT;
    tPreferences.frameInfo.contentChecksumFlag = (LZ4F_contentChecksum_t)LZ4IO_STREAMCHECKSUM_DEFAULT;
    tPreferences.favorDecSpeed = LZ4IO_FAVORDECSPEED_DEFAULT;
    
    if (u32Size < u32BlockSize)
    {
        s32CompSize = LZ4F_compressFrame_usingCDict(tLz4Context, pu8DstBuffer, u32DstBufferSize, strDecompress.data(), u32Size, NULL, &tPreferences);
        if (!LZ4F_isError(s32CompSize))
        {
            vCompress.resize(s32CompSize);
            memcpy(vCompress.data(), pu8DstBuffer, s32CompSize);
        }
    }
    else
    {
        int             s32ErrFlag      = 0;
        
        size_t          u32Offset       = 0;
        size_t          u32DataSize     = 0;
        size_t          u32LeftSize     = 0;
        
        s32CompSize = LZ4F_compressBegin_usingCDict(tLz4Context, pu8DstBuffer, u32DstBufferSize, NULL, &tPreferences);
        if (!LZ4F_isError(s32CompSize))
        {
            vCompress.insert(vCompress.end(), pu8DstBuffer, pu8DstBuffer + s32CompSize);
        }
        else
        {
            s32ErrFlag = 1;
        }
        
        u32Offset = 0;
        u32DataSize = u32BlockSize;
        u32LeftSize = u32Size;
        while (u32LeftSize > 0 && !s32ErrFlag)
        {
            s32CompSize = LZ4F_compressUpdate(tLz4Context, pu8DstBuffer, u32DstBufferSize, strDecompress.data() + u32Offset, u32DataSize, NULL);
            if (!LZ4F_isError(s32CompSize)) {
                vCompress.insert(vCompress.end(), pu8DstBuffer, pu8DstBuffer + s32CompSize);
            } else {
                s32ErrFlag = 1;
            }
            
            u32Offset += u32DataSize;
            u32LeftSize -= u32DataSize;
            u32DataSize = (u32LeftSize >= u32BlockSize) ? u32BlockSize : u32LeftSize;
        }
        
        if (!s32ErrFlag)
        {
            s32CompSize = LZ4F_compressEnd(tLz4Context, pu8DstBuffer, u32DstBufferSize, NULL);
            if (!LZ4F_isError(s32CompSize))
            {
                vCompress.insert(vCompress.end(), pu8DstBuffer, pu8DstBuffer + s32CompSize);
            }
            else
            {
                s32ErrFlag = 1;
            }
        }
        
        if (s32ErrFlag)
        {
            vCompress.clear();
        }
    }
    
    delete[] pu8DstBuffer;
    
    return vCompress.size();
}

int LZ4IO_Decompress(const std::string strCompress, unsigned int u32Size, std::vector<unsigned char> &vDecompress)
{
    unsigned char                   *pu8Compress        = NULL;
    unsigned char                   *pu8DstBuffer       = NULL;
    
    unsigned int                    u32MagicNumber      = 0;
    
    size_t                          u32Offset           = 0;
    size_t                          u32DstBufferSize    = 0;
    
    LZ4F_errorCode_t                u32ErrorCode        = 0;
    LZ4F_errorCode_t                u32NextToLoad       = 0;
    
    LZ4F_decompressionContext_t     tLz4Context;
    
    vDecompress.clear();
    if (u32Size <= MAGICNUMBER_SIZE)
    {
        return vDecompress.size();
    }
    pu8Compress = (unsigned char *) strCompress.data();
    
    u32ErrorCode = LZ4F_createDecompressionContext(&tLz4Context, LZ4F_VERSION);
    if (LZ4F_isError(u32ErrorCode))
    {
        return vDecompress.size();
    }
    
    u32DstBufferSize = 64 * 1024;
    pu8DstBuffer = new unsigned char[u32DstBufferSize];
    
    u32MagicNumber  =  pu8Compress[u32Offset + 0];
    u32MagicNumber += (pu8Compress[u32Offset + 1] << 8);
    u32MagicNumber += (pu8Compress[u32Offset + 2] << 16);
    u32MagicNumber += (pu8Compress[u32Offset + 3] << 24);
    
    if ((u32MagicNumber & LZ4IO_SKIPPABLEMASK) == LZ4IO_SKIPPABLE0)
    {
        u32MagicNumber = LZ4IO_SKIPPABLE0;
    }

    if (LZ4IO_MAGICNUMBER == u32MagicNumber)
    {
        int             s32ErrFlag      = 0;
        
        size_t          u32InSize       = 0;
        size_t          u32OutSize      = 0;
        size_t          u32LeftSize     = 0;
        
        u32LeftSize = u32Size;
        u32InSize = MAGICNUMBER_SIZE;
        u32NextToLoad = LZ4F_decompress_usingDict(tLz4Context, pu8DstBuffer, &u32OutSize, pu8Compress + u32Offset, &u32InSize, NULL, 0, NULL);
        if (LZ4F_isError(u32NextToLoad))
        {
            s32ErrFlag = 1;
        }
        u32Offset += MAGICNUMBER_SIZE;
        u32LeftSize -= MAGICNUMBER_SIZE;

        for (; u32NextToLoad && 0 == s32ErrFlag; )
        {
            size_t      u32SrcOffset = 0;
            
            if (u32NextToLoad >= u32LeftSize)
            {
                u32InSize = u32LeftSize;
            }
            else
            {
                u32InSize = u32NextToLoad;
            }
            u32OutSize = u32DstBufferSize;
            
            while (((u32SrcOffset < u32InSize) || (u32OutSize == u32DstBufferSize)) && 0 == s32ErrFlag)
            {
                size_t  u32Remaining = u32InSize - u32SrcOffset;

                u32OutSize = u32DstBufferSize;
                u32NextToLoad = LZ4F_decompress_usingDict(tLz4Context, pu8DstBuffer, &u32OutSize, pu8Compress + u32Offset + u32SrcOffset, &u32Remaining, NULL, 0, NULL);
                if (LZ4F_isError(u32NextToLoad))
                {
                    s32ErrFlag = 1;
                }
                else
                {
                    u32SrcOffset += u32Remaining;
                    
                    if (u32OutSize)
                    {
                        vDecompress.insert(vDecompress.end(), pu8DstBuffer, pu8DstBuffer + u32OutSize);
                    }
                }
                
                if (!u32NextToLoad) break;
            }
            u32Offset += u32InSize;
            u32LeftSize -= u32InSize;
        }
    
        if (s32ErrFlag)
        {
            vDecompress.clear();
        }
    }
    
    delete[] pu8DstBuffer;

    return vDecompress.size();
}

