/*
 * Copyright (c) 2013 Arttu Hynninen
 * Licensed under the MIT License. See the LICENSE file for the full text.
 */

#include <stdint.h>
#include "sha512.h"

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#pragma GCC optimize 0
	
#define GET_UINT64(b) ( \
	((uint64_t)(b)[0] << 56) | \
	((uint64_t)(b)[1] << 48) | \
	((uint64_t)(b)[2] << 40) | \
	((uint64_t)(b)[3] << 32) | \
	((uint64_t)(b)[4] << 24) | \
	((uint64_t)(b)[5] << 16) | \
	((uint64_t)(b)[6] <<  8) | \
	((uint64_t)(b)[7]      ))

#define PUT_UINT64(dst, x) { \
	(dst)[0] = (x) >> 56; \
	(dst)[1] = (x) >> 48; \
	(dst)[2] = (x) >> 40; \
	(dst)[3] = (x) >> 32; \
	(dst)[4] = (x) >> 24; \
	(dst)[5] = (x) >> 16; \
	(dst)[6] = (x) >>  8; \
	(dst)[7] = (x); }

#define ROTR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

#define S0(x) (ROTR(x, 1) ^ ROTR(x, 8) ^ ((x) >> 7))
#define S1(x) (ROTR(x, 19) ^ ROTR(x, 61) ^ ((x) >> 6))

#define T0(x) (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define T1(x) (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))

#define CH(a, b, c) (((a) & (b)) ^ ((~(a)) & (c)))
#define MAJ(a, b, c) (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))
#define WW(i) (w[i] = w[i - 16] + S0(w[i - 15]) + w[i - 7] + S1(w[i - 2]))

#define ROUND(a, b, c, d, e, f, g, h, k, w) { \
	uint64_t tmp0 = h + T1(e) + CH(e, f, g) + k + w; \
	uint64_t tmp1 = T0(a) + MAJ(a, b, c); \
	h = tmp0 + tmp1; \
	d += tmp0; }
	

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t n) {
    while (n-- > 0) *(dest++) = *(src++);
}

void memset(uint8_t *dest, uint8_t b, uint32_t n) {
    while (n-- > 0) *(dest++) = b;
}

void sha512_init(sha512_ctx *ctx)
{
	sha512_ctx tmp = SHA512_INIT;
	*ctx = tmp;
}

void sha512_chunk(sha512_ctx *ctx, const uint8_t chunk [128])
{
	const uint64_t rk [80] = {
		0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 
		0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 
		0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 
		0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 
		0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL, 
		0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 
		0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 
		0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL, 
		0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL, 
		0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 
		0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 
		0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL, 
		0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL, 
		0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 
		0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 
		0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL };
		
	uint64_t w [64];
	uint64_t a, b, c, d, e, f, g, h;
	
	int i;
	
	for (i = 0; i < 16; i++)
		w[i] = GET_UINT64(&chunk[8 * i]);
	
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];
	
	for (i = 0; i < 16; i += 8) {
		ROUND(a, b, c, d, e, f, g, h, rk[i    ], w[i    ]);
		ROUND(h, a, b, c, d, e, f, g, rk[i + 1], w[i + 1]);
		ROUND(g, h, a, b, c, d, e, f, rk[i + 2], w[i + 2]);
		ROUND(f, g, h, a, b, c, d, e, rk[i + 3], w[i + 3]);
		ROUND(e, f, g, h, a, b, c, d, rk[i + 4], w[i + 4]);
		ROUND(d, e, f, g, h, a, b, c, rk[i + 5], w[i + 5]);
		ROUND(c, d, e, f, g, h, a, b, rk[i + 6], w[i + 6]);
		ROUND(b, c, d, e, f, g, h, a, rk[i + 7], w[i + 7]);
	}
	
	for (i = 16; i < 80; i += 8) {
		ROUND(a, b, c, d, e, f, g, h, rk[i    ], WW(i    ));
		ROUND(h, a, b, c, d, e, f, g, rk[i + 1], WW(i + 1));
		ROUND(g, h, a, b, c, d, e, f, rk[i + 2], WW(i + 2));
		ROUND(f, g, h, a, b, c, d, e, rk[i + 3], WW(i + 3));
		ROUND(e, f, g, h, a, b, c, d, rk[i + 4], WW(i + 4));
		ROUND(d, e, f, g, h, a, b, c, rk[i + 5], WW(i + 5));
		ROUND(c, d, e, f, g, h, a, b, rk[i + 6], WW(i + 6));
		ROUND(b, c, d, e, f, g, h, a, rk[i + 7], WW(i + 7));
	}
	
	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void sha512_process(sha512_ctx *ctx, const uint8_t *data, uint32_t length)
{
	ctx->len += length;
		
	if (ctx->buflen != 0 && ctx->buflen + length >= 128) {
		int blen = 128 - ctx->buflen;
		memcpy(ctx->buffer + ctx->buflen, data, blen);
		sha512_chunk(ctx, ctx->buffer);
		data += blen;
		length -= blen;
		ctx->buflen = 0;
	}

	while (length >= 128) {
		sha512_chunk(ctx, data);
		data += 128;
		length -= 128;
	}
	
	if (length) {
		memcpy(ctx->buffer + ctx->buflen, data, length);
		ctx->buflen += length;
	}
}

void sha512_final(sha512_ctx *ctx, uint8_t out [64])
{
	const uint8_t fill [112] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	uint32_t flen = (ctx->buflen < 112) ? 112 - ctx->buflen : 240 - ctx->buflen;
	uint8_t buf [16];
	uint64_t hi_len = (uint64_t)(ctx->len >> 61);
	uint64_t lo_len = (uint64_t)(ctx->len << 3);
	
	int i;
	
	PUT_UINT64(&buf[0], hi_len);
	PUT_UINT64(&buf[8], lo_len);
	
	sha512_process(ctx, fill, flen);
	sha512_process(ctx, buf, 16);
	
	for (i = 0; i < 8; i++)
		PUT_UINT64(&out[8 * i], ctx->state[i]);
		
	memset((uint8_t*)ctx, 0, sizeof(sha512_ctx));
}

void sha512_simple(const uint8_t *data, uint32_t length, uint8_t out [64])
{
	sha512_ctx ctx = SHA512_INIT;
	sha512_process(&ctx, data, length);
	sha512_final(&ctx, out);
}
