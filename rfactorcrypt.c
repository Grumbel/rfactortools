/*
**  rFactor decryption module for Python
**  Copyright (C) 2013 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// This module is based rfactordec.c by Luigi Auriemma

#include <Python.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

u64  isi_keyz(int enctype, u64 key, u64 ret, int n);
u64  isi_key(int enctype, u64 key);
void isi_crypt(int enctype, u8 *output, u8 *input, int len, u64 key, int encrypt);
u64  isi_crypt2(int enctype, int pos, u64 key);
void std_err(const char* err);

void std_err(const char* err)
{
  if(err) {
    if(err[0]) fprintf(stderr, "\nError: %s\n", err);
  } else {
    perror("\nError");
  }

  exit(1);
}

u64 isi_keyz(int enctype, u64 key, u64 ret, int n)
{
  static const u64 keyz0[8][6] = {
    { 0xe56f8aa2ed826faaLL, 0x2f777cf9c70031d8LL, 0x12b42c082f7753a9LL, 0xe5c4f0aca218138dLL, 0xff00000000000000LL, 0x00000000000000ffLL },
    { 0x9367a56ab8f1d3ffLL, 0xe734c80097dadd31LL, 0x0ff5793be2d23f51LL, 0xa418d4ea4e115ebbLL, 0x0000ff0000000000LL, 0x0000000000ff0000LL },
    { 0x06a6fb1e97facec0LL, 0x2301031342e7afa0LL, 0x38af6d7a7a9f146bLL, 0xe281193b8565c2e0LL, 0x00000000ff000000LL, 0x000000ff00000000LL },
    { 0x69531562d7ce1b70LL, 0x1415b6af3cdfb64eLL, 0x514f29eeaaba7a93LL, 0xb1de75ad972ed241LL, 0x000000000000ff00LL, 0x00ff000000000000LL },
    { 0x0ff5793be2d23f51LL, 0xe734c80097dadd31LL, 0x9367a56ab8f1d3ffLL, 0xa418d4ea4e115ebbLL, 0x00000000000000ffLL, 0xff00000000000000LL },
    { 0x8cffa4096b4af822LL, 0xbd0e595d83943946LL, 0x3285a8ddd8d47599LL, 0x5f196cfd07285957LL, 0x0000000000ff0000LL, 0x0000ff0000000000LL },
    { 0x50c90b123d76ea44LL, 0xb4733727295f38d6LL, 0x13bb309071f26fb5LL, 0xfcdcdb8e48b1c147LL, 0x000000ff00000000LL, 0x00000000ff000000LL },
    { 0x0ff5793be2d23f51LL, 0xa418d4ea4e115ebbLL, 0x2d17e8d3ec148292LL, 0x8cffa4096b4af822LL, 0x00ff000000000000LL, 0x000000000000ff00LL },
  };
  static const u64 keyz1[8][6] = {
    { 0xe56f8aa2ed826faaLL, 0xe734c80097dadd31LL, 0x38af6d7a7a9f146bLL, 0xb1de75ad972ed241LL, 0xff00000000000000LL, 0x0000000000ff0000LL, },
    { 0x9367a56ab8f1d3ffLL, 0x2301031342e7afa0LL, 0x514f29eeaaba7a93LL, 0xa418d4ea4e115ebbLL, 0x0000ff0000000000LL, 0x000000ff00000000LL, },
    { 0x06a6fb1e97facec0LL, 0x1415b6af3cdfb64eLL, 0x9367a56ab8f1d3ffLL, 0x5f196cfd07285957LL, 0x00000000ff000000LL, 0x00ff000000000000LL, },
    { 0x69531562d7ce1b70LL, 0xe734c80097dadd31LL, 0x3285a8ddd8d47599LL, 0xfcdcdb8e48b1c147LL, 0x000000000000ff00LL, 0xff00000000000000LL, },
    { 0x0ff5793be2d23f51LL, 0xbd0e595d83943946LL, 0x13bb309071f26fb5LL, 0x8cffa4096b4af822LL, 0x00000000000000ffLL, 0x0000ff0000000000LL, },
    { 0x8cffa4096b4af822LL, 0xb4733727295f38d6LL, 0x2d17e8d3ec148292LL, 0xe5c4f0aca218138dLL, 0x0000000000ff0000LL, 0x00000000ff000000LL, },
    { 0x50c90b123d76ea44LL, 0xa418d4ea4e115ebbLL, 0x12b42c082f7753a9LL, 0xa418d4ea4e115ebbLL, 0x000000ff00000000LL, 0x000000000000ff00LL, },
    { 0x0ff5793be2d23f51LL, 0x2f777cf9c70031d8LL, 0x0ff5793be2d23f51LL, 0xe281193b8565c2e0LL, 0x00ff000000000000LL, 0x00000000000000ffLL, },
  };
  static const u64 shz_t[8][3] = {
    { 0x0c, 0x07, 0x38 },
    { 0x10, 0x11, 0x18 },
    { 0x06, 0x17, 0x08 },
    { 0x03, 0x06, 0x28 },
    { 0x26, 0x0e, 0x38 },
    { 0x12, 0x0a, 0x18 },
    { 0x0c, 0x07, 0x08 },
    { 0x16, 0x07, 0x28 }
  };
  u64     t1,
    t2,
    t3,
    *shz,
    *keyz = NULL;

  shz = (u64 *)shz_t[n];
  switch(enctype) {
    case 0: keyz = (u64 *)keyz0[n]; break;
    case 1: keyz = (u64 *)keyz1[n]; break;
    default: {
      fprintf(stderr, "\nError: unsupported enctype %d in isi_keyz\n", enctype);
      std_err("");
    }
  }

  t1 = ((ret & keyz[0]) ^ key) << shz[0];
  t2 = ((key & t1) ^ (t1 % keyz[1])) & keyz[2];
  key ^= t2;
  t1 = (((t2 & keyz[3]) >> shz[1]) ^ t1) & t1;
  key ^= t2;
  switch(n) {
    case 0: t3 = (key & keyz[4]) >> shz[2]; break;
    case 1: t3 = (ret & keyz[4]) >> shz[2]; break;
    case 2: t3 = (ret & keyz[4]) << shz[2]; break;
    case 3: t3 = (ret & keyz[4]) << shz[2]; break;
    case 4: t3 = (ret & keyz[4]) << shz[2]; break;
    case 5: t3 = (ret & keyz[4]) << shz[2]; break;
    case 6: t3 = (ret & keyz[4]) >> shz[2]; break;
    case 7: t3 = (ret & keyz[4]) >> shz[2]; break;
    default: t3 = 0;                        break;
  }
  ret = (t3 ^ (key & keyz[5])) | ret;
  n++;
  if(n < 8) ret = isi_keyz(enctype, key, ret, n);

  return ret;
}

u64 isi_key(int enctype, u64 key)
{
  return isi_keyz(enctype, key, 0, 0);
}

void isi_crypt(int enctype, u8* output, u8* input, int len, u64 key, int encrypt)
{
  u64 t;
  u64 i;
  u64 n;
  u64 c;
  u64 mask;
  int x;
  int blocksz = 0;

  t    = 0;
  x    = 0;
  mask = 1;

  if(len < 0) return;

  switch(enctype) {
    case 0: blocksz = 0x40; break;
    case 1: blocksz = 0x80; break;
    default: {
      fprintf(stderr, "\nError: unsupported enctype %d in isi_crypt\n", enctype);
      std_err("");
    }
  }

  n = len % blocksz;
  for(i = 0; i < n; i++) {
    c = input[x];
    output[x] = isi_crypt2(enctype, x, key) ^ c;
    if(encrypt) c = output[x];
    t |= ((c & mask) << (i & 0x3f));
    x++;
  }

  n = 0;
  for(len /= blocksz; len; len--) {
    key ^= t;
    t    = 0;
    n++;
    mask = 1 << (n & 7);
    for(i = 0; i < blocksz; i++) {
      c = input[x];
      output[x] = isi_crypt2(enctype, x, key) ^ c;
      if(encrypt) c = output[x];
      t |= ((c & mask) << i);
      x++;
    }
  }
}

u64 isi_crypt2(int enctype, int pos, u64 key)
{
  static const u8 table0[8] = { 0x00, 0x28, 0x18, 0x08, 0x20, 0x38, 0x10, 0x30 };
  static const u8 table1[8] = { 0x38, 0x20, 0x30, 0x10, 0x28, 0x00, 0x08, 0x18 };
  u64     t = 0;

  switch(enctype) {
    case 0: t = table0[pos & 7];    break;
    case 1: t = table1[key & 7];    break;
    default: {
      fprintf(stderr, "\nError: unsupported enctype %d in isi_crypt2\n", enctype);
      std_err("");
    }
  }
  return((((u64)0x000000ff000000ffLL << t) & key) >> t);
}

// Python Bindings

PyObject*
rfactor_encrypt(PyObject* self, PyObject* args)
{
  return Py_None;
}

static PyMethodDef rfactorcrypt_methods[] = {
  { "encrypt",    rfactor_encrypt,    METH_VARARGS, "Adjust GdkPixbuf color by applying a threshold" },
  { NULL, NULL, 0, NULL },
};

static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "rfactorcrypt",
  "rfactor encryption/decryption",
  -1,
  rfactorcrypt_methods,
  NULL, // reload
  NULL, // traverse
  NULL, // clear
  NULL  // free
};

void PyInit_rfactorcrypt(void)
{
  PyModule_Create(&moduledef);
}

/* EOF */
