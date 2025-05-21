#ifndef _SM3_H_
#define _SM3_H_

#include <stdint.h>
#include <stddef.h>

void sm3(const uint8_t *msg, size_t msglen, uint8_t digest[32]);

#endif
