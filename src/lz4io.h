#ifndef LZ4IO_H
#define LZ4IO_H

#include <vector>

#define LZ4IO_MAGICNUMBER               0x184D2204

int LZ4IO_Compress(const std::string &strDecompress, unsigned int u32Size, std::vector<unsigned char> &vCompress);

int LZ4IO_Decompress(const std::string strCompress, unsigned int u32Size, std::vector<unsigned char> &vDecompress);

#endif // LZ4IO_H
