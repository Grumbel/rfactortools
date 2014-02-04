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

// QuickBMS general functions



// the macro is better for size and debugging (no stack canary and ret)
//static void set_int3(const void *dllname, const void *hlib, const void *funcname, const void *funcaddr, const void *argc) {
#define set_int3(dllname, hlib, funcname, funcaddr, argc) { \
    if(g_int3) { \
        __asm__ __volatile__ ("movl %0, %%eax" :: "g"(dllname)  :        "ecx", "edx", "esi", "edi"); \
        __asm__ __volatile__ ("movl %0, %%ecx" :: "g"(hlib)     : "eax",        "edx", "esi", "edi"); \
        __asm__ __volatile__ ("movl %0, %%edx" :: "g"(funcname) : "eax", "ecx",        "esi", "edi"); \
        __asm__ __volatile__ ("movl %0, %%esi" :: "g"(funcaddr) : "eax", "ecx", "edx",        "edi"); \
        __asm__ __volatile__ ("movl %0, %%edi" :: "g"(argc)     : "eax", "ecx", "edx", "esi"       ); \
        __asm__ __volatile__ ("int3"); \
        __asm__ __volatile__ ("nop"); \
    } \
}



void show_dump(int left, u8 *data, int len, FILE *stream) {
    int                 rem;
    static const u8     hex[16] = "0123456789abcdef";
    u8                  leftbuff[80],
                        buff[67],
                        chr,
                        *bytes,
                        *p,
                        *limit,
                        *glimit = data + len;

    if(!data) return;
    if(len < 0) return;
    memset(buff + 2, ' ', 48);
    memset(leftbuff, ' ', sizeof(leftbuff));

    while(data < glimit) {
        limit = data + 16;
        if(limit > glimit) {
            limit = glimit;
            memset(buff, ' ', 48);
        }

        p     = buff;
        bytes = p + 50;
        while(data < limit) {
            chr = *data;
            *p++ = hex[chr >> 4];
            *p++ = hex[chr & 15];
            p++;
            *bytes++ = ((chr < ' ') || (chr >= 0x7f)) ? '.' : chr;
            data++;
        }
        *bytes++ = '\n';

        for(rem = left; rem >= sizeof(leftbuff); rem -= sizeof(leftbuff)) {
            fwrite(leftbuff, sizeof(leftbuff), 1, stream);
        }
        if(rem > 0) fwrite(leftbuff, rem, 1, stream);
        fwrite(buff, bytes - buff, 1, stream);
    }
}



int check_extension(u8 *fname, u8 *ext) {
    u8      *p;

    if(!fname || !ext) return(0);
    p = strrchr(fname, '.');
    if(!p) return(0);
    p++;
    if(!stricmp(p, ext)) return(1);
    return(0);
}



u8 *mystrcpy(u8 *dst, u8 *src, int max) {
    u8      *p,
            *l;

    if(dst && (max > 0)) {
        if(!src) src = "";
        p = dst;
        l = dst + max - 1;
        while(p < l) {
            if(!*src) break;
            *p++ = *src++;
        }
        *p = 0;
    }
    return(dst);
}



u8 *mystrdup_simple(u8 *str) { // multiplatform compatible
    int     len;
    u8      *o  = NULL;

    if(str) {
        len = strlen(str);
        o = malloc(len + 1);
        if(!o) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memcpy(o, str, len + 1);
    }
    return(o);
}



u8 *mystrdup(u8 **old_buff, u8 *str) { // multiplatform compatible
    int     len;
    u8      *o  = NULL;

    if(old_buff) o = *old_buff;
    if(str) {
        len = strlen(str);
        o = realloc(o, len + 1);
        if(!o) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memcpy(o, str, len + 1);
    }
    if(old_buff) {
        if(!o) {
            FREEZ(*old_buff)
        } else {
            *old_buff = o;
        }
    }
    return(o);
}



u8 *mystrchrs(u8 *str, u8 *chrs) {
    //int     i;
    u8      *p,
            *ret = NULL;

    if(str && chrs) {
        for(p = str; *p; p++) {
            if(strchr(chrs, *p)) return(p);
        }
        /*
        for(i = 0; chrs[i]; i++) {
            p = strchr(str, chrs[i]);
            if(p && (!ret || (p < ret))) {
                ret = p;
            }
        }
        */
    }
    return(ret);
}



u8 *mystrrchrs(u8 *str, u8 *chrs) {
    //int     i;
    u8      *p,
            *ret = NULL;

    if(str && chrs) {
        for(p = str + strlen(str) - 1; p >= str; p--) {
            if(strchr(chrs, *p)) return(p);
        }
        /*
        for(i = 0; chrs[i]; i++) {
            p = strrchr(str, chrs[i]);
            if(p) {
                str = p;
                ret = p;
            }
        }
        */
    }
    return(ret);
}



#define mystrstr    stristr
#define mystrrstr   strristr

/*
u8 *mystrstr(u8 *str, u8 *s) {
    u8      *p;

    if(str && s) {
        for(p = str; *p; p++) {
            if(!stricmp(p, s)) return(p);
        }
    }
    return(NULL);
}

u8 *mystrrstr(u8 *str, u8 *s) {
    int     slen;
    u8      *p;

    if(str && s) {
        slen = strlen(s);
        for(p = str + strlen(str) - slen; p >= str; p--) {
            if(!stricmp(p, s)) return(p);
        }
    }
    return(NULL);
}
*/



int check_is_dir(u8 *fname) {
    struct stat xstat;

    if(!fname) return(1);
    if(stat(fname, &xstat) < 0) return(0);
    if(!S_ISDIR(xstat.st_mode)) return(0);
    return(1);
}



u8 *get_main_path(u8 *fname, u8 *argv0, u8 *output) {
    static u8   fullname[PATHSZ + 1];
    DWORD   r;
    u8      *p;

    if(!output) output = fullname;
#ifdef WIN32
    r = GetModuleFileName(NULL, output, PATHSZ);
    if(!r || (r >= PATHSZ))
#endif
    sprintf(output, "%.*s", PATHSZ, argv0);

    if(check_is_dir(output)) return(output);
    p = mystrrchrs(output, "\\/");
    if(fname) {
        if(!p) p = output - 1;
        sprintf(p + 1, "%.*s", PATHSZ - (p - output), fname);
    } else {
        if(p) *p = 0;
    }
    return(output);
}



int copycut_folder(u8 *input, u8 *output) {
    u8      *p;

    if(!output) return(-1);
    if(input) mystrcpy(output, input, PATHSZ);
    if(check_is_dir(output)) return(0);
    p = mystrrchrs(output, "\\/");
    if(!p) {
        if(input) output[0] = 0;
    } else {
        *p = 0;
    }
    if(check_is_dir(output)) return(0);
    return(-1);
}



#ifdef WIN32
char *get_file(char *title, i32 bms, i32 multi) {
    OPENFILENAME    ofn;
    int     maxlen;
    char    *filename;

    if(multi) {
        maxlen = MULTI_PATHSZ; // 32k limit ansi, no limit unicode
    } else {
        maxlen = PATHSZ;
    }
    filename = calloc(maxlen + 1, 1);
    if(!filename) STD_ERR(QUICKBMS_ERROR_MEMORY);
    filename[0] = 0;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize     = OPENFILENAME_SIZE_VERSION_400 /*sizeof(ofn)*/;    // the additional fields used in win5 are useless
    if(bms) {
        ofn.lpstrFilter =
            "script/plugin (bms/txt/wcx)\0"  "*.bms;*.txt;*.wcx\0"
            //"WCX plugin\0"  "*.wcx\0"
            "(*.*)\0"       "*.*\0"
            "\0"            "\0";
    } else {
        ofn.lpstrFilter =
            "(*.*)\0"       "*.*\0"
            "\0"            "\0";
    }
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = maxlen;
    ofn.lpstrTitle      = title;
    ofn.Flags           = OFN_PATHMUSTEXIST |
                          OFN_FILEMUSTEXIST |
                          OFN_LONGNAMES     |
                          OFN_EXPLORER      |
                          0x10000000 /*OFN_FORCESHOWHIDDEN*/ |
                          OFN_ENABLESIZING  |
                          OFN_HIDEREADONLY  |
                          OFN_NOVALIDATE |
                          0;

    if(multi) ofn.Flags |= OFN_ALLOWMULTISELECT;

    printf("- %s\n", ofn.lpstrTitle);
    if(!GetOpenFileName(&ofn)) exit(1); // terminate immediately
    return(filename);
}

