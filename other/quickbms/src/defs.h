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

// QuickBMS enum, defines, global variables and so on

// QuickBMS return codes
enum {
    QUICKBMS_OK                 = 0,    // success
    QUICKBMS_ERROR_UNKNOWN      = 1,    // any error
    QUICKBMS_ERROR_MEMORY       = 2,    // unable to allocate memory, memory errors
    QUICKBMS_ERROR_FILE_READ    = 3,    // impossible to read/seek input file
    QUICKBMS_ERROR_FILE_WRITE   = 4,    // impossible to write output file
    QUICKBMS_ERROR_COMPRESSION  = 5,    // errors related to file compression
    QUICKBMS_ERROR_ENCRYPTION   = 6,    // errors related to file encryption
    QUICKBMS_ERROR_DLL          = 7,    // any external dll or executable
    QUICKBMS_ERROR_BMS          = 8,    // anything related the BMS script and language
    QUICKBMS_ERROR_ARGUMENTS    = 9,    // quickbms arguments (argc, argv)
    QUICKBMS_ERROR_FOLDER       = 10,   // problems with the input/output folders
    QUICKBMS_ERROR_USER         = 11,   // termination caused by the user
    QUICKBMS_ERROR_EXTRA        = 12,   // extra IO input/output
    QUICKBMS_ERROR_UPDATE       = 13,   // update feature
    //
    QUICKBMS_ERROR_DUMMY
};



enum {
    CMD_NONE = 0,
    CMD_CLog,
    CMD_Do,
    CMD_FindLoc,
    CMD_For,
    CMD_ForTo,  // for an easy handling of For
    CMD_Get,
    CMD_GetDString,
    CMD_GoTo,
    CMD_IDString,
    CMD_ImpType,
    CMD_Log,
    CMD_Math,
    CMD_Next,
    CMD_Open,
    CMD_SavePos,
    CMD_Set,
    CMD_While,
    CMD_String,
    CMD_CleanExit,
    CMD_If,
    CMD_Else,
    CMD_Elif,   // added by me
    CMD_EndIf,
    CMD_GetCT,
    CMD_ComType,
    CMD_ReverseLong,
        // added by me
    CMD_Endian,
    CMD_FileXOR,        // similar job done also by Encryption
    CMD_FileRot13,      // similar job done also by Encryption
    CMD_FileCrypt,      // experimental and useless
    CMD_Break,          // not necessary
    CMD_Strlen,         // not necessary (implemented in Set)
    CMD_GetVarChr,
    CMD_PutVarChr,
    CMD_Debug,          // only for debugging like -v, so not necessary
    CMD_Padding,        // useful but not necessary, can be done with GoTo
    CMD_Append,
    CMD_Encryption,
    CMD_Print,
    CMD_GetArray,
    CMD_PutArray,
    CMD_SortArray,
    CMD_StartFunction,
    CMD_CallFunction,
    CMD_EndFunction,
    CMD_ScanDir,        // not needed for the extraction jobs
    CMD_CallDLL,
    CMD_Put,            // not needed for the extraction jobs
    CMD_PutDString,     // not needed for the extraction jobs
    CMD_PutCT,          // not needed for the extraction jobs
    CMD_GetBits,        // rarely useful
    CMD_PutBits,        // rarely useful
    CMD_ReverseShort,   // rarely useful
    CMD_ReverseLongLong,// rarely useful
    CMD_Prev,           // like i--
    CMD_XMath,          // one line math
    //CMD_Continue,       // not implemented yet
    CMD_NOP
};



