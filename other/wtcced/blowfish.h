/*
This code has been found on the website

  http://www.di-mgt.com.au/crypto.html

The following is a small explanation from the website author:

Here is Bruce Schneier's code in C for his Blowfish algorithm. This version is fully ANSI compliant and contains the "missing" P-box values omitted from the book.
...
This code may be freely distributed. 
Updated 29 July 2003: thanks to Mehul Motani for pointing out an error in the code for readDataLine(). 
*/

#define UWORD32 unsigned long
#define UBYTE08 unsigned char

#define MAXKEYBYTES 56          /* 448 bits */

typedef struct
{
	unsigned long S[4][256], P[18];
} blf_ctx;

unsigned long F(blf_ctx *, unsigned long x);
void Blowfish_encipher(blf_ctx *, unsigned long *xl, unsigned long *xr);
void Blowfish_decipher(blf_ctx *, unsigned long *xl, unsigned long *xr);
short InitializeBlowfish(blf_ctx *, unsigned char key[], int keybytes);
void blf_enc(blf_ctx *c, unsigned long *data, int blocks);
void blf_dec(blf_ctx *c, unsigned long *data, int blocks);
void blf_key(blf_ctx *c, unsigned char *key, int len);

