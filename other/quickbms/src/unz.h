/*
    Copyright 2009-2013 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl-2.0.txt
*/

// all the compression algorithms

void *calldll_alloc(u8 *dump, u32 dumpsz, u32 argc, ...);

#define MYALLOC_ZEROES  16  // long story, anyway keep 16 for both XmemDecompress and general padding/blocks (indeed AES uses 16 bytes)
#define ASURACMP    // comment it if the input doesn't have the first 8 bytes assigned to zsize and size
#include "compression/asura_huffboh.c"
#include "compression/lzss.h"
	#define QLZ_COMPRESSION_LEVEL 3 // lame solution to avoid to use "unsigned int" which is not supported in QuickBMS
	#define QLZ_STREAMING_BUFFER 0
#include "compression/quicklz.h"
#include "compression/unq3huff.c"
#include "compression/unrlew.h"
#include "compression/unmeng.h"
#include "compression/unlz2k.h"
#include "compression/undarksector.h"
#include "compression/un49g.h"
#include "compression/unthandor.c"
#include "compression/tzar_lzss.h"
#include "compression/lzh.h"
#include "compression/sr3c.h"
#include "compression/undk2.h"
#include "compression/stalker_lza.h"
#include "compression/puyo.c"
#include "libs/lzhl/lzhl.h"
#include "compression/rdc.h"
#include "compression/ilzr.h"
#include "compression/libLZR.h"
#include "compression/mppc.c"
#include "compression/un434a.h"
#include "compression/fal_codec.h"
#include "compression/doomhuff.h"
#include "compression/msf.h"
#include "compression/unstargun.c"
#include "compression/ntcompress.h"
#include "compression/nintendo.h"
#include "compression/undact.h"
#include "compression/lz77_0.c"
#include "compression/lzbss.c"
#include "compression/bgbpaq0.c"
#include "compression/dict.c"
#include "compression/rep.c"
#include "compression/elias.h"
#include "compression/kzip_old.h"
#include "libs/uberflate/uberflate.c"
#include "compression/enet_compress.c"
#include "compression/lzfu.c"
#include "compression/he3.c"
#include "compression/ntfs_compress.c"
#include "compression/comprlib.c"
#include "compression/hd2.h"
#include "compression/prs.cpp"
#include "compression/sega_lz77.c"
#include "compression/unyakuza.h"
#include "libs/lz4/lz4.h"
#include "libs/lz4/lz4hc.h"
#include "compression/unlunar.h"
#include "compression/goldensun.h"
#include "compression/luminousarc.h"
#include "compression/fastlz.h"
#include "compression/zax.h"
#include "compression/shrinker.h"
#include "libs/mmini/mmini.h"
#include "libs/clzw/lzw.h"
#include "compression/lzham.h"
#include "compression/sega_lzs2.c"
#include "libs/lzlib/lzlib.h"
#include "compression/undflt.c"
#include "compression/ffce.c"
#include "libs/snappy/snappy-c.h"
#include "compression/scummvm.h"
#include "compression/compression_unknown.c"
#include "compression/blackdesert_unpack.c"
int pucrunch_UnPack(int loadAddr, const unsigned char *data, unsigned char *file, int flags);
static int clzw_outsz = 0;
void lzw_writebuf(void *stream, char *buf, unsigned size) {
    memcpy(stream + clzw_outsz, buf, size);
    clzw_outsz += size;
}
int unlpaq8(unsigned char *in, int insz, unsigned char *out, int size, int mem, int meth);
int rLZV1 (unsigned char *in, unsigned char *out, int ilen, int len);
int lzmat_decode(u8 *pbOut, u32 *pcbOut, u8 *pbIn, u32 cbIn);
// 7z_advancecomp is used only for zlib/deflate compression on Windows and it doesn't affect the performance (memory/cpu when launched)
#ifdef WIN32
int advancecomp_rfc1950(unsigned char *in, int insz, unsigned char *out, int outsz);
int advancecomp_deflate(unsigned char *in, int insz, unsigned char *out, int outsz);
int advancecomp_lzma(unsigned char *in, int insz, unsigned char *out, int outsz, int lzma_flags);
#else
    #define zlib_compress       advancecomp_rfc1950
    #define deflate_compress    advancecomp_deflate
    //#define lzma_compress       advancecomp_lzma
#endif
int KENS_Nemesis(unsigned char *in, int insz, unsigned char *out, int outsz);
int KENS_Kosinski(unsigned char *in, int insz, unsigned char *out, int outsz, int Moduled);
int KENS_Enigma(unsigned char *in, int insz, unsigned char *out, int outsz);
int KENS_Saxman(unsigned char *in, int insz, unsigned char *out, int outsz);
int _rnc_unpack(const unsigned char* input, unsigned long input_size, unsigned char* output, unsigned long have_ret_len);
void *rnc_pack (void *data, long datalen, long *packlen);
long rnc_unpack (void *packed, void *unpacked, long *leeway, long packed_len, long unpacked_len);
int PAK_explode(unsigned char * srcBuffer, unsigned char * dstBuffer, unsigned compressedSize, unsigned uncompressedSize, unsigned short flags);
int gz_unpack(unsigned char *in, int insz, unsigned char *out, int outsz);
int dmc2_uncompress(unsigned char *in, int insz, unsigned char *out, int outsz);
int ahuff_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int arith_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int arith1_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int arith1e_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int arithn_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int compand_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int huff_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int tdcb_lzss_init(int x1, int x2, int x3, int x4);
int lzss_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int lzw12_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int lzw15v_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
int silence_ExpandMemory(unsigned char *in, int insz, unsigned char *out, int outsz);
void lzrw3a_decompress(unsigned char*,unsigned char*,int,unsigned char *,int *);
int unsqueeze(unsigned char *in, int insz, unsigned char *out, int outsz);
int d3101(unsigned char *in, int insz, unsigned char *out, int outsz);
int yuke_bpe(unsigned char *in, int insz, unsigned char *out, int outsz, int fill_outsz);
int huffman_decode_memory(const unsigned char *bufin, int bufinlen, unsigned char **bufout, int *pbufoutlen);
int huffman_encode_memory(const unsigned char *bufin, int bufinlen, unsigned char **pbufout, int *pbufoutlen);
void Huffman_Uncompress( unsigned char *in, unsigned char *out, unsigned insize, unsigned outsize );
int Huffman_Compress( unsigned char *in, unsigned char *out, unsigned insize );
void LZ_Uncompress( unsigned char *in, unsigned char *out, unsigned insize );
int LZ_Compress( unsigned char *in, unsigned char *out, unsigned insize );
void Rice_Uncompress( void *in, void *out, unsigned insize, unsigned outsize, int format );
int Rice_Compress( void *in, void *out, unsigned insize, int format );
unsigned RLE_Uncompress( unsigned char *in, unsigned insize, unsigned char *out, unsigned outsize );
unsigned RLE_Compress( unsigned char *in, unsigned insize, unsigned char *out, unsigned outsize );
void SF_Uncompress( unsigned char *in, unsigned char *out, unsigned insize, unsigned outsize );
int SF_Compress( unsigned char *in, unsigned char *out, unsigned insize );
int Scz_Decompress_Buffer2Buffer( char *inbuffer, int N, char **outbuffer, int *M );
int szip_allow_encoding = 0;
int SZ_BufftoBuffDecompress(void *dest, size_t *destLen, const void *source, size_t sourceLen, void *param);
int SZ_BufftoBuffCompress(void *dest, size_t *destLen, const void *source, size_t sourceLen, void *param);
int unbpe2(unsigned char *in, int insz, unsigned char *out, int outsz);
int strexpand(char *dest, unsigned char *source, int maxlen, unsigned char **pairtable);
int hstest_hs_unpack(unsigned char *out, unsigned char *in, int insz);
int hstest_unpackc(unsigned char *out, unsigned char *in, int insz);
int unsixpack(unsigned char *in, int insz, unsigned char *out, int outsz);
int unashford(unsigned char *in, int insz, unsigned char *out, int outsz);
__stdcall int JCALG1_Decompress_Small(void *Source, void *Destination);
__stdcall void * JCALG1_AllocFunc(unsigned nMemSize) { return(malloc(nMemSize)); }
__stdcall int JCALG1_DeallocFunc(void *pBuffer) { free(pBuffer); return(1); }
__stdcall int JCALG1_CallbackFunc(unsigned pSourcePos, unsigned pDestinationPos) { return(1); }
__stdcall unsigned JCALG1_Compress(void *Source, unsigned Length, void *Destination, unsigned WindowSize, void *pAlloc, void *pDealloc, void *pCallback, int bDisableChecksum);
int unjam(unsigned char *in, int insz, unsigned char *out, int outsz);
int unsrank(unsigned char *in, int insz, unsigned char *out, int outsz);
int ZzUncompressBlock(unsigned char *buffer);
int sh_DecodeBlock(unsigned char *iBlock, unsigned char *oBlock, int bSize);
unsigned blz_depack_safe(const void *source, unsigned srclen, void *destination, unsigned depacked_length);
unsigned blz_depack(const void *source, void *destination, unsigned depacked_length);
unsigned blz_workmem_size(unsigned length);
unsigned blz_max_packed_size(unsigned length);
unsigned blz_pack(const void *source, void *destination, unsigned length, void *workmem);
int unpaq6(unsigned char *in, int insz, unsigned char *out, int outsz, int levelx);
int unppmdi(unsigned char *in, int insz, unsigned char *out, int outsz);
int unppmdi_raw(unsigned char *in, int insz, unsigned char *out, int outsz, int SaSize, int MaxOrder, int MRMethod);
int unppmdg(unsigned char *in, int insz, unsigned char *out, int outsz);
int unppmdg_raw(unsigned char *in, int insz, unsigned char *out, int outsz, int SaSize, int MaxOrder);
int unppmdj(unsigned char *in, int insz, unsigned char *out, int outsz);
int unppmdj_raw(unsigned char *in, int insz, unsigned char *out, int outsz, int SaSize, int MaxOrder, int CutOff);
int unppmdh(unsigned char *in, int insz, unsigned char *out, int outsz);
int unppmdh_raw(unsigned char *in, int insz, unsigned char *out, int outsz, int SaSize, int MaxOrder);
int unshrink(unsigned char *in, int insz, unsigned char *out, int outsz);
int unquad(unsigned char *src, int srcsz, unsigned char *dst, int dstsz);
int unbalz(unsigned char *src, int srcsz, unsigned char *dst, int dstsz);
unsigned GRZip_DecompressBlock(unsigned char * Input, unsigned Size, unsigned char * Output);
int de_lzah(unsigned char *in, int insz, unsigned char *out, int obytes);
int de_lzh(unsigned char *in, int ibytes, unsigned char *out, int obytes, int bits);
int lzf_decompress(const void *const in_data, int in_len, void *out_data, int out_len);
int lzf_compress(const void *const in_data, int in_len, void *out_data, int out_len);
unsigned aP_depack_safe(const void *source, unsigned srclen, void *destination, unsigned dstlen);
int bpe_expand(unsigned char *in, int insz, unsigned char *out, int outsz);
int unlzh(unsigned char *in, int insz, unsigned char *out, int outsz);
int unlzari(unsigned char *in, int insz, unsigned char *out, int outsz);
int uncompress_lzw(unsigned char *in, int insz, unsigned char *out, int outsz, int init_byte);
int undmc(unsigned char *in, int insz, unsigned char *out, int outsz);
int unlzx(unsigned char *in, int insz, unsigned char *out, int outsz);
int unmspack(unsigned char *in, int insz, unsigned char *out, int outsz, int window_bits, int interval, int algo);
uint32_t unlzw(uint8_t *outbuff, uint32_t maxsize, uint8_t *in, uint32_t insize);
uint32_t unlzwx(uint8_t *outbuff, uint32_t maxsize, uint8_t *in, uint32_t insize);
u32 __stdcall nitroDecompress(u8 *srcp, u32 size, u8 *dstp, signed char depth);
int unctw(unsigned char *in, int insz, unsigned char *out, int outsz);
int lzpx_unpack(unsigned char *in, unsigned char *out);
long lzwDecompress(unsigned char* compressedMem, unsigned char* decompressedMem, long compressedSize);
int iris_decompress(unsigned char *in, int insz, unsigned char *out, int outsz);
int iris_huffman(char *in, int insz, char *out, int outsz);
int iris_uo_huffman(char *in, int insz, char *out, int outsz);
int LZXdecompress(unsigned char *inbuf, unsigned char *outbuf, unsigned inlen, unsigned outlen);
int MRCIDecompressWrapper(const void *pb, int cb, const void *pOut, int cbOut); // return is S_OK
int unzpaq(unsigned char *in, int insz, unsigned char *out, int outsz);



int get_cpu_number(void) {
    #ifdef WIN32
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return info.dwNumberOfProcessors;
    #else
        #ifdef _SC_NPROCESSORS_ONLN
        return sysconf(_SC_NPROCESSORS_ONLN);
        #endif
    #endif
    return(-1);
}



// in recompression mode I consider MAXZIPLEN as universal for any algorithm, indeed I don't use /1000

void alloc_err(const char *fname, i32 line, const char *func);
int MAXZIPLEN(int n) {
    int     ret;
    if(n < 0) ALLOC_ERR;
    ret = ((n)+(((n)/10)+1)+12+512);
    if(ret < n) ALLOC_ERR;
    return(ret);
}

//#define MAXZIPLEN(n) ((n)+(((n)/10)+1)+12+512)  // 10 instead of 1000 and + 512
//#define MAXZIPLEN(n) ((n)+(((n)/1000)+1)+12)    // this is the correct one for zlib/deflate

#define QUICK_IN_OUT \
    unsigned char   *inl    = in + insz, \
                    *o      = out, \
                    *outl   = out + outsz;

#define lame_feof(X)    ((infile  >= infilel)  ? EOF : 0)
#define lame_getc(X)    ((infile  >= infilel)  ? EOF : (*infile++))
#define lame_putc(Y,X)  ((outfile >= outfilel) ? EOF : (*outfile++ = Y))
#define lame_fgetc      lame_getc
#define lame_fputc      lame_putc

// if I'm not in error, this is good if *ret_outsz was 0 and ret_out wasn't allocated
#define not_ret_out_boh(X) \
    if(!*ret_out) { \
        *X = 0; \
        *ret_out = malloc(*X); \
        if(!*ret_out) STD_ERR(QUICKBMS_ERROR_MEMORY); \
    }



int uncopy(unsigned char *in, int insz, unsigned char *out, int outsz) {
    if(outsz > insz) outsz = insz;
    memcpy(out, in, outsz);
    return(outsz);
}



int unlzo(u8 *in, int insz, u8 *out, int outsz, int type) {
#ifdef DISABLE_LZO
    fprintf(stderr, "\nError: LZO support has been disabled in this build\n");
    return(-1);
#else
    lzo_uint    len;
    int         err = LZO_E_OK;

    len = outsz;
    switch(type) {
        case COMP_LZO1:  { err = lzo1_decompress(in, insz, out, &len, NULL); break; }
        case COMP_LZO1A: { err = lzo1a_decompress(in, insz, out, &len, NULL); break; }
        case COMP_LZO1B: { err = lzo1b_decompress_safe(in, insz, out, &len, NULL); break; }
        case COMP_LZO1C: { err = lzo1c_decompress_safe(in, insz, out, &len, NULL); break; }
        case COMP_LZO1F: { err = lzo1f_decompress_safe(in, insz, out, &len, NULL); break; }
        case COMP_LZO1X: {
            if(comtype_dictionary) {
                err = lzo1x_decompress_dict_safe(in, insz, out, &len, NULL, comtype_dictionary, comtype_dictionary_len);
            } else {
                err = lzo1x_decompress_safe(in, insz, out, &len, NULL);
            }
            break;
        }
        case COMP_LZO1Y: {
            if(comtype_dictionary) {
                err = lzo1y_decompress_dict_safe(in, insz, out, &len, NULL, comtype_dictionary, comtype_dictionary_len);
            } else {
                err = lzo1y_decompress_safe(in, insz, out, &len, NULL);
            }
            break;
        }
        case COMP_LZO1Z: {
            if(comtype_dictionary) {
                err = lzo1z_decompress_dict_safe(in, insz, out, &len, NULL, comtype_dictionary, comtype_dictionary_len);
            } else {
                err = lzo1z_decompress_safe(in, insz, out, &len, NULL);
            }
            break;
        }
        case COMP_LZO2A: { err = lzo2a_decompress_safe(in, insz, out, &len, NULL); break; }
        default: {
            fprintf(stderr, "\nError: unsupported LZO decompression %d\n", type);
            return(-1);
            break;
        }
    }
    if((err != LZO_E_OK) && (err != LZO_E_INPUT_NOT_CONSUMED)) {
        fprintf(stderr, "\nError: the compressed LZO input is wrong or incomplete (%d)\n", err);
        return(-1);
    }
    return(len);
#endif
}



int lzo_compress(u8 *in, int insz, u8 *out, int outsz, int type) {
#ifdef DISABLE_LZO
    fprintf(stderr, "\nError: LZO support has been disabled in this build\n");
    return(-1);
#else
    lzo_uint    len;
    int         err = LZO_E_OK;
    static u8   *wrkmem = NULL;

    len = outsz;
    switch(type) {
        case COMP_LZO1_COMPRESS: {
            wrkmem = realloc(wrkmem, LZO1_99_MEM_COMPRESS);
            if(!wrkmem) STD_ERR(QUICKBMS_ERROR_MEMORY);
            err = lzo1_99_compress(in, insz, out, &len, wrkmem);
            break;
        }
        case COMP_LZO1X_COMPRESS: {
            wrkmem = realloc(wrkmem, LZO1X_999_MEM_COMPRESS);
            if(!wrkmem) STD_ERR(QUICKBMS_ERROR_MEMORY);
            if(comtype_dictionary) {
                err = lzo1x_999_compress_dict(in, insz, out, &len, wrkmem, comtype_dictionary, comtype_dictionary_len);
            } else {
                err = lzo1x_999_compress(in, insz, out, &len, wrkmem);
            }
            break;
        }
        case COMP_LZO2A_COMPRESS: {
            wrkmem = realloc(wrkmem, LZO2A_999_MEM_COMPRESS);
            if(!wrkmem) STD_ERR(QUICKBMS_ERROR_MEMORY);
            err = lzo2a_999_compress(in, insz, out, &len, wrkmem);
            break;
        }
        default: {
            fprintf(stderr, "\nError: unsupported LZO compression %d\n", type);
            return(-1);
            break;
        }
    }
    if(err != LZO_E_OK) {
        fprintf(stderr, "\nError: LZO compression (%d)\n", err);
        return(-1);
    }
    return(len);
#endif
}



int unucl(u8 *in, int insz, u8 *out, int outsz, int type) {
#ifdef DISABLE_UCL
    fprintf(stderr, "\nError: UCL support has been disabled in this build\n");
    return(-1);
#else
    ucl_uint    len;
    int         err = UCL_E_OK;

    len = outsz;
    switch(type) {
        case COMP_NRV2b: { err = ucl_nrv2b_decompress_safe_8(in, insz, out, &len, NULL); break; }
        case COMP_NRV2d: { err = ucl_nrv2d_decompress_safe_8(in, insz, out, &len, NULL); break; }
        case COMP_NRV2e: { err = ucl_nrv2e_decompress_safe_8(in, insz, out, &len, NULL); break; }
        default: {
            fprintf(stderr, "\nError: unsupported UCL decompression %d\n", type);
            return(-1);
            break;
        }
    }
    if((err != UCL_E_OK) && (err != UCL_E_INPUT_NOT_CONSUMED)) {
        fprintf(stderr, "\nError: the compressed UCL input is wrong or incomplete (%d)\n", err);
        return(-1);
    }
    return(len);
#endif
}