#define ISNUMTYPE(X)    ((X > 0) || (X == TYPE_ASIZE))
enum {  // the value is referred to their size which makes the job faster, numbers are positive and the others are negative!
    TYPE_NONE               = 0,
    TYPE_BYTE               = 1,
    TYPE_SHORT              = 2,
    TYPE_THREEBYTE          = 3,
    TYPE_LONG               = 4,
    TYPE_LONGLONG           = 8,
    TYPE_STRING             = -1,
    TYPE_ASIZE              = -2,
    TYPE_PURETEXT           = -3,
    TYPE_PURENUMBER         = -4,
    TYPE_TEXTORNUMBER       = -5,
    TYPE_FILENUMBER         = -6,
        // added by me
    TYPE_FILENAME           = -1000,
    TYPE_BASENAME           = -1001,
    TYPE_EXTENSION          = -1002,
    TYPE_UNICODE            = -1003,
    TYPE_BINARY             = -1004,
    TYPE_LINE               = -1005,
    TYPE_FULLNAME           = -1006,
    TYPE_CURRENT_FOLDER     = -1007,
    TYPE_FILE_FOLDER        = -1008,
    TYPE_INOUT_FOLDER       = -1009,
    TYPE_BMS_FOLDER         = -1010,
    TYPE_ALLOC              = -1011,
    TYPE_COMPRESSED         = -1012,
    TYPE_FLOAT              = -1013,
    TYPE_DOUBLE             = -1014,
    TYPE_LONGDOUBLE         = -1015,
    TYPE_VARIABLE           = -1016,    // c & 0x80
    TYPE_VARIABLE2          = -1017,    // unreal index numbers
    TYPE_VARIANT            = -1018,
    TYPE_BITS               = -1019,
    TYPE_TIME               = -1020,
    TYPE_TIME64             = -1021,
    TYPE_CLSID              = -1022,
    TYPE_IPV4               = -1023,
    TYPE_IPV6               = -1024,
    TYPE_ASM                = -1025,
    TYPE_VARIABLE3          = -1026,
    TYPE_SIGNED_BYTE        = -1027,
    TYPE_SIGNED_SHORT       = -1028,
    TYPE_SIGNED_THREEBYTE   = -1029,
    TYPE_SIGNED_LONG        = -1030,
    TYPE_VARIABLE4          = -1031,
        //
    TYPE_UNKNOWN            = -2000,
        // nop
    TYPE_NOP
};



