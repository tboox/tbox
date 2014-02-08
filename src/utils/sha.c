/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed ip the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		sha.c
 * @ingroup 	utils
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sha.h"
#include "bits.h"
#include "../libc/libc.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define rol(v, b) 				(((v) << (b)) | ((v) >> (32 - (b))))

// (R0 + R1), R2, R3, R4 are the different operations used in SHA1
#define blk0(i) 				(block[i] = tb_bits_be_to_ne_u32(((tb_uint32_t const*)buffer)[i]))
#define blk(i)  				(block[i] = rol(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1))

#define R0(v, w, x, y, z, i) 	z += ((w & (x ^ y)) ^ y) + blk0(i) + 0x5a827999 + rol(v, 5); 		w = rol(w, 30);
#define R1(v, w, x, y, z, i) 	z += ((w &(x ^ y)) ^ y) + blk(i) + 0x5a827999 + rol(v, 5); 			w = rol(w, 30);
#define R2(v, w, x, y, z, i) 	z += (w ^ x ^ y) + blk(i) + 0x6ed9eba1 + rol(v, 5); 				w = rol(w, 30);
#define R3(v, w, x, y, z, i) 	z += (((w | x) & y) | (w & x)) + blk(i) + 0x8f1bbcdc + rol(v, 5); 	w = rol(w, 30);
#define R4(v, w, x, y, z, i) 	z += (w ^ x ^ y) + blk(i) + 0xca62c1d6 + rol(v, 5); 				w = rol(w, 30);

#define Ch(x,y,z) 				(((x) & ((y) ^ (z))) ^ (z))
#define Maj(x,y,z) 				((((x) | (y)) & (z)) | ((x) & (y)))

#define Sigma0_256(x) 			(rol((x), 30) ^ rol((x), 19) ^ rol((x), 10))
#define Sigma1_256(x) 			(rol((x), 26) ^ rol((x), 21) ^ rol((x),  7))
#define sigma0_256(x) 			(rol((x), 25) ^ rol((x), 14) ^ ((x) >> 3))
#define sigma1_256(x) 			(rol((x), 15) ^ rol((x), 13) ^ ((x) >> 10))

#undef blk
#define blk(i)  				(block[i] = block[i - 16] + sigma0_256(block[i - 15]) + sigma1_256(block[i - 2]) + block[i - 7])

#define ROUND256(a,b,c,d,e,f,g,h) \
	T1 += (h) + Sigma1_256(e) + Ch((e), (f), (g)) + g_sha_k256[i]; \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	i++

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h) \
	T1 = blk0(i); \
	ROUND256(a,b,c,d,e,f,g,h)

#define ROUND256_16_TO_63(a,b,c,d,e,f,g,h) \
	T1 = blk(i); \
	ROUND256(a,b,c,d,e,f,g,h)

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

static tb_uint32_t const g_sha_k256[64] = 
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_sha_transform_sha1(tb_uint32_t state[5], tb_uint8_t const buffer[64])
{
	tb_uint32_t 	block[80];
	tb_uint32_t 	i, a, b, c, d, e;

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

#ifdef __tb_small__
	for (i = 0; i < 80; i++)
	{
		tb_int_t t;
		if (i < 16) t = tb_bits_be_to_ne_u32(((tb_uint32_t*)buffer)[i]);
		else t = rol(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1);
		block[i] = t;
		t += e + rol(a, 5);

		if (i < 40) 
		{
			if (i < 20) t += ((b & (c ^ d)) ^ d) + 0x5a827999;
			else t += ( b ^ c ^ d) + 0x6ED9EBA1;
		} 
		else 
		{
			if (i < 60) t += (((b | c) & d) | (b & c)) + 0x8f1bbcdc;
			else t += ( b ^ c ^ d) + 0xca62c1d6;
		}

		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = t;
	}
#else

	for (i = 0; i < 15; i += 5)
	{
		R0(a, b, c, d, e, 0 + i);
		R0(e, a, b, c, d, 1 + i);
		R0(d, e, a, b, c, 2 + i);
		R0(c, d, e, a, b, 3 + i);
		R0(b, c, d, e, a, 4 + i);
	}

	R0(a, b, c, d, e, 15);
	R1(e, a, b, c, d, 16);
	R1(d, e, a, b, c, 17);
	R1(c, d, e, a, b, 18);
	R1(b, c, d, e, a, 19);

	for (i = 20; i < 40; i += 5)
	{
		R2(a, b, c, d, e, 0 + i);
		R2(e, a, b, c, d, 1 + i);
		R2(d, e, a, b, c, 2 + i);
		R2(c, d, e, a, b, 3 + i);
		R2(b, c, d, e, a, 4 + i);
	}
	for (; i < 60; i += 5)
	{
		R3(a, b, c, d, e, 0 + i);
		R3(e, a, b, c, d, 1 + i);
		R3(d, e, a, b, c, 2 + i);
		R3(c, d, e, a, b, 3 + i);
		R3(b, c, d, e, a, 4 + i);
	}
	for (; i < 80; i += 5)
	{
		R4(a, b, c, d, e, 0 + i);
		R4(e, a, b, c, d, 1 + i);
		R4(d, e, a, b, c, 2 + i);
		R4(c, d, e, a, b, 3 + i);
		R4(b, c, d, e, a, 4 + i);
	}
#endif

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
}

