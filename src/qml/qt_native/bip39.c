#include "bip39.h"



static secp256k1_context *_ctx = NULL;
static pthread_once_t _ctx_once = PTHREAD_ONCE_INIT;

static void _ctx_init()
{
    _ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
}


static void BIP39SHA512Compress(uint64_t *r, const uint64_t *x)
{
    static const uint64_t k[] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
        0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
        0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
        0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
        0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
        0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
        0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
        0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
        0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
        0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
        0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
        0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
        0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
    };

    int i;
    uint64_t a = r[0], b = r[1], c = r[2], d = r[3], e = r[4], f = r[5], g = r[6], h = r[7], t1, t2, w[80];

    for (i = 0; i < 16; i++) w[i] = be64(x[i]);
    for (; i < 80; i++) w[i] = S3(w[i - 2]) + w[i - 7] + S2(w[i - 15]) + w[i - 16];

    for (i = 0; i < 80; i++) {
        t1 = h + S1(e) + ch(e, f, g) + k[i] + w[i];
        t2 = S0(a) + maj(a, b, c);
        h = g, g = f, f = e, e = d + t1, d = c, c = b, b = a, a = t1 + t2;
    }

    r[0] += a, r[1] += b, r[2] += c, r[3] += d, r[4] += e, r[5] += f, r[6] += g, r[7] += h;
    var_clean(&a, &b, &c, &d, &e, &f, &g, &h, &t1, &t2);
    mem_clean(w, sizeof(w));
}

void BIP39SHA512(void *md64, const void *data, size_t dataLen)
{
    size_t i;
    uint64_t x[16], buf[] = { 0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                              0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179 };

    assert(md64 != NULL);
    assert(data != NULL || dataLen == 0);

    for (i = 0; i < dataLen; i += 128) { // process data in 128 byte blocks
        memcpy(x, (const uint8_t *)data + i, (i + 128 < dataLen) ? 128 : dataLen - i);
        if (i + 128 > dataLen) break;
        BIP39SHA512Compress(buf, x);
    }

    memset((uint8_t *)x + (dataLen - i), 0, 128 - (dataLen - i)); // clear remainder of x
    ((uint8_t *)x)[dataLen - i] = 0x80; // append padding
    if (dataLen - i >= 112) BIP39SHA512Compress(buf, x), memset(x, 0, 128); // length goes to next block
    x[14] = 0, x[15] = be64((uint64_t)dataLen*8); // append length in bits
    BIP39SHA512Compress(buf, x); // finalize
    for (i = 0; i < 8; i++) buf[i] = be64(buf[i]); // endian swap
    memcpy(md64, buf, 64); // write to md
    mem_clean(x, sizeof(x));
    mem_clean(buf, sizeof(buf));
}

static void _BRSHA512Compress(uint64_t *r, const uint64_t *x)
{
    static const uint64_t k[] = {
        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
        0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
        0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
        0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
        0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
        0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
        0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
        0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
        0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
        0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
        0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
        0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
        0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
    };
    
    int i;
    uint64_t a = r[0], b = r[1], c = r[2], d = r[3], e = r[4], f = r[5], g = r[6], h = r[7], t1, t2, w[80];
    
    for (i = 0; i < 16; i++) w[i] = be64(x[i]);
    for (; i < 80; i++) w[i] = S3(w[i - 2]) + w[i - 7] + S2(w[i - 15]) + w[i - 16];
    
    for (i = 0; i < 80; i++) {
        t1 = h + S1(e) + ch(e, f, g) + k[i] + w[i];
        t2 = S0(a) + maj(a, b, c);
        h = g, g = f, f = e, e = d + t1, d = c, c = b, b = a, a = t1 + t2;
    }
    
    r[0] += a, r[1] += b, r[2] += c, r[3] += d, r[4] += e, r[5] += f, r[6] += g, r[7] += h;
    var_clean(&a, &b, &c, &d, &e, &f, &g, &h, &t1, &t2);
    mem_clean(w, sizeof(w));
}

void BRSHA512(void *md64, const void *data, size_t dataLen)
{
    size_t i;
    uint64_t x[16], buf[] = { 0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                              0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179 };
    
    assert(md64 != NULL);
    assert(data != NULL || dataLen == 0);

    for (i = 0; i < dataLen; i += 128) { // process data in 128 byte blocks
        memcpy(x, (const uint8_t *)data + i, (i + 128 < dataLen) ? 128 : dataLen - i);
        if (i + 128 > dataLen) break;
        _BRSHA512Compress(buf, x);
    }
    
    memset((uint8_t *)x + (dataLen - i), 0, 128 - (dataLen - i)); // clear remainder of x
    ((uint8_t *)x)[dataLen - i] = 0x80; // append padding
    if (dataLen - i >= 112) _BRSHA512Compress(buf, x), memset(x, 0, 128); // length goes to next block
    x[14] = 0, x[15] = be64((uint64_t)dataLen*8); // append length in bits
    _BRSHA512Compress(buf, x); // finalize
    for (i = 0; i < 8; i++) buf[i] = be64(buf[i]); // endian swap
    memcpy(md64, buf, 64); // write to md
    mem_clean(x, sizeof(x));
    mem_clean(buf, sizeof(buf));
}