/*
if you add a new compression algorithm remember to modify the following files:
- defs.h
- cmd.h     -> CMD_ComType_func
- perform.h -> perform_compression
*/
enum {  // note that the order must be not change due to the introduction of the scan feature
    COMP_NONE = 0,
    COMP_ZLIB,          // RFC 1950
    COMP_DEFLATE,       // RFC 1951
    COMP_LZO1,
    COMP_LZO1A,
    COMP_LZO1B,         // scan 5
    COMP_LZO1C,
    COMP_LZO1F,
    COMP_LZO1X,
    COMP_LZO1Y,
    COMP_LZO1Z,         // scan 10
    COMP_LZO2A,
    COMP_LZSS,
    COMP_LZX,
    COMP_GZIP,
    COMP_EXPLODE,       // scan 15
    COMP_LZMA,
    COMP_LZMA_86HEAD,
    COMP_LZMA_86DEC,
    COMP_LZMA_86DECHEAD,
    COMP_LZMA_EFS,      // scan 20
    COMP_BZIP2,
    COMP_XMEMLZX,
    COMP_HEX,
    COMP_BASE64,
    COMP_UUENCODE,      // scan 25
    COMP_ASCII85,
    COMP_YENC,
    COMP_UNLZW,
    COMP_UNLZWX,
    COMP_LZXCAB,        // scan 30
    COMP_LZXCHM,
    COMP_RLEW,
    COMP_LZJB,
    COMP_SFL_BLOCK,
    COMP_SFL_RLE,       // scan 35
    COMP_SFL_NULLS,
    COMP_SFL_BITS,
    COMP_LZMA2,
    COMP_LZMA2_86HEAD,
    COMP_LZMA2_86DEC,   // scan 40
    COMP_LZMA2_86DECHEAD,
    COMP_NRV2b,
    COMP_NRV2d,
    COMP_NRV2e,
    COMP_HUFFBOH,       // scan 45
    COMP_UNCOMPRESS,
    COMP_DMC,
    COMP_LZH,
    COMP_LZARI,
    COMP_TONY,          // scan 50
    COMP_RLE7,
    COMP_RLE0,
    COMP_RLE,
    COMP_RLEA,
    COMP_BPE,           // scan 55
    COMP_QUICKLZ,
    COMP_Q3HUFF,
    COMP_UNMENG,
    COMP_LZ2K,
    COMP_DARKSECTOR,    // scan 60
    COMP_MSZH,
    COMP_UN49G,
    COMP_UNTHANDOR,
    COMP_DOOMHUFF,
    COMP_APLIB,         // scan 65
    COMP_TZAR_LZSS,
    COMP_LZF,
    COMP_CLZ77,
    COMP_LZRW1,
    COMP_DHUFF,         // scan 70
    COMP_FIN,
    COMP_LZAH,
    COMP_LZH12,
    COMP_LZH13,
    COMP_GRZIP,         // scan 75
    COMP_CKRLE,
    COMP_QUAD,
    COMP_BALZ,
    COMP_DEFLATE64,
    COMP_SHRINK,        // scan 80
    COMP_PPMDI,
    COMP_MULTIBASE,
    COMP_BRIEFLZ,
    COMP_PAQ6,
    COMP_SHCODEC,       // scan 85
    COMP_HSTEST1,
    COMP_HSTEST2,
    COMP_SIXPACK,
    COMP_ASHFORD,
    COMP_JCALG,         // scan 90
    COMP_JAM,
    COMP_LZHLIB,
    COMP_SRANK,
    COMP_ZZIP,
    COMP_SCPACK,        // scan 95
    COMP_RLE3,
    COMP_BPE2,
    COMP_BCL_HUF,
    COMP_BCL_LZ,
    COMP_BCL_RICE,      // scan 100
    COMP_BCL_RLE,
    COMP_BCL_SF,
    COMP_SCZ,
    COMP_SZIP,
    COMP_PPMDI_RAW,     // scan 105
    COMP_PPMDG,
    COMP_PPMDG_RAW,
    COMP_PPMDJ,
    COMP_PPMDJ_RAW,
    COMP_SR3C,          // scan 110
    COMP_HUFFMANLIB,
    COMP_SFASTPACKER,
    COMP_SFASTPACKER2,
    COMP_DK2,
    COMP_LZ77WII,       // scan 115
    COMP_LZ77WII_RAW10,
    COMP_DARKSTONE,
    COMP_SFL_BLOCK_CHUNKED,
    COMP_YUKE_BPE,
    COMP_STALKER_LZA,   // scan 120
    COMP_PRS_8ING,
    COMP_PUYO_CNX,
    COMP_PUYO_CXLZ,
    COMP_PUYO_LZ00,
    COMP_PUYO_LZ01,     // scan 125
    COMP_PUYO_LZSS,
    COMP_PUYO_ONZ,
    COMP_PUYO_PRS,
    COMP_FALCOM,
    COMP_CPK,           // scan 130
    COMP_BZIP2_FILE,
    COMP_LZ77WII_RAW11,
    COMP_LZ77WII_RAW30,
    COMP_LZ77WII_RAW20,
    COMP_PGLZ,          // scan 135
    COMP_SLZ,
    COMP_SLZ_01,
    COMP_SLZ_02,
    COMP_LZHL,
    COMP_D3101,         // scan 140
    COMP_SQUEEZE,
    COMP_LZRW3,
    COMP_TDCB_ahuff,
    COMP_TDCB_arith,
    COMP_TDCB_arith1,   // scan 145
    COMP_TDCB_arith1e,
    COMP_TDCB_arithn,
    COMP_TDCB_compand,
    COMP_TDCB_huff,
    COMP_TDCB_lzss,     // scan 150
    COMP_TDCB_lzw12,
    COMP_TDCB_lzw15v,
    COMP_TDCB_silence,
    COMP_RDC,
    COMP_ILZR,          // scan 155
    COMP_DMC2,
    COMP_diffcomp,
    COMP_LZR,
    COMP_LZS,
    COMP_LZS_BIG,       // scan 160
    COMP_COPY,
    COMP_MOHLZSS,
    COMP_MOHRLE,
    COMP_YAZ0,
    COMP_BYTE2HEX,      // scan 165
    COMP_UN434A,
    COMP_UNZIP_DYNAMIC,
    COMP_XXENCODE,
    COMP_GZPACK,
    COMP_ZLIB_NOERROR,  // scan 170
    COMP_DEFLATE_NOERROR,
    COMP_PPMDH,
    COMP_PPMDH_RAW,
    COMP_RNC,
    COMP_RNC_RAW,       // scan 175
    COMP_FITD,
    COMP_KENS_Nemesis,
    COMP_KENS_Kosinski,
    COMP_KENS_Kosinski_moduled,
    COMP_KENS_Enigma,   // scan 180
    COMP_KENS_Saxman,
    COMP_DRAGONBALLZ,
    COMP_NITROSDK,
    COMP_ZDAEMON,
    COMP_SKULLTAG,      // scan 185
    COMP_MSF,
    COMP_STARGUNNER,
    COMP_NTCOMPRESS,
    COMP_CRLE,
    COMP_CTW,           // scan 190
    COMP_DACT_DELTA,
    COMP_DACT_MZLIB2,
    COMP_DACT_MZLIB,
    COMP_DACT_RLE,
    COMP_DACT_SNIBBLE,  // scan 195
    COMP_DACT_TEXT,
    COMP_DACT_TEXTRLE,
    COMP_EXECUTE,
    COMP_LZ77_0,
    COMP_LZBSS,         // scan 200
    COMP_BPAQ0,
    COMP_LZPX,
    COMP_MAR_RLE,
    COMP_GDCM_RLE,
    COMP_LZMAT,         // scan 205
    COMP_DICT,
    COMP_REP,
    COMP_LZP,
    COMP_ELIAS_DELTA,
    COMP_ELIAS_GAMMA,   // scan 210
    COMP_ELIAS_OMEGA,
    COMP_PACKBITS,
    COMP_DARKSECTOR_NOCHUNKS,
    COMP_ENET,
    COMP_EDUKE32,       // scan 215
    COMP_XU4_RLE,
    COMP_RVL,
    COMP_LZFU,
    COMP_LZFU_RAW,
    COMP_XU4_LZW,       // scan 220
    COMP_HE3,
    COMP_IRIS,
    COMP_IRIS_HUFFMAN,
    COMP_IRIS_UO_HUFFMAN,
    COMP_NTFS,          // scan 225
    COMP_PDB,
    COMP_COMPRLIB_SPREAD,
    COMP_COMPRLIB_RLE1,
    COMP_COMPRLIB_RLE2,
    COMP_COMPRLIB_RLE3, // scan 230
    COMP_COMPRLIB_RLE4,
    COMP_COMPRLIB_ARITH,
    COMP_COMPRLIB_SPLAY,
    COMP_CABEXTRACT,
    COMP_MRCI,          // scan 235
    COMP_HD2_01,
    COMP_HD2_08,
    COMP_HD2_01raw,
    COMP_RTL_LZNT1,
    COMP_RTL_XPRESS,    // scan 240
    COMP_RTL_XPRESS_HUFF,
    COMP_PRS,
    COMP_SEGA_LZ77,
    COMP_SAINT_SEYA,
    COMP_NTCOMPRESS30,  // scan 245
    COMP_NTCOMPRESS40,
    COMP_SLZ_03,
    COMP_YAKUZA,
    COMP_LZ4,
    COMP_SNAPPY,        // scan 250
    COMP_LUNAR_LZ1,
    COMP_LUNAR_LZ2,
    COMP_LUNAR_LZ3,
    COMP_LUNAR_LZ4,
    COMP_LUNAR_LZ5,     // scan 255
    COMP_LUNAR_LZ6,
    COMP_LUNAR_LZ7,
    COMP_LUNAR_LZ8,
    COMP_LUNAR_LZ9,
    COMP_LUNAR_LZ10,    // scan 260
    COMP_LUNAR_LZ11,
    COMP_LUNAR_LZ12,
    COMP_LUNAR_LZ13,
    COMP_LUNAR_LZ14,
    COMP_LUNAR_LZ15,    // scan 265
    COMP_LUNAR_LZ16,
    COMP_LUNAR_RLE1,
    COMP_LUNAR_RLE2,
    COMP_LUNAR_RLE3,
    COMP_LUNAR_RLE4,    // scan 270
    COMP_GOLDENSUN,
    COMP_LUMINOUSARC,
    COMP_LZV1,
    COMP_FASTLZAH,
    COMP_ZAX,           // scan 275
    COMP_SHRINKER,
    COMP_MMINI_HUFFMAN,
    COMP_MMINI_LZ1,
    COMP_MMINI,
    COMP_CLZW,          // scan 280
    COMP_LZHAM,
    COMP_LPAQ8,
    COMP_SEGA_LZS2,
    COMP_CALLDLL,
    COMP_WOLF,          // scan 285
    COMP_COREONLINE,
    COMP_MSZIP,
    COMP_QTM,
    COMP_MSLZSS,
    COMP_MSLZSS1,       // scan 290
    COMP_MSLZSS2,
    COMP_KWAJ,
    COMP_LZLIB,
    COMP_DFLT,
    COMP_LZMA_DYNAMIC,  // scan 295
    COMP_LZMA2_DYNAMIC,
    COMP_LZMA2_EFS,
    COMP_LZXCAB_DELTA,
    COMP_LZXCHM_DELTA,
    COMP_FFCE,          // scan 300
    COMP_SCUMMVM4,
    COMP_SCUMMVM5,
    COMP_SCUMMVM6,
    COMP_SCUMMVM7,
    COMP_SCUMMVM8,      // scan 305
    COMP_SCUMMVM9,
    COMP_SCUMMVM10,
    COMP_SCUMMVM11,
    COMP_SCUMMVM12,
    COMP_SCUMMVM13,     // scan 310
    COMP_SCUMMVM14,
    COMP_SCUMMVM15,
    COMP_SCUMMVM16,
    COMP_SCUMMVM17,
    COMP_SCUMMVM18,     // scan 315
    COMP_SCUMMVM19,
    COMP_SCUMMVM20,
    COMP_SCUMMVM21,
    COMP_SCUMMVM22,
    COMP_SCUMMVM23,     // scan 320
    COMP_SCUMMVM24,
    COMP_SCUMMVM25,
    COMP_SCUMMVM26,
    COMP_SCUMMVM27,
    COMP_SCUMMVM28,     // scan 325
    COMP_SCUMMVM29,
    COMP_SCUMMVM30,
    COMP_SCUMMVM31,
    COMP_SCUMMVM32,
    COMP_SCUMMVM33,     // scan 330
    COMP_SCUMMVM34,
    COMP_SCUMMVM35,
    COMP_SCUMMVM36,
    COMP_SCUMMVM37,
    COMP_SCUMMVM38,     // scan 335
    COMP_SCUMMVM39,
    COMP_SCUMMVM40,
    COMP_SCUMMVM41,
    COMP_SCUMMVM42,
    COMP_SCUMMVM43,     // scan 340
    COMP_SCUMMVM44,
    COMP_SCUMMVM45,
    COMP_SCUMMVM46,
    COMP_SCUMMVM47,
    COMP_SCUMMVM48,     // scan 345
    COMP_SCUMMVM49,
    COMP_SCUMMVM50,
    COMP_SCUMMVM51,
    COMP_SCUMMVM52,
    COMP_SCUMMVM53,     // scan 350
    COMP_LZS_UNZIP,
    COMP_LEGEND_OF_MANA,
    COMP_DIZZY,
    COMP_EDL1,
    COMP_EDL2,          // scan 355
    COMP_DUNGEON_KID,
    COMP_LUNAR_LZ17,
    COMP_LUNAR_LZ18,
    COMP_FRONTMISSION2,
    COMP_RLEINC1,       // scan 360
    COMP_RLEINC2,
    COMP_EVOLUTION,
    COMP_PUYO_LZ10,
    COMP_PUYO_LZ11,
    COMP_NISLZS,        // scan 365
    COMP_UNKNOWN1,
    COMP_UNKNOWN2,
    COMP_UNKNOWN3,
    COMP_UNKNOWN4,
    COMP_UNKNOWN5,      // scan 370
    COMP_UNKNOWN6,
    COMP_UNKNOWN7,
    COMP_UNKNOWN8,
    COMP_UNKNOWN9,
    COMP_UNKNOWN10,     // scan 375
    COMP_UNKNOWN11,
    COMP_UNKNOWN12,
    COMP_UNKNOWN13,
    COMP_UNKNOWN14,
    COMP_UNKNOWN15,     // scan 380
    COMP_UNKNOWN16,
    COMP_UNKNOWN17,
    COMP_UNKNOWN18,
    COMP_UNKNOWN19,
    COMP_BLACKDESERT,   // scan 385
    COMP_BLACKDESERT_RAW,
    COMP_PUCRUNCH,
    COMP_ZPAQ,
        // nop
    COMP_NOP,
        // compressors
    COMP_ZLIB_COMPRESS      = 10000,
    COMP_DEFLATE_COMPRESS,
    COMP_LZO1_COMPRESS,
    COMP_LZO1X_COMPRESS,
    COMP_LZO2A_COMPRESS,
    COMP_XMEMLZX_COMPRESS,
    COMP_BZIP2_COMPRESS,
    COMP_GZIP_COMPRESS,
    COMP_LZSS_COMPRESS,
    COMP_SFL_BLOCK_COMPRESS,
    COMP_SFL_RLE_COMPRESS,
    COMP_SFL_NULLS_COMPRESS,
    COMP_SFL_BITS_COMPRESS,
    COMP_LZF_COMPRESS,
    COMP_BRIEFLZ_COMPRESS,
    COMP_JCALG_COMPRESS,
    COMP_BCL_HUF_COMPRESS,
    COMP_BCL_LZ_COMPRESS,
    COMP_BCL_RICE_COMPRESS,
    COMP_BCL_RLE_COMPRESS,
    COMP_BCL_SF_COMPRESS,
    COMP_SZIP_COMPRESS,
    COMP_HUFFMANLIB_COMPRESS,
    COMP_LZMA_COMPRESS,
    COMP_LZMA_86HEAD_COMPRESS,
    COMP_LZMA_86DEC_COMPRESS,
    COMP_LZMA_86DECHEAD_COMPRESS,
    COMP_LZMA_EFS_COMPRESS,
    COMP_FALCOM_COMPRESS,
    COMP_KZIP_ZLIB_COMPRESS,
    COMP_KZIP_DEFLATE_COMPRESS,
    COMP_PRS_COMPRESS,
    COMP_RNC_COMPRESS,
    COMP_LZ4_COMPRESS,
    COMP_SFL_BLOCK_CHUNKED_COMPRESS,
        // nop
    COMP_ERROR
};