#define ZIP_BASE(NAME,WBITS) \
int NAME(u8 *in, int insz, u8 *out, int outsz) { \
    static z_stream *z  = NULL; \
    int     no_error = 0; \
    int     ret; \
    \
    if(!in && !out) { \
        if(z) { \
            deflateEnd(z); \
            free(z); \
        } \
        z = NULL; \
        return(-1); \
    } \
    \
    if(!z) { \
        z = malloc(sizeof(z_stream)); \
        if(!z) return(-1); \
        z->zalloc = Z_NULL; \
        z->zfree  = Z_NULL; \
        z->opaque = Z_NULL; \
        if(deflateInit2(z, 9, Z_DEFLATED, WBITS, 9, Z_DEFAULT_STRATEGY)) { \
            fprintf(stderr, "\nError: zlib initialization error\n"); \
            return(-1); \
        } \
    } \
    deflateReset(z); \
    \
    if(comtype_dictionary) { \
        deflateSetDictionary(z, comtype_dictionary, comtype_dictionary_len); \
    } \
    \
    z->next_in   = in; \
    z->avail_in  = insz; \
    z->next_out  = out; \
    z->avail_out = outsz; \
    ret = deflate(z, Z_FINISH); \
    if((ret != Z_STREAM_END) && !no_error) { \
        fprintf(stderr, "\nError: the compressed zlib/deflate input is wrong or incomplete (%d)\n", ret); \
        return(-1); \
    } \
    return(z->total_out); \
}
ZIP_BASE(zlib_compress,    15)
ZIP_BASE(deflate_compress, -15)



#define UNZIP_BASE(NAME,WBITS) \
int NAME(u8 *in, int insz, u8 *out, int outsz, int no_error) { \
    static z_stream *z  = NULL; \
    int     ret; \
    \
    if(!in && !out) { \
        if(z) { \
            inflateEnd(z); \
            free(z); \
        } \
        z = NULL; \
        return(-1); \
    } \
    \
    if(!z) { \
        z = malloc(sizeof(z_stream)); \
        if(!z) return(-1); \
        z->zalloc = Z_NULL; \
        z->zfree  = Z_NULL; \
        z->opaque = Z_NULL; \
        if(inflateInit2(z, WBITS)) { \
            fprintf(stderr, "\nError: zlib initialization error\n"); \
            return(-1); \
        } \
    } \
    inflateReset(z); \
    \
    if(comtype_dictionary) { \
        inflateSetDictionary(z, comtype_dictionary, comtype_dictionary_len); \
    } \
    \
    z->next_in   = in; \
    z->avail_in  = insz; \
    z->next_out  = out; \
    z->avail_out = outsz; \
    ret = inflate(z, Z_FINISH); \
    if((ret != Z_STREAM_END) && !no_error) { \
        fprintf(stderr, "\nError: the compressed zlib/deflate input is wrong or incomplete (%d)\n", ret); \
        return(-1); \
    } \
    return(z->total_out); \
}
UNZIP_BASE(unzip_zlib,    15)
UNZIP_BASE(unzip_deflate, -15)



int unzip_dynamic(u8 *in, int insz, u8 **ret_out, int *ret_outsz) {
    z_stream z;
    int     err,
            retsz,
            addsz,
            wbits,
            retry;

    if((in[0] == 0x78) || (in[0] == 0x68)) {    // just a simple guess to save time
        wbits = 15;     // zlib
    } else {
        wbits = -15;    // deflate
    }

    retry = 0;
redo:
    z.zalloc = NULL;
    z.zfree  = NULL;
    z.opaque = NULL;
    if(inflateInit2(&z, wbits)) {
        fprintf(stderr, "\nError: zlib initialization error\n");
        return(-1);
    }

    addsz = insz / 4;
    if(!addsz) addsz = insz;
    not_ret_out_boh(ret_outsz)

    retsz = 0;
    z.next_in  = in;
    z.avail_in = insz;
    while(z.avail_in) {
        z.next_out  = *ret_out + retsz;
        z.avail_out = *ret_outsz - retsz;
        err = inflate(&z, Z_FINISH);
        retsz = (u8 *)z.next_out - *ret_out;
        if(err == Z_STREAM_END) break;
        if(((err == Z_OK) && z.avail_in) || (err == Z_BUF_ERROR)) {
            myalloc(ret_out, *ret_outsz + addsz, ret_outsz);
        } else {
            //if(retsz <= 0) {
                //printf("\nError: invalid zlib compressed data (%d)\n", err);
                retsz = -1;
            //}
            break;
        }
    }
    inflateEnd(&z);
    if(retsz < 0) {
        if(wbits > 0) { // zlib->deflate
            wbits = -15;
        } else {        // deflate->zlib
            wbits = 15;
        }
        retry++;
        if(retry < 2) goto redo;
        myalloc(ret_out, insz, ret_outsz);
        memcpy(*ret_out, in, insz);
        retsz = insz;
    }
    return(retsz);
}



#ifdef WIN32    // it's a zlib with the adding of inflateBack9 which is not default
#include "compression/infback9.h"
typedef struct {
    u8      *p;
    u8      *l;
} zlib_func_t;
static unsigned zlib_inf(zlib_func_t *data, u8 **ret) {
    unsigned    len;

    *ret = data->p;
    len = data->l - data->p;
    data->p += len;
    return(len);
}
static int zlib_outf(zlib_func_t *data, u8 *buff, int len) {
    //int     size = data->l - data->p;
    if((data->p + len) > data->l) return(-1);
    memcpy(data->p, buff, len);
    data->p += len;
    return(0);
}
int inflate64(u8 *in, int insz, u8 *out, int outsz) {
    static unsigned char *window = NULL;    // from gun.c
    zlib_func_t myin,
                myout;
    static z_stream z;  // I don't know if inflate64 supports Reset
    int     ret;

    if(!window) {
        window = malloc(65536);
        if(!window) return(-1);
    }

    z.zalloc = Z_NULL;
    z.zfree  = Z_NULL;
    z.opaque = Z_NULL;
    if(inflateBack9Init(&z, window)) {
        fprintf(stderr, "\nError: inflate64 initialization error\n");
        return(-1);
    }

    if(comtype_dictionary) {    // supported?
        inflateSetDictionary(&z, comtype_dictionary, comtype_dictionary_len);
    }

    myin.p  = in;
    myin.l  = in + insz;
    myout.p = out;
    myout.l = out + outsz;

    z.next_in   = in;
    z.avail_in  = insz;
    z.next_out  = out;
    z.avail_out = outsz;
    ret = inflateBack9(&z,
        (void *)zlib_inf,  &myin,
        (void *)zlib_outf, &myout);
    if(ret != Z_STREAM_END) {
        inflateBack9End(&z);    // reset not supported by inflate9
        fprintf(stderr, "\nError: the compressed deflate64 input is wrong or incomplete (%d)\n", ret);
        return(-1);
    }

    outsz = myout.p - out;
    inflateBack9End(&z);
    return(outsz);
}
#else
int inflate64(u8 *in, int insz, u8 *out, int outsz) {
    fprintf(stderr, "\nError: inflate64 is not supported on this platform\n");
    myexit(QUICKBMS_ERROR_COMPRESSION);
    return -1;
}
#endif



int unbzip2(u8 *in, int insz, u8 *out, int outsz) { // no reset in bzlib
    int     err;

    err = BZ2_bzBuffToBuffDecompress(out, &outsz, in, insz, 0, 0);
    if(err != BZ_OK) {
        fprintf(stderr, "\nError: invalid bz2 compressed data (%d)\n", err);
        return(-1);
    }
    return(outsz);
}



int bzip2_compress(u8 *in, int insz, u8 *out, int outsz) {
    int     err;

    err = BZ2_bzBuffToBuffCompress(out, &outsz, in, insz, 9, 0, 0);
    if(err != BZ_OK) {
        fprintf(stderr, "\nError: invalid bz2 compressed data (%d)\n", err);
        return(-1);
    }
    return(outsz);
}



int unbzip2_file(u8 *in, int insz, u8 **ret_out, int *ret_outsz) { // no reset in bzlib
    bz_stream bz;
    int     err,
            retsz,
            addsz;

    bz.bzalloc = NULL;
    bz.bzfree  = NULL;
    bz.opaque  = NULL;
    if(BZ2_bzDecompressInit(&bz, 0, 0)
      != BZ_OK) return(-1);

    addsz = insz / 4;
    if(!addsz) addsz = insz;
    not_ret_out_boh(ret_outsz)

    retsz = 0;
    bz.next_in  = in;
    bz.avail_in = insz;
    while(bz.avail_in) {
        bz.next_out  = *ret_out + retsz;
        bz.avail_out = *ret_outsz - retsz;
        err = BZ2_bzDecompress(&bz);
        retsz = (u8 *)bz.next_out - *ret_out;
        if(err == BZ_STREAM_END) break;
        if(((err == BZ_OK) && bz.avail_in) || (err == BZ_OUTBUFF_FULL)) {
            myalloc(ret_out, *ret_outsz + addsz, ret_outsz);
        } else {
            //if(retsz <= 0) {
                fprintf(stderr, "\nError: invalid bz2 compressed data (%d)\n", err);
                retsz = -1;
            //}
            break;
        }
    }
    BZ2_bzDecompressEnd(&bz);
    return(retsz);
}



int unxmemlzx(u8 *in, int insz, u8 *out, int outsz) {
#ifdef WIN32
    typedef VOID*                       XMEMDECOMPRESSION_CONTEXT;
    typedef enum _XMEMCODEC_TYPE {
        XMEMCODEC_DEFAULT =             0,
        XMEMCODEC_LZX =                 1
    } XMEMCODEC_TYPE;
    typedef struct _XMEMCODEC_PARAMETERS_LZX {
        DWORD Flags;
        DWORD WindowSize;
        DWORD CompressionPartitionSize;
    } XMEMCODEC_PARAMETERS_LZX;
    HRESULT WINAPI XMemCreateDecompressionContext(
        XMEMCODEC_TYPE                  CodecType,
        CONST VOID*                     pCodecParams,
        DWORD                           Flags,
        XMEMDECOMPRESSION_CONTEXT*      pContext
    );
    HRESULT WINAPI XMemDecompress(
        XMEMDECOMPRESSION_CONTEXT       Context,
        VOID*                           pDestination,
        SIZE_T*                         pDestSize,
        CONST VOID*                     pSource,
        SIZE_T                          SrcSize
    );

    static XMEMDECOMPRESSION_CONTEXT ctx = NULL;
    static XMEMCODEC_PARAMETERS_LZX  *param = NULL;
    SIZE_T  ret;
    HRESULT hr;

    if(!ctx) {
        if(comtype_dictionary) {
            param = malloc(sizeof(XMEMCODEC_PARAMETERS_LZX));
            if(!param) STD_ERR(QUICKBMS_ERROR_MEMORY);
            param->Flags = 0;
            param->WindowSize = 128 * 1024;
            param->CompressionPartitionSize = 512 * 1024;
            //sscanf(comtype_dictionary, "%d %d",
            get_parameter_numbers(comtype_dictionary,
                (int *)&param->WindowSize, (int *)&param->CompressionPartitionSize, NULL);
        }

        hr = XMemCreateDecompressionContext(
            XMEMCODEC_DEFAULT,
            param,
            0,
            &ctx);
        if(hr != S_OK) return(-1);
    }
    // XMemResetDecompressionContext is used only for the streams

    ret = outsz;
    hr = XMemDecompress(ctx, out, &ret, in, insz + MYALLOC_ZEROES);
    // + MYALLOC_ZEROES: ehmmmm long story, watch myalloc() and DMC4

    // XMemDestroyDecompressionContext(ctx);
    if(hr != S_OK) return(-1);
    return(ret);
#else
    fprintf(stderr, "\nError: XMemDecompress is implemented only on Windows\n");
    return(-1);
#endif
}



int xmem_compress(u8 *in, int insz, u8 *out, int outsz) {
#ifdef WIN32
    typedef VOID*                       XMEMCOMPRESSION_CONTEXT;
    typedef enum _XMEMCODEC_TYPE {
        XMEMCODEC_DEFAULT =             0,
        XMEMCODEC_LZX =                 1
    } XMEMCODEC_TYPE;
    typedef struct _XMEMCODEC_PARAMETERS_LZX {
        DWORD Flags;
        DWORD WindowSize;
        DWORD CompressionPartitionSize;
    } XMEMCODEC_PARAMETERS_LZX;
    HRESULT WINAPI XMemCreateCompressionContext(
        XMEMCODEC_TYPE                  CodecType,
        CONST VOID*                     pCodecParams,
        DWORD                           Flags,
        XMEMCOMPRESSION_CONTEXT*        pContext
    );
    HRESULT WINAPI XMemCompress(
        XMEMCOMPRESSION_CONTEXT         Context,
        VOID*                           pDestination,
        SIZE_T*                         pDestSize,
        CONST VOID*                     pSource,
        SIZE_T                          SrcSize
    );

    static XMEMCOMPRESSION_CONTEXT   ctx = NULL;
    static XMEMCODEC_PARAMETERS_LZX  *param = NULL;
    SIZE_T  ret;
    HRESULT hr;

    if(!ctx) {
        if(comtype_dictionary) {
            param = malloc(sizeof(XMEMCODEC_PARAMETERS_LZX));
            if(!param) STD_ERR(QUICKBMS_ERROR_MEMORY);
            param->Flags = 0;
            param->WindowSize = 128 * 1024;
            param->CompressionPartitionSize = 512 * 1024;
            //sscanf(comtype_dictionary, "%d %d",
            get_parameter_numbers(comtype_dictionary,
                (int *)&param->WindowSize, (int *)&param->CompressionPartitionSize, NULL);
        }

        hr = XMemCreateCompressionContext(
            XMEMCODEC_DEFAULT,
            param,
            0,
            &ctx);
        if(hr != S_OK) return(-1);
    }
    // XMemResetCompressionContext is used only for the streams

    ret = outsz;
    hr = XMemCompress(ctx, out, &ret, in, insz); // no MYALLOC_ZEROES!

    // XMemDestroyCompressionContext(ctx);
    if(hr != S_OK) return(-1);
    return(ret);
#else
    fprintf(stderr, "\nError: XMemCompress is implemented only on Windows\n");
    return(-1);
#endif
}



int hex2byte(u8 *hex) {
    static const signed char hextable[256] =
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\xff\xff\xff\xff\xff\xff"
        "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";

    if((hextable[hex[0]] < 0) || (hextable[hex[1]] < 0)) return(-1);
    return((hextable[hex[0]] << 4) | hextable[hex[1]]);
}



// automatically does: hexadecimal, quoted printable, percentage encoding
int unhex(u8 *in, int insz, u8 *out, int outsz) {
    QUICK_IN_OUT
    int     c;

    while(in < inl) {
        c = hex2byte(in);
        if(c < 0) {
            in++;
        } else {
            if(o >= outl) return(-1);
            *o++ = c;
            in += 2;
        }
    }
    return(o - out);
}



int unbase64(u8 *in, int insz, u8 *out, int outsz) {
    int     xlen,
            a   = 0,
            b   = 0,
            c,
            step;
    u8      *limit,
            *data,
            *p;
    static const u8 base[128] = {   // supports also the Gamespy base64 and URLs
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x3e,0x00,0x3f,
        0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
        0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x3e,0x00,0x3f,0x00,0x3f,
        0x00,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
        0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x00,0x00,0x00,0x00,0x00
    };

    if(insz < 0) insz = strlen(in);
    xlen = ((insz >> 2) * 3) + 1;    // NULL included in output for text
    if((in != out) && (outsz >= 0)) {
        if(outsz < xlen) return(-1);
    } else {
        outsz = xlen;
    }
    data = in;

    p = out;
    limit = data + insz;

    for(step = 0; /* data < limit */; step++) {
        do {
            if(data >= limit) {
                c = 0;
                break;
            }
            c = *data;
            data++;
            if((c == '=') || (c == '_')) {  // supports also the Gamespy base64
                c = 0;
                break;
            }
        } while(c && ((c <= ' ') || (c > 0x7f)));
        if(!c) break;

        switch(step & 3) {
            case 0: {
                a = base[c];
                break;
            }
            case 1: {
                b = base[c];
                *p++ = (a << 2)        | (b >> 4);
                break;
            }
            case 2: {
                a = base[c];
                *p++ = ((b & 15) << 4) | (a >> 2);
                break;
            }
            case 3: {
                *p++ = ((a & 3) << 6)  | base[c];
                break;
            }
        }
    }
    *p = 0;
    return(p - out);
}



int unexplode(u8 *in, int insz, u8 *out, int outsz) {
typedef struct {
    u8      *in;
    int     insz;
    int     outsz;
} explode_info_t;

unsigned explode_read(void *how, unsigned char **buf) {
    explode_info_t *explode_info;

    explode_info = (explode_info_t *)how;
    *buf = explode_info->in;
    return(explode_info->insz);
}
int explode_write(void *how, unsigned char *buf, unsigned len) {
    explode_info_t *explode_info;

    explode_info = (explode_info_t *)how;
    explode_info->outsz = len;
    return(0);
}

    explode_info_t explode_info;

    explode_info.in    = in;
    explode_info.insz  = insz;
    explode_info.outsz = -1;

    blast(explode_read, &explode_info, explode_write, &explode_info, out, outsz);
    return(explode_info.outsz);
}



int lzma_compress(u8 *in, int insz, u8 *out, int outsz, int flags) {
void *SzAlloc(void *p, size_t size) { return(real_malloc(size)); }  // xmalloc doesn't return in case of error
void SzFree(void *p, void *address) { real_free(address); }
ISzAlloc g_Alloc = { SzAlloc, SzFree };

	CLzmaEncHandle  lzma;
    CLzmaEncProps   props;
    SizeT   outlen;
    int     err,
            filter  = 0,
            propsz  = 5,
            dictsz  = 27;   // it means: allocate (1 << (dictsz + 2)) bytes
    u8      *o;

    lzma = LzmaEnc_Create(&g_Alloc);
	if(!lzma) return(-1);

redo:
    o = out;
    LzmaEncProps_Init(&props);
    LzmaEncProps_Normalize(&props);

        props.level = 9;            /*  0 <= level <= 9 */
        props.dictSize = 1<<dictsz; /* (1 << 12) <= dictSize <= (1 << 27) for 32-bit version
                                       (1 << 12) <= dictSize <= (1 << 30) for 64-bit version
                                       default = (1 << 24) */
        // xz wants lc+lp <= 4
        //props.lc = 1;               /* 0 <= lc <= 8, default = 3 */
        //props.lp = 3;               /* 0 <= lp <= 4, default = 0 */
        props.lc = 8;               /* 0 <= lc <= 8, default = 3 */
        props.lp = 4;               /* 0 <= lp <= 4, default = 0 */

        props.pb = 0; /* yeah 0!*/  /* 0 <= pb <= 4, default = 2 */
        //props.algo = 1;             /* 0 - fast, 1 - normal, default = 1 */
        props.fb = 273;             /* 5 <= fb <= 273, default = 32 */
        //props.btMode = 1;           /* 0 - hashChain Mode, 1 - binTree mode - normal, default = 1 */
        props.numHashBytes = 4;     /* 2, 3 or 4, default = 4 */
        //props.mc = (1 << 30);       /* 1 <= mc <= (1 << 30), default = 32 */

    props.writeEndMark = 1;
    props.numThreads = get_cpu_number();
    // if(props.numThreads <= 0) LZMA will fix it automatically

    LzmaEnc_SetProps(lzma, &props);

    outlen = outsz;
    LzmaEnc_WriteProperties(lzma, o, &outlen);
    propsz = outlen;
    o     += propsz;
    outsz -= propsz;

        /* flags */

    if(flags & LZMA_FLAGS_EFS) {
        if(outsz < 4) return(-2);
        o[0] = 0;
        o[1] = 0 >> 8;
        o[2] = propsz;
        o[3] = propsz >> 8;
        o     += 4;
        outsz -= 4;
    }
    if(flags & LZMA_FLAGS_86_DECODER) {
        if(outsz < 1) return(-3);
        o[0] = filter;
        o++;
        outsz--;
    }
    if(flags & LZMA_FLAGS_86_HEADER) {
        if(outsz < 8) return(-4);
        o[0] = insz;  // 64bit
        o[1] = insz >> 8;
        o[2] = insz >> 16;
        o[3] = insz >> 24;
        o[4] = 0;
        o[5] = 0;
        o[6] = 0;
        o[7] = 0;
        o     += 8;
        outsz -= 8;
    }

        /* compression */

    outlen = outsz;
    err = LzmaEnc_MemEncode(lzma, o, &outlen, in, insz, props.writeEndMark, NULL, &g_Alloc, &g_Alloc);
    if((err == SZ_ERROR_PARAM) || (err == SZ_ERROR_MEM)) {
        dictsz--;
        if(dictsz >= 12) goto redo;
    }
    if(err != SZ_OK) {
        LzmaEnc_Destroy(lzma, &g_Alloc, &g_Alloc);
        fprintf(stderr, "\nError: lzma error %d\n", err);
        return -5;
    }
    LzmaEnc_Destroy(lzma, &g_Alloc, &g_Alloc);
    return((o - out) + outlen);
}



