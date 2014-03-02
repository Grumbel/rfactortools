/*
    Copyright 2007-2013 Luigi Auriemma

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <zlib.h>
#include "blowfish.h"
#include "wtcc_bf.h"
#include "show_dump.h"

#ifdef WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATHSLASH   '\\'
    #define MAKEDIR(x)  mkdir(x)
    HWND    mywnd   = NULL;
    char *get_file(char *title, int multi);
#else
    #include <unistd.h>
    #include <dirent.h>
    #define stricmp     strcasecmp
    #define PATHSLASH   '/'
    #define MAKEDIR(x)  mkdir(name, 0755)
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;



#define VER         "0.3.3"
#define BF_DECRYPT  0
#define BF_ENCRYPT  1
#define fin         in_file
#define fout        out_file



#define PATHSZ      1024
#define STD_ERR     std_err(NULL)
#define myexit(X)   std_err("")
#define mystrdup    strdup



typedef struct {
    u8      *name;
    int     size;
} files_t;



#include "utils.c"



int string2key(u8 *data);
void put32(u8 *data, int num);
int get32(u8 *data);
u8 *wtcc_encrypt(u8 *filebuff, int filelen, int *retlen, int filever);
u8 *wtcc_decrypt(u8 *filebuff, int filelen, int *retlen);
void wtcc_blowfish(u8 *key, int keylen, int keytable, u8 *data, int datasz, int enc);
void xor(u8 *data, int datasz, int chr);
u8 *fd_read(u8 *name, int *fdlen);
u8 *create_dir(u8 *name);
void fd_write(u8 *name, u8 *data, int datasz);
void std_err(u8 *err);



static const u8
        ver01_key[20]       = "\x46\x17\x80\x2d\x13\xe4\x2f\x2f\x27\x6f\x1e\x19\x23\x65\x69\x14\x41\x2c\x01\x05",
        ver23_key[16]       = "qcB81[O x_@)pobk",
        ver45_key[16]       = "\x45\x13\x3B\x81\x0B\xA1\x09\x96\x25\x42\xF0\xE5\x7D\x19\x11\x53",
        ver67_key[16]       = "\x8f\xf8\x9d\x48\x9a\x2e\x37\x62\x32\x54\xd7\xd8\xab\x05\x2c\x44",
        volvo_key[17]       = "\x70\xFB\xA4\x45\x19\x28\x49\x47\x39\x98\x93\x25\x69\x65\xB3\x20\x65",
        raceroom_key[20]    = "\x0c\x92\x5e\xda\xe9\x17\x42\xc0\x3c\x1e\x17\xed\xe1\x3d\x42\x06\xb4\xb9\x69\xea";

int     auto_try        = 1,
        customkeylen    = 0,
        debug           = 0;
u8      *customkey      = NULL;



int main(int argc, char *argv[]) {
    files_t *files              = NULL; // scan
    int     outdirlen           = 0,
            curr_file           = 0,
            input_total_files   = 0;
    u8      filedir[PATHSZ + 1] = "",
            outdir[PATHSZ + 1]  = "",
            *p                  = NULL;

    u32     bufflen,
            filelen;
    int     i,
            forceenc    = 0,
            encver      = 1;
    u8      *filebuff,
            *buff,
            *in_file,
            *out_file;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    fputs("\n"
        "Race WTCC files encrypter/decrypter " VER "\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

#ifdef WIN32    // scan + gui
    mywnd = GetForegroundWindow();
    if(GetWindowLong(mywnd, GWL_WNDPROC)) {
        for(i = 1; i < argc; i++) {
            if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
                break;
            }
        }
        i = 2 - (argc - i);
        if(i > 0) {
            fprintf(stderr, 
                "- GUI mode activated, remember that the tool works also from command-line\n"
                "  where are available various other options and the encryption mode\n"
                "\n");
            buff = calloc(argc + i + 1, sizeof(char *));
            if(!buff) STD_ERR;
            memcpy(buff, argv, sizeof(char *) * argc);
            argv = (void *)buff;
            argc -= (2 - i);
            if(i >= 2) argv[argc + 0] = get_file("select the input file to decrypt, type \"\" for the whole folder and subfolders", 1);
            if(i >= 1) argv[argc + 1] = get_file("select the output file or folder (type \"\") where decrypting the files", 1); // multi is not used, needed for ""
            argc += 2;
        }
    }
#endif

    if(argc < 3) {
        printf("\n"
            "Usage: %s [options] <input_file/folder> <output_file/folder>\n"
            "\n"
            "Options:\n"
            "-o       overwrite output file if already exists without prompting\n"
            "-v VER   specify the file version for encryption, default %u\n"
            "-e d/e   this option is here only for compatibility reasons, it allows you\n"
            "         to force decryption 'd' or encryption 'e'\n"
            "-k GAME  select the specific key for the specific GAME which uses a non\n"
            "         default key, the following is the list of games (example -k volvo):\n"
            "           raceroom\n"
            "           volvo\n"
            "-F W     filter the input files using the W wildcard, example -F \"*.gmt\"\n"
            "-V       debug\n"
            "\n"
            "The encryption or decryption function is automatically chosen by the tool\n"
            "after having checked if the file is encrypted or not, use -e to override it.\n"
            "Note that -k volvo worked also with some old versions of RaceRoom.\n"
            "For the JCA files of RaceRoom use QuickBMS and the relative script:\n"
            "  http://quickbms.aluigi.org\n"
            "\n", argv[0], encver);
        std_err("");
    }

    argc -= 2;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            printf("\nError: recheck your options (%s is not valid)\n", argv[i]);
            std_err("");
        }
        switch(argv[i][1]) {
            case 'o': overwrite         = 1;                        break;
            case 'v': encver            = atoi(argv[++i]);          break;
            case 'e': forceenc          = tolower(argv[++i][0]);    break;
            case 'k': customkey         = argv[++i];                break;
            case 'F': filter_in_files   = argv[++i];                break;
            case 'V': debug             = 1;                        break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n\n", argv[i]);
                std_err("");
                break;
            }
        }
    }

    if(customkey) {
        if(!stricmp(customkey, "volvo")) {
            customkey    = (u8 *)volvo_key;
            customkeylen = sizeof(volvo_key);

        } else if(!stricmp(customkey, "roomrace") || !stricmp(customkey, "raceroom")) {
            customkey    = (u8 *)raceroom_key;
            customkeylen = sizeof(raceroom_key);

        } else {
            printf("- unknown game specified with the -k option, I will consider it a hex key\n");
            customkey    = strdup(customkey);  // not needed but good
            customkeylen = string2key(customkey);
        }
    }

    in_file  = argv[argc];
    out_file = argv[argc + 1];

    // scan
#ifdef WIN32
    if(GetWindowLong(mywnd, GWL_WNDPROC) && fin[strlen(fin) + 1]) { // check if there are files after the folder
        i = snprintf(filedir, PATHSZ, "%s%c", fin, PATHSLASH);
        if((i < 0) || (i >= PATHSZ)) std_err("");
        for(p = fin;;) {
            p += strlen(p) + 1;
            if(!*p) break;
            mystrcpy(filedir + i, p, PATHSZ - i);
            add_files(filedir, 0, NULL);
        }
    } else
#endif
    if(check_is_dir(fin)) {
        printf("- start the scanning of the input folder: %s\n", fin);
        //strcpy(filedir, "."); // if you enable it you must add " + 2" to each add_files
        mystrcpy(filedir, fin, PATHSZ);
        recursive_dir(filedir, PATHSZ);
    }
    if(filedir[0]) {
        files = add_files(NULL, 0, &input_total_files);
        curr_file = 0;
        if(input_total_files <= 0) {
            printf("\nError: the input folder is empty\n");
            myexit(-2);
        }
    }
    if(check_is_dir(fout)) {
        outdirlen = snprintf(outdir, PATHSZ, "%s%c", fout, PATHSLASH);
        if((outdirlen < 0) || (outdirlen >= PATHSZ)) std_err("");
        fout = outdir;
    }

    int finlen = strlen(fin);
redo_scan:
    if(files) {
        fin = files[curr_file].name;
        curr_file++;
        fprintf(stderr, "\n");
    }
    if(outdirlen) {
        if(files) {
            for(p = fin + finlen; *p; p++) {
                if((*p != '\\') && (*p != '/')) break;
            }
        } else {
            p = strrchr(fin, '\\');
            if(!p) p = strrchr(fin, '/');
            if(p) {
                p++;
            } else {
                p = fin;
            }
        }
        mystrcpy(outdir + outdirlen, p, PATHSZ - outdirlen);
    }

redo:
    filebuff = fd_read(in_file, &filelen);

    if((memcmp(filebuff, "\x0f\xb1\xff\xff", 4) && memcmp(filebuff, "\x1f\x8b\x08", 3) && (forceenc != 'd')) || (forceenc == 'e')) {
        printf("- perform encryption:\n");
        buff = wtcc_encrypt(filebuff, filelen, &bufflen, encver);
    } else {
        printf("- perform decryption:\n");
        buff = wtcc_decrypt(filebuff, filelen, &bufflen);
    }
    if(!buff && auto_try) {
        auto_try++; // 1
        switch(auto_try) {
            case 2:
                customkey    = (u8 *)raceroom_key;
                customkeylen = sizeof(raceroom_key);
                break;
            case 3:
                customkey    = (u8 *)volvo_key;
                customkeylen = sizeof(volvo_key);
                break;
            default: auto_try = 0; break;
        }        
        free(filebuff);
        printf("\n");
        if(auto_try) goto redo;
    }

    fd_write(out_file, buff, bufflen);

    printf("- %u bytes written\n", bufflen);

    if(files && (curr_file < input_total_files)) {  // scan
        if(!((buff >= filebuff) && (buff <= (filebuff + 16)))) free(buff);
        free(filebuff);
        goto redo_scan;
    }
    fprintf(stderr, "- done\n");
    std_err("");    // scan
    return(0);
}



int string2key(u8 *data) {
    int     i,
            n;
    u8      *ret;

    ret = data;
    for(i = 0; *data; i++) {
        while(*data && (*data <= ' ')) data++;
        if(sscanf(data, "%02x", &n) != 1) break;
        ret[i] = n;
        data += 2;
    }
    return(i);
}



void put32(u8 *data, int num) {
    data[0] = num;
    data[1] = num >> 8;
    data[2] = num >> 16;
    data[3] = num >> 24;
}



int get32(u8 *data) {
    return(data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}



u8 *wtcc_filever_key(int encrypt, int filever, u32 *ret_keylen, u32 *ret_keytable, int *ret_dozip) {
    u32     keylen   = 0,
            keytable = 0;
    int     dozip    = 0;
    u8      *key     = NULL;

    if(ret_keylen)   keylen     = *ret_keylen;
    if(ret_keytable) keytable   = *ret_keytable;
    if(ret_dozip)    dozip      = *ret_dozip;

    if(!(filever & 1)) {
        if(dozip) printf("- alert: version %d should have the compression field set to zero\n", filever);
    }

    switch(filever) {
        case 0: {
            key      = (u8 *)ver01_key;
            keylen   = sizeof(ver01_key);
            keytable = 0;
            dozip    = 0;
            break;
        }
        case 1: {
            key      = (u8 *)ver01_key;
            keylen   = sizeof(ver01_key);
            keytable = 0;
            dozip    = 1;
            break;
        }
        case 2: {
            key      = (u8 *)ver23_key;
            keylen   = sizeof(ver23_key);
            keytable = 1;
            dozip    = 0;
            break;
        }
        case 3: {
            key      = (u8 *)ver23_key;
            keylen   = sizeof(ver23_key);
            keytable = 1;
            dozip    = 1;
            break;
        }
        case 4: {
            key      = (u8 *)ver45_key;
            keylen   = sizeof(ver45_key);
            keytable = 2;
            dozip    = 0;
            break;
        }
        case 5: {
            key      = (u8 *)ver45_key;
            keylen   = sizeof(ver45_key);
            keytable = 2;
            dozip    = 1;
            break;
        }
        case 6: {
            key      = (u8 *)ver67_key;
            keylen   = sizeof(ver67_key);
            keytable = 2;
            dozip    = 0;
            break;
        }
        case 7: {
            key      = (u8 *)ver67_key;
            keylen   = sizeof(ver67_key);
            keytable = 2;
            dozip    = 1;
            break;
        }
        default: {
            printf("\nError: unsupported file version\n");
            std_err("");
            break;
        }
    }

    if(ret_keylen)   *ret_keylen   = keylen;
    if(ret_keytable) *ret_keytable = keytable;
    if(ret_dozip)    *ret_dozip    = dozip;
    return(key);
}



u8 *wtcc_encrypt(u8 *filebuff, int filelen, int *retlen, int filever) {
    uLongf  zlen;
    u32     //zlen,
            bflen,
            keylen   = 0,
            keytable = 0;
    int     dozip    = 0;
    u8      *buff,
            *key     = NULL;

    zlen = filelen + (filelen / 1000) + 12; // must be at least 0.1% larger than sourceLen plus 12 bytes
    buff = malloc(16 + zlen);
    if(!buff) std_err(NULL);

    printf("- XOR %u bytes with 0xeb\n", filelen);
    xor(filebuff, filelen, 0xeb);

    key = wtcc_filever_key(1, filever, &keylen, &keytable, &dozip);

    if(customkey) {
        key     = customkey;
        keylen  = customkeylen;
    }

    if(dozip) {
        printf("- zip %u bytes\n", filelen);
        zlen = filelen;
        compress2(buff + 16, &zlen, filebuff, filelen, Z_BEST_COMPRESSION);
        bflen = zlen;
    } else {
        zlen = 0;
        memcpy(buff + 16, filebuff, filelen);
        bflen = filelen;
    }
    bflen = (bflen + 7) & ~7;

    printf("- encrypt %u bytes\n", bflen);
    wtcc_blowfish(key, keylen, keytable, buff + 16, bflen, BF_ENCRYPT);

    printf(
        "- file version     %u\n"
        "- blowfish size    %u (%s)\n"
        "- compressed size  %u\n"
        "- data size        %u\n",
        filever,
        bflen, (bflen & 7) ? "wrong" : "correct",
        (u32)zlen,
        filelen);

    put32(buff,      0xffffb10f);
    put32(buff + 4,  filever);
    put32(buff + 8,  filelen);
    put32(buff + 12, zlen);

    *retlen = 16 + bflen;
    return(buff);
}



u8 *wtcc_decrypt(u8 *filebuff, int filelen, int *retlen) {
    uLongf  uzlen;
    u32     filever,
            zlen,       // size of the compressed data
            bflen,      // size of the blowfish encrypted data
            bufflen,    // size of the output data
            //uzlen,      // size of the uncompressed data
            keylen      = 0,
            keytable    = 0;
    int     zerr,
            dozip       = 0,
            workaround  = 0;
    u8      backup[8],
            *buff,
            *uzbuff,
            *key        = NULL;

    if(!memcmp(filebuff, "\x1f\x8b\x08", 3)) {
        printf(
            "- this file is compressed with gzip\n"
            "  add the .gz extension to the filename and open it normally or just open it\n"
            "  directly with your favourite compression program\n");
        std_err("");
    }
    if(memcmp(filebuff, "\x0f\xb1\xff\xff", 4)) {
        printf("\nError: file seems not encrypted with the Race WTCC algorithm\n");
        std_err("");
    }
    filever = get32(filebuff + 4);
    bufflen = get32(filebuff + 8);
    zlen    = get32(filebuff + 12);
    buff    = filebuff + 16;
    bflen   = filebuff + filelen - buff;

    printf(
        "- file version     %u\n"
        "- blowfish size    %u (%s)\n"
        "- compressed size  %u\n"
        "- data size        %u\n",
        filever,
        bflen, (bflen & 7) ? "wrong" : "correct",
        zlen,
        bufflen);

    if(filelen < 16) {
        printf("\nError: file is too short\n");
        std_err("");
    }

    if(zlen) dozip = 1;
    key = wtcc_filever_key(0, filever, &keylen, &keytable, &dozip);
    if(!dozip) zlen = 0;

    if(customkey) {
        key     = customkey;
        keylen  = customkeylen;
    }

    if(zlen && (zlen & 7)) {
        memcpy(backup, buff + (zlen & ~7), zlen & 7);
    }

    printf("- decrypt %u bytes (0x%x)\n", bflen, bflen);
    wtcc_blowfish(key, keylen, keytable, buff, bflen, BF_DECRYPT);

    if(zlen) {
        printf("- unzip %u bytes\n", zlen);
        uzlen = bufflen;
        uzbuff = malloc(uzlen);
        if(!uzbuff) std_err(NULL);

redo_uncompress:
        if(debug) {
            printf("- DEBUG uncompress:\n");
            show_dump(buff, zlen, stdout);
        }

        zerr = uncompress(uzbuff, &uzlen, buff, zlen);

        if(zerr != Z_OK) {
            if(zerr == Z_BUF_ERROR) {
                // it's ok remember 00011_Airwaves_BMW.car of BTCC09 1.3
                uzlen = bufflen;
            } else {
                if(!workaround && (zlen & 7)) {
                    // again 00011_Airwaves_BMW.car of BTCC09 1.3
                    workaround++;
                    memcpy(buff + (zlen & ~7), backup, zlen & 7);
                    goto redo_uncompress;
                }
                if(auto_try) return(NULL);
                printf("\nError: zlib uncompress() error %d, contact me and report this error\n", zerr);
                std_err("");
            }
        }
        if(uzlen != bufflen) {
            if(auto_try) return(NULL);
            printf("\nError: the uncompressed size (%u) differs from that specified in the file\n", (u32)uzlen);
            std_err("");
        }
        buff = uzbuff;
    }

    printf("- XOR %u bytes with 0xeb\n", bufflen);
    xor(buff, bufflen, 0xeb);

    *retlen = bufflen;
    return(buff);
}



void wtcc_blowfish(u8 *key, int keylen, int keytable, u8 *data, int datasz, int enc) {
    blf_ctx bfx;
    int     i;

    printf("- use keytable %u and key: ", keytable);
    for(i = 0; i < keylen; i++) {
        printf("%02x", key[i]);
    }
    printf("\n");

    wtcc_blowfish_key(&bfx, key, keylen, keytable);

    // NO padding!
    // datasz = (datasz + 7) & (~7);

    if(debug) {
        printf("- DEBUG wtcc_blowfish before (0x%x bytes):\n", datasz);
        show_dump(data, datasz, stdout);
    }

    if(enc == BF_ENCRYPT) {
        blf_enc(&bfx, (void *)data, datasz / 8);
    } else {
        blf_dec(&bfx, (void *)data, datasz / 8);
    }

    if(debug) {
        printf("- DEBUG wtcc_blowfish after:\n");
        show_dump(data, datasz, stdout);
    }
}



void xor(u8 *data, int datasz, int chr) {
    while(datasz--) {
        *data++ ^= chr;
    }
}

