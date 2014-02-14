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

// QuickBMS perform* encryption and compression operations



#define string_to_execute_VAR       (1 << 0)
#define string_to_execute_INPUT     (1 << 1)
#define string_to_execute_OUTPUT    (1 << 2)
#define string_to_execute_REDIRECT  (1 << 3)
#define string_to_execute_MULTI     (1 << 4)

u8 *string_to_execute(u8 *str, u8 *input, u8 *output, int *res, int exec_checks) {
#define string_to_execute_realloc(X) \
            tmp = X; \
            if((exesz + tmp) >= totsz) { \
                totsz = exesz + tmp + 1024; \
                exe = realloc(exe, totsz + 1); \
                if(!exe) STD_ERR(QUICKBMS_ERROR_MEMORY); \
            }
    int     tmp,
            idx,
            exesz   = 0,
            totsz   = 0;
    u8      *var,
            *exe,
            *p,
            *l,
            *limit;

    if(!str) return(NULL);
    if(res) *res = 0;
    totsz = strlen(str) + 1024;
    exe = malloc(totsz + 1);
    if(!exe) STD_ERR(QUICKBMS_ERROR_MEMORY);
    exe[0] = 0;
    limit = str + strlen(str);
    for(p = str; *p && (p < limit);) {
        if((p[0] == '>') || (p[0] == '<')) {
            if(res) *res |= string_to_execute_REDIRECT;
        }
        if((p[0] == ';') || (p[0] == '&') || (p[0] == '|')) {
            if(res) *res |= string_to_execute_MULTI;
        }

        if(p[0] == '%') {
            p++;
            if(p[1] == '%') continue;
            l = strchr(p, '%');
            if(!l) continue;
            idx = get_var_from_name(p, l - p);
            if(idx < 0) continue;
            var = get_var(idx);
            if(!var) continue;
            if(exec_checks && mystrchrs(var, "\"\'><;&|")) {
            } else {
                string_to_execute_realloc(1 + strlen(var) + 1)
                exesz += sprintf(exe + exesz, "\"%s\"", var);
            }
            p = l + 1;
            if(res) *res |= string_to_execute_VAR;

        } else if(!strncmp(p, "#INPUT#", 7)) {
            //if(input) {
                if(exec_checks && mystrchrs(input, "\"\'><;&|")) {
                } else {
                    string_to_execute_realloc(1 + strlen(input) + 1)
                    exesz += sprintf(exe + exesz, "\"%s\"", input);
                }
            //}
            p += 7;
            if(res) *res |= string_to_execute_INPUT;

        } else if(!strncmp(p, "#OUTPUT#", 8)) {
            //if(output) {
                if(exec_checks && mystrchrs(output, "\"\'><;&|")) {
                } else {
                    string_to_execute_realloc(1 + strlen(output) + 1)
                    exesz += sprintf(exe + exesz, "\"%s\"", output);
                }
            //}
            p += 8;
            if(res) *res |= string_to_execute_OUTPUT;

        } else {
            string_to_execute_realloc(1)
            exe[exesz++] = *p;
            p++;
        }
    }
    exe = realloc(exe, exesz + 1);
    if(!exe) STD_ERR(QUICKBMS_ERROR_MEMORY);
    exe[exesz] = 0;
    return(exe);
}



u8 *quickbms_execute_pipe_path(u8 *mycmd) {
    int     len,
            tmp,
            quote = 0,
            new_exelen;
    u8      *old_path,
            *old_exe,
            *new_exe,
            *p,
            *l;

    for(p = mycmd; *p && (*p <= ' '); p++);
    l = NULL;
         if(*p == '\"') l = strchr(++p, '\"');
    else if(*p == '\'') l = strchr(++p, '\'');
    if(!l) for(l = p; *l && (*l > ' '); l++);
    tmp = *l;
    *l = 0;
    old_path = mystrdup_simple(p);
    len = strlen(old_path);
#ifdef WIN32
    if((len < 4) || stricmp(old_path + len - 4, ".exe")) {
        old_path = realloc(old_path, len + 4 + 1);
        if(!old_path) STD_ERR(QUICKBMS_ERROR_MEMORY);
        strcpy(old_path + len, ".exe");
    }
#endif
    old_exe = get_filename(old_path);
    *l = tmp;
    if(*l) l++;

    if((old_path[0] == '/') || strchr(old_path, ':')) {
        // do nothing, it's an absolute path
    } else {
        new_exe = quickbms_fopen(old_exe);
        if(new_exe) {
            if(mystrchrs(new_exe, " \t")) quote = 1;
            tmp = l - mycmd;
            len = strlen(l);
            new_exelen = strlen(new_exe);
            mycmd = realloc(mycmd, quote + new_exelen + quote + 1 + len + 1);
            if(!mycmd) STD_ERR(QUICKBMS_ERROR_MEMORY);
            mymemmove(mycmd + quote + new_exelen + quote + 1, mycmd + tmp, len + 1);
            p = mycmd;
            if(quote) *p++ = '\"';
            memcpy(p, new_exe, new_exelen);
            p += new_exelen;
            if(quote) *p++ = '\"';
            *p++ = ' ';
            FREEZ(new_exe)
        } else {
            // return the original command, it will be found automatically by system()
        }
    }
    FREEZ(old_path)
    return(mycmd);
}



// it's necessary to use real files because
// the executed command may not support sequential data (like pipes)
// so I MUST guarantee the 100% of compatibility even if it's slower
// and occupy more resources
int quickbms_execute_pipe(u8 *cmdstr, u8 *in, int insz, u8 **out, int outsz, u8 *my_fname) {
    FILE    *fd;
    int     res,
            t,
            size    = -1;
    u8      *in_fname   = NULL,
            *out_fname  = NULL,
            *mycmd      = NULL;

    if(!cmdstr) return(-1);
    if(!in) {
        if(!my_fname) return(-1);
    }
    if(insz < 0) return(-1);

    if(!enable_execute_pipe) {
        fprintf(stderr, "\n"
            "- the script has requested to run an executable:\n"
            "  %s\n"
            "\n"
            "  NOTE THAT I ASK THIS CONFIRMATION ONLY NOW SO CHECK THE SCRIPT BECAUSE YOU\n"
            "  WILL BE NO LONGER PROMPTED TO CONFIRM THE NEXT USAGE OF THE EXECUTE COMMAND!\n"
            "  THIS FEATURE IS DANGEROUS SO BE SURE TO KNOW WHAT YOU ARE DOING\n"
            "\n"
            "  do you want to continue (y/N)? ",
            cmdstr);
        if(get_yesno(NULL) != 'y') myexit(QUICKBMS_ERROR_USER);

        // *fixed, added warning*
        // do NOT enable enable_execute_pipe because if we have
        // multiple comtype then only the first one will be
        // visualized and the others may be malicious!
        enable_execute_pipe = 1;
    }

    if(!my_fname) {
        quickbms_tmpname(&in_fname,  NULL, "tmp");
        mydump(in_fname, in, insz);
        quickbms_tmpname(&out_fname, NULL, "tmp");
    }

    // I'm forced to use system() because it's possible that
    // we have a command which uses stdout and so I must grant
    // it's compatibility... yes I know that it's a big security
    // problem!

    mycmd = string_to_execute(cmdstr,
        my_fname ? my_fname : in_fname,
        my_fname ? my_fname : out_fname,
        &res, 1);
    mycmd = quickbms_execute_pipe_path(mycmd);
    invalid_chars_to_spaces(mycmd);
#ifdef WIN32
    t = strlen(mycmd);
    mycmd = realloc(mycmd, 1 + t + 1);
    if(!mycmd) STD_ERR(QUICKBMS_ERROR_MEMORY);
    mymemmove(mycmd + 1, mycmd, t + 1);
    mycmd[0] = '@'; // crazy but necessary!
#endif
    fprintf(stderr, "- execute:\n  %s\n", mycmd);
    system(mycmd);  // do NOT check the return value
    FREEZ(mycmd);

    fd = NULL;
    if(res & string_to_execute_OUTPUT) {
        if(out_fname) {
            fd = fopen(out_fname, "rb");
            if(!fd) goto quit;
        }
    } else if(res & string_to_execute_INPUT) {
        if(in_fname) {
            fd = fopen(in_fname, "rb");
            if(!fd) goto quit;
        }
    }
    if(fd) {
        fseek(fd, 0, SEEK_END);
        size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        if(out) {
            if(outsz < size) {
                outsz = size;
                if(size == -1) ALLOC_ERR;
                *out = realloc(*out, size + 1);
                if(!*out) STD_ERR(QUICKBMS_ERROR_MEMORY);
                (*out)[size] = 0;
            }
            size = fread(*out, 1, size, fd);
        } else {
            if(insz < size) size = insz;
            size = fread(in, 1, size, fd);
        }
        fclose(fd);
    }

quit:
    if(!my_fname) {
        unlink(in_fname);
        FREEZ(in_fname)
        unlink(out_fname);
        FREEZ(out_fname)
    }
    return(size);
}



int parse_bms(FILE *fds, u8 *inputs, int cmd);
int CMD_CallDLL_func(int cmd, u8 *input, u8 *output);



int quickbms_calldll_pipe(u8 *cmdstr, u8 *in, int insz, u8 *out, int outsz) {
    int     cmd,
            ret;
    u8      *calldll_cmd = NULL,
            *p;

    if(!cmdstr) return(-1);
    cmdstr = skip_delimit(cmdstr);

    p = mystrchrs(cmdstr, " \t");
    if(!p) return(-1);
    *p = 0;
    if(!stricmp(cmdstr, "calldll")) cmdstr = p + 1;
    *p = ' ';

    p = malloc(32 + strlen(cmdstr) + 1);
    if(!p) STD_ERR(QUICKBMS_ERROR_MEMORY);
    sprintf(p, "calldll %s", cmdstr);

    // #INPUT# -> "#INPUT#" to avoid the parse_bms comments
         if(in  && out)  calldll_cmd = string_to_execute(p, "#INPUT#",  "#OUTPUT#", &ret, 0);
    else if(!in && out)  calldll_cmd = string_to_execute(p, "#OUTPUT#", "#OUTPUT#", &ret, 0);
    else if(in  && !out) calldll_cmd = string_to_execute(p, "#INPUT#",  "#INPUT#",  &ret, 0);
    else                 calldll_cmd = mystrdup_simple(p);
    FREEZ(p);

    for(cmd = 0; CMD.type != CMD_NONE; cmd++);
    cmd++;

    //if((cmd + 1) < MAX_CMDS) // not needed because parse_bms already does this check

    parse_bms(NULL, calldll_cmd, cmd);
    ret = CMD_CallDLL_func(cmd, in, out);
    if(ret & string_to_execute_INPUT) {
        if(!(ret & string_to_execute_OUTPUT)) {
            if(out) {
                if(outsz > insz) outsz = insz;
                memcpy(out, in, outsz);
            }
        }
    }

    FREEZ(calldll_cmd);
    return(outsz);
}



ICE_KEY *do_ice_key(u8 *key, int keysz, int icecrypt) {
    ICE_KEY *ik;
    int     i       = 0,
            k,
            level   = 0;
    u8      buf[1024];

    if(keysz == 8) {
        level = 0;
    } else if(!(keysz % 16)) {
        level = keysz / 16;
    } else {
        fprintf(stderr, "\nError: your ICE key has an incorrect size\n");
        myexit(QUICKBMS_ERROR_ENCRYPTION);
    }

    if(icecrypt) {
        memset(buf, 0, sizeof(buf));
        for(k = 0; k < keysz; k++) {
            u8      c = key[k] & 0x7f;
            int     idx = i / 8;
            int     bit = i & 7;

            if (bit == 0) {
                buf[idx] = (c << 1);
            } else if (bit == 1) {
                buf[idx] |= c;
            } else {
                buf[idx] |= (c >> (bit - 1));
                buf[idx + 1] = (c << (9 - bit));
            }
            i += 7;
        }
        key = buf;
    }
    ik = ice_key_create(level);
    if(!ik) return(NULL);
    ice_key_set(ik, key);
    return(ik);
}