char *get_folder(char *title) {
    OPENFILENAME    ofn;
    char    *p;
    char    *filename;

    filename = malloc(PATHSZ + 1);
    if(!filename) STD_ERR(QUICKBMS_ERROR_MEMORY);

    strcpy(filename, "enter in the output folder and press Save");
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize     = OPENFILENAME_SIZE_VERSION_400 /*sizeof(ofn)*/;
    ofn.lpstrFilter     = "(*.*)\0" "*.*\0" "\0" "\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = PATHSZ;
    ofn.lpstrTitle      = title;
    ofn.Flags           = OFN_PATHMUSTEXIST |
                          // removed for folders OFN_FILEMUSTEXIST |
                          OFN_LONGNAMES     |
                          OFN_EXPLORER      |
                          0x10000000 /*OFN_FORCESHOWHIDDEN*/ |
                          OFN_ENABLESIZING  |
                          OFN_HIDEREADONLY  |
                          OFN_NOVALIDATE |
                          0;

    printf("- %s\n", ofn.lpstrTitle);
    if(!GetSaveFileName(&ofn)) exit(1); // terminate immediately
    p = mystrrchrs(filename, "\\/");
    if(p) *p = 0;
    return(filename);
}
#endif



int fgetz(u8 *data, int datalen, FILE *fd, u8 *fmt, ...) {
    va_list ap;
    u8      *p;

    if(!data) return(-1);
    if(datalen <= 0) return(-1);
    if(fmt) {
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
    data[0] = 0;
    if(!fgets(data, datalen, fd)) myexit(QUICKBMS_ERROR_UNKNOWN);    //return(-1);
    for(p = data; *p && (*p != '\r') && (*p != '\n'); p++);
    *p = 0;
    return(p - data);
}



QUICKBMS_int readbase(u8 *data, QUICKBMS_int size, QUICKBMS_int *readn) {
    static const u8 table[256] =    // fast performances
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
    int     num     = 0;
    int     sign;
    u8      c,
            *s,
            *hex_fix;

    s = data;
    if(!data || !size || !data[0]) {
        // do nothing (for readn)
    } else {
        // useful in some occasions, for example if the input is external!
        for(; *s; s++) {
            if(!strchr(" \t\r\n", *s)) break;
        }
        if(*s == '-') {
            sign = -1;
            s++;
        } else {
            sign = 0;
        }
        hex_fix = s;
        for(; *s; s++) {
            c = *s;
            //if((c == 'x') || (c == 'X') || (c == '$')) {  // auto base switching
            if(
                (((c == 'h') || (c == 'x') || (c == 'X')) && (s > hex_fix)) // 0x and 100h, NOT x123 or h123
             || (c == '$')                                                  // $1234 or 1234$
            ) {
                size = 16;
                continue;
            }
            c = table[c];
            if(c >= size) break;    // necessary to recognize the invalid chars based on the size
            num = (num * size) + c;
        }
        if(sign) num = -num;
    }
    if(readn) *readn = s - data;
    return(num);
}



int myisalnum(int chr) {
    if((chr >= '0') && (chr <= '9')) return(1);
    if((chr >= 'a') && (chr <= 'z')) return(1);
    if((chr >= 'A') && (chr <= 'Z')) return(1);
    if(chr == '-') return(1);   // negative number
    //if(chr == '+') return(1);   // positive number
    return(0);
}



int myishexdigit(int chr) {
    if((chr >= 'A') && (chr <= 'F')) return(1);
    if((chr >= 'a') && (chr <= 'f')) return(1);
    if((chr >= '0') && (chr <= '9')) return(1); // this is enough because hex start ever with 0x
    if(chr == '-') return(1);   // negative number
    //if(chr == '+') return(1);   // positive number
    //if(chr == '$') return(1);   // delphi/vb hex
    return(0);
}



int myisdigit(int chr) {
    if((chr >= '0') && (chr <= '9')) return(1); // this is enough because hex start ever with 0x
    if(chr == '-') return(1);   // negative number
    //if(chr == '+') return(1);   // positive number
    //if(chr == '$') return(1);   // delphi/vb hex
    return(0);
}



int myisdigitstr(u8 *str) { // only a quick version
    int     i;

    if(!str) return(0);
    if(!myisdigit(str[0])) return(0);
    for(i = 1; str[i]; i++) {
        if(i >= NUMBERSZ) return(0);    // avoid to waste time with long strings
        if(!strchr("0123456789abcdefABCDEFx$", str[i])) return(0);
    }
    return(1);
}



u8 *myitoa(int num) {
    static const u8 table[] = "0123456789abcdef";
    static u8       dstx[MULTISTATIC][3 + NUMBERSZ + 1] = {{""}};
    static int      dsty = 0;
    u_int   unum;
    u8      tmp[NUMBERSZ + 1],  // needed because hex numbers are inverted, I have already done various tests and this is the fastest!
            *p,                 // even faster than using directly dst as output
            *t,
            *dst;

    dst = (u8 *)dstx[dsty++ % MULTISTATIC];

    if(!num) {  // quick way, 0 is used enough often... ok it's probably useless
        dst[0] = '0';
        dst[1] = 0;
        return(dst);
    }

    p = dst;
    if(num < 0) {
        num = -num;
        *p++ = '-';
    }
    unum = num; // needed for the sign... many troubles

    //if((unum >= 0) && (unum <= 9)) {  // quick solution for numbers under 10, so uses only one char, (unum >= 0) avoids problems with 0x80000000
        //*p++ = table[unum];
        //*p   = 0;
        //return(dst);
    //}
    t = tmp + (NUMBERSZ - 1);   // the -1 is needed (old tests)
    *t = 0;
    t--;
    if(decimal_notation) {
        do {   // "unum" MUST be handled at the end of the cycle! example: 0
            *t = table[unum % (u_int)10];
            unum = unum / (u_int)10;
            if(!unum) break;
            t--;
        } while(t >= tmp);
    } else {
        *p++ = '0'; // hex notation is better for debugging
        *p++ = 'x';
        do {   // "unum" MUST be handled at the end of the cycle! example: 0
            *t = table[unum & 15];
            unum = unum >> (u_int)4;
            if(!unum) break;
            t--;
        } while(t >= tmp);
    }
    strcpy(p, t);

    //sprintf(dst, "%"PRId"", unum);  // old "one-instruction-only" solution, mine is better
    return(dst);
}



u8 *strdupcpy(u8 *dst, int *dstlen, u8 *src, int srclen) {
    int     tmp;

    if(srclen < 0) {
        if(src) srclen = strlen(src);
        else    srclen = 0;
    }

    /*
    // normal solution
    //if(srclen < STRINGSZ) srclen = STRINGSZ;  // disabled for testing
    if(dstlen) *dstlen = srclen;
    dst = realloc(dst, srclen + 2); // unicode
    if(!dst) STD_ERR(QUICKBMS_ERROR_MEMORY);
    // normal solution
    */

    // optimized solution
    if(!dstlen) {
        dstlen = &tmp;
        *dstlen = -1;
    }
    if(!dst || (*dstlen < srclen) || (*dstlen < 2)) {   // NULL + unicode to avoid srclen 0
        *dstlen = srclen;
        //if(*dstlen == -1) ALLOC_ERR;        // note that dstlen can't be < 0 due to the "srclen < 0" check
        //if(*dstlen == -2) ALLOC_ERR;        // big endian undelimited unicode
        if(*dstlen < STRINGSZ) *dstlen = STRINGSZ;    // better for numbers and common filenames
        dst = realloc(dst, (*dstlen) + 2);  // big endian undelimited unicode (now it's rare but in future it may be more used)
        if(!dst) STD_ERR(QUICKBMS_ERROR_MEMORY);
    }
    // optimized solution

    if(dst) {
        if(src) memcpy(dst, src, srclen);
        else    memset(dst, 0,   srclen);
        dst[srclen]     = 0;
        dst[srclen + 1] = 0;    // big endian undelimited unicode
    }
    return(dst);
}



u8 *re_strdup(u8 **ret_dst, u8 *src, int *retlen) {  // only for NULL delimited strings, NOT bytes!
    u8      *dst;

    if(ret_dst) dst = *ret_dst;
    else        dst = NULL;
    dst = strdupcpy(dst, retlen, src, -1);

    /*
    int     dstlen  = -1;
    // dst && src checked by strdupcpy
    if(retlen) dstlen = *retlen;
    dst = strdupcpy(dst, &dstlen, src, -1);
    if(retlen) *retlen = dstlen;
    */

    if(ret_dst) *ret_dst = dst;
    return(dst);
}



int strdup_replace(u8 **dstp, u8 *src, int src_len, int *dstp_len) {  // should improve a bit the performances
    if(!dstp) return(-1);
    *dstp = strdupcpy(*dstp, dstp_len, src, src_len);

    /*
    int     dst_len = -1;
    u8      *dst;

    if(!dstp) return(-1);
    dst = *dstp;

    if(!dstp_len && dst) {
        dst_len = strlen(dst);  // or is it better to use "dst_len = 0"?
    } else if(dstp_len) {
        dst_len = *dstp_len;
    }

    dst = strdupcpy(dst, &dst_len, src, src_len);

    *dstp = dst;
    if(dstp_len) *dstp_len = dst_len;
    */
    return(0);
}



int myisdechex_string(u8 *str) {
    QUICKBMS_int    len;

    // I have already verified that using a quick test only on the first char doesn't improve the performances if compared to the current full check
    if(!str) return(0);
    readbase(str, 10, &len);    // no need to know the number
    if(len <= 0) return(0);     // FALSE
    return(1);                  // TRUE
}



u16 swap16(u16 n) {
    n = (((n & 0xff00) >> 8) |
         ((n & 0x00ff) << 8));
    return(n);
}



u32 swap32(u32 n) {
    n = (((n & 0xff000000) >> 24) |
         ((n & 0x00ff0000) >>  8) |
         ((n & 0x0000ff00) <<  8) |
         ((n & 0x000000ff) << 24));
    return(n);
}



u64 swap64(u64 n) {
//#ifdef QUICKBMS64
    n = (((n & (u64)0xFF00000000000000ULL) >> (u64)56) |
         ((n & (u64)0x00FF000000000000ULL) >> (u64)40) |
         ((n & (u64)0x0000FF0000000000ULL) >> (u64)24) |
         ((n & (u64)0x000000FF00000000ULL) >> (u64) 8) |
         ((n & (u64)0x00000000FF000000ULL) << (u64) 8) |
         ((n & (u64)0x0000000000FF0000ULL) << (u64)24) |
         ((n & (u64)0x000000000000FF00ULL) << (u64)40) |
         ((n & (u64)0x00000000000000FFULL) << (u64)56));
//#else
//    n = swap32(n);
//#endif
    return(n);
}



u16 swap16le(u16 n) {
    int endian = 1;
    if(!*(char *)&endian) return swap16(n); // be cpu
    return n;                               // le cpu
}
u16 swap16be(u16 n) {
    int endian = 1;
    if(!*(char *)&endian) return n;         // be cpu
    return swap16(n);                       // le cpu
}



u32 swap32le(u32 n) {
    int endian = 1;
    if(!*(char *)&endian) return swap32(n); // be cpu
    return n;                               // le cpu
}
u32 swap32be(u32 n) {
    int endian = 1;
    if(!*(char *)&endian) return n;         // be cpu
    return swap32(n);                       // le cpu
}



u64 swap64le(u64 n) {
    int endian = 1;
    if(!*(char *)&endian) return swap64(n); // be cpu
    return n;                               // le cpu
}
u64 swap64be(u64 n) {
    int endian = 1;
    if(!*(char *)&endian) return n;         // be cpu
    return swap64(n);                       // le cpu
}



u16 myhtons(u16 n) {
    int endian = 1;
    if(!*(char *)&endian) return(n);
    return(swap16(n));
}
u16 myntohs(u16 n) {
    int endian = 1;
    if(!*(char *)&endian) return(n);
    return(swap16(n));
}
u32 myhtonl(u32 n) {
    int endian = 1;
    if(!*(char *)&endian) return(n);
    return(swap32(n));
}
u32 myntohl(u32 n) {
    int endian = 1;
    if(!*(char *)&endian) return(n);
    return(swap32(n));
}



u8 *strristr(u8 *s1, u8 *s2) {
    int     s1n,
            s2n;
    u8      *p;

    if(!s1 || !s2) return(NULL);
    s1n = strlen(s1);
    s2n = strlen(s2);
    if(s2n > s1n) return(NULL);
    for(p = s1 + (s1n - s2n); p >= s1; p--) {
        if(!strnicmp(p, s2, s2n)) return(p);
    }
    return(NULL);
}



int vspr(u8 **buff, u8 *fmt, va_list ap) {
    int     len,
            mlen;
    u8      *ret    = NULL;

    if(buff) *buff = NULL;
    if(!fmt) return(0);
    mlen = strlen(fmt) + 128;
    for(;;) {
        ret = realloc(ret, mlen + 1);
        if(!ret) return(0);     // return(-1);
        len = vsnprintf(ret, mlen, fmt, ap);
        if((len >= 0) && (len < mlen)) break;
        mlen += 128;
    }
    ret[len] = 0;
    if(buff) *buff = ret;
    return(len);
}



int spr(u8 **buff, u8 *fmt, ...) {
    va_list ap;
    int     len;

    va_start(ap, fmt);
    len = vspr(buff, fmt, ap);
    va_end(ap);
    return(len);
}



u8 *find_replace_string(u8 *buf, int *buflen, u8 *old, int oldlen, u8 *news, int newlen) {
    int     i,
            len,
            //len_bck,
            tlen,
            found;
    u8      *nbuf,
            *p;

    if(!buf) return(buf);
    found  = 0;
    len = -1;
    if(buflen) len = *buflen;
    if(len < 0) len = strlen(buf);
    if(oldlen < 0) {
        oldlen = 0;
        if(old) oldlen = strlen(old);
    }
    tlen    = len - oldlen;
    //len_bck = len;

    for(i = 0; i <= tlen; i++) {
        if(!strnicmp(buf + i, old, oldlen)) found++;
    }
    if(!found) return(buf); // nothing to change: return buf or a positive value

    //if(!news) return(NULL);  // if we want to know only if the searched string has been found, we will get NULL if YES and buf if NOT!!!
    if(newlen < 0) {
        newlen = 0;
        if(news) newlen = strlen(news);
    }

    if(newlen <= oldlen) {  // if the length of new string is equal/minor than the old one don't waste space for another buffer
        nbuf = buf;
    } else {                // allocate the new size
        nbuf = malloc(len + ((newlen - oldlen) * found) + 1);
        if(!nbuf) STD_ERR(QUICKBMS_ERROR_MEMORY);
    }

    p = nbuf;
    for(i = 0; i <= tlen;) {
        if(!strnicmp(buf + i, old, oldlen)) {
            memcpy(p, news, newlen);
            p += newlen;
            i += oldlen;
        } else {
            *p++ = buf[i];
            i++;
        }
    }
    while(i < len) {
        *p++ = buf[i];
        i++;
    }
    len = p - nbuf;
    if(buflen) *buflen = len;
    nbuf[len] = 0;  // hope the original input string has the +1 space
    return(nbuf);
}



u8 *numbers_to_bytes(u8 *str, int *ret_size, int hex) {
    static int  buffsz  = 0;
    static u8   *buff   = NULL;
    u_int   num;
    QUICKBMS_int len;
    int     i,
            t,
            size,
            slash_fix;
    u8      *s;

    if(ret_size) *ret_size = 0;
    if(!str) return(NULL);

    // try to guess non numbers, for example: filexor "mypassword"
    for(s = str; *s; s++) {
        if(*s <= ' ') continue;
        // number
            if(hex) {
                if(myishexdigit(*s)) break;
            } else {
                if(myisdigit(*s) || (*s == '$')) break;
            }
        if(*s == '\\') break;       // \x12

        // dump string
        s = str;    // dump also the initial spaces
        size = strlen(s);
        if(size > buffsz) {
            buffsz = size;
            buff = realloc(buff, buffsz + 1);
            if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
        }
        strcpy(buff, s);
        goto quit;
    }

    s = str;
    for(i = 0; *s;) {
        if(*s <= ' ') {
            s++;
            continue;
        }

        // yeah so it can handle also \x11\x22\x33
        slash_fix = -1;
        if(*s == '\\') {
            slash_fix = s - str;
            *s = '0';
        }

        //while(*s && !(myisdigit(*s) || (*s == '$'))) s++;  // this one handles also dots, commas and other bad chars

        // this one handles also dots, commas and other bad chars
        while(*s) {
            if(hex) {
                if(myishexdigit(*s)) break;
            } else {
                if(myisdigit(*s) || (*s == '$')) break;
            }
            s++;
        }

        num = readbase(s, hex ? 16 : 10, &len);

        if(slash_fix >= 0) str[slash_fix] = '\\';

        if(len <= 0) break;

        t = 1;
        if(num > 0xff) t = 4;

        if((i + t) > buffsz) {
            buffsz += t + STRINGSZ;
            buff = realloc(buff, buffsz + 1);
            if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
        }
        i += putxx(buff + i, num, t);

        s += len;
    }
    if(!buff) {
        buff = realloc(buff, buffsz + 1);
        if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
    }
    buff[i] = 0; // useless, only for possible new usages in future //, return ret as NULL
    size = i;
quit:
    if(ret_size) *ret_size = size;
    if(verbose > 0) {
        printf("- numbers_to_bytes of %d bytes\n ", (i32)size);
        for(i = 0; i < size; i++) printf(" 0x%02x", buff[i]);
        printf("\n");
    }
    return(buff);
}



// use real_* for improving speed avoiding the xalloc stuff
files_t *add_files(u8 *fname, int fsize, int *ret_files) {
    static int      filesi  = 0,
                    filesn  = 0;
    static files_t  *files  = NULL;
    files_t         *ret;

    if(ret_files) {
        *ret_files = filesi;
        files = real_realloc(files, sizeof(files_t) * (filesi + 1)); // not needed, but it's ok
        if(!files) STD_ERR(QUICKBMS_ERROR_MEMORY);
        files[filesi].name   = NULL;
        //files[filesi].offset = 0;
        files[filesi].size   = 0;
        ret    = files;
        filesi = 0;
        filesn = 0;
        files  = NULL;
        return(ret);
    }

    if(!fname) return(NULL);
    if(check_wildcards(fname, filter_in_files) < 0) return(NULL);

    if(filesi >= filesn) {
        filesn += 1024;
        files = real_realloc(files, sizeof(files_t) * filesn);
        if(!files) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memset(&files[filesi], 0, sizeof(files_t) * (filesn - filesi));
    }

    //mystrdup_simple(fname);
    files[filesi].name   = real_realloc(files[filesi].name, strlen(fname) + 1); // realloc in case of reusage
    if(!files[filesi].name) STD_ERR(QUICKBMS_ERROR_MEMORY);
    strcpy(files[filesi].name, fname);

    //files[filesi].offset = 0;
    files[filesi].size   = fsize;
    filesi++;
    return(NULL);
}



int quick_simple_tmpname_scanner(u8 *filedir, int filedirsz) {
    int     plen,
            namelen,
            ret     = -1;
#ifdef WIN32
    u8      *p;
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

    p = strrchr(filedir, '.');
    if(p) {
        sprintf(p, ".*");
    } else {
        sprintf(p, "%"PRIx".*", extracted_files);
    }
    plen = 0;

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
        strcpy(filedir + plen, wfd.cFileName);
        memcpy(filedir + plen, wfd.cFileName, namelen);
        filedir[plen + namelen] = 0;

        if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // no recursion
        } else {
            // file found!
            break;
        }
    } while(FindNextFile(hFind, &wfd));
    ret = 0;

