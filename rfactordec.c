/*
    Copyright 2008-2013 Luigi Auriemma

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

/*
  thanx to Donnie Werner (morning_wood) for the idea for this tool and providing some encrypted GMT files
  and thanx to mario for pointing me to ARCA Sim Racing and to some differences between various files
  all the values are at 64 bits to avoid casting bugs/problems
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATHSLASH   '\\'
    #define MAKEDIR(x)  mkdir(x)
    HWND    mywnd   = NULL;
    char *get_file(char *title, int multi);
#else
    #include <unistd.h>
    #define PATHSLASH   '/'
    #define MAKEDIR(x)  mkdir(name, 0755)
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;



#define VER         "0.2.2"
#define PRINTF64(x) (u32)(((x) >> 32) & 0xffffffff), (u32)((x) & 0xffffffff)    // I64x, llx? blah



#define PATHSZ      1024
#define STD_ERR     std_err(NULL)
#define myexit(X)   std_err("")
#define mystrdup    strdup
typedef struct {
    u8      *name;
    int     size;
} files_t;
files_t *add_files(u8 *fname, int fsize, int *ret_files);
int recursive_dir(u8 *filedir, int filedirsz);
u8 *mystrcpy(u8 *dst, u8 *src, int max);
int check_is_dir(u8 *fname);
int check_wildcard(u8 *fname, u8 *wildcard);

u8 *create_dir(u8 *name);
u8 *get_ext(u8 *fname);
u64 isi_keyz(int enctype, u64 key, u64 ret, int n);
u64 isi_key(int enctype, u64 key);
void isi_crypt(int enctype, u8 *output, u8 *input, int len, u64 key, int encrypt);
u64 isi_crypt2(int enctype, int pos, u64 key);
u64 hex64(u8 *str);
void fd_dump(u8 *fname, u8 *buff, int len, void *head, int head_len);
u8 *fd_load(u8 *fname, int *fsize);
void std_err(u8 *err);



int     overwrite   = 0;
u8      *filter_in_files    = NULL;



int get_yesno(void) {
    u8      ans[16];

    if(!fgets(ans, sizeof(ans), stdin)) return(0);
    return(tolower(ans[0]));
}



int main(int argc, char *argv[]) {
    files_t *files              = NULL; // scan
    int     outdirlen           = 0,
            curr_file           = 0,
            input_total_files   = 0;
    u8      filedir[PATHSZ + 1] = "",
            outdir[PATHSZ + 1]  = "",
            *p                  = NULL;

    u64     enc_head[2],
            key,
            sign        = 0x2eb8f5cc9b14ea3bLL; // ARCA Sim Racing
    int     i,
            buffsz,
            datasz,
            skip,
            encrypt     = 0,
            altmode     = 0,
            enctype     = 0,
            force_sign  = 0,
            dont_ask    = 0;
    u8      *buff,
            *data,
            *fin,
            *fout,
            *ext;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    fputs("\n"
        "ISI rFactor files decrypter/encrypter " VER "\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stderr);

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
        fprintf(stderr, "\n"
            "Usage: %s [options] <input_file/folder> <output_file/folder>\n"
            "\n"
            "Options:\n"
            "-o      overwrite output file if already exists without prompting\n"
            "-s SIGN specify the signature for encryption, default %08x%08x (ARCA)\n"
            "-e      force the encryption of input_file (default is only decryption)\n"
            "        at the moment only ARCA requires encrypted files to work, so if you\n"
            "        have modified the files of this game you need to use -e on them\n"
            "-F W    filter the input files using the W wildcard, example -F \"*.gmt\"\n"
            "-y      force encryption for unlisted signatures without asking\n"
            "\n"
            "input and output file can be also the same, so it will be overwritten\n"
            "use - as output_file for selecting stdout\n"
            "\n", argv[0], PRINTF64(sign));
        std_err("");
    }

    argc -= 2;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            fprintf(stderr, "\nError: recheck your options (%s is not valid)\n", argv[i]);
            std_err("");
        }
        switch(argv[i][1]) {
            case 'o': overwrite         = 1;                break;
            case 's': sign              = hex64(argv[++i]); break;
            case 'e': encrypt           = 1;                break;
            case 'F': filter_in_files   = argv[++i];        break;
            case 'y': dont_ask          = 1;                break;
            default: {
                fprintf(stderr, "\nError: wrong command-line argument (%s)\n\n", argv[i]);
                std_err("");
                } break;
        }
    }

    fin  = argv[argc];
    fout = argv[argc + 1];

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
redo_load:
    buff = fd_load(fin, &buffsz);
    fprintf(stderr, "- %d bytes loaded\n", buffsz);

redo:
    if(encrypt) {
        key  = sign * time(NULL) * buffsz;  // something random, not necessary
        enc_head[0] = sign ^ key;
        enc_head[1] = key;
        data = buff;
        datasz = buffsz;
    } else {
        sign = *(u64 *)(buff);
        key  = *(u64 *)(buff + 8);
        data = buff + 16;
        datasz = buffsz - 16;
        sign ^= key;
    }
    if(datasz < 0) {
        fprintf(stderr, "\nError: input file is too small\n");
        std_err("");
    }

    fprintf(stderr, "- signature %08x%08x\n", PRINTF64(sign));
    if(sign == 0x38af5637e81bc9a0LL) {
        fprintf(stderr, "- rFactor encrypted file\n");
    } else if(sign == 0x2eb8f5cc9b14ea3bLL) {
        fprintf(stderr, "- ARCA Sim Racing encrypted file\n");
    } else if(sign == 0x6a9d37283a9f3d9fLL) {
        fprintf(stderr, "- Simulador Turismo Carretera encrypted file\n");
    } else if(sign == 0xde4139f961fa2817LL) {
        fprintf(stderr, "- Top Race Simulador 2009 encrypted file\n");
    } else if(sign == 0x38af3150902cc55bLL) {
        fprintf(stderr, "- Superleague Formula\n");
        enctype = 1;
    } else if(sign == 0x4b1dca9f960524e8LL) {
        fprintf(stderr, "- Game Stock Car\n");
        enctype = 1;
    } else if(sign == 0x06a66ad328aeaed6LL) {
        fprintf(stderr, "- Simulador Turismo Carretera 2012 encrypted file\n");
        enctype = 1;
    } else if(sign == 0x28b7856a3a5996daLL) {
        fprintf(stderr, "- Game Stock Car: Formula Truck\n");
        enctype = 1;
    } else if(force_sign) {
        fprintf(stderr, "- unknown game, force mode\n");
        altmode = 0;
        enctype = 1;
    } else {
        if(!encrypt && !altmode) {
            buffsz -= 7;
            if(buffsz >= 0) {
                for(i = 0; i < buffsz; i++) {
                    buff[i] = buff[i + 7] - 1;
                }
                altmode = 1;
                fprintf(stderr, "- check the alternative modes\n");
                goto redo;
            }
        }
        if(!dont_ask) {
            fprintf(stderr, "\n"
                "Error: the signature of the input file is not known\n"
                "       if you are sure that the input file is encrypted contact me now!\n");
            fprintf(stderr, "\n"
                "       do you want to force the encryption using enctype 1 (y/N)?\n"
                "       ");
            if(get_yesno() != 'y') std_err("");
        }
        force_sign = 1;
        free(buff);
        buff = NULL;
        goto redo_load;
    }

    ext = get_ext(fin);
    if(!stricmp(ext, "GMT")) {
        skip = 4;
    } else {
        skip = 0;
    }
    if(skip) fprintf(stderr, "- bytes to not decrypt: %d\n", skip);

    fprintf(stderr, "- key %08x%08x\n", PRINTF64(key));
    key = isi_key(enctype, key);    // WRONG key for re-encryption! I will work on it in future
    fprintf(stderr, "- encryption key %08x%08x\n", PRINTF64(key));

    fprintf(stderr, "- start %s\n", encrypt ? "encryption" : "decryption");
    isi_crypt(enctype, data + skip, data + skip, datasz - skip, key, encrypt);

    if(!strcmp(fout, "-")) {
        if(encrypt) fwrite(&enc_head, 1, 16, stdout);
        fwrite(data, 1, datasz, stdout);
    } else {
        fd_dump(fout, data, datasz, &enc_head, encrypt ? 16 : 0);
    }
    free(buff);

    if(files && (curr_file < input_total_files)) {  // scan
        goto redo_scan;
    }
    fprintf(stderr, "- done\n");
    std_err("");    // scan
    return(0);
}



#ifdef WIN32
char *get_file(char *title, int multi) {
    OPENFILENAME    ofn;
    int     maxlen;
    char    *filename;

    if(multi) {
        maxlen = 32768; // 32k limit ansi, no limit unicode
    } else {
        maxlen = PATHSZ;
    }
    filename = malloc(maxlen + 1);
    if(!filename) STD_ERR;
    filename[0] = 0;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.lpstrFilter =
        "(*.*)\0"       "*.*\0"
        "\0"            "\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = maxlen;
    ofn.lpstrTitle      = title;
    ofn.Flags           = OFN_PATHMUSTEXIST | /*OFN_FILEMUSTEXIST |*/
                          OFN_LONGNAMES     | OFN_EXPLORER |
                          /*OFN_HIDEREADONLY  |*/ OFN_ENABLESIZING;
    if(multi) ofn.Flags |= OFN_ALLOWMULTISELECT;

    fprintf(stderr, "- %s\n", ofn.lpstrTitle);
    if(!GetOpenFileName(&ofn)) exit(1); // terminate immediately
    return(filename);
}
#endif