void show_lzma_error(int status) {
    fprintf(stderr, "\nError: the compressed LZMA input is wrong or incomplete (%d)\n", status);
    switch(status) {
        case LZMA_STATUS_NOT_FINISHED:                  fprintf(stderr, "       stream was not finished\n"); break;
        case LZMA_STATUS_NEEDS_MORE_INPUT:              fprintf(stderr, "       you must provide more input bytes\n"); break;
        case LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK:   fprintf(stderr, "       there is probability that stream was finished without end mark\n"); break;
        default: break;
    }
}



// LZMA_FLAGS_EFS: 2 bytes version, 2 bytes props size, props and data
#define LZMA2_PROPS_SIZE    1

#define UNLZMA_BASE(LZMA_VER) \
int unlzma##LZMA_VER(u8 *in, int insz, u8 **ret_out, int outsz, int lzma_flags, int *ret_outsz, int auto_size) { \
void *SzAlloc(void *p, size_t size) { return(real_malloc(size)); } \
void SzFree(void *p, void *address) { real_free(address); } \
ISzAlloc g_Alloc = { SzAlloc, SzFree }; \
    \
    CLzma##LZMA_VER##Dec    lzma; \
    ELzmaStatus status; \
    SizeT   inlen, \
            outlen; \
    int     x86State, \
            filter  = 0, \
            propsz  = LZMA##LZMA_VER##_PROPS_SIZE; \
    u8      *out, \
            *prop; \
    \
    if(lzma_flags & LZMA_FLAGS_EFS) { \
        if(insz < 4) return(-1); \
        propsz = in[2] | (in[3] << 8); \
        in += 4; \
        insz -= 4; \
    } \
    \
    if(lzma_flags & LZMA_FLAGS_86_DECODER) { \
        if(insz < 1) return(-2); \
        filter = in[0]; \
        in++; \
        insz--; \
    } \
    \
    if(insz < propsz) return(-3); \
    prop  = in; \
    in   += propsz; \
    inlen = insz - propsz; \
    \
    not_ret_out_boh(ret_outsz) \
    out = *ret_out; \
    if(lzma_flags & LZMA_FLAGS_86_HEADER) { \
        if(insz < 8) return(-4); \
        outsz = QUICK_GETi32(in, 0); \
        if(outsz < 0) return(-5); \
        myalloc(&out, outsz, ret_outsz); \
        *ret_out = out; \
        in   += 8; \
        inlen = insz - 8; \
    } \
    outlen = outsz; \
    \
    Lzma##LZMA_VER##Dec_Construct(&lzma); \
    if((void *)Lzma##LZMA_VER##Dec_Allocate == (void *)Lzma2Dec_Allocate) { \
        if(Lzma2Dec_Allocate((void *)&lzma, prop[0], &g_Alloc) != SZ_OK) { \
            if(Lzma2Dec_AllocateProbs((void *)&lzma, prop[0], &g_Alloc) != SZ_OK) { \
                return(-6); \
            } \
        } \
    } else { \
        if(LzmaDec_Allocate((void *)&lzma, prop, propsz, &g_Alloc) != SZ_OK) { \
            if(LzmaDec_AllocateProbs((void *)&lzma, prop, propsz, &g_Alloc) != SZ_OK) { \
                return(-7); \
            } \
        } \
    } \
    Lzma##LZMA_VER##Dec_Init(&lzma); \
    \
    int     r; \
    if(auto_size) { \
        int     _ip, \
                _ir, \
                _or, \
                outsz_inc; \
        \
        outsz_inc = outsz / 100; \
        if(!outsz_inc) outsz_inc++; \
        if(outsz_inc < (1024 * 1024)) outsz_inc = (1024 * 1024); \
        \
        outlen = 0; \
        for(_ip = 0; _ip < inlen; _ip += _ir) { \
            _or = outsz - outlen; \
            _ir = inlen - _ip; \
            r = Lzma##LZMA_VER##Dec_DecodeToBuf(&lzma, out + outlen, &_or, in + _ip, &_ir, LZMA_FINISH_ANY, &status); \
            if(r != SZ_OK) { \
                show_lzma_error(status); \
                outlen = -1; \
                goto quit; \
            } \
            outlen += _or; \
            if((status == LZMA_STATUS_FINISHED_WITH_MARK) || (status == LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK)) break; \
            \
            outsz += outsz_inc; \
            myalloc(&out, outsz, ret_outsz); \
            *ret_out = out; \
        } \
    } else { \
        r = Lzma##LZMA_VER##Dec_DecodeToBuf(&lzma, out, &outlen, in, &inlen, LZMA_FINISH_END, &status); \
        if( \
            (r != SZ_OK) \
         || !((status == LZMA_STATUS_FINISHED_WITH_MARK) || (status == LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK))) { \
            show_lzma_error(status); \
            outlen = - (100 + status); \
            goto quit; \
        } \
    } \
    if(filter) { \
        x86_Convert_Init(x86State); \
        x86_Convert(out, outlen, 0, &x86State, 0); \
    } \
quit: \
    Lzma##LZMA_VER##Dec_Free(&lzma, &g_Alloc); \
    return(outlen); \
}

UNLZMA_BASE()
UNLZMA_BASE(2)



int ungzip(u8 *in, int insz, u8 **ret_out, int *ret_outsz) {
    int     fsize = 0,
            guess_minsize;
    u8      flags,
            cm,
            *inl,
            *out;

    if(insz < 14) return(-1);
    if(in[0] != 0x1f) return(-1);
    not_ret_out_boh(ret_outsz)
    if(in[1] == 0x8b) {         // gzip
    } else if(in[1] == 0x9e) {  // old gzip
    } else if(in[1] == 0x1e) {  // pack
        return(gz_unpack(in + 2, insz - 2, *ret_out, *ret_outsz));
    } else if(in[1] == 0x9d) {  // lzw (experimental with known size only)
        return(uncompress_lzw(in + 3, insz - 3, *ret_out, *ret_outsz, in[2]));
    } else if(in[1] == 0xa0) {  // lzh (experimental with known size only)
        return(unlzh(in + 2, insz - 2, *ret_out, *ret_outsz));
    } else return(-1);
    inl = in + insz;

    guess_minsize = (insz - 12);    // blah
    if(guess_minsize) guess_minsize -= (guess_minsize / 1000);  // blah
    if(guess_minsize < 0) guess_minsize = 0;
    for(inl -= 4; inl > in; inl--) {  // lame, simple and working
        //fsize = getxx(inl, 4);
        fsize = QUICK_GETi32(inl, 0); // little endian
        if(fsize < guess_minsize) continue;
        if(fsize > 0) break;
    }

    in += 2;        // id1/id2
    cm = *in++;     // cm
    flags = *in++;  // flg
    in += 4;        // mtime
    in++;           // xfl
    in++;           // os
    if(flags & 4) {
        in += 2 + (in[0] | (in[1] << 8));
        if(in >= inl) return(-1);
    }
    if(flags & 8)  in += strlen(in) + 1;    // name (adding support for names is chaotic and insecure)
    if(flags & 16) in += strlen(in) + 1;    // comment
    if(flags & 2)  in += 2;                 // crc
    if(in >= inl) return(-1);

    out = *ret_out;
    myalloc(&out, fsize, ret_outsz);
    *ret_out = out;

    switch(cm) {    // based on the ZIP format, totally unrelated to the gzip format
        case 0:  fsize = uncopy(in, inl - in, out, fsize);              break;
        case 8:  fsize = unzip_deflate(in, inl - in, out, fsize, 0);    break;
        case 1:  fsize = unshrink(in, inl - in, out, fsize);            break;
        case 6:  fsize = unexplode(in, inl - in, out, fsize);           break;
        case 9:  fsize = inflate64(in, inl - in, out, fsize);           break;
        case 12: fsize = unbzip2(in, inl - in, out, fsize);             break;
        case 14: fsize = unlzma(in, inl - in, &out, fsize, LZMA_FLAGS_EFS, &fsize, 0); break;
        case 21: fsize = unxmemlzx(in, inl - in, out, fsize);           break;
        case 64: fsize = undarksector(in, inl - in, out, fsize, 1);     break;
        case 98: fsize = unppmdi(in, inl - in, out, fsize);             break;
        default: fsize = unzip_deflate(in, inl - in, out, fsize, 0);    break;
    }
    return(fsize);
}



int gzip_compress(u8 *in, int insz, u8 *out, int outsz) {
    int     len,
            crc;
    u8      *o;

    if(outsz < 18) return(-1);
    o = out;
    *o++ = 0x1f;    // ID1
    *o++ = 0x8b;    // ID2
    *o++ = 0x08;    // CM
    *o++ = 0x00;    // FLG
    *o++ = 0;  *o++ = 0;  *o++ = 0;  *o++ = 0;  // MTIME
    *o++ = 0x00;    // XFL
    *o++ = 0x00;    // OS
    len = deflate_compress(in, insz, o, outsz - (o - out));
    if(len < 0) return(len);
    o += len;
    crc = crc32(0, in, insz);  // CRC32
    *o++ = crc;
    *o++ = crc >> 8;
    *o++ = crc >> 16;
    *o++ = crc >> 24;
    *o++ = insz;    // ISIZE
    *o++ = insz >> 8;
    *o++ = insz >> 16;
    *o++ = insz >> 24;
    return(o - out);
}



// modified from http://cvs.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/uts/common/fs/zfs/lzjb.c
#define NBBY 8
#define	MATCH_BITS	6
#define	MATCH_MIN	3
#define	MATCH_MAX	((1 << MATCH_BITS) + (MATCH_MIN - 1))
#define	OFFSET_MASK	((1 << (16 - MATCH_BITS)) - 1)
#define	LEMPEL_SIZE	256

int
lzjb_decompress(u8 *s_start, u8 *d_start, size_t s_len, size_t d_len)
{
	u8 *src = s_start;
	u8 *dst = d_start;
	u8 *d_end = (u8 *)d_start + d_len;
	u8 *cpy, copymap = 0;
	int copymask = 1 << (NBBY - 1);

	while (dst < d_end) {
		if ((copymask <<= 1) == (1 << NBBY)) {
			copymask = 1;
			copymap = *src++;
		}
		if (copymap & copymask) {
			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
			src += 2;
			if ((cpy = dst - offset) < (u8 *)d_start)
				return (-1);
			while (--mlen >= 0 && dst < d_end)
				*dst++ = *cpy++;
		} else {
			*dst++ = *src++;
		}
	}
	return (dst - d_start);
}



// from http://rosettacode.org/wiki/Run-length_encoding#C
/*
int rle_decode(char *out, const char *in, int l)
{
  int i, tb;
  char c;
 
  for(tb=0 ; l>=0 ; l -= 2 ) {
    i = *in++;
    c = *in++;
    tb += i;
    while(i-- > 0) *out++ = c;
  }
  return tb;
}
*/

// http://www.compuphase.com/compress.htm
int unrle(unsigned char *output,unsigned char *input,int length)
{
  signed char count;
  unsigned char *o = output;

  while (length>0) {
    count=(signed char)*input++;
    if (count>0) {
      /* replicate run */
      memset(o,*input++,count);
    } else if (count<0) {
      /* literal run */
      count=(signed char)-count;
      memcpy(o,input,count);
      input+=count;
    } /* if */
    o+=count;
    length-=count;
  } /* while */
  return(o - output);
}



// must be configured
int another_rle(u8 *in, int insz, u8 *out, int outsz) {
    int     escape_chr = 0,
            i,
            o,
            c,
            n,
            lastc   = 0x80;

    if(comtype_dictionary) {
        //sscanf(comtype_dictionary, "%d", &escape_chr);
        get_parameter_numbers(comtype_dictionary, &escape_chr, NULL);
    }

    for(i = o = 0; ; lastc = c) {
        if(i >= insz) break;
        c = in[i++];
        if(c == escape_chr) {
            if(i >= insz) break;
            n = in[i++];
            if(n == escape_chr) {
                if(o >= outsz) return(-1);
                out[o++] = escape_chr;
            } else {
                if((o + n) > outsz) return(-1);
                memset(out + o, lastc, n);
                o += n;
            }
        } else {
            if(o >= outsz) return(-1);
            out[o++] = c;
        }
    }
    return(o);
}



int unquicklz(u8 *in, int insz, u8 *out, int outsz) {
    static qlz_state_decompress *state = NULL;
    int     tmp;

    if(!state) {
        state = malloc(sizeof(qlz_state_decompress));
        if(!state) return(-1);
    }
    memset(state, 0, sizeof(qlz_state_decompress));
    tmp = qlz_size_decompressed(in);
    if((tmp < 0) || (tmp > outsz)) return(-1);
    return(qlz_decompress(in, out, state));
}



// from libavcodec lcldec.c for the LossLess Codec Library
unsigned mszh_decomp(unsigned char * srcptr, int srclen, unsigned char * destptr, unsigned destsize)
{
    unsigned char *destptr_bak = destptr;
    unsigned char *destptr_end = destptr + destsize;
    unsigned char mask = 0;
    unsigned char maskbit = 0;
    unsigned ofs, cnt;

    while ((srclen > 0) && (destptr < destptr_end)) {
        if (maskbit == 0) {
            mask = *(srcptr++);
            maskbit = 8;
            srclen--;
            continue;
        }
        if ((mask & (1 << (--maskbit))) == 0) {
            if (destptr + 4 > destptr_end)
                break;
            *(int*)destptr = *(int*)srcptr;
            srclen -= 4;
            destptr += 4;
            srcptr += 4;
        } else {
            ofs = *(srcptr++);
            cnt = *(srcptr++);
            ofs += cnt * 256;
            cnt = ((cnt >> 3) & 0x1f) + 1;
            ofs &= 0x7ff;
            srclen -= 2;
            cnt *= 4;
            if (destptr + cnt > destptr_end) {
                cnt =  destptr_end - destptr;
            }
            if((destptr - ofs) < destptr_bak) return(-1);
            if((destptr + cnt) > destptr_end) return(-1);
            for (; cnt > 0; cnt--) {
                *(destptr) = *(destptr - ofs);
                destptr++;
            }
        }
    }

    return (destptr - destptr_bak);
}



int uudecode(u8 *in, int insz, u8 *out, int outsz, int xxe) {
    QUICK_IN_OUT
    int     cnt,
            c,
            m = 0;
    u8      a = 0,
            b = 0;
    static const u8 xxe_set[] = "+-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if(!strnicmp(in, "begin", 5)) {
        while((in < inl) && (*in != '\n') && (*in != '\r')) in++;
    }

    for(cnt = -1; in < inl; in++) {
        if(*in < ' ') {
            if(!strnicmp(in + 1, "end", 3)) break;
            continue;
        }
        a = b;
        if(xxe) {
            for(b = 0; xxe_set[b]; b++) {
                if(xxe_set[b] == *in) break;
            }
            b &= 0x3f;
        } else {
            b = (*in - ' ') & 0x3f;
        }
        if(m <= 0) {
            if(!strnicmp(in, "end", 3)) break;
            cnt = -1;
        }
        if(cnt < 0) {
            m = b;
        } else {
            switch(cnt & 3) {
                case 0: c = -1;                     break;
                case 1: c = (a << 2) | (b >> 4);    break;
                case 2: c = (a << 4) | (b >> 2);    break;
                case 3: c = (a << 6) | b;           break;
                default: break;
            }
            if(c >= 0) {
                if(o >= outl) return(-1);
                *o++ = c;
                m--;
            }
        }
        cnt++;
    }
    return(o - out);
}



// partially derived from http://www.stillhq.com/svn/trunk/ascii85/decode85.c
int unascii85(u8 *in, int insz, u8 *out, int outsz) {
    QUICK_IN_OUT
    static const unsigned pow85[] = { 85*85*85*85, 85*85*85, 85*85, 85, 1 };
    unsigned tuple;
    int     count,
            c;

    tuple = 0;
    count = 0;
    for(;;) {
        if(in >= inl) break;
        c = *in++;
        if(c <= ' ') continue;
        if(c == '<') {
            if(in >= inl) break;
            c = *in++;
            if(c == '~') {
                for(;;) {
                    if(in >= inl) break;
                    c = *in++;
                    if(c == 'z') {
                        if(count) break;
                        if((o + 4) > outl) return(-1);
                        *o++ = 0;
                        *o++ = 0;
                        *o++ = 0;
                        *o++ = 0;
                    } else if(c == '~') {
                        if(in >= inl) break;
                        c = *in++;
                        if(c == '>') {
                            if(count > 0) {
                                count--;
                                tuple += pow85[count];
                                if((o + count) > outl) return(-1);
                                if(count >= 1) *o++ = tuple >> 24;
                                if(count >= 2) *o++ = tuple >> 16;
                                if(count >= 3) *o++ = tuple >> 8;
                                if(count >= 4) *o++ = tuple;
                            }
                            if(in >= inl) break;
                            c = *in++;
                            break;
                        }
                    } else if(c <= ' ') {
                    } else {
                        if((c < '!') || (c > 'u')) break;
                        tuple += (c - '!') * pow85[count++];
                        if(count == 5) {
                            if((o + 4) > outl) return(-1);
                            *o++ = tuple >> 24;
                            *o++ = tuple >> 16;
                            *o++ = tuple >> 8;
                            *o++ = tuple;
                            tuple = 0;
                            count = 0;
                        }
                    }
                }
            } else {
                if((o + 2) > outl) return(-1);
                *o++ = '<';
                *o++ = c;
            }
        } else {
            if(o >= outl) return(-1);
            *o++ = c;
        }
    }
    return(o - out);
}



int unyenc(u8 *in, int insz, u8 *out, int outsz) {
    QUICK_IN_OUT
    u8      c;

    for(;;) {
        if(in >= inl) break;
        c = *in++;
        if((c == '\n') || (c == '\r')) continue;
        if(c == '=') {
            if(in >= inl) break;
            c = *in++;
            if(c == 'y') {
                while((in < inl) && (*in != '\n') && (*in != '\r')) in++;
                continue;
            }
            c -= 64;
        }
        c -= 42;
        if(o >= outl) return(-1);
        *o++ = c;
    }
    return(o - out);
}