quit:
    if(hFind != INVALID_HANDLE_VALUE) FindClose(hFind);
#else
    // do nothing, not supported
    // you must rename the file as .dat
#endif
    return(ret);
}



#define recursive_dir_skip_path 0
//#define recursive_dir_skip_path 2
int recursive_dir(u8 *filedir, int filedirsz) {
    int     plen,
            namelen,
            ret     = -1;

    if(!filedir) return(ret);
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
            add_files(filedir + recursive_dir_skip_path, wfd.nFileSizeLow, NULL);
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
            STD_ERR(QUICKBMS_ERROR_FOLDER);
        }
        add_files(filedir + recursive_dir_skip_path, xstat.st_size, NULL);
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
            STD_ERR(QUICKBMS_ERROR_FOLDER);
        }
        if(S_ISDIR(xstat.st_mode)) {
            if(recursive_dir(filedir, filedirsz) < 0) goto quit;
        } else {
            add_files(filedir + recursive_dir_skip_path, xstat.st_size, NULL);
        }
        FREEZ(namelist[i]);
    }
    ret = 0;

quit:
    for(; i < n; i++) FREEZ(namelist[i]);
    FREEZ(namelist);
#endif
    filedir[plen - 1] = 0;
    return(ret);
}



u8 *incremental_fread(FILE *fd, int *ret_size, int eol) {
    static const int    STDINSZ = 4096;
    int     len,
            size,
            buffsz  = 0;
    u8      *buff   = NULL;

    if(ret_size) *ret_size = 0;
    size = 0;
    for(;;) {
        if((size + STDINSZ) >= buffsz) {
            buffsz = size + STDINSZ;
            buff = (u8 *)realloc(buff, buffsz + 1);
            if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
        }
        if(eol) len = 1;
        else    len = buffsz - size;
        len = fread(buff + size, 1, len, fd);
        if(len <= 0) break;
        size += len;
        if(eol) {
            if((buff[size - len] == '\r') || (buff[size - len] == '\n')) {
                size -= len;
                break;
            }
        }
    }
    if(buff) buff[size] = 0;
    if(ret_size) *ret_size = size;
    return(buff);
}



