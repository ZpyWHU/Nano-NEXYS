#include "sm3.h"
#include <string.h>

// 常量
#define ROTL(x,n) (((x) << (n)) | ((x) >> (32 - (n))))
#define FF0(x,y,z) ((x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x)&(y)) | ((x)&(z)) | ((y)&(z)))
#define GG0(x,y,z) ((x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x)&(y)) | ((~(x)) & (z)))
#define P0(x) ((x) ^ ROTL((x),9) ^ ROTL((x),17))
#define P1(x) ((x) ^ ROTL((x),15) ^ ROTL((x),23))

static const uint32_t T[64] = {
    0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,
    0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,0x79cc4519,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,
    0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a,0x7a879d8a
};

static void sm3_compress(uint32_t digest[8], const uint8_t block[64])
{
    uint32_t W[68], W1[64], A,B,C,D,E,F,G,H;
    int j;

    for (j = 0; j < 16; j++) {
        W[j] = ((uint32_t)block[j*4] << 24) |
               ((uint32_t)block[j*4+1] << 16) |
               ((uint32_t)block[j*4+2] << 8) |
               ((uint32_t)block[j*4+3]);
    }

    for (j = 16; j < 68; j++) {
        W[j] = P1(W[j-16] ^ W[j-9] ^ ROTL(W[j-3],15)) ^ ROTL(W[j-13],7) ^ W[j-6];
    }

    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j+4];
    }

    A = digest[0]; B = digest[1]; C = digest[2]; D = digest[3];
    E = digest[4]; F = digest[5]; G = digest[6]; H = digest[7];

    for (j = 0; j < 64; j++) {
        uint32_t SS1 = ROTL((ROTL(A,12) + E + ROTL(T[j],j)) & 0xFFFFFFFF, 7);
        uint32_t SS2 = SS1 ^ ROTL(A,12);
        uint32_t TT1 = ((j<16)?FF0(A,B,C):FF1(A,B,C)) + D + SS2 + W1[j];
        uint32_t TT2 = ((j<16)?GG0(E,F,G):GG1(E,F,G)) + H + SS1 + W[j];
        D = C;
        C = ROTL(B,9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F,19);
        F = E;
        E = P0(TT2);
    }

    digest[0] ^= A;
    digest[1] ^= B;
    digest[2] ^= C;
    digest[3] ^= D;
    digest[4] ^= E;
    digest[5] ^= F;
    digest[6] ^= G;
    digest[7] ^= H;
}

void sm3(const uint8_t *msg, size_t len, uint8_t output[32])
{
    uint32_t digest[8] = {
        0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600,
        0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e
    };

    uint8_t buf[64];
    size_t i;
    uint64_t total = len * 8;

    while (len >= 64) {
        sm3_compress(digest, msg);
        msg += 64;
        len -= 64;
    }

    memset(buf, 0, 64);
    memcpy(buf, msg, len);
    buf[len] = 0x80;

    if (len > 55) {
        sm3_compress(digest, buf);
        memset(buf, 0, 64);
    }

    buf[56] = (total >> 56) & 0xFF;
    buf[57] = (total >> 48) & 0xFF;
    buf[58] = (total >> 40) & 0xFF;
    buf[59] = (total >> 32) & 0xFF;
    buf[60] = (total >> 24) & 0xFF;
    buf[61] = (total >> 16) & 0xFF;
    buf[62] = (total >> 8) & 0xFF;
    buf[63] = total & 0xFF;

    sm3_compress(digest, buf);

    for (i = 0; i < 8; i++) {
        output[i*4]   = (digest[i] >> 24) & 0xFF;
        output[i*4+1] = (digest[i] >> 16) & 0xFF;
        output[i*4+2] = (digest[i] >> 8) & 0xFF;
        output[i*4+3] = digest[i] & 0xFF;
    }
}