files_t *add_files(u8 *fname, int fsize, int *ret_files) {
    static int      filesi  = 0,
                    filesn  = 0;
    static files_t  *files  = NULL;
    files_t         *ret;

    if(ret_files) {
        *ret_files = filesi;
        files = realloc(files, sizeof(files_t) * (filesi + 1)); // not needed, but it's ok
        if(!files) STD_ERR;
        files[filesi].name   = NULL;
        //files[filesi].offset = 0;
        files[filesi].size   = 0;
        ret    = files;
        filesi = 0;
        filesn = 0;
        files  = NULL;
        return(ret);
    }

    if(filter_in_files && (check_wildcard(fname, filter_in_files) < 0)) return(NULL);

    if(filesi >= filesn) {
        filesn += 1024;
        files = realloc(files, sizeof(files_t) * filesn);
        if(!files) STD_ERR;
    }
    files[filesi].name   = mystrdup(fname);
    //files[filesi].offset = 0;
    files[filesi].size   = fsize;
    filesi++;
    return(NULL);
}

int recursive_dir(u8 *filedir, int filedirsz) {
    int     plen,
            namelen,
            ret     = -1;

#ifdef WIN32
    static int      winnt = -1;
    OSVERSIONINFO   osver;
    WIN32_FIND_DATA wfd;
    HANDLE          hFind = INVALID_HANDLE_VALUE;

    if(winnt < 0) {
        osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osver);
        if(osver.dwPlatformId >= VER_PLATFORM_WIN32_NT) {
            winnt = 1;
        } else {
            winnt = 0;
        }
    }

    plen = strlen(filedir);
    if((plen + 4) >= filedirsz) goto quit;
    strcpy(filedir + plen, "\\*.*");
    plen++;

    if(winnt) { // required to avoid problems with Vista and Windows7!
        hFind = FindFirstFileEx(filedir, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
    } else {
        hFind = FindFirstFile(filedir, &wfd);
    }
    if(hFind == INVALID_HANDLE_VALUE) goto quit;
    do {
        if(!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, "..")) continue;

        namelen = strlen(wfd.cFileName);
        if((plen + namelen) >= filedirsz) goto quit;
        //strcpy(filedir + plen, wfd.cFileName);
        memcpy(filedir + plen, wfd.cFileName, namelen);
        filedir[plen + namelen] = 0;

        if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if(recursive_dir(filedir, filedirsz) < 0) goto quit;
        } else {
            add_files(filedir, wfd.nFileSizeLow, NULL);
        }
    } while(FindNextFile(hFind, &wfd));
    ret = 0;