u8 *fdload(u8 *fname, int *fsize) {
    struct stat xstat;
    FILE    *fd;
    int     size;
    u8      *buff;

    if(!fname) return(NULL);
    fprintf(stderr, "  %s\n", fname);
    if(!strcmp(fname, "-")) {
        return(incremental_fread(stdin, fsize, 0));
    }
    fd = fopen(fname, "rb");
    if(!fd) return(NULL);
    fstat(fileno(fd), &xstat);
    size = xstat.st_size;
    if(size == -1) ALLOC_ERR;
    buff = malloc(size + 1);
    if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
    fread(buff, 1, size, fd);
    buff[size] = 0;
    fclose(fd);
    if(fsize) *fsize = size;
    return(buff);
}



int cstring(u8 *input, u8 *output, int maxchars, int *inlen) {
    i32     n,
            len;
    u8      *p,
            *o;

    if(!input || !output) {
        if(inlen) *inlen = 0;
        return(0);
    }

    p = input;
    o = output;
    while(*p) {
        if(maxchars >= 0) {
            if((o - output) >= maxchars) break;
        }
        if(*p == '\\') {
            p++;
            switch(*p) {
                case 0:  return(-1); break;
                //case '0':  n = '\0'; break;
                case 'a':  n = '\a'; break;
                case 'b':  n = '\b'; break;
                case 'e':  n = '\e'; break;
                case 'f':  n = '\f'; break;
                case 'n':  n = '\n'; break;
                case 'r':  n = '\r'; break;
                case 't':  n = '\t'; break;
                case 'v':  n = '\v'; break;
                case '\"': n = '\"'; break;
                case '\'': n = '\''; break;
                case '\\': n = '\\'; break;
                case '?':  n = '\?'; break;
                case '.':  n = '.';  break;
                case ' ':  n = ' ';  break;
                case 'u': {
                    if(sscanf(p + 1, "%04x%n", &n, &len) != 1) return(-1);
                    if(len > 4) len = 4;
                    p += len;
                    *o++ = n;   n = (u32)n >> 8;    // this is NOT a real unicode->utf8 conversion! maybe in the next versions
                    break;
                }
                case 'U': {
                    if(sscanf(p + 1, "%08x%n", &n, &len) != 1) return(-1);
                    if(len > 8) len = 8;
                    p += len;
                    *o++ = n;   n = (u32)n >> 8;    // this is NOT a real unicode->utf8 conversion! maybe in the next versions
                    *o++ = n;   n = (u32)n >> 8;
                    *o++ = n;   n = (u32)n >> 8;
                    break;
                }
                case 'x': {
                    //n = readbase(p + 1, 16, &len);
                    //if(len <= 0) return(-1);
                    if(sscanf(p + 1, "%02x%n", &n, &len) != 1) return(-1);
                    if(len > 2) len = 2;
                    p += len;
                    break;
                }
                default: {
                    //n = readbase(p, 8, &len);
                    //if(len <= 0) return(-1);
                    if(sscanf(p, "%3o%n", &n, &len) != 1) return(-1);
                    if(len > 3) len = 3;
                    p += (len - 1); // work-around for the subsequent p++;
                    break;
                }
            }
            *o++ = n;
        } else {
            *o++ = *p;
        }
        p++;
    }
    *o = 0;
    len = o - output;
    if(inlen) *inlen = p - input;
    return(len);
}



