#ifndef BIP39_H
#define BIP39_H









#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <../secp256k1/include/secp256k1.h>

// endian swapping
#if __BIG_ENDIAN__ || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define be32(x) (x)
#define le32(x) ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#define be64(x) (x)
#define le64(x) ((union { uint32_t u32[2]; uint64_t u64; }) { le32((uint32_t)(x)), le32((uint32_t)((x) >> 32)) }.u64)
#elif __LITTLE_ENDIAN__ || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define le32(x) (x)
#define be32(x) ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#define le64(x) (x)
#define be64(x) ((union { uint32_t u32[2]; uint64_t u64; }) { be32((uint32_t)((x) >> 32)), be32((uint32_t)(x)) }.u64)
#else // unknown endianess
#define be32(x) ((union { uint8_t u8[4]; uint32_t u32; }) { (x) >> 24, (x) >> 16, (x) >> 8, (x) }.u32)
#define le32(x) ((union { uint8_t u8[4]; uint32_t u32; }) { (x), (x) >> 8, (x) >> 16, (x) >> 24 }.u32)
#define be64(x) ((union { uint32_t u32[2]; uint64_t u64; }) { be32((uint32_t)((x) >> 32)), be32((uint32_t)(x)) }.u64)
#define le64(x) ((union { uint32_t u32[2]; uint64_t u64; }) { le32((uint32_t)(x)), le32((uint32_t)((x) >> 32)) }.u64)
#endif

// bitwise left rotation
#define rol32(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

// basic sha1 functions
#define f1(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define f2(x, y, z) ((x) ^ (y) ^ (z))
#define f3(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))

// basic sha1 operation
#define sha1(x, y, z) (t = rol32(a, 5) + (x) + e + (y) + (z), e = d, d = c, c = rol32(b, 30), b = a, a = t)

// bitwise right rotation
#define ror32(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

// basic sha2 functions
#define ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

// basic sha256 functions
#define s0(x) (ror32((x), 2) ^ ror32((x), 13) ^ ror32((x), 22))
#define s1(x) (ror32((x), 6) ^ ror32((x), 11) ^ ror32((x), 25))
#define s2(x) (ror32((x), 7) ^ ror32((x), 18) ^ ((x) >> 3))
#define s3(x) (ror32((x), 17) ^ ror32((x), 19) ^ ((x) >> 10))

// bitwise right rotation
#define ror64(a, b) (((a) >> (b)) | ((a) << (64 - (b))))

// basic sha512 opeartions
#define S0(x) (ror64((x), 28) ^ ror64((x), 34) ^ ror64((x), 39))
#define S1(x) (ror64((x), 14) ^ ror64((x), 18) ^ ror64((x), 41))
#define S2(x) (ror64((x), 1) ^ ror64((x), 8) ^ ((x) >> 7))
#define S3(x) (ror64((x), 19) ^ ror64((x), 61) ^ ((x) >> 6))

#define BIP32_SEED_KEY "Bitcoin seed"
#define BIP32_HARD                  0x80000000

typedef struct {
    char secret[64];
    uint8_t pubKey[65];
    int compressed;
} Key;


inline static void mem_clean(void *ptr, size_t len)
{
    void *(*volatile const memset_ptr)(void *, int, size_t) = memset;
    memset_ptr(ptr, 0, len);
}

#define var_clean(...) _var_clean(sizeof(*(_va_first(__VA_ARGS__))), __VA_ARGS__, NULL)
#define _va_first(first, ...) first

inline static void _var_clean(size_t size, ...)
{
    va_list args;
    va_start(args, size);
    for (void *ptr = va_arg(args, void *); ptr; ptr = va_arg(args, void *)) mem_clean(ptr, size);
    va_end(args);
}

static void BIP39SHA512Compress(uint64_t *r, const uint64_t *x);
void BIP39SHA512(void *md64, const void *data, size_t dataLen);
void BRSHA512(void *md64, const void *data, size_t dataLen);

void BIP39HMAC(void *mac, void (*hash)(void *, const void *, size_t), size_t hashLen, const void *key, size_t keyLen,
            const void *data, size_t dataLen);

int hex2byte(char *dst, const char *src) ;

void PBKDF2(void *dk, size_t dkLen, void (*hash)(void *, const void *, size_t), size_t hashLen,
              const void *pw, size_t pwLen, const void *salt, size_t saltLen, unsigned rounds);

void CKDpriv(uint8_t *k, uint8_t *c, uint32_t i);
int Secp256k1PointGen(unsigned char *p, const unsigned char *i);

#ifdef __cplusplus
}
#endif

#endif // BIP39_H