int doomhuff(int type, u8 *in, int insz, u8 *out, int outsz, int enc) {
    float   myfreq[256],
            t;
    int     i,
            n;
    u8      *p;

    if(comtype_dictionary) {
        p = comtype_dictionary;
        for(i = 0; i < 256; i++) {
            if(sscanf(p, "%f%n", &t, &n) != 1) break;
            myfreq[i] = t;
            for(p += n; *p; p++) {
                if(*p <= ' ') continue;
                if(*p == ',') continue;
                break;
            }
        }
        if(i < 256) {
            fprintf(stderr, "\nError: the provided custom huffman table is incomplete (%d elements)\n", i);
            return(-1);
        }
        doom_HuffInit(myfreq);
    } else if(type == 1) {
        doom_HuffInit(zdaemon_HuffFreq);
    } else if(type == 2) {
        doom_HuffInit(skulltag_HuffFreq);
    } else {
        doom_HuffInit(NULL);
    }
    if(!enc) {
        doom_HuffDecode(in, out, insz, &outsz, outsz);
    } else {
        doom_HuffEncode(in, out, insz, &outsz);
    }
    return(outsz);
}



// from Arkadi Kagan http://compressions.sourceforge.net/about.html
// converted to C by Luigi Auriemma
// I have ported only this one because the others are a mission-impossible, C++ sux
int CLZ77_Decode(unsigned char *target, long tlen, unsigned char *source, long slen) {
    static const int BITS_LEN = 4;
	long i;
	long block, len;
	long shift, border;
	unsigned char *s, *t, *p, *tl;
	unsigned char *flag;
	short *ptmp;

	t = target;
    tl = target + tlen;
	flag = source;
	block = 0;				// block - bit in single flag unsigned char
	shift = 16;				// shift offset to most significant bits
	border = 1;				// offset can`t be more then border
	for (s = source+1; (s < source+slen) && (t-target < tlen); )
	{
		if (shift > BITS_LEN)
			while (t-target >= border)
			{
				if (shift <= BITS_LEN) break;
				border = border << 1;
				shift--;
			}
		if (flag[0]&(1<<block))
		{
			ptmp = (short*)s;
			len = ((1<<shift)-1)&ptmp[0];
			p = t - (ptmp[0]>>shift) - 1;
            if((t + len) > tl) return(-1);
			for (i = 0; i < len; i++) {
				t[i] = p[i];
            }
			t += len;
			s += 2;
		} else
		{
            if(t >= tl) return(-1);
			*t++ = *s++;
			len = 1;
		}
		if (++block >= 8)
		{
			flag = s++;
			block = 0;
		}
	}
    return(t - target);
}



// http://www.ross.net/compression/download/original/old_lzrw1-a.c
int lzrw1_decompress(unsigned char *p_src_first, unsigned char *p_dst_first, unsigned src_len, int dst_len) {
    unsigned char  *p_src = p_src_first + 4,
	*p_dst = p_dst_first,
	*p_dst_end = p_dst_first + dst_len;
    unsigned char  *p_src_post = p_src_first + src_len;
    unsigned char  *p_src_max16 = p_src_first + src_len - (16 * 2);
    unsigned        control = 1;
    if (*p_src_first == 1) {	// fast_copy(p_src_first+4,p_dst_first,src_len-4);
    	memcpy(p_dst_first, p_src_first + 4, src_len - 4);
    	return (src_len - 4);
    }
    while (p_src != p_src_post) {
    	unsigned        unroll;
    	if (control == 1) {
    	    control = 0x10000 | *p_src++;
    	    control |= (*p_src++) << 8;
    	}
    	unroll = p_src <= p_src_max16 ? 16 : 1;
    	while (unroll--) {
    	    if (control & 1) {
    	    	unsigned        lenmt;
        		unsigned char  *p;
        		lenmt = *p_src++;
        		p = p_dst - (((lenmt & 0xF0) << 4) | *p_src++);
                if(p < p_dst_first) return(-1);
                if((p_dst + 3) > p_dst_end) return(-1);
        		*p_dst++ = *p++;
        		*p_dst++ = *p++;
        		*p_dst++ = *p++;
        		lenmt &= 0xF;
                if((p_dst + lenmt) > p_dst_end) return(-1);
                while (lenmt--) {
                    *p_dst++ = *p++;
                }
    	    } else {
        		if(p_dst >= p_dst_end) return(-1);
        		*p_dst++ = *p_src++;
    	    }
    	    control >>= 1;
    	}
    }
    return (p_dst - p_dst_first);
}




// modified by Luigi Auriemma
/*
 *  DHUFF.C:    Huffman Decompression Program.                            *
 *              14-August-1990    Bill Demas          Version 1.0         *
*/
int undhuff(unsigned char *in, int insz, unsigned char *out, int outsz) {
short           decomp_tree[512];
unsigned short  code[256];
unsigned char   code_length[256];

    unsigned char *inl;
    inl = in + insz;

    memset(decomp_tree, 0, sizeof(decomp_tree));
    memcpy(code, in, sizeof(code));                 in += sizeof(code);
    memcpy(code_length, in, sizeof(code_length));   in += sizeof(code_length);

   unsigned short  loop1;
   unsigned short  current_index;
   unsigned short  loop;
   unsigned short  current_node = 1;

   decomp_tree[1] = 1;

   for (loop = 0; loop < 256; loop++)
   {
      if (code_length[loop])
      {
	 current_index = 1;
	 for (loop1 = code_length[loop] - 1; loop1 > 0; loop1--)
	 {
	    current_index = (decomp_tree[current_index] << 1) +
			    ((code[loop] >> loop1) & 1);
        if(current_index > 512) return(-1);
	    if (!(decomp_tree[current_index]))
	       decomp_tree[current_index] = ++current_node;
	 }
	 decomp_tree[(decomp_tree[current_index] << 1) +
	   (code[loop] & 1)] = -loop;
      }
   }

   unsigned short  cindex = 1;
   unsigned char   curchar;
   short           bitshift;

   unsigned  charcount = 0L;

   while (charcount < outsz)
   {
      if(in >= inl) break;
      curchar = *in++;;

      for (bitshift = 7; bitshift >= 0; --bitshift)
      {
	 cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

	 if (decomp_tree[cindex] <= 0)
	 {
        //if(charcount >= outsz) return(-1); // not necessary
        out[charcount] = (int) (-decomp_tree[cindex]);

	    if ((++charcount) == outsz)
               bitshift = 0;
            else
               cindex = 1;
	 }
	 else
	    cindex = decomp_tree[cindex];
      }
   }
    return(charcount);
}



// Finish submission to the Dr Dobbs contest written by Jussi Puttonen, Timo Raita and Jukka Teuhola.
int unfin(unsigned char *in, int insz, unsigned char *out, int outsz) {
#define FIN_INDEX(p1,p2) (((unsigned)(unsigned char)p1<<7)^(unsigned char)p2)
static char pcTable[32768U];
   int ci,co;            // characters (in and out)
   char p1=0, p2=0;      // previous 2 characters
   int ctr=8;            // number of characters processed for this mask
   unsigned char mask=0; // mask to mark successful predictions

int i = 0;
int o = 0;

   memset (pcTable, 32, 32768U); // space (ASCII 32) is the most used char

   for(;;) {
      if(i >= insz) break;
      ci = in[i++];
      // get mask (for 8 characters)
      mask = (unsigned char)(char)ci;

      // for each bit in the mask
      for (ctr=0; ctr<8; ctr++){
         if (mask & (1<<ctr)){
            // predicted character
            co = pcTable[FIN_INDEX(p1,p2)];
         } else {
            // not predicted character
            if(i >= insz) break;
            co = in[i++];
	    pcTable[FIN_INDEX(p1,p2)] = (char)co;
         }
         if(o >= outsz) return(-1);
         out[o++] = co;
         p1 = p2; p2 = co;
      }
   }
   return(o);
}



// Copyright (c) 2002 Chilkat Software, Inc.  All Rights Reserved
int CK_RLE_decompress(unsigned char *buf,
    int len, 
    unsigned char *out,
    int uncompressLen)
    {
    unsigned char header;
    unsigned char *outPtr = out;
    unsigned char i;
    int outSize = 0;

    while (len)
	{
	header = *buf;
	buf++;
	len--;

	if (!(header & 128))
	    {
	    // There are header+1 different bytes.
	    for (i=0; i<=header; i++)
		{
		if (outSize >= uncompressLen) return -1;
		*outPtr = *buf;
		outPtr++;
		outSize++;
		buf++;
		len--;
		}
	    }
	else
	    {
	    unsigned n = (header & 127) + 2;
	    for (i=0; i<n; i++)
		{
		if (outSize >= uncompressLen) return -1;
		*outPtr = *buf;
		outPtr++;
		outSize++;
		}
	    buf++;
	    len--;
	    }
	}

    uncompressLen = outSize;
    //return 0;
    return(uncompressLen);
    }



// this function has been created by me from scratch (based on calcc) and is NOT optimized
// note: if outsz is not of the exact size then the returned size will be padded
int multi_base_decoder(int base, int alt, u8 *in, int insz, u8 *out, int outsz, u8 *mytable) {
static const u8 big_table[256] =
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";
static const u8 hex_table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const u8 b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const u8 g64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]";   // gamespy
static const u8 b32_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const u8 z32_table[] = "ybndrfg8ejkmcpqxot1uwisza345h769";   // z-base-32
static const u8 c32_table[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";   // Crockford's base32
static const u8 n32_table[] = "0123456789BCDFGHJKLMNPQRSTVWXYZ.";   // Nintendo
    u64     num;
    int     i,
            block    = 0,
            blockcpy = 0;
    u8      *p;
    u8      *table = NULL;

    if(mytable) {
        table = mytable;
    } else {
        if((base > 0) && (base <= 16)) {
            table = (u8 *)hex_table;
        } else if(base == 32) {
            switch(alt) {
                case 0: table = (u8 *)b32_table;    break;
                case 1: table = (u8 *)z32_table;    break;
                case 2: table = (u8 *)hex_table;    break;
                case 3: table = (u8 *)c32_table;    break;
                case 4: table = (u8 *)n32_table;    break;
                default: break;
            }
        } else if(base == 64) {
            switch(alt) {
                case 0: table = (u8 *)b64_table;    break;
                case 1: table = (u8 *)g64_table;    break;
                default: break;
            }
        } else if(base <= 62) {
            table = (u8 *)hex_table;
        } else {
            table = (u8 *)big_table;
        }
        if(!table) return(-1);
    }

    num = 1;    // I'm sure that exists a better and simpler way but I'm stupid, sorry
    for(block = 0; num; block++) {
        num *= base;
        switch(num - 1) {
            case 0xffLL:
            case 0xffffLL:
            case 0xffffffLL:
            case 0xffffffffLL:
            case 0xffffffffffLL:
            case 0xffffffffffffLL:
            case 0xffffffffffffffLL:
            case 0xffffffffffffffffLL: {
                for(blockcpy = 0, --num; num; num >>= 8, blockcpy++);
                //num = 0;  // num is already 0
                break;
            }
            default: break;
        }
    }
    if(!(block & 1)) block--;   // if pair

    QUICK_IN_OUT

    num = 0;
    for(i = 0;; i++) {
        num *= (u64)base;
        if(in < inl) {
            p = memchr(table, *in, base);
            if(p) {
                num += (p - table);
                in++;
            } else {
                in = inl; // finish
            }
        }
        if(i >= block) {
            i = blockcpy;
            while(i--) {
                if(o >= outl) /* do NOT use return(-1) here */ break;
                *o++ = num >> (u64)(i * 8);
            }
            num = 0;
            i = -1;
            if(in >= inl) break;
        }
    }
    //*o++ = 0;
    return(o - out);
}



int unlzhlib(unsigned char *in, int insz, unsigned char *out, int outsz) {
    QUICK_IN_OUT

    void *mymalloc (unsigned n) {return malloc (n);}
    void myfree (void *p) {free (p);}
    int myread (void *p, int n) {
        if((in + n) > inl) n = inl - in;
        memcpy(p, in, n);
        in += n;
        return(n);
    }
    int mywrite (void *p, int n) {
        if((o + n) > outl) return(-1);
        memcpy(o, p, n);
        o += n;
        return(n);
    }
    lzh_melt(myread, mywrite, mymalloc, myfree, outsz);
    return(o - out);
}



/***********************************************************
*
*    rle3.c
*
*      儔儞儗儞僌僗晞崋壔偺僥僗僩
*      儔儞挿晹暘傪儅乕僋晞崋偱幆暿偡傞
*
***********************************************************/
//int rl3_decode(unsigned char *out, unsigned char *in, int size) {
int rl3_decode(unsigned char *in, int insz, unsigned char *out, int outsz) {
#define MINLEN   4
#define LIMIT1   240
#define LIMIT2   (256 * (256 - LIMIT1) + LIMIT1 - 1)
unsigned char mch = 'Z';
	int rpos = 0, wpos = 0;
	while (rpos < insz) {
		int c = in[rpos++];
		int i, le;
		if (c == mch) {
			/* 儔儞挿傪擖椡 */
			le = in[rpos++];
			if (le >= LIMIT1) {
				le = ((le - LIMIT1) << 8) + in[rpos++] + LIMIT1;
			}
			le++;
			if (le >= MINLEN)  c = in[rpos++];
            if((wpos + le) > outsz) return(-1);
			for (i = 0; i < le; i++)  out[wpos++] = c;
		} else {
            if(wpos >= outsz) return(-1);
			out[wpos++] = c;
		}
	}
	return wpos;
}



typedef struct {
    unsigned char   *o;
    unsigned char   *ol;
} sr3c_write_ctx;
static int sr3c_write(const unsigned char *bytes, size_t n, int flush, sr3c_write_ctx *wc) {
    if((wc->o + n) > wc->ol) n = wc->ol - wc->o;
    if(n <= 0) return(-1);
    memcpy(wc->o, bytes, n);
    wc->o += n;
    return(0);
}
int unsr3c(unsigned char *in, int insz, unsigned char *out, int outsz) {
    sr3c_context_t *ctx;
    sr3c_write_ctx  wc;

    wc.o  = out;
    wc.ol = out + outsz;
    ctx = sr3c_alloc((sr3c_output_f_t)sr3c_write, &wc);
    if(sr3c_uncompress(in, insz, ctx)) {
        sr3c_free(ctx);
        return(-1);
    }
    sr3c_free(ctx);
    return(wc.o - out);
}



// blah, practically the library contains both the smart and simple mode
// while the pre-compiled demo on the website uses only the smart mode
// and so it's needed to select the smart-only or smart+simple mode
int SFUnpackSeg(unsigned char *in, int insz, unsigned char *out, int outsz, int smart_only) {
    int     i   = 0,
            o   = 0,
            t,
            a,
            c,
            ident = 0;

    if(insz < 2) return(-1);
    t = in[i] | (in[i + 1] << 8);
    i += 2;
    if(!smart_only) {
        if(t <= 0xff) {
            ident = t;
            t = 0;
        }
    }
    for(;;) {
        if(t) {
            if(insz <= t) break;
            c = in[insz - 2] | (in[insz - 1] << 8);
            insz -= 2;
            c -= (i + 1);
            if((o + c) > outsz) return(-1);
            memcpy(out + o, in + i, c);
            i += c;
            o += c;
        } else {
            if(i >= insz) break;
            a = in[i++];
            if(a != ident) {
                if((o + 1) > outsz) return(-1);
                out[o++] = a;
                continue;
            }
        }
        a = in[i++];
        c = in[i++];
        if(!c) {
            c = in[i] | (in[i + 1] << 8);
            i += 2;
        }
        c++;
        if((o + c) > outsz) return(-1);
        memset(out + o, a, c);
        o += c;
    }
    if(t) {
        c = insz - i;
        if((o + c) > outsz) return(-1);
        memcpy(out + o, in + i, c);
        i += c;
        o += c;
    }
    return(o);
}
int SFUnpack(unsigned char *in, int insz, unsigned char *out, int outsz, int smart_only) {
    int     i   = 0,
            o   = 0,
            r,
            chunksz;

    for(;;) {
        if((i + 2) > insz) break;
        chunksz = in[i] | (in[i + 1] << 8);
        i += 2;
        if((i + chunksz) > insz) break;
        r = SFUnpackSeg(in + i, chunksz, out + o, outsz - o, smart_only);
        if(r < 0) break;
        i += chunksz;
        o += r;
    }
    return(o);
}



// based on the information of Guy Ratajczak, it's just a lz77
int undarkstone(unsigned char *in, int insz, unsigned char *out, int outsz) {
    QUICK_IN_OUT
    int     i,
            j,
            flags,
            info,
            num;
    u8      *p;

    for(;;) {
        if(in >= inl) break;
        flags = *in++;
        for(i = 0; i < 8; i++) {
            if(o >= outl) break;    // needed
            if(flags & 1) {
                if(in >= inl) break;
                if(o >= outl) return(-1);
                *o++ = *in++;
            } else {
                if((in + 2) > inl) break;
                info = in[0] | (in[1] << 8);
                in += 2;
                num = 3 + (info >> 10);
                p = o - (info & 0x3ff);
                if(p < out) return(-1);
                if((o + num) > outl) return(-1);
                for(j = 0; j < num; j++) {
                    *o++ = *p++;
                }
            }
            flags >>= 1;
        }
    }
    return(o - out);
}



int sfl_block_chunked(unsigned char *in, int insz, unsigned char *out, int outsz) {
    int     chunk_zsize,
            chunk_size,
            i = 0,
            o = 0;

    while(o < outsz) {
        if((i + 2) > insz) break;
        chunk_zsize = in[i] | (in[i + 1] << 8);
        if(!chunk_zsize) break;
        i += 2;
        if((i + chunk_zsize) > insz) break;
        chunk_size = expand_block(in + i, out + o, chunk_zsize, outsz - o);
        i += chunk_zsize;
        o += chunk_size;
    }
    return(o);
}



int sfl_block_chunked_compress(unsigned char *in, int insz, unsigned char *out, int outsz) {
    int     chunk_zsize,
            chunk_size,
            i = 0,
            o = 0;

    while(i < insz) {
        chunk_size = 0x7fff - 1;    // consider copy flag
        if(chunk_size > (insz - i)) chunk_size = insz - i;
        chunk_zsize = compress_block(in + i, out + o + 2, chunk_size);
        if(chunk_zsize < 0) return -1;
        out[o]     = chunk_zsize;
        out[o + 1] = chunk_zsize >> 8;
        i += chunk_size;
        o += 2 + chunk_zsize;
    }
    return o;
}



// code from tpu: http://forum.xentax.com/viewtopic.php?p=30387#p30387
/* PRS get bit form lsb to msb, FPK get it form msb to lsb */
int prs_8ing_get_bits(int n, char *sbuf, int *sptr, int *blen)
{
    static int fbuf = 0;
   int retv;

   retv = 0;
   while(n){
      retv <<= 1;
      if((*blen)==0){
         fbuf = sbuf[*sptr];
         //if(*sptr<256)
            //{ fprintf(stderr, "[%02x] ", fbuf&0xff); fflush(0); }
         (*sptr)++;
         (*blen) = 8;
      }

      if(fbuf&0x80)
         retv |= 1;

      fbuf <<= 1;
      (*blen) --;
      n --;
   }

   return retv;
}
int prs_8ing_uncomp(char *dbuf, int dlen, char *sbuf, int slen)
{
   int sptr;
   int dptr;
   int i, flag, len, pos;

   int blen = 0;

   sptr = 0;
   dptr = 0;
   while(sptr<slen){
      flag = prs_8ing_get_bits(1, sbuf, &sptr, &blen);
      if(flag==1){
         //if(sptr<256)
            //{ fprintf(stderr, "%02x ", (u8)sbuf[sptr]); fflush(0); }
         if(dptr<dlen)
            dbuf[dptr++] = sbuf[sptr++];
      }else{
         flag = prs_8ing_get_bits(1, sbuf, &sptr, &blen);
         if(flag==0){
            len = prs_8ing_get_bits(2, sbuf, &sptr, &blen)+2;
            pos = sbuf[sptr++]|0xffffff00;
         }else{
            pos = (sbuf[sptr++]<<8)|0xffff0000;
            pos |= sbuf[sptr++]&0xff;
            len = pos&0x07;
            pos >>= 3;
            if(len==0){
               len = (sbuf[sptr++]&0xff)+1;
            }else{
               len += 2;
            }
         }
         //if(sptr<256)
            //{ fprintf(stderr, "<%08x(%08x): %08x %d> \n", dptr, dlen, pos, len); fflush(0); }
         pos += dptr;
         for(i=0; i<len; i++){
            if(dptr<dlen)
               dbuf[dptr++] = dbuf[pos++];
         }
      }
   }

   return dptr;
}