quit:
    if(hFind != INVALID_HANDLE_VALUE) FindClose(hFind);
#else
    struct  stat    xstat;
    struct  dirent  **namelist;
    int     n,
            i;

    n = scandir(filedir, &namelist, NULL, NULL);
    if(n < 0) {
        if(stat(filedir, &xstat) < 0) {
            fprintf(stderr, "**** %s", filedir);
            STD_ERR;
        }
        add_files(filedir, xstat.st_size, NULL);
        return(0);
    }

    plen = strlen(filedir);
    if((plen + 1) >= filedirsz) goto quit;
    strcpy(filedir + plen, "/");
    plen++;

    for(i = 0; i < n; i++) {
        if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;

        namelen = strlen(namelist[i]->d_name);
        if((plen + namelen) >= filedirsz) goto quit;
        //strcpy(filedir + plen, namelist[i]->d_name);
        memcpy(filedir + plen, namelist[i]->d_name, namelen);
        filedir[plen + namelen] = 0;

        if(stat(filedir, &xstat) < 0) {
            fprintf(stderr, "**** %s", filedir);
            STD_ERR;
        }
        if(S_ISDIR(xstat.st_mode)) {
            if(recursive_dir(filedir, filedirsz) < 0) goto quit;
        } else {
            add_files(filedir, xstat.st_size, NULL);
        }
        free(namelist[i]);
    }
    ret = 0;

quit:
    for(; i < n; i++) free(namelist[i]);
    free(namelist);
#endif
    filedir[plen - 1] = 0;
    return(ret);
}

u8 *mystrcpy(u8 *dst, u8 *src, int max) {
    u8      *p,
            *l;

    p = dst;
    l = dst + max - 1;
    while(p < l) {
        if(!*src) break;
        *p++ = *src++;
    }
    *p = 0;
    return(dst);
}