// alternative to sscanf so it's possible to use also commas and hex numbers
int get_parameter_numbers(u8 *s, ...) {
    va_list ap;
    int     i,
            *par;

    // do NOT reset the parameters because they could have default values different than 0!

    if(!s) return(0);
    va_start(ap, s);
    for(i = 0;; i++) {
        par = va_arg(ap, int *);
        if(!par) break;

        while(*s && !myisalnum(*s)) s++;
        if(!*s) break;
        *par = myatoi(s);
        while(*s && myisalnum(*s)) s++;
        if(!*s) break;
    }
    va_end(ap);
    return(i);
}



int check_wildcard(u8 *fname, u8 *wildcard) {
    u8      *f,
            *w,
            *a;

    if(!fname) return(-1);
    if(!wildcard) return(-1);
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
            if(((*f == '\\') || (*f == '/')) && ((*w == '\\') || (*w == '/'))) {
                f++;
                w++;
            } else if(tolower(*f) != tolower(*w)) {
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



int check_wildcards(u8 *fname, u8 **list) {
    int     i,
            fok     = 0,
            fnot    = 0,
            ret     = -1;

    // no wildcards to check = ok
    if(!list) return(0);
    for(i = 0; list[i]; i++) {
        if(list[i][0] == '!') {
            fnot++;
            if(!check_wildcard(fname, list[i] + 1)) return(-1);
        } else {
            fok++;
            if(!check_wildcard(fname, list[i])) ret = 0;
        }
    }
    if(!fok) return(0);     // -f "!*.mp3" with txt files
    return(ret);
}



int file_exists(u8 *fname) {
    FILE    *fd;

    // stdin/stdout ???
    if(!strcmp(fname, "-")) return 1;

    // needed for symlinks to folders
    if(check_is_dir(fname)) return 0;

    fd = fopen(fname, "rb");
    if(!fd) return 0;
    fclose(fd);
    return 1;
}



// mdir creates the folder
// cdir goes in the folder
// is_path allows to create folders with the name of the archive
u8 *create_dir(u8 *fname, int mdir, int cdir, int is_path, int filter_bad) {
    static u8   root_path[1+1] = { PATHSLASH, 0x00 };
    int     i;
    u8      *tmp    = NULL,
            *p,
            *l;

    if(!fname) return(NULL);

    if(filter_bad) {
        p = strchr(fname, ':'); // unused
        if(p) {
            *p = '_';
            fname = p + 1;
        }
        for(p = fname; *p && strchr("\\/. \t:", *p); p++) *p = '_';
        fname = p;
    }

    // do not use "continue"
    for(p = fname;; p = l + 1) {
        for(l = p; *l && (*l != '\\') && (*l != '/'); l++);
        if(!*l) {
            if(!is_path) break;
            l = NULL;
        }
        if(l) *l = 0;

        if(!p[0]) {
            if(p != fname) goto continue_loop;
            p = root_path;
        }

        if(filter_bad) {
            if(!strcmp(p, "..")) {
                p[0] = '_';
                p[1] = '_';
            }
        }

        if(cdir) {
            if(p == root_path) {
                //if(mdir) make_dir(p);
                if(chdir(p) < 0) goto quit_error;

            } else if(p[0] && (p[strlen(p) - 1] == ':')) {  // we need c:\, not c:
                //if(mdir) make_dir(p);
                if(chdir(p) < 0) goto quit_error;   // partition
                chdir(root_path);                   // root

            } else {
                if(file_exists(p)) {
                    tmp = malloc(strlen(p) + 32 /*"extract" + num*/ + 1);
                    if(!tmp) STD_ERR(QUICKBMS_ERROR_MEMORY);
                    sprintf(tmp, "%s_extract", p);
                    for(i = 0; file_exists(tmp); i++) {
                        sprintf(tmp, "%s_extract%d", p, (i32)i);
                    }
                    p = tmp;
                }
                if(mdir) make_dir(p);
                if(chdir(p) < 0) goto quit_error;
                if(p == tmp) {
                    FREEZ(tmp);
                    p = NULL;
                }
            }
        } else {
            if(mdir) make_dir(fname);
        }

        continue_loop:
        if(!l) break;
        *l = PATHSLASH;
    }
    return(fname);

    quit_error:
    fprintf(stderr, "\nError: impossible to create/enter in folder %s\n", p);
    STD_ERR(QUICKBMS_ERROR_FOLDER);
    return NULL;
}



int get_yesno(u8 *data) {
    u8      tmp[16];

    if(g_yes) return('y');
    if(!data) {
        if(fgetz(tmp, sizeof(tmp), stdin, NULL) < 0) return(0);
        data = tmp;
    }
    return(tolower(data[0]));
}



int check_overwrite(u8 *fname, int check_if_present_only) {
    int     c;

    if(force_overwrite > 0) return(0);
    if(force_overwrite < 0) return(-1);
    if(!fname) return(0);
    if(!file_exists(fname)) return(0);
    if(check_if_present_only) return(-1);
    if(force_rename) return(-2);
    printf(
        "\n"
        "- the file \"%s\" already exists\n  do you want to overwrite it?\n"
        "    y = overwrite (you can use also the 'o' key)\n"
        "    n = skip (default, just press RETURN)\n"
        "    a = overwrite all the files without asking\n"
        "    r = automatically rename the files with the same name\n"
        "    0 = skip all the existent files without asking\n"
        "  \n",
        fname);
    if(append_mode) printf("\n"
        "  (remember that you are in append mode so be sure that the output folder was\n"
        "  empty otherwise the new data will be appended to the existent files!) ");
    c = get_yesno(NULL);
    if(c == 'y') return(0);
    if(c == 'o') return(0); // Overwrite
    if(c == 'a') {
        force_overwrite = 1;
        return(0);
    }
    if(c == 'r') {
        force_rename = 1;
        return(-2);
    }
    if(c == '0') {
        force_overwrite = -1;
        return(-1);
    }
    return(-1);
}



u8 *myalloc(u8 **data, QUICKBMS_int wantsize, QUICKBMS_int *currsize) {
    QUICKBMS_int    ows;    // original wantsize
    u8      *old_data;      // allocate it at any cost

    if(wantsize < 0) {
        fprintf(stderr, "\nError: the requested amount of bytes to allocate is negative (0x%"PRIx")\n", wantsize);
        myexit(QUICKBMS_ERROR_MEMORY);
    }
    if(!wantsize) return(NULL);
    if(!data) return(NULL);

    ows = wantsize;
    wantsize += MYALLOC_ZEROES; // needed by XMemDecompress



    // quick secure way that uses the advantages of xdbg_alloc
    //if((wantsize < 0) || (wantsize < ows)) {    // due to integer rounding
    if(wantsize < 0) {
        fprintf(stderr, "\nError: the requested amount of bytes to allocate is negative/too big (0x%"PRIx")\n", wantsize);
        myexit(QUICKBMS_ERROR_MEMORY);
    }
    if(currsize && (ows <= *currsize)) {
        // too expensive: memset((*data) + ows, 0, *currsize - ows);
        if(*currsize > 0) goto quit; //return(*data);
    }
    *data = realloc(*data, wantsize);
    if(!*data) STD_ERR(QUICKBMS_ERROR_MEMORY);
    if(currsize) *currsize = ows;
    memset((*data) + ows, 0, wantsize - ows);
    goto quit; //return(*data);
    // end of quick secure way



    wantsize = (wantsize + 4095) & (~4095);     // not bad as fault-safe and fast alloc solution: padding (4096 is usually the default size of a memory page)
    if((wantsize < 0) || (wantsize < ows)) {    // due to integer rounding
        fprintf(stderr, "\nError: the requested amount of bytes to allocate is negative/too big (0x%"PRIx")\n", wantsize);
        myexit(QUICKBMS_ERROR_MEMORY);
        //wantsize = ows;   // remember memset MYALLOC_ZEROES
    }

    if(currsize && (wantsize <= *currsize)) {
        if(*currsize > 0) goto quit;
    }

    old_data = *data;
    *data = realloc(*data, wantsize);
    if(!*data) {
        FREEZ(old_data);
        *data = malloc(wantsize);
        if(!*data) {
            fprintf(stderr, "- try allocating %"PRIu" bytes\n", wantsize);
            STD_ERR(QUICKBMS_ERROR_MEMORY);
        }
    }
    if(currsize) *currsize = wantsize - MYALLOC_ZEROES;      // obviously
quit:
    memset((*data) + ows, 0, MYALLOC_ZEROES);   // ows is the original wantsize, useful in some cases like XMemDecompress
    return(*data);
}



int delimit(u8 *str) {
    u8      *p;

    if(!str) return(-1);
    for(p = str; *p && (*p != '\n') && (*p != '\r'); p++);
    *p = 0;
    return(p - str);
}



void alloc_err(const char *fname, i32 line, const char *func) {
    fprintf(stderr, "\n- error in %s line %d: %s()\n", fname, (i32)line, func);
    fprintf(stderr, "Error: tentative of allocating -1 bytes\n");
    myexit(QUICKBMS_ERROR_MEMORY);
}



void std_err(const char *fname, i32 line, const char *func, signed char error) {    // char avoids problems with int on 64bit
    fprintf(stderr, "\n- error in %s line %d: %s()\n", fname, (i32)line, func);
    perror("Error");
    if(error < 0) error = QUICKBMS_ERROR_UNKNOWN;
    myexit(error);
}



void winerr(DWORD error, char *msg) {
#ifdef WIN32
    char    *message = NULL;

    if(!error) error = GetLastError();
    if(!msg) msg = "";

    if(error) {
        FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
          NULL,
          error,
          0,
          (char *)&message,
          0,
          NULL);
    }
    if(message) {
        fprintf(stderr, "\nError: %s - %s\n", msg, message);
        LocalFree(message);
    } else {
        fprintf(stderr, "\nError: %s - unknown Windows error\n", msg);
    }
    myexit(QUICKBMS_ERROR_UNKNOWN);
#else
    STD_ERR(QUICKBMS_ERROR_UNKNOWN);
#endif
}



void myexit(int ret) {
    if(!ret && quick_gui_exit) {
        // nothing to do
    } else {
#ifdef WIN32
        u8      ans[16];

        if(g_is_gui) {
            fgetz(ans, sizeof(ans), stdin,
                "\nPress RETURN to quit");
        }
#endif
        if(ret == -1) {
            fprintf(stderr, "\n"
                "Note that if both the scripts and your files are correct then it's possible\n"
                "that the script needs a newer version of QuickBMS, in which case download it:\n"
                "\n"
                "  http://quickbms.aluigi.org\n"
                "\n");
        }
    }
    exit(ret);
}



u_int rol(u_int n1, u_int n2) {
    return((n1 << n2) | (n1 >> ((u_int)INTSZ - n2)));
}



u_int ror(u_int n1, u_int n2) {
    return((n1 >> n2) | (n1 << ((u_int)INTSZ - n2)));
}



u_int bitswap(u_int n1, u_int n2) {
    u_int   out,
            rem = 0;

    if(n2 < INTSZ) {
        rem = n1 & (((int)-1) ^ (((int)1 << n2) - (int)1));
    }

    for(out = 0; n2; n2--) {
        out = (out << (int)1) | (n1 & (int)1);
        n1 >>= (u_int)1;
    }
    return(out | rem);
}



u_int byteswap(u_int n1, u_int n2) {
    u_int   out,
            rem = 0;

    if(n2 < (INTSZ >> 3)) {
        rem = n1 & (((int)-1) ^ (((int)1 << (n2 << (int)3)) - (int)1));
    }

    for(out = 0; n2; n2--) {
        out = (out << (int)8) | (n1 & (int)0xff);
        n1 >>= (u_int)8;
    }
    return(out | rem);
}



int power(int n1, int n2) {
    int     out = 1;

    for(;;) {
        if(n2 & 1) out *= n1;
        n2 >>= (int)1;
        if(!n2) break;
        n1 *= n1;
    }
    return(out);
}



int mysqrt(int num) {
    int    ret    = 0,
           ret_sq = 0,
           b;
    int    s;

    for(s = (INTSZ >> 1) - 1; s >= 0; s--) {
        b = ret_sq + ((int)1 << (s << (int)1)) + ((ret << s) << (int)1);
        if(b <= num) {
            ret_sq = b;
            ret += (int)1 << s;
        }
    }
    return(ret);
}



int radix(int n1, int n2) {
    int     i,
            olds,    // due to the
            news;    // lack of bits

    if(!n1 || !n2) return(0);

    if(n2 == 2) return(mysqrt(n1)); // fast way

    for(i = olds = 1; ; i <<= 1) {   // faster???
        news = power(i, n2);
        if((news > n1) || (news < olds)) break;
        olds = news;
    }

    for(i >>= 1; ; i++) {
        news = power(i, n2);
        if((news > n1) || (news < olds)) break;
        olds = news;
    }
    return(i - 1);
}



u32 str2ip(u8 *data) {
    unsigned    a, b, c, d;

    if(!data[0]) return(0);
    sscanf(data, "%u.%u.%u.%u", &a, &b, &c, &d);
    return((a & 0xff) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0xff) << 24));
}



