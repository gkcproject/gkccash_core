/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2016-2017 XMRig       <support@xmrig.com>
 *
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <memory.h>
#include <x86intrin.h>
#include <inttypes.h>

#include "cryptonight.h"


#ifdef __GNUC__
#include <x86intrin.h>
#else
#include <intrin.h>
#define __restrict__ __restrict
#endif

#if defined(__INTEL_COMPILER)
#define ASM __asm__
#elif !defined(_MSC_VER)
#define ASM __asm__
#else
#define ASM __asm
#endif


namespace XMRig_cryptonight {

#define saes_data(w) {\
    w(0x63), w(0x7c), w(0x77), w(0x7b), w(0xf2), w(0x6b), w(0x6f), w(0xc5),\
    w(0x30), w(0x01), w(0x67), w(0x2b), w(0xfe), w(0xd7), w(0xab), w(0x76),\
    w(0xca), w(0x82), w(0xc9), w(0x7d), w(0xfa), w(0x59), w(0x47), w(0xf0),\
    w(0xad), w(0xd4), w(0xa2), w(0xaf), w(0x9c), w(0xa4), w(0x72), w(0xc0),\
    w(0xb7), w(0xfd), w(0x93), w(0x26), w(0x36), w(0x3f), w(0xf7), w(0xcc),\
    w(0x34), w(0xa5), w(0xe5), w(0xf1), w(0x71), w(0xd8), w(0x31), w(0x15),\
    w(0x04), w(0xc7), w(0x23), w(0xc3), w(0x18), w(0x96), w(0x05), w(0x9a),\
    w(0x07), w(0x12), w(0x80), w(0xe2), w(0xeb), w(0x27), w(0xb2), w(0x75),\
    w(0x09), w(0x83), w(0x2c), w(0x1a), w(0x1b), w(0x6e), w(0x5a), w(0xa0),\
    w(0x52), w(0x3b), w(0xd6), w(0xb3), w(0x29), w(0xe3), w(0x2f), w(0x84),\
    w(0x53), w(0xd1), w(0x00), w(0xed), w(0x20), w(0xfc), w(0xb1), w(0x5b),\
    w(0x6a), w(0xcb), w(0xbe), w(0x39), w(0x4a), w(0x4c), w(0x58), w(0xcf),\
    w(0xd0), w(0xef), w(0xaa), w(0xfb), w(0x43), w(0x4d), w(0x33), w(0x85),\
    w(0x45), w(0xf9), w(0x02), w(0x7f), w(0x50), w(0x3c), w(0x9f), w(0xa8),\
    w(0x51), w(0xa3), w(0x40), w(0x8f), w(0x92), w(0x9d), w(0x38), w(0xf5),\
    w(0xbc), w(0xb6), w(0xda), w(0x21), w(0x10), w(0xff), w(0xf3), w(0xd2),\
    w(0xcd), w(0x0c), w(0x13), w(0xec), w(0x5f), w(0x97), w(0x44), w(0x17),\
    w(0xc4), w(0xa7), w(0x7e), w(0x3d), w(0x64), w(0x5d), w(0x19), w(0x73),\
    w(0x60), w(0x81), w(0x4f), w(0xdc), w(0x22), w(0x2a), w(0x90), w(0x88),\
    w(0x46), w(0xee), w(0xb8), w(0x14), w(0xde), w(0x5e), w(0x0b), w(0xdb),\
    w(0xe0), w(0x32), w(0x3a), w(0x0a), w(0x49), w(0x06), w(0x24), w(0x5c),\
    w(0xc2), w(0xd3), w(0xac), w(0x62), w(0x91), w(0x95), w(0xe4), w(0x79),\
    w(0xe7), w(0xc8), w(0x37), w(0x6d), w(0x8d), w(0xd5), w(0x4e), w(0xa9),\
    w(0x6c), w(0x56), w(0xf4), w(0xea), w(0x65), w(0x7a), w(0xae), w(0x08),\
    w(0xba), w(0x78), w(0x25), w(0x2e), w(0x1c), w(0xa6), w(0xb4), w(0xc6),\
    w(0xe8), w(0xdd), w(0x74), w(0x1f), w(0x4b), w(0xbd), w(0x8b), w(0x8a),\
    w(0x70), w(0x3e), w(0xb5), w(0x66), w(0x48), w(0x03), w(0xf6), w(0x0e),\
    w(0x61), w(0x35), w(0x57), w(0xb9), w(0x86), w(0xc1), w(0x1d), w(0x9e),\
    w(0xe1), w(0xf8), w(0x98), w(0x11), w(0x69), w(0xd9), w(0x8e), w(0x94),\
    w(0x9b), w(0x1e), w(0x87), w(0xe9), w(0xce), w(0x55), w(0x28), w(0xdf),\
    w(0x8c), w(0xa1), w(0x89), w(0x0d), w(0xbf), w(0xe6), w(0x42), w(0x68),\
    w(0x41), w(0x99), w(0x2d), w(0x0f), w(0xb0), w(0x54), w(0xbb), w(0x16) }

#define SAES_WPOLY           0x011b

#define saes_b2w(b0, b1, b2, b3) (((uint32_t)(b3) << 24) | \
    ((uint32_t)(b2) << 16) | ((uint32_t)(b1) << 8) | (b0))

#define saes_f2(x)   ((x<<1) ^ (((x>>7) & 1) * SAES_WPOLY))
#define saes_f3(x)   (saes_f2(x) ^ x)
#define saes_h0(x)   (x)

#define saes_u0(p)   saes_b2w(saes_f2(p),          p,          p, saes_f3(p))
#define saes_u1(p)   saes_b2w(saes_f3(p), saes_f2(p),          p,          p)
#define saes_u2(p)   saes_b2w(         p, saes_f3(p), saes_f2(p),          p)
#define saes_u3(p)   saes_b2w(         p,          p, saes_f3(p), saes_f2(p))

alignas(16) const uint32_t saes_table[4][256] = { saes_data(saes_u0), saes_data(saes_u1), saes_data(saes_u2), saes_data(saes_u3) };
alignas(16) const uint8_t  saes_sbox[256] = saes_data(saes_h0);

static inline __m128i soft_aesenc(const uint32_t* in, __m128i key)
{
    const uint32_t x0 = in[0];
    const uint32_t x1 = in[1];
    const uint32_t x2 = in[2];
    const uint32_t x3 = in[3];

    __m128i out = _mm_set_epi32(
        (saes_table[0][x3 & 0xff] ^ saes_table[1][(x0 >> 8) & 0xff] ^ saes_table[2][(x1 >> 16) & 0xff] ^ saes_table[3][x2 >> 24]),
        (saes_table[0][x2 & 0xff] ^ saes_table[1][(x3 >> 8) & 0xff] ^ saes_table[2][(x0 >> 16) & 0xff] ^ saes_table[3][x1 >> 24]),
        (saes_table[0][x1 & 0xff] ^ saes_table[1][(x2 >> 8) & 0xff] ^ saes_table[2][(x3 >> 16) & 0xff] ^ saes_table[3][x0 >> 24]),
        (saes_table[0][x0 & 0xff] ^ saes_table[1][(x1 >> 8) & 0xff] ^ saes_table[2][(x2 >> 16) & 0xff] ^ saes_table[3][x3 >> 24]));

    return _mm_xor_si128(out, key);
}

static inline uint32_t sub_word(uint32_t key)
{
    return (saes_sbox[key >> 24 ] << 24)   | 
        (saes_sbox[(key >> 16) & 0xff] << 16 ) | 
        (saes_sbox[(key >> 8)  & 0xff] << 8  ) | 
         saes_sbox[key & 0xff];
}

#if defined(__clang__) || defined(XMRIG_ARM)
static inline uint32_t _rotr(uint32_t value, uint32_t amount)
{
    return (value >> amount) | (value << ((32 - amount) & 31));
}
#endif

template<uint8_t rcon>
static inline __m128i soft_aeskeygenassist(__m128i key)
{
    const uint32_t X1 = sub_word(_mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0x55)));
    const uint32_t X3 = sub_word(_mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0xFF)));
    return _mm_set_epi32(_rotr(X3, 8) ^ rcon, X3, _rotr(X1, 8) ^ rcon, X1);
}



#ifdef __cplusplus  
extern "C"  {
#endif


#ifndef KECCAK_ROUNDS
#define KECCAK_ROUNDS 24
#endif
    
#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

#define HASH_DATA_AREA 136
    
    
const uint64_t keccakf_rndc[24] = 
{
    0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
    0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
    0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
    0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
    0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 
    0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
    0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};
    
// update the state with given number of rounds

void keccakf(uint64_t st[25], int rounds)
{
    int i, j, round;
    uint64_t t, bc[5];

    for (round = 0; round < rounds; ++round) {

        // Theta
        bc[0] = st[0] ^ st[5] ^ st[10] ^ st[15] ^ st[20];
        bc[1] = st[1] ^ st[6] ^ st[11] ^ st[16] ^ st[21];
        bc[2] = st[2] ^ st[7] ^ st[12] ^ st[17] ^ st[22];
        bc[3] = st[3] ^ st[8] ^ st[13] ^ st[18] ^ st[23];
        bc[4] = st[4] ^ st[9] ^ st[14] ^ st[19] ^ st[24];

        for (i = 0; i < 5; ++i) {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
            st[i     ] ^= t;
            st[i +  5] ^= t;
            st[i + 10] ^= t;
            st[i + 15] ^= t;
            st[i + 20] ^= t;
        }

        // Rho Pi
        t = st[1];
        st[ 1] = ROTL64(st[ 6], 44);
        st[ 6] = ROTL64(st[ 9], 20);
        st[ 9] = ROTL64(st[22], 61);
        st[22] = ROTL64(st[14], 39);
        st[14] = ROTL64(st[20], 18);
        st[20] = ROTL64(st[ 2], 62);
        st[ 2] = ROTL64(st[12], 43);
        st[12] = ROTL64(st[13], 25);
        st[13] = ROTL64(st[19],  8);
        st[19] = ROTL64(st[23], 56);
        st[23] = ROTL64(st[15], 41);
        st[15] = ROTL64(st[ 4], 27);
        st[ 4] = ROTL64(st[24], 14);
        st[24] = ROTL64(st[21],  2);
        st[21] = ROTL64(st[ 8], 55);
        st[ 8] = ROTL64(st[16], 45);
        st[16] = ROTL64(st[ 5], 36);
        st[ 5] = ROTL64(st[ 3], 28);
        st[ 3] = ROTL64(st[18], 21);
        st[18] = ROTL64(st[17], 15);
        st[17] = ROTL64(st[11], 10);
        st[11] = ROTL64(st[ 7],  6);
        st[ 7] = ROTL64(st[10],  3);
        st[10] = ROTL64(t, 1);

        //  Chi
        // unrolled loop, where only last iteration is different
        j = 0;
        bc[0] = st[j    ];
        bc[1] = st[j + 1];

        st[j    ] ^= (~st[j + 1]) & st[j + 2];
        st[j + 1] ^= (~st[j + 2]) & st[j + 3];
        st[j + 2] ^= (~st[j + 3]) & st[j + 4];
        st[j + 3] ^= (~st[j + 4]) & bc[0];
        st[j + 4] ^= (~bc[0]) & bc[1];

        j = 5;
        bc[0] = st[j    ];
        bc[1] = st[j + 1];

        st[j    ] ^= (~st[j + 1]) & st[j + 2];
        st[j + 1] ^= (~st[j + 2]) & st[j + 3];
        st[j + 2] ^= (~st[j + 3]) & st[j + 4];
        st[j + 3] ^= (~st[j + 4]) & bc[0];
        st[j + 4] ^= (~bc[0]) & bc[1];

        j = 10;
        bc[0] = st[j    ];
        bc[1] = st[j + 1];

        st[j    ] ^= (~st[j + 1]) & st[j + 2];
        st[j + 1] ^= (~st[j + 2]) & st[j + 3];
        st[j + 2] ^= (~st[j + 3]) & st[j + 4];
        st[j + 3] ^= (~st[j + 4]) & bc[0];
        st[j + 4] ^= (~bc[0]) & bc[1];

        j = 15;
        bc[0] = st[j    ];
        bc[1] = st[j + 1];

        st[j    ] ^= (~st[j + 1]) & st[j + 2];
        st[j + 1] ^= (~st[j + 2]) & st[j + 3];
        st[j + 2] ^= (~st[j + 3]) & st[j + 4];
        st[j + 3] ^= (~st[j + 4]) & bc[0];
        st[j + 4] ^= (~bc[0]) & bc[1];

        j = 20;
        bc[0] = st[j    ];
        bc[1] = st[j + 1];
        bc[2] = st[j + 2];
        bc[3] = st[j + 3];
        bc[4] = st[j + 4];

        st[j    ] ^= (~bc[1]) & bc[2];
        st[j + 1] ^= (~bc[2]) & bc[3];
        st[j + 2] ^= (~bc[3]) & bc[4];
        st[j + 3] ^= (~bc[4]) & bc[0];
        st[j + 4] ^= (~bc[0]) & bc[1];
        
        //  Iota
        st[0] ^= keccakf_rndc[round];
    }
}

// compute a keccak hash (md) of given byte length from "in"
typedef uint64_t state_t[25];

void keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen)
{
    state_t st;
    uint8_t temp[144];
    int i, rsiz, rsizw;

    rsiz = sizeof(state_t) == mdlen ? HASH_DATA_AREA : 200 - 2 * mdlen;
    rsizw = rsiz / 8;
    
    memset(st, 0, sizeof(st));

    for ( ; inlen >= rsiz; inlen -= rsiz, in += rsiz) {
        for (i = 0; i < rsizw; i++)
            st[i] ^= ((uint64_t *) in)[i];
        keccakf(st, KECCAK_ROUNDS);
    }
    
    // last block and padding
    memcpy(temp, in, inlen);
    temp[inlen++] = 1;
    memset(temp + inlen, 0, rsiz - inlen);
    temp[rsiz - 1] |= 0x80;

    for (i = 0; i < rsizw; i++)
        st[i] ^= ((uint64_t *) temp)[i];

    keccakf(st, KECCAK_ROUNDS);

    memcpy(md, st, mdlen);
}

void keccak1600(const uint8_t *in, int inlen, uint8_t *md)
{
    keccak(in, inlen, md, sizeof(state_t));
}


typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum {SUCCESS = 0, FAIL = 1, BAD_HASHLEN = 2} HashReturn;


/* some sizes (number of bytes) */
#define ROWS 8
#define LENGTHFIELDLEN ROWS
#define COLS512 8
    
#define SIZE512 (ROWS*COLS512)
    
#define ROUNDS512 10
#define HASH_BIT_LEN 256
    
#define ROTL32(v, n) ((((v)<<(n))|((v)>>(32-(n))))&li_32(ffffffff))
    
    
#define li_32(h) 0x##h##u
#define EXT_BYTE(var,n) ((uint8_t)((uint32_t)(var) >> (8*n)))
#define u32BIG(a)				\
        ((ROTL32(a,8) & li_32(00FF00FF)) |        \
        (ROTL32(a,24) & li_32(FF00FF00)))
    
    
/* NIST API begin */
typedef struct {
    uint32_t chaining[SIZE512/sizeof(uint32_t)];            /* actual state */
    uint32_t block_counter1,
    block_counter2;         /* message block counter(s) */
    BitSequence buffer[SIZE512];      /* data buffer */
    int buf_ptr;              /* data buffer pointer */
    int bits_in_last_byte;    /* no. of message bits in last byte of
                   data buffer */
} groestlHashState;