void BIP39HMAC(void *mac, void (*hash)(void *, const void *, size_t), size_t hashLen, const void *key, size_t keyLen,
            const void *data, size_t dataLen)
{
    size_t i, blockLen = (hashLen > 32) ? 128 : 64;
    uint8_t k[hashLen];
    uint64_t kipad[(blockLen + dataLen)/sizeof(uint64_t) + 1], kopad[(blockLen + hashLen)/sizeof(uint64_t) + 1];

    assert(mac != NULL);
    assert(hash != NULL);
    assert(hashLen > 0 && (hashLen % 4) == 0);
    assert(key != NULL || keyLen == 0);
    assert(data != NULL || dataLen == 0);

    if (keyLen > blockLen) hash(k, key, keyLen), key = k, keyLen = sizeof(k);
    memset(kipad, 0, blockLen);
    memcpy(kipad, key, keyLen);
    for (i = 0; i < blockLen/sizeof(uint64_t); i++) kipad[i] ^= 0x3636363636363636;
    memset(kopad, 0, blockLen);
    memcpy(kopad, key, keyLen);
    for (i = 0; i < blockLen/sizeof(uint64_t); i++) kopad[i] ^= 0x5c5c5c5c5c5c5c5c;
    memcpy(&kipad[blockLen/sizeof(uint64_t)], data, dataLen);
    hash(&kopad[blockLen/sizeof(uint64_t)], kipad, blockLen + dataLen);
    hash(mac, kopad, blockLen + hashLen);

    mem_clean(k, sizeof(k));
    mem_clean(kipad, blockLen);
    mem_clean(kopad, blockLen);
}

int hex2byte(char *dst, const char *src)
{
    while(*src) {
        if(' ' == *src) {
            src++;
            continue;
        }
        sscanf(src, "%02X", dst);
        src += 2;
        dst++;
    }
    return 0;
}


void PBKDF2(void *dk, size_t dkLen, void (*hash)(void *, const void *, size_t), size_t hashLen,
              const void *pw, size_t pwLen, const void *salt, size_t saltLen, unsigned rounds)
{
    uint8_t s[saltLen + sizeof(uint32_t)];
    uint32_t i, j, U[hashLen/sizeof(uint32_t)], T[hashLen/sizeof(uint32_t)];

    assert(dk != NULL || dkLen == 0);
    assert(hash != NULL);
    assert(hashLen > 0 && (hashLen % 4) == 0);
    assert(pw != NULL || pwLen == 0);
    assert(salt != NULL || saltLen == 0);
    assert(rounds > 0);

    memcpy(s, salt, saltLen);

    for (i = 0; i < (dkLen + hashLen - 1)/hashLen; i++) {
        j = be32(i + 1);
        memcpy(s + saltLen, &j, sizeof(j));
        BIP39HMAC(U, hash, hashLen, pw, pwLen, s, sizeof(s)); // U1 = hmac_hash(pw, salt || be32(i))
        memcpy(T, U, sizeof(U));

        for (unsigned r = 1; r < rounds; r++) {
            BIP39HMAC(U, hash, hashLen, pw, pwLen, U, sizeof(U)); // Urounds = hmac_hash(pw, Urounds-1)
            for (j = 0; j < hashLen/sizeof(uint32_t); j++) T[j] ^= U[j]; // Ti = U1 ^ U2 ^ ... ^ Urounds
        }

        // dk = T1 || T2 || ... || Tdklen/hlen
        memcpy((uint8_t *)dk + i*hashLen, T, (i*hashLen + hashLen <= dkLen) ? hashLen : dkLen % hashLen);
    }

    mem_clean(s, sizeof(s));
    mem_clean(U, sizeof(U));
    mem_clean(T, sizeof(T));
}

int Secp256k1PointGen(unsigned char *p, const unsigned char *i)
{
    secp256k1_pubkey pubkey;
    size_t pLen = 33;//size of BRECPoint from BRChain

    pthread_once(&_ctx_once, _ctx_init);
    return (secp256k1_ec_pubkey_create(_ctx, &pubkey, (const unsigned char *)i) &&
            secp256k1_ec_pubkey_serialize(_ctx, (unsigned char *)p, &pLen, &pubkey, SECP256K1_EC_COMPRESSED));
}


inline static void UInt32SetBE(void *b4, uint32_t u)
{
    *(union _u32 { uint8_t u8[32/8]; } *)b4 =
        (union _u32) { (u >> 24) & 0xff, (u >> 16) & 0xff, (u >> 8) & 0xff, u & 0xff };
}

int Secp256k1ModAdd(unsigned char *a, const unsigned char *b)
{
    pthread_once(&_ctx_once, _ctx_init);
    return secp256k1_ec_privkey_tweak_add(_ctx, a, b);
}


void CKDpriv(uint8_t *k, uint8_t *c, uint32_t i)
{
    uint8_t buf[37] = {0};
    uint8_t I[64] = {0};

    if (i & BIP32_HARD) {
        buf[0] = 0;
        memcpy(buf + 1,k ,32);
    }
    else Secp256k1PointGen((unsigned char *)buf, (const unsigned char *)k);

    UInt32SetBE(&buf[33], i);

    BIP39HMAC(I, BIP39SHA512, 64, c, 32, buf, sizeof(buf)); // I = HMAC-SHA512(c, k|P(k) || i)

    Secp256k1ModAdd((unsigned char *)k, (const unsigned char *)I); // k = IL + k (mod n)

    memcpy(c,I + 32, 32);

    var_clean(&I);
    mem_clean(buf, sizeof(buf));
}