int check_is_dir(u8 *fname) {
    struct stat xstat;

    if(stat(fname, &xstat) < 0) return(0);
    if(!S_ISDIR(xstat.st_mode)) return(0);
    return(1);
}

int check_wildcard(u8 *fname, u8 *wildcard) {
    u8      *f,
            *w,
            *a;

    if(!wildcard) return(0);
    f = fname;
    w = wildcard;
    a = NULL;
    while(*f || *w) {
        if(!*w && !a) return(-1);
        if(*w == '?') {
            if(!*f) break;
            w++;
            f++;
        } else if(*w == '*') {
            w++;
            a = w;
        } else {
            if(!*f) break;
            if(tolower(*f) != tolower(*w)) {
                if(!a) return(-1);
                f++;
                w = a;
            } else {
                f++;
                w++;
            }
        }
    }
    if(*f || *w) return(-1);
    return(0);
}



u8 *create_dir(u8 *name) {
    struct stat xstat;
    int     n,
            namelen;
    u8      *p;

    namelen = strlen(name);

    p = strchr(name, ':');
    if(p) {
        p++;
    } else {
        p = name;
    }
    if((*p == '/') || (*p == '\\')) p++;

    for(; (p - name) < namelen; p++) {
        n = strcspn(p, "/\\");
        if(!n) continue;

        p += n;
        if(!*p) continue;
        *p = 0;

        if(stat(name, &xstat) < 0) {
            fprintf(stderr, "- create folder %s\n", name);
            MAKEDIR(name);
        } else if(!S_ISDIR(xstat.st_mode)) {
            fprintf(stderr, "\nError: %s is not a folder\n", name);
            std_err("");
        }

        *p = PATHSLASH;
    }
    return(name);
}



u8 *get_ext(u8 *fname) {
    u8      *p;

    p = strrchr(fname, '.');
    if(p) return(p + 1);
    return(fname + strlen(fname));
}



u64 isi_keyz(int enctype, u64 key, u64 ret, int n) {
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
    return(ret);
}



u64 isi_key(int enctype, u64 key) {
    return(isi_keyz(enctype, key, 0, 0));
}



void isi_crypt(int enctype, u8 *output, u8 *input, int len, u64 key, int encrypt) {
    u64     t,
            i,
            n,
            c,
            mask;
    int     x,
            blocksz = 0;

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



u64 isi_crypt2(int enctype, int pos, u64 key) {
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



u64 hex64(u8 *str) {
    u64     ret;
    int     n;
    u8      *p;

    if((strlen(str) > 2) && (str[0] == '0') && (tolower(str[1]) == 'x')) {
        str += 2;
    }

    ret = 0;
    for(p = str; *p; p += 2) {
        if(sscanf(p, "%02x", &n) != 1) break;
        ret = (ret << 8) | (n & 0xff);
    }
    return(ret);
}



void fd_dump(u8 *fname, u8 *buff, int len, void *head, int head_len) {
    FILE    *fd;
    u8      ans[16];

    fname = create_dir(fname);

    fprintf(stderr, "- write file %s\n", fname);
    if(!overwrite) {
        fd = fopen(fname, "rb");
        if(fd) {
            fclose(fd);
            fprintf(stderr, "- file already exists, do you want to overwrite it (y/N/all)?\n  ");
            fflush(stdin);
            fgets(ans, sizeof(ans), stdin);
            if(ans[0] == 'a') overwrite = 1;
            else if(ans[0] != 'y') exit(1); // terminate immediately
        }
    }
    fd = fopen(fname, "wb");
    if(!fd) std_err(NULL);
    if(head_len) fwrite(head, 1, head_len, fd);
    fwrite(buff, 1, len, fd);
    fclose(fd);
}



u8 *fd_load(u8 *fname, int *fsize) {
    struct  stat xstat;
    FILE    *fd;
    u8      *buff;

    fprintf(stderr, "- open file %s\n", fname);
    fd = fopen(fname, "rb");
    if(!fd) std_err(NULL);
    fstat(fileno(fd), &xstat);
    buff = malloc(xstat.st_size);
    if(!buff) std_err(NULL);
    fread(buff, 1, xstat.st_size, fd);
    fclose(fd);
    *fsize = xstat.st_size;
    return(buff);
}



void std_err(u8 *err) {
    if(err) {
        if(err[0]) fprintf(stderr, "\nError: %s\n", err);
    } else {
        perror("\nError");
    }
#ifdef WIN32
    u8      ans[16];
    if(GetWindowLong(mywnd, GWL_WNDPROC)) {
        fprintf(stderr, "\nPress RETURN to quit");
        fgets(ans, sizeof(ans), stdin);
    }
#endif
    exit(1);
}