const uint32_t T[512] = {0xa5f432c6, 0xc6a597f4, 0x84976ff8, 0xf884eb97, 0x99b05eee, 0xee99c7b0, 0x8d8c7af6, 0xf68df78c, 0xd17e8ff, 0xff0de517, 0xbddc0ad6, 0xd6bdb7dc, 0xb1c816de, 0xdeb1a7c8, 0x54fc6d91, 0x915439fc
, 0x50f09060, 0x6050c0f0, 0x3050702, 0x2030405, 0xa9e02ece, 0xcea987e0, 0x7d87d156, 0x567dac87, 0x192bcce7, 0xe719d52b, 0x62a613b5, 0xb56271a6, 0xe6317c4d, 0x4de69a31, 0x9ab559ec, 0xec9ac3b5
, 0x45cf408f, 0x8f4505cf, 0x9dbca31f, 0x1f9d3ebc, 0x40c04989, 0x894009c0, 0x879268fa, 0xfa87ef92, 0x153fd0ef, 0xef15c53f, 0xeb2694b2, 0xb2eb7f26, 0xc940ce8e, 0x8ec90740, 0xb1de6fb, 0xfb0bed1d
, 0xec2f6e41, 0x41ec822f, 0x67a91ab3, 0xb3677da9, 0xfd1c435f, 0x5ffdbe1c, 0xea256045, 0x45ea8a25, 0xbfdaf923, 0x23bf46da, 0xf7025153, 0x53f7a602, 0x96a145e4, 0xe496d3a1, 0x5bed769b, 0x9b5b2ded
, 0xc25d2875, 0x75c2ea5d, 0x1c24c5e1, 0xe11cd924, 0xaee9d43d, 0x3dae7ae9, 0x6abef24c, 0x4c6a98be, 0x5aee826c, 0x6c5ad8ee, 0x41c3bd7e, 0x7e41fcc3, 0x206f3f5, 0xf502f106, 0x4fd15283, 0x834f1dd1
, 0x5ce48c68, 0x685cd0e4, 0xf4075651, 0x51f4a207, 0x345c8dd1, 0xd134b95c, 0x818e1f9, 0xf908e918, 0x93ae4ce2, 0xe293dfae, 0x73953eab, 0xab734d95, 0x53f59762, 0x6253c4f5, 0x3f416b2a, 0x2a3f5441
, 0xc141c08, 0x80c1014, 0x52f66395, 0x955231f6, 0x65afe946, 0x46658caf, 0x5ee27f9d, 0x9d5e21e2, 0x28784830, 0x30286078, 0xa1f8cf37, 0x37a16ef8, 0xf111b0a, 0xa0f1411, 0xb5c4eb2f, 0x2fb55ec4
, 0x91b150e, 0xe091c1b, 0x365a7e24, 0x2436485a, 0x9bb6ad1b, 0x1b9b36b6, 0x3d4798df, 0xdf3da547, 0x266aa7cd, 0xcd26816a, 0x69bbf54e, 0x4e699cbb, 0xcd4c337f, 0x7fcdfe4c, 0x9fba50ea, 0xea9fcfba
, 0x1b2d3f12, 0x121b242d, 0x9eb9a41d, 0x1d9e3ab9, 0x749cc458, 0x5874b09c, 0x2e724634, 0x342e6872, 0x2d774136, 0x362d6c77, 0xb2cd11dc, 0xdcb2a3cd, 0xee299db4, 0xb4ee7329, 0xfb164d5b, 0x5bfbb616
, 0xf601a5a4, 0xa4f65301, 0x4dd7a176, 0x764decd7, 0x61a314b7, 0xb76175a3, 0xce49347d, 0x7dcefa49, 0x7b8ddf52, 0x527ba48d, 0x3e429fdd, 0xdd3ea142, 0x7193cd5e, 0x5e71bc93, 0x97a2b113, 0x139726a2
, 0xf504a2a6, 0xa6f55704, 0x68b801b9, 0xb96869b8, 0x0, 0x0, 0x2c74b5c1, 0xc12c9974, 0x60a0e040, 0x406080a0, 0x1f21c2e3, 0xe31fdd21, 0xc8433a79, 0x79c8f243, 0xed2c9ab6, 0xb6ed772c
, 0xbed90dd4, 0xd4beb3d9, 0x46ca478d, 0x8d4601ca, 0xd9701767, 0x67d9ce70, 0x4bddaf72, 0x724be4dd, 0xde79ed94, 0x94de3379, 0xd467ff98, 0x98d42b67, 0xe82393b0, 0xb0e87b23, 0x4ade5b85, 0x854a11de
, 0x6bbd06bb, 0xbb6b6dbd, 0x2a7ebbc5, 0xc52a917e, 0xe5347b4f, 0x4fe59e34, 0x163ad7ed, 0xed16c13a, 0xc554d286, 0x86c51754, 0xd762f89a, 0x9ad72f62, 0x55ff9966, 0x6655ccff, 0x94a7b611, 0x119422a7
, 0xcf4ac08a, 0x8acf0f4a, 0x1030d9e9, 0xe910c930, 0x60a0e04, 0x406080a, 0x819866fe, 0xfe81e798, 0xf00baba0, 0xa0f05b0b, 0x44ccb478, 0x7844f0cc, 0xbad5f025, 0x25ba4ad5, 0xe33e754b, 0x4be3963e
, 0xf30eaca2, 0xa2f35f0e, 0xfe19445d, 0x5dfeba19, 0xc05bdb80, 0x80c01b5b, 0x8a858005, 0x58a0a85, 0xadecd33f, 0x3fad7eec, 0xbcdffe21, 0x21bc42df, 0x48d8a870, 0x7048e0d8, 0x40cfdf1, 0xf104f90c
, 0xdf7a1963, 0x63dfc67a, 0xc1582f77, 0x77c1ee58, 0x759f30af, 0xaf75459f, 0x63a5e742, 0x426384a5, 0x30507020, 0x20304050, 0x1a2ecbe5, 0xe51ad12e, 0xe12effd, 0xfd0ee112, 0x6db708bf, 0xbf6d65b7
, 0x4cd45581, 0x814c19d4, 0x143c2418, 0x1814303c, 0x355f7926, 0x26354c5f, 0x2f71b2c3, 0xc32f9d71, 0xe13886be, 0xbee16738, 0xa2fdc835, 0x35a26afd, 0xcc4fc788, 0x88cc0b4f, 0x394b652e, 0x2e395c4b
, 0x57f96a93, 0x93573df9, 0xf20d5855, 0x55f2aa0d, 0x829d61fc, 0xfc82e39d, 0x47c9b37a, 0x7a47f4c9, 0xacef27c8, 0xc8ac8bef, 0xe73288ba, 0xbae76f32, 0x2b7d4f32, 0x322b647d, 0x95a442e6, 0xe695d7a4
, 0xa0fb3bc0, 0xc0a09bfb, 0x98b3aa19, 0x199832b3, 0xd168f69e, 0x9ed12768, 0x7f8122a3, 0xa37f5d81, 0x66aaee44, 0x446688aa, 0x7e82d654, 0x547ea882, 0xabe6dd3b, 0x3bab76e6, 0x839e950b, 0xb83169e
, 0xca45c98c, 0x8cca0345, 0x297bbcc7, 0xc729957b, 0xd36e056b, 0x6bd3d66e, 0x3c446c28, 0x283c5044, 0x798b2ca7, 0xa779558b, 0xe23d81bc, 0xbce2633d, 0x1d273116, 0x161d2c27, 0x769a37ad, 0xad76419a
, 0x3b4d96db, 0xdb3bad4d, 0x56fa9e64, 0x6456c8fa, 0x4ed2a674, 0x744ee8d2, 0x1e223614, 0x141e2822, 0xdb76e492, 0x92db3f76, 0xa1e120c, 0xc0a181e, 0x6cb4fc48, 0x486c90b4, 0xe4378fb8, 0xb8e46b37
, 0x5de7789f, 0x9f5d25e7, 0x6eb20fbd, 0xbd6e61b2, 0xef2a6943, 0x43ef862a, 0xa6f135c4, 0xc4a693f1, 0xa8e3da39, 0x39a872e3, 0xa4f7c631, 0x31a462f7, 0x37598ad3, 0xd337bd59, 0x8b8674f2, 0xf28bff86
, 0x325683d5, 0xd532b156, 0x43c54e8b, 0x8b430dc5, 0x59eb856e, 0x6e59dceb, 0xb7c218da, 0xdab7afc2, 0x8c8f8e01, 0x18c028f, 0x64ac1db1, 0xb16479ac, 0xd26df19c, 0x9cd2236d, 0xe03b7249, 0x49e0923b
, 0xb4c71fd8, 0xd8b4abc7, 0xfa15b9ac, 0xacfa4315, 0x709faf3, 0xf307fd09, 0x256fa0cf, 0xcf25856f, 0xafea20ca, 0xcaaf8fea, 0x8e897df4, 0xf48ef389, 0xe9206747, 0x47e98e20, 0x18283810, 0x10182028
, 0xd5640b6f, 0x6fd5de64, 0x888373f0, 0xf088fb83, 0x6fb1fb4a, 0x4a6f94b1, 0x7296ca5c, 0x5c72b896, 0x246c5438, 0x3824706c, 0xf1085f57, 0x57f1ae08, 0xc7522173, 0x73c7e652, 0x51f36497, 0x975135f3
, 0x2365aecb, 0xcb238d65, 0x7c8425a1, 0xa17c5984, 0x9cbf57e8, 0xe89ccbbf, 0x21635d3e, 0x3e217c63, 0xdd7cea96, 0x96dd377c, 0xdc7f1e61, 0x61dcc27f, 0x86919c0d, 0xd861a91, 0x85949b0f, 0xf851e94
, 0x90ab4be0, 0xe090dbab, 0x42c6ba7c, 0x7c42f8c6, 0xc4572671, 0x71c4e257, 0xaae529cc, 0xccaa83e5, 0xd873e390, 0x90d83b73, 0x50f0906, 0x6050c0f, 0x103f4f7, 0xf701f503, 0x12362a1c, 0x1c123836
, 0xa3fe3cc2, 0xc2a39ffe, 0x5fe18b6a, 0x6a5fd4e1, 0xf910beae, 0xaef94710, 0xd06b0269, 0x69d0d26b, 0x91a8bf17, 0x17912ea8, 0x58e87199, 0x995829e8, 0x2769533a, 0x3a277469, 0xb9d0f727, 0x27b94ed0
, 0x384891d9, 0xd938a948, 0x1335deeb, 0xeb13cd35, 0xb3cee52b, 0x2bb356ce, 0x33557722, 0x22334455, 0xbbd604d2, 0xd2bbbfd6, 0x709039a9, 0xa9704990, 0x89808707, 0x7890e80, 0xa7f2c133, 0x33a766f2
, 0xb6c1ec2d, 0x2db65ac1, 0x22665a3c, 0x3c227866, 0x92adb815, 0x15922aad, 0x2060a9c9, 0xc9208960, 0x49db5c87, 0x874915db, 0xff1ab0aa, 0xaaff4f1a, 0x7888d850, 0x5078a088, 0x7a8e2ba5, 0xa57a518e
, 0x8f8a8903, 0x38f068a, 0xf8134a59, 0x59f8b213, 0x809b9209, 0x980129b, 0x1739231a, 0x1a173439, 0xda751065, 0x65daca75, 0x315384d7, 0xd731b553, 0xc651d584, 0x84c61351, 0xb8d303d0, 0xd0b8bbd3
, 0xc35edc82, 0x82c31f5e, 0xb0cbe229, 0x29b052cb, 0x7799c35a, 0x5a77b499, 0x11332d1e, 0x1e113c33, 0xcb463d7b, 0x7bcbf646, 0xfc1fb7a8, 0xa8fc4b1f, 0xd6610c6d, 0x6dd6da61, 0x3a4e622c, 0x2c3a584e};

#define P_TYPE 0
#define Q_TYPE 1
    
const uint8_t shift_Values[2][8] = {{0,1,2,3,4,5,6,7},{1,3,5,7,0,2,4,6}};

const uint8_t indices_cyclic[15] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6};


#define ROTATE_COLUMN_DOWN(v1, v2, amount_bytes, temp_var) {temp_var = (v1<<(8*amount_bytes))|(v2>>(8*(4-amount_bytes))); \
                                                            v2 = (v2<<(8*amount_bytes))|(v1>>(8*(4-amount_bytes))); \
                                                            v1 = temp_var;}
  

#define COLUMN(x,y,i,c0,c1,c2,c3,c4,c5,c6,c7,tv1,tv2,tu,tl,t)				\
   tu = T[2*(uint32_t)x[4*c0+0]];               \
   tl = T[2*(uint32_t)x[4*c0+0]+1];         \
   tv1 = T[2*(uint32_t)x[4*c1+1]];          \
   tv2 = T[2*(uint32_t)x[4*c1+1]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,1,t)  \
   tu ^= tv1;                       \
   tl ^= tv2;                       \
   tv1 = T[2*(uint32_t)x[4*c2+2]];          \
   tv2 = T[2*(uint32_t)x[4*c2+2]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,2,t)  \
   tu ^= tv1;                       \
   tl ^= tv2;                       \
   tv1 = T[2*(uint32_t)x[4*c3+3]];          \
   tv2 = T[2*(uint32_t)x[4*c3+3]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,3,t)  \
   tu ^= tv1;                       \
   tl ^= tv2;                       \
   tl ^= T[2*(uint32_t)x[4*c4+0]];          \
   tu ^= T[2*(uint32_t)x[4*c4+0]+1];            \
   tv1 = T[2*(uint32_t)x[4*c5+1]];          \
   tv2 = T[2*(uint32_t)x[4*c5+1]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,1,t)  \
   tl ^= tv1;                       \
   tu ^= tv2;                       \
   tv1 = T[2*(uint32_t)x[4*c6+2]];          \
   tv2 = T[2*(uint32_t)x[4*c6+2]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,2,t)  \
   tl ^= tv1;                       \
   tu ^= tv2;                       \
   tv1 = T[2*(uint32_t)x[4*c7+3]];          \
   tv2 = T[2*(uint32_t)x[4*c7+3]+1];            \
   ROTATE_COLUMN_DOWN(tv1,tv2,3,t)  \
   tl ^= tv1;                       \
   tu ^= tv2;                       \
   y[i] = tu;                       \
   y[i+1] = tl;


