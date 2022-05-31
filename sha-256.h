#ifndef _SHA_256_H
#define _SHA_256_H

#include <stdint.h>


#define SHA_CONSTANTS 64
#define SHA_HASH_SIZE 8

// #define ROTL(a,b)   (((a) << (b)) | ((a) >> (32-(b)))) // only sha-1
#define ROTR(a,b)   (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z)   (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x)      (ROTR(x,2) ^ ROTR(x,13) ^ ROTR(x,22))
#define EP1(x)      (ROTR(x,6) ^ ROTR(x,11) ^ ROTR(x,25))
#define SIG0(x)     (ROTR(x,7) ^ ROTR(x,18) ^ ((x) >> 3))
#define SIG1(x)     (ROTR(x,17) ^ ROTR(x,19) ^ ((x) >> 10))

// structure for SHA256 digest
typedef struct{
    uint32_t *digest;
}s_SHA256_Digest;

// Prepare data for hashing
// Data must be a multiple of 512 bits, with termination bit and data length
uint8_t* sha256_PrepareData(uint8_t *data, uint64_t *size);
// Prepare W message
void sha256_PrepareMessage(uint32_t *W, uint8_t *data, uint64_t size);
// Execute SHA256 algorithm on data
uint32_t* sha256_Transform(uint8_t *data, uint64_t size);


#endif // _SHA_256_H