u8 *ip2str(u32 ip) {
    static u8  data[16];

    sprintf(data, "%u.%u.%u.%u",
        (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), ((ip >> 24) & 0xff));
    return(data);
}



u8 *clean_filename(u8 *fname, int *wildcard_extension) {
    u8      *p,
            *ext;

    if(fname[1] == ':') fname += 2;

    for(p = fname; *p && (*p != '\n') && (*p != '\r'); p++);
    *p = 0;

    if(wildcard_extension) {
        *wildcard_extension = -1;
        ext = strrchr(fname, '.');
        if(ext && !ext[1]) *wildcard_extension = ext - fname;
        ext = strrchr(fname, '*');
        if(ext && !ext[1]) *wildcard_extension = ext - fname;
    }

    for(p = fname; *p; p++) {
        if(strchr("?%*:|\"<>", *p)) {    // invalid filename chars not supported by the most used file systems
            *p = '_';
        }
    }
    *p = 0;

    // remove final spaces and dots
    for(p--; p >= fname; p--) {
        if((*p != ' ') && (*p != '.')) break;
        *p = 0;
    }
    return(fname);
}



int debug_privileges(void)  {
#ifdef WIN32
    TOKEN_PRIVILEGES tp;
    HANDLE  hp;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hp)) return(-1);
    memset(&tp, 0, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid)) return FALSE;
    if(!AdjustTokenPrivileges(hp, FALSE, &tp, sizeof(tp), NULL, NULL)) return(-1);
    CloseHandle(hp);
#endif
    return(0);
}



// I don't trust memmove, it gave me problems in the past
int mymemmove(u8 *dst, u8 *src, int size) {
    int     i;

    if(!dst || !src) return(0);
    if(size < 0) size = strlen(src) + 1;
    if(dst < src) {
        for(i = 0; i < size; i++) {
            dst[i] = src[i];
        }
    } else {
        for(i = size - 1; i >= 0; i--) {
            dst[i] = src[i];
        }
    }
    return(i);
}



int myrand(void) {
    static  int rnd = 0;

    if(!rnd) rnd = time(NULL);
    rnd = ((rnd * 0x343FD) + 0x269EC3);
    return(rnd);
}



u8 *quickbms_tmpname(u8 **fname, u8 *prefix, u8 *ext) {
    static int  cnt = 0;

    if(!prefix) prefix = temp_folder;
    for(;;) {
        spr(fname, "%s%cquickbms_%08x%08x%08x%08x.%s",
            prefix,
            PATHSLASH,
#ifdef WIN32
            (int)GetCurrentProcessId(),
#else
            getpid(),
#endif
            cnt++,
            (i32)myrand(), (i32)myrand(),
            ext);
        if(!file_exists(*fname)) return(*fname);
    }
    return(NULL);
}