// from cpk_uncompress.c of hcs: http://hcs64.com/files/utf_tab04.zip
// modified by Luigi Auriemma
// Decompress compressed segments in CRI CPK filesystems
static inline unsigned short CPK_get_next_bits(unsigned char *infile, int * const offset_p, unsigned char * const bit_pool_p, int * const bits_left_p, const int bit_count)
{
    unsigned short out_bits = 0;
    int num_bits_produced = 0;
    while (num_bits_produced < bit_count)
    {
        if (0 == *bits_left_p)
        {
            *bit_pool_p = infile[*offset_p];
            *bits_left_p = 8;
            --*offset_p;
        }

        int bits_this_round;
        if (*bits_left_p > (bit_count - num_bits_produced))
            bits_this_round = bit_count - num_bits_produced;
        else
            bits_this_round = *bits_left_p;

        out_bits <<= bits_this_round;
        out_bits |=
            (*bit_pool_p >> (*bits_left_p - bits_this_round)) &
            ((1 << bits_this_round) - 1);

        *bits_left_p -= bits_this_round;
        num_bits_produced += bits_this_round;
    }

    return out_bits;
}

#define CPK_GET_NEXT_BITS(bit_count) CPK_get_next_bits(infile, &input_offset, &bit_pool, &bits_left, bit_count)

int CPK_uncompress(unsigned char *infile, int input_size, unsigned char *output_buffer, int uncompressed_size) {
    if(uncompressed_size < 0x100) return(-1);
    uncompressed_size -= 0x100; // blah, terrible algorithm or terrible implementation

    const int input_end = input_size - 0x100 - 1;
    int input_offset = input_end;
    const int output_end = 0x100 + uncompressed_size - 1;
    unsigned char bit_pool = 0;
    int bits_left = 0;
    int bytes_output = 0;
    int     i;

    if(input_size < 0x100) return(-1);
    memcpy(output_buffer, infile + input_size - 0x100, 0x100);

    while ( bytes_output < uncompressed_size )
    {
        if(input_offset < 0) break;
        if (CPK_GET_NEXT_BITS(1))
        {
            int backreference_offset =
                output_end-bytes_output+CPK_GET_NEXT_BITS(13)+3;
            int backreference_length = 3;

            // decode variable length coding for length
            enum { vle_levels = 4 };
            int vle_lens[vle_levels] = { 2, 3, 5, 8 };
            int vle_level;
            for (vle_level = 0; vle_level < vle_levels; vle_level++)
            {
                int this_level = CPK_GET_NEXT_BITS(vle_lens[vle_level]);
                backreference_length += this_level;
                if (this_level != ((1 << vle_lens[vle_level])-1)) break;
            }
            if (vle_level == vle_levels)
            {
                int this_level;
                do
                {
                    this_level = CPK_GET_NEXT_BITS(8);
                    backreference_length += this_level;
                } while (this_level == 255);
            }

            //printf("0x%08lx backreference to 0x%lx, length 0x%lx\n", output_end-bytes_output, backreference_offset, backreference_length);
            for (i=0;i<backreference_length;i++)
            {
                output_buffer[output_end-bytes_output] = output_buffer[backreference_offset--];
                bytes_output++;
            }
        }
        else
        {
            // verbatim byte
            output_buffer[output_end-bytes_output] = CPK_GET_NEXT_BITS(8);
            //printf("0x%08lx verbatim byte\n", output_end-bytes_output);
            bytes_output++;
        }
    }

    return 0x100 + bytes_output;
}



/* ----------
 * pg_lzcompress.c -
 *
 *		This is an implementation of LZ compression for PostgreSQL.
 *		It uses a simple history table and generates 2-3 byte tags
 *		capable of backward copy information for 3-273 bytes with
 *		a max offset of 4095.
 ...(cut)...
 * Copyright (c) 1999-2009, PostgreSQL Global Development Group
 *
 * $PostgreSQL: pgsql/src/backend/utils/adt/pg_lzcompress.c,v 1.34 2009/06/11 14:49:03 momjian Exp $
 * ----------
 */
int
pglz_decompress(unsigned char *in, int insz, unsigned char *out, int outsz)
{
	const unsigned char *sp;
	const unsigned char *srcend;
	unsigned char *dp;
	unsigned char *destend;

	//sp = ((const unsigned char *) source) + sizeof(PGLZ_Header);
	//srcend = ((const unsigned char *) source) + VARSIZE(source);
	//dp = (unsigned char *) dest;
	//destend = dp + source->rawsize;
    sp      = in;
    srcend  = in + insz;
    dp      = out;
    destend = out + outsz;

	while (sp < srcend && dp < destend)
	{
		/*
		 * Read one control byte and process the next 8 items (or as many as
		 * remain in the compressed input).
		 */
		unsigned char ctrl = *sp++;
		int			ctrlc;

		for (ctrlc = 0; ctrlc < 8 && sp < srcend; ctrlc++)
		{
			if (ctrl & 1)
			{
				/*
				 * Otherwise it contains the match length minus 3 and the
				 * upper 4 bits of the offset. The next following byte
				 * contains the lower 8 bits of the offset. If the length is
				 * coded as 18, another extension tag byte tells how much
				 * longer the match really was (0-255).
				 */
				int		len;
				int		off;

				len = (sp[0] & 0x0f) + 3;
				off = ((sp[0] & 0xf0) << 4) | sp[1];
				sp += 2;
				if (len == 18)
					len += *sp++;

				/*
				 * Check for output buffer overrun, to ensure we don't clobber
				 * memory in case of corrupt input.  Note: we must advance dp
				 * here to ensure the error is detected below the loop.  We
				 * don't simply put the elog inside the loop since that will
				 * probably interfere with optimization.
				 */
				if (dp + len > destend)
				{
					dp += len;
					break;
				}

				/*
				 * Now we copy the bytes specified by the tag from OUTPUT to
				 * OUTPUT. It is dangerous and platform dependent to use
				 * memcpy() here, because the copied areas could overlap
				 * extremely!
				 */
				while (len--)
				{
					*dp = dp[-off];
					dp++;
				}
			}
			else
			{
				/*
				 * An unset control bit means LITERAL BYTE. So we just copy
				 * one from INPUT to OUTPUT.
				 */
				if (dp >= destend)		/* check for buffer overrun */
					break;		/* do not clobber memory */

				*dp++ = *sp++;
			}

			/*
			 * Advance the control bit
			 */
			ctrl >>= 1;
		}
	}

	/*
	 * Check we decompressed the right amount.
	 */
	//if (dp != destend || sp != srcend)
		//return(-1); //elog(ERROR, "compressed data is corrupt");

	/*
	 * That's it.
	 */
    return(dp - out);
}



/*
Simple Compression using an LZ buffer
Part 3 Revision 1.d:
An introduction to compression on the Amiga by Adisak Pochanayon
*/
// modified by Luigi Auriemma
#define HISTORY_SIZE     4096
#define MASK_history     (HISTORY_SIZE-1)
#define MASK_upper       (0xF0)
#define MASK_lower       (0x0F)
#define SHIFT_UPPER      16
#define LSR_upper        4
#define MAX_COMP_LEN     17
unsigned char LZ_history[HISTORY_SIZE];

#define UnPackSLZ_writechar(outchar) \
{ \
  if(o >= outl) return(-1); \
  *o++ = outchar; \
  LZ_history[lzhist_offset]=outchar; lzhist_offset=(lzhist_offset+1)&MASK_history; \
}

int UnPackSLZ(unsigned char *in, int insz, unsigned char *out, int outsz) {
    QUICK_IN_OUT

  short myTAG, mycount, myoffset;
  int loop1;
  short lzhist_offset=0;

  for(;;)  // loop forever (until goto occurs to break out of loop)
    {
      if(in >= inl) break;
      myTAG=*in++;
      for(loop1=0;(loop1!=8);loop1++)
        {
          if(myTAG&0x80)
            {
              if(in >= inl) break;
              if((mycount=*in++)==0)  // Check EXIT
                { goto skip2; } // goto's are gross but it's efficient :(
              else
                {
                  if(in >= inl) break;
                  myoffset=HISTORY_SIZE-(((MASK_upper&mycount)*SHIFT_UPPER)+(*in++));
                  mycount&=MASK_lower;
                  mycount+=2;
                  while(mycount!=0)
                    {
                      UnPackSLZ_writechar(LZ_history[(lzhist_offset+myoffset)&MASK_history]);
                      mycount--;
                    }
                }
            }
          else
            { if(in >= inl) break; UnPackSLZ_writechar(*in++); }
          myTAG+=myTAG;
        }
    }
skip2:
  return(o - out);
}



int slz_triace_blah(int x, int n, int a, unsigned char **o, unsigned char *outl) {
    unsigned char   *t = *o;
    if(x < n) {
        n = (((n - x) - 1) >> 1) + 1;
        x += n * 2;
        if((t + (n * 2)) > outl) return(-1);
        while(n--) {
            *t++ = a;
            *t++ = a >> 8;
        }
        *o = t;
    }
    return(x);
}
int slz_triace_old(unsigned char *in, int insz, unsigned char *out, int outsz, int type) {
    QUICK_IN_OUT
    int     flag, n, x, a;

    for(flag = 0; o < outl; flag >>= 1) {
        if(!(flag & 0xff00)) {
            if(in >= inl) break;
            flag = 0xff00 | *in++;
        }
        if(flag & 1) {
            if(in >= inl) break;
            if(o >= outl) return(-1);
            *o++ = *in++;
        } else {
            if((in + 2) > inl) break;
            n = *in++;
            n |= (*in++ << 8);
            x = ((n >> 12) & 0x0f) + 3;
            n &= 0x0fff;
            if((x < 0x12) || (type != 2)) {
                if((o - n) < out) return(-1);
                if((o + x) > outl) return(-1);
                while(x--) {
                    *o = *(o - n);
                    o++;
                }
            } else {
                if(n < 0x100) {
                    if(in >= inl) break;
                    a = *in++;
                    n += 0x13;
                } else {
                    a = n & 0xff;
                    n = (((n >> 8) & 0xf) + 3);
                }
                a |= (a << 8);
                if((o - out) & 1) {
                    if(o >= outl) return(-1);
                    *o++ = a;
                    if(n & 1) {
                        n = ((n - 2) >> 1) + 1;
                        if((o + (n * 2)) > outl) return(-1);
                        while(n--) {
                            *o++ = a;
                            *o++ = a >> 8;
                        }
                    } else if((n - 1) > 1) {
                        x = slz_triace_blah(1, n - 1, a, &o, outl);
                        if(x < 0) return(-1);
                        if(x < n) {
                            if(o >= outl) return(-1);
                            *o++ = a;
                        }
                    } else if(n > 1) {
                        if(o >= outl) return(-1);
                        *o++ = a;
                    }
                } else {
                    x = slz_triace_blah(0, n - 1, a, &o, outl);
                    if(x < 0) return(-1);
                    if(x < n) {
                        if(o >= outl) return(-1);
                        *o++ = a;
                    }
                }
            }
        }
    }
    return(o - out);
}



int slz_triace(unsigned char *in, int insz, unsigned char **ret_out, int outsz, int mode, int *ret_outsz) {
/*----------------------------------------------------------------------------*/
/*--  slz.c - Simple SLZ decompressor                                       --*/
/*--  Copyright (C) 2011 CUE                                                --*/
/*--                                                                        --*/
/*--  This program is free software: you can redistribute it and/or modify  --*/
/*--  it under the terms of the GNU General Public License as published by  --*/
/*--  the Free Software Foundation, either version 3 of the License, or     --*/
/*--  (at your option) any later version.                                   --*/
/*--                                                                        --*/
/*--  This program is distributed in the hope that it will be useful,       --*/
/*--  but WITHOUT ANY WARRANTY; without even the implied warranty of        --*/
/*--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          --*/
/*--  GNU General Public License for more details.                          --*/
/*--                                                                        --*/
/*--  You should have received a copy of the GNU General Public License     --*/
/*--  along with this program. If not, see <http://www.gnu.org/licenses/>.  --*/
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  SLZ header:
  - 0000:3 - signature, always "SLZ"
  - 0003:1 - compression mode: 0/STORE, 1/LZSS, 2/LZSS+RLE, 3/LZSS16
  - 0004:4 - decompressed length (sometimes wrong, do not use)
  - 0008:4 - compressed length
  - 000C:4 - offset to the 2nd file (0 if none), aligned to 4 bytes
------------------------------------------------------------------------------*/

  unsigned   flags = 0, pos = 0, len = 0;
    unsigned char   *out;
    u8      *slz = in;

    not_ret_out_boh(ret_outsz)
    out = *ret_out;

    if((insz >= 0x10) && !memcmp(slz, "SLZ", 3)) {
        mode = slz[3];
        outsz = QUICK_GETi32(slz, 8);
        if(outsz < 0) return(-1);
        myalloc(&out, outsz, ret_outsz);
        *ret_out = out;
        slz += 0x10;
    }

    u8      *raw = out,
            *raw_end = out + outsz;

    if (!mode) {
      while (raw < raw_end) *raw++ = *slz++;
    } else {
      flags = 0;
      while (raw < raw_end) {
        if ((flags >>= 1) <= 0xFFFF) {
          flags = 0x00FF0000 | *slz++;
          if (mode == 3) flags |= 0xFF000000 | (*slz++ << 8);
        }

        if (flags & 1) {
          *raw++ = *slz++;
          if (mode == 3) *raw++ = *slz++;
        } else {
          pos = *slz++;
          len = *slz++;
          if ((mode == 2) && (len >= 0xF0)) {
            if (len > 0xF0) {
              len = (len & 0xF) + 3;
            } else {
              len = pos + 0x13;
              pos = *slz++;
            }
            while (len--) *raw++ = pos;
          } else {
            pos |= (len & 0xF) << 8;
            len = (len >> 4) + 3;
            if (mode == 3) {
              len = (len - 1) << 1;
              pos <<= 1;
            }
            //while (len--) *raw++ = *(raw - pos);
            while (len--) {
                *raw = *(raw - pos);
                raw++;
            }
          }
        }
      }
    }
    return(raw - out);
}



int unlzhl(unsigned char *in, int insz, unsigned char *out, int outsz) {
    LZHL_DHANDLE hnd;
    hnd = LZHLCreateDecompressor();
    if(!LZHLDecompress(hnd, out, &outsz, in, &insz)) outsz = -1;
    LZHLDestroyDecompressor(hnd);
    return(outsz);
}



int unlzrw3(unsigned char *in, int insz, unsigned char *out, int outsz) {
    static u8   *wrk_mem = NULL;
    if(!wrk_mem) {
        wrk_mem = malloc(4096);
        if(!wrk_mem) STD_ERR(QUICKBMS_ERROR_MEMORY);
    }
    lzrw3a_decompress(wrk_mem, in, insz, out, &outsz);
    return(outsz);
}



/*
_DIFFERENTIAL COMPRESSION ALGORITHMS_
by James H. Sylvester
*/
int diffcomp(unsigned char *in, int insz, unsigned char *out, int outsz) {
    unsigned char   *infile   = in,
                    *infilel  = in + insz,
                    *outfile  = out,
                    *outfilel = out + outsz;

  const int blockfactor = 1;              /* adjust as desired */
  const int blocksize = blockfactor * 8;
  char  buffer [257] [8];             /* enter blocksize for second index */

  int   i, j;
  //FILE  *sf, *tf;        /* sourcefile & targetfile respectively */
  int   bestblock = -1;  /* best matching block in buffer */
  int   changeindex;

/* Initialize buffer with exactly same information as in PACK.C program. */
  for (i = 0; i < 256; i++)
    for (j = 0; j < blocksize; j++)
      buffer [i] [j] = i;
/* Reconstruct original data from encoded data in sourcefile. */
  while (1)  /* while true ==> stay in loop until internal exit */
  {
    if (bestblock == -1)     /* input data yet to be loaded */
    {
      bestblock = lame_getc(sf);
      if (bestblock == EOF)  /* original and encoded files had 0 bytes */
      {
        goto quit;
      }
      changeindex = lame_getc(sf);
    }
    else
    {
      bestblock = lame_getc(sf);
      if (bestblock == EOF)    /* input data ended with previous full block */
      {
        for (j = 0; j < blocksize; j++)  /* output full block */
          lame_putc(buffer [256] [j], tf);
        goto quit;
      }
      changeindex = lame_getc(sf);
      if (changeindex == EOF)  /* input data ended with unfull block */
      {
        for (j = 0; j < bestblock; j++)  /* reinterpret bestblock as  */
                                         /* last blocksize and output */
                                         /* this last, partial block  */
          lame_putc(buffer [256] [j], tf);
        goto quit;
      }
      for (j = 0; j < blocksize; j++)  /* output full block */
        lame_putc(buffer [256] [j], tf);
    }
    for (i = 0; i < blockfactor; i++)
    {
      if (i > 0)
        changeindex = lame_getc(sf);
      for (j = i*8; j < i*8+8; j++)
      {
        if (changeindex % 2 == 1)
          buffer [bestblock] [j] = lame_getc(sf);  /* directly load changes */
                                              /* into buffer bestblock */
        changeindex /= 2;
        buffer [256] [j] = buffer [bestblock] [j];  /* copy block info */
      }
    }
  }
quit:
  return(outfile - out);
}



int unlzs(unsigned char *in, int insz, unsigned char *out, int outsz, int big) {
    int     roff = 0,
            rlen = 0,
            ctype;

    ctype = RDP_MPPC_COMPRESSED | RDP_MPPC_FLUSH;
    if(big) ctype |= RDP_MPPC_BIG;
    if(mppc_expand(in, insz, ctype, &roff, &rlen) < 0) return(-1);
    if(rlen < 0) return(-1);
    if(rlen > outsz) rlen = outsz;
    memcpy(out, g_mppc_dict.hist + roff, rlen);
    return(rlen);
}



int moh_lzss(unsigned char *in, int insz, unsigned char *out, int outsz) {
    QUICK_IN_OUT
    unsigned        n,
                    x;
    unsigned char   b,
                    a;

    a = 0;
    b = 0;
    while(o < outl) {
        b <<= 1;
        if(!b) {
            if(in >= inl) break;
            a = *in++;
            b = 1;
        }
        if(a & b) {
            if(in >= inl) break;
            if(o >= outl) return(-1);
            *o++ = *in++;
        } else {
            if((in + 2) > inl) break;
            x = in[0] | (in[1] << 8);
            in += 2;
            n = ((x >> 12) & 0xf) + 3;
            if((o + n) > outl) return(-1);
            x = 0x1000 - (x & 0xfff);   // because it's already negative
            if((o - x) < out) return(-1);
            while(n--) {
                *o = *(o - x);
                o++;
            }
        }
    }
    return(o - out);
}



