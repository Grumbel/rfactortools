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

#define _WIN32_WINNT    0x0601
#define _WIN32_WINDOWS  0x0601
#define WINVER          0x0601

//#define NOLFS
#ifndef NOLFS   // 64 bit file support not really needed since the tool uses signed 32 bits at the moment, anyway I leave it enabled
    #define _LARGE_FILES        // if it's not supported the tool will work
    #define __USE_LARGEFILE64   // without support for large files
    #define __USE_FILE_OFFSET64
    #define _LARGEFILE_SOURCE
    #define _LARGEFILE64_SOURCE
    #define _FILE_OFFSET_BITS   64
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <inttypes.h>
#include <locale.h>
#include <fcntl.h>
#include "stristr.c"

#include "xalloc.h"

// disabled by default because there are some things that don't convince me
// for example during the disassembling of shellcode_Alpha2.txt
#ifdef ENABLE_BEAENGINE
    #define BEA_ENGINE_STATIC
    #define BEA_USE_STDCALL
    #include <BeaEngine.h>
#endif

//typedef int8_t      i8;
typedef uint8_t     u8;
//typedef int16_t     i16;
typedef uint16_t    u16;
typedef int32_t     i32;
typedef uint32_t    u32;
//typedef int64_t     i64;
typedef uint64_t    u64;

typedef int8_t      int8;
typedef uint8_t     uint8;
typedef int16_t     int16;
typedef uint16_t    uint16;
typedef int32_t     int32;
typedef uint32_t    uint32;
typedef int64_t     int64;
typedef uint64_t    uint64;
typedef unsigned char   byte;   // for sflcomp
typedef unsigned short  word;   // for sflcomp

#define QUICKBMS
// in case you want to make QuickBMS 64bit compatible
// start
#ifdef QUICKBMS64
    #define INTSZ           64
    #define QUICKBMS_int    int64_t     // trick for forcing the usage of signed 32 bit numbers on any system without modifying the code
    #define QUICKBMS_u_int  uint64_t    // used only in some rare occasions
    #define PRId            PRId64
    #define PRIu            PRIu64
    #define PRIx            "016"PRIx64
#else
    #define INTSZ           32
    #define QUICKBMS_int    int32_t     // trick for forcing the usage of signed 32 bit numbers on any system without modifying the code
    #define QUICKBMS_u_int  uint32_t    // used only in some rare occasions
    #define PRId            PRId32
    #define PRIu            PRIu32
    #define PRIx            "08"PRIx32
#endif
// end

#include <zlib.h>
#include <bzlib.h>
#ifndef DISABLE_UCL     // add -DDISABLE_UCL at compiling if you don't have UCL
    #include <ucl/ucl.h>
#endif
#ifndef DISABLE_LZO     // add -DDISABLE_LZO at compiling if you don't have LZO
    #include <lzo/lzo1.h>
    #include <lzo/lzo1a.h>
    #include <lzo/lzo1b.h>
    #include <lzo/lzo1c.h>
    #include <lzo/lzo1f.h>
    #include <lzo/lzo1x.h>
    #include <lzo/lzo1y.h>
    #include <lzo/lzo1z.h>
    #include <lzo/lzo2a.h>
#endif
#include "compression/blast.h"
#include "compression/sflcomp.h"
#include "libs/lzma/LzmaDec.h"
#include "libs/lzma/Lzma2Dec.h"
#include "libs/lzma/Bra.h"
#include "libs/lzma/LzmaEnc.h"

// or use -DDISABLE_SSL
#ifndef DISABLE_SSL
    // it's useless to enable the following
    //#define OPENSSL_DOING_MAKEDEPEND
    //#define OPENSSL_NO_KRB5
    #include <openssl/evp.h>
    #include <openssl/aes.h>
    #include <openssl/blowfish.h>
#endif
#include "encryption/tea.h"
#include "encryption/xtea.h"
#include "encryption/xxtea.h"
#include "myenc.h"
#include "encryption/twofish.h"
#include "encryption/seed.h"
#include "encryption/serpent.h"
#include "encryption/ice.h"
#include "encryption/rotor.c"
//#include "encryption/libkirk/kirk_engine.h"
int kirk_CMD0(u8* outbuff, u8* inbuff, int size, int generate_trash);
int kirk_CMD1(u8* outbuff, u8* inbuff, int size, int do_check);
int kirk_CMD4(u8* outbuff, u8* inbuff, int size);
int kirk_CMD7(u8* outbuff, u8* inbuff, int size);
int kirk_CMD10(u8* inbuff, int insize);
int kirk_CMD11(u8* outbuff, u8* inbuff, int size);
int kirk_CMD14(u8* outbuff, int size);
int kirk_init(); //CMD 0xF?
void xtea_crypt_ecb( xtea_context *ctx, int mode, u8 input[8], u8 output[8] );
#ifndef DISABLE_MCRYPT
    #include <mcrypt.h>
#endif
//#define DISABLE_TOMCRYPT    // useless at the moment
#ifndef DISABLE_TOMCRYPT
    #include <tomcrypt.h>
#endif
#include "encryption/zipcrypto.h"
int threeway_setkey(unsigned *key, unsigned char *data, int datalen);
void threeway_encrypt(unsigned *key, unsigned char *data, int datalen);
void threeway_decrypt(unsigned *key, unsigned char *data, int datalen);
void skipjack_makeKey(byte key[10], byte tab[10][256]);
void skipjack_encrypt(byte tab[10][256], byte in[8], byte out[8]);
void skipjack_decrypt(byte tab[10][256], byte in[8], byte out[8]);
#include "encryption/anubis.h"
typedef struct { Byte rk[16*17]; int Nr; } aria_ctx_t;
int ARIA_DecKeySetup(const Byte *mk, Byte *rk, int keyBits);
int ARIA_EncKeySetup(const Byte *mk, Byte *rk, int keyBits);
void ARIA_Crypt(const Byte *i, int Nr, const Byte *rk, Byte *o);
u_int *crypton_set_key(const u_int in_key[], const u_int key_len, u_int l_key[104]);
u_int crypton_encrypt(const u_int in_blk[4], u_int out_blk[4], u_int l_key[104]);
u_int crypton_decrypt(const u_int in_blk[4], u_int out_blk[4], u_int l_key[104]);
u_int *frog_set_key(const u_int in_key[], const u_int key_len);
void frog_encrypt(const u_int in_blk[4], u_int out_blk[4]);
void frog_decrypt(const u_int in_blk[4], u_int out_blk[4]);
typedef struct { u_int iv[2]; u_int key[8]; int type; } gost_ctx_t;
void gost_kboxinit(void);
void gostcrypt(u_int const in[2], u_int out[2], u_int const key[8]);
void gostdecrypt(u_int const in[2], u_int out[2], u_int const key[8]);
void gostofb(u_int const *in, u_int *out, int len, u_int const iv[2], u_int const key[8]);
void gostcfbencrypt(u_int const *in, u_int *out, int len, u_int iv[2], u_int const key[8]);
void gostcfbdecrypt(u_int const *in, u_int *out, int len, u_int iv[2], u_int const key[8]);
void lucifer(unsigned char *);
void lucifer_loadkey(unsigned char *, int);
u_int *mars_set_key(u_int key_blk[], u_int key_len);
void mars_encrypt(u_int in_blk[], u_int out_blk[]);
void mars_decrypt(u_int in_blk[], u_int out_blk[]);
void misty1_keyinit(u_int  *ek, u_int  *k);
void misty1_decrypt_block(u_int  *ek,u_int  c[2], u_int  p[2]);
void misty1_encrypt_block(u_int  *ek, u_int  p[2], u_int  c[2]);
typedef struct { u_int k[4]; } NOEKEONstruct;
void NOEKEONkeysetup(const unsigned char * const key, 
                    NOEKEONstruct * const structpointer);