u32 mydump(u8 *fname, u8 *data, u32 size) {
    FILE    *fd;

    fd = fopen(fname, "wb");
    if(!fd) STD_ERR(QUICKBMS_ERROR_FILE_WRITE);
    fwrite(data, 1, size, fd);
    fclose(fd);
    return(size);
}



// from NetBSD
void *mymemmem(const void *b1, const void *b2, size_t len1, size_t len2) {
    unsigned char *sp  = (unsigned char *) b1;
    unsigned char *pp  = (unsigned char *) b2;
    unsigned char *eos = sp + len1 - len2;

    if(!(b1 && b2 && len1 && len2))
        return NULL;

    while (sp <= eos) {
        if (*sp == *pp)
            if (memcmp(sp, pp, len2) == 0)
                return sp;
        sp++;
    }
    return NULL;
}



int invalid_chars_to_spaces(u8 *s) {
    if(!s) return(-1);
    for(; *s; s++) {
        if(*s < ' ') *s = ' ';
    }
    return(0);
}



u8 *quickbms_fopen(u8 *fname) {
    int     i;
    u8      *new_fname,
            *mypath,
            *p;

    p = mystrrchrs(fname, "\\/");
    if(p) fname = p + 1;

    new_fname = NULL;
    for(i = 0; ; i++) {
        switch(i) {
            case 0:  mypath = bms_folder;       break;
            case 1:  mypath = exe_folder;       break;
            case 2:  mypath = file_folder;      break;
            case 3:  mypath = current_folder;   break;
            case 4:  mypath = output_folder;    break;
            case 5:  mypath = ".";              break;
            default: mypath = NULL;             break;
        }
        if(!mypath) {
            FREEZ(new_fname)
            break;
        }
        spr(&new_fname, "%s%c%s", mypath, PATHSLASH, fname);

        if(file_exists(new_fname)) break;
    }
    return(new_fname);
}



u8 *skip_begin_string(u8 *p) {
    if(p) {
        while(*p) {
            if(*p > ' ') break;
            p++;
        }
    }
    return(p);
}



u8 *skip_end_string(u8 *p) {
    u8      *l;

    if(p) {
        for(l = p + strlen(p) - 1; l >= p; l--) {
            if(*l > ' ') return(l);
            *l = 0;
        }
    }
    return(p);
}



u8 *skip_delimit(u8 *p) {
    p = skip_begin_string(p);
    skip_end_string(p);
    return(p);
}



u8 *get_fullpath_from_name(u8 *fname) {
    static u8   tmp[PATHSZ + 1];
    u8      *out,
            *p;

    getcwd(tmp, PATHSZ);
    p = mystrrchrs(fname, "\\/");
    if(p) {
        *p++ = 0;
        out = malloc(PATHSZ + 1 + strlen(p) + 1);
        if(!out) STD_ERR(QUICKBMS_ERROR_MEMORY);
        out[0] = 0;

        if(chdir(fname) < 0) {
            strcpy(out, tmp);
        } else {
            getcwd(out, PATHSZ);
            chdir(tmp);
        }
        sprintf(out + strlen(out), "%c%s", PATHSLASH, p);
        p[-1] = PATHSLASH;

    } else {
        out = malloc(PATHSZ + 1 + strlen(fname) + 1);
        if(!out) STD_ERR(QUICKBMS_ERROR_MEMORY);
        sprintf(out, "%s%c%s", tmp, PATHSLASH, fname);
    }
    return(out);
}



// necessary to avoid that Windows handles the format... even if delimited by quotes
u8 **build_filter(u8 *filter) {
    int     i,
            len,
            ret_n;
    u8      *tmp_filter,
            *p,
            *l,
            **ret   = NULL;

    if(!filter || !filter[0]) return(NULL);

    fprintf(stderr, "- filter string: \"%s\"\n", filter);
    tmp_filter = fdload(filter, &len);
    if(!tmp_filter) tmp_filter = mystrdup_simple(filter);

    ret_n = 0;
    for(p = tmp_filter; p && *p; p = l) {
        for(     ; *p &&  strchr(" \t\r\n", *p); p++);

        for(l = p; *l && !strchr(",;|\r\n", *l); l++);
        if(!*l) l = NULL;
        else *l++ = 0;

        p = skip_delimit(p);
        if(!p[0]) continue;

        // "{}.exe" (/bin/find like)
        find_replace_string(p, NULL, "{}", -1, "*", -1);

        // "\"*.exe\""
        len = strlen(p);
        if((p[0] == '\"') && (p[len - 1] == '\"')) {
            len -= 2;
            mymemmove(p, p + 1, len);
            p[len] = 0;
        }

        ret = realloc(ret, (ret_n + 1) * sizeof(u8 *));
        if(!ret) STD_ERR(QUICKBMS_ERROR_MEMORY);
        ret[ret_n] = mystrdup_simple(p);
        ret_n++;
    }

    if(ret) {
        ret = realloc(ret, (ret_n + 1) * sizeof(u8 *));
        if(!ret) STD_ERR(QUICKBMS_ERROR_MEMORY);
        ret[ret_n] = NULL;
    }
    for(i = 0; ret[i]; i++) {
        fprintf(stderr, "- filter %3d: %s\n", (i32)(i + 1), ret[i]);
    }
    FREEZ(tmp_filter)
    return(ret);
}



void dump_cmdline(int argc, char **argv) {
    int     i;

    printf("- command-line arguments:\n");
    for(i = 0; i < argc; i++) {
        printf("  %s\n", argv[i]);
    }
}



void *malloc_copy(void *output, void *input, int size) {
    void    *ret;

    if(!input || (size < 0)) return NULL;
    if(size == -1) ALLOC_ERR;
    ret = realloc(output, size + 1);    // works if both output exists or is NULL
    if(!ret) STD_ERR(QUICKBMS_ERROR_MEMORY);
    memcpy(ret, input, size);
    ((u8 *)ret)[size] = 0;
    return(ret);
}



u8 *get_extension(u8 *fname) {
    u8      *p;

    if(fname) {
        p = strrchr(fname, '.');
        if(p) return(p + 1);
        return(fname + strlen(fname));
    }
    return(fname);
}



u8 *get_filename(u8 *fname) {
    u8      *p;

    if(fname) {
        p = mystrrchrs(fname, "\\/");
        if(p) return(p + 1);
    }
    return(fname);
}



u8 *append_list(u8 **ret_dst, u8 *src) {
    int     dstsz   = 0,
            srcsz   = 0;
    u8      *dst    = NULL;

    if(ret_dst) dst = *ret_dst;

    if(dst) dstsz = strlen(dst);
    if(src) srcsz = strlen(src);

    if(!dstsz) {
        dst = realloc(dst, srcsz + 1);
        if(!dst) STD_ERR(QUICKBMS_ERROR_MEMORY);
    } else {
        dst = realloc(dst, dstsz + 1 + srcsz + 1);
        if(!dst) STD_ERR(QUICKBMS_ERROR_MEMORY);
        dst[dstsz] = ';';
        dstsz++;
    }
    memcpy(dst + dstsz, src, srcsz);
    dst[dstsz + srcsz] = 0;

    if(ret_dst) *ret_dst = dst;
    return(dst);
}



// libiconv takes one megabyte when linked statically, so I prefer to use the Win32 API
#ifdef USE_LIBICONV
    #include <iconv.h>
#endif



// NEVER use sizeof(wchar_t) because on Linux it's 32bit instead of 16!
// sizeof(wchar_t) -> sizeof(u16)