int moh_rle(unsigned char *in, int insz, unsigned char *out, int outsz) {
    QUICK_IN_OUT
    unsigned char   b,
                    s;

    while(o < outl) {
        if(in >= inl) break;
        s = *in++;
        if(s < 0x80) {
            s++;
            if((in + s) > inl) break;
            if((o + s) > outl) return(-1);
            while(s--) {
                *o++ = *in++;
            }
        } else {
            if(in >= inl) break;
            b = *in++;
            s = 0x101 - s;
            if((o + s) > outl) return(-1);
            while(s--) {
                *o++ = b;
            }
        }
    }
    return(o - out);
}



// by thakis (http://www.amnoid.de/gc/)
// I have not verified if this algorithm is already implemented/known with other names but I guess yes... oh well
int decodeYaz0(u8* src, int srcSize, u8* dst, int uncompressedSize)
{
typedef struct {
  int srcPos, dstPos;
} Ret;
  Ret r = { 0, 0 };
  int i;
  //int srcPlace = 0, dstPlace = 0; //current read/write positions
  
  u32 validBitCount = 0; //number of valid bits left in "code" byte
  u8 currCodeByte = 0;
  while(r.dstPos < uncompressedSize)
  {
    //read new "code" byte if the current one is used up
    if(validBitCount == 0)
    {
      currCodeByte = src[r.srcPos];
      ++r.srcPos;
      validBitCount = 8;
    }
    
    if((currCodeByte & 0x80) != 0)
    {
      //straight copy
      dst[r.dstPos] = src[r.srcPos];
      r.dstPos++;
      r.srcPos++;
      //if(r.srcPos >= srcSize)
      //  return r;
    }
    else
    {
      //RLE part
      u8 byte1 = src[r.srcPos];
      u8 byte2 = src[r.srcPos + 1];
      r.srcPos += 2;
      //if(r.srcPos >= srcSize)
      //  return r;
      
      u32 dist = ((byte1 & 0xF) << 8) | byte2;
      u32 copySource = r.dstPos - (dist + 1);

      u32 numBytes = byte1 >> 4;
      if(numBytes == 0)
      {
        numBytes = src[r.srcPos] + 0x12;
        r.srcPos++;
        //if(r.srcPos >= srcSize)
        //  return r;
      }
      else
        numBytes += 2;

      //copy run
      for(i = 0; i < numBytes; ++i)
      {
        dst[r.dstPos] = dst[copySource];
        copySource++;
        r.dstPos++;
      }
    }
    
    //use next bit from "code" byte
    currCodeByte <<= 1;
    validBitCount-=1;    
  }

  return r.dstPos;
}



int byte2hex(u8 *input, int len, u8 *output, int outlen) {
    static const u8 hex[] = "0123456789ABCDEF";
    int     i;
    u8      *o,
            *l;

    if(len < 0) len = strlen(input);
    o = output;
    l = output + outlen;
    for(i = 0; i < len; i++) {
        if((o + 2 + 1) > l) break;
        *o++ = hex[input[i] >> 4];
        *o++ = hex[input[i] & 15];
    }
    *o = 0;
    return(o - output);
}



// code from Geoffrey W. Curtis
int undragonballz(u8 *in, int insz, u8 *out) {
    int     i;
    u8      *src,
            *src2,
            *end,
            *dst,
            v0,
            v1,
            v2,
            v3,
            v4;

    dst = out;
    src = in;
    end = src + insz;
    while (src < end)
    {
        v0 = *src++;
        
        v1 = (v0 >> 1) & 0x7F;
        v2 = (v1 >> 2);
        v3 = (v1 & 3);
        
        if (v0 & 1)
        {
            v4 = *src++;
            
            src2 = dst - v4;
        }
        else
        {
            src2 = src;
        }
        
        for (i = 0; i < v2; i++)
        {
            dst[0] = src2[0];
            dst[1] = src2[1];
            dst[2] = src2[2];
            dst[3] = src2[3];
            
            src2 += 4;
            dst += 4;
        }
        
        for (i = 0; i < v3; i++)
        {
            dst[0] = src2[0];
            
            ++src2;
            ++dst;
        }
        
        if (!(v0 & 1))
        {
            src = src2;
        }
    }
    return(dst - out);
}



int CRLE_Decode(u8 *target, int tlen, u8 *source, int slen)
{
	int i, s, t;
	u8 escape;
	s = t = 0;

	//tlen = ((DWORD*)source)[0];
	//s += sizeof(DWORD);
	escape = source[s];
	s++;

	//while (t < tlen)
    while((t < tlen) && (s < slen))
	{
		if (source[s] == escape)
		{
			for (i = 0; i < source[s+1]; i++)
				target[t++] = source[s+2];
			s += 3;
		}
		while((source[s] != escape) && (t < tlen))
			target[t++] = source[s++];
	}

	return t;
}



// ftp://ftp.elf.stuba.sk/pub/pc/pack/mar.rar
int MAR_RLE(unsigned char *HufBlock, int HufBlockSize, unsigned char *L, int N) {

   int I, BlockPos, Len, Inc, Code;

   BlockPos = 0;
   Len      = 0;
   Inc      = 1;

   for (I = 0; I < HufBlockSize; I++) {
      Code = HufBlock[I];
      if (Code == 0) {
         Len += Inc;
         Inc <<= 1;
      } else if (Code == 1) {
         Inc <<= 1;
         Len += Inc;
      } else {
         for (; Len > 0; Len--) L[BlockPos++] = 0;
         Len = 0;
         Inc = 1;
         L[BlockPos++] = Code - 1;
      }
   }

   for (; Len > 0; Len--) L[BlockPos++] = 0;

   //if (BlockPos != N) FatalError("BlockPos (%d) != N (%d) in DecodeRLE()",BlockPos,N);
    return(BlockPos);
}



// originally from http://gdcm.sourceforge.net
int gdcm_rle(unsigned char *in, int insz, unsigned char *out, int outsz) {
    int     numOutBytes = 0,
            numberOfReadBytes = 0;
    signed char byte;
        char nextByte;

    while( numberOfReadBytes < insz )
      {
      byte = in[numberOfReadBytes];
      numberOfReadBytes++;
      if( byte >= 0 /*&& byte <= 127*/ ) /* 2nd is always true */
        {
        memcpy( out + numOutBytes, in + numberOfReadBytes, byte+1 );
        numberOfReadBytes += byte+1;
        numOutBytes += byte+ 1;
        }
      else if( byte <= -1 && byte >= -127 )
        {
        nextByte = in[numberOfReadBytes];
        numberOfReadBytes++;
        memset(out + numOutBytes, nextByte, -byte + 1);
        numOutBytes += -byte + 1;
        }
      else /* byte == -128 */
        {
        //assert( byte == -128 );
        //out[numOutBytes++] = byte;  // added by me (Luigi) because I guess it's something missing
        }
        if(numOutBytes >= outsz) break;
        //assert( is.eof()
        //|| numberOfReadBytes + frame.Header.Offset[i] - is.tellg() == 0);
      //std::cerr << "numOutBytes: " << numOutBytes << " / " << length << "\n";
      //std::cerr << "numberOfReadBytes: " << numberOfReadBytes << "\n";
      }
    //assert( numOutBytes == length );

    return(numOutBytes);
}



enum { LZP_H_BITS=17, LZP_H_SIZE=1 << LZP_H_BITS, LZP_MATCH_FLAG=0xB5 };
#define LZP_ROR(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#define lzpC(X) (*(u32*)((X)-4))
static inline u32 lzpH(u32 c,u8* p) {
//    return (c+11*(c >> 15)+13*lzpC(p-1)) & (LZP_H_SIZE-1);
    return (c+5*LZP_ROR(c,17)+3*lzpC(p-1)) & (LZP_H_SIZE-1);
}
#define LZP_INIT(Pattern)                                                   \
    u32 i, k, n1=1, n=1;                                                   \
    u8* p, * InEnd=In+Size, * OutStart=Out, * HTable[LZP_H_SIZE];             \
    for (i=0;i < LZP_H_SIZE;i++)                HTable[i]=Pattern+5;            \
    lzpC(Out+4)=lzpC(In+4);                 lzpC(Out+8)=lzpC(In+8);         \
    i=lzpC(Out += 12)=lzpC(In += 12);       k=lzpH(i,Out);
int LZPDecode(u8* In,u32 Size,u8* Out,int MinLen)
{
    LZP_INIT(Out);
    do {
        p=HTable[k];
        if ( !--n )  { HTable[k]=Out;       n=n1; }
        if (*In++ != LZP_MATCH_FLAG || i != lzpC(p) || *--InEnd == 255)
                *Out++ = In[-1];
        else {
            HTable[k]=Out;                  n1 += (Out-p > (n1+1)*LZP_H_SIZE && n1 < 7);
            for (i=MinLen-1;*InEnd == 0;InEnd--)
                    i += 254;
            i += *InEnd;                    k=2*n1+2;
            do {
                if ( !--k ) { k=2*n1+1;     HTable[lzpH(lzpC(Out),Out)]=Out; }
                *Out++ = *p++;
            } while ( --i );
        }
        k=lzpH(i=lzpC(Out),Out);
    } while (In < InEnd);
    return (Out-OutStart);
}



int unpackbits(u8 *in, int insz, u8 *out) {
    int     i,
            j,
            o,
            count;

    o = 0;
    for(i = 0; i < insz;) {
        count = in[i++];
        if(count >= 128) {
            count = 256 - count;
            for(j = 0; j < (count + 1); j++) out[o++] = in[i];
            i++;
        } else {
            for(j = 0; j < (count + 1); j++) out[o++] = in[i++];
        }
    }
    return(o);
}



// from Eduke32
#if defined(__POWERPC__)
static uint32_t eduke32_LSWAPIB(uint32_t a) { return(((a>>8)&0xff00)+((a&0xff00)<<8)+(a<<24)+(a>>24)); }
static uint16_t eduke32_SSWAPIB(uint16_t a) { return((a>>8)+(a<<8)); }
#else
#define eduke32_LSWAPIB(a) (a)
#define eduke32_SSWAPIB(a) (a)
#endif
int32_t eduke32_lzwuncompress(char *compbuf, int32_t compleng, char *ucompbuf, int32_t ucompleng)
{
    int32_t i, dat, leng, bitcnt, *lptr, numnodes, totnodes, nbits, oneupnbits, hmask, *prefix;
    char ch, *ucptr, *suffix;
    int32_t ucomp = (int32_t)ucompbuf;

    if (compleng >= ucompleng) { memcpy(ucompbuf,compbuf,ucompleng); return ucompleng; }

    totnodes = eduke32_LSWAPIB(((int32_t *)compbuf)[0]); if (totnodes <= 0 || totnodes >= ucompleng+256) return 0;

    prefix = (int32_t *)malloc(totnodes*sizeof(int32_t)); if (!prefix) return 0;
    suffix = (char *)malloc(totnodes*sizeof(uint8_t)); if (!suffix) { free(prefix); return 0; }

    numnodes = 256; bitcnt = (4<<3); nbits = 8; oneupnbits = (1<<8); hmask = ((oneupnbits>>1)-1);
    do
    {
        lptr = (int32_t *)&compbuf[bitcnt>>3]; dat = ((eduke32_LSWAPIB(lptr[0])>>(bitcnt&7))&(oneupnbits-1));
        bitcnt += nbits; if ((dat&hmask) > ((numnodes-1)&hmask)) { dat &= hmask; bitcnt--; }

        prefix[numnodes] = dat;

        ucompbuf++;
        for (leng=0; dat>=256; dat=prefix[dat])
        {
            if ((int32_t)ucompbuf+leng-ucomp > ucompleng) goto bail;
            ucompbuf[leng++] = suffix[dat];
        }

        ucptr = &ucompbuf[leng-1];
        for (i=(leng>>1)-1; i>=0; i--) { ch = ucompbuf[i]; ucompbuf[i] = ucptr[-i]; ucptr[-i] = ch; }
        ucompbuf[-1] = dat; ucompbuf += leng;

        suffix[numnodes-1] = suffix[numnodes] = dat;

        numnodes++; if (numnodes > oneupnbits) { nbits++; oneupnbits <<= 1; hmask = ((oneupnbits>>1)-1); }
    }
    while (numnodes < totnodes);

bail:
    free(suffix); free(prefix);
    return (int32_t)ucompbuf-ucomp;
}




// 
/**
 * Decompress a block of RLE encoded memory.
 */
long xu4_rleDecompress(unsigned char *indata, long inlen, unsigned char *outdata, long outlen) {
    int i;
    unsigned char *p, *q;
    unsigned char ch, count, val;

    p = indata;
    q = outdata;
    while ((p - indata) < inlen) {
        ch = *p++;
        if (ch == 02) { // RLE_RUNSTART
            count = *p++;
            val = *p++;
            for (i = 0; i < count; i++) {
                *q++ = val;
                if ((q - outdata) >= outlen)
                    break;
            }
        } else {
            *q++ = ch;
            if ((q - outdata) >= outlen)
                break;
        }
    }

    return q - outdata;
}



// http://www.lemurproject.org
/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
int RVLCompress_decompress_ints (unsigned char *data_ptr,
                                                  int *out_ptr,
                                                  int num_bytes)
{

  unsigned char *data_end_ptr = data_ptr + num_bytes;
  unsigned char *data_curr_ptr;
  int *out_ptr_end = out_ptr;

  for (data_curr_ptr=data_ptr; data_curr_ptr<data_end_ptr; out_ptr_end++) {
    if (*data_curr_ptr & 128) {
      *out_ptr_end = 127 & *data_curr_ptr;
      data_curr_ptr ++;
    } else if (*(data_curr_ptr+1) & 128) {
      *out_ptr_end = *data_curr_ptr |
        ((*(data_curr_ptr + 1) & 127) << 7);
      data_curr_ptr += 2;
    } else if (*(data_curr_ptr+2) & 128) {
      *out_ptr_end = *data_curr_ptr |
        (*(data_curr_ptr + 1) << 7) |
        ((*(data_curr_ptr + 2) & 127) << 14);
      data_curr_ptr += 3;
    } else if (*(data_curr_ptr+3) & 128) {
      *out_ptr_end = *data_curr_ptr |
        (*(data_curr_ptr + 1) << 7) |
        (*(data_curr_ptr + 2) << 14) |
        ((*(data_curr_ptr + 3) & 127) << 21);
      data_curr_ptr += 4;
    } else {
      *out_ptr_end = *data_curr_ptr |
        (*(data_curr_ptr + 1) << 7) |
        (*(data_curr_ptr + 2) << 14) |
        (*(data_curr_ptr + 3) << 21) |
        ((*(data_curr_ptr + 4) & 127) << 28);
      data_curr_ptr += 5;
    }
  } // for
  
  return (out_ptr_end - out_ptr);
}



// http://code.google.com/p/pdb2txt/
size_t	pdb_decompress(u8 *source,size_t srclen,u8 *dest,size_t destlen)
{
  u8	    *se=source+srclen;
  u8	    *de=dest+destlen;
  u8	    *dd=dest;

  while (source<se && dest<de) {
    size_t c=*source++;
    if (c>=1 && c<=8) { // copy
      while (c-- && source<se && dest<de)
	*dest++=*source++;
    } else if (c<=0x7f) // this char
      *dest++=(u8)c;
    else if (c>=0xc0) { // space + c&0x7f
      *dest++=' ';
      if (dest<de)
	*dest++=(u8)c&0x7f;
    } else if (source<se) { // copy from decoded buf
      c=(c<<8)|*source++;
      int k=(c&0x3fff)>>3;
      c=3+(c&7);
      if (dest-k<dd || dest+c>de) // invalid buffer
	break;
      while (c-- && dest<de) {
	*dest=dest[-k];
	++dest;
      }
    }
  }
  return dest-dd;
}



int rtldecompress(int type, u8 *in, int insz, u8 *out, int outsz) {
#define RTL_COMPRESSION_FORMAT_LZNT1         (0x0002)   // winnt
#define RTL_COMPRESSION_FORMAT_XPRESS        (0x0003)   // added in Windows 8
#define RTL_COMPRESSION_FORMAT_XPRESS_HUFF   (0x0004)   // added in Windows 8
#ifdef WIN32
    static HMODULE hlib = NULL;
    static DWORD WINAPI (*RtlDecompressBuffer)(
      /*IN*/    USHORT  CompressionFormat,
      /*OUT*/   PUCHAR  UncompressedBuffer,
      /*IN*/    ULONG   UncompressedBufferSize,
      /*IN*/    PUCHAR  CompressedBuffer,
      /*IN*/    ULONG   CompressedBufferSize,
      /*OUT*/   PULONG  FinalUncompressedSize
    ) = NULL;

    ULONG   ret;

    if(!RtlDecompressBuffer) {
        hlib = LOADDLL("ntdll.dll");
        if(!hlib) hlib = LOADDLL("ntdll-8-32.dll");
        if(!hlib) return(-1);
        RtlDecompressBuffer = GETFUNC("RtlDecompressBuffer");
        if(!RtlDecompressBuffer) return(-1);
    }
    if(RtlDecompressBuffer(
        type,
        out,
        outsz,
        in,
        insz,
        &ret) != 0) return(-1); // STATUS_SUCCESS is 0
    return(ret);
#else
    return(-1);
#endif
}



// code by MrAdults (Se駉r Casaroja's Noesis)
// http://forum.xentax.com/viewtopic.php?p=52279#p52279
int Model_GMI_Decompress(unsigned char *src, int srcLen, unsigned char *dst, int dstLen)
{
   int srcPtr = 0;
   int dstPtr = 0;
   int i;
   int j;

   while (srcPtr < srcLen && dstPtr < dstLen)
   {
      unsigned char ctrl = src[srcPtr++];
      for (i = 0; i < 8 && srcPtr < srcLen; i++)
      {
         if (ctrl & (1<<i))
         { //literal
            dst[dstPtr++] = src[srcPtr++];
         }
         else
         { //ofs+len
            short ol = *(short *)(src+srcPtr);
            srcPtr += sizeof(short);
            int len = 3 + ((ol>>8) & 15);
            int relOfs = (ol & 255) | ((ol>>12) << 8);
            int ofs = dstPtr - ((dstPtr-18-relOfs) & 4095);
            for (j = 0; j < len; j++)
            {
               if (ofs+j < 0 || ofs+j >= dstPtr)
               {
                  dst[dstPtr++] = 0;
               }
               else
               {
                  dst[dstPtr++] = dst[ofs+j];
               }
            }
         }
      }
   }
   return dstPtr;
}



/*
http://pastebin.com/186Amx8T
*/

int dewolf(unsigned char * src, int srclen, unsigned char * dest, int destlen) {
    unsigned char m = src[8];
    int ps = 9, pd = 0;
    while(ps < srclen && pd < destlen)
    {
        if(ps>=0x422)
            ps=ps;
        if(src[ps] == m)
        {
            ps++;
            if(src[ps] == m)
                dest[pd++] = src[ps++];
            else
            {
                if(src[ps] >= m)
                    src[ps]--;
                int pos = 0, len = (src[ps] >> 3) + 4;
                unsigned char type1 = src[ps++] & 7;
                unsigned char type2 = type1 >> 2;
                type1 &= 3;
                if(type2)
                    len += src[ps++] << 5;
 
                if(type1 == 0)
                    pos = src[ps++] + 1;
                else if(type1 == 1)
                {
                    pos = src[ps] + (src[ps + 1] << 8) + 1;
                    ps += 2;
                }
                else if(type1 == 2)
                {
                    pos = src[ps] + (src[ps + 1] << 8) + (src[ps + 2] << 16) + 1;
                    ps += 3;
                }
                else
                    type1 = type1;
 
                int k;
                for(k = 0; k < len; ++k)
                    dest[pd + k] = dest[pd - pos + k];
                pd += len;
            }
        }
        else
        {
            dest[pd++] = src[ps++];
        }
    }
    ps = 0;
    return(pd);
}