void NOEKEONencrypt(const NOEKEONstruct * const structpointer, 
                   const unsigned char * const plaintext,
                   unsigned char * const ciphertext);
void NOEKEONdecrypt(const NOEKEONstruct * const structpointer,
                   const unsigned char * const ciphertext,
                   unsigned char * const plaintext);
#include "encryption/seal.h"
#include "encryption/safer.h"
#include "encryption/pc1.h"

#ifdef __DJGPP__
    #define NOLFS
    char **__crt0_glob_function (char *arg) { return 0; }
    void   __crt0_load_environment_file (char *progname) { }
#endif

#ifdef WIN32
    #include <windows.h>
    //#include <psapi.h>
    //#include <shlobj.h>
    //#include <tlhelp32.h>
    #include <wincrypt.h>
    #include <direct.h>
    //#include <ddk/ntifs.h>    // I want compatibility even with Win9x
    #include "extra/MemoryModule.h"
    #include "extra/backtrace.c"

    #define PATHSLASH   '\\'
    #define make_dir(x) mkdir(x)
    #define LOADDLL(X)  LoadLibrary(X)
    #define GETFUNC(X)  (void *)GetProcAddress(hlib, X)
    #define CLOSEDLL    FreeLibrary(hlib);

    char *get_file(char *title, i32 bms, i32 multi);
    char *get_folder(char *title);
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <dlfcn.h>      // -ldl
    #include <sys/mman.h>

    #define LOADDLL(X)  dlopen(X, RTLD_LAZY)
    #define GETFUNC(X)  (void *)dlsym(hlib, X)
    #define CLOSEDLL    dlclose(hlib);
    #define HMODULE     void *
    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
    //#define stristr     strcasestr
    #define GetCurrentProcessId getpid
    #define PATHSLASH   '/'
    #define make_dir(x) mkdir(x, 0755)
    #define USE_LIBICONV    // -liconv
#endif

#if defined(_LARGE_FILES)
    #if defined(__APPLE__)
        #define fseek   fseeko
        #define ftell   ftello
    #elif defined(__FreeBSD__)
    #elif !defined(NOLFS)       // use -DNOLFS if this tool can't be compiled on your OS!
        #define off_t   off64_t
        #define fopen   fopen64
        #define fseek   fseeko64
        #define ftell   ftello64
        #ifndef fstat
            #ifdef WIN32
                #define fstat   _fstati64
                #define stat    _stati64
            #else
                #define fstat   fstat64
                #define stat    stat64
            #endif
        #endif
    #endif
#endif

# ifndef __cdecl 
#  define __cdecl  __attribute__ ((__cdecl__))
# endif
# ifndef __stdcall
#  define __stdcall __attribute__ ((__stdcall__))
# endif
void __cxa_pure_virtual() { while(1); }

#include "threads.h"



#define VER             "0.5.29"
#define BUFFSZ          8192
#define MAX_ARGS        16          // fixed but exagerated
#define MAX_VARS        1024        // fixed but exagerated (name/value_static gives problems with allocated variables)
#define MAX_FILES       1024        // fixed but exagerated
#define MAX_CMDS        4096        // fixed but exagerated
#define MAX_ARRAYS      1024        // fixed but exagerated

#define STRINGSZ        273         // more than MAX_PATH, aligned with +1, 273*15+1 = 4096
#define VAR_NAMESZ      STRINGSZ    // 31          // +1 for alignment, 31 for a variable name is perfect
#define VAR_VALUESZ     STRINGSZ    // more than 256 and big enough to contain filenames
#define NUMBERSZ        24          // ready for 64 bits, includes also space for the NULL delimiter
#define PATHSZ          1024        // 257 was enough, theoretically the system could support 32kb but it's false
#define MULTI_PATHSZ    32768       // 32k limit ansi, no limit unicode
#define ENABLE_DIRECT_COPY

#if VAR_NAMESZ < NUMBERSZ
ERROR VAR_NAMESZ < NUMBERSZ
#endif

#define MYLITTLE_ENDIAN 0
#define MYBIG_ENDIAN    1

#define int             QUICKBMS_int
#define u_int           QUICKBMS_u_int

#define QUICKBMS_DUMMY  "QUICKBMS_DUMMY_TEMP"
#define CMD             command[cmd]
#define ARG             argument
#define NUM(X)          CMD.num[X]
#define STR(X)          CMD.str[X]
#define VARISNUM(X)     variable[CMD.var[X]].isnum
#define VARNAME(X)      get_varname(CMD.var[X])
#define VAR(X)          get_var(CMD.var[X])
#define VAR32(X)        get_var32(CMD.var[X])
#define VARSZ(X)        variable[CMD.var[X]].size   // due to the memory enhancement done on this tool, VARSZ returns ever STRINGSZ for sizes lower than this value... so do NOT trust this value!
//#define FILEZ(X)        ((NUM(X) < 0) ? NULL : filenumber[NUM(X)].fd)  // automatic support for MEMORY_FILE
#define DIRECT_ADDVAR(X,Y,Z) \
                        variable[CMD.var[X]].value   = Y; \
                        variable[CMD.var[X]].value32 = 0; \
                        variable[CMD.var[X]].isnum   = 0; \
                        variable[CMD.var[X]].size    = Z;
#define FILEZ(X)        NUM(X)
#define MEMORY_FNAME    "MEMORY_FILE"
#define MEMORY_FNAMESZ  (sizeof(MEMORY_FNAME) - 1)
#define TEMPORARY_FILE  "TEMPORARY_FILE"
#define ALLOC_ERR       alloc_err(__FILE__, __LINE__, __FUNCTION__)
#define STD_ERR(ERR)    std_err(__FILE__, __LINE__, __FUNCTION__, ERR)

#define FREEZ(X)        if(X) { \
                            free(X); \
                            X = NULL; \
                        }
#define FREEX(X,Y)      if(X) { \
                            Y; \
                            free(X); \
                            X = NULL; \
                        }
#define CHECK_FILENUM   if( \
                            !filenumber[fdnum].fd && \
                            !filenumber[fdnum].sd && \
                            !filenumber[fdnum].pd && \
                            !filenumber[fdnum].ad && \
                            !filenumber[fdnum].vd && \
                            !filenumber[fdnum].md \
                        ) { \
                            fprintf(stderr, "\nError: the specified file number (%d) has not been opened yet (line %d)\n", (i32)fdnum, (i32)__LINE__); \
                            myexit(QUICKBMS_ERROR_BMS); \
                        }
#define myatoi(X)       readbase(X, 10, NULL)
#define CSTRING(X,Y)    { \
                        mystrdup(&CMD.str[X], Y); \
                        CMD.num[X] = cstring(CMD.str[X], CMD.str[X], -1, NULL); \
                        }
#define QUICK_GETi32(X,Y)   ((X[Y]) | (X[Y+1] << 8) | (X[Y+2] << 16) | (X[Y+3] << 24))

// numbers_to_bytes returns a static buffer so do NOT free it
// NUMS2BYTES(input, input_size, output, output_size)
#define NUMS2BYTES(A,B,C,D) { \
                        tmp = numbers_to_bytes(A, &B, 0); \
                        myalloc(&C, B, &D); \
                        memcpy(C, tmp, B); \
                        }
#define NUMS2BYTES_HEX(A,B,C,D) { \
                        tmp = numbers_to_bytes(A, &B, 1); \
                        myalloc(&C, B, &D); \
                        memcpy(C, tmp, B); \
                        }