int utf8_to_utf16_chr(u8 *in, int insz, wchar_t *wc, int handle_endianess) {
    int     len = -1;
    wchar_t t16;

    if(!wc) wc = &t16;
    *wc = 0;

#ifdef WIN32
    int     codepage;
    for(codepage = 0; codepage < 2; codepage++) {
        *wc = 0;
        int     i;
        for(i = 1; i < insz; i++) {
            if(MultiByteToWideChar((!codepage) ? CP_UTF8 : CP_ACP, 0, in, i, (void *)wc, 1 /*chars not bytes!*/)) {
                len = i;
                break;
            }
        }
        if((len > 0) && (*wc != 0xfffd)) break;
    }
    if(len > insz) len = -1;
#endif
#ifdef USE_LIBICONV
    if(len <= 0) {
        static  iconv_t ic      = NULL;
        static  int old_endian  = -1;
        if(endian != old_endian) {
            old_endian = endian;
            if(ic) iconv_close(ic);
            //ic = iconv_open((endian == MYLITTLE_ENDIAN) ? "UTF-16LE" : "UTF-16BE", "UTF-8");   // utf8 to utf16
            ic = iconv_open("UTF-16LE", "UTF-8");   // correct, not tested on big endian CPU
        }
        if(ic != (iconv_t)(-1)) {
            size_t  il  = insz,
                    ol  = sizeof(u16) /*sizeof(wchar_t)*/;
            char    *ip = in,
                    *op = (void *)wc;
            if(iconv(ic, &ip, &il, &op, &ol) >= 0) {
                len = ip - (char *)in;
            }
        }
    }
#endif
    if(len <= 0) {
        len = mbtowc(wc, in, insz);
    }
    if(len <= 0) {
        len = 1;
        *wc = in[0];
    }
    if(handle_endianess) {
        if(endian == MYLITTLE_ENDIAN) *wc = swap16le(*wc);
        else                          *wc = swap16be(*wc);
    }
    return len; // input size
}



int utf16_to_utf8_chr(wchar_t wc, u8 *out, int outsz, int handle_endianess) {
    int     len = -1;

    if(handle_endianess) {
        if(endian == MYLITTLE_ENDIAN) wc = swap16le(wc);
        else                          wc = swap16be(wc);
    }
#ifdef WIN32
    len = WideCharToMultiByte(CP_UTF8, 0, (void *)&wc, 1 /*chars not bytes!*/, out, outsz, NULL, NULL);
    if(len > outsz) len = -1;
#endif
#ifdef USE_LIBICONV
    if(len <= 0) {
        static  iconv_t ic      = NULL;
        static  int old_endian  = -1;
        if(endian != old_endian) {
            old_endian = endian;
            if(ic) iconv_close(ic);
            //ic = iconv_open("UTF-8", (endian == MYLITTLE_ENDIAN) ? "UTF-16LE" : "UTF-16BE");   // utf16 to utf8
            ic = iconv_open("UTF-8", "UTF-16LE"); // wc is already endian-converted, so no need of LE/BE, not tested on big endian CPU
        }
        if(ic != (iconv_t)(-1)) {
            size_t  il  = sizeof(u16) /*sizeof(wchar_t)*/,
                    ol  = outsz;
            char    *ip = (void *)&wc,
                    *op = out;
            if(iconv(ic, &ip, &il, &op, &ol) >= 0) {
                len = op - (char *)out;
            }
        }
    }
#endif
    if(len <= 0) {
        len = wctomb(out, wc);
    }
    if(len <= 0) {
        len = 1;
        out[0] = wc;
    }
    return len; // output size
}



int utf8_to_utf16(u8 *in, int insz, u8 *out, int outsz, int rev) {
    int     i,
            o,
            t,
            len;
    wchar_t wc;

    if(!out) return -1;
    if(outsz < 0) return -1;
    if(insz < 0) insz = strlen(in);

#ifdef WIN32
    if(!rev) {
        int     codepage;
        for(codepage = 0; codepage < 2; codepage++) {
            t = outsz / sizeof(u16) /*sizeof(wchar_t)*/;
            o = MultiByteToWideChar((!codepage) ? CP_UTF8 : CP_ACP, 0, in, insz, (void *)out, t);
            if(o > t) o = -1;
            o *= sizeof(u16) /*sizeof(wchar_t)*/;
            for(i = 0; i < o; i += sizeof(u16) /*sizeof(wchar_t)*/) {
                wc = ((wchar_t *)(out + i))[0];
                if(wc == 0xfffd) break;
                if(endian == MYLITTLE_ENDIAN) ((wchar_t *)(out + i))[0] = swap16le(wc);
                else                          ((wchar_t *)(out + i))[0] = swap16be(wc);
            }
            if(i < o) continue;
            break;
        }
    } else {
        t = insz / sizeof(u16) /*sizeof(wchar_t)*/;
        o = WideCharToMultiByte(CP_UTF8, 0, (void *)in, t, out, outsz, NULL, NULL);
        if(o > t) o = -1;
    }
    if(o <= 0)
#endif
    {
#ifdef USE_LIBICONV
    iconv_t ic;
    if(!rev) ic = iconv_open((endian == MYLITTLE_ENDIAN) ? "UTF-16LE" : "UTF-16BE", "UTF-8");   // utf8 to utf16
    else     ic = iconv_open("UTF-8", (endian == MYLITTLE_ENDIAN) ? "UTF-16LE" : "UTF-16BE");   // utf16 to utf8
    if(ic != (iconv_t)(-1)) {

        size_t  il  = insz,
                ol  = outsz;
        char    *ip = in,
                *op = out;
        do {
            len = iconv(ic, &ip, &il, &op, &ol);
        } while(len > 0);
        iconv_close(ic);
        o = op - (char *)out;

    } else
#endif
    {

        o = 0;
        if(!rev) {  // 8 to 16
            for(i = 0; i < insz; i += len) {
                len = mbtowc(&wc, in + i, insz - i);
                if(len <= 0) {
                    wc = in[i];
                    len = 1;
                }
                if(endian == MYLITTLE_ENDIAN) {
                    out[o++] = wc & 0xff;
                    out[o++] = (wc >> 8) & 0xff;
                } else {
                    out[o++] = (wc >> 8) & 0xff;
                    out[o++] = wc & 0xff;
                }
            }
        } else {    // 16 to 8
            len = sizeof(u16) /*sizeof(wchar_t)*/;
            for(i = 0; (i + len) <= insz; i += len) {
                if(endian == MYLITTLE_ENDIAN) {
                    wc = in[i] | (in[i + 1] << 8);
                } else {
                    wc = (in[i] << 8) | in[i + 1];
                }
                t = wctomb(out + o, wc);
                if(t <= 0) {
                    out[o] = wc;
                    t = 1;
                }
                o += t;
            }
        }
    }
    }

    out[o++] = 0;
    if(!rev) out[o++] = 0;

    return o;
}



#ifdef WIN32
static PVOID WINAPI (*_AddVectoredContinueHandler)(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler) = NULL;
static PVOID WINAPI (*_AddVectoredExceptionHandler)(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler) = NULL;
int winapi_missing(void) {
    static HMODULE kernel32 = NULL;

    if(!kernel32) kernel32 = GetModuleHandle("kernel32.dll");   // LoadLibrary may be dangerous
    if(kernel32) {
        if(!_AddVectoredContinueHandler)
            _AddVectoredContinueHandler = (void *)GetProcAddress(kernel32, "AddVectoredContinueHandler");
        if(!_AddVectoredExceptionHandler)
            _AddVectoredExceptionHandler = (void *)GetProcAddress(kernel32, "AddVectoredExceptionHandler");
        return 0;
    }
    return -1;
}
#endif



void fix_my_d_option(u8 *fname, u8 *fdir) {
    static u8   tmp[PATHSZ + 1];
    int     i,
            eofdir = 0;
    u8      *p,
            *s,
            a,
            b;

    if(!fname) return;
    if(!fdir) {
        tmp[0] = 0;
        getcwd(tmp, PATHSZ);
        fdir = tmp;
    }

    for(p = fname, s = fdir; *p && *s; p++, s++) {

        // ./
        while((p[0] == '.') && strchr("\\/", p[1])) p += 2;
        while((s[0] == '.') && strchr("\\/", s[1])) s += 2;

        a = tolower(*p);
        b = tolower(*s);

        // \/
        if(strchr("\\/", a) && strchr("\\/", b)) continue;

        // different
        if(a != b) break;
    }
    if(!*p && !*s) eofdir = 1;

    // skip the next \/
    while(*p && strchr("\\/", *p)) p++;

    // going back till the previous \/ or fname (in case the previous check fails)
    if(!eofdir) {
        for(--p; p >= fname; p--) {
            if(strchr("\\/", *p)) break;
        }
        p++;
    }

    if(p > fname) {
        for(i = 0; p[i]; i++) {
            fname[i] = p[i];
        }
        fname[i] = 0;
    }
}



u32 mycrc(u8 *data, int datasz) {
    u32     crc;
    crc = adler32(0L, Z_NULL, 0);
    crc = adler32(crc, data, datasz);
    return crc;
}

