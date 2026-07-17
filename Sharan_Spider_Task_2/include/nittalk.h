#ifndef NITTALK_H
#define NITTALK_H

#include <stdio.h>
#include <stdint.h>

#define BASE_INT 5
#define PRIME_NO 18446744073709551521ULL

typedef struct{
    char ver_bytes[4];
    char file_name[64];
    uint32_t file_size;
} __attribute__((packed)) file_header;

void server1(int num);
void client1(const char *target_ip_add, const char *filename, int port);
uint64_t generate_private_key();
uint64_t crypto_math(const uint64_t base_int, const uint64_t prime_no, const uint64_t exp_num);
uint8_t keystream_generator(uint64_t *common_key);

#endif