#define MULTISTATIC     256 // this number is simply the amount of static buffers to use so that
                            // we can use the same function MULTISTATIC times without overlapped results!
#define strdup_dontuse  "Error: do NOT use strdup, use re_strdup or mystrdup!"
#define strdup          strdup_dontuse
#define far
//#define PRINTF64(X)     (i32)(((X) >> 32) & 0xffffffff), (i32)((X) & 0xffffffff)



#include "defs.h"



int debug_privileges(void);
int verbose_options(u8 *arg);
u8 *mystrdup_simple(u8 *str);
u8 *mystrdup(u8 **old_buff, u8 *str);
void show_dump(int left, u8 *data, int len, FILE *stream);
int get_parameter_numbers(u8 *str, ...);
QUICKBMS_int readbase(u8 *data, QUICKBMS_int size, QUICKBMS_int *readn);
void mex_default_init(int file_only);
int start_bms(int startcmd, int nop, int *ret_break);
int check_wildcard(u8 *fname, u8 *wildcard);
int check_wildcards(u8 *fname, u8 **list);
u8 *create_dir(u8 *fname, int mdir, int cdir, int is_path, int filter_bad);
int check_overwrite(u8 *fname, int check_if_present_only);
u8 *myalloc(u8 **data, QUICKBMS_int wantsize, QUICKBMS_int *currsize);
void std_err(const char *fname, i32 line, const char *func, signed char error);
void winerr(DWORD error, char *msg);
void myexit(int ret);



// boring 64bit compatibility
#undef int
#undef u_int
#if QUICKBMS_int != 32
    u8 *myalloc32(u8 **data, int wantsize, int *currsize) {
        QUICKBMS_int    lame;
        if(!currsize) {
            myalloc(data, wantsize, NULL);
        } else {
            lame = *currsize;
            myalloc(data, wantsize, &lame);
            *currsize = lame;
        }
        return(*data);
    }
    #define myalloc myalloc32
#endif
#include "sign_ext.h"
#include "unz.h"
#include "extra/wcx.c"
#include "extra/window.h"
#include "io/sockets.h"
#include "io/process.h"
#include "io/audio.h"
#include "io/video.h"
#include "io/winmsg.h"
#undef myalloc
#define MAINPROG
#include "disasm/disasm.h"
typedef struct t_asmmodel {            // Model to search for assembler command
  uchar          code[MAXCMDSIZE];     // Binary code
  uchar          mask[MAXCMDSIZE];     // Mask for binary code (0: bit ignored)
  int            length;               // Length of code, bytes (0: empty)
  int            jmpsize;              // Offset size if relative jump
  int            jmpoffset;            // Offset relative to IP
  int            jmppos;               // Position of jump offset in command
} t_asmmodel;
int    Assemble(uchar *cmd,ulong ip,t_asmmodel *model,int attempt,
         int constsize,uchar *errtext);
// restore int and u_int after main()



#define int             QUICKBMS_int
#define u_int           QUICKBMS_u_int



#include "utils.h"
#include "var.h"
#include "perform.h"
#include "hexhtml.h"
#include "file.h"
#include "cmd.h"
#include "bms.h"
#include "update.h"



int verbose_options(u8 *arg) {
    u8      *p;

    if(!arg || (strlen(arg) < 2)) return(-1);
    if(((arg[0] != '-') && (arg[0] != '/')) || (strlen(arg) != 2)) {
        for(p = arg; (*p == '-') || (*p == '/'); p++);
             if(!stricmp(p, "help"))        strcpy(arg, "-h");
        else if(!stricmp(p, "commands"))    strcpy(arg, "-c");
        else if(!stricmp(p, "filter"))      strcpy(arg, "-f");
        else if(!stricmp(p, "list"))        strcpy(arg, "-l");
        else if(!stricmp(p, "verbose"))     strcpy(arg, "-v");
        else if(!stricmp(p, "debug"))       strcpy(arg, "-V");
        else if(!stricmp(p, "listfile"))    strcpy(arg, "-L");
        else if(!stricmp(p, "hex"))         strcpy(arg, "-x");
        else if(!stricmp(p, "write"))       strcpy(arg, "-w");
        else if(!stricmp(p, "endian"))      strcpy(arg, "-E");
        else if(!stricmp(p, "void"))        strcpy(arg, "-0");
        else if(!stricmp(p, "reimport"))    strcpy(arg, "-r");
        else if(!stricmp(p, "reinject"))    strcpy(arg, "-r");
        else if(!stricmp(p, "sockets"))     strcpy(arg, "-n");
        else if(!stricmp(p, "network"))     strcpy(arg, "-n");
        else if(!stricmp(p, "process"))     strcpy(arg, "-p");
        else if(!stricmp(p, "audio"))       strcpy(arg, "-A");
        else if(!stricmp(p, "video"))       strcpy(arg, "-g");
        else if(!stricmp(p, "winmsg"))      strcpy(arg, "-m");
        else if(!stricmp(p, "calldll"))     strcpy(arg, "-C");
        else if(!stricmp(p, "hex_html"))    strcpy(arg, "-H");
        else if(!stricmp(p, "hex_console")) strcpy(arg, "-X");
        else if(!stricmp(p, "update"))      strcpy(arg, "-u");
        else if(!stricmp(p, "continue"))    strcpy(arg, "-.");
        else if(!stricmp(p, "continue_anyway"))     strcpy(arg, "-.");
        else if(!stricmp(p, "XDBG_ALLOC_ACTIVE"))   strcpy(arg, "-9");
        else if(!stricmp(p, "XDBG_ALLOC_INDEX"))    strcpy(arg, "-8");
        else if(!stricmp(p, "XDBG_ALLOC_VERBOSE"))  strcpy(arg, "-7");
        else if(!stricmp(p, "XDBG_HEAPVALIDATE"))   strcpy(arg, "-6");
        else if(!stricmp(p, "gui"))         strcpy(arg, "-G");
        else return(-1);
    }
    return(0);
}