static tb_void_t tb_sha_transform_sha2(tb_uint32_t *state, tb_uint8_t const buffer[64])
{
	tb_uint32_t i, a, b, c, d, e, f, g, h;
	tb_uint32_t block[64];
	tb_uint32_t T1;

	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	f = state[5];
	g = state[6];
	h = state[7];

#ifdef __tb_small__
	tb_uint32_t T2;
	for (i = 0; i < 64; i++) 
	{
		if (i < 16) T1 = blk0(i);
		else T1 = blk(i);

		T1 += h + Sigma1_256(e) + Ch(e, f, g) + g_sha_k256[i];
		T2 = Sigma0_256(a) + Maj(a, b, c);

		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
	}
#else
	for (i = 0; i < 16; ) 
	{
		ROUND256_0_TO_15(a, b, c, d, e, f, g, h);
		ROUND256_0_TO_15(h, a, b, c, d, e, f, g);
		ROUND256_0_TO_15(g, h, a, b, c, d, e, f);
		ROUND256_0_TO_15(f, g, h, a, b, c, d, e);
		ROUND256_0_TO_15(e, f, g, h, a, b, c, d);
		ROUND256_0_TO_15(d, e, f, g, h, a, b, c);
		ROUND256_0_TO_15(c, d, e, f, g, h, a, b);
		ROUND256_0_TO_15(b, c, d, e, f, g, h, a);
	}

	for ( ; i < 64; ) 
	{
		ROUND256_16_TO_63(a, b, c, d, e, f, g, h);
		ROUND256_16_TO_63(h, a, b, c, d, e, f, g);
		ROUND256_16_TO_63(g, h, a, b, c, d, e, f);
		ROUND256_16_TO_63(f, g, h, a, b, c, d, e);
		ROUND256_16_TO_63(e, f, g, h, a, b, c, d);
		ROUND256_16_TO_63(d, e, f, g, h, a, b, c);
		ROUND256_16_TO_63(c, d, e, f, g, h, a, b);
		ROUND256_16_TO_63(b, c, d, e, f, g, h, a);
	}
#endif

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;
}


/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */


tb_void_t tb_sha_init(tb_sha_t* sha, tb_size_t mode)
{
	tb_memset(sha, 0, sizeof(tb_sha_t));
	sha->digest_len = (mode >> 5) & 0xff;
	switch (mode) 
	{
	case TB_SHA_MODE_SHA1_160: // sha-1
		sha->state[0] = 0x67452301;
		sha->state[1] = 0xefcdab89;
		sha->state[2] = 0x98badcfe;
		sha->state[3] = 0x10325476;
		sha->state[4] = 0xc3d2e1f0;
		sha->transform = tb_sha_transform_sha1;
		break;
	case TB_SHA_MODE_SHA2_224: // sha-224
		sha->state[0] = 0xc1059ed8;
		sha->state[1] = 0x367cd507;
		sha->state[2] = 0x3070dd17;
		sha->state[3] = 0xf70e5939;
		sha->state[4] = 0xffc00b31;
		sha->state[5] = 0x68581511;
		sha->state[6] = 0x64f98fa7;
		sha->state[7] = 0xbefa4fa4;
		sha->transform = tb_sha_transform_sha2;
		break;
	case TB_SHA_MODE_SHA2_256: // sha-256
		sha->state[0] = 0x6a09e667;
		sha->state[1] = 0xbb67ae85;
		sha->state[2] = 0x3c6ef372;
		sha->state[3] = 0xa54ff53a;
		sha->state[4] = 0x510e527f;
		sha->state[5] = 0x9b05688c;
		sha->state[6] = 0x1f83d9ab;
		sha->state[7] = 0x5be0cd19;
		sha->transform = tb_sha_transform_sha2;
		break;
	default:
		tb_assert(0);
		break;
	}
	sha->count = 0;
}
tb_void_t tb_sha_exit(tb_sha_t* sha, tb_byte_t* data, tb_size_t size)
{
	tb_hize_t count = tb_bits_be_to_ne_u64(sha->count << 3);

	tb_sha_spak(sha, "\200", 1);

	while ((sha->count & 63) != 56)
		tb_sha_spak(sha, "", 1);

	tb_sha_spak(sha, (tb_byte_t*)&count, 8);

	tb_uint32_t i = 0;
	tb_uint32_t n = sha->digest_len;
	tb_assert((n << 2) <= size);
	for (i = 0; i < n; i++)
		tb_bits_set_u32_be(data + (i << 2), sha->state[i]);
}
tb_void_t tb_sha_spak(tb_sha_t* sha, tb_byte_t const* data, tb_size_t size)
{
	tb_uint32_t i, j;

	j = (tb_uint32_t)sha->count & 63;
	sha->count += size;

#ifdef __tb_small__
	for (i = 0; i < size; i++) 
	{
		sha->buffer[j++] = data[i];
		if (64 == j) 
		{
			sha->transform(sha->state, sha->buffer);
			j = 0;
		}
	}
#else
	if ((j + size) > 63)
	{
		tb_memcpy(&sha->buffer[j], data, (i = 64 - j));
		sha->transform(sha->state, sha->buffer);
		for (; i + 63 < size; i += 64)
			sha->transform(sha->state, &data[i]);
		j = 0;
	} 
	else i = 0;
	tb_memcpy(&sha->buffer[j], &data[i], size - i);
#endif
}

tb_size_t tb_sha_encode(tb_size_t mode, tb_byte_t const* ib, tb_size_t in, tb_byte_t* ob, tb_size_t on)
{
	tb_sha_t sha;

	// check
	tb_assert_and_check_return_val(ib && in && ob && on >= 16, 0);

	// init 
	tb_sha_init(&sha, mode);

	// spank
	tb_sha_spak(&sha, ib, in);

	// exit
	tb_sha_exit(&sha, ob, on);

	return (sha.digest_len << 2);
}