// code written by Ekey (h4x0r) of http://www.progamercity.net
// http://forum.xentax.com/viewtopic.php?p=80129&sid=d49d1cc543b79b937272aaff036fb046#p80129
unsigned __cdecl CO_Decompress(unsigned char *szOutBuf, unsigned szUncompressedSize, unsigned char *szInBuf)
{
  unsigned result;
  unsigned char *v4;
  int v5;
  char v6;
  char v7;
  int v8;
  int16_t v9;
  int v10;
  char v11;
  int v12;
  char v13;
  signed int v14;
  int16_t v15;
  int v16;
  char v17[4096];
  //int v19;
  unsigned char *v20;

  v4 = szInBuf;
  v5 = 0;
  memset(v17, 0, 0x1000u);
  result = 0;
LABEL_2:
  v6 = *(u8 *)v4;
  v15 = *(u8 *)v4++;
  v16 = 0;
  while ( v6 >= 0 )
  {
    v13 = *(u8 *)v4;
    if ( szUncompressedSize > result )
    {
      *(u8 *)(szOutBuf + result++) = v13;
      v17[(int16_t)v5] = v13;
      v5 = (v5 + 1) & 0xFFF;
      v14 = 1;
    }
    else
    {
      v14 = 0;
    }
    ++v4;
    if ( !v14 )
      return 0;
LABEL_13:
    v6 = 2 * v15;
    v15 *= 2;
    ++v16;
    if ( v16 == 8 )
      goto LABEL_2;
  }
  v7 = *(u8 *)v4;
  if ( !*(u8 *)v4 )
    return result;
  v9 = 16 * (256 - (u8)(v7 & 0xF0)) - *(u8 *)(v4 + 1);
  v10 = (v7 & 0xF) + 2;
  v8 = (u16)v10;
  v4 += 2;
  v20 = v4;
  if ( !(u16)v10 )
    goto LABEL_13;
  while ( 1 )
  {
    v12 = (int16_t)v5;
    v11 = v17[((int16_t)v5 + v9) & 0xFFF];
    if ( szUncompressedSize <= result )
      return 0;
    *(u8 *)(szOutBuf + result) = v11;
    --v8;
    ++result;
    v5 = (v5 + 1) & 0xFFF;
    v17[v12] = v11;
    if ( !(u16)v8 )
    {
      v4 = v20;
      goto LABEL_13;
    }
  }
  return result;
}



int unlzlib(u8 *in, int insz, u8 *out, int outsz) {
    int     i,
            o,
            len;

    // this method is terrible, I have found no examples
    // for doing a simple input->output job... really boring
    // note that I have not tried this thing
    struct LZ_Decoder  *lz;
    lz = LZ_decompress_open();
    if(!lz) return(-1);
    i = 0;
    o = 0;
    for(;;) {
        if(i < insz) {
            LZ_decompress_write(lz, in + i, 1);
            i++;
            if(i >= insz) LZ_decompress_finish(lz);
        } else {
            if(LZ_decompress_finished(lz)) break;
        }
        if(!LZ_decompress_finished(lz)) {
            if(o >= outsz) {
                o = -1;
                break;
            }
            len = LZ_decompress_read(lz, out + o, 1);
            if(len < 0) {
                o += len;
                break;
            }
            o += len;
        }
    }
    LZ_decompress_close(lz);
    return(o);
}



// PSP_Nanoha
int lzs_unzip(u8 *datapart, int Size_DataPart, u8 *dictionarypart, int dictionarypart_Length, u8 *b_unzip, int size_unzip) {
    const int THRESHOLD = 2;
    int i, f;
                int i_out = 0, i_data = 0, i_dic = 0; //下指标
                byte c;
                UInt16 flags; //标志，二进制的16位从右到左表示接下来的16组数据是否为压缩格式(1不压缩，0压缩)

                flags = 0;
                for (; ; )
                {
                    if (i_data == Size_DataPart)  //首先,读入两字节到flags
                        break;
                    flags = datapart[i_data++];  //先读入的在低8位
                    if (i_data == Size_DataPart)
                        break;
                    flags |= (UInt16)(datapart[i_data++] << 8); //后读入的在高8位

                    for ( f = 0; f < 16; f++) {  //进行16次读取数据的操作
                        if (((flags >> f) & 1) != 0)   //flag的右起第f位为1,表示接下来的是非压缩数据
                        {
                            if (i_dic == dictionarypart_Length)
                                break;
                            c = dictionarypart[i_dic++];
                            if(i_out >= size_unzip) return -1;
                            b_unzip[i_out++] = c;  //那么直接从字典段读取一字节到 输出数组
                        }
                        else     //flag的右起第f位为0,表示接下来的是压缩数据
                        {
                            if (i_data == Size_DataPart)
                                break;
                            UInt16 t = datapart[i_data++];
                            if (i_data == Size_DataPart)
                                break;
                            t |= (UInt16)(datapart[i_data++] << 8);  //读取两字节到t(跟flags一样,先读入的在低8位)

                            int size = (int)(t & 0x1F) + THRESHOLD;  //t的后5位 + THRESHOLD 为长度
                            int offset = (int)(t >> 5);  //t的前11为为距离 

                            for ( i = 0; i < size; i++, i_out++) {  //从 输出数组的 当前位置 - offset 开始,读取size个字节到输出数组
                                if(i_out >= size_unzip) return -1;
                                b_unzip[i_out] = b_unzip[i_out - offset]; //此过程必须一个字节一个字节的进行
                            }

                        } //循环结束的条件为 data段读完后再次收到读取data段的请求
                    }
                }
    return i_out;
}



// by CUE 2009
// http://www.romhacking.net/utilities/920/
int legend_of_mana(unsigned char *pak_buffer, int pak_length, unsigned char *raw_buffer, int raw_length) {
  unsigned char   *pak, *raw;
  unsigned char   code;
  unsigned char   x, y, z;
    signed char   i; // must be signed to extend the sign in the FB code
  unsigned short  n, p;

  //if (*pak_buffer == 0x01 /*MAGIC*/) pak = pak_buffer + 1;
  //else 
    pak = pak_buffer;
  raw = raw_buffer;

  do {
    /*
    // not enough memory?
    if (raw + MEMORY_FREE > raw_buffer + raw_length) {
      length = raw - raw_buffer;
      raw_length += MEGABYTE;
      raw_buffer = ReAssign(raw_buffer, raw_length, sizeof(char));
      raw = raw_buffer + length;
    }
    */

    // chunk
    switch (code = *pak++) {
      case 0xF0: // F0+XN: put (N+3) times {X}
        n = (*pak & 0xF) + 3;
        x = *pak++ >> 4;
        while (n--) {
          *raw++ = x;
        }
        break;

      case 0xF1: // F1+N+X: put (N+4) times {X}
        n = *pak++ + 4;
        x = *pak++;
        while (n--) {
          *raw++ = x;
        }
        break;

      case 0xF2: // F2+N+YX: put (N+2) times {X,Y}
        n = *pak++ + 2;
        x = *pak & 0xF;
        y = *pak++ >> 4;
        while (n--) {
          *raw++ = x;
          *raw++ = y;
        }
        break;

      case 0xF3: // F3+N+X+Y: put (N+2) times {X,Y}
        n = *pak++ + 2;
        x = *pak++;
        y = *pak++;
        while (n--) {
          *raw++ = x;
          *raw++ = y;
        }
        break;

      case 0xF4: // F4+N+X+Y+Z: put (N+2) times {X,Y,Z}
        n = *pak++ + 2;
        x = *pak++;
        y = *pak++;
        z = *pak++;
        while (n--) {
          *raw++ = x;
          *raw++ = y;
          *raw++ = z;
        }
        break;

      case 0xF5: // F5+N+X+{list}: put (N+4) times {X,byte}
        n = *pak++ + 4;
        x = *pak++;
        while (n--) {
          *raw++ = x;
          *raw++ = *pak++;
        }
        break;

      case 0xF6: // F6+N+X+Y+{list}: put (N+3) times {X,Y,byte}
        n = *pak++ + 3;
        x = *pak++;
        y = *pak++;
        while (n--) {
          *raw++ = x;
          *raw++ = y;
          *raw++ = *pak++;
        }
        break;

      case 0xF7: // F7+N+X+Y+Z+{list}: put (N+2) times {X,Y,Z,byte}
        n = *pak++ + 2;
        x = *pak++;
        y = *pak++;
        z = *pak++;
        while (n--) {
          *raw++ = x;
          *raw++ = y;
          *raw++ = z;
          *raw++ = *pak++;
        }
        break;

      case 0xF8: // F8+N+X: put from {X} to {X+(N+3)}
        n = *pak++ + 4;
        x = *pak++;
        while (n--) {
          *raw++ = x++;
        }
        break;

      case 0xF9: // F9+N+X: put from {X} to {X-(N+3)}
        n = *pak++ + 4;
        x = *pak++;
        while (n--) {
          *raw++ = x--;
        }
        break;

      case 0xFA: // FA+N+X+I: put from {X} to {X+(N+4)*I)}
        n = *pak++ + 5;
        x = *pak++;
        i = *pak++;
        while (n--) {
          *raw++ = x;
          x += i;
        }
        break;

      case 0xFB: // FB+N+X+Y+I: put from {YX} to {YX+(N+2)*I}
        n = *pak++ + 3;
        p = *(short *)pak; pak += 2;
        i = *pak++; // 'i' must be signed to extend the sign
        while (n--) {
          *(short *)raw = p; raw += 2;
          p += i;
        }
        break;

      case 0xFC: // FC+XY+NZ: put (N+4) bytes from '$-(ZXY+1)'
        p = (*(unsigned short *)pak++ & 0xFFF) + 1;
        n = (*pak++ >> 4) + 4;
        while (n--) {
          *raw = *(raw - p); raw++;
        }
        break;

      case 0xFD: // FD+X+N: put (N+20) bytes from '$-(X+1)'
        p = *pak++ + 1;
        n = *pak++ + 20;
        while (n--) {
          *raw = *(raw - p); raw++;
        }
        break;

      case 0xFE: // FE+XN: put (N+3) bytes from '$-8*(X+1)'
        p = ((*pak >> 4) + 1) << 3;
        n = (*pak++ & 0xF) + 3;
        while (n--) {
          *raw = *(raw - p); raw++;
        }
        break;

      case 0xFF: // FF: end of compressed data
        break;

      default: // N+{list}: put (N+1) times {byte}
        n = code + 1;
        while (n--) {
          *raw++ = *pak++;
        }
        break;
    }
  } while ((code != 0xFF) && (pak - pak_buffer < pak_length));

  //if (code != 0xFF) printf("WARNING: No end code found!\n\n");
  //if (pak - pak_buffer != pak_length) printf("WARNING: Bad coded length!\n\n");

  return raw - raw_buffer;
}




// by ffgriever
// http://www.romhacking.net/utilities/533/
int dizzy(u8 *in, int count, u8 *out) {
    u8 *inl = in + count;
    u8 *bck = out;
	char tile16dec[16], tile16[32];
    u32   adr_cnt;
    u32   cmprline_len, unc_bytes, run_bytes;
    u32 i;
		for ( i = 0; i < count; i++)
		{
            if(in >= inl) break;
			memset(tile16, 0, 32);
			memset(tile16dec, 0, 32);
            tile16[0] = *in++;
			unc_bytes = (((tile16[0]>>4) & 0x0F) + 1);
			run_bytes = (tile16[0] & 0x0F);
			if (((run_bytes + unc_bytes) > 16))
			{
				//printf("Mamy problem: dlugosc linii wieksza od 16!\nNumer linii: %d, adres: 0x%08x\n", i+1, adr_cnt);
				//break;
                return -1;
			}
			//printf("tile16[0] = 0x%02x\n", tile16[0]);
			cmprline_len = 17 - run_bytes;
			//printf("Line Len: %d, address: 0x%08x\n", cmprline_len, adr_cnt);
			tile16[1] = *in++;
			adr_cnt += cmprline_len;

			memcpy(tile16dec, &tile16[1], unc_bytes);
			memset(&tile16dec[unc_bytes], tile16[unc_bytes], run_bytes);
			memcpy(&tile16dec[unc_bytes+run_bytes], &tile16[unc_bytes+1], 16-unc_bytes-run_bytes);

            memcpy(out, tile16, cmprline_len);  //fwrite(tile16dec, 16, 1, fdout);
            out += cmprline_len;
		}
    return out - bck;
}



// c/o Noah 'Zoinkity' Granath (nefariousdogooder@yahoo.com) Oct. 2008
// RomHacking
unsigned long EDL_byteswap(unsigned long w)
{return (w >> 24) | ((w >> 8) & 0x0000ff00) | ((w << 8) & 0x00ff0000) | (w << 24);
}   
unsigned long EDL_helper(unsigned long long int *data,unsigned long bitcount,unsigned char *in,unsigned long *pos,unsigned long max,int endian)
{unsigned long x,y,z;

if(bitcount>32) return bitcount;   /*essentially, do nothing!*/
z=*data;
y=0;
x=max-*pos;
if(x>4) x=4;   /*#bytes to retrieve from file*/
//fseek(in,*pos,SEEK_SET);
//fread(&y,x,1,in);
memcpy(&y, in + (*pos), x);
if(endian) y=EDL_byteswap(y);
*pos+=x;

*data=y;       /*tack old data on the end of new data for a continuous bitstream*/
*data=*data<<bitcount;
*data|=z;

x*=8;          /*revise bitcount with number of bits retrieved*/
return bitcount+x;}

/*generate tables*/
int EDL_FillBuffer(unsigned short *large,unsigned char *what,long total,long num,char bufsize)
{unsigned char *buf;
unsigned short *when,*samp;
unsigned long *number;
long x,y,z,back;

       /*my implementation is stupid and alays copies the block, so this uses even more memory than it should
       if(!(what=realloc(what,num))
           {printf("\nVirtual memory exhausted.\nCan not continue.\n\tPress ENTER to quit.");
           getchar();
           return 0;
           }*/
       if(!(when=calloc(num,2)))
         {printf("\nVirtual memory exhausted.\nCan not continue.\n\tPress ENTER to quit.");
         getchar();
         return 0;
         }/*end calloc*/
       if(!(samp=calloc(num,2)))
         {printf("\nVirtual memory exhausted.\nCan not continue.\n\tPress ENTER to quit.");
         getchar();
         return 0;
         }/*end calloc*/
       if(!(number=calloc(16,4)))
         {printf("\nVirtual memory exhausted.\nCan not continue.\n\tPress ENTER to quit.");
         getchar();
         return 0;
         }/*end calloc*/
       memset(large,0,0xC00);         /*both buffers have 0x600 entries each*/

       /*build an occurance table*/
       back=0;  /*back will act as a counter here*/
       for(y=1;y<16;y++)/*sort occurance*/
          {for(x=0;x<total;x++)/*peek at list*/
                {if(what[x]==y)
                   {when[back]=x;
                   back++;
                   number[y]++;
                   }
                }/*end peek*/
          }/*end occurance*/

       x=0;
       for(y=1;y<16;y++)/*sort nibbles*/
          {for(z=number[y];z>0;z--) 
              {what[x]=y; x++;}
          }/*end sort*/
       free(number);

       /*generate bitsample table*/
       z=what[0];           /*first sample, so counting goes right*/
       back=0;              /*back will act as the increment counter*/
       for(x=0;x<num;x++)
          {y=what[x];
          if(y!=z) {z=y-z; back*=(1<<z); z=y;}
          y=(1<<y)|back;
          back++;
          do{samp[x]=samp[x]<<1;
             samp[x]+=(y&1);
             y=y>>1;
             }while(y!=1);
          }/*end bitsample table*/

       if(!(buf=calloc(1<<bufsize,1)))
         {printf("\nVirtual memory exhausted.\nCan not continue.\n\tPress ENTER to quit.");
         getchar();
         return 0;
         }/*end calloc      80013918*/
       
       for(x=0;x<num;x++)  /*fill buffer    8001392C*/
          {back=what[x];      /*#bits in sample*/
          if(back<bufsize)         /*normal entries*/
            {y=1<<back;
             z=samp[x];       /*offset within buffer*/
            do{
              large[z]=(when[x]<<7) + what[x];
              z+=y;
              }while(!(z>>bufsize));
            }/*end normal*/
          else
            {y=(1<<bufsize)-1; /*this corrects bitmask for buffer entries*/
            z=samp[x]&y;
            buf[z]=what[x];
            }/*end copies*/
          }/*end fill*/
       
       /*read coded types > bufsize    80013AA8*/
       z=0;      /*value*/
       for(x=0;!(x>>bufsize);x++)/*read buf*/
          {y=buf[x];
          if(y)
            {y-=bufsize;
            if(y>8) return -8;
            back=(z<<7) + (y<<4);  /*value*0x80 + bits<<4*/
            large[x]=back;
            z+=(1<<y);
            }/*end if(y)*/
          }/*end buf reading*/
       free(buf);
       if(z>0x1FF) return -9;

       /*do something tricky with the special entries    80013B3C*/
       back=1<<bufsize;
       for(x=0;x<num;x++)
          {if(what[x]<bufsize) continue;
          z=samp[x] & (back-1);
          z=large[z];     /*in dASM, this is labelled 'short'*/
          y=samp[x]>>bufsize;
          /*80013BEC*/
          do{large[y+(z>>7)+(1<<bufsize)]=(when[x]<<7)+what[x];
             y=y+(1<<(what[x]-bufsize));
             }while((y>>((z>>4)&7))==0);
          }
       free(when);
       free(samp);

return 0;}


