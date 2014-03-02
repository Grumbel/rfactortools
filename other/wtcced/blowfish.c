/*
Note: a couple of functions and bf_tab.h have been removed for occupying less space

This code has been found on the website

  http://www.di-mgt.com.au/crypto.html

The following is a small explanation from the website author:

Here is Bruce Schneier's code in C for his Blowfish algorithm. This version is fully ANSI compliant and contains the "missing" P-box values omitted from the book.
...
This code may be freely distributed. 
Updated 29 July 2003: thanks to Mehul Motani for pointing out an error in the code for readDataLine(). 
*/


#include <stdio.h>

#include "blowfish.h"

#define N               16
#define noErr            0
#define DATAERROR         -1
#define KEYBYTES         8

unsigned long F(blf_ctx *bc, unsigned long x)
{
   unsigned long a;
   unsigned long b;
   unsigned long c;
   unsigned long d;
   unsigned long y;

   d = x & 0x00FF;
   x >>= 8;
   c = x & 0x00FF;
   x >>= 8;
   b = x & 0x00FF;
   x >>= 8;
   a = x & 0x00FF;
   y = bc->S[0][a] + bc->S[1][b];
   y = y ^ bc->S[2][c];
   y = y + bc->S[3][d];

   return y;
}

void Blowfish_encipher(blf_ctx *bc, unsigned long *xl, unsigned long *xr)
{
   unsigned long  Xl;
   unsigned long  Xr;
   unsigned long  temp;
   short          i;

   Xl = *xl;
   Xr = *xr;

   for (i = 0; i < N; ++i)
   {
      Xl = Xl ^ bc->P[i];
      Xr = F(bc, Xl) ^ Xr;

      temp = Xl;
      Xl = Xr;
      Xr = temp;
   }

   temp = Xl;
   Xl = Xr;
   Xr = temp;

   Xr = Xr ^ bc->P[N];
   Xl = Xl ^ bc->P[N + 1];

   *xl = Xl;
   *xr = Xr;
}

void Blowfish_decipher(blf_ctx *bc, unsigned long *xl, unsigned long *xr)
{
   unsigned long  Xl;
   unsigned long  Xr;
   unsigned long  temp;
   short          i;

   Xl = *xl;
   Xr = *xr;

   for (i = N + 1; i > 1; --i)
   {
      Xl = Xl ^ bc->P[i];
      Xr = F(bc, Xl) ^ Xr;

      /* Exchange Xl and Xr */
      temp = Xl;
      Xl = Xr;
      Xr = temp;
   }

   /* Exchange Xl and Xr */
   temp = Xl;
   Xl = Xr;
   Xr = temp;

   Xr = Xr ^ bc->P[1];
   Xl = Xl ^ bc->P[0];

   *xl = Xl;
   *xr = Xr;
}

void blf_enc(blf_ctx *c, unsigned long *data, int blocks)
{
	unsigned long *d;
	int i;

	d = data;
	for (i = 0; i < blocks; i++)
	{
		Blowfish_encipher(c, d, d+1);
		d += 2;
	}
}

void blf_dec(blf_ctx *c, unsigned long *data, int blocks)
{
	unsigned long *d;
	int i;

	d = data;
	for (i = 0; i < blocks; i++)
	{
		Blowfish_decipher(c, d, d+1);
		d += 2;
	}
}


