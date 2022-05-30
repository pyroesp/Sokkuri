#include "sha-256.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// hash k constant
const uint32_t k[SHA_CONSTANTS] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

// starting hash values
const uint32_t hash[SHA_HASH_SIZE] = {
    0x6A09E667,
    0xBB67AE85,
    0x3C6EF372,
    0xA54FF53A,
    0x510E527F,
    0x9B05688C,
    0x1F83D9AB,
    0x5BE0CD19
};


uint8_t* sha256_PrepareData(uint8_t *data, uint32_t *size){
    uint32_t t = *size;
    uint32_t bit_idx = t;
    uint64_t length = t * 8;

    // check if data size is a multiple of 512 bits (or 64 bytes)
    if ((t % 64) != 0){
        // if not then increase data buffer to be a multiple of 512 bits
        t = t + (64 - (t % 64));
        data = (uint8_t*)realloc(data, sizeof(uint8_t) * t);
        memset(&data[*size], 0, sizeof(uint8_t) * (t - (*size)));
    }

    // check if there's enough space to add termination bit and data length
    if (t - (*size) <= 8){
        // if not, then increase data buffer with 64 extra bytes
        t += 64;
        data = (uint8_t*)realloc(data, sizeof(uint8_t) * t);
        memset(&data[*size], 0, sizeof(uint8_t) * 64);
    }

    // set termination bit at the end of the original data buffer
    data[bit_idx] = 0x80;
    // set the original data length, in bits, to the last 8 bytes of the increased data buffer
    data[t - 1] = (uint8_t)(length >> (0 * 8) & 0xFF);
    data[t - 2] = (uint8_t)(length >> (1 * 8) & 0xFF);
    data[t - 3] = (uint8_t)(length >> (2 * 8) & 0xFF);
    data[t - 4] = (uint8_t)(length >> (3 * 8) & 0xFF);
    data[t - 5] = (uint8_t)(length >> (4 * 8) & 0xFF);
    data[t - 6] = (uint8_t)(length >> (5 * 8) & 0xFF);
    data[t - 7] = (uint8_t)(length >> (6 * 8) & 0xFF);
    data[t - 8] = (uint8_t)(length >> (7 * 8) & 0xFF);

    // change the value of the data buffer to the new size
    *size = t;
    return data;
}


void sha256_PrepareMessage(uint32_t *W, uint8_t *data, uint32_t size){
    uint32_t i;
    uint32_t idx;

    memset(W, 0, 64); // clear W message
    for (i = 0, idx = 0; i < size/4; i++, idx += 4){
        // make 16 32 bit values from the data buffer
        W[i] = (data[idx] << 24) | (data[idx + 1] << 16) | (data[idx + 2] << 8) | data[idx + 3];
    }
}

uint32_t* sha256_Transform(uint8_t *data, uint32_t size){
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    uint32_t W[64];
    uint32_t *H; // pointer for starting/final hash

    uint32_t idx, i;

    // malloc hash buffer
    H = (uint32_t*)malloc(sizeof(uint32_t) * SHA_HASH_SIZE);
    memcpy(H, hash, sizeof(uint32_t) * SHA_HASH_SIZE);

    idx = 0;
    while(idx < size){ // loop to hash all data
        sha256_PrepareMessage(W, &data[idx], 64); // prepare W
        // do SHA256 algorithm
        for (i = 16; i < 64; i++)
            W[i] = SIG1(W[i - 2]) + W[i - 7] + SIG0(W[i - 15]) + W[i - 16];

        a = H[0];
        b = H[1];
        c = H[2];
        d = H[3];
        e = H[4];
        f = H[5];
        g = H[6];
        h = H[7];

        for (i = 0; i < 64; i++){
            t1 = h + EP1(e) + CH(e, f, g) + k[i] + W[i];
            t2 = EP0(a) + MAJ(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;

        idx += 64; // increase index by 64 bytes
    }
    return H; // H contains the hash of the file data (aka SHA256 digest)
}