/* compute one round of P (short variants) */
static void RND512P(uint8_t *x, uint32_t *y, uint32_t r) {
    uint32_t temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp;
    uint32_t* x32 = (uint32_t*)x;
    x32[ 0] ^= 0x00000000^r;
    x32[ 2] ^= 0x00000010^r;
    x32[ 4] ^= 0x00000020^r;
    x32[ 6] ^= 0x00000030^r;
    x32[ 8] ^= 0x00000040^r;
    x32[10] ^= 0x00000050^r;
    x32[12] ^= 0x00000060^r;
    x32[14] ^= 0x00000070^r;
    COLUMN(x,y, 0,  0,  2,  4,  6,  9, 11, 13, 15, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 2,  2,  4,  6,  8, 11, 13, 15,  1, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 4,  4,  6,  8, 10, 13, 15,  1,  3, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 6,  6,  8, 10, 12, 15,  1,  3,  5, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 8,  8, 10, 12, 14,  1,  3,  5,  7, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,10, 10, 12, 14,  0,  3,  5,  7,  9, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,12, 12, 14,  0,  2,  5,  7,  9, 11, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,14, 14,  0,  2,  4,  7,  9, 11, 13, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
}

/* compute one round of Q (short variants) */
static void RND512Q(uint8_t *x, uint32_t *y, uint32_t r) {
    uint32_t temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp;
    uint32_t* x32 = (uint32_t*)x;
    x32[ 0] = ~x32[ 0];
    x32[ 1] ^= 0xffffffff^r;
    x32[ 2] = ~x32[ 2];
    x32[ 3] ^= 0xefffffff^r;
    x32[ 4] = ~x32[ 4];
    x32[ 5] ^= 0xdfffffff^r;
    x32[ 6] = ~x32[ 6];
    x32[ 7] ^= 0xcfffffff^r;
    x32[ 8] = ~x32[ 8];
    x32[ 9] ^= 0xbfffffff^r;
    x32[10] = ~x32[10];
    x32[11] ^= 0xafffffff^r;
    x32[12] = ~x32[12];
    x32[13] ^= 0x9fffffff^r;
    x32[14] = ~x32[14];
    x32[15] ^= 0x8fffffff^r;
    COLUMN(x,y, 0,  2,  6, 10, 14,  1,  5,  9, 13, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 2,  4,  8, 12,  0,  3,  7, 11, 15, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 4,  6, 10, 14,  2,  5,  9, 13,  1, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 6,  8, 12,  0,  4,  7, 11, 15,  3, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y, 8, 10, 14,  2,  6,  9, 13,  1,  5, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,10, 12,  0,  4,  8, 11, 15,  3,  7, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,12, 14,  2,  6, 10, 13,  1,  5,  9, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
    COLUMN(x,y,14,  0,  4,  8, 12, 15,  3,  7, 11, temp_v1, temp_v2, temp_upper_value, temp_lower_value, temp);
}

/* compute compression function (short variants) */
static void F512(uint32_t *h, const uint32_t *m) {
    int i;
    uint32_t Ptmp[2*COLS512];
    uint32_t Qtmp[2*COLS512];
    uint32_t y[2*COLS512];
    uint32_t z[2*COLS512];

    for (i = 0; i < 2*COLS512; i++) {
        z[i] = m[i];
        Ptmp[i] = h[i]^m[i];
    }

    /* compute Q(m) */
    RND512Q((uint8_t*)z, y, 0x00000000);
    RND512Q((uint8_t*)y, z, 0x01000000);
    RND512Q((uint8_t*)z, y, 0x02000000);
    RND512Q((uint8_t*)y, z, 0x03000000);
    RND512Q((uint8_t*)z, y, 0x04000000);
    RND512Q((uint8_t*)y, z, 0x05000000);
    RND512Q((uint8_t*)z, y, 0x06000000);
    RND512Q((uint8_t*)y, z, 0x07000000);
    RND512Q((uint8_t*)z, y, 0x08000000);
    RND512Q((uint8_t*)y, Qtmp, 0x09000000);

    /* compute P(h+m) */
    RND512P((uint8_t*)Ptmp, y, 0x00000000);
    RND512P((uint8_t*)y, z, 0x00000001);
    RND512P((uint8_t*)z, y, 0x00000002);
    RND512P((uint8_t*)y, z, 0x00000003);
    RND512P((uint8_t*)z, y, 0x00000004);
    RND512P((uint8_t*)y, z, 0x00000005);
    RND512P((uint8_t*)z, y, 0x00000006);
    RND512P((uint8_t*)y, z, 0x00000007);
    RND512P((uint8_t*)z, y, 0x00000008);
    RND512P((uint8_t*)y, Ptmp, 0x00000009);

    /* compute P(h+m) + Q(m) + h */
    for (i = 0; i < 2*COLS512; i++) {
        h[i] ^= Ptmp[i]^Qtmp[i];
    }
}


/* digest up to msglen bytes of input (full blocks only) */
static void Transform(groestlHashState *ctx,
           const uint8_t *input, 
           int msglen) {

    /* digest message, one block at a time */
    for (; msglen >= SIZE512; 
    msglen -= SIZE512, input += SIZE512) {
        F512(ctx->chaining,(uint32_t*)input);

        /* increment block counter */
        ctx->block_counter1++;
        if (ctx->block_counter1 == 0) ctx->block_counter2++;
    }
}

/* given state h, do h <- P(h)+h */
static void OutputTransformation(groestlHashState *ctx) {
    int j;
    uint32_t temp[2*COLS512];
    uint32_t y[2*COLS512];
    uint32_t z[2*COLS512];

    for (j = 0; j < 2*COLS512; j++) {
        temp[j] = ctx->chaining[j];
    }
    RND512P((uint8_t*)temp, y, 0x00000000);
    RND512P((uint8_t*)y, z, 0x00000001);
    RND512P((uint8_t*)z, y, 0x00000002);
    RND512P((uint8_t*)y, z, 0x00000003);
    RND512P((uint8_t*)z, y, 0x00000004);
    RND512P((uint8_t*)y, z, 0x00000005);
    RND512P((uint8_t*)z, y, 0x00000006);
    RND512P((uint8_t*)y, z, 0x00000007);
    RND512P((uint8_t*)z, y, 0x00000008);
    RND512P((uint8_t*)y, temp, 0x00000009);
    for (j = 0; j < 2*COLS512; j++) {
        ctx->chaining[j] ^= temp[j];
    }                                     
}

/* initialise context */
static void GroestlInit(groestlHashState* ctx) {
    int i = 0;
    /* allocate memory for state and data buffer */

    for(;i<(SIZE512/sizeof(uint32_t));i++)
    {
        ctx->chaining[i] = 0;
    }

    /* set initial value */
    ctx->chaining[2*COLS512-1] = u32BIG((uint32_t)HASH_BIT_LEN);

    /* set other variables */
    ctx->buf_ptr = 0;
    ctx->block_counter1 = 0;
    ctx->block_counter2 = 0;
    ctx->bits_in_last_byte = 0;
}

/* update state with databitlen bits of input */
static void GroestlUpdate(groestlHashState* ctx,
          const BitSequence* input,
          DataLength databitlen) {
    int index = 0;
    int msglen = (int)(databitlen/8);
    int rem = (int)(databitlen%8);

    /* if the buffer contains data that has not yet been digested, first
     add data to buffer until full */
    if (ctx->buf_ptr) {
        while (ctx->buf_ptr < SIZE512 && index < msglen) {
            ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
        }
        if (ctx->buf_ptr < SIZE512) {
            /* buffer still not full, return */
            if (rem) {
                ctx->bits_in_last_byte = rem;
                ctx->buffer[(int)ctx->buf_ptr++] = input[index];
            }
            return;
        }

        /* digest buffer */
        ctx->buf_ptr = 0;
        Transform(ctx, ctx->buffer, SIZE512);
    }

    /* digest bulk of message */
    Transform(ctx, input+index, msglen-index);
    index += ((msglen-index)/SIZE512)*SIZE512;

    /* store remaining data in buffer */
    while (index < msglen) {
        ctx->buffer[(int)ctx->buf_ptr++] = input[index++];
    }

    /* if non-integral number of bytes have been supplied, store
    remaining bits in last byte, together with information about
    number of bits */
    if (rem) {
        ctx->bits_in_last_byte = rem;
        ctx->buffer[(int)ctx->buf_ptr++] = input[index];
    }
}

#define BILB ctx->bits_in_last_byte

/* finalise: process remaining data (including padding), perform
   output transformation, and write hash result to 'output' */
static void GroestlFinal(groestlHashState* ctx,
         BitSequence* output) {
    int i, j = 0, hashbytelen = HASH_BIT_LEN/8;
    uint8_t *s = (BitSequence*)ctx->chaining;

    /* pad with '1'-bit and first few '0'-bits */
    if (BILB) {
        ctx->buffer[(int)ctx->buf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
        ctx->buffer[(int)ctx->buf_ptr-1] ^= 0x1<<(7-BILB);
        BILB = 0;
    }
    else ctx->buffer[(int)ctx->buf_ptr++] = 0x80;

    /* pad with '0'-bits */
    if (ctx->buf_ptr > SIZE512-LENGTHFIELDLEN) {
        /* padding requires two blocks */
        while (ctx->buf_ptr < SIZE512) {
            ctx->buffer[(int)ctx->buf_ptr++] = 0;
        }
        /* digest first padding block */
        Transform(ctx, ctx->buffer, SIZE512);
        ctx->buf_ptr = 0;
    }
    while (ctx->buf_ptr < SIZE512-LENGTHFIELDLEN) {
        ctx->buffer[(int)ctx->buf_ptr++] = 0;
    }

    /* length padding */
    ctx->block_counter1++;
    if (ctx->block_counter1 == 0) ctx->block_counter2++;
    ctx->buf_ptr = SIZE512;

    while (ctx->buf_ptr > SIZE512-(int)sizeof(uint32_t)) {
        ctx->buffer[(int)--ctx->buf_ptr] = (uint8_t)ctx->block_counter1;
        ctx->block_counter1 >>= 8;
    }
    while (ctx->buf_ptr > SIZE512-LENGTHFIELDLEN) {
        ctx->buffer[(int)--ctx->buf_ptr] = (uint8_t)ctx->block_counter2;
        ctx->block_counter2 >>= 8;
    }
    /* digest final padding block */
    Transform(ctx, ctx->buffer, SIZE512); 
    /* perform output transformation */
    OutputTransformation(ctx);

    /* store hash result in output */
    for (i = SIZE512-hashbytelen; i < SIZE512; i++,j++) {
        output[j] = s[i];
    }

    /* zeroise relevant variables and deallocate memory */
    for (i = 0; i < COLS512; i++) {
        ctx->chaining[i] = 0;
    }
    for (i = 0; i < SIZE512; i++) {
        ctx->buffer[i] = 0;
    }
}

/* hash bit sequence */
void groestl(const BitSequence* data, 
        DataLength databitlen,
        BitSequence* hashval) {

    groestlHashState context;

    /* initialise */
    GroestlInit(&context);


    /* process message */
    GroestlUpdate(&context, data, databitlen);

    /* finalise */
    GroestlFinal(&context, hashval);
}




typedef struct {
    uint32_t h[8], s[4], t[2];
    int buflen, nullt;
    uint8_t buf[64];
} state;

typedef struct {
    state inner;
    state outer;
} hmac_state;

#define U8TO32(p) \
    (((uint32_t)((p)[0]) << 24) | ((uint32_t)((p)[1]) << 16) |    \
     ((uint32_t)((p)[2]) <<  8) | ((uint32_t)((p)[3])      ))
#define U32TO8(p, v) \
    (p)[0] = (uint8_t)((v) >> 24); (p)[1] = (uint8_t)((v) >> 16); \
    (p)[2] = (uint8_t)((v) >>  8); (p)[3] = (uint8_t)((v)      );

const uint8_t sigma[][16] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
    {14,10, 4, 8, 9,15,13, 6, 1,12, 0, 2,11, 7, 5, 3},
    {11, 8,12, 0, 5, 2,15,13,10,14, 3, 6, 7, 1, 9, 4},
    { 7, 9, 3, 1,13,12,11,14, 2, 6, 5,10, 4, 0,15, 8},
    { 9, 0, 5, 7, 2, 4,10,15,14, 1,11,12, 6, 8, 3,13},
    { 2,12, 6,10, 0,11, 8, 3, 4,13, 7, 5,15,14, 1, 9},
    {12, 5, 1,15,14,13, 4,10, 0, 7, 6, 3, 9, 2, 8,11},
    {13,11, 7,14,12, 1, 3, 9, 5, 0,15, 4, 8, 6, 2,10},
    { 6,15,14, 9,11, 3, 0, 8,12, 2,13, 7, 1, 4,10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5,15,11, 9,14, 3,12,13, 0},
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
    {14,10, 4, 8, 9,15,13, 6, 1,12, 0, 2,11, 7, 5, 3},
    {11, 8,12, 0, 5, 2,15,13,10,14, 3, 6, 7, 1, 9, 4},
    { 7, 9, 3, 1,13,12,11,14, 2, 6, 5,10, 4, 0,15, 8}
};

const uint32_t cst[16] = {
    0x243F6A88, 0x85A308D3, 0x13198A2E, 0x03707344,
    0xA4093822, 0x299F31D0, 0x082EFA98, 0xEC4E6C89,
    0x452821E6, 0x38D01377, 0xBE5466CF, 0x34E90C6C,
    0xC0AC29B7, 0xC97C50DD, 0x3F84D5B5, 0xB5470917
};

static const uint8_t padding[] = {
    0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


void blake256_compress(state *S, const uint8_t *block) {
    uint32_t v[16], m[16], i;

#define ROT(x,n) (((x)<<(32-n))|((x)>>(n)))
#define G(a,b,c,d,e)                                      \
    v[a] += (m[sigma[i][e]] ^ cst[sigma[i][e+1]]) + v[b]; \
    v[d] = ROT(v[d] ^ v[a],16);                           \
    v[c] += v[d];                                         \
    v[b] = ROT(v[b] ^ v[c],12);                           \
    v[a] += (m[sigma[i][e+1]] ^ cst[sigma[i][e]])+v[b];   \
    v[d] = ROT(v[d] ^ v[a], 8);                           \
    v[c] += v[d];                                         \
    v[b] = ROT(v[b] ^ v[c], 7);

    for (i = 0; i < 16; ++i) m[i] = U8TO32(block + i * 4);
    for (i = 0; i < 8;  ++i) v[i] = S->h[i];
    v[ 8] = S->s[0] ^ 0x243F6A88;
    v[ 9] = S->s[1] ^ 0x85A308D3;
    v[10] = S->s[2] ^ 0x13198A2E;
    v[11] = S->s[3] ^ 0x03707344;
    v[12] = 0xA4093822;
    v[13] = 0x299F31D0;
    v[14] = 0x082EFA98;
    v[15] = 0xEC4E6C89;

    if (S->nullt == 0) {
        v[12] ^= S->t[0];
        v[13] ^= S->t[0];
        v[14] ^= S->t[1];
        v[15] ^= S->t[1];
    }

    for (i = 0; i < 14; ++i) {
        G(0, 4,  8, 12,  0);
        G(1, 5,  9, 13,  2);
        G(2, 6, 10, 14,  4);
        G(3, 7, 11, 15,  6);
        G(3, 4,  9, 14, 14);
        G(2, 7,  8, 13, 12);
        G(0, 5, 10, 15,  8);
        G(1, 6, 11, 12, 10);
    }

    for (i = 0; i < 16; ++i) S->h[i % 8] ^= v[i];
    for (i = 0; i < 8;  ++i) S->h[i] ^= S->s[i % 4];
}

void blake256_init(state *S) {
    S->h[0] = 0x6A09E667;
    S->h[1] = 0xBB67AE85;
    S->h[2] = 0x3C6EF372;
    S->h[3] = 0xA54FF53A;
    S->h[4] = 0x510E527F;
    S->h[5] = 0x9B05688C;
    S->h[6] = 0x1F83D9AB;
    S->h[7] = 0x5BE0CD19;
    S->t[0] = S->t[1] = S->buflen = S->nullt = 0;
    S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0;
}

void blake224_init(state *S) {
    S->h[0] = 0xC1059ED8;
    S->h[1] = 0x367CD507;
    S->h[2] = 0x3070DD17;
    S->h[3] = 0xF70E5939;
    S->h[4] = 0xFFC00B31;
    S->h[5] = 0x68581511;
    S->h[6] = 0x64F98FA7;
    S->h[7] = 0xBEFA4FA4;
    S->t[0] = S->t[1] = S->buflen = S->nullt = 0;
    S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0;
}

// datalen = number of bits
void blake256_update(state *S, const uint8_t *data, uint64_t datalen) {
    int left = S->buflen >> 3;
    int fill = 64 - left;

    if (left && (((datalen >> 3) & 0x3F) >= (unsigned) fill)) {
        memcpy((void *) (S->buf + left), (void *) data, fill);
        S->t[0] += 512;
        if (S->t[0] == 0) S->t[1]++;
        blake256_compress(S, S->buf);
        data += fill;
        datalen -= (fill << 3);
        left = 0;
    }

    while (datalen >= 512) {
        S->t[0] += 512;
        if (S->t[0] == 0) S->t[1]++;
        blake256_compress(S, data);
        data += 64;
        datalen -= 512;
    }

    if (datalen > 0) {
        memcpy((void *) (S->buf + left), (void *) data, datalen >> 3);
        S->buflen = (left << 3) + (int) datalen;
    } else {
        S->buflen = 0;
    }
}

// datalen = number of bits
void blake224_update(state *S, const uint8_t *data, uint64_t datalen) {
    blake256_update(S, data, datalen);
}

void blake256_final_h(state *S, uint8_t *digest, uint8_t pa, uint8_t pb) {
    uint8_t msglen[8];
    uint32_t lo = S->t[0] + S->buflen, hi = S->t[1];
    if (lo < (unsigned) S->buflen) hi++;
    U32TO8(msglen + 0, hi);
    U32TO8(msglen + 4, lo);

    if (S->buflen == 440) { /* one padding byte */
        S->t[0] -= 8;
        blake256_update(S, &pa, 8);
    } else {
        if (S->buflen < 440) { /* enough space to fill the block  */
            if (S->buflen == 0) S->nullt = 1;
            S->t[0] -= 440 - S->buflen;
            blake256_update(S, padding, 440 - S->buflen);
        } else { /* need 2 compressions */
            S->t[0] -= 512 - S->buflen;
            blake256_update(S, padding, 512 - S->buflen);
            S->t[0] -= 440;
            blake256_update(S, padding + 1, 440);
            S->nullt = 1;
        }
        blake256_update(S, &pb, 8);
        S->t[0] -= 8;
    }
    S->t[0] -= 64;
    blake256_update(S, msglen, 64);

    U32TO8(digest +  0, S->h[0]);
    U32TO8(digest +  4, S->h[1]);
    U32TO8(digest +  8, S->h[2]);
    U32TO8(digest + 12, S->h[3]);
    U32TO8(digest + 16, S->h[4]);
    U32TO8(digest + 20, S->h[5]);
    U32TO8(digest + 24, S->h[6]);
    U32TO8(digest + 28, S->h[7]);
}

void blake256_final(state *S, uint8_t *digest) {
    blake256_final_h(S, digest, 0x81, 0x01);
}

void blake224_final(state *S, uint8_t *digest) {
    blake256_final_h(S, digest, 0x80, 0x00);
}

// inlen = number of bytes
void blake256_hash(uint8_t *out, const uint8_t *in, uint64_t inlen) {
    state S;
    blake256_init(&S);
    blake256_update(&S, in, inlen * 8);
    blake256_final(&S, out);
}

// inlen = number of bytes
void blake224_hash(uint8_t *out, const uint8_t *in, uint64_t inlen) {
    state S;
    blake224_init(&S);
    blake224_update(&S, in, inlen * 8);
    blake224_final(&S, out);
}

// keylen = number of bytes
void hmac_blake256_init(hmac_state *S, const uint8_t *_key, uint64_t keylen) {
    const uint8_t *key = _key;
    uint8_t keyhash[32];
    uint8_t pad[64];
    uint64_t i;

    if (keylen > 64) {
        blake256_hash(keyhash, key, keylen);
        key = keyhash;
        keylen = 32;
    }

    blake256_init(&S->inner);
    memset(pad, 0x36, 64);
    for (i = 0; i < keylen; ++i) {
        pad[i] ^= key[i];
    }
    blake256_update(&S->inner, pad, 512);

    blake256_init(&S->outer);
    memset(pad, 0x5c, 64);
    for (i = 0; i < keylen; ++i) {
        pad[i] ^= key[i];
    }
    blake256_update(&S->outer, pad, 512);

    memset(keyhash, 0, 32);
}

// keylen = number of bytes
void hmac_blake224_init(hmac_state *S, const uint8_t *_key, uint64_t keylen) {
    const uint8_t *key = _key;
    uint8_t keyhash[32];
    uint8_t pad[64];
    uint64_t i;

    if (keylen > 64) {
        blake256_hash(keyhash, key, keylen);
        key = keyhash;
        keylen = 28;
    }

    blake224_init(&S->inner);
    memset(pad, 0x36, 64);
    for (i = 0; i < keylen; ++i) {
        pad[i] ^= key[i];
    }
    blake224_update(&S->inner, pad, 512);

    blake224_init(&S->outer);
    memset(pad, 0x5c, 64);
    for (i = 0; i < keylen; ++i) {
        pad[i] ^= key[i];
    }
    blake224_update(&S->outer, pad, 512);

    memset(keyhash, 0, 32);
}

// datalen = number of bits
void hmac_blake256_update(hmac_state *S, const uint8_t *data, uint64_t datalen) {
    // update the inner state
    blake256_update(&S->inner, data, datalen);
}

// datalen = number of bits
void hmac_blake224_update(hmac_state *S, const uint8_t *data, uint64_t datalen) {
    // update the inner state
    blake224_update(&S->inner, data, datalen);
}

void hmac_blake256_final(hmac_state *S, uint8_t *digest) {
    uint8_t ihash[32];
    blake256_final(&S->inner, ihash);
    blake256_update(&S->outer, ihash, 256);
    blake256_final(&S->outer, digest);
    memset(ihash, 0, 32);
}

void hmac_blake224_final(hmac_state *S, uint8_t *digest) {
    uint8_t ihash[32];
    blake224_final(&S->inner, ihash);
    blake224_update(&S->outer, ihash, 224);
    blake224_final(&S->outer, digest);
    memset(ihash, 0, 32);
}

// keylen = number of bytes; inlen = number of bytes
void hmac_blake256_hash(uint8_t *out, const uint8_t *key, uint64_t keylen, const uint8_t *in, uint64_t inlen) {
    hmac_state S;
    hmac_blake256_init(&S, key, keylen);
    hmac_blake256_update(&S, in, inlen * 8);
    hmac_blake256_final(&S, out);
}

// keylen = number of bytes; inlen = number of bytes
void hmac_blake224_hash(uint8_t *out, const uint8_t *key, uint64_t keylen, const uint8_t *in, uint64_t inlen) {
    hmac_state S;
    hmac_blake224_init(&S, key, keylen);
    hmac_blake224_update(&S, in, inlen * 8);
    hmac_blake224_final(&S, out);
}





/*typedef unsigned long long uint64;*/
typedef uint64_t uint64;

/*define data alignment for different C compilers*/
#if defined(__GNUC__)
#define DATA_ALIGN16(x) x __attribute__ ((aligned(16)))
#else
#define DATA_ALIGN16(x) __declspec(align(16)) x
#endif


typedef struct {
    int hashbitlen;                   /*the message digest size*/
    unsigned long long databitlen;    /*the message size in bits*/
    unsigned long long datasize_in_buffer;      /*the size of the message remained in buffer; assumed to be multiple of 8bits except for the last partial block at the end of the message*/
    DATA_ALIGN16(uint64 x[8][2]);     /*the 1024-bit state, ( x[i][0] || x[i][1] ) is the ith row of the state in the pseudocode*/
    unsigned char buffer[64];         /*the 512-bit message block to be hashed;*/
} hashState;


/*The initial hash value H(0)*/
const unsigned char JH224_H0[128]={0x2d,0xfe,0xdd,0x62,0xf9,0x9a,0x98,0xac,0xae,0x7c,0xac,0xd6,0x19,0xd6,0x34,0xe7,0xa4,0x83,0x10,0x5,0xbc,0x30,0x12,0x16,0xb8,0x60,0x38,0xc6,0xc9,0x66,0x14,0x94,0x66,0xd9,0x89,0x9f,0x25,0x80,0x70,0x6f,0xce,0x9e,0xa3,0x1b,0x1d,0x9b,0x1a,0xdc,0x11,0xe8,0x32,0x5f,0x7b,0x36,0x6e,0x10,0xf9,0x94,0x85,0x7f,0x2,0xfa,0x6,0xc1,0x1b,0x4f,0x1b,0x5c,0xd8,0xc8,0x40,0xb3,0x97,0xf6,0xa1,0x7f,0x6e,0x73,0x80,0x99,0xdc,0xdf,0x93,0xa5,0xad,0xea,0xa3,0xd3,0xa4,0x31,0xe8,0xde,0xc9,0x53,0x9a,0x68,0x22,0xb4,0xa9,0x8a,0xec,0x86,0xa1,0xe4,0xd5,0x74,0xac,0x95,0x9c,0xe5,0x6c,0xf0,0x15,0x96,0xd,0xea,0xb5,0xab,0x2b,0xbf,0x96,0x11,0xdc,0xf0,0xdd,0x64,0xea,0x6e};
const unsigned char JH256_H0[128]={0xeb,0x98,0xa3,0x41,0x2c,0x20,0xd3,0xeb,0x92,0xcd,0xbe,0x7b,0x9c,0xb2,0x45,0xc1,0x1c,0x93,0x51,0x91,0x60,0xd4,0xc7,0xfa,0x26,0x0,0x82,0xd6,0x7e,0x50,0x8a,0x3,0xa4,0x23,0x9e,0x26,0x77,0x26,0xb9,0x45,0xe0,0xfb,0x1a,0x48,0xd4,0x1a,0x94,0x77,0xcd,0xb5,0xab,0x26,0x2,0x6b,0x17,0x7a,0x56,0xf0,0x24,0x42,0xf,0xff,0x2f,0xa8,0x71,0xa3,0x96,0x89,0x7f,0x2e,0x4d,0x75,0x1d,0x14,0x49,0x8,0xf7,0x7d,0xe2,0x62,0x27,0x76,0x95,0xf7,0x76,0x24,0x8f,0x94,0x87,0xd5,0xb6,0x57,0x47,0x80,0x29,0x6c,0x5c,0x5e,0x27,0x2d,0xac,0x8e,0xd,0x6c,0x51,0x84,0x50,0xc6,0x57,0x5,0x7a,0xf,0x7b,0xe4,0xd3,0x67,0x70,0x24,0x12,0xea,0x89,0xe3,0xab,0x13,0xd3,0x1c,0xd7,0x69};
const unsigned char JH384_H0[128]={0x48,0x1e,0x3b,0xc6,0xd8,0x13,0x39,0x8a,0x6d,0x3b,0x5e,0x89,0x4a,0xde,0x87,0x9b,0x63,0xfa,0xea,0x68,0xd4,0x80,0xad,0x2e,0x33,0x2c,0xcb,0x21,0x48,0xf,0x82,0x67,0x98,0xae,0xc8,0x4d,0x90,0x82,0xb9,0x28,0xd4,0x55,0xea,0x30,0x41,0x11,0x42,0x49,0x36,0xf5,0x55,0xb2,0x92,0x48,0x47,0xec,0xc7,0x25,0xa,0x93,0xba,0xf4,0x3c,0xe1,0x56,0x9b,0x7f,0x8a,0x27,0xdb,0x45,0x4c,0x9e,0xfc,0xbd,0x49,0x63,0x97,0xaf,0xe,0x58,0x9f,0xc2,0x7d,0x26,0xaa,0x80,0xcd,0x80,0xc0,0x8b,0x8c,0x9d,0xeb,0x2e,0xda,0x8a,0x79,0x81,0xe8,0xf8,0xd5,0x37,0x3a,0xf4,0x39,0x67,0xad,0xdd,0xd1,0x7a,0x71,0xa9,0xb4,0xd3,0xbd,0xa4,0x75,0xd3,0x94,0x97,0x6c,0x3f,0xba,0x98,0x42,0x73,0x7f};
const unsigned char JH512_H0[128]={0x6f,0xd1,0x4b,0x96,0x3e,0x0,0xaa,0x17,0x63,0x6a,0x2e,0x5,0x7a,0x15,0xd5,0x43,0x8a,0x22,0x5e,0x8d,0xc,0x97,0xef,0xb,0xe9,0x34,0x12,0x59,0xf2,0xb3,0xc3,0x61,0x89,0x1d,0xa0,0xc1,0x53,0x6f,0x80,0x1e,0x2a,0xa9,0x5,0x6b,0xea,0x2b,0x6d,0x80,0x58,0x8e,0xcc,0xdb,0x20,0x75,0xba,0xa6,0xa9,0xf,0x3a,0x76,0xba,0xf8,0x3b,0xf7,0x1,0x69,0xe6,0x5,0x41,0xe3,0x4a,0x69,0x46,0xb5,0x8a,0x8e,0x2e,0x6f,0xe6,0x5a,0x10,0x47,0xa7,0xd0,0xc1,0x84,0x3c,0x24,0x3b,0x6e,0x71,0xb1,0x2d,0x5a,0xc1,0x99,0xcf,0x57,0xf6,0xec,0x9d,0xb1,0xf8,0x56,0xa7,0x6,0x88,0x7c,0x57,0x16,0xb1,0x56,0xe3,0xc2,0xfc,0xdf,0xe6,0x85,0x17,0xfb,0x54,0x5a,0x46,0x78,0xcc,0x8c,0xdd,0x4b};

/*42 round constants, each round constant is 32-byte (256-bit)*/
const unsigned char E8_bitslice_roundconstant[42][32]={
{0x72,0xd5,0xde,0xa2,0xdf,0x15,0xf8,0x67,0x7b,0x84,0x15,0xa,0xb7,0x23,0x15,0x57,0x81,0xab,0xd6,0x90,0x4d,0x5a,0x87,0xf6,0x4e,0x9f,0x4f,0xc5,0xc3,0xd1,0x2b,0x40},
{0xea,0x98,0x3a,0xe0,0x5c,0x45,0xfa,0x9c,0x3,0xc5,0xd2,0x99,0x66,0xb2,0x99,0x9a,0x66,0x2,0x96,0xb4,0xf2,0xbb,0x53,0x8a,0xb5,0x56,0x14,0x1a,0x88,0xdb,0xa2,0x31},
{0x3,0xa3,0x5a,0x5c,0x9a,0x19,0xe,0xdb,0x40,0x3f,0xb2,0xa,0x87,0xc1,0x44,0x10,0x1c,0x5,0x19,0x80,0x84,0x9e,0x95,0x1d,0x6f,0x33,0xeb,0xad,0x5e,0xe7,0xcd,0xdc},
{0x10,0xba,0x13,0x92,0x2,0xbf,0x6b,0x41,0xdc,0x78,0x65,0x15,0xf7,0xbb,0x27,0xd0,0xa,0x2c,0x81,0x39,0x37,0xaa,0x78,0x50,0x3f,0x1a,0xbf,0xd2,0x41,0x0,0x91,0xd3},
{0x42,0x2d,0x5a,0xd,0xf6,0xcc,0x7e,0x90,0xdd,0x62,0x9f,0x9c,0x92,0xc0,0x97,0xce,0x18,0x5c,0xa7,0xb,0xc7,0x2b,0x44,0xac,0xd1,0xdf,0x65,0xd6,0x63,0xc6,0xfc,0x23},
{0x97,0x6e,0x6c,0x3,0x9e,0xe0,0xb8,0x1a,0x21,0x5,0x45,0x7e,0x44,0x6c,0xec,0xa8,0xee,0xf1,0x3,0xbb,0x5d,0x8e,0x61,0xfa,0xfd,0x96,0x97,0xb2,0x94,0x83,0x81,0x97},
{0x4a,0x8e,0x85,0x37,0xdb,0x3,0x30,0x2f,0x2a,0x67,0x8d,0x2d,0xfb,0x9f,0x6a,0x95,0x8a,0xfe,0x73,0x81,0xf8,0xb8,0x69,0x6c,0x8a,0xc7,0x72,0x46,0xc0,0x7f,0x42,0x14},
{0xc5,0xf4,0x15,0x8f,0xbd,0xc7,0x5e,0xc4,0x75,0x44,0x6f,0xa7,0x8f,0x11,0xbb,0x80,0x52,0xde,0x75,0xb7,0xae,0xe4,0x88,0xbc,0x82,0xb8,0x0,0x1e,0x98,0xa6,0xa3,0xf4},
{0x8e,0xf4,0x8f,0x33,0xa9,0xa3,0x63,0x15,0xaa,0x5f,0x56,0x24,0xd5,0xb7,0xf9,0x89,0xb6,0xf1,0xed,0x20,0x7c,0x5a,0xe0,0xfd,0x36,0xca,0xe9,0x5a,0x6,0x42,0x2c,0x36},
{0xce,0x29,0x35,0x43,0x4e,0xfe,0x98,0x3d,0x53,0x3a,0xf9,0x74,0x73,0x9a,0x4b,0xa7,0xd0,0xf5,0x1f,0x59,0x6f,0x4e,0x81,0x86,0xe,0x9d,0xad,0x81,0xaf,0xd8,0x5a,0x9f},
{0xa7,0x5,0x6,0x67,0xee,0x34,0x62,0x6a,0x8b,0xb,0x28,0xbe,0x6e,0xb9,0x17,0x27,0x47,0x74,0x7,0x26,0xc6,0x80,0x10,0x3f,0xe0,0xa0,0x7e,0x6f,0xc6,0x7e,0x48,0x7b},
{0xd,0x55,0xa,0xa5,0x4a,0xf8,0xa4,0xc0,0x91,0xe3,0xe7,0x9f,0x97,0x8e,0xf1,0x9e,0x86,0x76,0x72,0x81,0x50,0x60,0x8d,0xd4,0x7e,0x9e,0x5a,0x41,0xf3,0xe5,0xb0,0x62},
{0xfc,0x9f,0x1f,0xec,0x40,0x54,0x20,0x7a,0xe3,0xe4,0x1a,0x0,0xce,0xf4,0xc9,0x84,0x4f,0xd7,0x94,0xf5,0x9d,0xfa,0x95,0xd8,0x55,0x2e,0x7e,0x11,0x24,0xc3,0x54,0xa5},
{0x5b,0xdf,0x72,0x28,0xbd,0xfe,0x6e,0x28,0x78,0xf5,0x7f,0xe2,0xf,0xa5,0xc4,0xb2,0x5,0x89,0x7c,0xef,0xee,0x49,0xd3,0x2e,0x44,0x7e,0x93,0x85,0xeb,0x28,0x59,0x7f},
{0x70,0x5f,0x69,0x37,0xb3,0x24,0x31,0x4a,0x5e,0x86,0x28,0xf1,0x1d,0xd6,0xe4,0x65,0xc7,0x1b,0x77,0x4,0x51,0xb9,0x20,0xe7,0x74,0xfe,0x43,0xe8,0x23,0xd4,0x87,0x8a},
{0x7d,0x29,0xe8,0xa3,0x92,0x76,0x94,0xf2,0xdd,0xcb,0x7a,0x9,0x9b,0x30,0xd9,0xc1,0x1d,0x1b,0x30,0xfb,0x5b,0xdc,0x1b,0xe0,0xda,0x24,0x49,0x4f,0xf2,0x9c,0x82,0xbf},
{0xa4,0xe7,0xba,0x31,0xb4,0x70,0xbf,0xff,0xd,0x32,0x44,0x5,0xde,0xf8,0xbc,0x48,0x3b,0xae,0xfc,0x32,0x53,0xbb,0xd3,0x39,0x45,0x9f,0xc3,0xc1,0xe0,0x29,0x8b,0xa0},
{0xe5,0xc9,0x5,0xfd,0xf7,0xae,0x9,0xf,0x94,0x70,0x34,0x12,0x42,0x90,0xf1,0x34,0xa2,0x71,0xb7,0x1,0xe3,0x44,0xed,0x95,0xe9,0x3b,0x8e,0x36,0x4f,0x2f,0x98,0x4a},
{0x88,0x40,0x1d,0x63,0xa0,0x6c,0xf6,0x15,0x47,0xc1,0x44,0x4b,0x87,0x52,0xaf,0xff,0x7e,0xbb,0x4a,0xf1,0xe2,0xa,0xc6,0x30,0x46,0x70,0xb6,0xc5,0xcc,0x6e,0x8c,0xe6},
{0xa4,0xd5,0xa4,0x56,0xbd,0x4f,0xca,0x0,0xda,0x9d,0x84,0x4b,0xc8,0x3e,0x18,0xae,0x73,0x57,0xce,0x45,0x30,0x64,0xd1,0xad,0xe8,0xa6,0xce,0x68,0x14,0x5c,0x25,0x67},
{0xa3,0xda,0x8c,0xf2,0xcb,0xe,0xe1,0x16,0x33,0xe9,0x6,0x58,0x9a,0x94,0x99,0x9a,0x1f,0x60,0xb2,0x20,0xc2,0x6f,0x84,0x7b,0xd1,0xce,0xac,0x7f,0xa0,0xd1,0x85,0x18},
{0x32,0x59,0x5b,0xa1,0x8d,0xdd,0x19,0xd3,0x50,0x9a,0x1c,0xc0,0xaa,0xa5,0xb4,0x46,0x9f,0x3d,0x63,0x67,0xe4,0x4,0x6b,0xba,0xf6,0xca,0x19,0xab,0xb,0x56,0xee,0x7e},
{0x1f,0xb1,0x79,0xea,0xa9,0x28,0x21,0x74,0xe9,0xbd,0xf7,0x35,0x3b,0x36,0x51,0xee,0x1d,0x57,0xac,0x5a,0x75,0x50,0xd3,0x76,0x3a,0x46,0xc2,0xfe,0xa3,0x7d,0x70,0x1},
{0xf7,0x35,0xc1,0xaf,0x98,0xa4,0xd8,0x42,0x78,0xed,0xec,0x20,0x9e,0x6b,0x67,0x79,0x41,0x83,0x63,0x15,0xea,0x3a,0xdb,0xa8,0xfa,0xc3,0x3b,0x4d,0x32,0x83,0x2c,0x83},
{0xa7,0x40,0x3b,0x1f,0x1c,0x27,0x47,0xf3,0x59,0x40,0xf0,0x34,0xb7,0x2d,0x76,0x9a,0xe7,0x3e,0x4e,0x6c,0xd2,0x21,0x4f,0xfd,0xb8,0xfd,0x8d,0x39,0xdc,0x57,0x59,0xef},
{0x8d,0x9b,0xc,0x49,0x2b,0x49,0xeb,0xda,0x5b,0xa2,0xd7,0x49,0x68,0xf3,0x70,0xd,0x7d,0x3b,0xae,0xd0,0x7a,0x8d,0x55,0x84,0xf5,0xa5,0xe9,0xf0,0xe4,0xf8,0x8e,0x65},
{0xa0,0xb8,0xa2,0xf4,0x36,0x10,0x3b,0x53,0xc,0xa8,0x7,0x9e,0x75,0x3e,0xec,0x5a,0x91,0x68,0x94,0x92,0x56,0xe8,0x88,0x4f,0x5b,0xb0,0x5c,0x55,0xf8,0xba,0xbc,0x4c},
{0xe3,0xbb,0x3b,0x99,0xf3,0x87,0x94,0x7b,0x75,0xda,0xf4,0xd6,0x72,0x6b,0x1c,0x5d,0x64,0xae,0xac,0x28,0xdc,0x34,0xb3,0x6d,0x6c,0x34,0xa5,0x50,0xb8,0x28,0xdb,0x71},
{0xf8,0x61,0xe2,0xf2,0x10,0x8d,0x51,0x2a,0xe3,0xdb,0x64,0x33,0x59,0xdd,0x75,0xfc,0x1c,0xac,0xbc,0xf1,0x43,0xce,0x3f,0xa2,0x67,0xbb,0xd1,0x3c,0x2,0xe8,0x43,0xb0},
{0x33,0xa,0x5b,0xca,0x88,0x29,0xa1,0x75,0x7f,0x34,0x19,0x4d,0xb4,0x16,0x53,0x5c,0x92,0x3b,0x94,0xc3,0xe,0x79,0x4d,0x1e,0x79,0x74,0x75,0xd7,0xb6,0xee,0xaf,0x3f},
{0xea,0xa8,0xd4,0xf7,0xbe,0x1a,0x39,0x21,0x5c,0xf4,0x7e,0x9,0x4c,0x23,0x27,0x51,0x26,0xa3,0x24,0x53,0xba,0x32,0x3c,0xd2,0x44,0xa3,0x17,0x4a,0x6d,0xa6,0xd5,0xad},
{0xb5,0x1d,0x3e,0xa6,0xaf,0xf2,0xc9,0x8,0x83,0x59,0x3d,0x98,0x91,0x6b,0x3c,0x56,0x4c,0xf8,0x7c,0xa1,0x72,0x86,0x60,0x4d,0x46,0xe2,0x3e,0xcc,0x8,0x6e,0xc7,0xf6},
{0x2f,0x98,0x33,0xb3,0xb1,0xbc,0x76,0x5e,0x2b,0xd6,0x66,0xa5,0xef,0xc4,0xe6,0x2a,0x6,0xf4,0xb6,0xe8,0xbe,0xc1,0xd4,0x36,0x74,0xee,0x82,0x15,0xbc,0xef,0x21,0x63},
{0xfd,0xc1,0x4e,0xd,0xf4,0x53,0xc9,0x69,0xa7,0x7d,0x5a,0xc4,0x6,0x58,0x58,0x26,0x7e,0xc1,0x14,0x16,0x6,0xe0,0xfa,0x16,0x7e,0x90,0xaf,0x3d,0x28,0x63,0x9d,0x3f},
{0xd2,0xc9,0xf2,0xe3,0x0,0x9b,0xd2,0xc,0x5f,0xaa,0xce,0x30,0xb7,0xd4,0xc,0x30,0x74,0x2a,0x51,0x16,0xf2,0xe0,0x32,0x98,0xd,0xeb,0x30,0xd8,0xe3,0xce,0xf8,0x9a},
{0x4b,0xc5,0x9e,0x7b,0xb5,0xf1,0x79,0x92,0xff,0x51,0xe6,0x6e,0x4,0x86,0x68,0xd3,0x9b,0x23,0x4d,0x57,0xe6,0x96,0x67,0x31,0xcc,0xe6,0xa6,0xf3,0x17,0xa,0x75,0x5},
{0xb1,0x76,0x81,0xd9,0x13,0x32,0x6c,0xce,0x3c,0x17,0x52,0x84,0xf8,0x5,0xa2,0x62,0xf4,0x2b,0xcb,0xb3,0x78,0x47,0x15,0x47,0xff,0x46,0x54,0x82,0x23,0x93,0x6a,0x48},
{0x38,0xdf,0x58,0x7,0x4e,0x5e,0x65,0x65,0xf2,0xfc,0x7c,0x89,0xfc,0x86,0x50,0x8e,0x31,0x70,0x2e,0x44,0xd0,0xb,0xca,0x86,0xf0,0x40,0x9,0xa2,0x30,0x78,0x47,0x4e},
{0x65,0xa0,0xee,0x39,0xd1,0xf7,0x38,0x83,0xf7,0x5e,0xe9,0x37,0xe4,0x2c,0x3a,0xbd,0x21,0x97,0xb2,0x26,0x1,0x13,0xf8,0x6f,0xa3,0x44,0xed,0xd1,0xef,0x9f,0xde,0xe7},
{0x8b,0xa0,0xdf,0x15,0x76,0x25,0x92,0xd9,0x3c,0x85,0xf7,0xf6,0x12,0xdc,0x42,0xbe,0xd8,0xa7,0xec,0x7c,0xab,0x27,0xb0,0x7e,0x53,0x8d,0x7d,0xda,0xaa,0x3e,0xa8,0xde},
{0xaa,0x25,0xce,0x93,0xbd,0x2,0x69,0xd8,0x5a,0xf6,0x43,0xfd,0x1a,0x73,0x8,0xf9,0xc0,0x5f,0xef,0xda,0x17,0x4a,0x19,0xa5,0x97,0x4d,0x66,0x33,0x4c,0xfd,0x21,0x6a},
{0x35,0xb4,0x98,0x31,0xdb,0x41,0x15,0x70,0xea,0x1e,0xf,0xbb,0xed,0xcd,0x54,0x9b,0x9a,0xd0,0x63,0xa1,0x51,0x97,0x40,0x72,0xf6,0x75,0x9d,0xbf,0x91,0x47,0x6f,0xe2}};


static void E8(hashState *state);  /*The bijective function E8, in bitslice form*/
static void F8(hashState *state);  /*The compression function F8 */

/*The API functions*/
static HashReturn Init(hashState *state, int hashbitlen);
static HashReturn Update(hashState *state, const BitSequence *data, DataLength databitlen);
static HashReturn Final(hashState *state, BitSequence *hashval);
HashReturn jh_hash(int hashbitlen, const BitSequence *data,DataLength databitlen, BitSequence *hashval);

/*swapping bit 2i with bit 2i+1 of 64-bit x*/
#define SWAP1(x)   (x) = ((((x) & 0x5555555555555555ULL) << 1) | (((x) & 0xaaaaaaaaaaaaaaaaULL) >> 1));
/*swapping bits 4i||4i+1 with bits 4i+2||4i+3 of 64-bit x*/
#define SWAP2(x)   (x) = ((((x) & 0x3333333333333333ULL) << 2) | (((x) & 0xccccccccccccccccULL) >> 2));
/*swapping bits 8i||8i+1||8i+2||8i+3 with bits 8i+4||8i+5||8i+6||8i+7 of 64-bit x*/
#define SWAP4(x)   (x) = ((((x) & 0x0f0f0f0f0f0f0f0fULL) << 4) | (((x) & 0xf0f0f0f0f0f0f0f0ULL) >> 4));
/*swapping bits 16i||16i+1||......||16i+7  with bits 16i+8||16i+9||......||16i+15 of 64-bit x*/
#define SWAP8(x)   (x) = ((((x) & 0x00ff00ff00ff00ffULL) << 8) | (((x) & 0xff00ff00ff00ff00ULL) >> 8));
/*swapping bits 32i||32i+1||......||32i+15 with bits 32i+16||32i+17||......||32i+31 of 64-bit x*/
#define SWAP16(x)  (x) = ((((x) & 0x0000ffff0000ffffULL) << 16) | (((x) & 0xffff0000ffff0000ULL) >> 16));
/*swapping bits 64i||64i+1||......||64i+31 with bits 64i+32||64i+33||......||64i+63 of 64-bit x*/
#define SWAP32(x)  (x) = (((x) << 32) | ((x) >> 32));

/*The MDS transform*/
#define L(m0,m1,m2,m3,m4,m5,m6,m7) \
      (m4) ^= (m1);                \
      (m5) ^= (m2);                \
      (m6) ^= (m0) ^ (m3);         \
      (m7) ^= (m0);                \
      (m0) ^= (m5);                \
      (m1) ^= (m6);                \
      (m2) ^= (m4) ^ (m7);         \
      (m3) ^= (m4);

/*Two Sboxes are computed in parallel, each Sbox implements S0 and S1, selected by a constant bit*/
/*The reason to compute two Sboxes in parallel is to try to fully utilize the parallel processing power*/
#define SS(m0,m1,m2,m3,m4,m5,m6,m7,cc0,cc1)   \
      m3  = ~(m3);                  \
      m7  = ~(m7);                  \
      m0 ^= ((~(m2)) & (cc0));      \
      m4 ^= ((~(m6)) & (cc1));      \
      temp0 = (cc0) ^ ((m0) & (m1));\
      temp1 = (cc1) ^ ((m4) & (m5));\
      m0 ^= ((m2) & (m3));          \
      m4 ^= ((m6) & (m7));          \
      m3 ^= ((~(m1)) & (m2));       \
      m7 ^= ((~(m5)) & (m6));       \
      m1 ^= ((m0) & (m2));          \
      m5 ^= ((m4) & (m6));          \
      m2 ^= ((m0) & (~(m3)));       \
      m6 ^= ((m4) & (~(m7)));       \
      m0 ^= ((m1) | (m3));          \
      m4 ^= ((m5) | (m7));          \
      m3 ^= ((m1) & (m2));          \
      m7 ^= ((m5) & (m6));          \
      m1 ^= (temp0 & (m0));         \
      m5 ^= (temp1 & (m4));         \
      m2 ^= temp0;                  \
      m6 ^= temp1;

/*The bijective function E8, in bitslice form*/
static void E8(hashState *state)
{
    uint64 i,roundnumber,temp0,temp1;

    for (roundnumber = 0; roundnumber < 42; roundnumber = roundnumber+7) {
        /*round 7*roundnumber+0: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+0])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+0])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP1(state->x[1][i]); SWAP1(state->x[3][i]); SWAP1(state->x[5][i]); SWAP1(state->x[7][i]);
        }

        /*round 7*roundnumber+1: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+1])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+1])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP2(state->x[1][i]); SWAP2(state->x[3][i]); SWAP2(state->x[5][i]); SWAP2(state->x[7][i]);
        }

        /*round 7*roundnumber+2: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+2])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+2])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP4(state->x[1][i]); SWAP4(state->x[3][i]); SWAP4(state->x[5][i]); SWAP4(state->x[7][i]);
        }

        /*round 7*roundnumber+3: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+3])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+3])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP8(state->x[1][i]); SWAP8(state->x[3][i]); SWAP8(state->x[5][i]); SWAP8(state->x[7][i]);
        }

        /*round 7*roundnumber+4: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+4])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+4])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP16(state->x[1][i]); SWAP16(state->x[3][i]); SWAP16(state->x[5][i]); SWAP16(state->x[7][i]);
        }

        /*round 7*roundnumber+5: Sbox, MDS and Swapping layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+5])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+5])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
            SWAP32(state->x[1][i]); SWAP32(state->x[3][i]); SWAP32(state->x[5][i]); SWAP32(state->x[7][i]);
        }

        /*round 7*roundnumber+6: Sbox and MDS layers*/
        for (i = 0; i < 2; i++) {
            SS(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i],((uint64*)E8_bitslice_roundconstant[roundnumber+6])[i],((uint64*)E8_bitslice_roundconstant[roundnumber+6])[i+2] );
            L(state->x[0][i],state->x[2][i],state->x[4][i],state->x[6][i],state->x[1][i],state->x[3][i],state->x[5][i],state->x[7][i]);
        }
        /*round 7*roundnumber+6: swapping layer*/
        for (i = 1; i < 8; i = i+2) {
            temp0 = state->x[i][0]; state->x[i][0] = state->x[i][1]; state->x[i][1] = temp0;
        }
    }

}

/*The compression function F8 */
static void F8(hashState *state)
{
    uint64  i;

    /*xor the 512-bit message with the fist half of the 1024-bit hash state*/
    for (i = 0; i < 8; i++)  state->x[i >> 1][i & 1] ^= ((uint64*)state->buffer)[i];

    /*the bijective function E8 */
    E8(state);

    /*xor the 512-bit message with the second half of the 1024-bit hash state*/
    for (i = 0; i < 8; i++)  state->x[(8+i) >> 1][(8+i) & 1] ^= ((uint64*)state->buffer)[i];
}

/*before hashing a message, initialize the hash state as H0 */
static HashReturn Init(hashState *state, int hashbitlen)
{
    state->databitlen = 0;
    state->datasize_in_buffer = 0;

    /*initialize the initial hash value of JH*/
    state->hashbitlen = hashbitlen;

    /*load the intital hash value into state*/
    switch (hashbitlen)
    {
        case 224: memcpy(state->x,JH224_H0,128); break;
        case 256: memcpy(state->x,JH256_H0,128); break;
        case 384: memcpy(state->x,JH384_H0,128); break;
        case 512: memcpy(state->x,JH512_H0,128); break;
    }

    return(SUCCESS);
}


/*hash each 512-bit message block, except the last partial block*/
static HashReturn Update(hashState *state, const BitSequence *data, DataLength databitlen)
{
    DataLength index; /*the starting address of the data to be compressed*/

    state->databitlen += databitlen;
    index = 0;

    /*if there is remaining data in the buffer, fill it to a full message block first*/
    /*we assume that the size of the data in the buffer is the multiple of 8 bits if it is not at the end of a message*/

    /*There is data in the buffer, but the incoming data is insufficient for a full block*/
    if ( (state->datasize_in_buffer > 0 ) && (( state->datasize_in_buffer + databitlen) < 512)  ) {
        if ( (databitlen & 7) == 0 ) {
            memcpy(state->buffer + (state->datasize_in_buffer >> 3), data, 64-(state->datasize_in_buffer >> 3)) ;
        }
        else memcpy(state->buffer + (state->datasize_in_buffer >> 3), data, 64-(state->datasize_in_buffer >> 3)+1) ;
        state->datasize_in_buffer += databitlen;
        databitlen = 0;
    }

    /*There is data in the buffer, and the incoming data is sufficient for a full block*/
    if ( (state->datasize_in_buffer > 0 ) && (( state->datasize_in_buffer + databitlen) >= 512)  ) {
        memcpy( state->buffer + (state->datasize_in_buffer >> 3), data, 64-(state->datasize_in_buffer >> 3) ) ;
        index = 64-(state->datasize_in_buffer >> 3);
        databitlen = databitlen - (512 - state->datasize_in_buffer);
        F8(state);
        state->datasize_in_buffer = 0;
    }

    /*hash the remaining full message blocks*/
    for ( ; databitlen >= 512; index = index+64, databitlen = databitlen - 512) {
        memcpy(state->buffer, data+index, 64);
        F8(state);
    }

    /*store the partial block into buffer, assume that -- if part of the last byte is not part of the message, then that part consists of 0 bits*/
    if ( databitlen > 0) {
        if ((databitlen & 7) == 0)
            memcpy(state->buffer, data+index, (databitlen & 0x1ff) >> 3);
        else
            memcpy(state->buffer, data+index, ((databitlen & 0x1ff) >> 3)+1);
        state->datasize_in_buffer = databitlen;
    }

    return(SUCCESS);
}

/*pad the message, process the padded block(s), truncate the hash value H to obtain the message digest*/
static HashReturn Final(hashState *state, BitSequence *hashval)
{
    unsigned int i;

    if ( (state->databitlen & 0x1ff) == 0 ) {
        /*pad the message when databitlen is multiple of 512 bits, then process the padded block*/
        memset(state->buffer, 0, 64);
        state->buffer[0]  = 0x80;
        state->buffer[63] = state->databitlen & 0xff;
        state->buffer[62] = (state->databitlen >> 8)  & 0xff;
        state->buffer[61] = (state->databitlen >> 16) & 0xff;
        state->buffer[60] = (state->databitlen >> 24) & 0xff;
        state->buffer[59] = (state->databitlen >> 32) & 0xff;
        state->buffer[58] = (state->databitlen >> 40) & 0xff;
        state->buffer[57] = (state->databitlen >> 48) & 0xff;
        state->buffer[56] = (state->databitlen >> 56) & 0xff;
        F8(state);
    }
    else {
        /*set the rest of the bytes in the buffer to 0*/
        if ( (state->datasize_in_buffer & 7) == 0)
            for (i = (state->databitlen & 0x1ff) >> 3; i < 64; i++)  state->buffer[i] = 0;
        else
            for (i = ((state->databitlen & 0x1ff) >> 3)+1; i < 64; i++)  state->buffer[i] = 0;

        /*pad and process the partial block when databitlen is not multiple of 512 bits, then hash the padded blocks*/
        state->buffer[((state->databitlen & 0x1ff) >> 3)] |= 1 << (7- (state->databitlen & 7));

        F8(state);
        memset(state->buffer, 0, 64);
        state->buffer[63] = state->databitlen & 0xff;
        state->buffer[62] = (state->databitlen >> 8) & 0xff;
        state->buffer[61] = (state->databitlen >> 16) & 0xff;
        state->buffer[60] = (state->databitlen >> 24) & 0xff;
        state->buffer[59] = (state->databitlen >> 32) & 0xff;
        state->buffer[58] = (state->databitlen >> 40) & 0xff;
        state->buffer[57] = (state->databitlen >> 48) & 0xff;
        state->buffer[56] = (state->databitlen >> 56) & 0xff;
        F8(state);
    }

    /*truncating the final hash value to generate the message digest*/
    switch(state->hashbitlen) {
        case 224: memcpy(hashval,(unsigned char*)state->x+64+36,28);  break;
        case 256: memcpy(hashval,(unsigned char*)state->x+64+32,32);  break;
        case 384: memcpy(hashval,(unsigned char*)state->x+64+16,48);  break;
        case 512: memcpy(hashval,(unsigned char*)state->x+64,64);     break;
    }

    return(SUCCESS);
}

/* hash a message,
   three inputs: message digest size in bits (hashbitlen); message (data); message length in bits (databitlen)
   one output:   message digest (hashval)
*/
HashReturn jh_hash(int hashbitlen, const BitSequence *data,DataLength databitlen, BitSequence *hashval)
{
    hashState state;

    if ( hashbitlen == 224 || hashbitlen == 256 || hashbitlen == 384 || hashbitlen == 512 ) {
        Init(&state, hashbitlen);
        Update(&state, data, databitlen);
        Final(&state, hashval);
        return SUCCESS;
    }
    else
        return(BAD_HASHLEN);
}


#ifndef RETURN_VALUES
#  define RETURN_VALUES
#  if defined( DLL_EXPORT )
#    if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#      define VOID_RETURN    __declspec( dllexport ) void __stdcall
#      define INT_RETURN     __declspec( dllexport ) int  __stdcall
#    elif defined( __GNUC__ )
#      define VOID_RETURN    __declspec( __dllexport__ ) void
#      define INT_RETURN     __declspec( __dllexport__ ) int
#    else
#      error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#    endif
#  elif defined( DLL_IMPORT )
#    if defined( _MSC_VER ) || defined ( __INTEL_COMPILER )
#      define VOID_RETURN    __declspec( dllimport ) void __stdcall
#      define INT_RETURN     __declspec( dllimport ) int  __stdcall
#    elif defined( __GNUC__ )
#      define VOID_RETURN    __declspec( __dllimport__ ) void
#      define INT_RETURN     __declspec( __dllimport__ ) int
#    else
#      error Use of the DLL is only available on the Microsoft, Intel and GCC compilers
#    endif
#  elif defined( __WATCOMC__ )
#    define VOID_RETURN  void __cdecl
#    define INT_RETURN   int  __cdecl
#  else
#    define VOID_RETURN  void
#    define INT_RETURN   int
#  endif
#endif

/*  These defines are used to declare buffers in a way that allows
    faster operations on longer variables to be used.  In all these
    defines 'size' must be a power of 2 and >= 8

    dec_unit_type(size,x)       declares a variable 'x' of length
                                'size' bits

    dec_bufr_type(size,bsize,x) declares a buffer 'x' of length 'bsize'
                                bytes defined as an array of variables
                                each of 'size' bits (bsize must be a
                                multiple of size / 8)

    ptr_cast(x,size)            casts a pointer to a pointer to a
                                varaiable of length 'size' bits
*/

#define ui_type(size)               uint##size##_t
#define dec_unit_type(size,x)       typedef ui_type(size) x
#define dec_bufr_type(size,bsize,x) typedef ui_type(size) x[bsize / (size >> 3)]
#define ptr_cast(x,size)            ((ui_type(size)*)(x))

typedef unsigned int    uint_t;             /* native unsigned integer */
typedef uint8_t         u08b_t;             /*  8-bit unsigned integer */
typedef uint64_t        u64b_t;             /* 64-bit unsigned integer */

#ifndef RotL_64
#define RotL_64(x,N)    (((x) << (N)) | ((x) >> (64-(N))))
#endif

/*
 * Skein is "natively" little-endian (unlike SHA-xxx), for optimal
 * performance on x86 CPUs.  The Skein code requires the following
 * definitions for dealing with endianness:
 *
 *    SKEIN_NEED_SWAP:  0 for little-endian, 1 for big-endian
 *    Skein_Put64_LSB_First
 *    Skein_Get64_LSB_First
 *    Skein_Swap64
 *
 * If SKEIN_NEED_SWAP is defined at compile time, it is used here
 * along with the portable versions of Put64/Get64/Swap64, which
 * are slow in general.
 *
 * Otherwise, an "auto-detect" of endianness is attempted below.
 * If the default handling doesn't work well, the user may insert
 * platform-specific code instead (e.g., for big-endian CPUs).
 *
 */
#ifndef SKEIN_NEED_SWAP /* compile-time "override" for endianness? */

#define IS_BIG_ENDIAN      4321 /* byte 0 is most significant (mc68k) */
#define IS_LITTLE_ENDIAN   1234 /* byte 0 is least significant (i386) */

#if BYTE_ORDER == LITTLE_ENDIAN && !defined(PLATFORM_BYTE_ORDER)
#  define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif

#if BYTE_ORDER == BIG_ENDIAN && !defined(PLATFORM_BYTE_ORDER)
#  define PLATFORM_BYTE_ORDER IS_BIG_ENDIAN
#endif

/* special handler for IA64, which may be either endianness (?)  */
/* here we assume little-endian, but this may need to be changed */
#if defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#  define PLATFORM_MUST_ALIGN (1)
#ifndef PLATFORM_BYTE_ORDER
#  define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN
#endif
#endif

#ifndef   PLATFORM_MUST_ALIGN
#  define PLATFORM_MUST_ALIGN (0)
#endif


#if   PLATFORM_BYTE_ORDER == IS_BIG_ENDIAN
    /* here for big-endian CPUs */
#define SKEIN_NEED_SWAP   (1)
#elif PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN
    /* here for x86 and x86-64 CPUs (and other detected little-endian CPUs) */
#define SKEIN_NEED_SWAP   (0)
#if   PLATFORM_MUST_ALIGN == 0              /* ok to use "fast" versions? */
#define Skein_Put64_LSB_First(dst08,src64,bCnt) memcpy(dst08,src64,bCnt)
#define Skein_Get64_LSB_First(dst64,src08,wCnt) memcpy(dst64,src08,8*(wCnt))
#endif
#else
#error "Skein needs endianness setting!"
#endif

#endif /* ifndef SKEIN_NEED_SWAP */

/*
 ******************************************************************
 *      Provide any definitions still needed.
 ******************************************************************
 */
#ifndef Skein_Swap64  /* swap for big-endian, nop for little-endian */
#if     SKEIN_NEED_SWAP
#define Skein_Swap64(w64)                       \
  ( (( ((u64b_t)(w64))       & 0xFF) << 56) |   \
    (((((u64b_t)(w64)) >> 8) & 0xFF) << 48) |   \
    (((((u64b_t)(w64)) >>16) & 0xFF) << 40) |   \
    (((((u64b_t)(w64)) >>24) & 0xFF) << 32) |   \
    (((((u64b_t)(w64)) >>32) & 0xFF) << 24) |   \
    (((((u64b_t)(w64)) >>40) & 0xFF) << 16) |   \
    (((((u64b_t)(w64)) >>48) & 0xFF) <<  8) |   \
    (((((u64b_t)(w64)) >>56) & 0xFF)      ) )
#else
#define Skein_Swap64(w64)  (w64)
#endif
#endif  /* ifndef Skein_Swap64 */


#ifndef Skein_Put64_LSB_First
void    Skein_Put64_LSB_First(u08b_t *dst,const u64b_t *src,size_t bCnt)
#ifdef  SKEIN_PORT_CODE /* instantiate the function code here? */
    { /* this version is fully portable (big-endian or little-endian), but slow */
    size_t n;

    for (n=0;n<bCnt;n++)
        dst[n] = (u08b_t) (src[n>>3] >> (8*(n&7)));
    }
#else
    ;    /* output only the function prototype */
#endif
#endif   /* ifndef Skein_Put64_LSB_First */


#ifndef Skein_Get64_LSB_First
void    Skein_Get64_LSB_First(u64b_t *dst,const u08b_t *src,size_t wCnt)
#ifdef  SKEIN_PORT_CODE /* instantiate the function code here? */
    { /* this version is fully portable (big-endian or little-endian), but slow */
    size_t n;

    for (n=0;n<8*wCnt;n+=8)
        dst[n/8] = (((u64b_t) src[n  ])      ) +
                   (((u64b_t) src[n+1]) <<  8) +
                   (((u64b_t) src[n+2]) << 16) +
                   (((u64b_t) src[n+3]) << 24) +
                   (((u64b_t) src[n+4]) << 32) +
                   (((u64b_t) src[n+5]) << 40) +
                   (((u64b_t) src[n+6]) << 48) +
                   (((u64b_t) src[n+7]) << 56) ;
    }
#else
    ;    /* output only the function prototype */
#endif
#endif   /* ifndef Skein_Get64_LSB_First */


typedef enum
{
    SKEIN_SUCCESS         =      0,          /* return codes from Skein calls */
    SKEIN_FAIL            =      1,
    SKEIN_BAD_HASHLEN     =      2
}
SkeinHashReturn;

typedef size_t   SkeinDataLength;                /* bit count  type */
typedef u08b_t   SkeinBitSequence;               /* bit stream type */

#ifndef SKEIN_512_NIST_MAX_HASHBITS
#define SKEIN_512_NIST_MAX_HASHBITS (512)
#endif

#define  SKEIN_MODIFIER_WORDS  ( 2)          /* number of modifier (tweak) words */

#define  SKEIN_512_STATE_WORDS ( 8)
#define  SKEIN_MAX_STATE_WORDS (16)

#define  SKEIN_512_STATE_BYTES ( 8*SKEIN_512_STATE_WORDS)
#define  SKEIN_512_STATE_BITS  (64*SKEIN_512_STATE_WORDS)
#define  SKEIN_512_BLOCK_BYTES ( 8*SKEIN_512_STATE_WORDS)

#define SKEIN_RND_SPECIAL       (1000u)
#define SKEIN_RND_KEY_INITIAL   (SKEIN_RND_SPECIAL+0u)
#define SKEIN_RND_KEY_INJECT    (SKEIN_RND_SPECIAL+1u)
#define SKEIN_RND_FEED_FWD      (SKEIN_RND_SPECIAL+2u)

typedef struct
{
    size_t  hashBitLen;                      /* size of hash result, in bits */
    size_t  bCnt;                            /* current byte count in buffer b[] */
    u64b_t  T[SKEIN_MODIFIER_WORDS];         /* tweak words: T[0]=byte cnt, T[1]=flags */
} Skein_Ctxt_Hdr_t;

typedef struct                               /*  512-bit Skein hash context structure */
{
    Skein_Ctxt_Hdr_t h;                      /* common header context variables */
    u64b_t  X[SKEIN_512_STATE_WORDS];        /* chaining variables */
    u08b_t  b[SKEIN_512_BLOCK_BYTES];        /* partial block buffer (8-byte aligned) */
} Skein_512_Ctxt_t;

/*   Skein APIs for (incremental) "straight hashing" */
static SkeinHashReturn  Skein_512_Init  (Skein_512_Ctxt_t *ctx, size_t hashBitLen);
static SkeinHashReturn  Skein_512_Update(Skein_512_Ctxt_t *ctx, const u08b_t *msg, size_t msgByteCnt);
static SkeinHashReturn  Skein_512_Final (Skein_512_Ctxt_t *ctx, u08b_t * hashVal);

#ifndef SKEIN_TREE_HASH
#define SKEIN_TREE_HASH (1)
#endif

/*****************************************************************
** "Internal" Skein definitions
**    -- not needed for sequential hashing API, but will be 
**           helpful for other uses of Skein (e.g., tree hash mode).
**    -- included here so that they can be shared between
**           reference and optimized code.
******************************************************************/

/* tweak word T[1]: bit field starting positions */
#define SKEIN_T1_BIT(BIT)       ((BIT) - 64)            /* offset 64 because it's the second word  */

#define SKEIN_T1_POS_TREE_LVL   SKEIN_T1_BIT(112)       /* bits 112..118: level in hash tree       */
#define SKEIN_T1_POS_BIT_PAD    SKEIN_T1_BIT(119)       /* bit  119     : partial final input byte */
#define SKEIN_T1_POS_BLK_TYPE   SKEIN_T1_BIT(120)       /* bits 120..125: type field               */
#define SKEIN_T1_POS_FIRST      SKEIN_T1_BIT(126)       /* bits 126     : first block flag         */
#define SKEIN_T1_POS_FINAL      SKEIN_T1_BIT(127)       /* bit  127     : final block flag         */

/* tweak word T[1]: flag bit definition(s) */
#define SKEIN_T1_FLAG_FIRST     (((u64b_t)  1 ) << SKEIN_T1_POS_FIRST)
#define SKEIN_T1_FLAG_FINAL     (((u64b_t)  1 ) << SKEIN_T1_POS_FINAL)
#define SKEIN_T1_FLAG_BIT_PAD   (((u64b_t)  1 ) << SKEIN_T1_POS_BIT_PAD)

/* tweak word T[1]: tree level bit field mask */
#define SKEIN_T1_TREE_LVL_MASK  (((u64b_t)0x7F) << SKEIN_T1_POS_TREE_LVL)
#define SKEIN_T1_TREE_LEVEL(n)  (((u64b_t) (n)) << SKEIN_T1_POS_TREE_LVL)

/* tweak word T[1]: block type field */
#define SKEIN_BLK_TYPE_KEY      ( 0)                    /* key, for MAC and KDF */
#define SKEIN_BLK_TYPE_CFG      ( 4)                    /* configuration block */
#define SKEIN_BLK_TYPE_PERS     ( 8)                    /* personalization string */
#define SKEIN_BLK_TYPE_PK       (12)                    /* public key (for digital signature hashing) */
#define SKEIN_BLK_TYPE_KDF      (16)                    /* key identifier for KDF */
#define SKEIN_BLK_TYPE_NONCE    (20)                    /* nonce for PRNG */
#define SKEIN_BLK_TYPE_MSG      (48)                    /* message processing */
#define SKEIN_BLK_TYPE_OUT      (63)                    /* output stage */
#define SKEIN_BLK_TYPE_MASK     (63)                    /* bit field mask */

#define SKEIN_T1_BLK_TYPE(T)   (((u64b_t) (SKEIN_BLK_TYPE_##T)) << SKEIN_T1_POS_BLK_TYPE)
#define SKEIN_T1_BLK_TYPE_KEY   SKEIN_T1_BLK_TYPE(KEY)  /* key, for MAC and KDF */
#define SKEIN_T1_BLK_TYPE_CFG   SKEIN_T1_BLK_TYPE(CFG)  /* configuration block */
#define SKEIN_T1_BLK_TYPE_PERS  SKEIN_T1_BLK_TYPE(PERS) /* personalization string */
#define SKEIN_T1_BLK_TYPE_PK    SKEIN_T1_BLK_TYPE(PK)   /* public key (for digital signature hashing) */
#define SKEIN_T1_BLK_TYPE_KDF   SKEIN_T1_BLK_TYPE(KDF)  /* key identifier for KDF */
#define SKEIN_T1_BLK_TYPE_NONCE SKEIN_T1_BLK_TYPE(NONCE)/* nonce for PRNG */
#define SKEIN_T1_BLK_TYPE_MSG   SKEIN_T1_BLK_TYPE(MSG)  /* message processing */
#define SKEIN_T1_BLK_TYPE_OUT   SKEIN_T1_BLK_TYPE(OUT)  /* output stage */
#define SKEIN_T1_BLK_TYPE_MASK  SKEIN_T1_BLK_TYPE(MASK) /* field bit mask */

#define SKEIN_T1_BLK_TYPE_CFG_FINAL       (SKEIN_T1_BLK_TYPE_CFG | SKEIN_T1_FLAG_FINAL)
#define SKEIN_T1_BLK_TYPE_OUT_FINAL       (SKEIN_T1_BLK_TYPE_OUT | SKEIN_T1_FLAG_FINAL)

#define SKEIN_VERSION           (1)

#ifndef SKEIN_ID_STRING_LE      /* allow compile-time personalization */
#define SKEIN_ID_STRING_LE      (0x33414853)            /* "SHA3" (little-endian)*/
#endif

#define SKEIN_MK_64(hi32,lo32)  ((lo32) + (((u64b_t) (hi32)) << 32))
#define SKEIN_SCHEMA_VER        SKEIN_MK_64(SKEIN_VERSION,SKEIN_ID_STRING_LE)
#define SKEIN_KS_PARITY         SKEIN_MK_64(0x1BD11BDA,0xA9FC1A22)

#define SKEIN_CFG_STR_LEN       (4*8)

/* bit field definitions in config block treeInfo word */
#define SKEIN_CFG_TREE_LEAF_SIZE_POS  ( 0)
#define SKEIN_CFG_TREE_NODE_SIZE_POS  ( 8)
#define SKEIN_CFG_TREE_MAX_LEVEL_POS  (16)

#define SKEIN_CFG_TREE_LEAF_SIZE_MSK  (((u64b_t) 0xFF) << SKEIN_CFG_TREE_LEAF_SIZE_POS)
#define SKEIN_CFG_TREE_NODE_SIZE_MSK  (((u64b_t) 0xFF) << SKEIN_CFG_TREE_NODE_SIZE_POS)
#define SKEIN_CFG_TREE_MAX_LEVEL_MSK  (((u64b_t) 0xFF) << SKEIN_CFG_TREE_MAX_LEVEL_POS)

#define SKEIN_CFG_TREE_INFO(leaf,node,maxLvl)                   \
  ( (((u64b_t)(leaf  )) << SKEIN_CFG_TREE_LEAF_SIZE_POS) |    \
  (((u64b_t)(node  )) << SKEIN_CFG_TREE_NODE_SIZE_POS) |    \
  (((u64b_t)(maxLvl)) << SKEIN_CFG_TREE_MAX_LEVEL_POS) )

#define SKEIN_CFG_TREE_INFO_SEQUENTIAL SKEIN_CFG_TREE_INFO(0,0,0) /* use as treeInfo in InitExt() call for sequential processing */

/*
**   Skein macros for getting/setting tweak words, etc.
**   These are useful for partial input bytes, hash tree init/update, etc.
**/
#define Skein_Get_Tweak(ctxPtr,TWK_NUM)         ((ctxPtr)->h.T[TWK_NUM])
#define Skein_Set_Tweak(ctxPtr,TWK_NUM,tVal)    {(ctxPtr)->h.T[TWK_NUM] = (tVal);}

#define Skein_Get_T0(ctxPtr)    Skein_Get_Tweak(ctxPtr,0)
#define Skein_Get_T1(ctxPtr)    Skein_Get_Tweak(ctxPtr,1)
#define Skein_Set_T0(ctxPtr,T0) Skein_Set_Tweak(ctxPtr,0,T0)
#define Skein_Set_T1(ctxPtr,T1) Skein_Set_Tweak(ctxPtr,1,T1)

/* set both tweak words at once */
#define Skein_Set_T0_T1(ctxPtr,T0,T1)           \
{                                               \
    Skein_Set_T0(ctxPtr,(T0));                  \
    Skein_Set_T1(ctxPtr,(T1));                  \
}

#define Skein_Set_Type(ctxPtr,BLK_TYPE)         \
  Skein_Set_T1(ctxPtr,SKEIN_T1_BLK_TYPE_##BLK_TYPE)

/* set up for starting with a new type: h.T[0]=0; h.T[1] = NEW_TYPE; h.bCnt=0; */
#define Skein_Start_New_Type(ctxPtr,BLK_TYPE)   \
{ Skein_Set_T0_T1(ctxPtr,0,SKEIN_T1_FLAG_FIRST | SKEIN_T1_BLK_TYPE_##BLK_TYPE); (ctxPtr)->h.bCnt=0; }

#define Skein_Clear_First_Flag(hdr)      { (hdr).T[1] &= ~SKEIN_T1_FLAG_FIRST;       }
#define Skein_Set_Bit_Pad_Flag(hdr)      { (hdr).T[1] |=  SKEIN_T1_FLAG_BIT_PAD;     }

#define Skein_Set_Tree_Level(hdr,height) { (hdr).T[1] |= SKEIN_T1_TREE_LEVEL(height);}

/*****************************************************************
** "Internal" Skein definitions for debugging and error checking
******************************************************************/
#define Skein_Show_Block(bits,ctx,X,blkPtr,wPtr,ksEvenPtr,ksOddPtr)
#define Skein_Show_Round(bits,ctx,r,X)
#define Skein_Show_R_Ptr(bits,ctx,r,X_ptr)
#define Skein_Show_Final(bits,ctx,cnt,outPtr)
#define Skein_Show_Key(bits,ctx,key,keyBytes)


#ifndef SKEIN_ERR_CHECK        /* run-time checks (e.g., bad params, uninitialized context)? */
#define Skein_Assert(x,retCode)/* default: ignore all Asserts, for performance */
#define Skein_assert(x)
#elif   defined(SKEIN_ASSERT)
#include <assert.h>
#define Skein_Assert(x,retCode) assert(x) 
#define Skein_assert(x)         assert(x) 
#else
#include <assert.h>     
#define Skein_Assert(x,retCode) { if (!(x)) return retCode; } /*  caller  error */
#define Skein_assert(x)         assert(x)                     /* internal error */
#endif

/*****************************************************************
** Skein block function constants (shared across Ref and Opt code)
******************************************************************/
enum    
{   
    /* Skein_512 round rotation constants */
    R_512_0_0=46, R_512_0_1=36, R_512_0_2=19, R_512_0_3=37,
    R_512_1_0=33, R_512_1_1=27, R_512_1_2=14, R_512_1_3=42,
    R_512_2_0=17, R_512_2_1=49, R_512_2_2=36, R_512_2_3=39,
    R_512_3_0=44, R_512_3_1= 9, R_512_3_2=54, R_512_3_3=56,
    R_512_4_0=39, R_512_4_1=30, R_512_4_2=34, R_512_4_3=24,
    R_512_5_0=13, R_512_5_1=50, R_512_5_2=10, R_512_5_3=17,
    R_512_6_0=25, R_512_6_1=29, R_512_6_2=39, R_512_6_3=43,
    R_512_7_0= 8, R_512_7_1=35, R_512_7_2=56, R_512_7_3=22,
};

#ifndef SKEIN_ROUNDS
#define SKEIN_512_ROUNDS_TOTAL (72)
#else                                        /* allow command-line define in range 8*(5..14)   */
#define SKEIN_512_ROUNDS_TOTAL (8*((((SKEIN_ROUNDS/ 10) + 5) % 10) + 5))
#endif


/*
***************** Pre-computed Skein IVs *******************
**
** NOTE: these values are not "magic" constants, but
** are generated using the Threefish block function.
** They are pre-computed here only for speed; i.e., to
** avoid the need for a Threefish call during Init().
**
** The IV for any fixed hash length may be pre-computed.
** Only the most common values are included here.
**
************************************************************
**/

#define MK_64 SKEIN_MK_64

/* blkSize =  512 bits. hashSize =  256 bits */
const u64b_t SKEIN_512_IV_256[] =
    {
    MK_64(0xCCD044A1,0x2FDB3E13),
    MK_64(0xE8359030,0x1A79A9EB),
    MK_64(0x55AEA061,0x4F816E6F),
    MK_64(0x2A2767A4,0xAE9B94DB),
    MK_64(0xEC06025E,0x74DD7683),
    MK_64(0xE7A436CD,0xC4746251),
    MK_64(0xC36FBAF9,0x393AD185),
    MK_64(0x3EEDBA18,0x33EDFC13)
    };

#ifndef SKEIN_USE_ASM
#define SKEIN_USE_ASM   (0)                     /* default is all C code (no ASM) */
#endif

#ifndef SKEIN_LOOP
#define SKEIN_LOOP 001                          /* default: unroll 256 and 512, but not 1024 */
#endif

#define BLK_BITS        (WCNT*64)               /* some useful definitions for code here */
#define KW_TWK_BASE     (0)
#define KW_KEY_BASE     (3)
#define ks              (kw + KW_KEY_BASE)                
#define ts              (kw + KW_TWK_BASE)

#ifdef SKEIN_DEBUG
#define DebugSaveTweak(ctx) { ctx->h.T[0] = ts[0]; ctx->h.T[1] = ts[1]; }
#else
#define DebugSaveTweak(ctx)
#endif

/*****************************  Skein_512 ******************************/
#if !(SKEIN_USE_ASM & 512)
static void Skein_512_Process_Block(Skein_512_Ctxt_t *ctx,const u08b_t *blkPtr,size_t blkCnt,size_t byteCntAdd)
    { /* do it in C */
    enum
        {
        WCNT = SKEIN_512_STATE_WORDS
        };
#undef  RCNT
#define RCNT  (SKEIN_512_ROUNDS_TOTAL/8)

#ifdef  SKEIN_LOOP                              /* configure how much to unroll the loop */
#define SKEIN_UNROLL_512 (((SKEIN_LOOP)/10)%10)
#else
#define SKEIN_UNROLL_512 (0)
#endif

#if SKEIN_UNROLL_512
#if (RCNT % SKEIN_UNROLL_512)
#error "Invalid SKEIN_UNROLL_512"               /* sanity check on unroll count */
#endif
    size_t  r;
    u64b_t  kw[WCNT+4+RCNT*2];                  /* key schedule words : chaining vars + tweak + "rotation"*/
#else
    u64b_t  kw[WCNT+4];                         /* key schedule words : chaining vars + tweak */
#endif
    u64b_t  X0,X1,X2,X3,X4,X5,X6,X7;            /* local copy of vars, for speed */
    u64b_t  w [WCNT];                           /* local copy of input block */
#ifdef SKEIN_DEBUG
    const u64b_t *Xptr[8];                      /* use for debugging (help compiler put Xn in registers) */
    Xptr[0] = &X0;  Xptr[1] = &X1;  Xptr[2] = &X2;  Xptr[3] = &X3;
    Xptr[4] = &X4;  Xptr[5] = &X5;  Xptr[6] = &X6;  Xptr[7] = &X7;
#endif

    Skein_assert(blkCnt != 0);                  /* never call with blkCnt == 0! */
    ts[0] = ctx->h.T[0];
    ts[1] = ctx->h.T[1];
    do  {
        /* this implementation only supports 2**64 input bytes (no carry out here) */
        ts[0] += byteCntAdd;                    /* update processed length */

        /* precompute the key schedule for this block */
        ks[0] = ctx->X[0];
        ks[1] = ctx->X[1];
        ks[2] = ctx->X[2];
        ks[3] = ctx->X[3];
        ks[4] = ctx->X[4];
        ks[5] = ctx->X[5];
        ks[6] = ctx->X[6];
        ks[7] = ctx->X[7];
        ks[8] = ks[0] ^ ks[1] ^ ks[2] ^ ks[3] ^ 
                ks[4] ^ ks[5] ^ ks[6] ^ ks[7] ^ SKEIN_KS_PARITY;

        ts[2] = ts[0] ^ ts[1];

        Skein_Get64_LSB_First(w,blkPtr,WCNT); /* get input block in little-endian format */
        DebugSaveTweak(ctx);
        Skein_Show_Block(BLK_BITS,&ctx->h,ctx->X,blkPtr,w,ks,ts);

        X0   = w[0] + ks[0];                    /* do the first full key injection */
        X1   = w[1] + ks[1];
        X2   = w[2] + ks[2];
        X3   = w[3] + ks[3];
        X4   = w[4] + ks[4];
        X5   = w[5] + ks[5] + ts[0];
        X6   = w[6] + ks[6] + ts[1];
        X7   = w[7] + ks[7];

        blkPtr += SKEIN_512_BLOCK_BYTES;

        Skein_Show_R_Ptr(BLK_BITS,&ctx->h,SKEIN_RND_KEY_INITIAL,Xptr);
        /* run the rounds */
#define Round512(p0,p1,p2,p3,p4,p5,p6,p7,ROT,rNum)                  \
    X##p0 += X##p1; X##p1 = RotL_64(X##p1,ROT##_0); X##p1 ^= X##p0; \
    X##p2 += X##p3; X##p3 = RotL_64(X##p3,ROT##_1); X##p3 ^= X##p2; \
    X##p4 += X##p5; X##p5 = RotL_64(X##p5,ROT##_2); X##p5 ^= X##p4; \
    X##p6 += X##p7; X##p7 = RotL_64(X##p7,ROT##_3); X##p7 ^= X##p6; \

#if SKEIN_UNROLL_512 == 0                       
#define R512(p0,p1,p2,p3,p4,p5,p6,p7,ROT,rNum)      /* unrolled */  \
    Round512(p0,p1,p2,p3,p4,p5,p6,p7,ROT,rNum)                      \
    Skein_Show_R_Ptr(BLK_BITS,&ctx->h,rNum,Xptr);

#define I512(R)                                                     \
    X0   += ks[((R)+1) % 9];   /* inject the key schedule value */  \
    X1   += ks[((R)+2) % 9];                                        \
    X2   += ks[((R)+3) % 9];                                        \
    X3   += ks[((R)+4) % 9];                                        \
    X4   += ks[((R)+5) % 9];                                        \
    X5   += ks[((R)+6) % 9] + ts[((R)+1) % 3];                      \
    X6   += ks[((R)+7) % 9] + ts[((R)+2) % 3];                      \
    X7   += ks[((R)+8) % 9] +     (R)+1;                            \
    Skein_Show_R_Ptr(BLK_BITS,&ctx->h,SKEIN_RND_KEY_INJECT,Xptr);
#else                                       /* looping version */
#define R512(p0,p1,p2,p3,p4,p5,p6,p7,ROT,rNum)                      \
    Round512(p0,p1,p2,p3,p4,p5,p6,p7,ROT,rNum)                      \
    Skein_Show_R_Ptr(BLK_BITS,&ctx->h,4*(r-1)+rNum,Xptr);

#define I512(R)                                                     \
    X0   += ks[r+(R)+0];        /* inject the key schedule value */ \
    X1   += ks[r+(R)+1];                                            \
    X2   += ks[r+(R)+2];                                            \
    X3   += ks[r+(R)+3];                                            \
    X4   += ks[r+(R)+4];                                            \
    X5   += ks[r+(R)+5] + ts[r+(R)+0];                              \
    X6   += ks[r+(R)+6] + ts[r+(R)+1];                              \
    X7   += ks[r+(R)+7] +    r+(R)   ;                              \
    ks[r +       (R)+8] = ks[r+(R)-1];  /* rotate key schedule */   \
    ts[r +       (R)+2] = ts[r+(R)-1];                              \
    Skein_Show_R_Ptr(BLK_BITS,&ctx->h,SKEIN_RND_KEY_INJECT,Xptr);

    for (r=1;r < 2*RCNT;r+=2*SKEIN_UNROLL_512)   /* loop thru it */
#endif                         /* end of looped code definitions */
        {
#define R512_8_rounds(R)  /* do 8 full rounds */  \
        R512(0,1,2,3,4,5,6,7,R_512_0,8*(R)+ 1);   \
        R512(2,1,4,7,6,5,0,3,R_512_1,8*(R)+ 2);   \
        R512(4,1,6,3,0,5,2,7,R_512_2,8*(R)+ 3);   \
        R512(6,1,0,7,2,5,4,3,R_512_3,8*(R)+ 4);   \
        I512(2*(R));                              \
        R512(0,1,2,3,4,5,6,7,R_512_4,8*(R)+ 5);   \
        R512(2,1,4,7,6,5,0,3,R_512_5,8*(R)+ 6);   \
        R512(4,1,6,3,0,5,2,7,R_512_6,8*(R)+ 7);   \
        R512(6,1,0,7,2,5,4,3,R_512_7,8*(R)+ 8);   \
        I512(2*(R)+1);        /* and key injection */

        R512_8_rounds( 0);

#define R512_Unroll_R(NN) ((SKEIN_UNROLL_512 == 0 && SKEIN_512_ROUNDS_TOTAL/8 > (NN)) || (SKEIN_UNROLL_512 > (NN)))

#if   R512_Unroll_R( 1)
        R512_8_rounds( 1);
#endif
#if   R512_Unroll_R( 2)
        R512_8_rounds( 2);
#endif
#if   R512_Unroll_R( 3)
        R512_8_rounds( 3);
#endif
#if   R512_Unroll_R( 4)
        R512_8_rounds( 4);
#endif
#if   R512_Unroll_R( 5)
        R512_8_rounds( 5);
#endif
#if   R512_Unroll_R( 6)
        R512_8_rounds( 6);
#endif
#if   R512_Unroll_R( 7)
        R512_8_rounds( 7);
#endif
#if   R512_Unroll_R( 8)
        R512_8_rounds( 8);
#endif
#if   R512_Unroll_R( 9)
        R512_8_rounds( 9);
#endif
#if   R512_Unroll_R(10)
        R512_8_rounds(10);
#endif
#if   R512_Unroll_R(11)
        R512_8_rounds(11);
#endif
#if   R512_Unroll_R(12)
        R512_8_rounds(12);
#endif
#if   R512_Unroll_R(13)
        R512_8_rounds(13);
#endif
#if   R512_Unroll_R(14)
        R512_8_rounds(14);
#endif
#if  (SKEIN_UNROLL_512 > 14)
#error  "need more unrolling in Skein_512_Process_Block"
#endif
        }

        /* do the final "feedforward" xor, update context chaining vars */
        ctx->X[0] = X0 ^ w[0];
        ctx->X[1] = X1 ^ w[1];
        ctx->X[2] = X2 ^ w[2];
        ctx->X[3] = X3 ^ w[3];
        ctx->X[4] = X4 ^ w[4];
        ctx->X[5] = X5 ^ w[5];
        ctx->X[6] = X6 ^ w[6];
        ctx->X[7] = X7 ^ w[7];
        Skein_Show_Round(BLK_BITS,&ctx->h,SKEIN_RND_FEED_FWD,ctx->X);

        ts[1] &= ~SKEIN_T1_FLAG_FIRST;
        }
    while (--blkCnt);
    ctx->h.T[0] = ts[0];
    ctx->h.T[1] = ts[1];
    }
#endif

/*****************************************************************/
/*     512-bit Skein                                             */
/*****************************************************************/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* init the context for a straight hashing operation  */
static SkeinHashReturn Skein_512_Init(Skein_512_Ctxt_t *ctx, size_t hashBitLen)
{
    union
        {
        u08b_t  b[SKEIN_512_STATE_BYTES];
        u64b_t  w[SKEIN_512_STATE_WORDS];
        } cfg;                              /* config block */
        
    Skein_Assert(hashBitLen > 0,SKEIN_BAD_HASHLEN);
    ctx->h.hashBitLen = hashBitLen;         /* output hash bit count */

    switch (hashBitLen)
        {             /* use pre-computed values, where available */
#ifndef SKEIN_NO_PRECOMP
        case  256: memcpy(ctx->X,SKEIN_512_IV_256,sizeof(ctx->X));  break;
#endif
        default:
            /* here if there is no precomputed IV value available */
            /* build/process the config block, type == CONFIG (could be precomputed) */
            Skein_Start_New_Type(ctx,CFG_FINAL);        /* set tweaks: T0=0; T1=CFG | FINAL */

            cfg.w[0] = Skein_Swap64(SKEIN_SCHEMA_VER);  /* set the schema, version */
            cfg.w[1] = Skein_Swap64(hashBitLen);        /* hash result length in bits */
            cfg.w[2] = Skein_Swap64(SKEIN_CFG_TREE_INFO_SEQUENTIAL);
            memset(&cfg.w[3],0,sizeof(cfg) - 3*sizeof(cfg.w[0])); /* zero pad config block */

            /* compute the initial chaining values from config block */
            memset(ctx->X,0,sizeof(ctx->X));            /* zero the chaining variables */
            Skein_512_Process_Block(ctx,cfg.b,1,SKEIN_CFG_STR_LEN);
            break;
        }

    /* The chaining vars ctx->X are now initialized for the given hashBitLen. */
    /* Set up to process the data message portion of the hash (default) */
    Skein_Start_New_Type(ctx,MSG);              /* T0=0, T1= MSG type */

    return SKEIN_SUCCESS;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* process the input bytes */
static SkeinHashReturn Skein_512_Update(Skein_512_Ctxt_t *ctx, const u08b_t *msg, size_t msgByteCnt)
{
    size_t n;

    Skein_Assert(ctx->h.bCnt <= SKEIN_512_BLOCK_BYTES,SKEIN_FAIL);    /* catch uninitialized context */

    /* process full blocks, if any */
    if (msgByteCnt + ctx->h.bCnt > SKEIN_512_BLOCK_BYTES)
    {
        if (ctx->h.bCnt)                              /* finish up any buffered message data */
        {
            n = SKEIN_512_BLOCK_BYTES - ctx->h.bCnt;  /* # bytes free in buffer b[] */
            if (n)
            {
                Skein_assert(n < msgByteCnt);         /* check on our logic here */
                memcpy(&ctx->b[ctx->h.bCnt],msg,n);
                msgByteCnt  -= n;
                msg         += n;
                ctx->h.bCnt += n;
            }
            Skein_assert(ctx->h.bCnt == SKEIN_512_BLOCK_BYTES);
            Skein_512_Process_Block(ctx,ctx->b,1,SKEIN_512_BLOCK_BYTES);
            ctx->h.bCnt = 0;
        }
        /* now process any remaining full blocks, directly from input message data */
        if (msgByteCnt > SKEIN_512_BLOCK_BYTES)
        {
            n = (msgByteCnt-1) / SKEIN_512_BLOCK_BYTES;   /* number of full blocks to process */
            Skein_512_Process_Block(ctx,msg,n,SKEIN_512_BLOCK_BYTES);
            msgByteCnt -= n * SKEIN_512_BLOCK_BYTES;
            msg        += n * SKEIN_512_BLOCK_BYTES;
        }
        Skein_assert(ctx->h.bCnt == 0);
    }

    /* copy any remaining source message data bytes into b[] */
    if (msgByteCnt)
    {
        Skein_assert(msgByteCnt + ctx->h.bCnt <= SKEIN_512_BLOCK_BYTES);
        memcpy(&ctx->b[ctx->h.bCnt],msg,msgByteCnt);
        ctx->h.bCnt += msgByteCnt;
    }

    return SKEIN_SUCCESS;
}
   
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* finalize the hash computation and output the result */
static SkeinHashReturn Skein_512_Final(Skein_512_Ctxt_t *ctx, u08b_t *hashVal)
{
    size_t i,n,byteCnt;
    u64b_t X[SKEIN_512_STATE_WORDS];
    Skein_Assert(ctx->h.bCnt <= SKEIN_512_BLOCK_BYTES,SKEIN_FAIL);    /* catch uninitialized context */

    ctx->h.T[1] |= SKEIN_T1_FLAG_FINAL;                 /* tag as the final block */
    if (ctx->h.bCnt < SKEIN_512_BLOCK_BYTES)            /* zero pad b[] if necessary */
        memset(&ctx->b[ctx->h.bCnt],0,SKEIN_512_BLOCK_BYTES - ctx->h.bCnt);

    Skein_512_Process_Block(ctx,ctx->b,1,ctx->h.bCnt);  /* process the final block */
    
    /* now output the result */
    byteCnt = (ctx->h.hashBitLen + 7) >> 3;             /* total number of output bytes */

    /* run Threefish in "counter mode" to generate output */
    memset(ctx->b,0,sizeof(ctx->b));  /* zero out b[], so it can hold the counter */
    memcpy(X,ctx->X,sizeof(X));       /* keep a local copy of counter mode "key" */
    for (i=0;i*SKEIN_512_BLOCK_BYTES < byteCnt;i++)
    {
        ((u64b_t *)ctx->b)[0]= Skein_Swap64((u64b_t) i); /* build the counter block */
        Skein_Start_New_Type(ctx,OUT_FINAL);
        Skein_512_Process_Block(ctx,ctx->b,1,sizeof(u64b_t)); /* run "counter mode" */
        n = byteCnt - i*SKEIN_512_BLOCK_BYTES;   /* number of output bytes left to go */
        if (n >= SKEIN_512_BLOCK_BYTES)
            n  = SKEIN_512_BLOCK_BYTES;
        Skein_Put64_LSB_First(hashVal+i*SKEIN_512_BLOCK_BYTES,ctx->X,n);   /* "output" the ctr mode bytes */
        Skein_Show_Final(512,&ctx->h,n,hashVal+i*SKEIN_512_BLOCK_BYTES);
        memcpy(ctx->X,X,sizeof(X));   /* restore the counter mode key for next time */
    }
    return SKEIN_SUCCESS;
}

#if defined(SKEIN_CODE_SIZE) || defined(SKEIN_PERF)
static size_t Skein_512_API_CodeSize(void)
{
    return ((u08b_t *) Skein_512_API_CodeSize) -
           ((u08b_t *) Skein_512_Init);
}
#endif

typedef struct
{
    uint_t  statebits;                      /* 256, 512, or 1024 */
    union
    {
        Skein_Ctxt_Hdr_t h;                 /* common header "overlay" */
        Skein_512_Ctxt_t ctx_512;
    } u;
}
skeinHashState;

/* "incremental" hashing API */
static SkeinHashReturn SkeinInit  (skeinHashState *state, int hashbitlen);
static SkeinHashReturn SkeinUpdate(skeinHashState *state, const SkeinBitSequence *data, SkeinDataLength databitlen);
static SkeinHashReturn SkeinFinal (skeinHashState *state,       SkeinBitSequence *hashval);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* select the context size and init the context */
static SkeinHashReturn SkeinInit(skeinHashState *state, int hashbitlen)
{
    state->statebits = 64*SKEIN_512_STATE_WORDS;
    return Skein_512_Init(&state->u.ctx_512,(size_t) hashbitlen);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* process data to be hashed */
static SkeinHashReturn SkeinUpdate(skeinHashState *state, const SkeinBitSequence *data, SkeinDataLength databitlen)
{
    /* only the final Update() call is allowed do partial bytes, else assert an error */
    Skein_Assert((state->u.h.T[1] & SKEIN_T1_FLAG_BIT_PAD) == 0 || databitlen == 0, SKEIN_FAIL);

    Skein_Assert(state->statebits % 256 == 0 && (state->statebits-256) < 1024,SKEIN_FAIL);
    if ((databitlen & 7) == 0)  /* partial bytes? */
    {
        return Skein_512_Update(&state->u.ctx_512,data,databitlen >> 3);
    }
    else
    {   /* handle partial final byte */
        size_t bCnt = (databitlen >> 3) + 1;                  /* number of bytes to handle (nonzero here!) */
        u08b_t b,mask;

        mask = (u08b_t) (1u << (7 - (databitlen & 7)));       /* partial byte bit mask */
        b    = (u08b_t) ((data[bCnt-1] & (0-mask)) | mask);   /* apply bit padding on final byte */

        Skein_512_Update(&state->u.ctx_512,data,bCnt-1); /* process all but the final byte    */
        Skein_512_Update(&state->u.ctx_512,&b  ,  1   ); /* process the (masked) partial byte */
        Skein_Set_Bit_Pad_Flag(state->u.h);                    /* set tweak flag for the final call */

        return SKEIN_SUCCESS;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* finalize hash computation and output the result (hashbitlen bits) */
static SkeinHashReturn SkeinFinal(skeinHashState *state, SkeinBitSequence *hashval)
{
    Skein_Assert(state->statebits % 256 == 0 && (state->statebits-256) < 1024,FAIL);
    return Skein_512_Final(&state->u.ctx_512,hashval);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* all-in-one hash function */
SkeinHashReturn skein_hash(int hashbitlen, const SkeinBitSequence *data, /* all-in-one call */
                SkeinDataLength databitlen,SkeinBitSequence *hashval)
{
    skeinHashState  state;
    SkeinHashReturn r = SkeinInit(&state,hashbitlen);
    if (r == SKEIN_SUCCESS)
    { /* these calls do not fail when called properly */
        r = SkeinUpdate(&state,data,databitlen);
        SkeinFinal(&state,hashval);
    }
    return r;
}

void xmr_skein(const SkeinBitSequence *data, SkeinBitSequence *hashval){
#define XMR_HASHBITLEN 256
#define XMR_DATABITLEN 1600

    // Init
    skeinHashState  state;
    state.statebits = 64*SKEIN_512_STATE_WORDS;

    // Skein_512_Init(&state.u.ctx_512, (size_t)XMR_HASHBITLEN);
    state.u.ctx_512.h.hashBitLen = XMR_HASHBITLEN;
    memcpy(state.u.ctx_512.X,SKEIN_512_IV_256,sizeof(state.u.ctx_512.X));
    Skein_512_Ctxt_t* ctx = &(state.u.ctx_512);
    Skein_Start_New_Type(ctx,MSG);

    // Update
    if ((XMR_DATABITLEN & 7) == 0){  /* partial bytes? */
        Skein_512_Update(&state.u.ctx_512,data,XMR_DATABITLEN >> 3);
    }else{   /* handle partial final byte */
        size_t bCnt = (XMR_DATABITLEN >> 3) + 1;                  /* number of bytes to handle (nonzero here!) */
        u08b_t b,mask;

        mask = (u08b_t) (1u << (7 - (XMR_DATABITLEN & 7)));       /* partial byte bit mask */
        b    = (u08b_t) ((data[bCnt-1] & (0-mask)) | mask);   /* apply bit padding on final byte */

        Skein_512_Update(&state.u.ctx_512,data,bCnt-1); /* process all but the final byte    */
        Skein_512_Update(&state.u.ctx_512,&b  ,  1   ); /* process the (masked) partial byte */
        Skein_Set_Bit_Pad_Flag(state.u.h);                    /* set tweak flag for the final call */
    }

    // Finalize
    Skein_512_Final(&state.u.ctx_512, hashval);
}


#ifdef __cplusplus
}
#endif


static inline void do_blake_hash(const void* input, size_t len, char* output) {
    blake256_hash(reinterpret_cast<uint8_t*>(output), static_cast<const uint8_t*>(input), len);
}


static inline void do_groestl_hash(const void* input, size_t len, char* output) {
    groestl(static_cast<const uint8_t*>(input), len * 8, reinterpret_cast<uint8_t*>(output));
}


static inline void do_jh_hash(const void* input, size_t len, char* output) {
    jh_hash(32 * 8, static_cast<const uint8_t*>(input), 8 * len, reinterpret_cast<uint8_t*>(output));
}


static inline void do_skein_hash(const void* input, size_t len, char* output) {
    xmr_skein(static_cast<const uint8_t*>(input), reinterpret_cast<uint8_t*>(output));
}


void (* const extra_hashes[4])(const void *, size_t, char *) = {do_blake_hash, do_groestl_hash, do_jh_hash, do_skein_hash};


#if defined(__x86_64__) || defined(_M_AMD64)
#   define EXTRACT64(X) _mm_cvtsi128_si64(X)

#   ifdef __GNUC__
static inline uint64_t __umul128(uint64_t a, uint64_t b, uint64_t* hi)
{
    unsigned __int128 r = (unsigned __int128) a * (unsigned __int128) b;
    *hi = r >> 64;
    return (uint64_t) r;
}
#   else
    #define __umul128 _umul128
#   endif
#elif defined(__i386__) || defined(_M_IX86)
#   define HI32(X) \
    _mm_srli_si128((X), 4)


#   define EXTRACT64(X) \
    ((uint64_t)(uint32_t)_mm_cvtsi128_si32(X) | \
    ((uint64_t)(uint32_t)_mm_cvtsi128_si32(HI32(X)) << 32))

static inline uint64_t __umul128(uint64_t multiplier, uint64_t multiplicand, uint64_t *product_hi) {
    // multiplier   = ab = a * 2^32 + b
    // multiplicand = cd = c * 2^32 + d
    // ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
    uint64_t a = multiplier >> 32;
    uint64_t b = multiplier & 0xFFFFFFFF;
    uint64_t c = multiplicand >> 32;
    uint64_t d = multiplicand & 0xFFFFFFFF;

    //uint64_t ac = a * c;
    uint64_t ad = a * d;
    //uint64_t bc = b * c;
    uint64_t bd = b * d;

    uint64_t adbc = ad + (b * c);
    uint64_t adbc_carry = adbc < ad ? 1 : 0;

    // multiplier * multiplicand = product_hi * 2^64 + product_lo
    uint64_t product_lo = bd + (adbc << 32);
    uint64_t product_lo_carry = product_lo < bd ? 1 : 0;
    *product_hi = (a * c) + (adbc >> 32) + (adbc_carry << 32) + product_lo_carry;

    return product_lo;
}
#endif


// This will shift and xor tmp1 into itself as 4 32-bit vals such as
// sl_xor(a1 a2 a3 a4) = a1 (a2^a1) (a3^a2^a1) (a4^a3^a2^a1)
static inline __m128i sl_xor(__m128i tmp1)
{
    __m128i tmp4;
    tmp4 = _mm_slli_si128(tmp1, 0x04);
    tmp1 = _mm_xor_si128(tmp1, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    tmp1 = _mm_xor_si128(tmp1, tmp4);
    tmp4 = _mm_slli_si128(tmp4, 0x04);
    tmp1 = _mm_xor_si128(tmp1, tmp4);
    return tmp1;
}


template<uint8_t rcon>
static inline void aes_genkey_sub(__m128i* xout0, __m128i* xout2)
{
    __m128i xout1 = _mm_aeskeygenassist_si128(*xout2, rcon);
    xout1  = _mm_shuffle_epi32(xout1, 0xFF); // see PSHUFD, set all elems to 4th elem
    *xout0 = sl_xor(*xout0);
    *xout0 = _mm_xor_si128(*xout0, xout1);
    xout1  = _mm_aeskeygenassist_si128(*xout0, 0x00);
    xout1  = _mm_shuffle_epi32(xout1, 0xAA); // see PSHUFD, set all elems to 3rd elem
    *xout2 = sl_xor(*xout2);
    *xout2 = _mm_xor_si128(*xout2, xout1);
}


template<uint8_t rcon>
static inline void soft_aes_genkey_sub(__m128i* xout0, __m128i* xout2)
{
    __m128i xout1 = soft_aeskeygenassist<rcon>(*xout2);
    xout1  = _mm_shuffle_epi32(xout1, 0xFF); // see PSHUFD, set all elems to 4th elem
    *xout0 = sl_xor(*xout0);
    *xout0 = _mm_xor_si128(*xout0, xout1);
    xout1  = soft_aeskeygenassist<0x00>(*xout0);
    xout1  = _mm_shuffle_epi32(xout1, 0xAA); // see PSHUFD, set all elems to 3rd elem
    *xout2 = sl_xor(*xout2);
    *xout2 = _mm_xor_si128(*xout2, xout1);
}


template<bool SOFT_AES>
static inline void aes_genkey(const __m128i* memory, __m128i* k0, __m128i* k1, __m128i* k2, __m128i* k3, __m128i* k4, __m128i* k5, __m128i* k6, __m128i* k7, __m128i* k8, __m128i* k9)
{
    __m128i xout0 = _mm_load_si128(memory);
    __m128i xout2 = _mm_load_si128(memory + 1);
    *k0 = xout0;
    *k1 = xout2;

    SOFT_AES ? soft_aes_genkey_sub<0x01>(&xout0, &xout2) : aes_genkey_sub<0x01>(&xout0, &xout2);
    *k2 = xout0;
    *k3 = xout2;

    SOFT_AES ? soft_aes_genkey_sub<0x02>(&xout0, &xout2) : aes_genkey_sub<0x02>(&xout0, &xout2);
    *k4 = xout0;
    *k5 = xout2;

    SOFT_AES ? soft_aes_genkey_sub<0x04>(&xout0, &xout2) : aes_genkey_sub<0x04>(&xout0, &xout2);
    *k6 = xout0;
    *k7 = xout2;

    SOFT_AES ? soft_aes_genkey_sub<0x08>(&xout0, &xout2) : aes_genkey_sub<0x08>(&xout0, &xout2);
    *k8 = xout0;
    *k9 = xout2;
}


template<bool SOFT_AES>
static inline void aes_round(__m128i key, __m128i* x0, __m128i* x1, __m128i* x2, __m128i* x3, __m128i* x4, __m128i* x5, __m128i* x6, __m128i* x7)
{
    if (SOFT_AES) {
        *x0 = soft_aesenc((uint32_t*)x0, key);
        *x1 = soft_aesenc((uint32_t*)x1, key);
        *x2 = soft_aesenc((uint32_t*)x2, key);
        *x3 = soft_aesenc((uint32_t*)x3, key);
        *x4 = soft_aesenc((uint32_t*)x4, key);
        *x5 = soft_aesenc((uint32_t*)x5, key);
        *x6 = soft_aesenc((uint32_t*)x6, key);
        *x7 = soft_aesenc((uint32_t*)x7, key);
    }
    else {
        *x0 = _mm_aesenc_si128(*x0, key);
        *x1 = _mm_aesenc_si128(*x1, key);
        *x2 = _mm_aesenc_si128(*x2, key);
        *x3 = _mm_aesenc_si128(*x3, key);
        *x4 = _mm_aesenc_si128(*x4, key);
        *x5 = _mm_aesenc_si128(*x5, key);
        *x6 = _mm_aesenc_si128(*x6, key);
        *x7 = _mm_aesenc_si128(*x7, key);
    }
}


template<size_t MEM, bool SOFT_AES>
static inline void cn_explode_scratchpad(const __m128i *input, __m128i *output)
{
    __m128i xin0, xin1, xin2, xin3, xin4, xin5, xin6, xin7;
    __m128i k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;

    aes_genkey<SOFT_AES>(input, &k0, &k1, &k2, &k3, &k4, &k5, &k6, &k7, &k8, &k9);

    xin0 = _mm_load_si128(input + 4);
    xin1 = _mm_load_si128(input + 5);
    xin2 = _mm_load_si128(input + 6);
    xin3 = _mm_load_si128(input + 7);
    xin4 = _mm_load_si128(input + 8);
    xin5 = _mm_load_si128(input + 9);
    xin6 = _mm_load_si128(input + 10);
    xin7 = _mm_load_si128(input + 11);

    for (size_t i = 0; i < MEM / sizeof(__m128i); i += 8) {
        aes_round<SOFT_AES>(k0, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k1, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k2, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k3, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k4, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k5, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k6, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k7, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k8, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);
        aes_round<SOFT_AES>(k9, &xin0, &xin1, &xin2, &xin3, &xin4, &xin5, &xin6, &xin7);

        _mm_store_si128(output + i + 0, xin0);
        _mm_store_si128(output + i + 1, xin1);
        _mm_store_si128(output + i + 2, xin2);
        _mm_store_si128(output + i + 3, xin3);
        _mm_store_si128(output + i + 4, xin4);
        _mm_store_si128(output + i + 5, xin5);
        _mm_store_si128(output + i + 6, xin6);
        _mm_store_si128(output + i + 7, xin7);
    }
}


template<size_t MEM, bool SOFT_AES>
static inline void cn_implode_scratchpad(const __m128i *input, __m128i *output)
{
    __m128i xout0, xout1, xout2, xout3, xout4, xout5, xout6, xout7;
    __m128i k0, k1, k2, k3, k4, k5, k6, k7, k8, k9;

    aes_genkey<SOFT_AES>(output + 2, &k0, &k1, &k2, &k3, &k4, &k5, &k6, &k7, &k8, &k9);

    xout0 = _mm_load_si128(output + 4);
    xout1 = _mm_load_si128(output + 5);
    xout2 = _mm_load_si128(output + 6);
    xout3 = _mm_load_si128(output + 7);
    xout4 = _mm_load_si128(output + 8);
    xout5 = _mm_load_si128(output + 9);
    xout6 = _mm_load_si128(output + 10);
    xout7 = _mm_load_si128(output + 11);

    for (size_t i = 0; i < MEM / sizeof(__m128i); i += 8)
    {
        xout0 = _mm_xor_si128(_mm_load_si128(input + i + 0), xout0);
        xout1 = _mm_xor_si128(_mm_load_si128(input + i + 1), xout1);
        xout2 = _mm_xor_si128(_mm_load_si128(input + i + 2), xout2);
        xout3 = _mm_xor_si128(_mm_load_si128(input + i + 3), xout3);
        xout4 = _mm_xor_si128(_mm_load_si128(input + i + 4), xout4);
        xout5 = _mm_xor_si128(_mm_load_si128(input + i + 5), xout5);
        xout6 = _mm_xor_si128(_mm_load_si128(input + i + 6), xout6);
        xout7 = _mm_xor_si128(_mm_load_si128(input + i + 7), xout7);

        aes_round<SOFT_AES>(k0, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k1, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k2, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k3, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k4, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k5, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k6, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k7, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k8, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
        aes_round<SOFT_AES>(k9, &xout0, &xout1, &xout2, &xout3, &xout4, &xout5, &xout6, &xout7);
    }

    _mm_store_si128(output + 4, xout0);
    _mm_store_si128(output + 5, xout1);
    _mm_store_si128(output + 6, xout2);
    _mm_store_si128(output + 7, xout3);
    _mm_store_si128(output + 8, xout4);
    _mm_store_si128(output + 9, xout5);
    _mm_store_si128(output + 10, xout6);
    _mm_store_si128(output + 11, xout7);
}


template<size_t ITERATIONS, size_t MEM, size_t MASK, bool SOFT_AES>
inline void cryptonight_hash(const void *__restrict__ input, size_t size, void *__restrict__ output, cryptonight_ctx *__restrict__ ctx)
{
    keccak(static_cast<const uint8_t*>(input), (int) size, ctx->state0, 200);

    cn_explode_scratchpad<MEM, SOFT_AES>((__m128i*) ctx->state0, (__m128i*) ctx->memory);

    const uint8_t* l0 = ctx->memory;
    uint64_t* h0 = reinterpret_cast<uint64_t*>(ctx->state0);

    uint64_t al0 = h0[0] ^ h0[4];
    uint64_t ah0 = h0[1] ^ h0[5];
    __m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);

    uint64_t idx0 = h0[0] ^ h0[4];

    for (size_t i = 0; i < ITERATIONS; i++) {
        __m128i cx;

        if (SOFT_AES) {
            cx = soft_aesenc((uint32_t*)&l0[idx0 & MASK], _mm_set_epi64x(ah0, al0));
        }
        else {  
            cx = _mm_load_si128((__m128i *) &l0[idx0 & MASK]);
            cx = _mm_aesenc_si128(cx, _mm_set_epi64x(ah0, al0));
        }
        _mm_store_si128((__m128i *) &l0[idx0 & MASK], _mm_xor_si128(bx0, cx));
        idx0 = EXTRACT64(cx);
        bx0 = cx;

        uint64_t hi, lo, cl, ch;
        cl = ((uint64_t*) &l0[idx0 & MASK])[0];
        ch = ((uint64_t*) &l0[idx0 & MASK])[1];
        lo = __umul128(idx0, cl, &hi);

        al0 += hi;
        ah0 += lo;

        ((uint64_t*)&l0[idx0 & MASK])[0] = al0;
        ((uint64_t*)&l0[idx0 & MASK])[1] = ah0;

        ah0 ^= ch;
        al0 ^= cl;
        idx0 = al0;
    }

    cn_implode_scratchpad<MEM, SOFT_AES>((__m128i*) ctx->memory, (__m128i*) ctx->state0);

    keccakf(h0, 24);
    extra_hashes[ctx->state0[0] & 3](ctx->state0, 200, static_cast<char*>(output));
}


template<size_t ITERATIONS, size_t MEM, size_t MASK, bool SOFT_AES>
inline void cryptonight_double_hash(const void *__restrict__ input, size_t size, void *__restrict__ output, struct cryptonight_ctx *__restrict__ ctx)
{
    keccak((const uint8_t *) input,        (int) size, ctx->state0, 200);
    keccak((const uint8_t *) input + size, (int) size, ctx->state1, 200);

    const uint8_t* l0 = ctx->memory;
    const uint8_t* l1 = ctx->memory + MEM;
    uint64_t* h0 = reinterpret_cast<uint64_t*>(ctx->state0);
    uint64_t* h1 = reinterpret_cast<uint64_t*>(ctx->state1);

    cn_explode_scratchpad<MEM, SOFT_AES>((__m128i*) h0, (__m128i*) l0);
    cn_explode_scratchpad<MEM, SOFT_AES>((__m128i*) h1, (__m128i*) l1);

    uint64_t al0 = h0[0] ^ h0[4];
    uint64_t al1 = h1[0] ^ h1[4];
    uint64_t ah0 = h0[1] ^ h0[5];
    uint64_t ah1 = h1[1] ^ h1[5];

    __m128i bx0 = _mm_set_epi64x(h0[3] ^ h0[7], h0[2] ^ h0[6]);
    __m128i bx1 = _mm_set_epi64x(h1[3] ^ h1[7], h1[2] ^ h1[6]);

    uint64_t idx0 = h0[0] ^ h0[4];
    uint64_t idx1 = h1[0] ^ h1[4];

    for (size_t i = 0; i < ITERATIONS; i++) {
        __m128i cx0, cx1;

        if (SOFT_AES) {
            cx0 = soft_aesenc((uint32_t*)&l0[idx0 & MASK], _mm_set_epi64x(ah0, al0));
            cx1 = soft_aesenc((uint32_t*)&l1[idx1 & MASK], _mm_set_epi64x(ah1, al1));
        }
        else {
            cx0 = _mm_load_si128((__m128i *) &l0[idx0 & MASK]);
            cx1 = _mm_load_si128((__m128i *) &l1[idx1 & MASK]);
            cx0 = _mm_aesenc_si128(cx0, _mm_set_epi64x(ah0, al0));
            cx1 = _mm_aesenc_si128(cx1, _mm_set_epi64x(ah1, al1));
        }

        _mm_store_si128((__m128i *) &l0[idx0 & MASK], _mm_xor_si128(bx0, cx0));
        _mm_store_si128((__m128i *) &l1[idx1 & MASK], _mm_xor_si128(bx1, cx1));

        idx0 = EXTRACT64(cx0);
        idx1 = EXTRACT64(cx1);

        bx0 = cx0;
        bx1 = cx1;

        uint64_t hi, lo, cl, ch;
        cl = ((uint64_t*) &l0[idx0 & MASK])[0];
        ch = ((uint64_t*) &l0[idx0 & MASK])[1];
        lo = __umul128(idx0, cl, &hi);

        al0 += hi;
        ah0 += lo;

        ((uint64_t*) &l0[idx0 & MASK])[0] = al0;
        ((uint64_t*) &l0[idx0 & MASK])[1] = ah0;

        ah0 ^= ch;
        al0 ^= cl;
        idx0 = al0;

        cl = ((uint64_t*) &l1[idx1 & MASK])[0];
        ch = ((uint64_t*) &l1[idx1 & MASK])[1];
        lo = __umul128(idx1, cl, &hi);

        al1 += hi;
        ah1 += lo;

        ((uint64_t*) &l1[idx1 & MASK])[0] = al1;
        ((uint64_t*) &l1[idx1 & MASK])[1] = ah1;

        ah1 ^= ch;
        al1 ^= cl;
        idx1 = al1;
    }

    cn_implode_scratchpad<MEM, SOFT_AES>((__m128i*) l0, (__m128i*) h0);
    cn_implode_scratchpad<MEM, SOFT_AES>((__m128i*) l1, (__m128i*) h1);

    keccakf(h0, 24);
    keccakf(h1, 24);

    extra_hashes[ctx->state0[0] & 3](ctx->state0, 200, static_cast<char*>(output));
    extra_hashes[ctx->state1[0] & 3](ctx->state1, 200, static_cast<char*>(output) + 32);
}


static void cryptonight_av1_aesni(const void *input, size_t size, void *output, struct cryptonight_ctx *ctx) {
#   if !defined(XMRIG_ARMv7)
    cryptonight_hash<0x80000, MEMORY, 0x1FFFF0, false>(input, size, output, ctx);
#   endif
}


static void cryptonight_av2_aesni_double(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
#   if !defined(XMRIG_ARMv7)
    cryptonight_double_hash<0x80000, MEMORY, 0x1FFFF0, false>(input, size, output, ctx);
#   endif
}


static void cryptonight_av3_softaes(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    cryptonight_hash<0x80000, MEMORY, 0x1FFFF0, true>(input, size, output, ctx);
}


static void cryptonight_av4_softaes_double(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    cryptonight_double_hash<0x80000, MEMORY, 0x1FFFF0, true>(input, size, output, ctx);
}


#ifndef XMRIG_NO_AEON
static void cryptonight_lite_av1_aesni(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    #   if !defined(XMRIG_ARMv7)
    cryptonight_hash<0x40000, MEMORY_LITE, 0xFFFF0, false>(input, size, output, ctx);
#endif
}


static void cryptonight_lite_av2_aesni_double(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
#   if !defined(XMRIG_ARMv7)
    cryptonight_double_hash<0x40000, MEMORY_LITE, 0xFFFF0, false>(input, size, output, ctx);
#   endif
}


static void cryptonight_lite_av3_softaes(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    cryptonight_hash<0x40000, MEMORY_LITE, 0xFFFF0, true>(input, size, output, ctx);
}


static void cryptonight_lite_av4_softaes_double(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    cryptonight_double_hash<0x40000, MEMORY_LITE, 0xFFFF0, true>(input, size, output, ctx);
}

void (*cryptonight_variations[8])(const void *input, size_t size, void *output, cryptonight_ctx *ctx) = {
            cryptonight_av1_aesni,
            cryptonight_av2_aesni_double,
            cryptonight_av3_softaes,
            cryptonight_av4_softaes_double,
            cryptonight_lite_av1_aesni,
            cryptonight_lite_av2_aesni_double,
            cryptonight_lite_av3_softaes,
            cryptonight_lite_av4_softaes_double
        };
#else
void (*cryptonight_variations[4])(const void *input, size_t size, void *output, cryptonight_ctx *ctx) = {
            cryptonight_av1_aesni,
            cryptonight_av2_aesni_double,
            cryptonight_av3_softaes,
            cryptonight_av4_softaes_double
        };
#endif

}

static void cpuid(int CPUInfo[4], int InfoType)
{
    ASM __volatile__
    (
    "cpuid":
        "=a" (CPUInfo[0]),
        "=b" (CPUInfo[1]),
        "=c" (CPUInfo[2]),
        "=d" (CPUInfo[3]) :
            "a" (InfoType), "c" (0)
        );
}

static int check_aes_hw(void)
{
    int cpuid_results[4];
    static int supported = -1;

    if(supported >= 0)
        return supported;

    cpuid(cpuid_results,1);
    return supported = cpuid_results[2] & (1 << 25);
}

void cryptonight_hash_ctx(const void *input, size_t size, void *output, cryptonight_ctx *ctx)
{
    const int index = (check_aes_hw() ? 0 : 2);
    
    XMRig_cryptonight::cryptonight_variations[index](input, size, output, ctx);
}