void myhelp(u8 *argv0) {
    printf("\n"
        "Usage: %s [options] <script.BMS> <input_archive/folder> [output_folder]\n"
        "\n"
        "Options:\n"
        "-l     list the files without extracting them\n"
        "-f W   filter the files to extract using the W wildcards separated by comma or\n"
        "       semicolon, example -f \"*.mp3,*.txt;*myname*\"\n"
        "       if the filter starts with ! it's considered an ignore filter\n"
        "       the filter can be also a text file containing filters\n"
        "       example: quickbms -f \"*.mp3;!*.ogg\" script.bms archive.dat output\n"
        "       example: quickbms -f myfilters_list.txt script.bms archive.dat\n"
        "       please use {} instead of * to avoid problems on Windows\n"
        "-F W   as above but works only with the files in the input folder (if used)\n"
        "       example: quickbms -F \"*.dat\" script.bms input_folder output_folder\n"
        "-o     if the output files already exist this option will overwrite them\n"
        "       automatically without asking the user confirmation\n"
        "-k     keep the current files if already exist without asking (skip all)\n"
        "-r     experimental reimport option that should work with many archives:\n"
        "         quickbms script.bms archive.pak output_folder\n"
        "         modify the needed files in output_folder and maybe remove the others\n"
        "         quickbms -w -r script.bms archive.pak output_folder\n"
        "       you must read section 3 of quickbms.txt before using this feature\n"
        "-s SF  add a script file or command before the execution of the input script,\n"
        "       useful if an archive uses a different endianess or encryption and so on\n"
        "       SF can be a script or directly the bms instruction you want to execute\n"
        "-u     check if there is a new version of QuickBMS available\n"
        "-.     don't terminate QuickBMS if there is an error while parsing multiple\n"
        "       files (like wrong compression or small file), just continue with the\n"
        "       other files in the folder\n"
        "\n"
        "Advanced options:\n"
        "-d     automatically create an additional output folder with the name of the\n"
        "       input folder and file processed, eg. models/mychar/mychar.arc/FILES,\n"
        "       -d works also if input and output folders are the same (rename folder)\n"
        "-D     similar to -d but will not create the folder with the filename\n"
        "-E     experimental option for automatically reversing the endianess of any\n"
        "       file simply reading it field by field (so each get will produce a put)\n"
        "-c     quick list of the basic BMS commands and some notes about this tool\n"
        "-S CMD execute the command CMD on each file extracted, you must specify the\n"
        "       #INPUT# placeholder which will be replaced by the name of the file\n"
        "       example: -S \"lame -b 192 -t --quiet #INPUT#\"\n"
        "-Y     automatically answer yes to any question\n"
        "-O F   redirect the output of all the extracted files to the file F\n"
        "\n"
        "Debug and experimental options:\n"
        "-v     verbose debug information, useful for verifying possible errors\n"
        "-V     alternative verbose output, useful for programmers\n"
        "-L F   dump the offset/size/name of the files inside the file F\n"
        "-x     use the hexadecimal notation in myitoa (debug)\n"
        "-0     no extraction of files, useful for testing a script without using space\n"
        "-R     needed for the programs that act as interface for QuickBMS\n"
        "-a S   pass arguments to the input script like quickbms_arg1, 2, 3 and so on\n"
        "-H     cool HTML hex viewer output, use it only with very small files!\n"
        //"       T can be -1, 1, 2 or 3 and is a different type of output\n"
        "-X     cool hex viewer output on the console (support Less-like keys)\n"
        "-9     toggle XDBG_ALLOC_ACTIVE  (%s)\n"
        "-8     toggle XDBG_ALLOC_INDEX   (%s)\n"
        "-7     toggle XDBG_ALLOC_VERBOSE (%s)\n"
        "-6     toggle XDBG_HEAPVALIDATE  (%s)\n"
        "-3     execute an INT3 before each CallDll, compression and encryption\n"
        "\n"
        "Features and security activation options:\n"
        "-w     enable the write mode required to write physical input files with Put*\n"
        "-C     enable the usage of CallDll without asking permission\n"
        "-n     enable the usage of network sockets\n"
        "-p     enable the usage of processes\n"
        "-A     enable the usage of audio device\n"
        "-g     enable the usage of video graphic device\n"
        "-m     enable the usage of Windows messages\n"
        "-G     enable the GUI mode on Windows\n"
        "\n"
        "Examples:\n"
        "  quickbms c:\\zip.bms c:\\myfile.zip \"c:\\new folder\"\n"
        "  quickbms -l -f \"*.txt\" zip.bms myfile.zip\n"
        "  quickbms -F \"*.bff\" c:\\nfsshift.bms c:\\Shift\\Pakfiles c:\\output\n"
        "\n"
        "Check quickbms.txt for the manual and the list of additional credits.\n"
        "\n", argv0,
        XDBG_ALLOC_ACTIVE  ? "enabled" : "disabled",
        XDBG_ALLOC_INDEX   ? "enabled" : "disabled",
        XDBG_ALLOC_VERBOSE ? "enabled" : "disabled",
        XDBG_HEAPVALIDATE  ? "enabled" : "disabled");
}



void quick_bms_list(void) {
    fputs("\n"
        "quick reference list of the BMS commands:\n"
        "\n"
        " CLog <filename> <offset> <compressed_size> <uncompressed_size> [file]\n"
        "    extract the file at give offset decompressing its content\n"
        "\n"
        " Do\n"
        " ...\n"
        " While <variable> <condition> <variable>\n"
        "    perform a loop which ends when the condition is no longer valid\n"
        "\n"
        " FindLoc <variable> <type> <string> [file] [ret_if_err]\n"
        "    if the string is found put its offset in the variable\n"
        "    by default if FindLoc doesn't find the string it terminates the script\n"
        "    while if ret_if_err is specified (for example -1 or \"\") it will be put in\n"
        "    variable instead of terminating\n"
        "\n"
        " For [variable] = [value] [To] [variable]\n"
        " ...\n"
        " Next [variable]\n"
        "    classical for(;;) loop, Next simply increments the value of the variable\n"
        "    the arguments are optionals for using this For like an endless loop and\n"
        "    To can be substituited with any condition like != == < <= > >= and so on\n"
        "\n"
        " Break\n"
        "    quit a loop (experimental)\n"
        "\n"
        " Get <variable> <type> [file]\n"
        "    read a number (8, 16, 32 bits) or a string\n"
        "\n"
        " GetDString <variable> <length> [file]\n"
        "    read a string of the specified length\n"
        "\n"
        " GoTo <offset> [file]\n"
        "    reach the specified offset, if it's negative it goes from the end\n"
        "\n"
        " IDString [file] <string>\n"
        "    check if the data in the file matches the given string\n"
        "\n"
        " Log <filename> <offset> <size> [file]\n"
        "    extract the file at the given offset with that size\n"
        "\n"
        " Math <variable> <operator> <variable>\n"
        "    perform a mathematical operation on the first variable, available op:\n"
        "    + * / - ^ & | % ! ~ << >> r (rot right) l (rot left) s (bit s) w (byte s)\n"
        "\n"
        " Open <folder> <filename> <file>\n"
        "    open a new file and assign the given file number\n"
        "\n"
        " SavePos <variable> [file]\n"
        "    save the current offset in the variable\n"
        "\n"
        " Set <variable> [type] <variable>\n"
        "    assign the content of the second variable to the first one, type ignored\n"
        "\n"
        " String <variable> <operator> <variable>\n"
        "    perform an append/removing/xor operation on the first variable\n"
        "\n"
        " CleanExit\n"
        "    terminate the extraction\n"
        "\n"
        " If <variable> <criterium> <variable>\n"
        " ...\n"
        " Else / Elif / Else If\n"
        " ...\n"
        " EndIf\n"
        "    classical if(...) { ... } else if { ... } else { ... }\n"
        "\n"
        " GetCT <variable> <type> <character> [file]\n"
        "    read a string (type is useless) delimited by the given character\n"
        "\n"
        " ComType <type> [dictionary]\n"
        "    specify the type of compression to use in CLog: quickbms.txt for the list\n"
        "\n"
        " ReverseLong <variable>\n"
        "    invert the order/endianess of the variable\n"
        "\n"
        " Endian <type>\n"
        "    choose between little and big endian order of the read numbers\n"
        "\n"
        " FileXOR <string_of_numbers> [offset]\n"
        "    xor the read data with the sequence of numbers in the given string\n"
        "\n"
        " FileRot13 <string_of_numbers> [offset]\n"
        "    add/substract the read data with the sequence of numbers in the string\n"
        "\n"
        " Strlen <variable> <variable>\n"
        "    put the length of the second variable in the first one\n"
        "\n"
        " GetVarChr <variable> <variable> <offset> [type]\n"
        "    put the byte at the given offset of the second variable in the first one\n"
        "\n"
        " PutVarChr <variable> <offset> <variable> [type]\n"
        "    put the byte in the second variable in the first one at the given offset\n"
        "\n"
        " Padding <number> [file]\n"
        "    adjust the current offset of the file using the specified number (size of\n"
        "    padding), note that at the moment the padding is performed only when\n"
        "    this command is called and not automatically after each file reading\n"
        "\n"
        " Append\n"
        "    enable/disable the writing of the data at the end of the files with *Log\n"
        "\n"
        " Encryption <algorithm> <key> [ivec] [mode] [keylen]\n"
        "    enable that type of decryption: quickbms.txt for the list\n"
        "\n"
        " Print \"message\"\n"
        "    display a message, you can display the content of the variables simply\n"
        "    specifying their name between '%' like: Print \"my offset is %OFFSET%\"\n"
        "\n"
        " GetArray <variable> <array_num> <index>\n"
        "    get the value stored at the index position of array_num\n"
        "\n"
        " PutArray <array_num> <index> <variable>\n"
        "    store the variable at the index position of array_num\n"
        "\n"
        " StartFunction NAME\n"
        " ...\n"
        " EndFunction\n"
        " CallFunction NAME\n"
        "    experimental functions for recursive archives\n"
        "\n"
        "Refer to quickbms.txt for the rest of the commands and their details!\n"
        "\n"
        "NOTES:\n"
        "- a variable and a fixed number are the same thing internally in the tool\n"
        "  because all the data is handled as strings with the consequent pros\n"
        "  (incredibly versatile) and cons (slowness with some types of scripts)\n"
        "- everything is case insensitive (ABC is like abc) except the content of\n"
        "  strings and variables (excluded some operations like in String)\n"
        "- the [file] field is optional, if not specified it's 0 so the main file\n"
        "- also the final ';' char of the original BMS language is optional\n"
        "- example of <string_of_numbers>: \"0x123 123 456 -12 -0x7f\" or 0xff or \"\"\n"
        "- both hexadecimal (0x) and decimal numbers are supported, negatives included\n"
        "- all the mathematical operations are performed using signed 32 bit numbers\n"
        "- available types of data: long (32 bits), short (16), byte (8), string\n"
        "- all the fixed strings are handled in C syntax like \"\\x12\\x34\\\\hello\\\"bye\\0\"\n"
        "- do not use variable names which start with a number like 123MYVAR or -MYVAR\n"
        "- if you use the file MEMORY_FILE will be used a special memory buffer, create\n"
        "  it with CLog or Log and use it normally like any other file\n"
        "- is possible to use multiple memory files: MEMORY_FILE, MEMORY_FILE2,\n"
        "  MEMORY_FILE3, MEMORY_FILE4 and so on\n"
        "- use TEMPORARY_FILE for creating a file with this exact name also in -l mode\n"
        "\n"
        "information about the original BMS scripting language and original examples:\n"
        "  http://wiki.xentax.com/index.php/BMS\n"
        "  http://multiex.xentax.com/complete_scripts.txt\n"
        "\n"
        "check the source code of this tool for the additional enhancements implemented\n"
        "by me (like support for xor, rot13, lzo, lzss, zlib/deflate and so on) or send\n"
        "me a mail because various features are not documented yet or just watch the\n"
        "examples provided on the project's homepage which cover ALL the enhancements:\n"
        "  http://quickbms.aluigi.org\n"
        "\n"
        "the tool supports also the \"multiex inifile\" commands in case of need.\n"
        "\n", stdout);
}



