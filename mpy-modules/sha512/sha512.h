/*
 * Copyright (c) 2013 Arttu Hynninen
 * Licensed under the MIT License. See the LICENSE file for the full text.
 */

#pragma once
#include <stdint.h>

typedef struct _sha512_ctx {
	uint64_t len;
	uint64_t state [8];
	uint8_t buflen;
	uint8_t buffer [128];
} sha512_ctx;

/*
 * Macros for static initialization
 */

#define SHA512_INIT_STATE { \
	0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL, \
	0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL }

#define SHA512_INIT_BUFFER { \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define SHA512_INIT { 0, SHA512_INIT_STATE, 0, SHA512_INIT_BUFFER }

/*
 * Initializes the context.
 */
void sha512_init(sha512_ctx *ctx);

/*
 * Processes the input buffer.
 */
void sha512_process(sha512_ctx *ctx, const uint8_t *data, uint32_t length);

/*
 * Finalizes and outputs the calculated hash into the buffer.
 */
void sha512_final(sha512_ctx *ctx, uint8_t out [64]);

/*
 * Digests the input buffer and outputs the calculated hash in one function call.
 */
void sha512_simple(const uint8_t *data, uint32_t length, uint8_t out [64]);