#define QUICK_COMP_ENUM(X) \
    COMP_##X,
#define QUICK_COMP_ASSIGN(X) \
    else if(!stricmp(str, #X)) \
        compression_type = COMP_##X;
#define QUICK_COMP_ASSIGN2(X,Y) \
    else if(!stricmp(str, #X) || !stricmp(str, #Y)) \
        compression_type = COMP_##X;
#define QUICK_COMP_ASSIGN3(X,Y,Z) \
    else if(!stricmp(str, #X) || !stricmp(str, #Y) || !stricmp(str, #Z)) \
        compression_type = COMP_##X;
#define QUICK_COMP_CASE(X) \
    case COMP_##X:  set_int3(COMP_##X, in, zsize, out, size);



#define QUICK_CRYPT_CASE(X) \
    if(X) { \
        if(datalen < 0) return(0); \
        set_int3(X, data, datalen, NULL, NULL);



//#pragma pack(1)



enum {
    LZMA_FLAGS_NONE         = 0,
    LZMA_FLAGS_86_HEADER    = 1,
    LZMA_FLAGS_86_DECODER   = 2,
    LZMA_FLAGS_EFS          = 4,
    LZMA_FLAGS_NOP
};



typedef struct {
    void    *info;
    u8      *data;
    int     size;
} data_t;



typedef struct {
    u8      active;
    int     vars;
    int     *var;           // example: idx of i and j
    int     arrays;
    data_t  *array;         // list of arrays containing the various values of i:j
} sub_variable_t;



typedef struct {
    // for optimizing the usage of the memory I use a static buffer and an allocated pointer used if
    // the static buffer is not big enough
    // pros: fast and avoids memory consumption with xalloc
    // cons: wastes memory, moreover with -9 (compared with the allocated only version)

    u8      *name;          // name of the variable, it can be also a fixed number since "everything" is handled as a variable
    u8      *name_alloc;
    u8      name_static[VAR_NAMESZ + 1];

    u8      *value;         // content of the variable
    u8      *value_alloc;
    u8      value_static[VAR_VALUESZ + 1];

    int     value32;        // number

    int     size;           // used for avoiding to waste realloc too much, not so much important and well used in reality

    u8      isnum;          // 1 if it's a number, 0 if a string
    u8      constant;       // 1 if the variable is a fixed number and not a "real" variable
    u8      binary;         // 1 if the variable is binary
    u8      reserved;

    sub_variable_t  *sub_var;
} variable_t;



typedef struct {
    int     var[MAX_ARGS];  // pointer to a variable
    int     num[MAX_ARGS];  // simple number
    u8      *str[MAX_ARGS]; // fixed string
    u8      type;           // type of command to execute
    u8      *debug_line;    // used with -v
} command_t;



#define FDBITS \
    u8      bitchr; \
    u8      bitpos; \
    u_int   bitoff;



typedef struct {
    u8      byte;
    u8      idx;    // it's necessary to save the memory although idx can be truncated
    u8      flags;
    u8      *name;
} hexhtml_t;



typedef struct {
    FILE    *fd;
    u8      *fullname;      // just the same input filename, like c:\myfile.pak or ..\..\myfile.pak
    u8      *filename;      // input filename only, like myfile.pak
    u8      *basename;      // input basename only, like myfile
    u8      *fileext;       // input extension only, like pak
    FDBITS
    hexhtml_t   *hexhtml;
    int     hexhtml_size;
    int     coverage;       // experimental coverage
    void    *sd;            // socket operations
    void    *pd;            // process memory operations
    void    *ad;            // audio operations
    void    *vd;            // video operations
    void    *md;            // Windows messages operations
} filenumber_t;



typedef struct {
    u8      *data;
    int     pos;
    int     size;
    int     maxsize;
    FDBITS
    hexhtml_t   *hexhtml;
    int     hexhtml_size;
    int     coverage;       // experimental coverage
} memory_file_t;



typedef struct {
    int         allocated_elements;
    int         elements;
    variable_t  *var;
} array_t;



typedef struct {
    u8      *name;
    //int     offset; // unused at the moment
    int     size;
} files_t;



typedef struct {
    u32     g1;
    u16     g2;
    u16     g3;
    u8      g4;
    u8      g5;
    u8      g6;
    u8      g7;
    u8      g8;
    u8      g9;
    u8      g10;
    u8      g11;
} clsid_t;



filenumber_t    filenumber[MAX_FILES + 1];
variable_t      variable_main[MAX_VARS + 1];
variable_t      *variable = variable_main;  // remember to reinitialize it every time (to avoid problems with callfunction)
command_t       command[MAX_CMDS + 1];
memory_file_t   memory_file[MAX_FILES + 1];
array_t         array[MAX_ARRAYS + 1];



#ifndef DISABLE_SSL
EVP_CIPHER_CTX  *evp_ctx        = NULL;
EVP_MD_CTX      *evpmd_ctx      = NULL;
BF_KEY          *blowfish_ctx   = NULL;
typedef struct {
    AES_KEY     ctx;
    u8          ivec[AES_BLOCK_SIZE];
    u8          ecount[AES_BLOCK_SIZE];
	unsigned    num;
} aes_ctr_ctx_t;
aes_ctr_ctx_t   *aes_ctr_ctx    = NULL;
#endif
tea_context     *tea_ctx        = NULL;
xtea_context    *xtea_ctx       = NULL;
xxtea_context   *xxtea_ctx      = NULL;
swap_context    *swap_ctx       = NULL;
math_context    *math_ctx       = NULL;
xmath_context   *xmath_ctx      = NULL;
random_context  *random_ctx     = NULL;
xor_context     *xor_ctx        = NULL;
rot_context     *rot_ctx        = NULL;
rotate_context  *rotate_ctx     = NULL;
reverse_context *reverse_ctx    = NULL;
inc_context     *inc_ctx        = NULL;
charset_context *charset_ctx    = NULL;
charset_context *charset2_ctx   = NULL;
TWOFISH_context *twofish_ctx    = NULL;
SEED_context    *seed_ctx       = NULL;
serpent_context_t *serpent_ctx  = NULL;
ICE_KEY         *ice_ctx        = NULL; // must be not allocated
Rotorobj        *rotor_ctx      = NULL;
ssc_context     *ssc_ctx        = NULL;
wincrypt_context *wincrypt_ctx  = NULL;
cunprot_context *cunprot_ctx    = NULL;
u32             *zipcrypto_ctx  = NULL;
u32             *threeway_ctx   = NULL;
void            *skipjack_ctx   = NULL;
ANUBISstruct    *anubis_ctx     = NULL;
aria_ctx_t      *aria_ctx       = NULL;
u32             *crypton_ctx    = NULL;
u32             *frog_ctx       = NULL;
gost_ctx_t      *gost_ctx       = NULL;
int             lucifer_ctx     = 0;
u32             *mars_ctx       = NULL;
u32             *misty1_ctx     = NULL;
NOEKEONstruct   *noekeon_ctx    = NULL;
seal_ctx_t      *seal_ctx       = NULL;
safer_key_t     *safer_ctx      = NULL;
int             kirk_ctx        = -1;
u8              *pc1_128_ctx    = NULL;
u8              *pc1_256_ctx    = NULL;
#ifndef DISABLE_MCRYPT
    MCRYPT      mcrypt_ctx      = NULL;
#endif
#ifndef DISABLE_TOMCRYPT
    typedef struct {
        int     idx;
        int     cipher;
        int     hash;
        u8      *key;
        int     keysz;
        u8      *ivec;      // allocated
        int     ivecsz;
        u8      *nonce;     // allocated
        int     noncelen;
        u8      *header;    // allocated
        int     headerlen;
        u8      *tweak;     // allocated
    } TOMCRYPT;
    TOMCRYPT    *tomcrypt_ctx   = NULL;
#endif
crc_context     *crc_ctx        = NULL;
u8              *execute_ctx    = NULL;
u8              *calldll_ctx    = NULL;
FILE    *listfd                 = NULL;
int     bms_line_number         = 0,
        extracted_files         = 0,
        reimported_files        = 0,
        endian                  = MYLITTLE_ENDIAN,
        list_only               = 0,
        force_overwrite         = 0,
        force_rename            = 0,
        verbose                 = 0,
        quick_gui_exit          = 0,
        compression_type        = COMP_ZLIB,
        *file_xor_pos           = NULL,
        file_xor_size           = 0,
        *file_rot13_pos         = NULL,
        file_rot13_size         = 0,
        *file_crypt_pos         = NULL,
        file_crypt_size         = 0,
        comtype_dictionary_len  = 0,
        comtype_scan            = 0,
        encrypt_mode            = 0,
        append_mode             = 0,
        temporary_file_used     = 0,
        quickbms_version        = 0,
        decimal_notation        = 1,    // myitoa is a bit slower (due to the %/) but is better for some strings+num combinations
        mex_default             = 0,
        write_mode              = 0,
        input_total_files       = 0,
        endian_killer           = 0,
        void_dump               = 0,
        reimport                = 0,
        enable_hexhtml          = 0,
        continue_anyway         = 0,
        g_yes                   = 0,
        g_int3                  = 0,
        g_is_gui                = 0;
        //min_int               = 1 << ((sizeof(int) << 3) - 1),
        //max_int               = (u_int)(1 << ((sizeof(int) << 3) - 1)) - 1;
u8      current_folder[PATHSZ + 1] = "",  // just the current folder when the program is launched
        bms_folder[PATHSZ + 1]  = "",
        exe_folder[PATHSZ + 1]  = "",
        file_folder[PATHSZ + 1] = "",
        temp_folder[PATHSZ + 1] = "",
        *output_folder          = NULL,     // points to fdir
        **filter_files          = NULL,     // the wildcard
        **filter_in_files       = NULL,     // the wildcard
        *file_xor               = NULL,     // contains all the XOR numbers
        *file_rot13             = NULL,     // contains all the rot13 numbers
        *file_crypt             = NULL,     // nothing
        *comtype_dictionary     = NULL,
        *quickbms_execute_file  = NULL,
        *g_force_output         = NULL;
int     EXTRCNT_idx             = 0,
        BytesRead_idx           = 0,
        NotEOF_idx              = 0,
        SOF_idx                 = 0,
        EOF_idx                 = 0;



// experimental input and output
int     enable_sockets          = 0,
        enable_process          = 0,
        enable_audio            = 0,
        enable_video            = 0,
        enable_winmsg           = 0,
        enable_calldll          = 0,
        enable_execute_pipe     = 0;



#ifdef WIN32
    OSVERSIONINFO   g_osver = {0};
#endif



//#pragma pack()