int set_console_title(u8 *options_db, u8 *bms, u8 *fname) {
#ifdef WIN32
    static  u8  title[1024] = "";
    u8      options[256 + 1];
    int     i,
            len;

    len = 0;
    for(i = 0; i < 256; i++) {
        if(options_db[i]) options[len++] = i;
    }
    options[len] = 0;

    len = snprintf(
        title,
        sizeof(title),
        "%s -%s: %s . %s",
        VER,
        options,
        bms,
        fname);

    if((len < 0) || (len > sizeof(title))) {
        title[sizeof(title) - 1] = 0;
    }

    SetConsoleTitle(title);
#endif
    return(0);
}



int     g_quickbms_exception_test   = -1;
i32     g_quickbms_argc             = 0;
char    **g_quickbms_argv           = NULL;
char    g_quickbms_arg0[PATHSZ + 1] = "";



#ifdef WIN32
// the goal of these functions is avoiding the error dialog box and terminating the process immediately
int show_exceptionrecord(EXCEPTION_RECORD *ExceptionRecord, i32 level) {
    static void *  old_addr = NULL - 1;
    static int  called = 0;
    int     i;
    if(!ExceptionRecord) return -1;
    if((old_addr == ExceptionRecord->ExceptionAddress) || (called >= 10)) {
        // corrupted handler, it happened one time with compression 148
        // yeah I know that it's not a perfect solution
        TerminateProcess(GetCurrentProcess(), 9);
        Sleep(-1);  // it will be killed automatically
    }
    old_addr = ExceptionRecord->ExceptionAddress;
    called++;

    fprintf(stderr, "%.*s*EH* ExceptionCode      %08x\n", level * 4, "", (i32)ExceptionRecord->ExceptionCode);
    fprintf(stderr, "%.*s*EH* ExceptionFlags     %08x\n", level * 4, "", (i32)ExceptionRecord->ExceptionFlags);
    fprintf(stderr, "%.*s*EH* ExceptionAddress   %08x\n", level * 4, "", (i32)ExceptionRecord->ExceptionAddress);
    module_t *module;   // placed here in case of crashes
    module = scan_modules(NULL, GetCurrentProcessId(), NULL, NULL);
    if(module) {
        for(i = 0; module[i].addr; i++) {
            if((ExceptionRecord->ExceptionAddress >= module[i].addr) && (ExceptionRecord->ExceptionAddress < (module[i].addr + module[i].size))) {
                fprintf(stderr, "%.*s                        %p + %08x %s\n", level * 4, "", module[i].addr, (i32)(ExceptionRecord->ExceptionAddress - module[i].addr), module[i].szModule);
            }
        }
    }
    fprintf(stderr, "%.*s*EH* NumberParameters   %08x\n", level * 4, "", (i32)ExceptionRecord->NumberParameters);
    for(i = 0; i < ExceptionRecord->NumberParameters; i++) {
        fprintf(stderr, "%.*s*EH*                    %08x\n", level * 4, "", (i32)ExceptionRecord->ExceptionInformation[i]);
    }
    show_exceptionrecord(ExceptionRecord->ExceptionRecord, level + 1);
    return 0;
}
void exception_handler(EXCEPTION_POINTERS *ExceptionInfo) {
    if(ExceptionInfo && ExceptionInfo->ExceptionRecord && (ExceptionInfo->ExceptionRecord->ExceptionCode <= 0x7fffffff)) {
        return;
    }
    fprintf(stderr,
        "\n"
        "-------------------\n"
        "*EXCEPTION HANDLER*\n"
        "-------------------\n"
        "An error or crash occurred:\n"
        "\n"
    );
    if(ExceptionInfo) {
        show_exceptionrecord(ExceptionInfo->ExceptionRecord, 0);
        /* this hex dump is useless
        u8      *p;
        if(ExceptionInfo->ContextRecord) {
            // alpha, mips, x86, x86_64... show_dump is easier
            // skip the last zeroes to avoid too much data
            for(p = (u8 *)ExceptionInfo->ContextRecord + sizeof(CONTEXT) - sizeof(u32); p >= (u8 *)ExceptionInfo->ContextRecord; p -= sizeof(u32)) {
                if(((u32 *)p)[0]) break;
            }
            show_dump(2, (u8 *)ExceptionInfo->ContextRecord, (p + sizeof(u32)) - (u8 *)ExceptionInfo->ContextRecord /|*sizeof(CONTEXT)*|/, stderr);
        }
        */
        if(ExceptionInfo->ExceptionRecord && (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) && GetModuleHandle("HsSrv")) {
            fprintf(stderr,
                "\n"
                "Probably the crash has been caused by your Asus Xonar/Unixonar drivers.\n"
                "More information and details are available in quickbms.txt\n"
                "Some ways to fix the bug:\n"
                "- disable the GX mode (emulated EAX) of the Asus driver\n"
                "- disable the Asus HookSupport Manager application (HsMgr.exe)\n"
                "- start QuickBMS with the -9 option (create a link)\n"
                "- contact Asus! :)\n"
                "\n");
        }


        // backtrace part

        struct output_buffer ob;
        output_init(&ob, g_backtrace_output, BACKTRACE_BUFFER_MAX);

        if (!SymInitialize(GetCurrentProcess(), 0, TRUE)) {
            output_print(&ob,"Failed to init symbol context\n");
        }
        else {
            bfd_init();
            struct bfd_set *set = calloc(1,sizeof(*set));
            _backtrace(&ob , set , 128 , ExceptionInfo->ContextRecord);
            release_set(set);

            SymCleanup(GetCurrentProcess());
        }

        fputs("\n*EH* Stack Trace:\n", stderr);
        fputs(g_backtrace_output , stderr);

    }

    if(!g_quickbms_exception_test && XDBG_ALLOC_ACTIVE && g_is_gui) {

        // the problem is caused by some programs that read the memory of the other processes
        // when GetOpenFileName is called but they are so dumb to read the data before the
        // allocated memory or the blocks tagged as PAGE_NOACCESS or PAGE_GUARD.

        printf(
            "\n"
            "\n"
            "It seems you have some program running on your system that doesn't allow\n"
            "QuickBMS to run because it reads invalid zones of the memory of this process.\n"
            "This is usually caused by Xonar drivers or some Nvidia software with special\n"
            "options enabled and maybe also some antivirus software.\n"
            "\n"
            "You can bypass the problem by launching QuickBMS with the -9 option by\n"
            "creating a link to quickbms.exe or simply by answering to the following\n"
            "question:\n"
            "\n"
            "- Do you want to launch QuickBMS with the -9 option? (y/N)\n"
            "  ");
        if(get_yesno(NULL) == 'y') {
            // spawnv note: do not enable this (don't know why but doesn't work if you specify the folder): copycut_folder(NULL, g_quickbms_arg0);
            char    *myargv[g_quickbms_argc + 2 + 1]; // 2 = -9 -G
            int32   i;
            for(i = 0; i < g_quickbms_argc; i++) {
                myargv[i] = g_quickbms_argv[i];
            }
            myargv[i++] = "-9";
            if(g_is_gui == 2) myargv[i++] = "-G";   // auto enabled gui mode, we need to consider that the user used -G in his command-line
            myargv[i]   = NULL;
            printf("\n\n\n");
            spawnv(P_NOWAITO, g_quickbms_arg0, (void *)myargv);
            exit(QUICKBMS_ERROR_UNKNOWN);   // yeah, no myexit() because we don't need to wait
        }
    }

    myexit(QUICKBMS_ERROR_UNKNOWN);
}
LONG CALLBACK VectoredHandler(EXCEPTION_POINTERS *ExceptionInfo) {
    exception_handler(ExceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}
LONG WINAPI UnhandledException(EXCEPTION_POINTERS *ExceptionInfo) {
    exception_handler(ExceptionInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif



i32 main(i32 argc, char *argv[]) {
    static u8   filedir[PATHSZ + 1] = ".",  // don't waste the stack
                bckdir[PATHSZ + 1]  = ".";
    files_t *files          = NULL;
    FILE    *fds,
            *pre_fd;
    time_t  benchmark       = 0;
    int     i,
            argi,
            cmd,
            mybreak         = 0,
            curr_file       = 0,
            wcx_plugin      = 0,
            update          = 0,
            quickbms_outname = 0,
            fname_multi_select = 0;
    u8      options_db[256],
            *newdir,
            *bms,
            *fname,
            *fdir           = ".",
            *p,
            *tmp,
            *pre_script     = NULL,
            *listfile       = NULL,
            *quickbms_arg   = NULL,
            *filter_files_tmp = NULL,
            *filter_in_files_tmp = NULL;

#ifdef WIN32
    // useful moreover in future
    g_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&g_osver);

    if(!winapi_missing()) {
        // disabled because it may cause problems with Win8.1
        //SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

        //don't enable: if(_AddVectoredContinueHandler) _AddVectoredContinueHandler(1, VectoredHandler);

        // my original solution
        g_backtrace_output = malloc(BACKTRACE_BUFFER_MAX);
        //if(_AddVectoredExceptionHandler) _AddVectoredExceptionHandler(1, VectoredHandler);
        SetUnhandledExceptionFilter(UnhandledException);
    }
#endif

    //setbuf(stdout, NULL); // should increase the speed with lot of files
    setbuf(stderr, NULL);

    fflush(stdin);  // useless?
    #ifdef O_BINARY
    setmode(fileno(stdin), O_BINARY);
    #endif

    srand(time(NULL));

    //setlocale(LC_ALL, "");    // disabled for the moment

    //xdbg_alloc_extreme();

    fputs("\n"
        "QuickBMS generic files extractor and reimporter "VER
#ifdef QUICKBMS64
        " (64bit test)"
#endif
        "\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "        (" __DATE__ " - " __TIME__ ")\n"
        "\n"
        "                  " "http://quickbms.aluigi.org" "\n"
        "               " "http://twitter.com/luigi_auriemma" "\n"
        "\n", stderr);

#ifdef WIN32
    DWORD   r;
    r = GetModuleFileName(NULL, g_quickbms_arg0, PATHSZ);
    if(!r || (r >= PATHSZ))
#endif
        mystrcpy(g_quickbms_arg0, argv[0], PATHSZ);
    g_quickbms_argc = argc;
    g_quickbms_argv = argv;

#ifdef WIN32
    int check_if_running_from_doubleclick(void) {
        // -1 = error
        // 0  = console
        // 1  = gui/double-click

        if(g_osver.dwMajorVersion > 4) {
            // this method is very easy and works well, tested on XP/2003/win7/win8
            // doesn't work with win98
            if(GetWindowLong(GetForegroundWindow(), GWL_WNDPROC)) {
                return 1;
            }
            return 0;
        }

        // for Win98 only
        int     ret = -1;
        DWORD pid = GetCurrentProcessId();
        if(pid) {
            HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if(h != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32  pe, pp;
                pe.dwSize = sizeof(PROCESSENTRY32);
                if(Process32First(h, &pe)) { do {
                    if(pe.th32ProcessID == pid) {
                        pp.dwSize = sizeof(PROCESSENTRY32);
                        if(Process32First(h, &pp)) { do {
                            if(pp.th32ProcessID == pe.th32ParentProcessID) {
                                     if(!stricmp(get_filename(pp.szExeFile), "winoa386.mod")) ret = 0;
                                else if(!stricmp(get_filename(pp.szExeFile), "cmd.exe"))      ret = 0;
                                else ret = 1;    // found but no command.com, so it's probably explorer.exe
                                break;
                            }
                        } while(Process32Next(h, &pp)); }
                        break;
                    }
                } while(Process32Next(h, &pe)); }
                CloseHandle(h);
            }
        }
        return ret;
    }

    // necessary to handle the GUI and the secure memory
    // before the execption handler that may be used
    // in some situations on Win8.1 and bugged software

    for(i = 1; i < argc; i++) {
        if(verbose_options(argv[i]) < 0) break;
        switch(argv[i][1]) {
            case 'f': i++;  break;
            case 'F': i++;  break;
            case 'L': i++;  break;
            case 'a': i++;  break;
            case 's': i++;  break;
            case 'S': i++;  break;
            case 'O': i++;  break;
            case 'G': g_is_gui = !g_is_gui; break;
            case '9': XDBG_ALLOC_ACTIVE = !XDBG_ALLOC_ACTIVE; break;
            default: break;
        }
    }
    argi = i;

    if(check_if_running_from_doubleclick() == 1) g_is_gui = 2;

    if(g_is_gui) {

        g_quickbms_exception_test = 0;

        i = argi;
        if(i > argc) i = argc;
        i = 3 - (argc - i);
        if(i > 0) {
            fprintf(stderr,
                "- GUI mode activated, remember that the tool works also from command-line\n"
                "  where are available various options like folder scanning, filters and so on\n"
                "\n");
            bms = calloc(argc + i + 1, sizeof(char *));
            if(!bms) STD_ERR(QUICKBMS_ERROR_MEMORY);
            memcpy(bms, argv, sizeof(char *) * argc);
            argv = (void *)bms;
            argc -= (3 - i);
            if(i >= 3)   argv[argc]     = get_file("select the BMS script or plugin to use", 1, 0);
            if(i >= 2) { argv[argc + 1] = get_file("select the input archives/files to extract, type * or \"\" for whole folder and subfolders", 0, fname_multi_select = 1); }
            if(i >= 1)   argv[argc + 2] = get_folder("select the output folder where extracting the files");
            argc += 3;
        }
    }
#endif

    g_quickbms_exception_test = 1;

    if(argc < 3) {
        if((argc >= 2) && (argv[1][1] == 'c')) {
            quick_bms_list();

        } else if((argc >= 2) && (argv[1][1] == 'u')) {
            quickbms_update();

        } else if((argc >= 2) && !stricmp(argv[1], "--version")) {
            printf(VER
#ifdef QUICKBMS64
                " (64bit test)"
#endif
                "\n");

        } else {
            myhelp(g_quickbms_arg0);
        }
        myexit(QUICKBMS_ERROR_ARGUMENTS);
    }

    memset(options_db, 0, sizeof(options_db));
    for(i = 1; i < argc; i++) {
        if(verbose_options(argv[i]) < 0) {
            if((i + 3) >= argc) break;
            fprintf(stderr, "\nError: wrong command-line argument (%s)\n", argv[i]);
            myexit(QUICKBMS_ERROR_ARGUMENTS);
        }
        options_db[(u8)argv[i][1]]++;
        switch(argv[i][1]) {
            case '-':
            case '?':
            case 'h': myhelp(g_quickbms_arg0);  myexit(QUICKBMS_ERROR_ARGUMENTS);   break;
            case 'c': quick_bms_list(); myexit(QUICKBMS_ERROR_ARGUMENTS);   break;
            case 'l': list_only             = 1;                            break;
            case 'f': append_list(&filter_files_tmp,    argv[++i]);         break;
            case 'F': append_list(&filter_in_files_tmp, argv[++i]);         break;
            case 'o': force_overwrite       = 1;                            break;
            case 'k': force_overwrite       = -1;                           break;
            case 'v': verbose               = 1; dump_cmdline(argc, argv);  break;
            case 'V': verbose               = -1;                           break;
            case 'L': listfile              = mystrdup_simple(argv[++i]);   break;
            case 'R': quick_gui_exit        = 1;                            break;  // internal usage for external programs
            case 'x': decimal_notation      = 0;                            break;
            case 'w': write_mode            = 1;                            break;
            case 'a': quickbms_arg          = mystrdup_simple(argv[++i]);   break;
            case 'd': quickbms_outname      = 1;                            break;
            case 'D': quickbms_outname      = -1;                           break;
            case 'E': endian_killer         = 1;                            break;
            case '0': void_dump             = 1;                            break;
            case 'r': reimport              = 1;                            break;
            case 'n': enable_sockets        = 1;                            break;
            case 'p': enable_process        = 1;                            break;
            case 'A': enable_audio          = 1;                            break;
            case 'g': enable_video          = 1;                            break;
            case 'm': enable_winmsg         = 1;                            break;
            case 'C': enable_calldll        = 1;                            break;
            case 'H': enable_hexhtml        = -1;                           break;
            case 'X': enable_hexhtml = -1; hexhtml_output = HEXHTML_CONSOLE; break;
            case 's': pre_script            = mystrdup_simple(argv[++i]);   break;
            case 'u': update                = 1;                            break;
            case '.': continue_anyway       = 1;                            break;
            case '9': break;    // already handled! XDBG_ALLOC_ACTIVE     = !XDBG_ALLOC_ACTIVE;           break;  // xdbg_toggle() is not ready yet
            case '8': XDBG_ALLOC_INDEX      = !XDBG_ALLOC_INDEX;            break;
            case '7': XDBG_ALLOC_VERBOSE    = !XDBG_ALLOC_VERBOSE;          break;
            case '6': XDBG_HEAPVALIDATE     = !XDBG_HEAPVALIDATE;           break;
            case 'S': quickbms_execute_file = mystrdup_simple(argv[++i]);   break;
            case 'Y': g_yes                 = 1;                            break;
            case '3': g_int3                = 1;                            break;
            case 'O': g_force_output        = mystrdup_simple(argv[++i]);   break;
            case 'G': break;    // already handled! g_is_gui              = !g_is_gui;                    break;
            default: {
                fprintf(stderr, "\nError: wrong command-line argument (%s)\n", argv[i]);
                myexit(QUICKBMS_ERROR_ARGUMENTS);
            }
        }
    }

    if(update) quickbms_update();

    if(reimport) fprintf(stderr, "- REIMPORT mode enabled!\n");

    bms   = argv[i++];
    fname = argv[i++];
    if(i < argc) fdir = argv[i++];

    if(!bms || !fname || !fdir) {
        fprintf(stderr, "\n"
            "Error: you missed one or more arguments:\n"
            "       - bms:   %s\n"
            "       - fname: %s\n"
            "       - fdir:  %s\n"
            "\n",
            bms, fname, fdir);
        myexit(QUICKBMS_ERROR_ARGUMENTS);
    }

    if(bms)   bms   = mystrdup_simple(bms);
    if(fname) {
        if(fname_multi_select) {
            fname = malloc_copy(NULL, fname, MULTI_PATHSZ);
        } else {
            fname = mystrdup_simple(fname);
        }
    }
    if(fdir) fdir = mystrdup_simple(fdir);

    // useful for get_file on Windows7 where it's used '*' to work
    for(p = fname + strlen(fname) - 1; p >= (fname + 1) /* leave / or \ on fname[0] */; p--) {
        if(!strchr("\\/* ", *p)) break;
        *p = 0;
    }
    if((p >= fname) && (*p == ':')) {
        p[1] = '\\';
        p[2] = 0;
    }

    // fix and build filters

    filter_files    = build_filter(filter_files_tmp);
    FREEZ(filter_files_tmp)

    filter_in_files = build_filter(filter_in_files_tmp);
    FREEZ(filter_in_files_tmp)

    temp_folder[0] = 0;
#ifdef WIN32
    GetTempPath(sizeof(temp_folder), temp_folder);
#endif
    if(!temp_folder[0]) {
        p = getenv ("TMP");
        if(!p) p = getenv ("TEMP");
        if(!p) p = getenv ("TMPDIR");
#ifdef WIN32
        if(!p) p = "c:\\windows\\temp";
#else
        if(!p) p = "/tmp";
#endif
        mystrcpy(temp_folder, p, PATHSZ);
    }

    if(!getcwd(current_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);

    output_folder = fdir;
    if(!chdir(output_folder)) { // ???
        output_folder = malloc(PATHSZ + 1);
        if(!output_folder) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(!getcwd(output_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
        if(chdir(current_folder) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }

    copycut_folder(fname, file_folder); // this is ok also with windows multifile
    if(!file_folder[0]) {
        if(!getcwd(file_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }

    /* problems with multifile, do NOT USE the following!
    if(!chdir(file_folder)) {   // ???
        if(!getcwd(file_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
        if(chdir(current_folder) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
        p = get_filename(fname);
        fname = malloc(strlen(file_folder) + 1 + strlen(p) + 1);
        if(!fname) STD_ERR(QUICKBMS_ERROR_MEMORY);
        sprintf(fname, "%s%c%s", file_folder, PATHSLASH, p);
    }
    */

    bms_init(0);

    get_main_path(NULL, g_quickbms_arg0, exe_folder);
    if(!exe_folder[0]) {
        if(!getcwd(exe_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }

    // the following is used only for calldll so it's not much important
    if(strchr(bms, ':') || (bms[0] == '/') || (bms[0] == '\\')) {   // almost absolute path
        bms_folder[0] = 0;
    } else {
        mystrcpy(bms_folder, current_folder, PATHSZ);
    }
    mystrcpy(bms_folder + strlen(bms_folder), bms, PATHSZ - strlen(bms_folder));
    copycut_folder(NULL, bms_folder);

    newdir = NULL;
#ifdef WIN32
    if(g_is_gui && fname[strlen(fname) + 1]) { // check if there are files after the folder
        newdir = fname;
        if(!getcwd(bckdir, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
        if(chdir(newdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
        for(p = fname;;) {
            p += strlen(p) + 1;
            if(!*p) break;
            add_files(p, 0, NULL);
        }
    } else
#endif

    if(check_is_dir(fname)) {
        mystrcpy(file_folder, fname, PATHSZ);
        newdir = fname;
        fprintf(stderr, "- start the scanning of the input folder: %s\n", newdir);
        if(!getcwd(bckdir, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
        if(chdir(newdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
        strcpy(filedir, ".");
        recursive_dir(filedir, PATHSZ);
    }
    // if one of the above was done finish the job
    if(newdir) {
        files = add_files(NULL, 0, &input_total_files);
        curr_file = 0;
        if(input_total_files <= 0) {
            fprintf(stderr, "\nError: the input folder is empty\n");
            myexit(QUICKBMS_ERROR_FOLDER);
        }
        if(chdir(bckdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }

    p = strchr(current_folder, ':');    if(p && !p[1]) strcpy(p + 1, "\\");
    p = strchr(bms_folder, ':');        if(p && !p[1]) strcpy(p + 1, "\\");
    p = strchr(exe_folder, ':');        if(p && !p[1]) strcpy(p + 1, "\\");
    p = strchr(file_folder, ':');       if(p && !p[1]) strcpy(p + 1, "\\");
    p = strchr(output_folder, ':');     if(p && !p[1]) strcpy(p + 1, "\\");

    // boring stuff for having a full file_folder, maybe this one is the good time
    tmp = malloc(PATHSZ + 1);
    if(!tmp) STD_ERR(QUICKBMS_ERROR_MEMORY);
    if(!getcwd(tmp, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
    if(chdir(current_folder) >= 0) {
        if(chdir(file_folder) >= 0) {
            if(!getcwd(file_folder, PATHSZ)) STD_ERR(QUICKBMS_ERROR_FOLDER);
        }
        if(chdir(tmp) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }
    free(tmp);

    if(verbose) {
        fprintf(stderr, "- current_folder: %s\n", current_folder);
        fprintf(stderr, "- bms_folder:     %s\n", bms_folder);
        fprintf(stderr, "- exe_folder:     %s\n", exe_folder);
        fprintf(stderr, "- file_folder:    %s\n", file_folder);
        fprintf(stderr, "- output_folder:  %s\n", output_folder);
        fprintf(stderr, "- temp_folder:    %s\n", temp_folder);
    }

    set_quickbms_arg(quickbms_arg);

    if(check_extension(bms, "wcx")) wcx_plugin = 1;

redo:
    benchmark = time(NULL);
    if(files) {
        fname = files[curr_file].name;
        curr_file++;
        if(chdir(bckdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
        if(chdir(newdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }
    if(wcx_plugin) {
        if(wcx(NULL, fname) < 0) STD_ERR(QUICKBMS_ERROR_EXTRA);
    } else {
        myfopen(fname, 0, 1);
    }
    if(files) {
        if(chdir(bckdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
    }

    if(wcx_plugin) {
        fprintf(stderr, "- open WCX plugin %s\n", bms);
        if(wcx(bms, fname) < 0) STD_ERR(QUICKBMS_ERROR_EXTRA);
    } else {
        fprintf(stderr, "- open script %s\n", bms);
        if(!strcmp(bms, "-")) {
            fds = stdin;
        } else {
            fds = fopen(bms, "rb");
            if(!fds) STD_ERR(QUICKBMS_ERROR_FILE_READ);
        }
        cmd = 0;
        if(pre_script) {
            p = quickbms_fopen(pre_script);
            pre_fd = fopen(p, "rb");
            free(p);
            if(pre_fd) {
                cmd = parse_bms(pre_fd, NULL, cmd);
                fclose(pre_fd);
            } else {
                cmd = parse_bms(NULL, pre_script, cmd);
            }
        }
        cmd = parse_bms(fds, NULL, cmd);
        if(fds != stdin) fclose(fds);
    }

    if(listfile && !listfd) {
        listfd = fopen(listfile, "wb");
        if(!listfd) STD_ERR(QUICKBMS_ERROR_FILE_WRITE);
    }

    if(!list_only) {
        if(/*!list_only &&*/ fdir && fdir[0] /* && strcmp(fdir, ".")*/) {
            fprintf(stderr, "- set output folder %s\n", fdir);
            if(chdir(fdir) < 0) {
                fprintf(stderr, "- the folder doesn't exist, do you want to create it (y/N)?:\n  ");
                if(get_yesno(NULL) != 'y') STD_ERR(QUICKBMS_ERROR_USER);
                fdir = create_dir(fdir, 1, 1, 1, 0);   // fdir must remain modified
                if(!fdir) STD_ERR(QUICKBMS_ERROR_FOLDER);
            }
            if(quickbms_outname) {
                p = fname;
                if(!files || (input_total_files <= 1)) {
                    p = get_filename(p);    // take only the name of the file instead of the whole path
                }
                tmp = mystrdup_simple(p);   // don't change fname

                fix_my_d_option(tmp, NULL); // compare file path and output path

                if(!create_dir(tmp, 1, 1, (quickbms_outname > 0) ? 1 : 0, 1)) STD_ERR(QUICKBMS_ERROR_FOLDER);
                FREEZ(tmp)
            }
        }
    }

    set_console_title(options_db, bms, fname);

    fprintf(stderr, "\n"
        "  %-*s filesize   filename\n"
        "--------------------------------------\n",
        sizeof(int) * 2, "offset");

    if(wcx_plugin) {
        wcx(NULL, NULL);
    } else {
        start_bms(-1, 0, &mybreak);
    }

    benchmark = time(NULL) - benchmark;
    if(reimport) {
        fprintf(stderr, "\n- %"PRId" files reimported in %d seconds\n", reimported_files, (i32)benchmark);
    } else {
        fprintf(stderr, "\n- %"PRId" files found in %d seconds\n", extracted_files, (i32)benchmark);
    }

    if(files && (curr_file < input_total_files)) {
        bms_init(1);
        goto redo;
    }

    bms_finish();
    if(listfile) {
        fclose(listfd);
    }
    myexit(QUICKBMS_OK);
    return(QUICKBMS_OK);
}