/*cool bitwise table type*/
unsigned long EDLdec1(unsigned long pos,unsigned char *in,unsigned char *out,unsigned long size,unsigned long max,int endian)
{unsigned char bits[9];  /*what=p->list of slots*/
long x,y,z,stack=0;
unsigned long count=0,num,back;  /*count=#bits in register, num=#to copy, back=#to backtrack*/
unsigned short small[0x600],large[0x600];   /*when=p->occurance in list*/
unsigned char  table1[]={0,1,2,3,4,5,6,7,8,0xA,0xC,0xE,0x10,0x14,0x18,0x1C,0x20,0x28,0x30,0x38,0x40,0x50,0x60,0x70,0x80,0xA0,0xC0,0xE0,0xFF,0,0,0};
unsigned char  table2[]={0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0,0};
unsigned short table3[]={0,1,2,3,4,6,8,0xC,0x10,0x18,0x20,0x30,0x40,0x60,0x80,0xC0,0x100,0x180,0x200,0x300,0x400,0x600,0x800,0xC00,0x1000,0x1800,0x2000,0x3000,0x4000,0x6000};
unsigned char  table4[]={0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,0xA,0xA,0xB,0xB,0xC,0xC,0xD,0xD,0,0};
unsigned char what[0x400];
unsigned long long data=0;       /*64bit datatable container*/

int ftell_out = 0;
size+=pos;

for(/*pos+=12*/;pos<=size;back=0)
   {memset(bits,0,8); /*clear bits between... stuff*/
   count=EDL_helper(&data,count,in,&pos,size,endian);
   x=data&1; data=data>>1; count--;
   
   if(x)     /*mode 1 - tables*/
     {count=EDL_helper(&data,count,in,&pos,size,endian);/*build large table*/
     x=data&0x1FF; data=data>>9; count-=9;
     //if(flagrant.message) printf("\nmode1\tpos: %X\tout: %X\tdata: %X",pos,ftell_out,data);
     if(x) /*construct tables*/
       {
       memset(what,0,0x400);
       num=0;    /*true # entries, since 0 entries are not counted!*/
       for(y=0;y<x;y++)/*fill table with nibbles*/
          {count=EDL_helper(&data,count,in,&pos,size,endian);
          back=data&1; data=data>>1; count--;
          if(back)/*grab nibble*/
            {count=EDL_helper(&data,count,in,&pos,size,endian);
            stack=data&0xF; data=data>>4; count-=4;
            }/*end grab*/
          what[y]=stack;
          if(stack) num++;   /*count nonzero entries*/
          }/*end fill*/
       x=EDL_FillBuffer(large,what,x,num,10);
       }/*end construction*/
     if(x<0) {if(x < 0) return -1;
             if(x) return x;}

     count=EDL_helper(&data,count,in,&pos,size,endian);/*build smaller table*/
     x=data&0x1FF; data=data>>9; count-=9;
     if(x) /*construct tables*/
       {
       memset(what,0,0x400);
       num=0;    /*true # entries, since 0 entries are not counted!*/
       for(y=0;y<x;y++)/*fill table with nibbles*/
          {count=EDL_helper(&data,count,in,&pos,size,endian);
          back=data&1; data=data>>1; count--;
          if(back)/*grab nibble*/
            {count=EDL_helper(&data,count,in,&pos,size,endian);
            stack=data&0xF; data=data>>4; count-=4;
            }/*end grab*/
          what[y]=stack;
          if(stack) num++;   /*count nonzero entries*/
          }/*end fill*/
       x=EDL_FillBuffer(small,what,x,num,8);
       }/*end construction*/
     if(x<0) {if(x < 0) return -1;
             if(x) return x;}
     
     /*write data*/
    do{
     count=EDL_helper(&data,count,in,&pos,size,endian);/*build smaller table*/
     x=data&0x3FF;
     x=large[x];          /*x=short from thingy*/
     y=x&0xF;             /*y=normal bitcount*/
     z=(x>>4)&7;       /*z=backtrack bitcount*/
     //if(flagrant.message) printf("\n\tout: %X\tsample: %04X\tvalue: %X\tdata: %X",ftell_out,x,x>>7,data);
     if(y==0)  /*backtrack entry*/
       {x=x>>7; /*short's data*/
        y=(1<<z)-1;       /*bitmask*/
        count=EDL_helper(&data,count,in,&pos,size,endian);
        y=(data>>10)&y;
        x+=y;
        x=large[x+0x400];
        y=x&0xF;
        }/*end backtrack entry*/
     
     data=data>>y; count-=y;
     y=0;
     x=x>>7;       /*data only*/
     if(x<0x100) 
       {out[ftell_out++] = x;
       if(ftell_out>max) return ftell_out;}
     else if(x>0x100)/*copy previous*/
        {z=table2[x-0x101];
        if(z)        /*segment*/
          {count=EDL_helper(&data,count,in,&pos,size,endian);
          y=(1<<z)-1;  /*mask*/
          y=data&y; data=data>>z; count-=z;
          }        /*end segment*/
        z=table1[x-0x101];
        num=z+y+3;
        count=EDL_helper(&data,count,in,&pos,size,endian);
        x=data&0xFF; x=small[x];

        y=x&0xF;             /*y=normal bitcount*/
        z=(x&0x70)>>4;       /*z=backtrack bitcount*/
        if(y==0)  /*backtrack entry*/
          {x=x>>7; /*short's data*/
          y=(1<<z)-1;       /*bitmask*/
          count=EDL_helper(&data,count,in,&pos,size,endian);
          y=(data>>8)&y;
          x+=y;
          x=small[x+0x100];
          y=x&0xF;
          }/*end backtrack entry*/
        data=data>>y; count-=y;
        
        /*pull number of bits*/
        y=0;
        x=x>>7;
        z=table4[x];
        if(z)        /*segment*/
          {count=EDL_helper(&data,count,in,&pos,size,endian);
          y=data&((1<<z)-1); data=data>>z; count-=z;
          }        /*end segment*/
        z=table3[x];
        back=z+y+1;

        /*copy run*/
        for(x=0;num>0;num--)
           {z=ftell_out-back;
           if(z<0 || z>=ftell_out) x=0;
           else{//fseek(out,0-back,SEEK_END);     /*backward position*/
               //x=fgetc(out);
               x = out[ftell_out - back];
               }
           //fseek(out,0,SEEK_END);
           out[ftell_out++] = x;
           if(ftell_out>max) return ftell_out;     /*failsafe*/
           }/*end copy run*/
/*        for(x=0;num>0;num-=x)      this is faster but would need a catch
           {x=num;                   to keep it from copying bytes that have
           if(x>8) x=8;              not yet been written
           fseek(out,0-back,SEEK_END);
           fread(bits,1,x,out);
           fseek(out,0,SEEK_END);
           fwrite(bits,1,x,out);
           if(ftell_out>max) return ftell_out;
           }end debug-sometime-later*/
        }/*end copy previous*/
    }while(x!=0x100);     
   }/*mode 1*/
   else      /*mode 0 - */
     {count=EDL_helper(&data,count,in,&pos,size,endian);
     num=data&0x7FFF; data=data>>15; count-=15;
     //if(flagrant.message) printf("\nmode0\tpos: %X\tout: %X",pos,ftell_out);
     if(num) 
       {for(/*fseek(out,0,SEEK_END)*/;num>0;num--)
           {count=EDL_helper(&data,count,in,&pos,size,endian);
           x=data&0xFF; data=data>>8; count-=8;
           out[ftell_out++] = x;
           }/*end for()*/
       }/*write bytes*/
     }/*mode 0*/

   /*test EOF*/
   count=EDL_helper(&data,count,in,&pos,size,endian);
   x=data&1; data=data>>1; count--;
   if(x) return ftell_out;        /*1=EOF marker*/
   }

return ftell_out;}

/*boring RLE magic*/
unsigned long EDLdec2(unsigned long pos,unsigned char *in,unsigned char *out,unsigned long size,unsigned long max,int endian)
{unsigned char bits[9];
long x;
unsigned long count=0,num,back;  /*count=#bits in register, num=#to copy, back=#to backtrack*/
unsigned long long int data=0;       /*64bit datatable container*/
int ftell_out = 0;
/*set up data and count*/
size+=pos;

for(/*pos+=12*/;pos<=size;back=0)
   {memset(bits,0,9); /*clear bits between... stuff*/
   count=EDL_helper(&data,count,in,&pos,size,endian);
   x=data&1; data=data>>1; count--;
   if(x)     /*mode 1 - copy*/
     {count=EDL_helper(&data,count,in,&pos,size,endian);
     bits[0]=data&1; bits[1]=(data&2)>>1; bits[2]=(data&4)>>2; bits[3]=(data&8)>>3;
     //if(flagrant.message) {printf("\nmode1\tpos: %X\tout: %X\tdata: %X",pos,ftell_out,data);
     //                      printf("\n\t%X%X%X%X",bits[0],bits[1],bits[2],bits[3]);}
     if(bits[0]) {/*bit1:1*/
           num=2;
           data=data>>2; count-=2;
           if(bits[1]) 
             {data=data>>1; count--;
             num++;
             bits[8]=3;
             if(bits[2])
               {bits[8]=11;
               count=EDL_helper(&data,count,in,&pos,size,endian);
               num=data&0xFF; data=data>>8; count-=8;
               if(num==0) return ftell_out;   /*this implies #bytes=0, signifying EOF*/
               num+=8;
               }/*bits[2]*/
             }/*bits[1]*/
           }/*bits[0]=1*/
     else{
         bits[8]=3;
         num=4;
         data=data>>3; count-=3;     /*minimum shift*/
         if(bits[1]) num++;
         if(bits[2])
           {bits[8]=4;
           data=data>>1; count--;
           num=2*(num-1)+bits[3];
           if(num==9)          /*special case write mode*/
             {count=EDL_helper(&data,count,in,&pos,size,endian);
             num=data&0xF; data=data>>4; count-=4;
             num*=4;
             for(num+=12;num>0;num--)
                {count=EDL_helper(&data,count,in,&pos,size,endian);
                 x=data&0xFF; data=data>>8; count-=8;
                 out[ftell_out++] = x;
                 if(ftell_out>max) return ftell_out;     /*failsafe*/
                }
             continue;
             }/*num==9*/
           }/*bits[2]*/
          }/*bits[0]=0*/

     if(bits[8])        /*handle those backward offset types*/
       {count=EDL_helper(&data,count,in,&pos,size,endian);  /*copy next 6, then work out size*/
       bits[0]=data&1; bits[1]=(data&2)>>1; 
       bits[2]=(data&4)>>2; bits[3]=(data&8)>>3;
       bits[4]=(data&0x10)>>4; bits[5]=(data&0x20)>>5;
       data=data>>1; count--;
       //if(flagrant.message) printf("\t%X\t%X%X%X%X%X%X",bits[8],bits[0],bits[1],bits[2],bits[3],bits[4],bits[5]);
       
       if(bits[0])
         {if(bits[2])
            {if(bits[4])       /*10101 10111 11101 11111*/
               {data=data>>4; count-=4;
               back=0x400;
               if(bits[1]) back+=0x200;
               if(bits[3]) back+=0x100;
               }/*end bits[4]*/
             else              /*101000 101001 101100 101101 111000 111001 111100 111101*/
               {data=data>>5; count-=5;
               back=0x800;
               if(bits[1]) back+=0x400;
               if(bits[3]) back+=0x200;
               if(bits[5]) back+=0x100;
               }/*end bits[4]==0*/
            }/*end bits[2]*/
         else        /*bits[2]==0*/
            {if(bits[1])           /*110*/
               {back=0x100;
               data=data>>2; count-=2;
               }
            else                   /*1000 1001*/
               {data=data>>3; count-=3;
               back=0x200;
               if(bits[3]) back+=0x100;
               }
            }/*end bits[2]==0*/
         }/*end bits[0]*/  
       }/*bits[8]*/
     
     /*get the backward offset*/
     count=EDL_helper(&data,count,in,&pos,size,endian);
     back=(data&0xFF)+back+1;        /*assured to copy at least 1 byte*/
     data=data>>8; count-=8;
     //if(flagrant.message) printf("\n\tnum: %X\tback: %X",num,back);
     /*copy data from source*/
        for(x=0;num>0;num--)
           {x=ftell_out-back;
           if(x<0 || x>=ftell_out) x=0;
           else{//fseek(out,0-back,SEEK_END);     /*backward position*/
               //x=fgetc(out);
                x = out[ftell_out - back];
               }
           //fseek(out,0,SEEK_END);
           out[ftell_out++] = x;
           if(ftell_out>max) return ftell_out;     /*failsafe*/
           }/*end copy run*/
/*        for(x=0;num>0;num-=x)      this is faster but would need a catch
           {x=num;                   to keep it from copying bytes that have
           if(x>8) x=8;              not yet been written
           fseek(out,0-back,SEEK_END);
           fread(bits,1,x,out);
           fseek(out,0,SEEK_END);
           fwrite(bits,1,x,out);
           if(ftell_out>max) return ftell_out;
           }end debug-sometime-later*/    
     }/*if()*/
   else{     /*mode 0 - push one byte to output*/
        count=EDL_helper(&data,count,in,&pos,size,endian);
        //if(flagrant.message) printf("\nmode0\tpos: %X\tout: %X\tdata: %X",pos,ftell_out,data);
        x=data&0xFF; data=data>>8; count-=8;
        out[ftell_out++] = x;
        if(ftell_out>max) return ftell_out;     /*failsafe*/
        }
   }

return ftell_out;}



int dungeon_kid(u8 *ROM, u8 *log) {
    int readbyte, repeatval, reps, output = 0;
    for(;;) {
     readbyte=*ROM++;
     if(readbyte==0xDD)
     {
        //Read repeated bytes.
        repeatval=*ROM++;
        reps=*ROM++;
        while(reps>0)
        {
           log[output] = repeatval;
           output++;
           reps--;
        }
     }
     else if(readbyte==0xCC)
     {
        //Repeat Colored Lines.
        reps=*ROM++;
        readbyte=0xFF;
        while(reps>0)
        {
           log[output] = readbyte;
           output++;
           reps--;
        }
     }
     else if(readbyte==0xBB)
     {
        //Repeat Blank Lines
        reps=*ROM++;
        readbyte=0x00;
        while(reps>0)
        {
           log[output] = readbyte;
           output++;
           reps--;
        }
     }
     else if(readbyte==0xAA)
     {
        //cout<<"????\n";
     }
     else if(readbyte==0x99)
     {
        //Avoid next control code.
        readbyte=*ROM++;
        log[output] = readbyte;
        output++;
     }
     else if(readbyte==0xEE)
     {
        break; //cout<<"End compression.\n";
     }
     else
     {
        log[output] = readbyte;
        output++;
     }
     readbyte=*ROM++;
  }
    return output;
}



// original code by AID_X, Dr. MefistO - http://lab313.ru
int frontmission2(u8 *pFile, int PBsize, u8 *extrbuf) {
    int     i, CC, RR, RepCount, z = 0;
    u16     RRRR;
    u8      *pFilel = pFile + PBsize;
    while(pFile < pFilel) {
        CC = *pFile++;

        if((CC >= 0x00) && (CC <= 0x3f)) {
            RepCount = CC + 1;

            for(i = 1; i <= RepCount; i++) {
              RR = *pFile++;
              extrbuf[z++] = RR;
            }
        } else if((CC >= 0x40) && (CC <= 0x7f)) {
            RepCount = CC - 0x40 + 3;
            RR = *pFile++;
            for(i = 1; i <= RepCount; i++) {
              extrbuf[z++] = RR;
            }
        } else if((CC >= 0x80) && (CC <= 0xbf)) {
            RepCount = CC - 0x80 + 2;
            RR = *pFile++;
            for(i = 1; i <= RepCount; i++) {
              extrbuf[z] = extrbuf[z - RR];
              z++;
            }
        } else if((CC >= 0xc0) && (CC <= 0xff)) {
            RepCount = CC - 0xC0 + 2;
            RRRR = pFile[0] | (pFile[1] << 8);
            pFile += 2;
            for(i = 1; i <= RepCount; i++) {
              extrbuf[z] = extrbuf[z - RRRR];
              z++;
            }
        }
    }
    return z;
}



// coverted to C from:
//  CompressTools (c) 2012 by Bregalad
//  RLEINC is (c) 2012 by Joel Yliluoma, http://iki.fi/bisqwit/

int rleinc1(u8 *encoded, int encoded_size, u8 *result) {
    u8 *bck = result;
        //int end_where = -1;
        int i, b, n;
        for( i=0; i < encoded_size; )
        {
            int c = encoded[i++] & 0xFF;
            if((c & 0x80) != 0) // RUN
            {
                for( b = encoded[i++], n = 0x101-c; n > 0; --n)
                    *result++ = ( (byte)b );
            }
            else
            {
                if((c & 0x40) != 0) // SEQ
                {
                    if(c == 0x40)
                    {
                        //end_where = i;
                        break;
                    }
                    for( b = encoded[i++], n = c-0x3F; n > 0; --n)
                        *result++ = ( (byte)(b++ & 0xFF) );
                }
                else // LIT
                {
                    for( n = c; n >= 0; --n)
                        *result++ = ( encoded[i + n] );
                    i += (c + 1);
                }
            }
        }
    return result - bck;
}



int rleinc2(u8 *encoded, int encoded_size, u8 *result) {
    u8 *bck = result;
    int i,b,n;
        //int end_where = -1;
        for( i=0; i < encoded_size; )
        {
            int c = encoded[i++] & 0xFF;
            if((c & 0x80) != 0)
            {
                if(c >= 0xA0) // RUN
                    for( b = encoded[i++], n = 0x101-c; n > 0; --n)
                        *result++ = ( (byte)b );
                else // DBL
                {
                    byte b1 = encoded[i++];
                    byte b2 = encoded[i++];
                    for( n = c-0x7D; n > 0; --n)
                    {
                        *result++ = (b1);
                        byte tmp=b1;
                        b1=b2;
                        b2=tmp;
                    }
                }
            }
            else
            {
                if((c & 0x40) != 0) // SEQ
                {
                    if(c == 0x40)
                    {
                        //end_where = i;
                        break;
                    }
                    for( b = encoded[i++], n = c-0x3F; n > 0; --n)
                        *result++ = ( (byte)(b++ & 0xFF) );
                }
                else // LIT
                {
                    for( n = c; n >= 0; --n)
                        *result++ = ( encoded[i + n] );
                    i += (c + 1);
                }
            }
        }
    return result - bck;
}



// original code from GMMan Evolution Engine Cache Extractor http://forum.xentax.com/viewtopic.php?f=32&t=10782
int evolution_unpack(unsigned char *compressedData, int compressedData_Length, unsigned char *decompressedData, int decompressedData_Length)
		{
            int i;
			int compPos = 0;
			int decompPos = 0;
			int compLen = compressedData_Length;
			int decompLen = decompressedData_Length;

			while (compPos < compLen)
			{
				unsigned char codeWord = compressedData[compPos++];
				if (codeWord <= 0x1f)
				{
					// Encode literal
					if (decompPos + codeWord + 1 > decompLen) return -1; //throw new IndexOutOfRangeException("Attempting to index past decompression buffer.");
					if (compPos + codeWord + 1 > compLen) return -1; //throw new IndexOutOfRangeException("Attempting to index past compression buffer.");
					for ( i = codeWord; i >= 0; --i)
					{
						decompressedData[decompPos] = compressedData[compPos];
						++decompPos;
						++compPos;
					}

				}
				else
				{
					// Encode dictionary
					int copyLen = codeWord >> 5; // High 3 bits are copy length
					if (copyLen == 7) // If those three make 7, then there are more bytes to copy (maybe)
					{
						if (compPos >= compLen) return -1; //throw new IndexOutOfRangeException("Attempting to index past compression buffer.");
						copyLen += compressedData[compPos++]; // Grab next byte and add 7 to it
					}
					if (compPos >= compLen) return -1; //throw new IndexOutOfRangeException("Attempting to index past compression buffer.");
					int dictDist = ((codeWord & 0x1f) << 8) | compressedData[compPos]; // 13 bits code lookback offset
					++compPos;
					copyLen += 2; // Add 2 to copy length
					if (decompPos + copyLen > decompLen) return -1; //throw new IndexOutOfRangeException("Attempting to index past decompression buffer.");
					int decompDistBeginPos = decompPos - 1 - dictDist;
					if (decompDistBeginPos < 0) return -1; //throw new IndexOutOfRangeException("Attempting to index below decompression buffer.");
					for ( i = 0; i < copyLen; ++i, ++decompPos)
					{
						decompressedData[decompPos] = decompressedData[decompDistBeginPos + i];
					}
				}
			}
    return decompPos;
}



int nislzs(u8 *binaryReader, u8 *binaryWriter, int num3) {
    u8  *bck = binaryWriter;

	int num5 = 256;
	byte array[num5];
    int i;
	for ( i = 0; i < num5; i++)
	{
		array[i] = 255;
	}
	int j = 0;
	int num6 = 0;
	byte b = *binaryReader++;
	if (*binaryReader++ != 0 || *binaryReader++ != 0 || *binaryReader++ != 0)
	{
		return -1; //throw new Exception("padding bytes are not 0");
	}
	while (j < num3)
	{
		byte b2 = *binaryReader++;
		if (b2 == b)
		{
			byte b3 = *binaryReader++;
			if (b3 == b)
			{
				*binaryWriter++ = (b3);
				array[num6++] = b3;
				num6 %= num5;
				j++;
			}
			else
			{
				byte b4 = *binaryReader++;
				byte b5 = b4;
				byte b6 = b3;
				if (b6 >= b)
				{
					b6 -= 1;
				}
                int k;
				for ( k = 0; k < (int)b5; k++)
				{
					b2 = array[(num6 + num5 - (int)b6) & (num5 - 1)];
					*binaryWriter++ = (b2);
					array[num6++] = b2;
					num6 %= num5;
					j++;
				}
			}
		}
		else
		{
			*binaryWriter++ = (b2);
			array[num6++] = b2;
			num6 %= num5;
			j++;
		}
	}

    return binaryWriter - bck;
}