#ifndef DISABLE_MCRYPT
MCRYPT quick_mcrypt_check(u8 *type) {
    u8      tmp[64],
            *p,
            *mode,
            *algo;

    if(!type) type = "";
    mystrcpy(tmp, type, sizeof(tmp));

    // myisalnum gets also the '-' which is a perfect thing
    // NEVER use '-' as delimiter because "rijndael-*" use it

    algo = tmp;
    if(!strnicmp(tmp, "mcrypt", 6)) {
        for(algo = tmp + 6; *algo; algo++) {
            if(myisalnum(*algo)) break;
        }
    }
    p = strchr(algo, '_');
    if(!p) p = strchr(algo, ',');
    if(p) {
        *p = 0;
        mode = p + 1;
    } else {
        mode = MCRYPT_ECB;
    }
    return(mcrypt_module_open(algo, NULL, mode, NULL));
}
#endif



#ifndef DISABLE_TOMCRYPT
// nonce:001122334455667788 header:aabbccddeeff0011 ivec:FFff00112233AAbb tweak:0011223344
void tomcrypt_lame_ivec(TOMCRYPT *ctx, u8 *ivec, int ivecsz) {
    int     t,
            *y;
    u8      *p,
            *s,
            *l,
            **x,
            *limit;

    if(!ctx || !ivec || (ivecsz < 0)) return;
    limit = ivec + ivecsz;  // ivec is NULL delimited

    for(p = ivec; p < limit; p = l + 1) {
        while(*p && (*p <= ' ')) p++;
        l = strchr(p, ' ');
        if(!l) l = strchr(p, '\t');
        if(!l) l = limit;

        // ':' in all of the following
        if((s = stristr(p, "nonce:")) || (s = stristr(p, "salt:")) ||
           (s = stristr(p, "adata:")) || (s = stristr(p, "skey:")) ||
           (s = stristr(p, "key2:"))  || (s = stristr(p, /*salt_*/"key:"))) {
            x = &ctx->nonce;
            y = &ctx->noncelen;
        } else if((s = stristr(p, "header:"))) {
            x = &ctx->header;
            y = &ctx->headerlen;
        } else if((s = stristr(p, "ivec:"))) {
            x = &ctx->ivec;
            y = &ctx->ivecsz;
        } else if((s = stristr(p, "tweak:"))) {
            x = &ctx->tweak;
            y = NULL;
        } else {
            break;
        }

        s = strchr(s, ':');
        if(!s) break;
        s++;
        if(l < s) break;
        *x = malloc((l - s) + 1);   // / 2, but it's ok (+1 is not needed)
        if(!*x) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(y) *y = 0;

        t = unhex(s, l - s, *x, l - s);
        if(t < 0) {
            FREEZ(*x)
        }
        if((t >= 0) && y) *y = t;
    }

    if(!ctx->ivec) {
        ctx->ivec = malloc(ivecsz);
        if(!ctx->ivec) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memcpy(ctx->ivec, ivec, ivecsz);
        ctx->ivecsz = ivecsz;
    }
}



