/*
 *Base64Encode/Base64Decode
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "base64.h"

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

typedef unsigned int UINT4;

static void Encode(unsigned char *output, const UINT4 *input,
           unsigned int len);
static void Decode(UINT4 *output, const unsigned char *input,
           unsigned int len);

/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] =
{
#ifndef CHARSET_EBCDIC
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
#else /*CHARSET_EBCDIC*/
    /* EBCDIC table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 63, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 64, 64, 64, 64, 64, 64,
    64, 35, 36, 37, 38, 39, 40, 41, 42, 43, 64, 64, 64, 64, 64, 64,
    64, 64, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8, 64, 64, 64, 64, 64, 64,
    64,  9, 10, 11, 12, 13, 14, 15, 16, 17, 64, 64, 64, 64, 64, 64,
    64, 64, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64
#endif /*CHARSET_EBCDIC*/
};

int Base64DecodeLen(const char *bufcoded)
{
    int nbytesdecoded;
    register const unsigned char *bufin;
    register int nprbytes;

    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);

    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
    int nCover = strlen(bufcoded) - nprbytes;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    return nbytesdecoded - nCover;
}

int Base64Decode(char *bufplain, const char *bufcoded)
{
#ifdef CHARSET_EBCDIC
    int i;
#endif              /* CHARSET_EBCDIC */
    int len;
    
    len = Base64DecodeBinary((unsigned char *) bufplain, bufcoded);
#ifdef CHARSET_EBCDIC
    for (i = 0; i < len; i++)
    bufplain[i] = os_toebcdic[bufplain[i]];
#endif              /* CHARSET_EBCDIC */
    //bufplain[len] = '\0';
    return len;
}

/* This is the same as ap_base64udecode() except on EBCDIC machines, where
 * the conversion of the output to ebcdic is left out.
 */
int Base64DecodeBinary(unsigned char *bufplain,
                   const char *bufcoded)
{
    int nbytesdecoded;
    register const unsigned char *bufin;
    register unsigned char *bufout;
    register int nprbytes;

    bufin = (const unsigned char *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (unsigned char *) bufplain;
    bufin = (const unsigned char *) bufcoded;

    while (nprbytes > 4) {
    *(bufout++) =
        (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    *(bufout++) =
        (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    *(bufout++) =
        (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    bufin += 4;
    nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
    *(bufout++) =
        (unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
    *(bufout++) =
        (unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
    *(bufout++) =
        (unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}

static const char basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64EncodeLen(int len)
{
    return ((len + 2) / 3 * 4) + 1;
}

int Base64Encode(char *encoded, const char *string, int len)
{
#ifndef CHARSET_EBCDIC
    return Base64EncodeBinary(encoded, (const unsigned char *) string, len);
#else               /* CHARSET_EBCDIC */
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
    *p++ = basis_64[(os_toascii[string[i]] >> 2) & 0x3F];
    *p++ = basis_64[((os_toascii[string[i]] & 0x3) << 4) |
                    ((int) (os_toascii[string[i + 1]] & 0xF0) >> 4)];
    *p++ = basis_64[((os_toascii[string[i + 1]] & 0xF) << 2) |
                    ((int) (os_toascii[string[i + 2]] & 0xC0) >> 6)];
    *p++ = basis_64[os_toascii[string[i + 2]] & 0x3F];
    }
    if (i < len) {
    *p++ = basis_64[(os_toascii[string[i]] >> 2) & 0x3F];
    if (i == (len - 1)) {
        *p++ = basis_64[((os_toascii[string[i]] & 0x3) << 4)];
        *p++ = '=';
    }
    else {
        *p++ = basis_64[((os_toascii[string[i]] & 0x3) << 4) |
                        ((int) (os_toascii[string[i + 1]] & 0xF0) >> 4)];
        *p++ = basis_64[((os_toascii[string[i + 1]] & 0xF) << 2)];
    }
    *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
#endif              /* CHARSET_EBCDIC */
}

/* This is the same as Base64Encode() except on EBCDIC machines, where
 * the conversion of the input to ascii is left out.
 */
int Base64EncodeBinary(char *encoded,
                             const unsigned char *string, int len)
{
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    *p++ = basis_64[((string[i] & 0x3) << 4) |
                    ((int) (string[i + 1] & 0xF0) >> 4)];
    *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
                    ((int) (string[i + 2] & 0xC0) >> 6)];
    *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
    *p++ = basis_64[(string[i] >> 2) & 0x3F];
    if (i == (len - 1)) {
        *p++ = basis_64[((string[i] & 0x3) << 4)];
        *p++ = '=';
    }
    else {
        *p++ = basis_64[((string[i] & 0x3) << 4) |
                        ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
    }
    *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}

/////////////////////////////////////////////////////////////////////////////////////
// The MD5 algorithm
// 
/////////////////////////////////////////////////////////////////////////////////////

static unsigned char PADDING[64] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
   a multiple of 4.
 */
static void Encode(unsigned char *output, const UINT4 *input, unsigned int len)
{
    unsigned int i, j;
    UINT4 k;

    for (i = 0, j = 0; j < len; i++, j += 4) {
    k = input[i];
    output[j] = (unsigned char) (k & 0xff);
    output[j + 1] = (unsigned char) ((k >> 8) & 0xff);
    output[j + 2] = (unsigned char) ((k >> 16) & 0xff);
    output[j + 3] = (unsigned char) ((k >> 24) & 0xff);
    }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
 * a multiple of 4.
 */
static void Decode(UINT4 *output, const unsigned char *input, unsigned int len)
{
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((UINT4) input[j]) | (((UINT4) input[j + 1]) << 8) |
        (((UINT4) input[j + 2]) << 16) | (((UINT4) input[j + 3]) << 24);
}


