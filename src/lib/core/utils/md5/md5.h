#ifndef MD5_H
#define MD5_H

#include <stddef.h>
#include <stdint.h>

#define MD5_DIGEST_STRING_LENGTH 17

typedef struct MD5Context {
    size_t input_size;
    uint32_t buffer[4];
    uint8_t input[64];
    uint8_t digest[16];
} MD5Context;

void md5_init(MD5Context* ctx);
void md5_update(MD5Context* ctx, uint8_t* buffer, size_t buffer_length);
void md5_final(MD5Context* ctx);

void md5_string(char* input, size_t input_length, char digest[MD5_DIGEST_STRING_LENGTH]);

#endif