// badly implemented because it's intended only as a test
TOMCRYPT *tomcrypt_doit(TOMCRYPT *ctx, u8 *type, u8 *in, int insz, u8 *out, int outsz, i32 *ret) {
    static int      init = 0;
    symmetric_ECB   ecb;
    symmetric_CFB   cfb;
    symmetric_OFB   ofb;
    symmetric_CBC   cbc;
    symmetric_CTR   ctr;
    symmetric_LRW   lrw;
    symmetric_F8    f8;
    symmetric_xts   xts;
    long    tmp;
    i32     stat;
    int     keysz,
            ivecsz,
            noncelen,
            headerlen,
            use_tomcrypt    = 0;
    u8      tag[64] = "",
            desc[64],
            *p,
            *l,
            *key,
            *ivec,
            *nonce,
            *header,
            *tweak;

    static int blowfish_idx = -1;
    static int rc5_idx = -1;
    static int rc6_idx = -1;
    static int rc2_idx = -1;
    static int saferp_idx = -1;
    static int safer_k64_idx = -1;
    static int safer_k128_idx = -1;
    static int safer_sk64_idx = -1;
    static int safer_sk128_idx = -1;
    static int rijndael_idx = -1;
    static int aes_idx = -1;
    static int rijndael_enc_idx = -1;
    static int aes_enc_idx = -1;
    static int xtea_idx = -1;
    static int twofish_idx = -1;
    static int des_idx = -1;
    static int des3_idx = -1;
    static int cast5_idx = -1;
    static int noekeon_idx = -1;
    static int skipjack_idx = -1;
    static int khazad_idx = -1;
    static int anubis_idx = -1;
    static int kseed_idx = -1;
    static int kasumi_idx = -1;
    static int multi2_idx = -1;

    static int chc_idx = -1;
    static int whirlpool_idx = -1;
    static int sha512_idx = -1;
    static int sha384_idx = -1;
    static int sha256_idx = -1;
    static int sha224_idx = -1;
    static int sha1_idx = -1;
    static int md5_idx = -1;
    static int md4_idx = -1;
    static int md2_idx = -1;
    static int tiger_idx = -1;
    static int rmd128_idx = -1;
    static int rmd160_idx = -1;
    static int rmd256_idx = -1;
    static int rmd320_idx = -1;

    if(!init) {
        #define TOMCRYPT_REGISTER_CIPHER(X) \
            register_cipher(&X##_desc); \
            X##_idx = find_cipher(#X);
            //if(X##_idx < 0) goto quit;
        #define TOMCRYPT_REGISTER_HASH(X) \
            register_hash(&X##_desc); \
            X##_idx = find_hash(#X);
            //if(X##_idx < 0) goto quit;

        TOMCRYPT_REGISTER_CIPHER(blowfish)
        TOMCRYPT_REGISTER_CIPHER(rc5)
        TOMCRYPT_REGISTER_CIPHER(rc6)
        TOMCRYPT_REGISTER_CIPHER(rc2)
        //TOMCRYPT_REGISTER_CIPHER(saferp)
        //TOMCRYPT_REGISTER_CIPHER(safer_k64)
        //TOMCRYPT_REGISTER_CIPHER(safer_k128)
        //TOMCRYPT_REGISTER_CIPHER(safer_sk64)
        //TOMCRYPT_REGISTER_CIPHER(safer_sk128)
        register_cipher(&safer_k64_desc);   safer_k64_idx   = find_cipher("safer-k64");
        register_cipher(&safer_k128_desc);  safer_k128_idx  = find_cipher("safer-k128");
        register_cipher(&safer_sk64_desc);  safer_sk64_idx  = find_cipher("safer-sk64");
        register_cipher(&safer_sk128_desc); safer_sk128_idx = find_cipher("safer-sk128");
        TOMCRYPT_REGISTER_CIPHER(rijndael)
        //TOMCRYPT_REGISTER_CIPHER(aes)
        register_cipher(&aes_desc);     aes_idx = find_cipher("rijndael");
        //TOMCRYPT_REGISTER_CIPHER(rijndael_enc)
        //TOMCRYPT_REGISTER_CIPHER(aes_enc)
        TOMCRYPT_REGISTER_CIPHER(xtea)
        TOMCRYPT_REGISTER_CIPHER(twofish)
        TOMCRYPT_REGISTER_CIPHER(des)
        //TOMCRYPT_REGISTER_CIPHER(des3)
        register_cipher(&des3_desc);     des3_idx = find_cipher("3des");
        TOMCRYPT_REGISTER_CIPHER(cast5)
        TOMCRYPT_REGISTER_CIPHER(noekeon)
        TOMCRYPT_REGISTER_CIPHER(skipjack)
        TOMCRYPT_REGISTER_CIPHER(khazad)
        TOMCRYPT_REGISTER_CIPHER(anubis)
        //TOMCRYPT_REGISTER_CIPHER(kseed)
        register_cipher(&kseed_desc);   kseed_idx = find_cipher("seed");
        TOMCRYPT_REGISTER_CIPHER(kasumi)
        TOMCRYPT_REGISTER_CIPHER(multi2)

        TOMCRYPT_REGISTER_HASH(chc)
        TOMCRYPT_REGISTER_HASH(whirlpool)
        TOMCRYPT_REGISTER_HASH(sha512)
        TOMCRYPT_REGISTER_HASH(sha384)
        TOMCRYPT_REGISTER_HASH(sha256)
        TOMCRYPT_REGISTER_HASH(sha224)
        TOMCRYPT_REGISTER_HASH(sha1)
        TOMCRYPT_REGISTER_HASH(md5)
        TOMCRYPT_REGISTER_HASH(md4)
        TOMCRYPT_REGISTER_HASH(md2)
        TOMCRYPT_REGISTER_HASH(tiger)
        TOMCRYPT_REGISTER_HASH(rmd128)
        TOMCRYPT_REGISTER_HASH(rmd160)
        TOMCRYPT_REGISTER_HASH(rmd256)
        TOMCRYPT_REGISTER_HASH(rmd320)

        init = 1;
    }

    if(type) {
        mystrcpy(desc, type, sizeof(desc));

        ctx = calloc(1, sizeof(TOMCRYPT));
        if(!ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        ctx->idx = -1;  // 0 is AES

        #define TOMCRYPT_IDX(X,Y) \
            else if(!stricmp(p, #X)) { \
                ctx->idx = X##_idx; \
                Y; \
            }

        for(p = desc; *p; p = l + 1) {
            l = strchr(p, ' ');
            if(l) *l = 0;

            while(*p && (*p <= ' ')) p++;
            if(!strnicmp(p, "tomcrypt", 8)) {
                use_tomcrypt = 1;
                p += 8;
            }
            if(!strnicmp(p, "libtomcrypt", 11)) {
                use_tomcrypt = 1;
                p += 11;
            }
            while(*p && (*p <= ' ')) p++;

            if(!stricmp(p, "")) {}  // needed because the others are "else"

            TOMCRYPT_IDX(blowfish,      ctx->cipher = 1)
            TOMCRYPT_IDX(rc5,           ctx->cipher = 1)
            TOMCRYPT_IDX(rc6,           ctx->cipher = 1)
            TOMCRYPT_IDX(rc2,           ctx->cipher = 1)
            TOMCRYPT_IDX(saferp,        ctx->cipher = 1)
            TOMCRYPT_IDX(safer_k64,     ctx->cipher = 1)
            TOMCRYPT_IDX(safer_k128,    ctx->cipher = 1)
            TOMCRYPT_IDX(safer_sk64,    ctx->cipher = 1)
            TOMCRYPT_IDX(safer_sk128,   ctx->cipher = 1)
            TOMCRYPT_IDX(rijndael,      ctx->cipher = 1)
            TOMCRYPT_IDX(aes,           ctx->cipher = 1)
            TOMCRYPT_IDX(rijndael_enc,  ctx->cipher = 1)
            TOMCRYPT_IDX(aes_enc,       ctx->cipher = 1)
            TOMCRYPT_IDX(xtea,          ctx->cipher = 1)
            TOMCRYPT_IDX(twofish,       ctx->cipher = 1)
            TOMCRYPT_IDX(des,           ctx->cipher = 1)
            TOMCRYPT_IDX(des3,          ctx->cipher = 1)
            TOMCRYPT_IDX(cast5,         ctx->cipher = 1)
            TOMCRYPT_IDX(noekeon,       ctx->cipher = 1)
            TOMCRYPT_IDX(skipjack,      ctx->cipher = 1)
            TOMCRYPT_IDX(khazad,        ctx->cipher = 1)
            TOMCRYPT_IDX(anubis,        ctx->cipher = 1)
            TOMCRYPT_IDX(kseed,         ctx->cipher = 1)
            TOMCRYPT_IDX(kasumi,        ctx->cipher = 1)
            TOMCRYPT_IDX(multi2,        ctx->cipher = 1)

            TOMCRYPT_IDX(chc,           ctx->hash = 1)
            TOMCRYPT_IDX(whirlpool,     ctx->hash = 1)
            TOMCRYPT_IDX(sha512,        ctx->hash = 1)
            TOMCRYPT_IDX(sha384,        ctx->hash = 1)
            TOMCRYPT_IDX(sha256,        ctx->hash = 1)
            TOMCRYPT_IDX(sha224,        ctx->hash = 1)
            TOMCRYPT_IDX(sha1,          ctx->hash = 1)
            TOMCRYPT_IDX(md5,           ctx->hash = 1)
            TOMCRYPT_IDX(md4,           ctx->hash = 1)
            TOMCRYPT_IDX(md2,           ctx->hash = 1)
            TOMCRYPT_IDX(tiger,         ctx->hash = 1)
            TOMCRYPT_IDX(rmd128,        ctx->hash = 1)
            TOMCRYPT_IDX(rmd160,        ctx->hash = 1)
            TOMCRYPT_IDX(rmd256,        ctx->hash = 1)
            TOMCRYPT_IDX(rmd320,        ctx->hash = 1)

            else if(stristr(p, "ecb"))      ctx->cipher = 1;
            else if(stristr(p, "cfb"))      ctx->cipher = 2;
            else if(stristr(p, "ofb"))      ctx->cipher = 3;
            else if(stristr(p, "cbc"))      ctx->cipher = 4;
            else if(stristr(p, "ctr"))      ctx->cipher = 5;
            else if(stristr(p, "lrw"))      ctx->cipher = 6;
            else if(stristr(p, "f8"))       ctx->cipher = 7;
            else if(stristr(p, "xts"))      ctx->cipher = 8;

            else if(stristr(p, "hmac"))     ctx->cipher = 10;
            else if(stristr(p, "omac"))     ctx->cipher = 11;
            else if(stristr(p, "pmac"))     ctx->cipher = 12;
            else if(stristr(p, "eax"))      ctx->cipher = 13;
            else if(stristr(p, "ocb"))      ctx->cipher = 14;
            else if(stristr(p, "ccm"))      ctx->cipher = 15;
            else if(stristr(p, "gcm"))      ctx->cipher = 16;
            else if(stristr(p, "pelican"))  ctx->cipher = 17;
            else if(stristr(p, "xcbc"))     ctx->cipher = 18;
            else if(stristr(p, "f9"))       ctx->cipher = 19;

            if(!l) break;
            *l = ' ';
        }

        if(!use_tomcrypt || (ctx->idx < 0)) {
            FREEZ(ctx)
        }
        return(ctx);
    }

    key         = ctx->key;
    keysz       = ctx->keysz;
    ivec        = ctx->ivec;
    ivecsz      = ctx->ivecsz;
    nonce       = ctx->nonce;
    noncelen    = ctx->noncelen;
    header      = ctx->header;
    headerlen   = ctx->headerlen;
    tweak       = ctx->tweak;

    //if(outsz > insz) outsz = insz;
    tmp = outsz;

    #define TOMCRYPT_CRYPT_MODE(X) \
        if(X##_setiv(ivec, ivecsz, &X)) goto quit; \
        if(encrypt_mode) { if(X##_encrypt(in, out, insz, &X)) goto quit; } \
        else { if(X##_decrypt(in, out, insz, &X)) goto quit; } \
        X##_done(&X);

    if(ret) *ret = 0;
    if(ctx->idx < 0) ctx->idx = 0;
    if(ctx->hash) {
        if(hash_memory(ctx->idx, in, insz, out, &tmp)) goto quit;

    } else if(ctx->cipher == 1) {
        if(ecb_start(ctx->idx,       key, keysz, 0, &ecb)) goto quit;
        if(encrypt_mode) { if(ecb_encrypt(in, out, insz, &ecb)) goto quit; }
        else { if(ecb_decrypt(in, out, insz, &ecb)) goto quit; }
        ecb_done(&ecb);

    } else if(ctx->cipher == 2) {
        if(cfb_start(ctx->idx, ivec, key, keysz, 0, &cfb)) goto quit;
        TOMCRYPT_CRYPT_MODE(cfb)

    } else if(ctx->cipher == 3) {
        if(ofb_start(ctx->idx, ivec, key, keysz, 0, &ofb)) goto quit;
        TOMCRYPT_CRYPT_MODE(ofb)

    } else if(ctx->cipher == 4) {
        if(cbc_start(ctx->idx, ivec, key, keysz, 0, &cbc)) goto quit;
        TOMCRYPT_CRYPT_MODE(cbc)

    } else if(ctx->cipher == 5) {
        if(ctr_start(ctx->idx, ivec, key, keysz, 0, CTR_COUNTER_LITTLE_ENDIAN, &ctr)) goto quit;
        TOMCRYPT_CRYPT_MODE(ctr)

    } else if(ctx->cipher == 6) {
        if(lrw_start(ctx->idx, ivec, key, keysz, tweak, 0, &lrw)) goto quit;
        TOMCRYPT_CRYPT_MODE(lrw)

    } else if(ctx->cipher == 7) {
        if(f8_start(ctx->idx, ivec, key, keysz, nonce, noncelen, 0, &f8)) goto quit;
        TOMCRYPT_CRYPT_MODE(f8)

    } else if(ctx->cipher == 8) {
        if(xts_start(ctx->idx, key, nonce, keysz, 0, &xts)) goto quit;
        if(encrypt_mode) { if(xts_encrypt(in, insz, out, tweak, &xts)) goto quit; }
        else { if(xts_decrypt(in, insz, out, tweak, &xts)) goto quit; }
        xts_done(&xts);

    } else if(ctx->cipher == 10) {
        if(hmac_memory(ctx->idx, key, keysz, in, insz, out, &tmp)) goto quit;

    } else if(ctx->cipher == 11) {
        if(omac_memory(ctx->idx, key, keysz, in, insz, out, &tmp)) goto quit;

    } else if(ctx->cipher == 12) {
        if(pmac_memory(ctx->idx, key, keysz, in, insz, out, &tmp)) goto quit;

    } else if(ctx->cipher == 13) {
        tmp = sizeof(tag);
        if(encrypt_mode) {
            if(eax_encrypt_authenticate_memory(
                ctx->idx,
                key, keysz,
                nonce, noncelen,
                header, headerlen,
                in, insz,
                out,
                tag, &tmp)) goto quit;
        } else {
            if(eax_decrypt_verify_memory(
                ctx->idx,
                key, keysz,
                nonce, noncelen,
                header, headerlen,
                in, insz,
                out,
                tag, tmp,
                &stat)) goto quit;
        }
        tmp = insz;

    } else if(ctx->cipher == 14) {
        tmp = sizeof(tag);
        if(encrypt_mode) {
            if(ocb_encrypt_authenticate_memory(
                ctx->idx,
                key, keysz,
                nonce,
                in, insz,
                out,
                tag, &tmp)) goto quit;
        } else {
            if(ocb_decrypt_verify_memory(
                ctx->idx,
                key, keysz,
                nonce,
                in, insz,
                out,
                tag, tmp,
                &stat)) goto quit;
        }
        tmp = insz;

    } else if(ctx->cipher == 15) {
        tmp = sizeof(tag);
        if(ccm_memory(
            ctx->idx,
            key, keysz,
            NULL, //uskey,
            nonce, noncelen,
            header, headerlen,
            in, insz,
            out,
            tag, &tmp,
            encrypt_mode ? CCM_ENCRYPT: CCM_DECRYPT)) goto quit;
        tmp = insz;

    } else if(ctx->cipher == 16) {
        tmp = sizeof(tag);
        if(gcm_memory(
            ctx->idx,
            key, keysz,
            ivec, ivecsz,
            nonce, noncelen, //adata, adatalen,
            in, insz,
            out,
            tag, &tmp,
            encrypt_mode ? GCM_ENCRYPT: GCM_DECRYPT)) goto quit;
        tmp = insz;

    } else if(ctx->cipher == 17) {
        if(pelican_memory(key, keysz, in, insz, out)) goto quit;

    } else if(ctx->cipher == 18) {
        if(xcbc_memory(ctx->idx, key, keysz, in, insz, out, &tmp)) goto quit;

    } else if(ctx->cipher == 19) {
        if(f9_memory(ctx->idx, key, keysz, in, insz, out, &tmp)) goto quit;
    }
    if(ret) *ret = tmp;
    return(ctx);
quit:
    return(NULL);
}
#endif



// if datalen is negative then it will return 0 if encryption is enabled or -1 if disabled
int perform_encryption(u8 *data, int datalen) {
#define ENCRYPT_BLOCKS(X,Y) { \
            tmp = datalen / X; \
            for(i = 0; i < tmp; i++) { \
                Y; \
                data += X; \
            } \
        }

#ifndef DISABLE_SSL
    EVP_MD_CTX  *tmpctx;
    u8      digest[EVP_MAX_MD_SIZE],
            digest_hex[(EVP_MAX_MD_SIZE * 2) + 1];
#endif
    u_int   crc = 0;
    int     i   = 0;
    i32     tmp = 0;

    // if(datalen <= 0) NEVER ENABLE THIS because it's needed
    // if(!data)        NEVER

    QUICK_CRYPT_CASE(wincrypt_ctx)
        if(!encrypt_mode) datalen = wincrypt_decrypt(wincrypt_ctx, data, datalen);
        else              datalen = wincrypt_encrypt(wincrypt_ctx, data, datalen);

#ifndef DISABLE_SSL
    } else QUICK_CRYPT_CASE(evp_ctx)
        tmp = datalen;
        if(reimport) {
            i = evp_ctx->encrypt;
            evp_ctx->encrypt = encrypt_mode;
        }
        EVP_CipherUpdate(evp_ctx, data, &tmp, data, datalen);
        if(reimport) evp_ctx->encrypt = i;
        //EVP_CipherFinal(evp_ctx, data + datalen, &tmp);   // it causes tons of problems
        //datalen += tmp;

    } else QUICK_CRYPT_CASE(evpmd_ctx)  // probably I seem crazy for all these operations... but it's perfect!
        tmpctx = calloc(1, sizeof(EVP_MD_CTX));
        if(!tmpctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        EVP_DigestUpdate(evpmd_ctx, data, datalen);
        EVP_MD_CTX_copy_ex(tmpctx, evpmd_ctx);
        EVP_DigestFinal(evpmd_ctx, digest, &tmp);
        FREEZ(evpmd_ctx);
        evpmd_ctx = tmpctx;
        add_var(0, "QUICKBMS_HASH", digest, 0, tmp);
        byte2hex(digest, tmp, digest_hex, sizeof(digest_hex));
        add_var(0, "QUICKBMS_HEXHASH", digest_hex, 0, -1);

    } else QUICK_CRYPT_CASE(blowfish_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, BF_decrypt((void *)data, blowfish_ctx))
        else              ENCRYPT_BLOCKS(8, BF_encrypt((void *)data, blowfish_ctx))

    } else QUICK_CRYPT_CASE(aes_ctr_ctx)
        AES_ctr128_encrypt(data, data, datalen, &aes_ctr_ctx->ctx, aes_ctr_ctx->ivec, aes_ctr_ctx->ecount, &aes_ctr_ctx->num);
#endif
#ifndef DISABLE_MCRYPT
    } else QUICK_CRYPT_CASE(mcrypt_ctx)
        if(!encrypt_mode) mdecrypt_generic(mcrypt_ctx, data, datalen);
        else              mcrypt_generic(mcrypt_ctx, data, datalen);
#endif
#ifndef DISABLE_TOMCRYPT
    } else QUICK_CRYPT_CASE(tomcrypt_ctx)
        if(tomcrypt_ctx->hash) {
            tomcrypt_doit(tomcrypt_ctx, NULL, data, datalen, digest, EVP_MAX_MD_SIZE, &tmp);
            if(tmp >= 0) {
                add_var(0, "QUICKBMS_HASH", digest, 0, tmp);
                byte2hex(digest, tmp, digest_hex, sizeof(digest_hex));
                add_var(0, "QUICKBMS_HEXHASH", digest_hex, 0, -1);
            }
        } else {
            tomcrypt_doit(tomcrypt_ctx, NULL, data, datalen, data, datalen, NULL);
        }
#endif

    } else QUICK_CRYPT_CASE(tea_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, tea_crypt(tea_ctx, TEA_DECRYPT, data, data))
        else              ENCRYPT_BLOCKS(8, tea_crypt(tea_ctx, TEA_ENCRYPT, data, data))

    } else QUICK_CRYPT_CASE(xtea_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, xtea_crypt_ecb(xtea_ctx, XTEA_DECRYPT, data, data))
        else              ENCRYPT_BLOCKS(8, xtea_crypt_ecb(xtea_ctx, XTEA_ENCRYPT, data, data))

    } else QUICK_CRYPT_CASE(xxtea_ctx)
        if(!encrypt_mode) xxtea_crypt(xxtea_ctx, XXTEA_DECRYPT, data, datalen);
        else              xxtea_crypt(xxtea_ctx, XXTEA_ENCRYPT, data, datalen);

    } else QUICK_CRYPT_CASE(swap_ctx)
        swap_crypt(swap_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(math_ctx)
        math_crypt(math_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(xmath_ctx)
        xmath_crypt(xmath_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(random_ctx)
        random_crypt(random_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(xor_ctx)
        xor_crypt(xor_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(rot_ctx)
        if(!encrypt_mode) rot_decrypt(rot_ctx, data, datalen);
        else              rot_encrypt(rot_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(rotate_ctx)
        rotate_crypt(rotate_ctx, data, datalen, encrypt_mode);

    } else QUICK_CRYPT_CASE(reverse_ctx)
        reverse_crypt(reverse_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(inc_ctx)
        if(!encrypt_mode) inc_crypt(inc_ctx, data, datalen, 0);
        else              inc_crypt(inc_ctx, data, datalen, 1);

    } else QUICK_CRYPT_CASE(charset_ctx)
        if(!encrypt_mode) charset_decrypt(charset_ctx, data, datalen);
        else              charset_encrypt(charset_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(charset2_ctx)
        if(!encrypt_mode) charset_encrypt(charset2_ctx, data, datalen); // yes, it's encrypted first
        else              charset_decrypt(charset2_ctx, data, datalen); // and decrypted

    } else QUICK_CRYPT_CASE(twofish_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, do_twofish_decrypt(twofish_ctx, data, data))
        else              ENCRYPT_BLOCKS(16, do_twofish_encrypt(twofish_ctx, data, data))

    } else QUICK_CRYPT_CASE(seed_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, do_seed_decrypt(seed_ctx, data, data))
        else              ENCRYPT_BLOCKS(16, do_seed_encrypt(seed_ctx, data, data))

    } else QUICK_CRYPT_CASE(serpent_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, serpent_decrypt_internal(serpent_ctx, (void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(16, serpent_encrypt_internal(serpent_ctx, (void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(ice_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, ice_key_decrypt(ice_ctx, (void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(8, ice_key_encrypt(ice_ctx, (void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(rotor_ctx)
        if(!encrypt_mode) RTR_d_region(rotor_ctx, data, datalen, TRUE);
        else              RTR_e_region(rotor_ctx, data, datalen, TRUE);

    } else QUICK_CRYPT_CASE(ssc_ctx)
        if(!encrypt_mode) ssc_decrypt(ssc_ctx->key, ssc_ctx->keysz, data, datalen);
        else              ssc_encrypt(ssc_ctx->key, ssc_ctx->keysz, data, datalen);

    } else QUICK_CRYPT_CASE(cunprot_ctx)
        if(!encrypt_mode) datalen = cunprot_decrypt(cunprot_ctx, data, datalen);
        else              datalen = cunprot_encrypt(cunprot_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(zipcrypto_ctx) // the 12 bytes header must be removed by the user
        if(!encrypt_mode) zipcrypto_decrypt(zipcrypto_ctx, (void *)get_crc_table(), data, datalen);
        else              zipcrypto_encrypt(zipcrypto_ctx, (void *)get_crc_table(), data, datalen);
        if(zipcrypto_ctx[3]) {  // yeah this is valid only for the decryption
            if(datalen < 12) {
                datalen = 0;
            } else {
                datalen -= 12;
                mymemmove(data, data + 12, datalen);
            }
        }

    } else QUICK_CRYPT_CASE(threeway_ctx)
        if(!encrypt_mode) threeway_decrypt(threeway_ctx, data, datalen);
        else              threeway_encrypt(threeway_ctx, data, datalen);

    } else QUICK_CRYPT_CASE(skipjack_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, skipjack_decrypt(skipjack_ctx, data, data))
        else              ENCRYPT_BLOCKS(8, skipjack_encrypt(skipjack_ctx, data, data))

    } else QUICK_CRYPT_CASE(anubis_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, ANUBISdecrypt(anubis_ctx, data, data))
        else              ENCRYPT_BLOCKS(16, ANUBISencrypt(anubis_ctx, data, data))

    } else QUICK_CRYPT_CASE(aria_ctx)
        ENCRYPT_BLOCKS(16, ARIA_Crypt(data, aria_ctx->Nr, aria_ctx->rk, data))

    } else QUICK_CRYPT_CASE(crypton_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, crypton_decrypt((void *)data, (void *)data, crypton_ctx))
        else              ENCRYPT_BLOCKS(16, crypton_encrypt((void *)data, (void *)data, crypton_ctx))

    } else QUICK_CRYPT_CASE(frog_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, frog_decrypt((void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(16, frog_encrypt((void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(gost_ctx)
        if(!gost_ctx->type) {
            if(!encrypt_mode) ENCRYPT_BLOCKS(8, gostdecrypt((void *)data, (void *)data, gost_ctx->key))
            else              ENCRYPT_BLOCKS(8, gostcrypt((void *)data, (void *)data, gost_ctx->key))
        } else if(gost_ctx->type == 1) {
            gostofb((void *)data, (void *)data, datalen, gost_ctx->iv, gost_ctx->key);
        } else if(gost_ctx->type == 2) {
            if(!encrypt_mode) ENCRYPT_BLOCKS(8, gostcfbdecrypt((void *)data, (void *)data, datalen, gost_ctx->iv, gost_ctx->key))
            else              ENCRYPT_BLOCKS(8, gostcfbencrypt((void *)data, (void *)data, datalen, gost_ctx->iv, gost_ctx->key))
        }

    } else QUICK_CRYPT_CASE(lucifer_ctx)
        ENCRYPT_BLOCKS(16, lucifer(data))

    } else if(kirk_ctx >= 0) {  // set_int3 here?
        if(datalen < 0) return(0);
        switch(kirk_ctx)
        {
            case 0:  kirk_CMD0(data, data, datalen, 0); break;
            case 1:  kirk_CMD1(data, data, datalen, 0); break;
            case 4:  kirk_CMD4(data, data, datalen);    break;
            case 7:  kirk_CMD7(data, data, datalen);    break;
            case 10: kirk_CMD10(data, datalen);         break;
            case 11: kirk_CMD11(data, data, datalen);   break;
            case 14: kirk_CMD14(data, datalen);         break;
            default: break;
        }

    } else QUICK_CRYPT_CASE(mars_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, mars_decrypt((void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(16, mars_encrypt((void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(misty1_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, misty1_decrypt_block(misty1_ctx, (void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(8, misty1_encrypt_block(misty1_ctx, (void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(noekeon_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(16, NOEKEONdecrypt(noekeon_ctx, (void *)data, (void *)data))
        else              ENCRYPT_BLOCKS(16, NOEKEONencrypt(noekeon_ctx, (void *)data, (void *)data))

    } else QUICK_CRYPT_CASE(seal_ctx)
        seal_encrypt(seal_ctx, (void *)data, datalen);

    } else QUICK_CRYPT_CASE(safer_ctx)
        if(!encrypt_mode) ENCRYPT_BLOCKS(8, Safer_Decrypt_Block((void *)data, (void *)safer_ctx, (void *)data))
        else              ENCRYPT_BLOCKS(8, Safer_Encrypt_Block((void *)data, (void *)safer_ctx, (void *)data))

    } else QUICK_CRYPT_CASE(pc1_128_ctx)
        pc1_128(pc1_128_ctx, data, datalen, encrypt_mode);

    } else QUICK_CRYPT_CASE(pc1_256_ctx)
        pc1_256(pc1_256_ctx, data, datalen, encrypt_mode);

    } else QUICK_CRYPT_CASE(crc_ctx)
        crc = crc_calc(crc_ctx, data, datalen);
        add_var(0, "QUICKBMS_CRC", NULL, crc, sizeof(u_int));

    } else QUICK_CRYPT_CASE(execute_ctx)
        quickbms_execute_pipe(execute_ctx, data, datalen, NULL, 0, NULL);

    } else QUICK_CRYPT_CASE(calldll_ctx)
        quickbms_calldll_pipe(calldll_ctx, data, datalen, NULL, 0);

    } else {
        if(datalen < 0) return(-1);
    }
    //return(0);  // don't return datalen because they are almost all block cipher encryptions and so it's all padded/aligned
    return(datalen);    // from version 0.3.11 I return datalen, only if I'm 100% sure that it's correct
}



int perform_compression(u8 *in, int zsize, u8 **ret_out, int size, int *outsize) {
    int     tmp1,
            tmp2,
            tmp3,
            tmp4,
            old_outsize;
    i32     t32 = 0;
    u8      *out,
            *p,
            *l;

    old_outsize = *outsize;

    out = *ret_out;
    switch(compression_type) {
        QUICK_COMP_CASE(ZLIB) size = unzip_zlib(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(DEFLATE) size = unzip_deflate(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(LZO1)
        QUICK_COMP_CASE(LZO1A)
        QUICK_COMP_CASE(LZO1B)
        QUICK_COMP_CASE(LZO1C)
        QUICK_COMP_CASE(LZO1F)
        QUICK_COMP_CASE(LZO1X)
        QUICK_COMP_CASE(LZO1Y)
        QUICK_COMP_CASE(LZO1Z)
        QUICK_COMP_CASE(LZO2A) size = unlzo(in, zsize, out, size, compression_type); break;
        QUICK_COMP_CASE(LZSS) size = unlzss(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZX) size = unlzx(in, zsize, out, size); break;
        QUICK_COMP_CASE(GZIP)
            t32 = *outsize;
            size = ungzip(in, zsize, &out, &t32); // outsize and NOT size because must be reallocated
            *outsize = t32;
            break;
        QUICK_COMP_CASE(EXPLODE) size = unexplode(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZMA)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_NONE, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA_86HEAD)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_86_HEADER, &t32, 0); // contains the uncompressed size
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA_86DEC)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_86_DECODER, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA_86DECHEAD)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_86_DECODER | LZMA_FLAGS_86_HEADER, &t32, 0); // contains the uncompressed size
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA_EFS)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_EFS, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(BZIP2) size = unbzip2(in, zsize, out, size); break;
        QUICK_COMP_CASE(XMEMLZX) size = unxmemlzx(in, zsize, out, size); break;
        QUICK_COMP_CASE(HEX) size = unhex(in, zsize, out, size); break;
        QUICK_COMP_CASE(BASE64) size = unbase64(in, zsize, out, size); break;
        QUICK_COMP_CASE(UUENCODE) size = uudecode(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(XXENCODE) size = uudecode(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(ASCII85) size = unascii85(in, zsize, out, size); break;
        QUICK_COMP_CASE(YENC) size = unyenc(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNLZW) size = unlzw(out, size, in, zsize); break;
        QUICK_COMP_CASE(UNLZWX) size = unlzwx(out, size, in, zsize); break;
        //QUICK_COMP_CASE(CAB) size = unmspack_cab(in, zsize, out, size); break;
        //QUICK_COMP_CASE(CHM) size = unmspack_chm(in, zsize, out, size); break;
        //QUICK_COMP_CASE(SZDD) size = unmspack_szdd(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZXCAB) size = unmspack(in, zsize, out, size, 21, 0, 0); break;
        QUICK_COMP_CASE(LZXCHM) size = unmspack(in, zsize, out, size, 16, 2, 0); break;
        QUICK_COMP_CASE(RLEW) size = unrlew(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZJB) size = lzjb_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(SFL_BLOCK) size = expand_block(in, out, zsize, size); break;
        QUICK_COMP_CASE(SFL_RLE) size = expand_rle(in, out, zsize, size); break;
        QUICK_COMP_CASE(SFL_NULLS) size = expand_nulls(in, out, zsize, size); break;
        QUICK_COMP_CASE(SFL_BITS) size = expand_bits(in, out, zsize, size); break;
        QUICK_COMP_CASE(LZMA2)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_NONE, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA2_86HEAD)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_86_HEADER, &t32, 0); // contains the uncompressed size
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA2_86DEC)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_86_DECODER, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA2_86DECHEAD)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_86_DECODER | LZMA_FLAGS_86_HEADER, &t32, 0); // contains the uncompressed size
            *outsize = t32;
            break;
        QUICK_COMP_CASE(NRV2b)
        QUICK_COMP_CASE(NRV2d)
        QUICK_COMP_CASE(NRV2e) size = unucl(in, zsize, out, size, compression_type); break;
        QUICK_COMP_CASE(HUFFBOH) size = huffboh_unpack_mem2mem(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNCOMPRESS) size = uncompress_lzw(in, zsize, out, size, -1); break;
        QUICK_COMP_CASE(DMC) size = undmc(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZH) size = unlzh(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZARI) size = unlzari(in, zsize, out, size); break;
        QUICK_COMP_CASE(TONY) size = Screen__decompressTony(in, zsize, out, size); break;
        QUICK_COMP_CASE(RLE7) size = Screen__decompressRLE7(in, zsize, out, size); break;
        QUICK_COMP_CASE(RLE0) size = Screen__decompressRLE0(in, zsize, out, size); break;
        QUICK_COMP_CASE(RLE)
            //size = rle_decode(out, in, zsize);
            size = unrle(out, in, zsize);
            break;
        QUICK_COMP_CASE(RLEA) size = another_rle(in, zsize, out, size); break;
        QUICK_COMP_CASE(BPE) size = bpe_expand(in, zsize, out, size); break;
        QUICK_COMP_CASE(QUICKLZ) size = unquicklz(in, zsize, out, size); break;
        QUICK_COMP_CASE(Q3HUFF) size = unq3huff(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNMENG) size = unmeng(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZ2K)
            unlz2k_init();
            size = unlz2k(in, out, zsize, size);
            break;
        QUICK_COMP_CASE(DARKSECTOR) size = undarksector(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(MSZH) size = mszh_decomp(in, zsize, out, size); break;
        QUICK_COMP_CASE(UN49G)
            un49g_init();
            size = un49g(out, in);
            break;
        QUICK_COMP_CASE(UNTHANDOR) size = unthandor(in, zsize, out, size); break;
        QUICK_COMP_CASE(DOOMHUFF) size = doomhuff(0, in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(ZDAEMON) size = doomhuff(1, in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(SKULLTAG) size = doomhuff(2, in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(APLIB) size = aP_depack_safe(in, zsize, out, size); break;
        QUICK_COMP_CASE(TZAR_LZSS)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the tzar_lzss decompression requires the setting of the dictionary in\n"
                    "       field comtype with the name of the variable containing the type of\n"
                    "       tzar decompression (from 0xa1 to 0xc5), like:\n"
                    "         comtype tzar_lzss MYVAR\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }

            tzar_lzss_init();
            t32 = size;
            tzar_lzss(in, zsize, out, &t32,    // it's so horrible because the last argument is dynamic
                //get_var32(get_var_from_name(comtype_dictionary, comtype_dictionary_len))
                myatoi(comtype_dictionary)
            );
            size = t32;
            break;
        QUICK_COMP_CASE(LZF) size = lzf_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(CLZ77) size = CLZ77_Decode(out, size, in, zsize); break;
        QUICK_COMP_CASE(LZRW1) size = lzrw1_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DHUFF) size = undhuff(in, zsize, out, size); break;
        QUICK_COMP_CASE(FIN) size = unfin(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZAH) size = de_lzah(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZH12) size = de_lzh(in, zsize, out, size, 12); break;
        QUICK_COMP_CASE(LZH13) size = de_lzh(in, zsize, out, size, 13); break;
#ifdef WIN32   // the library is not by default in linux and it's too big to attach in quickbms
        QUICK_COMP_CASE(GRZIP) size = GRZip_DecompressBlock(in, zsize, out); break;
#endif
        QUICK_COMP_CASE(CKRLE) size = CK_RLE_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(QUAD) size = unquad(in, zsize, out, size); break;
        QUICK_COMP_CASE(BALZ) size = unbalz(in, zsize, out, size); break;
        // it's a zlib with the adding of inflateBack9 which is not default
        QUICK_COMP_CASE(DEFLATE64) size = inflate64(in, zsize, out, size); break;
        QUICK_COMP_CASE(SHRINK) size = unshrink(in, zsize, out, size); break;
        QUICK_COMP_CASE(PPMDI) size = unppmdi(in, zsize, out, size); /* PKWARE specifics */ break;
        QUICK_COMP_CASE(MULTIBASE)
            size = multi_base_decoder(  // the usage of comtype_dictionary_len avoids wasting 2 vars
                comtype_dictionary_len & 0xff, (comtype_dictionary_len >> 8) & 0xff,
                in, zsize, out, size,
                comtype_dictionary);
            break;
        QUICK_COMP_CASE(BRIEFLZ) size = blz_depack_safe(in, zsize, out, size); break;
        QUICK_COMP_CASE(PAQ6)
            size = -1;
            /* wasting of memory and speed!!!

            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PAQ6 decompression requires the setting of the dictionary in\n"
                    "       field comtype with the name of the variable containing the level of\n"
                    "       compression (from 0 to 9), like:\n"
                    "         comtype paq6 MYVAR\n"
                    "         comtype paq6 3 # default level\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            size = unpaq6(in, zsize, out, size,
                //get_var32(get_var_from_name(comtype_dictionary, comtype_dictionary_len))
                myatoi(comtype_dictionary)
            );
            */
            break;
        QUICK_COMP_CASE(SHCODEC) size = sh_DecodeBlock(in, out, zsize); break;
        QUICK_COMP_CASE(HSTEST1) size = hstest_hs_unpack(out, in, zsize); break;
        QUICK_COMP_CASE(HSTEST2) size = hstest_unpackc(out, in, zsize); break;
        QUICK_COMP_CASE(SIXPACK) size = unsixpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(ASHFORD) size = unashford(in, zsize, out, size); break;
#ifdef WIN32    // the alternative is using the compiled code directly
        QUICK_COMP_CASE(JCALG) size = JCALG1_Decompress_Small(in, out); break;
#endif
        QUICK_COMP_CASE(JAM) size = unjam(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZHLIB) size = unlzhlib(in, zsize, out, size); break;
        QUICK_COMP_CASE(SRANK) size = unsrank(in, zsize, out, size); break;
        QUICK_COMP_CASE(ZZIP)
            if(size >= zsize) { // zzip is horrible to use in this way
                memcpy(out, in, zsize);
                size = ZzUncompressBlock(out);
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(SCPACK) size = strexpand(out, in, size, (unsigned char **)comtype_dictionary); break;
        QUICK_COMP_CASE(RLE3) size = rl3_decode(in, zsize, out, size); break;
        QUICK_COMP_CASE(BPE2) size = unbpe2(in, zsize, out, size); break;
        QUICK_COMP_CASE(BCL_HUF) Huffman_Uncompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(BCL_LZ) LZ_Uncompress(in, out, zsize); break;
        QUICK_COMP_CASE(BCL_RICE)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the BCL_RICE decompression requires the setting of the dictionary in\n"
                    "       field comtype with the name of the variable containing the type of\n"
                    "       compression (from 1 to 8, read rice.h), like:\n"
                    "         comtype bcl_rice 1\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            Rice_Uncompress(in, out, zsize, size,
                //get_var32(get_var_from_name(comtype_dictionary, comtype_dictionary_len))
                myatoi(comtype_dictionary)
            );
            break;
        QUICK_COMP_CASE(BCL_RLE) size = RLE_Uncompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(BCL_SF) SF_Uncompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCZ)
            t32 = size;
            if(Scz_Decompress_Buffer2Buffer(in, zsize, (void *)&p, &t32) && (t32 <= size)) {
                size = t32;
                memcpy(out, p, size);
                free(p);
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(SZIP)
            t32 = size;
            SZ_BufftoBuffDecompress(out, &t32, in, zsize, NULL);
            size = t32;
            break;
        QUICK_COMP_CASE(PPMDI_RAW)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PPMDi decompression requires the setting of the dictionary field\n"
                    "       in comtype specifying SaSize, MaxOrder and Method, like:\n"
                    "         comtype ppmdi_raw \"10 4 0\"\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            tmp1 = tmp2 = tmp3 = 0;
            //sscanf(comtype_dictionary, "%d %d %d", &tmp1, &tmp2, &tmp3);
            get_parameter_numbers(comtype_dictionary, &tmp1, &tmp2, &tmp3, NULL);
            size = unppmdi_raw(in, zsize, out, size, tmp1, tmp2, tmp3);
            break;
        QUICK_COMP_CASE(PPMDG) size = unppmdg(in, zsize, out, size); break;
        QUICK_COMP_CASE(PPMDG_RAW)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PPMdG decompression requires the setting of the dictionary field\n"
                    "       in comtype specifying SaSize and MaxOrder, like:\n"
                    "         comtype ppmdg_raw \"10 4\"\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            tmp1 = tmp2 = 0;
            //sscanf(comtype_dictionary, "%d %d", &tmp1, &tmp2);
            get_parameter_numbers(comtype_dictionary, &tmp1, &tmp2, NULL);
            size = unppmdg_raw(in, zsize, out, size, tmp1, tmp2);
            break;
        QUICK_COMP_CASE(PPMDJ) size = unppmdj(in, zsize, out, size); break;
        QUICK_COMP_CASE(PPMDJ_RAW)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PPMdJ decompression requires the setting of the dictionary field\n"
                    "       in comtype specifying SaSize, MaxOrder and CutOff, like:\n"
                    "         comtype ppmdj_raw \"10 4 0\"\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            tmp1 = tmp2 = tmp3 = 0;
            //sscanf(comtype_dictionary, "%d %d %d", &tmp1, &tmp2, &tmp3);
            get_parameter_numbers(comtype_dictionary, &tmp1, &tmp2, &tmp3, NULL);
            size = unppmdj_raw(in, zsize, out, size, tmp1, tmp2, tmp3);
            break;
        QUICK_COMP_CASE(SR3C) size = unsr3c(in, zsize, out, size); break;
        QUICK_COMP_CASE(HUFFMANLIB)
            t32 = size;
            if(!huffman_decode_memory(in, zsize, &p, &t32) && (t32 <= size)) {
                size = t32;
                memcpy(out, p, size);
                free(p);
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(SFASTPACKER) size = SFUnpack(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(SFASTPACKER2) size = SFUnpack(in, zsize, out, size, 1);   /* smart mode only */ break;
        QUICK_COMP_CASE(DK2) undk2_init(); size = undk2(out, in, 0); break;
        QUICK_COMP_CASE(LZ77WII)
            t32 = *outsize;
            size = unlz77wii(in, zsize, &out, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZ77WII_RAW10) size = unlz77wii_raw10(in, zsize, out, size); break;
        QUICK_COMP_CASE(DARKSTONE) size = undarkstone(in, zsize, out, size); break;
        QUICK_COMP_CASE(SFL_BLOCK_CHUNKED) size = sfl_block_chunked(in, zsize, out, size); break;
        QUICK_COMP_CASE(YUKE_BPE) size = yuke_bpe(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(STALKER_LZA)
            stalker_lza_init();
            t32 = size;
            stalker_lza(in, zsize, &p, &t32);  // size is filled by the function
            size = t32;
            if(/*(tmp1 >= 0) &&*/ (size > 0)) {
                myalloc(&out, size, outsize);
                memcpy(out, p, size);
                free(p);
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(PRS_8ING) size = prs_8ing_uncomp(out, size, in, zsize); break;
        QUICK_COMP_CASE(PUYO_CNX) size = puyo_cnx(in, zsize, out, size);  /* _unpack is the old one */ break;
        QUICK_COMP_CASE(PUYO_CXLZ) size = puyo_cxlz_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(PUYO_LZ00) size = puyo_lz00(in, zsize, out, size); /* _unpack is the old one */ break;
        QUICK_COMP_CASE(PUYO_LZ01) size = puyo_lz01(in, zsize, out, size); /* _unpack is the old one */ break;
        QUICK_COMP_CASE(PUYO_LZSS) size = puyo_lzss_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(PUYO_ONZ) size = puyo_onz_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(PUYO_PRS) size = puyo_prs(in, zsize, out, size);  /* _unpack is the old one */ break;
        //QUICK_COMP_CASE(PUYO_PVZ) size = puyo_pvz_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(FALCOM) size = falcom_DecodeData(out, size, in, zsize); break;
        QUICK_COMP_CASE(CPK) size = CPK_uncompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(BZIP2_FILE)
            t32 = *outsize;
            size = unbzip2_file(in, zsize, &out, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZ77WII_RAW11) size = unlz77wii_raw11(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZ77WII_RAW30) size = unlz77wii_raw30(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZ77WII_RAW20) size = unlz77wii_raw20(in, zsize, out, size); break;
        QUICK_COMP_CASE(PGLZ) size = pglz_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(SLZ) size = UnPackSLZ(in, zsize, out, size); break;
        QUICK_COMP_CASE(SLZ_01)
            t32 = *outsize;
            size = slz_triace(in, zsize, &out, size, 1, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(SLZ_02)
            t32 = *outsize;
            size = slz_triace(in, zsize, &out, size, 2, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(SLZ_03)
            t32 = *outsize;
            size = slz_triace(in, zsize, &out, size, 3, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZHL) size = unlzhl(in, zsize, out, size); break;
        QUICK_COMP_CASE(D3101) size = d3101(in, zsize, out, size); break;
        QUICK_COMP_CASE(SQUEEZE) size = unsqueeze(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZRW3) size = unlzrw3(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_ahuff) size = ahuff_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_arith) size = arith_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_arith1) size = arith1_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_arith1e) size = arith1e_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_arithn) size = arithn_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_compand) size = compand_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_huff) size = huff_ExpandMemory(in, zsize, out, size); break;
        //QUICK_COMP_CASE(TDCB_lzss) size = lzss_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_lzss)
            tmp1 = 12;  // INDEX_BIT_COUNT
            tmp2 = 4;   // LENGTH_BIT_COUNT
            tmp3 = 9;   // DUMMY9
            tmp4 = 0;   // END_OF_STREAM
            if(comtype_dictionary && (comtype_dictionary_len > 0)) {
                get_parameter_numbers(comtype_dictionary,
                    &tmp1, &tmp2, &tmp3, &tmp4, NULL);
            }
            tdcb_lzss_init(tmp1, tmp2, tmp3, tmp4);
            size = lzss_ExpandMemory(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(TDCB_lzw12) size = lzw12_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_lzw15v) size = lzw15v_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(TDCB_silence) size = silence_ExpandMemory(in, zsize, out, size); break;
        QUICK_COMP_CASE(RDC) size = rdc_decompress(in, zsize, out); break;
        QUICK_COMP_CASE(ILZR) size = ilzr_expand(in, zsize, out, size); break;
        QUICK_COMP_CASE(DMC2) size = dmc2_uncompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(diffcomp) size = diffcomp(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZR) size = LZRDecompress(out, size, in, in + zsize); break;
        QUICK_COMP_CASE(LZS) size = unlzs(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(LZS_BIG) size = unlzs(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(COPY) size = uncopy(in, zsize, out, size); break;
        QUICK_COMP_CASE(MOHLZSS) size = moh_lzss(in, zsize, out, size); break;
        QUICK_COMP_CASE(MOHRLE) size = moh_rle(in, zsize, out, size); break;
        QUICK_COMP_CASE(YAZ0) size = decodeYaz0(in, zsize, out, size); break;
        QUICK_COMP_CASE(BYTE2HEX) size = byte2hex(in, zsize, out, size); break;
        QUICK_COMP_CASE(UN434A) un434a_init(); size = un434a(in, out); break;
        QUICK_COMP_CASE(UNZIP_DYNAMIC)
            t32 = *outsize;
            size = unzip_dynamic(in, zsize, &out, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(GZPACK) size = gz_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(ZLIB_NOERROR) size = unzip_zlib(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(DEFLATE_NOERROR) size = unzip_deflate(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(PPMDH) size = unppmdh(in, zsize, out, size); break;
        QUICK_COMP_CASE(PPMDH_RAW)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PPMdH decompression requires the setting of the dictionary field\n"
                    "       in comtype specifying SaSize and MaxOrder, like:\n"
                    "         comtype ppmdh_raw \"10 4\"\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            tmp1 = tmp2 = 0;
            //sscanf(comtype_dictionary, "%d %d", &tmp1, &tmp2);
            get_parameter_numbers(comtype_dictionary, &tmp1, &tmp2, NULL);
            size = unppmdh_raw(in, zsize, out, size, tmp1, tmp2);
            break;
        QUICK_COMP_CASE(RNC)
            size = _rnc_unpack(in, zsize, out, 0);
            //size = rnc_unpack(in, out, NULL, -1, -1);
            //size = RncDecoder__unpackM1(in, zsize, out, size);  // auto guess
            break;
        QUICK_COMP_CASE(RNC_RAW)
            //size = _rnc_unpack(in, zsize, out, size);
            //size = rnc_unpack(in, out, NULL, zsize, size);
            //size = RncDecoder__unpackM1(in, zsize, out, size);  // auto guess
            break;
        QUICK_COMP_CASE(FITD)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the PAK_explode decompression requires the setting of the dictionary\n"
                    "       field in comtype specifying info5, like:\n"
                    "         get info5 byte\n"
                    "         comtype ppmdh_raw info5\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            tmp1 = 0;
            get_parameter_numbers(comtype_dictionary, &tmp1, NULL);
            PAK_explode(in, out, zsize, size, tmp1);    // no return value
            break;
        QUICK_COMP_CASE(KENS_Nemesis) size = KENS_Nemesis(in, zsize, out, size); break;
        QUICK_COMP_CASE(KENS_Kosinski) size = KENS_Kosinski(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(KENS_Kosinski_moduled) size = KENS_Kosinski(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(KENS_Enigma) size = KENS_Enigma(in, zsize, out, size); break;
        QUICK_COMP_CASE(KENS_Saxman) size = KENS_Saxman(in, zsize, out, size); break;
        QUICK_COMP_CASE(DRAGONBALLZ) size = undragonballz(in, zsize, out); break;
        QUICK_COMP_CASE(NITROSDK) size = nitroDecompress(in, zsize, out, 1); break;
        QUICK_COMP_CASE(MSF) size = unmsf(in, zsize, out, size); break;
        QUICK_COMP_CASE(STARGUNNER) size = unstargun(in, size, out); /* yeah it's size and not zsize */ break;
        QUICK_COMP_CASE(NTCOMPRESS)
            ntcompress_init();
            switch(ntcompress_type(in[0])) {
                case 0x00:  size = unlz77wii_raw00(in, zsize, out, size);   break;
                case 0x10:  size = unlz77wii_raw10(in, zsize, out, size);   break;
                case 0x11:  size = unlz77wii_raw11(in, zsize, out, size);   break;
                case 0x20:  size = unlz77wii_raw20(in, zsize, out, size);   break;
                case 0x30:  size = ntcompress_30(in, zsize, out);           break;
                case 0x40:  size = ntcompress_40(in, zsize, out);           break;
                default:    size = -1;
                // modify nintendo.h too
            }
            ntcompress_free(NULL);
            break;
        QUICK_COMP_CASE(CRLE) size = CRLE_Decode(out, size, in, zsize); break;
#ifdef WIN32    // too boring to make the .a for linux too
        QUICK_COMP_CASE(CTW) size = unctw(in, zsize, out, size); break;
#endif
        QUICK_COMP_CASE(DACT_DELTA) size = dact_delta_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_MZLIB2) size = dact_mzlib2_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_MZLIB) size = dact_mzlib_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_RLE) size = dact_rle_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_SNIBBLE) size = dact_snibble_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_TEXT) size = dact_text_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(DACT_TEXTRLE) size = dact_textrle_decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(EXECUTE)
            size = quickbms_execute_pipe(comtype_dictionary, in, zsize, &out, size, NULL);
            if(size >= 0) *outsize = size;
            break;
        QUICK_COMP_CASE(CALLDLL) size = quickbms_calldll_pipe(comtype_dictionary, in, zsize, out, size); break;
        QUICK_COMP_CASE(LZ77_0) size = PDRLELZ_DecompLZ2(in, out, zsize); break;
        QUICK_COMP_CASE(LZBSS) size = LZBSS_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(BPAQ0) size = BPAQ0_DecodeData(in, out); break;
        QUICK_COMP_CASE(LZPX) size = lzpx_unpack(in, out); break;
        QUICK_COMP_CASE(MAR_RLE) size = MAR_RLE(in, zsize, out, size); break;
        QUICK_COMP_CASE(GDCM_RLE) size = gdcm_rle(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZMAT)
            t32 = size;
            if(lzmat_decode(out, &t32, in, zsize)) size = -1;
            size = t32;
            break;
        QUICK_COMP_CASE(DICT)
            t32 = size;
            if(DictDecode(in, zsize, out, &t32) < 0) {
                size = -1;
            } else {
                size = t32;
            }
            break;
        QUICK_COMP_CASE(REP) size = unrep(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZP) size = LZPDecode(in, zsize, out, 32); break;
        QUICK_COMP_CASE(ELIAS_DELTA) size = eliasDeltaDecode(in, zsize, out); break;
        QUICK_COMP_CASE(ELIAS_GAMMA) size = eliasGammaDecode(in, zsize, out); break;
        QUICK_COMP_CASE(ELIAS_OMEGA) size = eliasOmegaDecode(in, zsize, out); break;
        QUICK_COMP_CASE(PACKBITS) size = unpackbits(in, zsize, out); break;
        QUICK_COMP_CASE(DARKSECTOR_NOCHUNKS) size = undarksector(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(ENET)
            ENetRangeCoder  enet_ctx;
            memset(&enet_ctx, 0, sizeof(enet_ctx));
            size = enet_range_coder_decompress (&enet_ctx, in, zsize, out, size);
            break;
        QUICK_COMP_CASE(EDUKE32) size = eduke32_lzwuncompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(XU4_RLE) size = xu4_rleDecompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(RVL) size = RVLCompress_decompress_ints(in, (void *)out, zsize); break;
        QUICK_COMP_CASE(LZFU)
            t32 = size; // unused
            p = pst_lzfu_decompress(in, zsize, &t32, 1);
            size = t32;
            if(p) {
                myalloc(&out, size, outsize);
                memcpy(out, p, size);
                free(p);
            }
            break;
        QUICK_COMP_CASE(LZFU_RAW)
            t32 = size; // unused
            p = pst_lzfu_decompress(in, zsize, &t32, 0);
            size = t32;
            if(p) {
                myalloc(&out, size, outsize);
                memcpy(out, p, size);
                free(p);
            }
            break;
        QUICK_COMP_CASE(XU4_LZW) size = lzwDecompress(in, out, zsize); break;
        QUICK_COMP_CASE(HE3) size = decode_he3_data(in, out, size, 0); /* don't check the signature */ break;
        QUICK_COMP_CASE(IRIS) size = iris_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(IRIS_HUFFMAN) size = iris_huffman(in, zsize, out, size); break;
        QUICK_COMP_CASE(IRIS_UO_HUFFMAN) size = iris_uo_huffman(in, zsize, out, size); break;
        QUICK_COMP_CASE(NTFS) size = ntfs_decompress(out, size, in, zsize); break;
        QUICK_COMP_CASE(PDB) size = pdb_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_SPREAD) size = decode_spread(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_RLE1) size = decode_rle1(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_RLE2) size = decode_rle2(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_RLE3) size = decode_rle3(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_RLE4) size = rle4decoding(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_ARITH) size = decode_arith(in, zsize, out, size); break;
        QUICK_COMP_CASE(COMPRLIB_SPLAY) size = decode_splay(in, zsize, out, size); break;
        QUICK_COMP_CASE(CABEXTRACT) size = LZXdecompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(MRCI) MRCIDecompressWrapper(in, zsize, out, size); /* no size returned */ break;
        QUICK_COMP_CASE(HD2_01) hd2_init(); size = hd2_01(in, out, zsize); break;
        QUICK_COMP_CASE(HD2_08)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the hd2_08 decompression requires the setting of the dictionary\n"
                    "       field in comtype specifying the 0x2c bytes of the first chunk\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            hd2_init();
            size = hd2_08(in, out, zsize, comtype_dictionary);
            break;
        QUICK_COMP_CASE(HD2_01raw) hd2_init(); size = hd2_01raw(in, out, zsize); break;
        QUICK_COMP_CASE(RTL_LZNT1) size = rtldecompress(RTL_COMPRESSION_FORMAT_LZNT1, in, zsize, out ,size); break;
        QUICK_COMP_CASE(RTL_XPRESS) size = rtldecompress(RTL_COMPRESSION_FORMAT_XPRESS, in, zsize, out ,size); break;
        QUICK_COMP_CASE(RTL_XPRESS_HUFF) size = rtldecompress(RTL_COMPRESSION_FORMAT_XPRESS_HUFF, in, zsize, out ,size); break;
        QUICK_COMP_CASE(PRS) size = prs_decompress(in, out); break;
        QUICK_COMP_CASE(SEGA_LZ77) size = sega_lz77(in, zsize, out); break;
        QUICK_COMP_CASE(SAINT_SEYA) size = Model_GMI_Decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(NTCOMPRESS30) size = ntcompress_30(in, zsize, out); break;
        QUICK_COMP_CASE(NTCOMPRESS40) size = ntcompress_40(in, zsize, out); break;
        QUICK_COMP_CASE(YAKUZA) size = unyakuza(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(LZ4) size = LZ4_decompress_safe(in, out, zsize, size); break;
        QUICK_COMP_CASE(SNAPPY)
            //size = unsnappy(in, zsize, out, size);
            size_t  snappy_tmp = size;
            if(snappy_uncompress(in, zsize, out, &snappy_tmp) == SNAPPY_OK) {
                size = snappy_tmp;
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(LUNAR_LZ1)  size = lunar_uncompress(in, zsize, out, size, LC_LZ1,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ2)  size = lunar_uncompress(in, zsize, out, size, LC_LZ2,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ3)  size = lunar_uncompress(in, zsize, out, size, LC_LZ3,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ4)  size = lunar_uncompress(in, zsize, out, size, LC_LZ4,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ5)  size = lunar_uncompress(in, zsize, out, size, LC_LZ5,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ6)  size = lunar_uncompress(in, zsize, out, size, LC_LZ6,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ7)  size = lunar_uncompress(in, zsize, out, size, LC_LZ7,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ8)  size = lunar_uncompress(in, zsize, out, size, LC_LZ8,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ9)  size = lunar_uncompress(in, zsize, out, size, LC_LZ9,  myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ10) size = lunar_uncompress(in, zsize, out, size, LC_LZ10, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ11) size = lunar_uncompress(in, zsize, out, size, LC_LZ11, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ12) size = lunar_uncompress(in, zsize, out, size, LC_LZ12, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ13) size = lunar_uncompress(in, zsize, out, size, LC_LZ13, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ14) size = lunar_uncompress(in, zsize, out, size, LC_LZ14, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ15) size = lunar_uncompress(in, zsize, out, size, LC_LZ15, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ16) size = lunar_uncompress(in, zsize, out, size, LC_LZ16, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ17) size = lunar_uncompress(in, zsize, out, size, LC_LZ17, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_LZ18) size = lunar_uncompress(in, zsize, out, size, LC_LZ18, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_RLE1) size = lunar_uncompress(in, zsize, out, size, LC_RLE1, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_RLE2) size = lunar_uncompress(in, zsize, out, size, LC_RLE2, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_RLE3) size = lunar_uncompress(in, zsize, out, size, LC_RLE3, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(LUNAR_RLE4) size = lunar_uncompress(in, zsize, out, size, LC_RLE4, myatoi(comtype_dictionary)); break;
        QUICK_COMP_CASE(GOLDENSUN) size = goldensun(in, zsize, out, size); break;
        QUICK_COMP_CASE(LUMINOUSARC) size = luminousarc(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZV1) size = rLZV1(in, out, zsize, size); break;
        QUICK_COMP_CASE(FASTLZAH) size = fastlz_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(ZAX) size = zax_uncompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(SHRINKER) size = shrinker_decompress(in, out, size);  /* yes, it's size */ break;
        QUICK_COMP_CASE(MMINI_HUFFMAN)
            p = malloc(MMINI_HUFFHEAP_SIZE);
            if(!p) STD_ERR(QUICKBMS_ERROR_MEMORY);
            size = mmini_huffman_decompress(in, zsize, out, size, p);
            free(p);
            break;
        QUICK_COMP_CASE(MMINI_LZ1) size = mmini_lzl_decompress(in, zsize, out, size); break;
        QUICK_COMP_CASE(MMINI)
            l = malloc(size);
            p = malloc(MMINI_HUFFHEAP_SIZE);
            if(!l || !p) STD_ERR(QUICKBMS_ERROR_MEMORY);
            size = mmini_decompress(in, zsize, l, size, out, size, p);
            free(p);
            free(l);
            break;
        QUICK_COMP_CASE(CLZW)
            lzw_dec_t  *clzw_ctx;
            clzw_ctx = calloc(1, sizeof(lzw_dec_t));
            if(!clzw_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
            clzw_outsz = 0;
            lzw_dec_init(clzw_ctx, out);
            lzw_decode(clzw_ctx, in, zsize);
            size = clzw_outsz;
            free(clzw_ctx);
            break;
        QUICK_COMP_CASE(LZHAM) size = unlzham(in, zsize, out, size); break;
        QUICK_COMP_CASE(LPAQ8)

            size = -1;
            /* wasting of memory and speed!!!

            tmp1 = 0;
            tmp2 = 0;
            get_parameter_numbers(comtype_dictionary, &tmp1, &tmp2, NULL);
            size = unlpaq8(in, zsize, out, size, tmp1, tmp2);
            */
            break;
        QUICK_COMP_CASE(SEGA_LZS2)
            t32 = *outsize;
            size = sega_lzs2(in, zsize, &out, &t32);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(WOLF) size = dewolf(in, zsize, out, size); break;
        QUICK_COMP_CASE(COREONLINE) size = CO_Decompress(out, size, in); break;
        QUICK_COMP_CASE(MSZIP) size = unmspack(in, zsize, out, size, -1, -1, 1); break;
        QUICK_COMP_CASE(QTM)
            tmp1 = 0;
            get_parameter_numbers(comtype_dictionary, &tmp1, NULL);
            size = unmspack(in, zsize, out, size, tmp1 ? tmp1 : 21, -1, 2);
            break;
        QUICK_COMP_CASE(MSLZSS) size = unmspack(in, zsize, out, size, -1,  0, 3); break;
        QUICK_COMP_CASE(MSLZSS1) size = unmspack(in, zsize, out, size, -1,  1, 3); break;
        QUICK_COMP_CASE(MSLZSS2) size = unmspack(in, zsize, out, size, -1,  2, 3); break;
        QUICK_COMP_CASE(KWAJ) size = unmspack(in, zsize, out, size, -1, -1, 4); break;
        QUICK_COMP_CASE(LZLIB) size = unlzlib(in, zsize, out, size); break;
        QUICK_COMP_CASE(DFLT) size = undflt(in, zsize, out, size); break;
        QUICK_COMP_CASE(LZMA_DYNAMIC)
            t32 = *outsize;
            size = unlzma(in, zsize, &out, size, LZMA_FLAGS_NONE, &t32, 1);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA2_DYNAMIC)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_NONE, &t32, 1);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZMA2_EFS)
            t32 = *outsize;
            size = unlzma2(in, zsize, &out, size, LZMA_FLAGS_EFS, &t32, 0);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(LZXCAB_DELTA) size = unmspack(in, zsize, out, size, 21, 0, 5); break;
        QUICK_COMP_CASE(LZXCHM_DELTA) size = unmspack(in, zsize, out, size, 16, 2, 5); break;
        QUICK_COMP_CASE(FFCE) ffce_init(); size = ffce(in, out, 0); break;
        //QUICK_COMP_CASE(SCUMMVM1)  size = Screen__decompressTony(in, zsize, out, size); break;
        //QUICK_COMP_CASE(SCUMMVM2)  size = Screen__decompressRLE7(in, zsize, out, size); break;
        //QUICK_COMP_CASE(SCUMMVM3)  size = Screen__decompressRLE0(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM4)  size = Screen__decompressHIF(in, out); break;
        QUICK_COMP_CASE(SCUMMVM5)  size = decompressSPCN(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM6)  size = RncDecoder__unpackM1(in, zsize, out, size); break;   // RNC1
        QUICK_COMP_CASE(SCUMMVM7)  size = RncDecoder__unpackM2(in, zsize, out, size); break;   // RNC2
        QUICK_COMP_CASE(SCUMMVM8)  size = CineUnpacker__unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM9)  size = delphineUnpack(out, size, in, zsize); break;
        QUICK_COMP_CASE(SCUMMVM10) size = DataIO__unpackChunk(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM11) size = DecompressorHuffman__unpack(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCUMMVM12) size = DecompressorLZW__unpackLZW(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCUMMVM13) size = DecompressorLZW__unpackLZW1(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCUMMVM14) size = DecompressorLZW__reorderPic(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM15) size = DecompressorLZW__reorderView(in, out); break;
        QUICK_COMP_CASE(SCUMMVM16) size = DecompressorLZS__unpack(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCUMMVM17) size = decodeSRLE(out, in, size); break;
        QUICK_COMP_CASE(SCUMMVM18) size = Screen__unpackRle(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM19) size = PS2Icon__decompressData(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM20) size = LZWDecoder__lzwExpand(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM21) size = uncompressPlane(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM22) size = unbarchive(in, zsize, out); break;
        QUICK_COMP_CASE(SCUMMVM23) size = Screen__decompressRLE256(out, in, size); break;
        QUICK_COMP_CASE(SCUMMVM24) size = Screen__decompressHIF_other(in, out /*, uint32 *skipData*/); break;
        QUICK_COMP_CASE(SCUMMVM25) size = FileExpander__process(out, in, size, zsize); break;
        QUICK_COMP_CASE(SCUMMVM26) size = SoundTownsPC98_v2__voicePlay(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM27) size = VQAMovie__decodeSND1(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM28) size = Background__decodeComponent(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM29) size = AnimFrame__decomp34(in, out, size, 0x7, 3); break;
        QUICK_COMP_CASE(SCUMMVM30) size = AnimFrame__decomp34(in, out, size, 0xf, 4); break;
        QUICK_COMP_CASE(SCUMMVM31) size = AnimFrame__decomp5(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM32) size = AnimFrame__decomp7(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM33) size = AnimFrame__decompFF(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM34) size = SavegameStream__readCompressed(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM35) size = dimuse_compDecode(in, out); break;
        QUICK_COMP_CASE(SCUMMVM36) size = decompressADPCM(in, out, 1); break;
        QUICK_COMP_CASE(SCUMMVM37) size = decompressADPCM(in, out, 2); break;
        QUICK_COMP_CASE(SCUMMVM38) size = MohawkBitmap__unpackRiven(in, zsize, out); break;
        QUICK_COMP_CASE(SCUMMVM39) size = MohawkBitmap__drawRLE8(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM40) size = LzhDecompressor__decompress(in, out, zsize, size); break;
        QUICK_COMP_CASE(SCUMMVM41) size = AnimationDecoder__decode_data(in, zsize, out); break;
        QUICK_COMP_CASE(SCUMMVM42) size = MusicPlayerMac_t7g__decompressMidi(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM43) size = StuffItArchive__decompress14(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM44) size = decompressIconPlanar(out, size, in); break;
        QUICK_COMP_CASE(SCUMMVM45) size = DrasculaEngine__decodeRLE(in, out /*, uint16 pitch*/); break;
        QUICK_COMP_CASE(SCUMMVM46) size = bompDecodeLine(out, in, size); break;
        QUICK_COMP_CASE(SCUMMVM47) size = bompDecodeLineReverse(out, in, size); break;
        QUICK_COMP_CASE(SCUMMVM48) size = ToucheEngine__res_decodeScanLineImageRLE(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM49) size = AnimationPlayer__rleDecode(in, out, size); break;
        QUICK_COMP_CASE(SCUMMVM50) size = Graphics__decodeRLE(out, size, in); break;
        QUICK_COMP_CASE(SCUMMVM51) size = MSRLEDecoder__decode8(in, zsize, out, size); break;
        QUICK_COMP_CASE(SCUMMVM52) size = unarj(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(SCUMMVM53) size = unarj(in, zsize, out, size, 1); break;
        QUICK_COMP_CASE(LZS_UNZIP)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the LZS_UNZIP decompression requires the setting of the dictionary in\n"
                    "       field comtype, like:\n"
                    "         comtype lzs_unzip \"abcabcabcabc\"\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            size = lzs_unzip(in, zsize, comtype_dictionary, comtype_dictionary_len, out, size);
            break;
        QUICK_COMP_CASE(LEGEND_OF_MANA) size = legend_of_mana(in, zsize, out, size); break;
        QUICK_COMP_CASE(DIZZY) size = dizzy(in, zsize, out); break;
        QUICK_COMP_CASE(EDL1) size = EDLdec1(0, in, out, zsize, size, 0); break;
        QUICK_COMP_CASE(EDL2) size = EDLdec2(0, in, out, zsize, size, 0); break;
        QUICK_COMP_CASE(DUNGEON_KID) size = dungeon_kid(in, out); break;
        QUICK_COMP_CASE(FRONTMISSION2) size = frontmission2(in, zsize, out); break;
        QUICK_COMP_CASE(RLEINC1) size = rleinc1(in, zsize, out); break;
        QUICK_COMP_CASE(RLEINC2) size = rleinc2(in, zsize, out); break;
        QUICK_COMP_CASE(EVOLUTION) size = evolution_unpack(in, zsize, out, size); break;
        QUICK_COMP_CASE(PUYO_LZ10) size = puyo_lz10(in, zsize, out, size); break;
        QUICK_COMP_CASE(PUYO_LZ11) size = puyo_lz11(in, zsize, out, size); break;
        QUICK_COMP_CASE(NISLZS) size = nislzs(in, out, size); break;
        QUICK_COMP_CASE(UNKNOWN1)  compression_unknown_init(); size = compression_unknown1(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN2)  compression_unknown_init(); size = compression_unknown2(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN3)  compression_unknown_init(); size = compression_unknown3(in, zsize, out, size, 0); break;
        QUICK_COMP_CASE(UNKNOWN4)  compression_unknown_init(); size = compression_unknown4(in, zsize, comtype_dictionary, comtype_dictionary_len, out, size); break;
        QUICK_COMP_CASE(UNKNOWN5)  compression_unknown_init(); size = compression_unknown5(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN6)  compression_unknown_init(); size = compression_unknown6(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN7)  compression_unknown_init(); size = compression_unknown7(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN8)  compression_unknown_init(); size = compression_unknown8(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN9)  compression_unknown_init(); size = compression_unknown9(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN10) compression_unknown_init(); size = compression_unknown10(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN11) compression_unknown_init(); size = compression_unknown11(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN12) compression_unknown_init(); size = compression_unknown12(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN13) compression_unknown_init(); size = compression_unknown13(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN14) compression_unknown_init(); size = compression_unknown14(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN15) compression_unknown_init(); size = compression_unknown15(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN16) compression_unknown_init(); size = compression_unknown16(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN17) compression_unknown_init(); size = compression_unknown17(in, zsize, out, size); break;
        QUICK_COMP_CASE(UNKNOWN18) compression_unknown_init(); size = compression_unknown18(in, zsize, comtype_dictionary, comtype_dictionary_len, out, size); break;
        QUICK_COMP_CASE(UNKNOWN19) compression_unknown_init(); size = compression_unknown19(comtype_dictionary, in, out, size); break;
        QUICK_COMP_CASE(BLACKDESERT) size = blackdesert_unpack(in, out); break;
        QUICK_COMP_CASE(BLACKDESERT_RAW) size = blackdesert_unpack_core(in, out, size, out, zsize); break;
        QUICK_COMP_CASE(PUCRUNCH) size = pucrunch_UnPack(-1, in, out, 0); break;
        QUICK_COMP_CASE(ZPAQ) size = unzpaq(in, zsize, out, size); break;

        /*
        ############
        compressions
        ############
        */

        QUICK_COMP_CASE(ZLIB_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = advancecomp_rfc1950(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(DEFLATE_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = advancecomp_deflate(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(LZO1_COMPRESS)
        QUICK_COMP_CASE(LZO1X_COMPRESS)
        QUICK_COMP_CASE(LZO2A_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzo_compress(in, zsize, out, size, compression_type);
            break;
        QUICK_COMP_CASE(XMEMLZX_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = xmem_compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(BZIP2_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = bzip2_compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(GZIP_COMPRESS)
            size = 20 + MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = gzip_compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(LZSS_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzss_compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(SFL_BLOCK_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = compress_block(in, out, zsize);
            break;
        QUICK_COMP_CASE(SFL_RLE_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = compress_rle(in, out, zsize);
            break;
        QUICK_COMP_CASE(SFL_NULLS_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = compress_nulls(in, out, zsize);
            break;
        QUICK_COMP_CASE(SFL_BITS_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = compress_bits(in, out, zsize);
            break;
        QUICK_COMP_CASE(LZF_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzf_compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(BRIEFLZ_COMPRESS)
            size = blz_max_packed_size(size);
            myalloc(&out, size, outsize);
            p = malloc(blz_workmem_size(zsize));
            if(!p) STD_ERR(QUICKBMS_ERROR_MEMORY);
            size = blz_pack(in, out, zsize, p);
            free(p);
            break;
#ifdef WIN32    // the alternative is using the compiled code directly
        QUICK_COMP_CASE(JCALG_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = JCALG1_Compress(in, zsize, out, 1024 * 1024, &JCALG1_AllocFunc, &JCALG1_DeallocFunc, &JCALG1_CallbackFunc, 0);
            break;
#endif
        QUICK_COMP_CASE(BCL_HUF_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = Huffman_Compress(in, out, zsize);
            break;
        QUICK_COMP_CASE(BCL_LZ_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = LZ_Compress(in, out, zsize);
            break;
        QUICK_COMP_CASE(BCL_RICE_COMPRESS)
            if(!comtype_dictionary) {
                fprintf(stderr, "\n"
                    "Error: the BCL_RICE decompression requires the setting of the dictionary in\n"
                    "       field comtype with the name of the variable containing the type of\n"
                    "       compression (from 1 to 8, read rice.h), like:\n"
                    "         comtype bcl_rice 1\n");
                //myexit(QUICKBMS_ERROR_BMS);
                size = -1;
                break;
            }
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = Rice_Compress(in, out, zsize,
                //get_var32(get_var_from_name(comtype_dictionary, comtype_dictionary_len))
                myatoi(comtype_dictionary)
            );
            break;
        QUICK_COMP_CASE(BCL_RLE_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = RLE_Compress(in, zsize, out, size);
            break;
        QUICK_COMP_CASE(BCL_SF_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = SF_Compress(in, out, zsize);
            break;
        QUICK_COMP_CASE(SZIP_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            t32 = *outsize;
            SZ_BufftoBuffCompress(out, &t32, in, zsize, NULL);
            *outsize = t32;
            break;
        QUICK_COMP_CASE(HUFFMANLIB_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            t32 = size;
            if(!huffman_encode_memory(in, zsize, &p, &t32)) {
                size = t32;
                myalloc(&out, size, outsize);
                memcpy(out, p, size);
                free(p);
            } else {
                size = -1;
            }
            break;
        QUICK_COMP_CASE(LZMA_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzma_compress(in, zsize, out, size, LZMA_FLAGS_NONE);
            break;
        QUICK_COMP_CASE(LZMA_86HEAD_COMPRESS)
            size = 8 + MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzma_compress(in, zsize, out, size, LZMA_FLAGS_86_HEADER);
            break;
        QUICK_COMP_CASE(LZMA_86DEC_COMPRESS)
            size = 1 + MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzma_compress(in, zsize, out, size, LZMA_FLAGS_86_DECODER);
            break;
        QUICK_COMP_CASE(LZMA_86DECHEAD_COMPRESS)
            size = 1 + 8 + MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzma_compress(in, zsize, out, size, LZMA_FLAGS_86_DECODER | LZMA_FLAGS_86_HEADER);
            break;
        QUICK_COMP_CASE(LZMA_EFS_COMPRESS)
            size = 4 + MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = lzma_compress(in, zsize, out, size, LZMA_FLAGS_EFS);
            break;
        QUICK_COMP_CASE(FALCOM_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = falcom_EncodeData(out, size, in, zsize);
            break;
        QUICK_COMP_CASE(KZIP_ZLIB_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = uberflate(in, zsize, out, size, 1);
            break;
        QUICK_COMP_CASE(KZIP_DEFLATE_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = uberflate(in, zsize, out, size, 0);
            break;
        QUICK_COMP_CASE(PRS_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = prs_compress(in, out, zsize);
            break;
        QUICK_COMP_CASE(RNC_COMPRESS)
            //size = MAXZIPLEN(size);
            //myalloc(&out, size, outsize);

            long plen;
            p = rnc_pack(in, zsize, &plen);
            if(p) {
                size = plen;
                myalloc(&out, size, outsize);
                memcpy(out, p, size);
                free(p);
            }
            break;
        QUICK_COMP_CASE(LZ4_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = LZ4_compressHC_limitedOutput(in, out, zsize, size);
            break;
        QUICK_COMP_CASE(SFL_BLOCK_CHUNKED_COMPRESS)
            size = MAXZIPLEN(size);
            myalloc(&out, size, outsize);
            size = sfl_block_chunked_compress(in, zsize, out, size);
            break;
        default: {
            fprintf(stderr, "\nError: unsupported compression type %d\n", (i32)compression_type);
            break;
        }
    }
    *ret_out = out;

    if(*outsize == old_outsize) {   // the check is made on those algorithms that don't reallocate out
        if(size > *outsize) {       // "limit" possible overflows with some unsafe algorithms (like sflcomp)
            fprintf(stderr, "\n"
                "Error: the uncompressed data (%"PRId") is bigger than the allocated buffer (%"PRId")\n", size, *outsize);
            myexit(QUICKBMS_ERROR_COMPRESSION);
        }
    }
    return(size);
}


