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

// QuickBMS CMD operations



int check_condition(int cmd, int res1, u8 *xcond, int res2) {
    static const int    MAX_IFS = 16;
    int     var1n   = 0,
            var2n   = 0,
            res     = FALSE,
            sign    = 0,
            idx,
            ifs,
            i,
            res_db[MAX_IFS];
    u8      *cond,
            *var1   = NULL,
            *var2   = NULL,
            *p,
            *cond_db[MAX_IFS];

    if(cmd >= 0) {
        if((CMD.var[0] < 0) || (CMD.var[2] < 0)) return(TRUE); // needed for CMD_Else!
    }

    idx = 0;
    for(ifs = 0; ifs < MAX_IFS; ifs++) {

        var1n   = 0;
        var2n   = 0;
        sign    = 0;
        var1    = NULL;
        var2    = NULL;

        if(cmd < 0) {
            var1n = res1;
            cond  = xcond;
            var2n = res2;
        } else {
            cond = STR(idx + 1);
            if(VARISNUM(idx + 0) && VARISNUM(idx + 2)) {
                var1n = VAR32(idx + 0);
                var2n = VAR32(idx + 2);
            } else {
                var1 = VAR(idx + 0);
                var2 = VAR(idx + 2);
                if(myisdechex_string(var1) && myisdechex_string(var2)) {
                    var1 = NULL;
                    var2 = NULL;
                    var1n = VAR32(idx + 0);
                    var2n = VAR32(idx + 2);
                }
                // in the For command I use a Set instruction at the beginning of the cycle with a String type
                // now the downside is that it's a bit slower but being used only at the beginning of the cycle there is no
                // loss of time (some milliseconds on tons of For cycles) and there is the pro of using also things like:
                //  for i = "hello" != "ciao"
            }
            idx += 3;
        }

        // replacing strcmp with a switch changes nothing in performance
        if(!cond) return(res);
        if(cond[0] == 'u') {    // only the first and only 'u' to avoid loss of performances
            sign = 1;
            cond++;
        }
        if(!strcmp(cond, "<") || !stricmp(cond, "minor") || !stricmp(cond, "lower") || !stricmp(cond, "below")) {
            if(var1 && var2) {
                if(sign) {
                    if(strcmp (var1, var2) < 0) res = TRUE;
                } else {
                    if(stricmp(var1, var2) < 0) res = TRUE;
                }
            } else {
                if(sign) {
                    if((u_int)var1n < (u_int)var2n) res = TRUE;
                } else {
                    if(var1n < var2n) res = TRUE;
                }
            }
        } else if(!strcmp(cond, ">") || !stricmp(cond, "major") || !stricmp(cond, "greater") || !stricmp(cond, "above")) {
            if(var1 && var2) {
                if(sign) {
                    if(strcmp (var1, var2) > 0) res = TRUE;
                } else {
                    if(stricmp(var1, var2) > 0) res = TRUE;
                }
            } else {
                if(sign) {
                    if((u_int)var1n > (u_int)var2n) res = TRUE;
                } else {
                    if(var1n > var2n) res = TRUE;
                }
            }
        } else if(!strcmp(cond, "<>") || !strcmp(cond, "!=") || !strcmp(cond, "!==") || !stricmp(cond, "different")) {
            if(var1 && var2) {
                if(sign) {
                    if(strcmp (var1, var2) != 0) res = TRUE;
                } else {
                    if(stricmp(var1, var2) != 0) res = TRUE;
                }
            } else {
                if(var1n != var2n) res = TRUE;
            }
        } else if(!strcmp(cond, "=") || !strcmp(cond, "==") || !strcmp(cond, "===") || !stricmp(cond, "equal") || !stricmp(cond, "strcmp")  || !stricmp(cond, "stricmp")  || !stricmp(cond, "strcasecmp")) {
            if(var1 && var2) {
                if(sign) {
                    if(!strcmp (var1, var2)) res = TRUE;
                } else {
                    if(!stricmp(var1, var2)) res = TRUE;
                }
            } else {
                if(var1n == var2n) res = TRUE;
            }
        } else if(!strcmp(cond, ">=")) {
            if(var1 && var2) {
                if(sign) {
                    if(strcmp (var1, var2) >= 0) res = TRUE;
                } else {
                    if(stricmp(var1, var2) >= 0) res = TRUE;
                }
            } else {
                if(sign) {
                    if((u_int)var1n >= (u_int)var2n) res = TRUE;
                } else {
                    if(var1n >= var2n) res = TRUE;
                }
            }
        } else if(!strcmp(cond, "<=")) {
            if(var1 && var2) {
                if(sign) {
                    if(strcmp (var1, var2) <= 0) res = TRUE;
                } else {
                    if(stricmp(var1, var2) <= 0) res = TRUE;
                }
            } else {
                if(sign) {
                    if((u_int)var1n <= (u_int)var2n) res = TRUE;
                } else {
                    if(var1n <= var2n) res = TRUE;
                }
            }
        // added by me
        } else if(!strcmp(cond, "&") || !strcmp(cond, "&&") || !stricmp(cond, "and") || !stricmp(cond, "strstr") || !stricmp(cond, "stristr") || !stricmp(cond, "strcasestr")) {
            if(var1 && var2) {
                if(sign) {
                    if(strstr (var1, var2)) res = TRUE;
                } else {
                    if(stristr(var1, var2)) res = TRUE;
                }
            } else {
                if(var1n & var2n) res = TRUE;
            }
        } else if(!strcmp(cond, "^") || !strcmp(cond, "^^") || !stricmp(cond, "xor")) {
            if(var1 && var2) {
                if(sign) {
                    if(!strcmp (var1, var2)) res = TRUE;
                } else {
                    if(!stricmp(var1, var2)) res = TRUE;
                }
            } else {
                if(var1n ^ var2n) res = TRUE;
            }
        } else if(!strcmp(cond, "|") || !strcmp(cond, "||") || !stricmp(cond, "or")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(var1n | var2n) res = TRUE;
            }
        } else if(!strcmp(cond, "%") || !stricmp(cond, "mod")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(sign) {
                    if(!var2n || ((u_int)var1n % (u_int)var2n)) res = TRUE;
                } else {
                    if(!var2n || (var1n % var2n)) res = TRUE;
                }
            }
        } else if(!strcmp(cond, "/") || !stricmp(cond, "div")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(sign) {
                    if(!var2n || ((u_int)var1n / (u_int)var2n)) res = TRUE;
                } else {
                    if(!var2n || (var1n / var2n)) res = TRUE;
                }
            }
        } else if(!strcmp(cond, "<<") || !stricmp(cond, "shl")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(sign) {
                    if((u_int)var1n << (u_int)var2n) res = TRUE;
                } else {
                    if(var1n << var2n) res = TRUE;
                }
            }
        } else if(!strcmp(cond, ">>") || !stricmp(cond, "shr")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(sign) {
                    if((u_int)var1n >> (u_int)var2n) res = TRUE;
                } else {
                    if(var1n >> var2n) res = TRUE;
                }
            }
        } else if(!strcmp(cond, "!") || !stricmp(cond, "not")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(!var1n) res = TRUE;
            }
        } else if(!strcmp(cond, "!!")) {
            if(var1) {
                res = TRUE;
            } else {
                if(var1n) res = TRUE;
            }
        } else if(!strcmp(cond, "~") || !stricmp(cond, "complement")) {
            if(var1 && var2) {
                res = TRUE;
            } else {
                if(~var1n) res = TRUE;
            }
        } else if(!stricmp(cond, "strncmp") || !stricmp(cond, "strnicmp") || !stricmp(cond, "strncasecmp")) {
            if(var1 && var2) {
                if(sign) {
                    if(!strncmp (var1, var2, strlen(var2))) res = TRUE;
                } else {
                    if(!strnicmp(var1, var2, strlen(var2))) res = TRUE;
                }
            } else {
                if((var1n & var2n) == var2n) res = TRUE;
            }
        } else if(!stricmp(cond, "ext") || !stricmp(cond, "extension")) {
            if(var1 && var2) {
                p = strrchr(var1, '.');
                if(p) {
                    if(sign) {
                        if(!strcmp (p + 1, var2)) res = TRUE;
                    } else {
                        if(!stricmp(p + 1, var2)) res = TRUE;
                    }
                }
            } else {
                res = TRUE;
            }
        } else if(!stricmp(cond, "basename")) {
            if(var1 && var2) {
                p = strrchr(var1, '.');
                if(p) {
                    *p = 0;
                    if(sign) {
                        if(!strcmp (var1, var2)) res = TRUE;
                    } else {
                        if(!stricmp(var1, var2)) res = TRUE;
                    }
                    *p = '.';
                }
            } else {
                res = TRUE;
            }
        } else {
            if(var1 && var2) {
                fprintf(stderr, "\nError: invalid condition %s\n", cond);
                myexit(QUICKBMS_ERROR_BMS);
            }
            if(math_operations(var1n, cond[0], var2n, sign)) res = TRUE;
        }
        if(verbose > 0) printf("             condition %s is%smet\n", cond, (res == TRUE) ? " " : " not ");

        if(cmd < 0) break;
        res_db[ifs] = res;
        if((idx + 4) > MAX_ARGS) break;
        cond = STR(idx);
        if(!cond) break;
        idx++;
        cond_db[ifs] = cond;
    }

    while(ifs > 0) {
        for(i = 0; (i + 1) <= ifs; i++) {
            res = check_condition(-1, res_db[i], cond_db[i], res_db[i + 1]);
            res_db[i + 1] = res;
        }
        ifs--;
        for(i = 0; (i + 1) <= ifs; i++) {
            res_db[i] = res_db[i + 1];
        }
    }

    return(res);
}



int CMD_CLog_func(int cmd) {
    int     fd,
            offset,
            size,
            zsize;
    u8      *name;

    name    = VAR(0);
    offset  = VAR32(1);
    zsize   = VAR32(2);
    size    = VAR32(5);
    fd      = FILEZ(7);

    if(dumpa(fd, name, VARNAME(0), offset, size, zsize) < 0) return(-1);
    return(0);
}



int CMD_FindLoc_func(int cmd) {
    static u8   *sign   = NULL,
                *buff   = NULL;
    int     fd,
            i,
            idx,
            len,
            oldoff,
            tmpoff,
            offset  = -1,
            str_len,
            sign_len;
    u8      *str,
            *ret_if_error;

    fd      = FILEZ(3);
    oldoff  = myftell(fd);
    str     = STR(2);   // remember that str can be also a sequence of bytes, included 0x00!
    str_len = NUM(2);
    ret_if_error = STR(4);

    // the following has been disabled because causes troubles with old scripts
    //quick_var_from_name_check(&str, &str_len);  // so it supports also the variables

    if(
        (NUM(1) == TYPE_STRING) ||
        (NUM(1) == TYPE_BINARY)
    ) {
        if(str_len == strlen(str)) {
            idx = get_var_from_name(str, -1);
            if(idx >= 0) {
                str     = get_var(idx);
                str_len = strlen(str);
            }
        }

        sign_len = str_len;
        if(sign_len == -1) ALLOC_ERR;
        sign = realloc(sign, sign_len + 1);
        if(!sign) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memcpy(sign, str, sign_len);
        sign[sign_len] = 0;

    } else if(NUM(1) == TYPE_UNICODE) {
        if(str_len == strlen(str)) {
            idx = get_var_from_name(str, -1);
            if(idx >= 0) {
                str     = get_var(idx);
                str_len = strlen(str);
            }
        }

        sign_len = (str_len + 1) * sizeof(u16) /*sizeof(wchar_t)*/;  // yeah includes also the NULL delimiter, boring unicode
        if(sign_len < str_len) ALLOC_ERR;
        if(sign_len == -1) ALLOC_ERR;
        sign = realloc(sign, sign_len + 1);
        if(!sign) STD_ERR(QUICKBMS_ERROR_MEMORY);

        utf8_to_utf16(str, str_len, sign, sign_len, 0);

    } else {
        sign_len = NUM(1);  // yeah the type in NUM(1) is written for having the size of the parameter, watch enum
        if(sign_len < 0) STD_ERR(QUICKBMS_ERROR_BMS);
        if(sign_len == -1) ALLOC_ERR;
        sign = realloc(sign, sign_len + 1);
        if(!sign) STD_ERR(QUICKBMS_ERROR_MEMORY);
        sign[sign_len] = 0;
        putxx(sign, myatoi(str), sign_len);

        if(str_len == strlen(str)) {
            idx = get_var_from_name(str, -1);
            if(idx >= 0) {
                sign_len = sizeof(u32);   // 32bit
                if(sign_len == -1) ALLOC_ERR;
                sign = realloc(sign, sign_len + 1);
                if(!sign) STD_ERR(QUICKBMS_ERROR_MEMORY);
                sign[sign_len] = 0;
                putxx(sign, get_var32(idx), sign_len);
            }
        }
    }
    if(sign_len <= 0) goto quit;
    if(sign_len > BUFFSZ) { // lazy boy
        fprintf(stderr, "\nError: the FindLoc function works with a searchable string of max %d bytes\n", BUFFSZ);
        myexit(QUICKBMS_ERROR_BMS);
    }

    if(!buff) {
        buff = malloc(BUFFSZ + 1);
        if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
    }
    tmpoff = oldoff;

    for(;;) {
        len = myfr(fd, buff, -1);   // -1 uses BUFFSZ automatically and doesn't quit if the file terminates
        if(len < sign_len) break;   // performes (len <= 0) too automatically
        for(i = 0; i <= (len - sign_len); i++) {
            if(!memcmp(buff + i, sign, sign_len)) {
                offset = (myftell(fd) - len) + i;
                goto quit;
            }
        }
        tmpoff += i + 1;
        myfseek(fd, tmpoff, SEEK_SET);
        //myfseek(fd, sign_len - 1, SEEK_CUR);
    }

quit:
    myfseek(fd, oldoff, SEEK_SET);
    if(offset == -1) {
        if(ret_if_error) {
            add_var(CMD.var[0], NULL, ret_if_error, 0, -1);
        } else {
            return(-1); // confirmed
        }
    } else {
        add_var(CMD.var[0], NULL, NULL, offset, sizeof(int));
    }
    return(0);
}



int CMD_GetBits_func(int cmd) {
    FDBITS
    int     fd,
            len     = -1,
            tmpn    = 0,
            bits,
            verbose_offset = 0;
    u8      *tmp    = NULL;

    fd   = NUM(2);
    bits = VAR32(1);

    if(verbose < 0) verbose_offset = myftell(fd);

    my_fdbits(fd, &bitchr, &bitpos, &bitoff, 0, 0, 0);
    if(myftell(fd) != bitoff) {
        bitchr = 0;
        bitpos = 0;
    }
    if(bits <= 32) {
        tmpn = fd_read_bits(bits, &bitchr, &bitpos, fd);
    } else {
        len = ((bits + 7) & (~7)) / 8;
        tmp = calloc(len + 1, 1);
        if(!tmp) STD_ERR(QUICKBMS_ERROR_MEMORY);
        len = bits2str(tmp, len, bits, &bitchr, &bitpos, fd);
    }
    my_fdbits(fd, NULL, NULL, NULL, bitchr, bitpos, myftell(fd));

    if(tmp) {
        if(verbose < 0) verbose_print(verbose_offset, "getbits", CMD.var[0], tmp, len, 0, bits);
        add_var(CMD.var[0], NULL, tmp, 0, len);
        FREEZ(tmp)
    } else {
        if(verbose < 0) verbose_print(verbose_offset, "getbits", CMD.var[0], NULL, 0, tmpn, bits);
        add_var(CMD.var[0], NULL, NULL, tmpn, sizeof(int));
    }
    return(0);
}



int CMD_PutBits_func(int cmd) {
    FDBITS
    int     fd,
            len     = -1,
            tmpn    = 0,
            bits,
            verbose_offset = 0;
    u8      *tmp    = NULL;

    fd   = NUM(2);
    bits = VAR32(1);

    if(verbose < 0) verbose_offset = myftell(fd);

    my_fdbits(fd, &bitchr, &bitpos, &bitoff, 0, 0, 0);
    if(myftell(fd) != bitoff) {
        bitchr = 0;
        bitpos = 0;
    }
    if(bits <= 32) {
        tmpn = VAR32(0);
        if(verbose < 0) verbose_print(verbose_offset, "putbits", CMD.var[0], NULL, 0, tmpn, bits);
        fd_write_bits(tmpn, bits, &bitchr, &bitpos, fd);
    } else {
        len = ((bits + 7) & (~7)) / 8;
        tmp = VAR(0);
        if(len > VARSZ(0)) len = VARSZ(0);
        if(verbose < 0) verbose_print(verbose_offset, "putbits", CMD.var[0], tmp, len, 0, bits);
        len = str2bits(tmp, len, bits, &bitchr, &bitpos, fd);
    }
    my_fdbits(fd, NULL, NULL, NULL, bitchr, bitpos, myftell(fd));
    return(0);
}



int CMD_Get_func(int cmd) {
    int     fd,
            type,
            tmpn    = 0,
            error   = 0,
            verbose_offset = 0;
    u8      *tmp    = NULL;

    fd   = FILEZ(2);
    type = NUM(1);

    if(verbose < 0) verbose_offset = myftell(fd);

    tmp = myfrx(fd, type, &tmpn, &error);
    // now tmp can be also NULL because myfrx is string/int
    //if(!tmp) return(-1);    // here should be good to quit... but I leave it as is for back compatibility with the old quickbms!
    if(error) return(-1);
    if(tmp) {
        if(verbose < 0) verbose_print(verbose_offset, "get", CMD.var[0], tmp, -1, 0, type);
        add_var(CMD.var[0], NULL, tmp, 0, -1);
    } else {
        if(verbose < 0) verbose_print(verbose_offset, "get", CMD.var[0], NULL, 0, tmpn, type);
        add_var(CMD.var[0], NULL, NULL, tmpn, sizeof(int));
    }
    return(0);
}



int CMD_IDString_func(int cmd) {
    static int  buffsz  = 0;
    static u8   *sign   = NULL,
                *buff   = NULL;
    int     fd,
            len;

    fd   = FILEZ(0);
    sign = STR(1);
    len  = NUM(1);
    if(len == -1) ALLOC_ERR;    // in case of updates
    myalloc(&buff, len, &buffsz);   // memcmp, so not + 1
    myfr(fd, buff, len);
    if(memcmp(buff, sign, len)) {
        if((len == 4) &&    // automatic endianess... cool
           (buff[0] == sign[3]) && (buff[1] == sign[2]) && 
           (buff[2] == sign[1]) && (buff[3] == sign[0])) {
            endian = (endian == MYLITTLE_ENDIAN) ? MYBIG_ENDIAN : MYLITTLE_ENDIAN;
            return(0);
        }
        /*
        fprintf(stderr, "\n"
            "- signature doesn't match the one expected by the script:\n"
            "  this one: \"%.60s\"\n"
            "  expected: \"%.60s\"\n",
            buff, sign);
        */
        fprintf(stderr, "\n"
            "- signature of %d bytes at offset 0x%"PRIx" doesn't match the one\n"
            "  expected by the script:\n"
            "\n"
            "  this one: \"%.60s\"\n",
            (i32)len, myftell(fd) - len,
            buff);
        show_dump(2, buff, len, stderr);
        fprintf(stderr, "\n"
            "  expected: \"%.60s\"\n",
            sign);
        show_dump(2, sign, len, stderr);
        return(-1);
    }
    return(0);
}



int CMD_GoTo_func(int cmd) {
    int     fd,
            pos;
    u8      *str;

    fd  = FILEZ(1);
    str = VAR(0);

    if(!stricmp(str, "SOF") || !stricmp(str, "SEEK_SET")) {
        myfseek(fd, 0, SEEK_SET);
    } else if(!stricmp(str, "EOF") || !stricmp(str, "SEEK_END")) {
        myfseek(fd, 0, SEEK_END);
    } else {
        pos = VAR32(0);
        if((NUM(2) == SEEK_SET) && (pos < 0) && var_is_a_constant(CMD.var[0])) {
            myfseek(fd, pos, SEEK_END); // only constants can be negative, not vars
        } else {
            myfseek(fd, pos, NUM(2));   //SEEK_SET);
        }
    }
    return(0);
}



int CMD_SavePos_func(int cmd) {
    int     fd;

    fd  = FILEZ(1);
    add_var(CMD.var[0], NULL, NULL, myftell(fd), sizeof(int));
    return(0);
}



int old_set_math_operator(u8 **data) {
    int     ret = 0;
    u8      *p;

    p = *data;

    #define SET_MATH_OPERATOR_ASSIGN(X, Y) \
        if(!strnicmp(p, X, sizeof(X) -1)) { \
            ret = Y; \
            p += sizeof(X) - 1; \
        } else

    SET_MATH_OPERATOR_ASSIGN("<<<", 'l')
    SET_MATH_OPERATOR_ASSIGN(">>>", 'r')
    SET_MATH_OPERATOR_ASSIGN("**", 'p')
    SET_MATH_OPERATOR_ASSIGN("//", 'v')
    SET_MATH_OPERATOR_ASSIGN("&&", 'x')
    SET_MATH_OPERATOR_ASSIGN("<>", 'z')
    SET_MATH_OPERATOR_ASSIGN("%%", -176)
    {}

    if(*p == '?') {
        p++;
        SET_MATH_OPERATOR_ASSIGN("add", '+')
        SET_MATH_OPERATOR_ASSIGN("mul", '*')
        SET_MATH_OPERATOR_ASSIGN("div", '/')
        SET_MATH_OPERATOR_ASSIGN("sub", '-')
        SET_MATH_OPERATOR_ASSIGN("xor", '^')
        SET_MATH_OPERATOR_ASSIGN("and", '&')
        SET_MATH_OPERATOR_ASSIGN("or",  '|')
        SET_MATH_OPERATOR_ASSIGN("mod", '%')
        SET_MATH_OPERATOR_ASSIGN("not", '!')
        SET_MATH_OPERATOR_ASSIGN("com", '~')
        SET_MATH_OPERATOR_ASSIGN("shl", '<')
        SET_MATH_OPERATOR_ASSIGN("shr", '>')
        SET_MATH_OPERATOR_ASSIGN("rol", 'l')
        SET_MATH_OPERATOR_ASSIGN("ror", 'r')
        SET_MATH_OPERATOR_ASSIGN("byteswap",'s')
        SET_MATH_OPERATOR_ASSIGN("bitswap", 'w')
        SET_MATH_OPERATOR_ASSIGN("equal", '=')
        SET_MATH_OPERATOR_ASSIGN("neg", 'n')
        SET_MATH_OPERATOR_ASSIGN("abs", 'a')
        SET_MATH_OPERATOR_ASSIGN("radix", 'v')
        SET_MATH_OPERATOR_ASSIGN("pow", 'p')
        SET_MATH_OPERATOR_ASSIGN("align", 'x')
        SET_MATH_OPERATOR_ASSIGN("bitswap2", 'z')
        // math.h
        SET_MATH_OPERATOR_ASSIGN("sin", -1)
        SET_MATH_OPERATOR_ASSIGN("cos", -2)
        SET_MATH_OPERATOR_ASSIGN("tan", -3)
        SET_MATH_OPERATOR_ASSIGN("sinh", -4)
        SET_MATH_OPERATOR_ASSIGN("cosh", -5)
        SET_MATH_OPERATOR_ASSIGN("tanh", -6)
        SET_MATH_OPERATOR_ASSIGN("asin", -7)
        SET_MATH_OPERATOR_ASSIGN("acos", -8)
        SET_MATH_OPERATOR_ASSIGN("atan", -9)
        SET_MATH_OPERATOR_ASSIGN("atan2", -10)
        SET_MATH_OPERATOR_ASSIGN("exp", -11)
        SET_MATH_OPERATOR_ASSIGN("log", -12)
        SET_MATH_OPERATOR_ASSIGN("log10", -13)
        SET_MATH_OPERATOR_ASSIGN("pow", -14)
        SET_MATH_OPERATOR_ASSIGN("sqrt", -15)
        SET_MATH_OPERATOR_ASSIGN("ceil", -16)
        SET_MATH_OPERATOR_ASSIGN("floor", -17)
        SET_MATH_OPERATOR_ASSIGN("fabs", -18)
        SET_MATH_OPERATOR_ASSIGN("ldexp", -19)
        SET_MATH_OPERATOR_ASSIGN("frexp", -20)
        SET_MATH_OPERATOR_ASSIGN("modf", -21)
        SET_MATH_OPERATOR_ASSIGN("fmod", -22)
        SET_MATH_OPERATOR_ASSIGN("_hypot", -23)
        SET_MATH_OPERATOR_ASSIGN("_j0", -24)
        SET_MATH_OPERATOR_ASSIGN("_j1", -25)
        SET_MATH_OPERATOR_ASSIGN("_jn", -26)
        SET_MATH_OPERATOR_ASSIGN("_y0", -27)
        SET_MATH_OPERATOR_ASSIGN("_y1", -28)
        SET_MATH_OPERATOR_ASSIGN("_yn", -29)
        SET_MATH_OPERATOR_ASSIGN("sinf", -30)
        SET_MATH_OPERATOR_ASSIGN("sinl", -31)
        SET_MATH_OPERATOR_ASSIGN("cosf", -32)
        SET_MATH_OPERATOR_ASSIGN("cosl", -33)
        SET_MATH_OPERATOR_ASSIGN("tanf", -34)
        SET_MATH_OPERATOR_ASSIGN("tanl", -35)
        SET_MATH_OPERATOR_ASSIGN("asinf", -36)
        SET_MATH_OPERATOR_ASSIGN("asinl", -37)
        SET_MATH_OPERATOR_ASSIGN("acosf", -38)
        SET_MATH_OPERATOR_ASSIGN("acosl", -39)
        SET_MATH_OPERATOR_ASSIGN("atanf", -40)
        SET_MATH_OPERATOR_ASSIGN("atanl", -41)
        SET_MATH_OPERATOR_ASSIGN("atan2f", -42)
        SET_MATH_OPERATOR_ASSIGN("atan2l", -43)
        SET_MATH_OPERATOR_ASSIGN("sinhf", -44)
        SET_MATH_OPERATOR_ASSIGN("sinhl", -45)
        SET_MATH_OPERATOR_ASSIGN("coshf", -46)
        SET_MATH_OPERATOR_ASSIGN("coshl", -47)
        SET_MATH_OPERATOR_ASSIGN("tanhf", -48)
        SET_MATH_OPERATOR_ASSIGN("tanhl", -49)
        SET_MATH_OPERATOR_ASSIGN("acosh", -50)
        SET_MATH_OPERATOR_ASSIGN("acoshf", -51)
        SET_MATH_OPERATOR_ASSIGN("acoshl", -52)
        SET_MATH_OPERATOR_ASSIGN("asinh", -53)
        SET_MATH_OPERATOR_ASSIGN("asinhf", -54)
        SET_MATH_OPERATOR_ASSIGN("asinhl", -55)
        SET_MATH_OPERATOR_ASSIGN("atanh", -56)
        SET_MATH_OPERATOR_ASSIGN("atanhf", -57)
        SET_MATH_OPERATOR_ASSIGN("atanhl", -58)
        SET_MATH_OPERATOR_ASSIGN("expf", -59)
        SET_MATH_OPERATOR_ASSIGN("expl", -60)
        SET_MATH_OPERATOR_ASSIGN("exp2", -61)
        SET_MATH_OPERATOR_ASSIGN("exp2f", -62)
        SET_MATH_OPERATOR_ASSIGN("exp2l", -63)
        SET_MATH_OPERATOR_ASSIGN("expm1", -64)
        SET_MATH_OPERATOR_ASSIGN("expm1f", -65)
        SET_MATH_OPERATOR_ASSIGN("expm1l", -66)
        SET_MATH_OPERATOR_ASSIGN("frexpf", -67)
        SET_MATH_OPERATOR_ASSIGN("frexpl", -68)
        SET_MATH_OPERATOR_ASSIGN("ilogb", -69)
        SET_MATH_OPERATOR_ASSIGN("ilogbf", -70)
        SET_MATH_OPERATOR_ASSIGN("ilogbl", -71)
        SET_MATH_OPERATOR_ASSIGN("ldexpf", -72)
        SET_MATH_OPERATOR_ASSIGN("ldexpl", -73)
        SET_MATH_OPERATOR_ASSIGN("logf", -74)
        SET_MATH_OPERATOR_ASSIGN("logl", -75)
        SET_MATH_OPERATOR_ASSIGN("log10f", -76)
        SET_MATH_OPERATOR_ASSIGN("log10l", -77)
        SET_MATH_OPERATOR_ASSIGN("log1p", -78)
        SET_MATH_OPERATOR_ASSIGN("log1pf", -79)
        SET_MATH_OPERATOR_ASSIGN("log1pl", -80)
        SET_MATH_OPERATOR_ASSIGN("log2", -81)
        SET_MATH_OPERATOR_ASSIGN("log2f", -82)
        SET_MATH_OPERATOR_ASSIGN("log2l", -83)
        SET_MATH_OPERATOR_ASSIGN("logb", -84)
        SET_MATH_OPERATOR_ASSIGN("logbf", -85)
        SET_MATH_OPERATOR_ASSIGN("logbl", -86)
        SET_MATH_OPERATOR_ASSIGN("modff", -87)
        SET_MATH_OPERATOR_ASSIGN("modfl", -88)
        SET_MATH_OPERATOR_ASSIGN("scalbn", -89)
        SET_MATH_OPERATOR_ASSIGN("scalbnf", -90)
        SET_MATH_OPERATOR_ASSIGN("scalbnl", -91)
        SET_MATH_OPERATOR_ASSIGN("scalbln", -92)
        SET_MATH_OPERATOR_ASSIGN("scalblnf", -93)
        SET_MATH_OPERATOR_ASSIGN("scalblnl", -94)
        SET_MATH_OPERATOR_ASSIGN("cbrt", -95)
        SET_MATH_OPERATOR_ASSIGN("cbrtf", -96)
        SET_MATH_OPERATOR_ASSIGN("cbrtl", -97)
        SET_MATH_OPERATOR_ASSIGN("fabsf", -98)
        SET_MATH_OPERATOR_ASSIGN("fabsl", -99)
        SET_MATH_OPERATOR_ASSIGN("hypot", -100)
        SET_MATH_OPERATOR_ASSIGN("hypotf", -101)
        SET_MATH_OPERATOR_ASSIGN("hypotl", -102)
        SET_MATH_OPERATOR_ASSIGN("powf", -103)
        SET_MATH_OPERATOR_ASSIGN("powl", -104)
        SET_MATH_OPERATOR_ASSIGN("sqrtf", -105)
        SET_MATH_OPERATOR_ASSIGN("sqrtl", -106)
        SET_MATH_OPERATOR_ASSIGN("erf", -107)
        SET_MATH_OPERATOR_ASSIGN("erff", -108)
        SET_MATH_OPERATOR_ASSIGN("erfl", -109)
        SET_MATH_OPERATOR_ASSIGN("erfc", -110)
        SET_MATH_OPERATOR_ASSIGN("erfcf", -111)
        SET_MATH_OPERATOR_ASSIGN("erfcl", -112)
        SET_MATH_OPERATOR_ASSIGN("lgamma", -113)
        SET_MATH_OPERATOR_ASSIGN("lgammaf", -114)
        SET_MATH_OPERATOR_ASSIGN("lgammal", -115)
        SET_MATH_OPERATOR_ASSIGN("tgamma", -116)
        SET_MATH_OPERATOR_ASSIGN("tgammaf", -117)
        SET_MATH_OPERATOR_ASSIGN("tgammal", -118)
        SET_MATH_OPERATOR_ASSIGN("ceilf", -119)
        SET_MATH_OPERATOR_ASSIGN("ceill", -120)
        SET_MATH_OPERATOR_ASSIGN("floorf", -121)
        SET_MATH_OPERATOR_ASSIGN("floorl", -122)
        SET_MATH_OPERATOR_ASSIGN("nearbyint", -123)
        SET_MATH_OPERATOR_ASSIGN("nearbyintf", -124)
        SET_MATH_OPERATOR_ASSIGN("nearbyintl", -125)
        SET_MATH_OPERATOR_ASSIGN("rint", -126)
        SET_MATH_OPERATOR_ASSIGN("rintf", -127)
        SET_MATH_OPERATOR_ASSIGN("rintl", -128)
        SET_MATH_OPERATOR_ASSIGN("lrint", -129)
        SET_MATH_OPERATOR_ASSIGN("lrintf", -130)
        SET_MATH_OPERATOR_ASSIGN("lrintl", -131)
        SET_MATH_OPERATOR_ASSIGN("llrint", -132)
        SET_MATH_OPERATOR_ASSIGN("llrintf", -133)
        SET_MATH_OPERATOR_ASSIGN("llrintl", -134)
        SET_MATH_OPERATOR_ASSIGN("round", -135)
        SET_MATH_OPERATOR_ASSIGN("roundf", -136)
        SET_MATH_OPERATOR_ASSIGN("roundl", -137)
        SET_MATH_OPERATOR_ASSIGN("lround", -138)
        SET_MATH_OPERATOR_ASSIGN("lroundf", -139)
        SET_MATH_OPERATOR_ASSIGN("lroundl", -140)
        SET_MATH_OPERATOR_ASSIGN("llround", -141)
        SET_MATH_OPERATOR_ASSIGN("llroundf", -142)
        SET_MATH_OPERATOR_ASSIGN("llroundl", -143)
        SET_MATH_OPERATOR_ASSIGN("trunc", -144)
        SET_MATH_OPERATOR_ASSIGN("truncf", -145)
        SET_MATH_OPERATOR_ASSIGN("truncl", -146)
        SET_MATH_OPERATOR_ASSIGN("fmodf", -147)
        SET_MATH_OPERATOR_ASSIGN("fmodl", -148)
        SET_MATH_OPERATOR_ASSIGN("remainder", -149)
        SET_MATH_OPERATOR_ASSIGN("remainderf", -150)
        SET_MATH_OPERATOR_ASSIGN("remainderl", -151)
        SET_MATH_OPERATOR_ASSIGN("remquo", -152)
        SET_MATH_OPERATOR_ASSIGN("remquof", -153)
        SET_MATH_OPERATOR_ASSIGN("remquol", -154)
        SET_MATH_OPERATOR_ASSIGN("copysign", -155)
        SET_MATH_OPERATOR_ASSIGN("copysignf", -156)
        SET_MATH_OPERATOR_ASSIGN("copysignl", -157)
        SET_MATH_OPERATOR_ASSIGN("nextafter", -158)
        SET_MATH_OPERATOR_ASSIGN("nextafterf", -159)
        SET_MATH_OPERATOR_ASSIGN("nextafterl", -160)
        SET_MATH_OPERATOR_ASSIGN("nexttoward", -161)
        SET_MATH_OPERATOR_ASSIGN("nexttowardf", -162)
        SET_MATH_OPERATOR_ASSIGN("nexttowardl", -163)
        SET_MATH_OPERATOR_ASSIGN("fdim", -164)
        SET_MATH_OPERATOR_ASSIGN("fdimf", -165)
        SET_MATH_OPERATOR_ASSIGN("fdiml", -166)
        SET_MATH_OPERATOR_ASSIGN("fmax", -167)
        SET_MATH_OPERATOR_ASSIGN("fmaxf", -168)
        SET_MATH_OPERATOR_ASSIGN("fmaxl", -169)
        SET_MATH_OPERATOR_ASSIGN("fmin", -170)
        SET_MATH_OPERATOR_ASSIGN("fminf", -171)
        SET_MATH_OPERATOR_ASSIGN("fminl", -172)
        SET_MATH_OPERATOR_ASSIGN("fma", -173)
        SET_MATH_OPERATOR_ASSIGN("fmaf", -174)
        SET_MATH_OPERATOR_ASSIGN("fmal", -175)
        SET_MATH_OPERATOR_ASSIGN("perc", -176)
        {} // else
    } else {
        ret = tolower(*p);
        p++;
    }
    *data = p;
    return(ret);
}



int set_math_operator(u8 *data, int *ret_sign, u8 **ret_next) {
    int     ret     = 0,
            sign    = 0;
    u8      *p;

    if(ret_sign) *ret_sign = 0;
    if(ret_next) *ret_next = NULL;
    for(p = data; *p; p++) {
        if(*p <= ' ') break;
        if(tolower(*p) == 'u') {          // unsigned
            sign = 1;
        //} else if(tolower(*p) == 'i') {   // signed (default)
            //CMD.num[2] = 0;
        } else if(!ret) {    // operator
            ret = old_set_math_operator(&p); //tolower(*p);
            p--; // needed
        }
    }
    if(ret_sign) *ret_sign = sign;
    if(ret_next) *ret_next = p;
    return ret;
}



int math_operations(int var1i, int op, int var2i, int sign) {
#define DO_MATH_SIGN(var1,var2) \
    switch(op) { \
        case '+': var1 += var2;                 break; \
        case '*': var1 *= var2;                 break; \
        case '/': if(!var2) { var1 = 0; } else { var1 /= var2; } break; \
        case '-': var1 -= var2;                 break; \
        case '^': var1 ^= var2;                 break; \
        case '&': var1 &= var2;                 break; \
        case '|': var1 |= var2;                 break; \
        case '%': if(!var2) { var1 = 0; } else { var1 %= var2; } break; \
        case '!': var1 = !var2;                 break; \
        case '~': var1 = ~var2;                 break; \
        case '<': var1 = var1 << var2;          break; \
        case '>': var1 = var1 >> var2;          break; \
        case 'l': var1 = rol(var1, var2);       break; \
        case 'r': var1 = ror(var1, var2);       break; \
        case 's': var1 = byteswap(var1, var2);  break; \
        case 'w': var1 = bitswap(var1, var2);   break; \
        case '=': var1 = var2;                  break; \
        case 'n': var1 = -var2;                 break; \
        case 'a': var1 = (var2 < 0) ? (-var2) : var2;   break; \
        case 'v': var1 = radix(var1, var2);     break; \
        case 'p': var1 = power(var1, var2);     break; \
        case 'x': if(var2 && (var1 % var2)) { var1 += (var2 - (var1 % var2)); } break; \
        case 'z': { \
            var1 &= (((u_int)1 << (var2 * (u_int)2)) - (u_int)1); \
            var1 = (var1 << var2) | (var1 >> var2); \
            var1 &= (((u_int)1 << (var2 * (u_int)2)) - (u_int)1); \
            break; \
        } \
        case -1: var1 = sin(var1); break; \
        case -2: var1 = cos(var1); break; \
        case -3: var1 = tan(var1); break; \
        case -4: var1 = sinh(var1); break; \
        case -5: var1 = cosh(var1); break; \
        case -6: var1 = tanh(var1); break; \
        case -7: var1 = asin(var1); break; \
        case -8: var1 = acos(var1); break; \
        case -9: var1 = atan(var1); break; \
        case -10: var1 = atan2(var1, var2); break; \
        case -11: var1 = exp(var1); break; \
        case -12: var1 = log(var1); break; \
        case -13: var1 = log10(var1); break; \
        case -14: var1 = pow(var1, var2); break; \
        case -15: var1 = sqrt(var1); break; \
        case -16: var1 = ceil(var1); break; \
        case -17: var1 = floor(var1); break; \
        case -18: var1 = fabs(var1); break; \
        case -19: var1 = ldexp(var1, var2); break; \
        case -20: var1 = frexp(var2, (void *)&var1); break; \
        case -21: var1 = modf(var2, (void *)&var1); break; \
        case -22: var1 = fmod(var1, var2); break; \
        case -30: var1 = sinf(var1); break; \
        case -31: var1 = sinl(var1); break; \
        case -32: var1 = cosf(var1); break; \
        case -33: var1 = cosl(var1); break; \
        case -34: var1 = tanf(var1); break; \
        case -35: var1 = tanl(var1); break; \
        case -36: var1 = asinf(var1); break; \
        case -37: var1 = asinl(var1); break; \
        case -38: var1 = acosf(var1); break; \
        case -39: var1 = acosl(var1); break; \
        case -40: var1 = atanf(var1); break; \
        case -41: var1 = atanl(var1); break; \
        case -42: var1 = atan2f(var1, var2); break; \
        case -43: var1 = atan2l(var1, var2); break; \
        case -44: var1 = sinhf(var1); break; \
        case -45: var1 = sinhl(var1); break; \
        case -46: var1 = coshf(var1); break; \
        case -47: var1 = coshl(var1); break; \
        case -48: var1 = tanhf(var1); break; \
        case -49: var1 = tanhl(var1); break; \
        case -50: var1 = acosh(var1); break; \
        case -51: var1 = acoshf(var1); break; \
        case -52: var1 = acoshl(var1); break; \
        case -53: var1 = asinh(var1); break; \
        case -54: var1 = asinhf(var1); break; \
        case -55: var1 = asinhl(var1); break; \
        case -56: var1 = atanh(var1); break; \
        case -57: var1 = atanhf(var1); break; \
        case -58: var1 = atanhl(var1); break; \
        case -59: var1 = expf(var1); break; \
        case -60: var1 = expl(var1); break; \
        case -61: var1 = exp2(var1); break; \
        case -62: var1 = exp2f(var1); break; \
        case -63: var1 = exp2l(var1); break; \
        case -64: var1 = expm1(var1); break; \
        case -65: var1 = expm1f(var1); break; \
        case -66: var1 = expm1l(var1); break; \
        case -67: var1 = frexpf(var2, (void *)&var1); break; \
        case -68: var1 = frexpl(var2, (void *)&var1); break; \
        case -69: var1 = ilogb(var1); break; \
        case -70: var1 = ilogbf(var1); break; \
        case -71: var1 = ilogbl(var1); break; \
        case -72: var1 = ldexpf(var1, var2); break; \
        case -73: var1 = ldexpl(var1, var2); break; \
        case -74: var1 = logf(var1); break; \
        case -75: var1 = logl(var1); break; \
        case -76: var1 = log10f(var1); break; \
        case -77: var1 = log10l(var1); break; \
        case -78: var1 = log1p(var1); break; \
        case -79: var1 = log1pf(var1); break; \
        case -80: var1 = log1pl(var1); break; \
        case -81: var1 = log2(var1); break; \
        case -82: var1 = log2f(var1); break; \
        case -83: var1 = log2l(var1); break; \
        case -84: var1 = logb(var1); break; \
        case -85: var1 = logbf(var1); break; \
        case -86: var1 = logbl(var1); break; \
        case -87: var1 = modff(var2, (void *)&var1); break; \
        case -88: var1 = modfl(var2, (void *)&var1); break; \
        case -89: var1 = scalbn(var1, var2); break; \
        case -90: var1 = scalbnf(var1, var2); break; \
        case -91: var1 = scalbnl(var1, var2); break; \
        case -92: var1 = scalbln(var1, var2); break; \
        case -93: var1 = scalblnf(var1, var2); break; \
        case -94: var1 = scalblnl(var1, var2); break; \
        case -95: var1 = cbrt(var1); break; \
        case -96: var1 = cbrtf(var1); break; \
        case -97: var1 = cbrtl(var1); break; \
        case -98: var1 = fabsf(var1); break; \
        case -99: var1 = fabsl(var1); break; \
        case -100: var1 = hypot(var1, var2); break; \
        case -101: var1 = hypotf(var1, var2); break; \
        case -102: var1 = hypotl(var1, var2); break; \
        case -103: var1 = powf(var1, var2); break; \
        case -104: var1 = powl(var1, var2); break; \
        case -105: var1 = sqrtf(var1); break; \
        case -106: var1 = sqrtl(var1); break; \
        case -107: var1 = erf(var1); break; \
        case -108: var1 = erff(var1); break; \
        case -109: var1 = erfl(var1); break; \
        case -110: var1 = erfc(var1); break; \
        case -111: var1 = erfcf(var1); break; \
        case -112: var1 = erfcl(var1); break; \
        case -113: var1 = lgamma(var1); break; \
        case -114: var1 = lgammaf(var1); break; \
        case -115: var1 = lgammal(var1); break; \
        case -116: var1 = tgamma(var1); break; \
        case -117: var1 = tgammaf(var1); break; \
        case -118: var1 = tgammal(var1); break; \
        case -119: var1 = ceilf(var1); break; \
        case -120: var1 = ceill(var1); break; \
        case -121: var1 = floorf(var1); break; \
        case -122: var1 = floorl(var1); break; \
        case -123: var1 = nearbyint(var1); break; \
        case -124: var1 = nearbyintf(var1); break; \
        case -125: var1 = nearbyintl(var1); break; \
        case -126: var1 = rint(var1); break; \
        case -127: var1 = rintf(var1); break; \
        case -128: var1 = rintl(var1); break; \
        case -129: var1 = lrint(var1); break; \
        case -130: var1 = lrintf(var1); break; \
        case -131: var1 = lrintl(var1); break; \
        case -132: var1 = llrint(var1); break; \
        case -133: var1 = llrintf(var1); break; \
        case -134: var1 = llrintl(var1); break; \
        case -135: var1 = round(var1); break; \
        case -136: var1 = roundf(var1); break; \
        case -137: var1 = roundl(var1); break; \
        case -138: var1 = lround(var1); break; \
        case -139: var1 = lroundf(var1); break; \
        case -140: var1 = lroundl(var1); break; \
        case -141: var1 = llround(var1); break; \
        case -142: var1 = llroundf(var1); break; \
        case -143: var1 = llroundl(var1); break; \
        case -144: var1 = trunc(var1); break; \
        case -145: var1 = truncf(var1); break; \
        case -146: var1 = truncl(var1); break; \
        case -147: var1 = fmodf(var1, var2); break; \
        case -148: var1 = fmodl(var1, var2); break; \
        case -149: var1 = remainder(var1, var2); break; \
        case -150: var1 = remainderf(var1, var2); break; \
        case -151: var1 = remainderl(var1, var2); break; \
        case -152: var1 = remquo(var1, var2, (void *)&var1); break; \
        case -153: var1 = remquof(var1, var2, (void *)&var1); break; \
        case -154: var1 = remquol(var1, var2, (void *)&var1); break; \
        case -155: var1 = copysign(var1, var2); break; \
        case -156: var1 = copysignf(var1, var2); break; \
        case -157: var1 = copysignl(var1, var2); break; \
        case -158: var1 = nextafter(var1, var2); break; \
        case -159: var1 = nextafterf(var1, var2); break; \
        case -160: var1 = nextafterl(var1, var2); break; \
        case -161: var1 = nexttoward(var1, var2); break; \
        case -162: var1 = nexttowardf(var1, var2); break; \
        case -163: var1 = nexttowardl(var1, var2); break; \
        case -164: var1 = fdim(var1, var2); break; \
        case -165: var1 = fdimf(var1, var2); break; \
        case -166: var1 = fdiml(var1, var2); break; \
        case -167: var1 = fmax(var1, var2); break; \
        case -168: var1 = fmaxf(var1, var2); break; \
        case -169: var1 = fmaxl(var1, var2); break; \
        case -170: var1 = fmin(var1, var2); break; \
        case -171: var1 = fminf(var1, var2); break; \
        case -172: var1 = fminl(var1, var2); break; \
        case -173: var1 = fma(var1, var2, var1); break; \
        case -174: var1 = fmaf(var1, var2, var1); break; \
        case -175: var1 = fmal(var1, var2, var1); break; \
        case -176: var1 = (int)(((u64)var1 * (u64)var2) / (u64)100); break; \
        default: { \
            fprintf(stderr, "\nError: invalid operator \'%c\'\n", (i32)op); \
            myexit(QUICKBMS_ERROR_BMS); \
            break; \
        } \
    } \
    return(var1);

    /* not available on linux
        case -23: var1 = _hypot(var1, var2); break; \
        case -24: var1 = _j0(var1); break; \
        case -25: var1 = _j1(var1); break; \
        case -26: var1 = _jn(var1, var2); break; \
        case -27: var1 = _y0(var1); break; \
        case -28: var1 = _y1(var1); break; \
        case -29: var1 = _yn(var1, var2); break; \
    */

    u_int   var1u,
            var2u;

    if(sign <= 0) { // signed
        DO_MATH_SIGN(var1i, var2i);
    } else {        // unsigned
        var1u = (u_int)var1i;
        var2u = (u_int)var2i;
        DO_MATH_SIGN(var1u, var2u);
    }
    return(-1);
}



int CMD_Math_func(int cmd) {
    int     op,
            var1,
            var2,
            sign;

    var1 = VAR32(0);
    op   = NUM(1);
    var2 = VAR32(2);
    sign = NUM(2);

    var1 = math_operations(var1, op, var2, sign);

    add_var(CMD.var[0], NULL, NULL, var1, sizeof(int));
    return(0);
}



    // this part derives from my calcc tool
    // it needs to be improved in future



int calcc_findop(int c) {
    return(
        (c == '~') || (c == '!') ||
        (c == '<') || (c == '>') ||
        (c == '^') || (c == '&') || (c == '|') ||
        (c == '*') || (c == '/') || (c == '%') ||
        (c == '+') || (c == '-') ||
        (c == '=') ||
        (c == '(') || (c == ')') ||
        (c == '?') ||
        !c);
}

int calcc_get_num(char *p, u64 input) {
    int     num,
            var;
    char    *l;

    if(calcc_findop(*p)) return(0);

    for(l = p; !calcc_findop(*l); l++);

    if(!strnicmp(p, "#INPUT#", l - p)) return input;

    var = get_var_from_name(p, l - p);
    if(var >= 0) {
        num = get_var32(var);
    } else {
        num = readbase(p, 10, NULL);
    }
    return num;
}

int calcc(char *cmd, u64 input) {
    #define calcc_MOV(X,Y)    memmove(X, Y, slen - (Y - buff));

    u_int   left,
            right,
            tot;
    int     op,
            len,
            slen;
    char    //buff[READSZ * 9],   // should be enough
            tmp[INTSZ + 1],
            *data,
            *pl,
            *pr,
            *p,
            *l,
            *special;

    // the following allocation is lame and consumes lot of memory, I will fix it in the next versions... maybe :)
    static int  buffsz  = 0;
    static char *buff   = NULL;
    len = (strlen(cmd) * (2 /*"0x"*/ + (sizeof(int) * 2))) + 1;
    if(len < 0) STD_ERR(QUICKBMS_ERROR_MEMORY);
    if(len > buffsz) {
        buffsz = len;
        buff = realloc(buff, buffsz);
        if(!buff) return 0;
    }
    strcpy(buff, cmd);

        for(p = data = buff; *p; p++) {
            if(*p == '\"') *p = '\'';
            if(*p == '\'') {
                for(l = p; *l; l++) {
                    if(*l == '\"') *l = '\'';
                    *data++ = *l;
                    if((l != p) && (*l == '\'')) break;
                }
                p = l;
                continue;
            }

            *p = tolower(*p);
            if((*p == '\r') || (*p == '\n')) {
                break;
            } else if(*p <= ' ') {
                continue;
            } else if((*p == '[') || (*p == '{')) {
                *data++ = '(';
            } else if((*p == ']') || (*p == '}')) {
                *data++ = ')';
            /*
            } else if((*p == '<') && (p[1] == '<')) {
                continue;
            } else if((*p == '>') && (p[1] == '>')) {
                continue;
            } else if((*p == '&') && (p[1] == '&')) {
                continue;
            } else if((*p == '|') && (p[1] == '|')) {
                continue;
            */
            } else {
                *data++ = *p;
            }
        }
        *data = 0;
        slen = (data - buff) + 1;   // NULL included!!!

        for(;;) {
            data = buff;

            special = NULL;
            if(*data == '\'') {
                special = data;
                for(++data; *data && (*data != '\''); data++);
                if(*data) data++;
            }

            pr = strchr(data, ')');
            if(pr) {
                for(pl = pr; (pl >= data) && (*pl != '('); pl--);
                data = pl + 1;
                if(data == buff) {
                    fprintf(stderr, "- no opened parenthesis found\n");
                    *data = 0;
                    break;
                }

                for(p = data; !calcc_findop(*p); p++);
                if(p == pr) {   // find (num)
                    memmove(pl, pl + 1, pr - (pl + 1));
                    calcc_MOV(pr - 1, pr + 1);
                    slen -= 2;
                    continue;
                }
                *pr = 0;
            } else if((pl = strchr(data, '('))) {
                fprintf(stderr, "- no closed parenthesis found\n");
                *data = 0;
                break;
            }

            #define calcc_BLA(x)  if((p = strchr(data, x))) { op = x;
            //#define calcc_BLAH(x) calcc_BLA(x) }

            // priority based only on the operator
            // I don't consider the left/right position at the moment
            // the user MUST ever use the parenthesis
            calcc_BLA('~')
                calcc_MOV(p + 1, p);
                slen++;
                if(!pr) pr = p;
                pr++;
                *p++ = '0';
            } else calcc_BLA('!')
                calcc_MOV(p + 1, p);
                slen++;
                if(!pr) pr = p;
                pr++;
                *p++ = '0';
            }
            else calcc_BLA('<')
                if(p[1] == '>') { op = 'z'; p[1] = ' '; }   // <>
                if((p[1] == '<') && (p[2] == '<')) { op = 'l'; p[1] = ' '; p[2] = ' '; }    // <<<
            }
            else calcc_BLA('>')
                if((p[1] == '>') && (p[2] == '>')) { op = 'r'; p[1] = ' '; p[2] = ' '; }    // >>>
            }
            else calcc_BLA('&')
                if(p[1] == '&') { op = 'x'; p[1] = ' '; }
            }
            else calcc_BLA('^')
            }
            else calcc_BLA('|')
            }
            else calcc_BLA('*')
                if(p[1] == '*') { op = 'p'; p[1] = ' '; }
            }
            else calcc_BLA('/')
                if(p[1] == '/') { op = 'v'; p[1] = ' '; }
            }
            else calcc_BLA('%')
                if(p[1] == '%') { op = -176; p[1] = ' '; }
            }
            else calcc_BLA('-')
                if(p[1] == '-') {
                    l = p + 2;
                    calcc_findop(*l) ? (p[1] = '1') : (*p = '1');
                }
            }
            else calcc_BLA('+')
                if(p[1] == '+') {
                    l = p + 2;
                    calcc_findop(*l) ? (p[1] = '1') : (*p = '1');
                }
            }
            else calcc_BLA('=')
            }
            else calcc_BLA('?')
                l = p;  // tmp
                op = old_set_math_operator((u8 **)&l);
            }
            else break;

            #undef calcc_BLA
            //#undef calcc_BLAH

            for(l = p - 1; (l >= data) && !calcc_findop(*l); l--);

            if(special) l = special - 1;
            left  = calcc_get_num(++l, input);
            right = calcc_get_num(++p, input);

            tot = math_operations(left, op, right, 0);

            if(pr) *pr = ')';

            while(!calcc_findop(*p)) p++;

            //len = sprintf(tmp, "h%s", calcc_showbase(tot, 16, 0));
            len = sprintf(tmp, "0x%"PRIx, tot);
            if(len != (p - l)) {
                calcc_MOV(l + len, p);
                slen += (l + len) - p;
            }
            memcpy(l, tmp, len);
        }

        if(!*buff) return 0; //continue;

        tot = calcc_get_num(buff, input);

    return tot;
}



int CMD_XMath_func(int cmd) {
    int     ret;

    ret = calcc(STR(1), VAR32(0));

    add_var(CMD.var[0], NULL, NULL, ret, sizeof(int));
    return 0;
}



int CMD_Log_func(int cmd) {
    int     fd,
            offset,
            size;
    u8      *name;

    name    = VAR(0);
    offset  = VAR32(1);
    size    = VAR32(2);
    fd      = FILEZ(5);

    if(dumpa(fd, name, VARNAME(0), offset, size, 0) < 0) return(-1);
    return(0);
}



int CMD_Next_func(int cmd) {
    int     var;

    if(verbose < 0) printf(".\n");  // useful
    if(CMD.var[0] < 0) return(0);   // like } in the C for(;;)
    if(CMD.var[2] < 0) {
        var = VAR32(0) + 1;
    } else {
        var = math_operations(VAR32(0), NUM(1), VAR32(2), NUM(2));
    }
    if(verbose < 0) printf(". %"PRIx"\n", var);  // useful
    add_var(CMD.var[0], NULL, NULL, var, sizeof(int));
    return(0);
}



int CMD_Prev_func(int cmd) {
    int     var;

    if(verbose < 0) printf(".\n");  // useful
    if(CMD.var[0] < 0) return(0);   // like } in the C for(;;)
    var = VAR32(0) - 1;
    if(verbose < 0) printf(". %"PRIx"\n", var);  // useful
    add_var(CMD.var[0], NULL, NULL, var, sizeof(int));
    return(0);
}



int CMD_GetDString_func(int cmd) {
    static int  buffsz  = 0;
    static u8   *buff   = NULL;
    int     fd,
            size,
            idx,
            verbose_offset = 0;
    u8      *var,
            *p;

    fd   = FILEZ(2);

    var = VAR(1);
    p = strrchr(VAR(1), '*');
    if(p && (strlen(p) >= 2)) { // *1 = 2 chars
        idx = get_var_from_name(var, p - var);
        if(idx >= 0) {
            size = get_var32(idx);
        } else {
            size = myatoi(var);
        }
        size *= myatoi(p + 1);
    } else {
        size = VAR32(1);
    }

    if(verbose < 0) verbose_offset = myftell(fd);

    if(size == -1) ALLOC_ERR;
    myalloc(&buff, size + 1, &buffsz);
    myfr(fd, buff, size);
    buff[size] = 0;
    if(verbose < 0) verbose_print(verbose_offset, "getdstr", CMD.var[0], buff, size, 0, size);
    add_var(CMD.var[0], NULL, buff, 0, size);
    return(0);
}



int CMD_ReverseShort_func(int cmd) {
    int     n;

    n = VAR32(0);
    n = swap16(n);
    add_var(CMD.var[0], NULL, NULL, n, sizeof(int));
    return(0);
}



int CMD_ReverseLong_func(int cmd) {
    int     n;

    n = VAR32(0);
    n = swap32(n);
    add_var(CMD.var[0], NULL, NULL, n, sizeof(int));
    return(0);
}



int CMD_ReverseLongLong_func(int cmd) {
    int     n;

    n = VAR32(0);
    n = swap64(n);
    add_var(CMD.var[0], NULL, NULL, n, sizeof(int));
    return(0);
}



int CMD_Set_func(int cmd) {
    static int  buffsz  = 0;
    static u8   *buff   = NULL;
    int     i,
            c,
            len,
            unicnt      = 0,
            varn        = 0,
            varsz       = -1;
    u16     wc          = 0;
    u8      *var        = NULL,
            *p;
    u8      tmp[32];

    if(NUM(1) == TYPE_UNICODE) {    // utf16 to utf8
        p = VAR(2);
        for(i = 0;;) {
            c = *p++;
            // no c check here

            // shared with fgetss
            if(!unicnt) wc = 0;

            if(endian == MYLITTLE_ENDIAN) {
                if(unicnt) wc |= (c << 8);
                else       wc |= c;
            } else {
                if(unicnt) wc |= c;
                else       wc |= (c << 8);
            }
            unicnt++;
            if(unicnt < 2) continue;
            unicnt = 0;

            len = utf16_to_utf8_chr(wc, tmp, sizeof(tmp), 0);

            if((len == 1) && (tmp[0] == 0x00)) break;

            if((i + len) >= buffsz) {
                //if((buffsz + len + STRINGSZ + 1) < buffsz) ALLOC_ERR;
                buffsz += len + STRINGSZ;
                buff = realloc(buff, buffsz + 1);
                if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
            }
            memcpy(buff + i, tmp, len);
            i += len;
        }
        if(!buff) buff = malloc(1);   // useful
        buff[i] = 0;
        var   = buff;
        varsz = -1;

    } else if(NUM(1) == TYPE_BINARY) {
        var   = STR(2);
        varsz = NUM(2);
        i = get_var_from_name(var, varsz);
        if(i >= 0) {
            p = get_var(i);
            mystrdup(&var, p);
            varsz = cstring(var, var, -1, NULL);
        }

    } else if(NUM(1) == TYPE_ALLOC) {
        varsz = VAR32(2);
        if(varsz == -1) ALLOC_ERR;
        var   = malloc(varsz + 1);
        if(!var) STD_ERR(QUICKBMS_ERROR_MEMORY);

    } else if(NUM(1) == TYPE_FILENAME) {
        var   = VAR(2); // this is var2!!!
        p = get_filename(var);
        var = re_strdup(NULL, p, NULL);

    } else if(NUM(1) == TYPE_BASENAME) {
        var   = VAR(2); // this is var2!!!
        p = get_filename(var);
        var = re_strdup(NULL, p, NULL);
        p = strrchr(var, '.');
        if(p) *p = 0;

    } else if(NUM(1) == TYPE_EXTENSION) {
        var   = VAR(2); // this is var2!!!
        p = get_extension(var);
        var = re_strdup(NULL, p, NULL);

    } else if(NUM(1) == TYPE_UNKNOWN) {
        fprintf(stderr, "\n- please insert the content for the variable %s:\n  ", get_varname(CMD.var[0]));
        var = incremental_fread(stdin, &varsz, 1);

    } else if(ISNUMTYPE(NUM(1))) { // number type
        varn  = VAR32(2);
        varsz = VARSZ(2);

    } else {
        var   = VAR(2);
        varsz = VARSZ(2);
    }

    if(CMD.var[0] < 0) {    // MEMORY_FILE
        dumpa_memory_file(&memory_file[-CMD.var[0]], &var, varsz, NULL);
    } else {
        if(var) {
            add_var(CMD.var[0], NULL, var, 0, varsz);
        } else {
            add_var(CMD.var[0], NULL, NULL, varn, sizeof(int));
        }
    }
    return(0);
}



int quick_check_printf_write(u8 *str, u8 *args_format, int args_format_size) {
    int     ret = 0;
    u8      *s;

    if(args_format) memset(args_format, 0, args_format_size);

    // _set_printf_count_output exists only of msvcr80/90
    if(!str) return(0);
    for(s = str; *s; s++) {
        if(*s != '%') continue;
        for(++s; *s; s++) { // don't use tolower or it could get confused with I32/I64
            if(*s == '%') break;
            if(*s == '*') {
                if(args_format && (ret < args_format_size)) args_format[ret] = *s;
                ret++;
                continue;
            }
            if(strchr("cCdiouxXeEfgGaAnpsS", *s)) {
                if(args_format && (ret < args_format_size)) args_format[ret] = *s;
                ret++;
                break;
            }
        }
        if(*s == 'n') return(-1);   // here or some lines below
    }
    return(ret);
}



int quick_check_printf_32bit_nums(u8 *str) {
    int     ret = 0;
    u8      *s;

    // _set_printf_count_output exists only of msvcr80/90
    if(!str) return(0);
    for(s = str; *s; s++) {
        if(*s != '%') continue;
        for(++s; *s; s++) { // don't use tolower or it could get confused with I32/I64
            if(*s == '%') break;
            if(strchr("cCdiouxXeEfgGaAnpsS", *s)) {
                ret++;
                break;
            }
            // automatically skips length specifiers "hljztL"
        }
        if(!strchr("cCdifouxXpn", *s)) return(-1);  // char, float, integer, hex
    }
    return(ret);
}



double printf_int_to_float(int t) {
    float   float_tmp;
    double  double_tmp;
    if(sizeof(t) == sizeof(float)) {
        memcpy(&float_tmp, &t, sizeof(t));
        double_tmp = float_tmp;
    } else {
        memcpy(&double_tmp, &t, sizeof(t));
    }
    return double_tmp;
}



int CMD_String_func(int cmd) {
    static u8   *var1   = NULL;
    int     i,
            op,
            len1,
            len2,
            quote       = 0,
            num         = 0,
            fixed_len   = -1,
            args,
            tmp[MAX_ARGS + 1];
    u8      args_format[5],
            *var2,
            *p,
            *l;

    len1 = VARSZ(0);    // string/binary alternative to re_strdup
    if(len1 == -1) ALLOC_ERR;
    var1 = realloc(var1, len1 + 1);
    if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
    memcpy(var1, VAR(0), len1);
    var1[len1] = 0;
    //re_strdup(&var1, VAR(0), NULL);   // needed for editing
    op   = NUM(1);
    var2 = VAR(2);

    len1 = strlen(var1);
    len2 = strlen(var2);
    if(myisdechex_string(var2)) num = myatoi(var2);
    if(len2) {
        switch(op) {
            case '=': {
                if(num) {
                    for(i = INTSZ - 8; i >= 0; i -= 8) {
                        if(num & ((u_int)0xff << i)) break;
                    }
                    len2 = (i + 8) / 8;
                    var2 = (u8 *)&num;
                }
                if(len2 == -1) ALLOC_ERR;
                var1 = realloc(var1, len2 + 1);
                if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                strncpy(var1, var2, len2);
                var1[len2] = 0;
                break;
            }
            case '+': {
                tmp[0] = len1 + len2;
                if(tmp[0] < len1) ALLOC_ERR;
                if(tmp[0] < len2) ALLOC_ERR;
                if(tmp[0] == -1) ALLOC_ERR;
                var1 = realloc(var1, tmp[0] + 1);
                if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                strcpy(var1 + len1, var2);
                break;
            }
            case '-': { // I know that this is not the same method used in BMS but you can't "substract" a string from the end of another... it means nothing!
                if(num > 0) {
                    if(num <= len1) var1[len1 - num] = 0;
                } else if(num < 0) {
                    num = -num;
                    if(num <= len1) var1[num] = 0;
                } else {
                    while((p = (u8 *)stristr(var1, var2))) {
                        mymemmove(p, p + len2, -1);
                    }
                }
                break;
            }
            case '^': {
                if(len2 > 0) {  // avoid possible division by zero
                    for(i = 0; i < len1; i++) {
                        var1[i] ^= var2[i % len2];
                    }
                }
                break;
            }
            case '<': { // var1="thisisastring" var2="4" = "isastring"
                if(num > 0) {
                    if(num <= len1) {
                        mymemmove(var1, var1 + num, -1);
                    }
                } else {
                    for(p = var1;; p = l + 1) {
                        l = (u8 *)stristr(p, var2);
                        if(!l) break;
                        if(!*l) break;
                    }
                    if(p != var1) p[0] = 0;
                    //printf("\nError: no string variable2 supported in String for operator %c\n", (i32)op);
                    //myexit(QUICKBMS_ERROR_BMS);
                }
                break;
            }
            //case '/': 
            //case '*': 
            case '%': {
                if(num > 0) {
                    var1[len1 % num] = 0;
                } else {
                    fprintf(stderr, "\nError: no string variable2 supported in String for operator %c\n", (i32)op);
                    myexit(QUICKBMS_ERROR_BMS);
                }
                break;
            }
            case '&': { // var1="thisisastring" var2="isa" = "isastring"
                //if(num > 0) {
                    //var1[len1 & num] = 0; // use % for this job, & means nothing
                //} else {
                    p = (u8 *)stristr(var1, var2);
                    if(p) mymemmove(var1, p, -1);
                //}
                break;
            }
            case '|': { // var1="thisisastring" var2="isa" = "string"
                p = (u8 *)stristr(var1, var2);
                if(p) mymemmove(var1, p + len2, -1);
                break;
            }
            case '$': {
                p = (u8 *)strristr(var1, var2);
                if(p) mymemmove(var1, p, -1);
                break;
            }
            case '!': {
                p = (u8 *)strristr(var1, var2);
                if(p) mymemmove(var1, p + len2, -1);
                break;
            }
            case '>': { // var1="thisisastring" var2="isa" = "this" (from end)
                if(num > 0) {
                    if(num <= len1) {
                        var1[len1 - num] = 0;
                    }
                } else {
                    p = (u8 *)strristr(var1, var2);
                    if(p) *p = 0;
                }
                break;
            }
            case 'r': {
                len1 = len2;
                if(len1 == -1) ALLOC_ERR;
                var1 = realloc(var1, len1 + 1);
                if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                for(i = 0; i < len1; i++) {
                    var1[i] = var2[(len1 - i) - 1];
                }
                var1[i] = 0;
                break;
            }
            case 'b':
                len2 = VARSZ(2);    // binary stuff
            case 'B': {
                len1 = (len2 * 2) + 1;  // checked by byte2hex
                if(len1 < len2) ALLOC_ERR;
                if(len1 == -1) ALLOC_ERR;
                var1 = realloc(var1, len1 + 1);
                if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                len1 = byte2hex(var2, len2, var1, len1);
                if(len1 < 0) len1 = 0;
                var1[len1] = 0; // already done by byte2hex
                break;
            }
            case 'h': {
                len1 = len2;
                if(len1) len1 /= 2;
                if(len1 == -1) ALLOC_ERR;
                var1 = realloc(var1, len1 + 1);
                if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                fixed_len = unhex(var2, len2, var1, len1);
                if(fixed_len < 0) fixed_len = 0;
                var1[fixed_len] = 0;
                break;
            }
            case 'e':
                len1 = VARSZ(0);    // binary stuff
                len2 = VARSZ(2);    // binary stuff
            case 'E': {
                if(len1 < len2) {
                    len1 = len2;
                    if(len1 == -1) ALLOC_ERR;
                    var1 = realloc(var1, len1 + 1);
                    if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                    var1[len1] = 0;
                }
                memcpy(var1, var2, len1);
                fixed_len = perform_encryption(var1, len1);
                if(fixed_len < 0) fixed_len = len1;
                var1[fixed_len] = 0;
                break;
            }
            case 'c':
                len1 = VARSZ(0);    // binary stuff
                len2 = VARSZ(2);    // binary stuff
            case 'C': {
                if(len1 < len2) {   // at least equal in length
                    len1 = len2;
                    if(len1 == -1) ALLOC_ERR;
                    var1 = realloc(var1, len1 + 1);
                    if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                    var1[len1] = 0;
                }
                fixed_len = perform_compression(var2, len2, &var1, len1, &len1);
                if(fixed_len < 0) fixed_len = len1;
                var1[fixed_len] = 0;
                break;
            }
            case 'u': {
                if(len1 < len2) {
                    len1 = len2;
                    if(len1 == -1) ALLOC_ERR;
                    var1 = realloc(var1, len1 + 1);
                    if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                }
                for(i = 0; i < len2; i++) {
                    var1[i] = toupper(var2[i]);
                }
                var1[i] = 0;
                break;
            }
            case 'l': {
                if(len1 < len2) {
                    len1 = len2;
                    if(len1 == -1) ALLOC_ERR;
                    var1 = realloc(var1, len1 + 1);
                    if(!var1) STD_ERR(QUICKBMS_ERROR_MEMORY);
                }
                for(i = 0; i < len2; i++) {
                    var1[i] = tolower(var2[i]);
                }
                var1[i] = 0;
                break;
            }
            case 'R': {
                p = find_replace_string(var1, &len1, var2, -1, VAR(3), -1);
                if(p != var1) FREEX(var1,)
                var1 = p;
                break;
            }
            case 'p': { // *printf-like
                len1 = 0;
                FREEX(var1,)    // oh yeah this sux, that's why I classify it as experimental work-around
                if(!var2) var2 = "";
                args = quick_check_printf_write(var2, args_format, sizeof(args_format));
                if(args < 0) var2 = "";

                /*
                // the perfect solution but unfortunately C doesn't allow to pass values of different sizes :(
                #define STRING_PRINTF(N) \
                    ( \
                    strchr("npsS", args_format[N]) ? \
                        VAR(3+N) : \
                        ( \
                        strchr("fFeEgGaA", args_format[N]) ? \
                            printf_int_to_float(VAR32(3+N)) : \
                            VAR32(3+N) \
                        ) \
                    )
                */

                /* old solution, doesn't consider the printf arguments
                #define STRING_PRINTF(N) \
                    (myisdigitstr(VAR(3+N)) ? (void *)VAR32(3+N) : VAR(3+N))
                */

                #define STRING_PRINTF(N) \
                    (strchr("npsS", args_format[N]) ? VAR(3+N) : (void *)VAR32(3+N))

                switch(NUM(0)) {
                    case 1: {
                        if(strchr("fFeEgGaA", args_format[0])) {
                            len1 = spr(&var1, var2,
                                printf_int_to_float(VAR32(3+0)));
                        } else {
                            len1 = spr(&var1, var2,
                                STRING_PRINTF(0));
                        }
                        break;
                    }
                    case 2: {
                        len1 = spr(&var1, var2,
                            STRING_PRINTF(0),
                            STRING_PRINTF(1));
                        break;
                    }
                    case 3: {
                        len1 = spr(&var1, var2,
                            STRING_PRINTF(0),
                            STRING_PRINTF(1),
                            STRING_PRINTF(2));
                        break;
                    }
                    case 4: {
                        len1 = spr(&var1, var2,
                            STRING_PRINTF(0),
                            STRING_PRINTF(1),
                            STRING_PRINTF(2),
                            STRING_PRINTF(3));
                        break;
                    }
                    case 5: {
                        len1 = spr(&var1, var2,
                            STRING_PRINTF(0),
                            STRING_PRINTF(1),
                            STRING_PRINTF(2),
                            STRING_PRINTF(3),
                            STRING_PRINTF(4));
                        break;
                    }
                    default: {
                        len1 = spr(&var1, "%s", var2);
                        break;
                    }
                }
                break;
            }
            case 's': { // *scanf-like, experimental and potentially dangerous
                args = quick_check_printf_32bit_nums(var2); // only 32bit numbers are allowed at the moment!
                if((args >= 0) && var1 && var2) {
                    if(args > NUM(0)) args = NUM(0);
                    memset(&tmp, 0, sizeof(tmp));
                    sscanf(var1, var2,
                        &tmp[0], &tmp[1], &tmp[2], &tmp[3],
                        &tmp[4], &tmp[5], &tmp[6], &tmp[7],
                        &tmp[8], &tmp[9]);
                    for(i = 0; i < args; i++) {
                        add_var(CMD.var[3 + i], NULL, NULL, tmp[i], sizeof(int));
                    }
                }
                break;
            }
            case 'S': {
                p = var2;
                for(i = 0; CMD.var[2 + i] >= 0; i++) {
                    if(!*p) break;
                    quote = 0;
                    while(strchr(" \t\r\n,|", *p)) p++;
                    if(!*p) break;
                    if((*p == '\"') || (*p == '\'')) quote = *p++;
                    for(l = p; *l; l++) {
                        if(strchr(" \t\r\n,|", *l)) {
                            if(quote) continue;
                            break;
                        }
                        if(*l == '\\') {
                            if(quote && (l[1] == quote)) {
                                l++;
                                continue;
                            }
                        }
                        if(quote && (*l == quote)) {
                            break;
                        }
                    }
                    add_var(CMD.var[2 + i], NULL, p, 0, l - p);
                    if(quote && *l) l++;
                    p = l;
                }
                var1 = myitoa(i);   // number of elements
                break;
            }
            case 'x': {
                mystrdup(&var1, var2);
                len1 = cstring(var1, var1, -1, NULL);
                break;
            }
            default: {
                fprintf(stderr, "\nError: invalid operator %c\n", (i32)op);
                myexit(QUICKBMS_ERROR_BMS);
                break;
            }
        }
        add_var(CMD.var[0], NULL, var1, 0, fixed_len);
    }
    return(0);
}



int CMD_ImpType_func(int cmd) {
    if(verbose > 0) printf("- ImpType command %d ignored (not supported)\n", (i32)cmd);
    return(0);
}



int CMD_Open_func(int cmd) {
    static u8   current_dir[PATHSZ + 1]; // used only here so don't waste the stack
    static u8   *fname  = NULL;
    filenumber_t    *filez;
    int     fdnum,
            special = 0;
    u8      *fdir,
            *p,
            *msg;

    fdir    = VAR(0);
    re_strdup(&fname, VAR(1), NULL);   // needed for modifying it
    fdnum   = NUM(2);

    getcwd(current_dir, PATHSZ);

    filez = &filenumber[0];     // everything is ever referred to the main file

    if(fname[0] == '?') {
        fname = realloc(fname, PATHSZ + 1);
        if(!fname) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(!stricmp(fdir, "FDDE")) {
            msg = "you must choose the extension of the other file to load";
        } else if(!stricmp(fdir, "FDSE")) {
            msg = "you must choose the name of the other file to load in the same folder";
        } else {
            msg = "you must choose the name of the other file to load";
        }
#ifdef WIN32
        if(g_is_gui && stricmp(fdir, "FDDE")) { // FDDE must be specified manually in the console
            p = get_file(msg, 0, 0);
            strcpy(fname, p);
            free(p);
        } else
#endif
        fgetz(fname, PATHSZ, stdin,
            "\n- %s:\n  ", msg);
    }

    if(!stricmp(fdir, "FDDE")) {    // fname is the new extension
        special = 1;
        fdir = file_folder;
        p = strrchr(filez->fullname, '.');
        if(p) p++;
        else  p = filez->fullname + strlen(filez->fullname);
        // g_is_gui doesn't matter here because the extension must be inserted manually
        fname = realloc(fname, (p - filez->fullname) + strlen(fname) + 1);
        if(!fname) STD_ERR(QUICKBMS_ERROR_MEMORY);
        mymemmove(fname + (p - filez->fullname), fname, -1);
        memcpy(fname, filez->fullname, p - filez->fullname);
    } else if(!stricmp(fdir, "FDSE")) { // I don't know if this is correct because it's not documented!
        special = 1;
        fdir = file_folder;
        p = get_filename(filez->fullname);
        if(g_is_gui && mystrchrs(fname, "\\/")) {
            // usually fname is already a full path but not ever
            fdir = NULL;
        } else {
            fname = realloc(fname, (p - filez->fullname) + strlen(fname) + 1);
            if(!fname) STD_ERR(QUICKBMS_ERROR_MEMORY);
            mymemmove(fname + (p - filez->fullname), fname, -1);
            memcpy(fname, filez->fullname, p - filez->fullname);
        }
    } else {
        if(g_is_gui && mystrchrs(fname, "\\/")) {
            fdir = NULL;
        }
        special = 0;
    }

    if(fdir && fdir[0]) {
        if(!special && (strchr(fdir, ':') || (fdir[0] == '\\') || (fdir[0] == '/') || strstr(fdir, ".."))) {
            //strcpy(fdir, ".");
        } else {
            fprintf(stderr, "- enter in folder %s\n", fdir);
            if(chdir(fdir) < 0) STD_ERR(QUICKBMS_ERROR_FOLDER);
        }
    }

    if(CMD.var[3]) {    // check mode
        if(myfopen(fname, fdnum, 0) < 0) {
            add_var(CMD.var[3], NULL, NULL, 0, sizeof(int));    // doesn't exist
        } else {
            add_var(CMD.var[3], NULL, NULL, 1, sizeof(int));    // exists
        }
    } else {
        myfopen(fname, fdnum, 1);
    }

    chdir(current_dir); // return to the output folder
    return(0);
}



int CMD_GetCT_func(int cmd) {
    int     fd,
            verbose_offset = 0;
    u8      *tmp;

    fd = FILEZ(3);

    if(verbose < 0) verbose_offset = myftell(fd);

    //if(NUM(1) < 0) {
        // ok
    //} else {
        //printf("\nError: GetCT is supported only with String type\n");
        //myexit(QUICKBMS_ERROR_BMS);
    //}
    tmp = fgetss(fd, VAR32(2), (NUM(1) == TYPE_UNICODE) ? 1 : 0, 0);
    if(!tmp) return(-1);    // compability with old quickbms
    if(verbose < 0) verbose_print(verbose_offset, "getct", CMD.var[0], tmp, -1, 0, VAR32(2));
    add_var(CMD.var[0], NULL, tmp, 0, -1); // fgetss is handled as a string function at the moment
    return(0);
}



int CMD_ComType_func(int cmd) {
    u8      tmp_str[32],
            *str;

    str = STR(0);
    str = skip_delimit(str);
    FREEZ(comtype_dictionary)
    comtype_dictionary     = STR(1);
    comtype_dictionary_len = NUM(1);
    if((CMD.var[1] >= 0) && !comtype_dictionary) {
        comtype_dictionary     = VAR(1);
        comtype_dictionary_len = VAR32(2);
    }
    if(comtype_dictionary_len <= 0) comtype_dictionary = NULL;
    comtype_scan           = 0;

    // quick_var_from_name_check can be used only for
    // instructions used in the same function or for
    // data copied in other buffers because it's a
    // temporary content!
    quick_var_from_name_check(&comtype_dictionary, &comtype_dictionary_len);
    if(comtype_dictionary && (comtype_dictionary_len > 0)) {
        comtype_dictionary = malloc_copy(NULL, comtype_dictionary, comtype_dictionary_len + 1);
    }

    if(!stricmp(str, "?")) {
        fgetz(tmp_str, sizeof(tmp_str), stdin,
            "\n- you must specify the compression algorithm to use:\n  ");
        str = tmp_str;
    }

    if(!strnicmp(str, "COMP_", 5)) str += 5;

    if(!stricmp(str, "copy") || !stricmp(str, "null") || !str[0]) compression_type = COMP_COPY;
    QUICK_COMP_ASSIGN2(ZLIB, ZLIB1)
    QUICK_COMP_ASSIGN2(DEFLATE, INFLATE)
    QUICK_COMP_ASSIGN(LZO1)
    QUICK_COMP_ASSIGN(LZO1A)
    QUICK_COMP_ASSIGN(LZO1B)
    QUICK_COMP_ASSIGN(LZO1C)
    QUICK_COMP_ASSIGN(LZO1F)
    QUICK_COMP_ASSIGN3(LZO1X, LZO, MINILZO)
    QUICK_COMP_ASSIGN(LZO1Y)
    QUICK_COMP_ASSIGN(LZO1Z)
    QUICK_COMP_ASSIGN(LZO2A)
    QUICK_COMP_ASSIGN(LZSS)
    QUICK_COMP_ASSIGN(LZX)
    QUICK_COMP_ASSIGN(GZIP)
    else if(!stricmp(str, "pkware") || !stricmp(str, "blast") || !stricmp(str, "explode") || !stricmp(str, "implode")) compression_type = COMP_EXPLODE;
    QUICK_COMP_ASSIGN(LZMA)                             // 5 bytes + lzma
    QUICK_COMP_ASSIGN2(LZMA_86HEAD, LZMA86HEAD)         // 5 bytes + 8 bytes (size) + lzma
    QUICK_COMP_ASSIGN2(LZMA_86DEC, LZMA86DEC)           // 1 byte + 5 bytes + lzma
    QUICK_COMP_ASSIGN2(LZMA_86DECHEAD, LZMA86DECHEAD)   // 1 byte + 5 bytes + 8 bytes (size) + lzma
    QUICK_COMP_ASSIGN2(LZMA_EFS, LZMAEFS)               // 2 + 2 + x + lzma
    QUICK_COMP_ASSIGN(BZIP2)
    else if(!stricmp(str, "XMemDecompress") || !stricmp(str, "XMEMCODEC_DEFAULT") || !stricmp(str, "XMEMCODEC_LZX") || !stricmp(str, "xcompress")) compression_type = COMP_XMEMLZX;
    QUICK_COMP_ASSIGN2(HEX, HEX2BYTE)
    QUICK_COMP_ASSIGN(BASE64)
    QUICK_COMP_ASSIGN2(UUENCODE, UUDECODE)
    QUICK_COMP_ASSIGN2(XXENCODE, XXDECODE)
    QUICK_COMP_ASSIGN(ASCII85)
    QUICK_COMP_ASSIGN(YENC)
    QUICK_COMP_ASSIGN2(UNLZW, COM_LZW_Decompress)
    QUICK_COMP_ASSIGN2(UNLZWX, milestone_lzw)
    //else if(!stricmp(str, "cab") || !stricmp(str, "mscab") || !stricmp(str, "mscf")) compression_type = COMP_CAB;
    //else if(!stricmp(str, "chm") || !stricmp(str, "mschm") || !stricmp(str, "itsf")) compression_type = COMP_CHM;
    //else if(!stricmp(str, "szdd")) compression_type = COMP_SZDD;
    QUICK_COMP_ASSIGN2(LZXCAB, MSLZX)
    QUICK_COMP_ASSIGN(LZXCHM)
    QUICK_COMP_ASSIGN(RLEW)
    QUICK_COMP_ASSIGN(LZJB)
    QUICK_COMP_ASSIGN(SFL_BLOCK)
    QUICK_COMP_ASSIGN(SFL_RLE)
    QUICK_COMP_ASSIGN(SFL_NULLS)
    QUICK_COMP_ASSIGN(SFL_BITS)
    QUICK_COMP_ASSIGN(LZMA2)            // 1 bytes + lzma2
    QUICK_COMP_ASSIGN(LZMA2_86HEAD)     // 1 bytes + 8 bytes (size) + lzma2
    QUICK_COMP_ASSIGN(LZMA2_86DEC)      // 1 byte + 1 bytes + lzma2
    QUICK_COMP_ASSIGN(LZMA2_86DECHEAD)  // 1 byte + 1 bytes + 8 bytes (size) + lzma2
    QUICK_COMP_ASSIGN(NRV2b)
    QUICK_COMP_ASSIGN(NRV2d)
    QUICK_COMP_ASSIGN(NRV2e)
    QUICK_COMP_ASSIGN(HUFFBOH)
    QUICK_COMP_ASSIGN3(UNCOMPRESS, COMPRESS, LZW)
    QUICK_COMP_ASSIGN(DMC)
    QUICK_COMP_ASSIGN3(LZH, LZHUF, LHA)
    QUICK_COMP_ASSIGN(LZARI)
    QUICK_COMP_ASSIGN(RLE)
    QUICK_COMP_ASSIGN(RLEA)
    QUICK_COMP_ASSIGN(BPE)
    QUICK_COMP_ASSIGN(QUICKLZ)
    QUICK_COMP_ASSIGN(Q3HUFF)
    QUICK_COMP_ASSIGN(UNMENG)
    QUICK_COMP_ASSIGN(LZ2K)
    QUICK_COMP_ASSIGN(DARKSECTOR)
    QUICK_COMP_ASSIGN(MSZH)
    QUICK_COMP_ASSIGN(UN49G)
    QUICK_COMP_ASSIGN(UNTHANDOR)
    QUICK_COMP_ASSIGN(DOOMHUFF)
    QUICK_COMP_ASSIGN(APLIB)
    QUICK_COMP_ASSIGN(TZAR_LZSS)
    QUICK_COMP_ASSIGN2(LZF, FASTLZ)     // "It is possible to use FastLZ as a drop-in replacement for Marc Lehmann's LibLZF."
    QUICK_COMP_ASSIGN(CLZ77)
    QUICK_COMP_ASSIGN(LZRW1)
    QUICK_COMP_ASSIGN(DHUFF)
    QUICK_COMP_ASSIGN(FIN)
    QUICK_COMP_ASSIGN(LZAH)
    QUICK_COMP_ASSIGN(LZH12)            // -lh4-
    QUICK_COMP_ASSIGN(LZH13)            // -lh5-
    QUICK_COMP_ASSIGN(GRZIP)
    QUICK_COMP_ASSIGN(CKRLE)
    QUICK_COMP_ASSIGN(QUAD)
    QUICK_COMP_ASSIGN(BALZ)
    QUICK_COMP_ASSIGN2(DEFLATE64, INFLATE64)
    QUICK_COMP_ASSIGN(SHRINK)
    else if(!stricmp(str, "ppmd") || !stricmp(str, "ppmd8") || !strnicmp(str, "ppmdi", 5)) compression_type = COMP_PPMDI;
    //QUICK_COMP_ASSIGN(PPMDRAW)
    else if(!stricmp(str, "z-base-32")) {       compression_type = COMP_MULTIBASE; comtype_dictionary_len = 32 | (1 << 8); }
    else if(!stricmp(str, "base32hex")) {       compression_type = COMP_MULTIBASE; comtype_dictionary_len = 32 | (2 << 8); }
    else if(!stricmp(str, "base32crockford")) { compression_type = COMP_MULTIBASE; comtype_dictionary_len = 32 | (3 << 8); }
    else if(!stricmp(str, "base32nintendo")) {  compression_type = COMP_MULTIBASE; comtype_dictionary_len = 32 | (4 << 8); }
    else if(!strnicmp(str, "base", 4)) {        compression_type = COMP_MULTIBASE; comtype_dictionary_len = myatoi(str + 4);
                                                if(comtype_dictionary_len <= 0) comtype_dictionary_len = 64; }
    QUICK_COMP_ASSIGN(BRIEFLZ)
    QUICK_COMP_ASSIGN(PAQ6)
    QUICK_COMP_ASSIGN(SHCODEC)
    QUICK_COMP_ASSIGN2(HSTEST1, hstest_hs_unpack)
    QUICK_COMP_ASSIGN2(HSTEST2, hstest_unpackc)
    QUICK_COMP_ASSIGN(SIXPACK)
    QUICK_COMP_ASSIGN(ASHFORD)
    QUICK_COMP_ASSIGN(JCALG)
    QUICK_COMP_ASSIGN(JAM)
    QUICK_COMP_ASSIGN(LZHLIB)
    QUICK_COMP_ASSIGN(SRANK)
    QUICK_COMP_ASSIGN(ZZIP)
    QUICK_COMP_ASSIGN(SCPACK)
    QUICK_COMP_ASSIGN(RLE3)
    QUICK_COMP_ASSIGN(BPE2)
    QUICK_COMP_ASSIGN(BCL_HUF)
    QUICK_COMP_ASSIGN(BCL_LZ)
    QUICK_COMP_ASSIGN(BCL_RICE)
    QUICK_COMP_ASSIGN(BCL_RLE)
    QUICK_COMP_ASSIGN(BCL_SF)
    QUICK_COMP_ASSIGN(SCZ)
    QUICK_COMP_ASSIGN(SZIP)
    QUICK_COMP_ASSIGN2(PPMDI_RAW, ppmdi1_raw)
    QUICK_COMP_ASSIGN(PPMDG)
    QUICK_COMP_ASSIGN(PPMDG_RAW)
    QUICK_COMP_ASSIGN(PPMDJ)
    QUICK_COMP_ASSIGN(PPMDJ_RAW)
    QUICK_COMP_ASSIGN(SR3C)
    QUICK_COMP_ASSIGN2(HUFFMANLIB, HUFFMAN)
    QUICK_COMP_ASSIGN(SFASTPACKER)
    QUICK_COMP_ASSIGN(SFASTPACKER2)
    QUICK_COMP_ASSIGN2(DK2, EA)
    QUICK_COMP_ASSIGN(LZ77WII)
    QUICK_COMP_ASSIGN2(LZ77WII_RAW10, lz77wii_raw)
    QUICK_COMP_ASSIGN(DARKSTONE)
    QUICK_COMP_ASSIGN(SFL_BLOCK_CHUNKED)
    QUICK_COMP_ASSIGN(YUKE_BPE)
    QUICK_COMP_ASSIGN(STALKER_LZA)
    QUICK_COMP_ASSIGN(PRS_8ING)
    QUICK_COMP_ASSIGN(PUYO_CNX)
    QUICK_COMP_ASSIGN(PUYO_CXLZ)
    QUICK_COMP_ASSIGN(PUYO_LZ00)
    QUICK_COMP_ASSIGN(PUYO_LZ01)
    QUICK_COMP_ASSIGN(PUYO_LZSS)
    QUICK_COMP_ASSIGN(PUYO_ONZ)
    QUICK_COMP_ASSIGN(PUYO_PRS)
    //QUICK_COMP_ASSIGN(PUYO_PVZ)
    QUICK_COMP_ASSIGN(FALCOM)
    QUICK_COMP_ASSIGN(CPK)
    QUICK_COMP_ASSIGN(BZIP2_FILE)
    QUICK_COMP_ASSIGN(LZ77WII_RAW11)
    QUICK_COMP_ASSIGN(LZ77WII_RAW30)
    QUICK_COMP_ASSIGN(LZ77WII_RAW20)
    QUICK_COMP_ASSIGN(PGLZ)
    QUICK_COMP_ASSIGN2(SLZ, UnPackSLZ)
    QUICK_COMP_ASSIGN(SLZ_01)
    QUICK_COMP_ASSIGN(SLZ_02)
    QUICK_COMP_ASSIGN(LZHL)
    QUICK_COMP_ASSIGN(D3101)
    QUICK_COMP_ASSIGN(SQUEEZE)
    QUICK_COMP_ASSIGN(LZRW3)
    QUICK_COMP_ASSIGN(TDCB_ahuff)
    QUICK_COMP_ASSIGN(TDCB_arith)
    QUICK_COMP_ASSIGN(TDCB_arith1)
    QUICK_COMP_ASSIGN(TDCB_arith1e)
    QUICK_COMP_ASSIGN(TDCB_arithn)
    QUICK_COMP_ASSIGN(TDCB_compand)
    QUICK_COMP_ASSIGN(TDCB_huff)
    QUICK_COMP_ASSIGN(TDCB_lzss)
    QUICK_COMP_ASSIGN(TDCB_lzw12)
    QUICK_COMP_ASSIGN(TDCB_lzw15v)
    QUICK_COMP_ASSIGN(TDCB_silence)
    QUICK_COMP_ASSIGN(RDC)
    QUICK_COMP_ASSIGN(ILZR)
    QUICK_COMP_ASSIGN(DMC2)
    QUICK_COMP_ASSIGN(diffcomp)
    QUICK_COMP_ASSIGN(LZR)
    QUICK_COMP_ASSIGN2(LZS, MPPC)
    QUICK_COMP_ASSIGN2(LZS_BIG, MPPC_BIG)
    QUICK_COMP_ASSIGN(MOHLZSS)
    QUICK_COMP_ASSIGN(MOHRLE)
    QUICK_COMP_ASSIGN2(YAZ0, SZS)
    QUICK_COMP_ASSIGN(BYTE2HEX)
    QUICK_COMP_ASSIGN(UN434A)
    QUICK_COMP_ASSIGN2(GZPACK, PACK)
    else if(stristr(str, "zip_dynamic") || stristr(str, "zlib_dynamic")) compression_type = COMP_UNZIP_DYNAMIC;
    QUICK_COMP_ASSIGN(ZLIB_NOERROR)
    QUICK_COMP_ASSIGN(DEFLATE_NOERROR)
    QUICK_COMP_ASSIGN2(PPMDH, PPMD7)
    QUICK_COMP_ASSIGN(PPMDH_RAW)
    QUICK_COMP_ASSIGN(RNC)
    QUICK_COMP_ASSIGN(RNC_RAW)
    QUICK_COMP_ASSIGN2(FITD, PAK_explode)
    QUICK_COMP_ASSIGN(KENS_Nemesis)
    QUICK_COMP_ASSIGN(KENS_Kosinski)
    QUICK_COMP_ASSIGN(KENS_Kosinski_moduled)
    QUICK_COMP_ASSIGN(KENS_Enigma)
    QUICK_COMP_ASSIGN(KENS_Saxman)
    QUICK_COMP_ASSIGN(DRAGONBALLZ)
    QUICK_COMP_ASSIGN2(NITROSDK, NITRO)
    QUICK_COMP_ASSIGN(ZDAEMON)
    QUICK_COMP_ASSIGN(SKULLTAG)
    QUICK_COMP_ASSIGN(MSF)
    QUICK_COMP_ASSIGN(STARGUNNER)
    QUICK_COMP_ASSIGN(NTCOMPRESS)
    QUICK_COMP_ASSIGN(CRLE)
    QUICK_COMP_ASSIGN(CTW)
    QUICK_COMP_ASSIGN(DACT_DELTA)
    QUICK_COMP_ASSIGN(DACT_MZLIB2)
    QUICK_COMP_ASSIGN(DACT_MZLIB)
    QUICK_COMP_ASSIGN(DACT_RLE)
    QUICK_COMP_ASSIGN(DACT_SNIBBLE)
    QUICK_COMP_ASSIGN(DACT_TEXT)
    QUICK_COMP_ASSIGN(DACT_TEXTRLE)
    QUICK_COMP_ASSIGN(EXECUTE)
    QUICK_COMP_ASSIGN(LZ77_0)
    QUICK_COMP_ASSIGN(LZBSS)
    QUICK_COMP_ASSIGN(BPAQ0)
    QUICK_COMP_ASSIGN2(LZPX, LZPXJ)
    QUICK_COMP_ASSIGN(MAR_RLE)
    QUICK_COMP_ASSIGN(GDCM_RLE)
    QUICK_COMP_ASSIGN(LZMAT)
    QUICK_COMP_ASSIGN(DICT)
    QUICK_COMP_ASSIGN(REP)
    QUICK_COMP_ASSIGN(LZP)
    QUICK_COMP_ASSIGN(ELIAS_DELTA)
    QUICK_COMP_ASSIGN(ELIAS_GAMMA)
    QUICK_COMP_ASSIGN(ELIAS_OMEGA)
    QUICK_COMP_ASSIGN(PACKBITS)
    QUICK_COMP_ASSIGN(DARKSECTOR_NOCHUNKS)
    QUICK_COMP_ASSIGN(ENET)
    QUICK_COMP_ASSIGN(EDUKE32)
    QUICK_COMP_ASSIGN(XU4_RLE)
    QUICK_COMP_ASSIGN(RVL)
    QUICK_COMP_ASSIGN(LZFU)
    QUICK_COMP_ASSIGN(LZFU_RAW)
    QUICK_COMP_ASSIGN(XU4_LZW)
    QUICK_COMP_ASSIGN(HE3)
    QUICK_COMP_ASSIGN(IRIS)
    QUICK_COMP_ASSIGN(IRIS_HUFFMAN)
    QUICK_COMP_ASSIGN(IRIS_UO_HUFFMAN)
    QUICK_COMP_ASSIGN(NTFS)
    QUICK_COMP_ASSIGN(PDB)
    QUICK_COMP_ASSIGN(COMPRLIB_SPREAD)
    QUICK_COMP_ASSIGN(COMPRLIB_RLE1)
    QUICK_COMP_ASSIGN(COMPRLIB_RLE2)
    QUICK_COMP_ASSIGN(COMPRLIB_RLE3)
    QUICK_COMP_ASSIGN(COMPRLIB_RLE4)
    QUICK_COMP_ASSIGN(COMPRLIB_ARITH)
    QUICK_COMP_ASSIGN(COMPRLIB_SPLAY)
    QUICK_COMP_ASSIGN(CABEXTRACT)
    QUICK_COMP_ASSIGN(MRCI)
    QUICK_COMP_ASSIGN(HD2_01)
    QUICK_COMP_ASSIGN(HD2_08)
    QUICK_COMP_ASSIGN(HD2_01raw)
    QUICK_COMP_ASSIGN(RTL_LZNT1)
    QUICK_COMP_ASSIGN(RTL_XPRESS)
    QUICK_COMP_ASSIGN(RTL_XPRESS_HUFF)
    QUICK_COMP_ASSIGN(PRS)
    QUICK_COMP_ASSIGN(SEGA_LZ77)
    QUICK_COMP_ASSIGN(SAINT_SEYA)
    QUICK_COMP_ASSIGN2(NTCOMPRESS30, lz77wii_raw30)
    QUICK_COMP_ASSIGN2(NTCOMPRESS40, lz77wii_raw40)
    QUICK_COMP_ASSIGN(SLZ_03)
    QUICK_COMP_ASSIGN(YAKUZA)
    QUICK_COMP_ASSIGN(LZ4)
    QUICK_COMP_ASSIGN(SNAPPY)
    QUICK_COMP_ASSIGN(LUNAR_LZ1)
    QUICK_COMP_ASSIGN(LUNAR_LZ2)
    QUICK_COMP_ASSIGN(LUNAR_LZ3)
    QUICK_COMP_ASSIGN(LUNAR_LZ4)
    QUICK_COMP_ASSIGN(LUNAR_LZ5)
    QUICK_COMP_ASSIGN(LUNAR_LZ6)
    QUICK_COMP_ASSIGN(LUNAR_LZ7)
    QUICK_COMP_ASSIGN(LUNAR_LZ8)
    QUICK_COMP_ASSIGN(LUNAR_LZ9)
    QUICK_COMP_ASSIGN(LUNAR_LZ10)
    QUICK_COMP_ASSIGN(LUNAR_LZ11)
    QUICK_COMP_ASSIGN(LUNAR_LZ12)
    QUICK_COMP_ASSIGN(LUNAR_LZ13)
    QUICK_COMP_ASSIGN(LUNAR_LZ14)
    QUICK_COMP_ASSIGN(LUNAR_LZ15)
    QUICK_COMP_ASSIGN(LUNAR_LZ16)
    QUICK_COMP_ASSIGN(LUNAR_RLE1)
    QUICK_COMP_ASSIGN(LUNAR_RLE2)
    QUICK_COMP_ASSIGN(LUNAR_RLE3)
    QUICK_COMP_ASSIGN(LUNAR_RLE4)
    QUICK_COMP_ASSIGN(GOLDENSUN)
    QUICK_COMP_ASSIGN(LUMINOUSARC)
    QUICK_COMP_ASSIGN(LZV1)
    QUICK_COMP_ASSIGN(FASTLZAH)
    QUICK_COMP_ASSIGN(ZAX)
    QUICK_COMP_ASSIGN(SHRINKER)
    QUICK_COMP_ASSIGN(MMINI_HUFFMAN)
    QUICK_COMP_ASSIGN(MMINI_LZ1)
    QUICK_COMP_ASSIGN(MMINI)
    QUICK_COMP_ASSIGN(CLZW)
    QUICK_COMP_ASSIGN(LZHAM)
    QUICK_COMP_ASSIGN(LPAQ8)
    QUICK_COMP_ASSIGN(SEGA_LZS2)
    QUICK_COMP_ASSIGN(CALLDLL)
    QUICK_COMP_ASSIGN(WOLF)
    QUICK_COMP_ASSIGN(COREONLINE)
    QUICK_COMP_ASSIGN(MSZIP)
    QUICK_COMP_ASSIGN(QTM)
    QUICK_COMP_ASSIGN(MSLZSS)
    QUICK_COMP_ASSIGN(MSLZSS1)
    QUICK_COMP_ASSIGN(MSLZSS2)
    QUICK_COMP_ASSIGN2(KWAJ, MSLZH)
    QUICK_COMP_ASSIGN(LZLIB)
    QUICK_COMP_ASSIGN(DFLT)
    QUICK_COMP_ASSIGN(LZMA_DYNAMIC)
    QUICK_COMP_ASSIGN(LZMA2_DYNAMIC)
    QUICK_COMP_ASSIGN(LZMA2_EFS)
    QUICK_COMP_ASSIGN2(LZXCAB_DELTA, mslzx_delta)
    QUICK_COMP_ASSIGN(LZXCHM_DELTA)
    QUICK_COMP_ASSIGN(FFCE)
    QUICK_COMP_ASSIGN2(TONY, SCUMMVM1)  //QUICK_COMP_ASSIGN2(SCUMMVM1, TONY)
    QUICK_COMP_ASSIGN2(RLE7, SCUMMVM2)  //QUICK_COMP_ASSIGN2(SCUMMVM2, RLE7)
    QUICK_COMP_ASSIGN2(RLE0, SCUMMVM3)  //QUICK_COMP_ASSIGN2(SCUMMVM3, RLE0)
    QUICK_COMP_ASSIGN2(SCUMMVM4, decompressHIF)
    QUICK_COMP_ASSIGN2(SCUMMVM5, decompressSPCN)
    QUICK_COMP_ASSIGN3(SCUMMVM6, UNPACKM1, RNC1)
    QUICK_COMP_ASSIGN3(SCUMMVM7, UNPACKM2, RNC2)
    QUICK_COMP_ASSIGN2(SCUMMVM8, CineUnpacker)
    QUICK_COMP_ASSIGN2(SCUMMVM9, delphineUnpack)
    QUICK_COMP_ASSIGN(SCUMMVM10)
    QUICK_COMP_ASSIGN(SCUMMVM11)
    QUICK_COMP_ASSIGN2(SCUMMVM12, unpackLZW)
    QUICK_COMP_ASSIGN2(SCUMMVM13, unpackLZW1)
    QUICK_COMP_ASSIGN2(SCUMMVM14, reorderPic)
    QUICK_COMP_ASSIGN2(SCUMMVM15, reorderView)
    QUICK_COMP_ASSIGN(SCUMMVM16)
    QUICK_COMP_ASSIGN2(SCUMMVM17, decodeSRLE)
    QUICK_COMP_ASSIGN(SCUMMVM18)
    QUICK_COMP_ASSIGN2(SCUMMVM19, PS2ICON)
    QUICK_COMP_ASSIGN(SCUMMVM20)
    QUICK_COMP_ASSIGN2(SCUMMVM21, uncompressPlane)
    QUICK_COMP_ASSIGN2(SCUMMVM22, unbarchive)
    QUICK_COMP_ASSIGN2(SCUMMVM23, decompressRLE256)
    QUICK_COMP_ASSIGN2(SCUMMVM24, decompressHIF_other)
    QUICK_COMP_ASSIGN(SCUMMVM25)
    QUICK_COMP_ASSIGN2(SCUMMVM26, SoundTownsPC98)
    QUICK_COMP_ASSIGN2(SCUMMVM27, decodeSND1)
    QUICK_COMP_ASSIGN(SCUMMVM28)
    QUICK_COMP_ASSIGN2(SCUMMVM29, decomp3)
    QUICK_COMP_ASSIGN2(SCUMMVM30, decomp4)
    QUICK_COMP_ASSIGN2(SCUMMVM31, decomp5)
    QUICK_COMP_ASSIGN2(SCUMMVM32, decomp7)
    QUICK_COMP_ASSIGN2(SCUMMVM33, decompFF)
    QUICK_COMP_ASSIGN(SCUMMVM34)
    QUICK_COMP_ASSIGN2(SCUMMVM35, DIMUSE)
    QUICK_COMP_ASSIGN3(SCUMMVM36, ADPCM, ADPCM1)
    QUICK_COMP_ASSIGN2(SCUMMVM37, ADPCM2)
    QUICK_COMP_ASSIGN2(SCUMMVM38, unpackRiven)
    QUICK_COMP_ASSIGN2(SCUMMVM39, drawRLE8)
    QUICK_COMP_ASSIGN(SCUMMVM40)
    QUICK_COMP_ASSIGN(SCUMMVM41)
    QUICK_COMP_ASSIGN2(SCUMMVM42, MusicPlayerMac)
    QUICK_COMP_ASSIGN2(SCUMMVM43, STUFFIT)
    QUICK_COMP_ASSIGN2(SCUMMVM44, ICONPLANAR)
    QUICK_COMP_ASSIGN2(SCUMMVM45, DRASCULA)
    QUICK_COMP_ASSIGN2(SCUMMVM46, bompDecodeLine)
    QUICK_COMP_ASSIGN2(SCUMMVM47, bompDecodeLineReverse)
    QUICK_COMP_ASSIGN2(SCUMMVM48, TOUCHEENGINE)
    QUICK_COMP_ASSIGN(SCUMMVM49)
    QUICK_COMP_ASSIGN(SCUMMVM50)
    QUICK_COMP_ASSIGN2(SCUMMVM51, MSRLE)
    QUICK_COMP_ASSIGN2(SCUMMVM52, UNARJ)
    QUICK_COMP_ASSIGN2(SCUMMVM53, UNARJ4)
    QUICK_COMP_ASSIGN2(LZS_UNZIP, PSP_Nanoha)
    QUICK_COMP_ASSIGN(LEGEND_OF_MANA)
    QUICK_COMP_ASSIGN(DIZZY)
    QUICK_COMP_ASSIGN(EDL1)
    QUICK_COMP_ASSIGN(EDL2)
    QUICK_COMP_ASSIGN(DUNGEON_KID)
    QUICK_COMP_ASSIGN(LUNAR_LZ17)
    QUICK_COMP_ASSIGN(LUNAR_LZ18)
    QUICK_COMP_ASSIGN(FRONTMISSION2)
    QUICK_COMP_ASSIGN(RLEINC1)
    QUICK_COMP_ASSIGN(RLEINC2)
    QUICK_COMP_ASSIGN(EVOLUTION)
    QUICK_COMP_ASSIGN(PUYO_LZ10)
    QUICK_COMP_ASSIGN(PUYO_LZ11)
    QUICK_COMP_ASSIGN(NISLZS)
    QUICK_COMP_ASSIGN(UNKNOWN1)
    QUICK_COMP_ASSIGN(UNKNOWN2)
    QUICK_COMP_ASSIGN(UNKNOWN3)
    QUICK_COMP_ASSIGN(UNKNOWN4)
    QUICK_COMP_ASSIGN(UNKNOWN5)
    QUICK_COMP_ASSIGN(UNKNOWN6)
    QUICK_COMP_ASSIGN(UNKNOWN7)
    QUICK_COMP_ASSIGN(UNKNOWN8)
    QUICK_COMP_ASSIGN(UNKNOWN9)
    QUICK_COMP_ASSIGN(UNKNOWN10)
    QUICK_COMP_ASSIGN(UNKNOWN11)
    QUICK_COMP_ASSIGN(UNKNOWN12)
    QUICK_COMP_ASSIGN(UNKNOWN13)
    QUICK_COMP_ASSIGN(UNKNOWN14)
    QUICK_COMP_ASSIGN(UNKNOWN15)
    QUICK_COMP_ASSIGN(UNKNOWN16)
    QUICK_COMP_ASSIGN(UNKNOWN17)
    QUICK_COMP_ASSIGN(UNKNOWN18)
    QUICK_COMP_ASSIGN(UNKNOWN19)
    QUICK_COMP_ASSIGN(BLACKDESERT)
    QUICK_COMP_ASSIGN(BLACKDESERT_RAW)
    QUICK_COMP_ASSIGN(PUCRUNCH)
    QUICK_COMP_ASSIGN(ZPAQ)
    /*
    ----------------------------------------------
    compression algorithms like COMP_ZLIB_COMPRESS
    ----------------------------------------------
    */
    else if(!stricmp(str, "kzip") || !stricmp(str, "kzip_zlib"))  compression_type = COMP_KZIP_ZLIB_COMPRESS;               // exactly like zlib
    else if(!stricmp(str, "kzip_deflate") || !stricmp(str, "kzip_inflate")) compression_type = COMP_KZIP_DEFLATE_COMPRESS;  // exactly like deflate
    else if(!stricmp(str, "uberflate") || !stricmp(str, "uberflate_zlib"))  compression_type = COMP_KZIP_ZLIB_COMPRESS;               // exactly like zlib
    else if(!stricmp(str, "uberflate_deflate") || !stricmp(str, "uberflate_inflate")) compression_type = COMP_KZIP_DEFLATE_COMPRESS;  // exactly like deflate
    QUICK_COMP_ASSIGN(ZLIB_COMPRESS)
    QUICK_COMP_ASSIGN(DEFLATE_COMPRESS)
    QUICK_COMP_ASSIGN(LZO1_COMPRESS)
    QUICK_COMP_ASSIGN(LZO1X_COMPRESS)
    QUICK_COMP_ASSIGN(LZO2A_COMPRESS)
    QUICK_COMP_ASSIGN(XMEMLZX_COMPRESS)
    QUICK_COMP_ASSIGN(BZIP2_COMPRESS)
    QUICK_COMP_ASSIGN(GZIP_COMPRESS)
    QUICK_COMP_ASSIGN(LZSS_COMPRESS)
    QUICK_COMP_ASSIGN(SFL_BLOCK_COMPRESS)
    QUICK_COMP_ASSIGN(SFL_RLE_COMPRESS)
    QUICK_COMP_ASSIGN(SFL_NULLS_COMPRESS)
    QUICK_COMP_ASSIGN(SFL_BITS_COMPRESS)
    QUICK_COMP_ASSIGN(LZF_COMPRESS)
    QUICK_COMP_ASSIGN(BRIEFLZ_COMPRESS)
    QUICK_COMP_ASSIGN(JCALG_COMPRESS)
    QUICK_COMP_ASSIGN(BCL_HUF_COMPRESS)
    QUICK_COMP_ASSIGN(BCL_LZ_COMPRESS)
    QUICK_COMP_ASSIGN(BCL_RICE_COMPRESS)
    QUICK_COMP_ASSIGN(BCL_RLE_COMPRESS)
    QUICK_COMP_ASSIGN(BCL_SF_COMPRESS)
    QUICK_COMP_ASSIGN(SZIP_COMPRESS)
    QUICK_COMP_ASSIGN(HUFFMANLIB_COMPRESS)
    QUICK_COMP_ASSIGN(LZMA_COMPRESS)
    QUICK_COMP_ASSIGN(LZMA_86HEAD_COMPRESS)
    QUICK_COMP_ASSIGN(LZMA_86DEC_COMPRESS)
    QUICK_COMP_ASSIGN(LZMA_86DECHEAD_COMPRESS)
    QUICK_COMP_ASSIGN(LZMA_EFS_COMPRESS)
    QUICK_COMP_ASSIGN(FALCOM_COMPRESS)
    QUICK_COMP_ASSIGN(KZIP_ZLIB_COMPRESS)
    QUICK_COMP_ASSIGN(KZIP_DEFLATE_COMPRESS)
    QUICK_COMP_ASSIGN(PRS_COMPRESS)
    QUICK_COMP_ASSIGN(RNC_COMPRESS)
    QUICK_COMP_ASSIGN(SFL_BLOCK_CHUNKED_COMPRESS)
    else {
        compression_type = get_var32(get_var_from_name(str, -1));   // cool for the future
        if((compression_type <= COMP_NONE) || (compression_type >= COMP_NOP)) {
            fprintf(stderr, "\nError: invalid compression type %s (%d)\n", str, (i32)compression_type);
            myexit(QUICKBMS_ERROR_BMS);
        }
        comtype_scan = 1;   // avoids the quitting of QuickBMS in case of wrong algo
    }
    return(0);
}



int CMD_FileXOR_func(int cmd) {
    int     fd,
            pos_offset,
            curroff;
    u8      *tmp;

    if(CMD.var[0] >= 0) {
        NUMS2BYTES(VAR(0), CMD.num[1], CMD.str[0], CMD.num[0])
    }
    file_xor            = STR(0);
    file_xor_size       = NUM(1);
    if(!file_xor_size) {
        file_xor        = NULL;
        file_xor_pos    = NULL;
    } else {
        file_xor_pos    = &NUM(2);
        pos_offset      = VAR32(3);
        fd              = FILEZ(4); // not implemented
        if(pos_offset >= 0) {
            curroff = myftell(fd);
            if(curroff >= pos_offset) {
                (*file_xor_pos) = curroff - pos_offset;
            } else {
                (*file_xor_pos) = file_xor_size - ((pos_offset - curroff) % file_xor_size);
            }
        }
    }
    return(0);
}



int CMD_FileRot13_func(int cmd) {
    int     fd,
            pos_offset,
            curroff;
    u8      *tmp;

    if(CMD.var[0] >= 0) {
        NUMS2BYTES(VAR(0), CMD.num[1], CMD.str[0], CMD.num[0])
    }
    file_rot13          = STR(0);
    file_rot13_size     = NUM(1);
    if(!file_rot13_size) {
        file_rot13      = NULL;
        file_rot13_pos  = NULL;
    } else {
        file_rot13_pos  = &NUM(2);
        pos_offset      = VAR32(3);
        fd              = FILEZ(4); // not implemented
        if(pos_offset >= 0) {
            curroff = myftell(fd);
            if(curroff >= pos_offset) {
                (*file_rot13_pos) = curroff - pos_offset;
            } else {
                (*file_rot13_pos) = file_rot13_size - ((pos_offset - curroff) % file_rot13_size);
            }
        }
    }
    return(0);
}



int CMD_FileCrypt_func(int cmd) {
    int     fd,
            pos_offset,
            curroff;
    u8      *tmp;

    if(CMD.var[0] >= 0) {
        NUMS2BYTES(VAR(0), CMD.num[1], CMD.str[0], CMD.num[0])
    }
    file_crypt          = STR(0);
    file_crypt_size     = NUM(1);
    if(!file_crypt_size) {
        file_crypt      = NULL;
        file_crypt_pos  = NULL;
    } else {
        file_crypt_pos  = &NUM(2);
        pos_offset      = VAR32(3);
        fd              = FILEZ(4); // not implemented
        if(pos_offset >= 0) {
            curroff = myftell(fd);
            if(curroff >= pos_offset) {
                (*file_crypt_pos) = curroff - pos_offset;
            } else {
                (*file_crypt_pos) = file_crypt_size - ((pos_offset - curroff) % file_crypt_size);
            }
        }
    }
    return(0);
}



int CMD_Strlen_func(int cmd) {
    add_var(CMD.var[0], NULL, NULL, strlen(VAR(1)), sizeof(int));
    return(0);
}



int CMD_GetVarChr_func(int cmd) {
    int     varsz,
            offset,
            fdnum,
            numsz,
            num;
    u8      *var;

    if(CMD.var[1] < 0) {
        fdnum = -CMD.var[1];
        if((fdnum <= 0) || (fdnum > MAX_FILES)) {
            fprintf(stderr, "\nError: invalid MEMORY_FILE number in GetVarChr\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        var   = memory_file[fdnum].data;
        varsz = memory_file[fdnum].size;
    } else {
        var   = VAR(1);
        varsz = VARSZ(1);
    }
    offset = VAR32(2);
    numsz  = NUM(3);
    if(numsz < 0) {  // so anything but TYPE_LONG, TYPE_SHORT, TYPE_BYTE, TYPE_THREEBYTE
        fprintf(stderr, "\nError: GetVarChr supports only the numerical types\n");
        myexit(QUICKBMS_ERROR_BMS);
    }

    if((offset < 0) || ((offset + numsz) > varsz)) {
        fprintf(stderr, "\nError: offset in GetVarChr (0x%"PRIx") is bigger than the var (0x%08x)\n", offset, (i32)varsz);
        myexit(QUICKBMS_ERROR_BMS);
    }

    num = getxx(var + offset, numsz);
    if(verbose < 0) verbose_print(offset, "getvarc", CMD.var[0], NULL, 0, num, numsz);
    add_var(CMD.var[0], NULL, NULL, num, sizeof(int));
    return(0);
}



int CMD_PutVarChr_func(int cmd) {
    int     varsz,
            offset,
            fdnum = 0,
            numsz,
            num,
            newoff;
    u8      *var;

    if(CMD.var[0] < 0) {
        fdnum = -CMD.var[0];
        if((fdnum <= 0) || (fdnum > MAX_FILES)) {
            fprintf(stderr, "\nError: invalid MEMORY_FILE number in PutVarChr\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        var   = memory_file[fdnum].data;
        varsz = memory_file[fdnum].size;
    } else {
        var   = VAR(0);
        varsz = VARSZ(0);
    }
    offset = VAR32(1);
    num    = VAR32(2);
    numsz  = NUM(3);
    if(numsz < 0) {  // so anything but TYPE_LONG, TYPE_SHORT, TYPE_BYTE, TYPE_THREEBYTE
        fprintf(stderr, "\nError: PutVarChr supports only the numerical types\n");
        myexit(QUICKBMS_ERROR_BMS);
    }

    if(offset < 0) {    // from the end, should work ONLY with memory_files
        offset = varsz + offset;    // like varsz - (-offset)
        if(offset < 0) {
            fprintf(stderr, "\nError: offset in PutVarChr (0x%"PRIx") is negative\n", offset);
            myexit(QUICKBMS_ERROR_BMS);
        }
    }
    newoff = offset + numsz;
    if((offset > varsz) || (newoff > varsz)) {  // this mode is experimental!
        if(newoff < offset) ALLOC_ERR;
        if(newoff == -1) ALLOC_ERR;
        var = realloc(var, newoff + 1);
        if(!var) STD_ERR(QUICKBMS_ERROR_MEMORY);
        memset(var + varsz, 0, (newoff + 1) - varsz); // not needed
        varsz = newoff;
        if(CMD.var[0] < 0) {
            memory_file[fdnum].data    = var;
            memory_file[fdnum].size    = varsz;
            memory_file[fdnum].maxsize = varsz;
        } else {
            //add_var(CMD.var[0], NULL, var, 0, varsz);
            DIRECT_ADDVAR(0, var, varsz);   // saves memory and is faster
        }
        //printf("\nError: offset in PutVarChr (0x%"PRIx") is bigger than the var (0x%08x)\n", offset, (i32)varsz);
        //myexit(QUICKBMS_ERROR_BMS);
    }

    if(verbose < 0) verbose_print(offset, "putvarc", CMD.var[0], NULL, 0, num, numsz);
    putxx(var + offset, num, numsz);
    return(0);
}



int CMD_Debug_func(int cmd) {
    int     type;

    type = NUM(0);
    if(verbose) {   // both positive and negative
        verbose = 0;
    } else {
        verbose = 1;
        if(type) verbose = -1;
    }
    return(0);
}



int CMD_Padding_func(int cmd) {
    int     fd;
    u_int   tmp,
            size,
            offset;

    fd   = FILEZ(1);
    size = VAR32(0);

    offset = myftell(fd);
    tmp = offset % size;
    if(tmp) myfseek(fd, size - tmp, SEEK_CUR);
    return(0);
}



int CMD_Encryption_func(int cmd, int invert_mode) {
#ifndef DISABLE_SSL
#define IVEC_MYCRYPTO(X) \
    mycrypto = ivec ? EVP_##X##_cbc() : EVP_##X##_ecb()
#define AUTO_MYCRYPTO(X) \
    } else if(!stricmp(type, #X)) { \
        mycrypto = EVP_##X();
#define AUTO_MYHASH(X) \
    } else if(!stricmp(type, #X)) { \
        myhash = EVP_##X();

    static int  last_cmd    = -1;   // for reimport mode;
    if(invert_mode) {
        cmd = last_cmd;
        if(cmd < 0) return(0);  // no encryption previously used
    }
    last_cmd = cmd;

    static int  load_algos      = 0;
    const EVP_CIPHER  *mycrypto = NULL;
    const EVP_MD      *myhash   = NULL;
#endif
    u32     tmp1,
            tmp2,
            tmp3,
            tmp4;
    int     keysz,
            ivecsz,
            force_keysz;
    u8      tmp_str[32],
            *type,
            *key,
            *ivec;

    // reset ANY ctx
#ifndef DISABLE_SSL
    FREEX(evp_ctx, EVP_CIPHER_CTX_cleanup(evp_ctx))
    FREEX(evpmd_ctx, EVP_MD_CTX_cleanup(evpmd_ctx))
    FREEX(blowfish_ctx,)
#endif
    FREEX(tea_ctx,)
    FREEX(xtea_ctx,)
    FREEX(xxtea_ctx,)
    FREEX(swap_ctx,)
    FREEX(math_ctx,)
    FREEX(xmath_ctx,)
    FREEX(random_ctx,)
    FREEX(xor_ctx, free(xor_ctx->key))
    FREEX(rot_ctx, free(rot_ctx->key))
    FREEX(rotate_ctx,)
    FREEX(reverse_ctx,)
    FREEX(inc_ctx,)
    FREEX(charset_ctx,)
    FREEX(charset2_ctx,)
    FREEX(twofish_ctx,)
    FREEX(seed_ctx,)
    FREEX(serpent_ctx,)
    if(ice_ctx) {
        ice_key_destroy(ice_ctx);
        ice_ctx = NULL;
    }
    if(rotor_ctx) {
        rotor_dealloc(rotor_ctx);
        rotor_ctx = NULL;
    }
    FREEX(ssc_ctx, free(ssc_ctx->key))
    FREEX(wincrypt_ctx,)
    FREEX(cunprot_ctx,)
    FREEX(zipcrypto_ctx,)
    FREEX(threeway_ctx,)
    FREEX(skipjack_ctx,)
    FREEX(anubis_ctx,)
    FREEX(aria_ctx,)
    FREEX(crypton_ctx,)
    if(frog_ctx) frog_ctx = NULL;   // this is different!
    FREEX(gost_ctx,)
    if(lucifer_ctx) lucifer_ctx = 0; // different
    if(mars_ctx) mars_ctx = NULL;   // this is different!
    FREEX(misty1_ctx,)
    FREEX(noekeon_ctx,)
    FREEX(seal_ctx,)
    FREEX(safer_ctx,)
    if(kirk_ctx >= 0) kirk_ctx = -1;
    FREEX(pc1_128_ctx,)
    FREEX(pc1_256_ctx,)
    FREEX(crc_ctx,)
    FREEX(execute_ctx,)
    FREEX(calldll_ctx,)
#ifndef DISABLE_MCRYPT
    if(mcrypt_ctx) {
        mcrypt_generic_deinit(mcrypt_ctx);
        mcrypt_module_close(mcrypt_ctx);
        mcrypt_ctx = NULL;
    }
#endif
#ifndef DISABLE_TOMCRYPT
    if(tomcrypt_ctx) {
        FREEZ(tomcrypt_ctx->ivec)
        FREEZ(tomcrypt_ctx->nonce)
        FREEZ(tomcrypt_ctx->header)
        FREEZ(tomcrypt_ctx->tweak)
    }
    FREEX(tomcrypt_ctx,)
#endif
    if(cmd < 0) return(0);  // bms init

    type   = VAR(0);
    type   = skip_delimit(type);
    key    = STR(1);
    keysz  = NUM(1);
    ivec   = STR(2);    // ivec can be NULL (ecb) or a string (CBC)
    ivecsz = NUM(2);
    if(ivecsz <= 0) ivec = NULL; // so can be used "" to skip it
    encrypt_mode = 0;
    if(CMD.var[3] >= 0) encrypt_mode = VAR32(3);
    if(invert_mode) encrypt_mode = !encrypt_mode;

    quick_var_from_name_check(&key,  &keysz);
    quick_var_from_name_check(&ivec, &ivecsz);
    if(CMD.var[4] >= 0) {
        force_keysz = VAR32(4);
        if(force_keysz > 0) keysz = force_keysz;    // no checks on the length
    }

    if(!stricmp(type, "?")) {
        fgetz(tmp_str, sizeof(tmp_str), stdin,
            "\n- you must specify the encryption algorithm to use:\n  ");
        type = tmp_str;
    }

    if(!strnicmp(type, "EVP_", 4)) type += 4;

#ifndef DISABLE_SSL
    if(!load_algos) {
        OpenSSL_add_all_algorithms();
        load_algos = 1;
    }
#endif

    if(!stricmp(type, "")) {    // || !keysz) {
        // do nothing, disables the encryption
        last_cmd = -1;

#ifndef DISABLE_SSL
    } else if(!stricmp(type, "des")) {
        IVEC_MYCRYPTO(des);

    } else if(!stricmp(type, "3des2") || !stricmp(type, "3des-112") || !stricmp(type, "des_ede") || !stricmp(type, "des_ede2")) {
        IVEC_MYCRYPTO(des_ede);

    } else if(!stricmp(type, "3des") || !stricmp(type, "3des-168") || !stricmp(type, "des_ede3")) {
        IVEC_MYCRYPTO(des_ede3);

    } else if(!stricmp(type, "desx")) {
        mycrypto = EVP_desx_cbc();

    } else if(!stricmp(type, "rc4") || !stricmp(type, "arc4")) {
        mycrypto = EVP_rc4();

#ifndef OPENSSL_NO_IDEA
    } else if(!stricmp(type, "idea")) {
        IVEC_MYCRYPTO(idea);
#endif

    } else if(!stricmp(type, "rc2")) {
        IVEC_MYCRYPTO(rc2);

    } else if(!stricmp(type, "blowfish")) {
        //IVEC_MYCRYPTO(bf); // blowfish must be handled manually because BF_decrypt != BF_ecb_encrypt
        blowfish_ctx = calloc(1, sizeof(BF_KEY));
        if(!blowfish_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        BF_set_key(blowfish_ctx, keysz, key);

    } else if(!stricmp(type, "cast5")) {
        IVEC_MYCRYPTO(cast5);

    } else if(!stricmp(type, "aes") || !stricmp(type, "Rijndael")) {
        switch(keysz << 3) {
            case 128: IVEC_MYCRYPTO(aes_128); break;
            case 192: IVEC_MYCRYPTO(aes_192); break;
            case 256: IVEC_MYCRYPTO(aes_256); break;
            default: {
                fprintf(stderr, "\nError: the key for algorithm %s has an invalid size (%d)\n", type, (i32)keysz);
                myexit(QUICKBMS_ERROR_ENCRYPTION);
                break;
            }
        }

    } else if(!stricmp(type, "aes_128_ctr") || !stricmp(type, "aes_192_ctr") || !stricmp(type, "aes_256_ctr")) {
        switch(keysz << 3) {
            case 128: break;
            case 192: break;
            case 256: break;
            default: {
                fprintf(stderr, "\nError: the key for algorithm %s has an invalid size (%d)\n", type, (i32)keysz);
                myexit(QUICKBMS_ERROR_ENCRYPTION);
                break;
            }
        }
        aes_ctr_ctx = calloc(1, sizeof(aes_ctr_ctx_t));
        if(!aes_ctr_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(!encrypt_mode) {
            AES_set_decrypt_key(key, keysz << 3, &aes_ctr_ctx->ctx);
        } else {
            AES_set_encrypt_key(key, keysz << 3, &aes_ctr_ctx->ctx);
        }
        if((ivecsz > 0) && (ivecsz < AES_BLOCK_SIZE)) memcpy(aes_ctr_ctx->ivec, ivec, ivecsz);
#endif

    } else if(!stricmp(type, "seed")) {
//#ifndef OPENSSL_NO_SEED
//        IVEC_MYCRYPTO(seed);
//#else
        seed_ctx = calloc(1, sizeof(SEED_context));
        if(!seed_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        do_seed_setkey(seed_ctx, key, keysz);
//#endif
    } else if(!stricmp(type, "tea")) {
        tea_ctx = calloc(1, sizeof(tea_context));
        if(!tea_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);

        tmp1 = 0x9e3779b9;
        tmp2 = 0xc6ef3720;
        tmp3 = 1;   // big endian
        tmp4 = 32;
        if(ivec) get_parameter_numbers(ivec, &tmp1, &tmp2, &tmp3, &tmp4, NULL);
        tea_setup(tea_ctx, key, ivec ? 1 : 0, tmp1, tmp2, tmp3, tmp4);

    } else if(!stricmp(type, "xtea")) {
        xtea_ctx = calloc(1, sizeof(xtea_context));
        if(!xtea_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        tmp1 = 0x9E3779B9;
        tmp2 = 1;   // big endian
        tmp3 = 32;
        if(ivec) get_parameter_numbers(ivec, &tmp1, &tmp2, &tmp3, NULL);
        xtea_setupx(xtea_ctx, key, ivec ? 1 : 0, tmp1, tmp2, tmp3);

    } else if(!stricmp(type, "xxtea")) {
        xxtea_ctx = calloc(1, sizeof(xxtea_context));
        if(!xxtea_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        tmp1 = 0x9e3779b9;
        tmp2 = 1;   // big endian
        tmp3 = 6;
        if(ivec) get_parameter_numbers(ivec, &tmp1, &tmp2, &tmp3, NULL);
        xxtea_setup(xxtea_ctx, key, ivec ? 1 : 0, tmp1, tmp2, tmp3);

    } else if(!stricmp(type, "swap")) {
        swap_ctx = calloc(1, sizeof(swap_context));
        if(!swap_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        swap_setkey(swap_ctx, myatoi(key), ivec);

    } else if(!stricmp(type, "math")) {
        math_ctx = calloc(1, sizeof(math_context));
        if(!math_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        math_setkey(math_ctx, key, ivec);

    } else if(!stricmp(type, "xmath")) {
        xmath_ctx = calloc(1, sizeof(xmath_context));
        if(!xmath_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        xmath_setkey(xmath_ctx, key, ivec);

    } else if(!stricmp(type, "random")) {
        random_ctx = calloc(1, sizeof(random_context));
        if(!random_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        random_setkey(random_ctx, key, ivec);

    } else if(!stricmp(type, "xor")) {
        xor_ctx = calloc(1, sizeof(xor_context));
        if(!xor_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(xor_setkey(xor_ctx, key, keysz) < 0) {
            fprintf(stderr, "\nError: xor_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "rot") || !stricmp(type, "rot13")) {
        rot_ctx = calloc(1, sizeof(rot_context));
        if(!rot_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(rot_setkey(rot_ctx, key, keysz) < 0) {
            fprintf(stderr, "\nError: rot_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "rotate") || !stricmp(type, "ror") || !stricmp(type, "rol")) {
        rotate_ctx = calloc(1, sizeof(rotate_context));
        if(!rotate_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        rotate_setkey(rotate_ctx, key, ivec);

    } else if(!stricmp(type, "reverse")) {
        reverse_ctx = calloc(1, sizeof(reverse_context));
        if(!reverse_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        reverse_setkey(reverse_ctx, key);

    } else if(stristr(type, "incremental")) {
        inc_ctx = calloc(1, sizeof(inc_context));
        if(!inc_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(stristr(type, "rot") || stristr(type, "add") || stristr(type, "sum") || stristr(type, "sub")) {
            inc_setkey(inc_ctx, 1, myatoi(key), myatoi(ivec));
        } else {
            inc_setkey(inc_ctx, 0, myatoi(key), myatoi(ivec));
        }

    } else if(!stricmp(type, "charset") || !stricmp(type, "chartable")) {
        charset_ctx = calloc(1, sizeof(charset_context));
        if(!charset_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        charset_setkey(charset_ctx, key, keysz);

    } else if(!stricmp(type, "charset2") || !stricmp(type, "chartable2")) {
        charset2_ctx = calloc(1, sizeof(charset_context));
        if(!charset2_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        charset_setkey(charset2_ctx, key, keysz);

    } else if(!stricmp(type, "ebcdic")) {
        charset2_ctx = calloc(1, sizeof(charset_context));
        if(!charset2_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        charset_setkey(charset2_ctx, (void *)ebcdic_charset, sizeof(ebcdic_charset));

    } else if(!stricmp(type, "twofish")) {
        twofish_ctx = calloc(1, sizeof(TWOFISH_context));
        if(!twofish_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        do_twofish_setkey(twofish_ctx, key, keysz);

    } else if(!stricmp(type, "serpent")) {
        serpent_ctx = calloc(1, sizeof(serpent_context_t));
        if(!serpent_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        serpent_setkey_internal(serpent_ctx, key, keysz);

    } else if(!stricmp(type, "icecrypt")) {
        ice_ctx = do_ice_key(key, keysz, 1);
        if(!ice_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);

    } else if(!stricmp(type, "ice")) {
        ice_ctx = do_ice_key(key, keysz, 0);
        if(!ice_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);

    } else if(!stricmp(type, "rotor")) {
        rotor_ctx = rotorobj_new(ivec ? myatoi(ivec) : 12, key, keysz);
        if(!rotor_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);

    } else if(!stricmp(type, "ssc")) {
        ssc_ctx = calloc(1, sizeof(ssc_context));
        if(!ssc_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(ssc_setkey(ssc_ctx, key, keysz) < 0) {
            fprintf(stderr, "\nError: ssc_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "wincrypt") || !stricmp(type, "CryptDecrypt") || !stricmp(type, "CryptEncrypt")) {
        wincrypt_ctx = calloc(1, sizeof(wincrypt_context));
        if(!wincrypt_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(wincrypt_setkey(wincrypt_ctx, key, keysz, ivec) < 0) {
            fprintf(stderr, "\nError: wincrypt_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "cryptunprotect") || !stricmp(type, "CryptUnprotectData") || !stricmp(type, "cunprot")) {
        cunprot_ctx = calloc(1, sizeof(cunprot_context));
        if(!cunprot_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(cunprot_setkey(cunprot_ctx, key, keysz) < 0) {
            fprintf(stderr, "\nError: cunprot_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "zipcrypto")) {
        zipcrypto_ctx = calloc(3+1, sizeof(u_int)); // the additional 1 is used for the -12 trick
        if(!zipcrypto_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        zipcrypto_init_keys(key, zipcrypto_ctx, (void *)get_crc_table());
        if(ivec) zipcrypto_ctx[3] = myatoi(ivec);

#ifndef DISABLE_SSL
#if OPENSSL_VERSION_NUMBER < 0x1000000fL
    #define OPENSSL_NO_WHIRLPOOL    // uff, necessary
#endif
    // blah, openssl doesn't catch all the names... boring
    // directly from evp.h with a simple strings substitution
    AUTO_MYHASH(md_null)
#ifndef OPENSSL_NO_MD2
    AUTO_MYHASH(md2)
#endif
#ifndef OPENSSL_NO_MD4
    AUTO_MYHASH(md4)
#endif
#ifndef OPENSSL_NO_MD5
    AUTO_MYHASH(md5)
#endif
#ifndef OPENSSL_NO_SHA
    AUTO_MYHASH(sha)
    AUTO_MYHASH(sha1)
    AUTO_MYHASH(dss)
    AUTO_MYHASH(dss1)
#ifndef OPENSSL_NO_ECDSA
    AUTO_MYHASH(ecdsa)
#endif
#endif
#ifndef OPENSSL_NO_SHA256
    AUTO_MYHASH(sha224)
    AUTO_MYHASH(sha256)
#endif
#ifndef OPENSSL_NO_SHA512
    AUTO_MYHASH(sha384)
    AUTO_MYHASH(sha512)
#endif
#ifndef OPENSSL_NO_MDC2
    AUTO_MYHASH(mdc2)
#endif
#ifndef OPENSSL_NO_RIPEMD
    AUTO_MYHASH(ripemd160)
#endif
#ifndef OPENSSL_NO_WHIRLPOOL
    AUTO_MYHASH(whirlpool)    // not all versions support it
#endif
    AUTO_MYCRYPTO(enc_null)        /* does nothing :-) */
#ifndef OPENSSL_NO_DES
    AUTO_MYCRYPTO(des_ecb)
    AUTO_MYCRYPTO(des_ede)
    AUTO_MYCRYPTO(des_ede3)
    AUTO_MYCRYPTO(des_ede_ecb)
    AUTO_MYCRYPTO(des_ede3_ecb)
    AUTO_MYCRYPTO(des_cfb64)
# define EVP_des_cfb EVP_des_cfb64
    AUTO_MYCRYPTO(des_cfb1)
    AUTO_MYCRYPTO(des_cfb8)
    AUTO_MYCRYPTO(des_ede_cfb64)
# define EVP_des_ede_cfb EVP_des_ede_cfb64
#if 0
    AUTO_MYCRYPTO(des_ede_cfb1)
    AUTO_MYCRYPTO(des_ede_cfb8)
#endif
    AUTO_MYCRYPTO(des_ede3_cfb64)
# define EVP_des_ede3_cfb EVP_des_ede3_cfb64
    AUTO_MYCRYPTO(des_ede3_cfb1)
    AUTO_MYCRYPTO(des_ede3_cfb8)
    AUTO_MYCRYPTO(des_ofb)
    AUTO_MYCRYPTO(des_ede_ofb)
    AUTO_MYCRYPTO(des_ede3_ofb)
    AUTO_MYCRYPTO(des_cbc)
    AUTO_MYCRYPTO(des_ede_cbc)
    AUTO_MYCRYPTO(des_ede3_cbc)
    AUTO_MYCRYPTO(desx_cbc)
/* This should now be supported through the dev_crypto ENGINE. But also, why are
 * rc4 and md5 declarations made here inside a "NO_DES" precompiler branch? */
#if 0
# ifdef OPENSSL_OPENBSD_DEV_CRYPTO
    AUTO_MYCRYPTO(dev_crypto_des_ede3_cbc)
    AUTO_MYCRYPTO(dev_crypto_rc4)
    AUTO_MYHASH(dev_crypto_md5)
# endif
#endif
#endif
#ifndef OPENSSL_NO_RC4
    AUTO_MYCRYPTO(rc4)
    AUTO_MYCRYPTO(rc4_40)
#endif
#ifndef OPENSSL_NO_IDEA
    AUTO_MYCRYPTO(idea_ecb)
    AUTO_MYCRYPTO(idea_cfb64)
# define EVP_idea_cfb EVP_idea_cfb64
    AUTO_MYCRYPTO(idea_ofb)
    AUTO_MYCRYPTO(idea_cbc)
#endif
#ifndef OPENSSL_NO_RC2
    AUTO_MYCRYPTO(rc2_ecb)
    AUTO_MYCRYPTO(rc2_cbc)
    AUTO_MYCRYPTO(rc2_40_cbc)
    AUTO_MYCRYPTO(rc2_64_cbc)
    AUTO_MYCRYPTO(rc2_cfb64)
# define EVP_rc2_cfb EVP_rc2_cfb64
    AUTO_MYCRYPTO(rc2_ofb)
#endif
#ifndef OPENSSL_NO_BF
    AUTO_MYCRYPTO(bf_ecb)
    AUTO_MYCRYPTO(bf_cbc)
    AUTO_MYCRYPTO(bf_cfb64)
# define EVP_bf_cfb EVP_bf_cfb64
    AUTO_MYCRYPTO(bf_ofb)
#endif
#ifndef OPENSSL_NO_CAST
    AUTO_MYCRYPTO(cast5_ecb)
    AUTO_MYCRYPTO(cast5_cbc)
    AUTO_MYCRYPTO(cast5_cfb64)
# define EVP_cast5_cfb EVP_cast5_cfb64
    AUTO_MYCRYPTO(cast5_ofb)
#endif
#ifndef OPENSSL_NO_RC5
    AUTO_MYCRYPTO(rc5_32_12_16_cbc)
    AUTO_MYCRYPTO(rc5_32_12_16_ecb)
    AUTO_MYCRYPTO(rc5_32_12_16_cfb64)
# define EVP_rc5_32_12_16_cfb EVP_rc5_32_12_16_cfb64
    AUTO_MYCRYPTO(rc5_32_12_16_ofb)
#endif
#ifndef OPENSSL_NO_AES
    AUTO_MYCRYPTO(aes_128_ecb)
    AUTO_MYCRYPTO(aes_128_cbc)
    AUTO_MYCRYPTO(aes_128_cfb1)
    AUTO_MYCRYPTO(aes_128_cfb8)
    AUTO_MYCRYPTO(aes_128_cfb128)
# define EVP_aes_128_cfb EVP_aes_128_cfb128
    AUTO_MYCRYPTO(aes_128_ofb)
#if 0
    AUTO_MYCRYPTO(aes_128_ctr)
#endif
    AUTO_MYCRYPTO(aes_192_ecb)
    AUTO_MYCRYPTO(aes_192_cbc)
    AUTO_MYCRYPTO(aes_192_cfb1)
    AUTO_MYCRYPTO(aes_192_cfb8)
    AUTO_MYCRYPTO(aes_192_cfb128)
# define EVP_aes_192_cfb EVP_aes_192_cfb128
    AUTO_MYCRYPTO(aes_192_ofb)
#if 0
    AUTO_MYCRYPTO(aes_192_ctr)
#endif
    AUTO_MYCRYPTO(aes_256_ecb)
    AUTO_MYCRYPTO(aes_256_cbc)
    AUTO_MYCRYPTO(aes_256_cfb1)
    AUTO_MYCRYPTO(aes_256_cfb8)
    AUTO_MYCRYPTO(aes_256_cfb128)
# define EVP_aes_256_cfb EVP_aes_256_cfb128
    AUTO_MYCRYPTO(aes_256_ofb)
#if 0
    AUTO_MYCRYPTO(aes_256_ctr)
#endif
#endif
#ifndef OPENSSL_NO_CAMELLIA
    AUTO_MYCRYPTO(camellia_128_ecb)
    AUTO_MYCRYPTO(camellia_128_cbc)
    AUTO_MYCRYPTO(camellia_128_cfb1)
    AUTO_MYCRYPTO(camellia_128_cfb8)
    AUTO_MYCRYPTO(camellia_128_cfb128)
# define EVP_camellia_128_cfb EVP_camellia_128_cfb128
    AUTO_MYCRYPTO(camellia_128_ofb)
    AUTO_MYCRYPTO(camellia_192_ecb)
    AUTO_MYCRYPTO(camellia_192_cbc)
    AUTO_MYCRYPTO(camellia_192_cfb1)
    AUTO_MYCRYPTO(camellia_192_cfb8)
    AUTO_MYCRYPTO(camellia_192_cfb128)
# define EVP_camellia_192_cfb EVP_camellia_192_cfb128
    AUTO_MYCRYPTO(camellia_192_ofb)
    AUTO_MYCRYPTO(camellia_256_ecb)
    AUTO_MYCRYPTO(camellia_256_cbc)
    AUTO_MYCRYPTO(camellia_256_cfb1)
    AUTO_MYCRYPTO(camellia_256_cfb8)
    AUTO_MYCRYPTO(camellia_256_cfb128)
# define EVP_camellia_256_cfb EVP_camellia_256_cfb128
    AUTO_MYCRYPTO(camellia_256_ofb)
#endif

#ifndef OPENSSL_NO_SEED
    AUTO_MYCRYPTO(seed_ecb)
    AUTO_MYCRYPTO(seed_cbc)
    AUTO_MYCRYPTO(seed_cfb128)
# define EVP_seed_cfb EVP_seed_cfb128
    AUTO_MYCRYPTO(seed_ofb)
#endif
#endif

#ifndef DISABLE_MCRYPT
    } else if((mcrypt_ctx = quick_mcrypt_check(type))) {    // libmcrypt
        if(mcrypt_generic_init(mcrypt_ctx, key, keysz, ivec) < 0) {
            fprintf(stderr, "\nError: mcrypt key failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }
#endif

#ifndef DISABLE_TOMCRYPT
    } else if((tomcrypt_ctx = tomcrypt_doit(NULL, type, NULL, 0, NULL, 0, NULL))) {    // libtomcrypt
        tomcrypt_ctx->key   = key;
        tomcrypt_ctx->keysz = keysz;
        tomcrypt_lame_ivec(tomcrypt_ctx, ivec, ivecsz);
#endif

    } else if(!strnicmp(type, "crc", 3) || !stricmp(type, "checksum")) {
        crc_ctx = calloc(1, sizeof(crc_context));
        if(!crc_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        crc_ctx->poly           = 0xedb88320;   // it's the one where the second element is 0x77073096
        crc_ctx->bits           = 32;
        crc_ctx->init           = -1;
        crc_ctx->final          = -1;
        crc_ctx->type           = 0;
        crc_ctx->rever          = 0;
        crc_ctx->bitmask_side   = 1;
        if(key && key[0]) crc_ctx->poly = myatoi(key);
        if(ivec) {
            int     x1=crc_ctx->init,x2=crc_ctx->final;
            get_parameter_numbers(ivec,
                &crc_ctx->bits, &x1, &x2, &crc_ctx->type, &crc_ctx->rever, &crc_ctx->bitmask_side, NULL);
            crc_ctx->init=x1; crc_ctx->final=x2;
        }
        if(!key || !key[0]) {   // useless, in case of key "" and size 16
                   if(!stricmp(type, "crc8") || (crc_ctx->bits == 8)) {
                crc_ctx->poly           = 0;
                crc_ctx->bits           = 8;
                crc_ctx->init           = 0;
                crc_ctx->final          = 0;
                crc_ctx->type           = 5;
                crc_ctx->rever          = 0;
                crc_ctx->bitmask_side   = 0;
            } else if(!stricmp(type, "crc16") || (crc_ctx->bits == 16)) {
                crc_ctx->poly           = 0xa001;   // second value equal to 0xc0c1
                crc_ctx->bits           = 16;
                crc_ctx->init           = 0;
                crc_ctx->final          = 0;
                crc_ctx->type           = 0;
                crc_ctx->rever          = 0;
                crc_ctx->bitmask_side   = 1;
            } else if(!stricmp(type, "crc64") || (crc_ctx->bits == 64)) {
                crc_ctx->poly           = 0xad93d23594c935a9LL;   // second value 0x7ad870c830358979
                crc_ctx->bits           = 64;
                crc_ctx->init           = 0;
                crc_ctx->final          = 0;
                crc_ctx->type           = 0;
                crc_ctx->rever          = 1;
                crc_ctx->bitmask_side   = 0;
            }
        }

        crc_make_table((void *)crc_ctx->table, NULL, crc_ctx->poly, crc_ctx->bits, endian, crc_ctx->rever, crc_ctx->bitmask_side, NULL);
        add_var(0, "QUICKBMS_CRC", NULL, crc_ctx->table[1], sizeof(u_int)); // used for debugging

    } else if(!stricmp(type, "execute")) {
        execute_ctx = mystrdup_simple(key);

    } else if(!stricmp(type, "calldll")) {
        calldll_ctx = mystrdup_simple(key);

    // the following algorithms have been implemented "before" adding the mcrypt
    // library... well I don't want to remove them and the hours I lost
    // note also that those are all untested, just added and not verified
    } else if(!stricmp(type, "3way")) {
        threeway_ctx = calloc(3, sizeof(u_int));
        if(!threeway_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(threeway_setkey(threeway_ctx, key, keysz) < 0) {
            fprintf(stderr, "\nError: threeway_setkey failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }

    } else if(!stricmp(type, "skipjack")) {
        skipjack_ctx = calloc(10, 256);
        if(!skipjack_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        skipjack_makeKey(key, skipjack_ctx);

    } else if(!stricmp(type, "anubis")) {
        anubis_ctx = calloc(1, sizeof(ANUBISstruct));
        if(!anubis_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        anubis_ctx->keyBits = keysz * 8;
        ANUBISkeysetup(key, anubis_ctx);

    } else if(!stricmp(type, "aria")) {
        aria_ctx = calloc(1, sizeof(aria_ctx_t));
        if(!aria_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(!encrypt_mode) aria_ctx->Nr = ARIA_DecKeySetup(key, aria_ctx->rk, keysz * 8);
        else              aria_ctx->Nr = ARIA_EncKeySetup(key, aria_ctx->rk, keysz * 8);

    } else if(!stricmp(type, "crypton")) {
        crypton_ctx = calloc(104, sizeof(u_int));
        if(!crypton_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        crypton_set_key((void *)key, keysz, crypton_ctx);

    } else if(!stricmp(type, "frog")) {
        frog_ctx = frog_set_key((void *)key, keysz);

    } else if(!strnicmp(type, "gost", 4)) {
        gost_ctx = calloc(1, sizeof(gost_ctx_t));
        if(!gost_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        gost_kboxinit();
        memcpy(gost_ctx->key, key, 4*8);
        if(ivec) memcpy(gost_ctx->iv, ivec, 4*2);
        if(stristr(type + 4, "ofb")) gost_ctx->type = 1;
        else if(stristr(type + 4, "cfb")) gost_ctx->type = 2;

    } else if(!stricmp(type, "lucifer")) {
        lucifer_ctx = 1;
        if(!encrypt_mode) lucifer_loadkey(key, 0);  // or 1?
        else              lucifer_loadkey(key, 1);  // or 0?

    } else if(!stricmp(type, "kirk")) {
        if(!ivec) {
            kirk_ctx = 1;
        } else {
            kirk_ctx = myatoi(ivec);
        }
        kirk_init();

    } else if(!stricmp(type, "pc1") || !stricmp(type, "pc1_128") || !stricmp(type, "pc1_256")) {
        switch(keysz) {
            case 16: {
                pc1_128_ctx = calloc(keysz, 1);
                if(!pc1_128_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
                memcpy(pc1_128_ctx, key, keysz);
                break;
            }
            case 32: {
                pc1_256_ctx = calloc(keysz, 1);
                if(!pc1_256_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
                memcpy(pc1_256_ctx, key, keysz);
                break;
            }
            default: {
                fprintf(stderr, "\nError: the key for algorithm %s has an invalid size (%d)\n", type, (i32)keysz);
                myexit(QUICKBMS_ERROR_ENCRYPTION);
                break;
            }
        }

    } else if(!stricmp(type, "mars")) {
        mars_ctx = mars_set_key((void *)key, keysz);

    } else if(!stricmp(type, "misty1")) {
        misty1_ctx = calloc(4, 4);
        if(!misty1_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        misty1_keyinit(misty1_ctx, (void *)key);

    } else if(!stricmp(type, "noekeon")) {
        noekeon_ctx = calloc(1, sizeof(NOEKEONstruct));
        if(!noekeon_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        NOEKEONkeysetup(key, noekeon_ctx);

    } else if(!stricmp(type, "seal")) {
        seal_ctx = calloc(1, sizeof(seal_ctx_t));
        if(!seal_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        seal_key(seal_ctx, key);

    } else if(!stricmp(type, "safer")) {
        safer_ctx = calloc(1, sizeof(safer_key_t));
        if(!safer_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        Safer_Init_Module();
        Safer_Expand_Userkey(key, key + SAFER_BLOCK_LEN, ivec ? myatoi(ivec) : SAFER_K128_DEFAULT_NOF_ROUNDS, 0, (void *)safer_ctx);

    } else {
#ifndef DISABLE_SSL
        mycrypto = EVP_get_cipherbyname(type);
        if(!mycrypto) {
            myhash = EVP_get_digestbyname(type);
            if(!myhash) {
#else
        {   {
#endif
                fprintf(stderr, "\nError: unsupported encryption/hashing type (%s)\n",  type);
                myexit(QUICKBMS_ERROR_ENCRYPTION);
            }
        }
    }

#ifndef DISABLE_SSL
    if(mycrypto) {  // handled for last because it's global for OpenSSL
        evp_ctx = calloc(1, sizeof(EVP_CIPHER_CTX));
        if(!evp_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        EVP_CIPHER_CTX_init(evp_ctx);
        if(!EVP_CipherInit(evp_ctx, mycrypto, NULL, NULL, encrypt_mode)) {
            fprintf(stderr, "\nError: EVP_CipherInit failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }
        if(!EVP_CIPHER_CTX_set_key_length(evp_ctx, keysz)) {
            fprintf(stderr, "\nError: EVP_CIPHER_CTX_set_key_length failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }
        //EVP_CIPHER_CTX_set_padding(evp_ctx, 0);   // do not enable it: "If the pad parameter is zero then no padding is performed, the total amount of data encrypted or decrypted must then be a multiple of the block size or an error will occur."
        if(!EVP_CipherInit(evp_ctx, NULL, key, ivec, encrypt_mode)) {
            fprintf(stderr, "\nError: EVP_CipherInit key failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }
    }
    if(myhash) {    // handled for last because it's global for OpenSSL
        evpmd_ctx = calloc(1, sizeof(EVP_MD_CTX));
        if(!evpmd_ctx) STD_ERR(QUICKBMS_ERROR_MEMORY);
        EVP_MD_CTX_init(evpmd_ctx);
        if(!EVP_DigestInit(evpmd_ctx, myhash)) {
            fprintf(stderr, "\nError: EVP_DigestInit failed\n");
            myexit(QUICKBMS_ERROR_ENCRYPTION);
        }
    }
#endif

    if((verbose > 0) && keysz) printf("- encryption with algorithm %s and key of %d bytes\n", type, (i32)keysz);
    return(0);
}



int CMD_Print_func(int cmd) {
    FILE    *fdo    = stdout;
    i32     //i,
            t,
            idx,
            len,
            force_len,
            hex,
            c_out,
            space;
    u8      *p,
            *msg,
            *var,
            *flags;

    msg = STR(0);
    fprintf(stderr, "- SCRIPT's MESSAGE:\n");

    if(quick_gui_exit) fdo = stderr;

    while(*msg) {
        fprintf(stderr, "  ");
        //for(i = 0; i < 77; i++) { // removed because useless
        for(;;) {
            if(!*msg) break;
            if(*msg == '%') {
                msg++;
                p = strchr(msg, '%');
                if(!p) continue;
                hex       = 0;
                c_out     = 0;
                space     = 0;
                force_len = -1;
                idx = get_var_from_name(msg, p - msg);
                if(idx < 0) {
                    for(flags = msg; flags < p; flags++) {
                        if(*flags == '|') break;
                    }
                    if(flags >= p) continue;
                    idx = get_var_from_name(msg, flags - msg);
                    if(idx < 0) continue;
                    for(++flags; flags < p; flags++) {
                        if(strchr("hex", tolower(*flags))) {
                            hex = 1;
                        } else if(strchr("dump", tolower(*flags))) {
                            hex = -1;
                        } else if(strchr("c", tolower(*flags))) {
                            c_out = -1;
                        } else {
                            if(strchr(flags, ' ')) space = 1;
                            t = get_var_from_name(flags, p - flags);
                            if(t >= 0) {
                                force_len = get_var32(t);
                            } else {
                                if(sscanf(flags, "%d%n", &force_len, &len) == 1) {
                                    flags += (len - 1); // due to flags++
                                }
                            }
                        }
                    }
                }
                var = get_var(idx);
                len = strlen(var);
                if(force_len > 0) {
                    len = variable[idx].size;
                    if(force_len < len) len = force_len;
                }
                if(hex > 0) {
                    if(variable[idx].isnum) {
                        fprintf(fdo, "0x%"PRIx, get_var32(idx));
                    } else {
                        while(len--) {
                            fprintf(fdo, "%02x%s", *var, space ? " " : "");
                            var++;
                        }
                    }
                } else if(hex < 0) {
                    fputc('\n', fdo);
                    show_dump(2, var, len, fdo);
                    var += len;
                } else if(c_out) {
                    fputc('\"', fdo);
                    while(len--) {
                        fprintf(fdo, "\\x%02x", *var);
                        var++;
                    }
                    fputc('\"', fdo);
                } else {
                    fwrite(var, 1, len, fdo);
                }
                msg = p + 1;
            } else {
                if(*msg == '\n') {
                    msg++;
                    break;
                }
                fputc(*msg, fdo);
                msg++;
            }
        }
        fputc('\n', fdo);
    }
    fprintf(stderr, "\n");
    return(0);
}



int CMD_GetArray_func(int cmd) {
    array_t *ar;
    int     n,
            idx,
            array_num;

    n         = CMD.var[0];
    array_num = VAR32(1);
    idx       = VAR32(2);

    if((array_num < 0) || (array_num >= MAX_ARRAYS)) {
        fprintf(stderr, "\nError: the script uses more arrays (%"PRId") than supported (%d)\n", array_num, MAX_ARRAYS);
        myexit(QUICKBMS_ERROR_BMS);
    }
    ar = &array[array_num];

    if((idx < 0) || (idx >= ar->elements)) {
        fprintf(stderr, "\nError: the script uses more array indexes (%"PRId") than supported (%"PRId")\n", idx, ar->elements);
        myexit(QUICKBMS_ERROR_BMS);
    }

    check_variable_errors(idx, &ar->var[idx]);
    variable_copy(&variable[n], &ar->var[idx], 1);

    if(verbose < 0) {
        if(variable[n].isnum) printf(". %08x getarr  %-10s 0x%"PRIx" %d:%d\n", 0, get_varname(n), get_var32(n), (i32)array_num, (i32)idx);
        else                  printf(". %08x getarr  %-10s \"%s\" %d:%d\n",    0, get_varname(n), get_var(n),   (i32)array_num, (i32)idx);
    }
    return(0);
}



int CMD_PutArray_func(int cmd) {
    static const int    PutArray_func_blocks = 4096;
    array_t *ar;
    int     i,
            n,
            t,
            idx,
            array_num;

    array_num = VAR32(0);
    idx       = VAR32(1);
    n         = CMD.var[2];

    if((array_num < 0) || (array_num >= MAX_ARRAYS)) {
        fprintf(stderr, "\nError: the script uses more arrays (%"PRId") than supported (%d)\n", array_num, MAX_ARRAYS);
        myexit(QUICKBMS_ERROR_BMS);
    }
    ar = &array[array_num];

    if((idx < 0) /*|| (idx >= ar->elements)*/) {
        fprintf(stderr, "\nError: the script uses negative array indexes (%"PRId")\n", idx);
        myexit(QUICKBMS_ERROR_BMS);
    }

    // in future I will probably use linked lists but they are not a good idea for xalloc 

    if(idx >= ar->elements) {
        if(idx >= ar->allocated_elements) {
            t = (idx + PutArray_func_blocks + 1) * sizeof(variable_t);   // +1 is necessary?
            if(t < idx) ALLOC_ERR;
            ar->var = realloc(ar->var, t);
            if(!ar->var) STD_ERR(QUICKBMS_ERROR_MEMORY);
            ar->allocated_elements += PutArray_func_blocks;

            // lame, after realloc we need to restore the links to the static variables
            for(i = 0; i < ar->elements; i++) {
                if(ar->var[i].name  && (ar->var[i].name  != ar->var[i].name_alloc))  ar->var[i].name  = ar->var[i].name_static;
                if(ar->var[i].value && (ar->var[i].value != ar->var[i].value_alloc)) ar->var[i].value = ar->var[i].value_static;
            }
        }

        for(i = ar->elements; i <= idx; i++) {   // <= remember!!! (example 0 and 0)
            // no need to use FREEZ_VAR(&(ar->var[i])); because these arrays are not allocated
            memset(&(ar->var[i]), 0, sizeof(variable_t));
        }

        ar->elements = idx + 1;
    }

    variable_copy(&(ar->var[idx]), &variable[n], 1);

    if(verbose < 0) {
        if(variable[n].isnum) printf(". %08x putarr  %-10s 0x%"PRIx" %d:%d\n", 0, get_varname(n), get_var32(n), (i32)array_num, (i32)idx);
        else                  printf(". %08x putarr  %-10s \"%s\" %d:%d\n",    0, get_varname(n), get_var(n),   (i32)array_num, (i32)idx);
    }
    return(0);
}



void SortArray_swap(int X, int i, int j) {
    variable_t  tmp;

    memcpy(&tmp,             &array[X].var[i], sizeof(variable_t));
    memcpy(&array[X].var[i], &array[X].var[j], sizeof(variable_t));
    memcpy(&array[X].var[j], &tmp,             sizeof(variable_t));
    if(array[X].var[i].name  && (array[X].var[i].name  != array[X].var[i].name_alloc))  array[X].var[i].name  = array[X].var[i].name_static;
    if(array[X].var[i].value && (array[X].var[i].value != array[X].var[i].value_alloc)) array[X].var[i].value = array[X].var[i].value_static;
    if(array[X].var[j].name  && (array[X].var[j].name  != array[X].var[j].name_alloc))  array[X].var[j].name  = array[X].var[j].name_static;
    if(array[X].var[j].value && (array[X].var[j].value != array[X].var[j].value_alloc)) array[X].var[j].value = array[X].var[j].value_static;
}



int CMD_SortArray_func(int cmd) {
    array_t *ar;
    int     i,
            j,
            x,
            all,
            elements,
            array_num;
    u8      *a,
            *b;

    array_num = VAR32(0);
    if(CMD.var[1] >= 0) all = VAR32(1);
    else                all = 0;

    if((array_num < 0) || (array_num >= MAX_ARRAYS)) {
        fprintf(stderr, "\nError: this BMS script uses more arrays than how much supported\n");
        myexit(QUICKBMS_ERROR_BMS);
    }

    ar = &array[array_num];
    elements = ar->elements;

    for(i = 0; i < elements; i++) {
        check_variable_errors(i, &ar->var[i]);
    }

    for(i = 0; i < (elements - 1); i++) {
        for(j = i + 1; j < elements; j++) {
            a = ar->var[j].value; if(!a) a = ar->var[j].name;
            b = ar->var[i].value; if(!b) b = ar->var[i].name;

            if(!ar->var[i].isnum && !ar->var[j].isnum) {
                //x = MIN(ar->var[j].size, ar->var[i].size);
                //if(ar->var[j].size >= ar->var[i].size) continue;
                if(stricmp(a, b) >= 0) continue;

            } else if(ar->var[i].isnum && !ar->var[j].isnum) {
                if(stricmp(a, myitoa(ar->var[i].value32)) >= 0) continue;

            } else if(!ar->var[i].isnum && ar->var[j].isnum) {
                if(stricmp(myitoa(ar->var[j].value32), b) >= 0) continue;

            } else {
                if(ar->var[j].value32 >= ar->var[i].value32) continue;
            }
            SortArray_swap(array_num, i, j);
            if(all) {
                for(x = 0; x < MAX_ARRAYS; x++) {
                    if(x == array_num) continue;                // already sorted array
                    if(array[x].elements <= 0) continue;        // empty
                    if(elements != array[x].elements) continue; // different number of elements
                    SortArray_swap(x, i, j);
                }
            }
        }
    }
    return(0);
}



int CMD_Function_func(int start_cmd, int nop, int *ret_break) {
    variable_t  *newvar = NULL,
                *oldvar = NULL;
    int     ret,
            cmd,
            i,
            n,
            argc,
            keep_vars,
            variables;
    u8      *func_name,
            *tmp;

    cmd = start_cmd;

    if(CMD.type != CMD_CallFunction) {   // quick skip
        for(cmd++; CMD.type != CMD_NONE; cmd++) {
            if(CMD.type == CMD_EndFunction) return(cmd);
            if(CMD.type == CMD_StartFunction) break;
        }
        fprintf(stderr, "\nError: no EndFunction command found\n");
        myexit(QUICKBMS_ERROR_BMS);
    }
    if(nop) return(start_cmd);

    func_name = STR(0);
    keep_vars = NUM(1);

    // explanation: we must replicate the whole variable database otherwise there are huge problems
    variables = MAX_VARS;
    //for(variables = 0; variable[variables].name; variables++);

    if(!keep_vars) {
        newvar = calloc(variables + 1, sizeof(variable_t));
        if(!newvar) STD_ERR(QUICKBMS_ERROR_MEMORY);  // calloc is better so it zeroes also the last variable automatically
        for(i = 0; i < variables; i++) {    // duplicate the strings, the first NULL in re_strdup is NECESSARY!!!
            if(!variable[i].name) break;
            variable_copy(&newvar[i], &variable[i], 0);
        }
        oldvar   = variable;
        variable = newvar;
    }

    argc = NUM(0);
    if(argc < 0) argc = 0;
    if(argc >= MAX_ARGS) argc = MAX_ARGS;
    tmp = NULL;

    // COPY THIS TO run_exe IF YOU MODIFY IT!
    for(i = 0; i < argc; i++) { // wow, looks chaotic?
        if(!tmp) {
            tmp = malloc(strlen(func_name) + 4 + 32 + 1);
            if(!tmp) STD_ERR(QUICKBMS_ERROR_MEMORY);
        }
        sprintf(tmp, "%s_arg%d", func_name, (i32)i + 1);

        n = CMD.var[2 + i];
        if(n < 0) {    // MEMORY_FILE
            n = -n;
            add_var(0, tmp, (void *)memory_file[n].data + memory_file[n].pos, 0, memory_file[n].size - memory_file[n].pos);
        } else {
            if(var_is_a_string(n)) {
                add_var(0, tmp, get_var(n), 0, -1);
            } else {
                if(CMD.num[2 + i]) {    // &var
                    add_var(0, tmp, NULL, variable[n].value32, -1);
                } else {
                    add_var(0, tmp, NULL, get_var32(n), -1);
                }
            }
        }
    }
    FREEZ(tmp)

    for(cmd = 0;; cmd++) {
        if(CMD.type == CMD_NONE) {
            fprintf(stderr, "\nError: the function %s has not been found\n", func_name);
            myexit(QUICKBMS_ERROR_BMS);
        }
        if((CMD.type == CMD_StartFunction) && !stricmp(func_name, STR(0))) break;
    }

    ret = start_bms(cmd + 1, nop, ret_break);

    if(!keep_vars) {
        for(i = 0; i < variables; i++) {
            FREEZ_VAR(&newvar[i]);
        }
        FREEZ(newvar)
        variable = oldvar;
    }

    if(ret < 0) return(ret);
    return(start_cmd);
}



#ifdef WIN32
int run_windows_exe(char *buff) {
	PROCESS_INFORMATION pi;
	STARTUPINFO         si;
    GetStartupInfo(&si);
    if(!CreateProcess(NULL, buff, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) return(-1);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
#endif



int run_exe(int cmd, int argc, void *args[]) {
    static u8   *buff = NULL;
    int     i,
            n,
            len;
    u8      tmp_num[NUMBERSZ + 1],
            *p;

    len = 0;
    for(i = 0; i < argc; i++) { // wow, looks chaotic?
        n = CMD.var[4 + i];
        if(n < 0) {    // MEMORY_FILE
            p = args[i];
        } else {
            if(var_is_a_string(n)) {
                p = args[i];
            } else {
                sprintf(tmp_num, "%"PRId, (int)(args[i]));
                p = tmp_num;
            }
        }

        buff = realloc(buff, len + (1 + 1 + strlen(p) + 1) + 1);
        if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
        if(i) buff[len++] = ' ';
        len += sprintf(buff + len, "\"%s\"", p);
    }

#ifdef WIN32
    if(run_windows_exe(buff) < 0) return -1;
#else
    buff = realloc(buff, len + 1 + 1 + 1);
    if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
    len += sprintf(buff + len, " &");
    if(system(buff) < 0) return -1;
#endif
    return 0;
}



#include "calling_conventions.h"



void *calldll_alloc(u8 *dump, u32 dumpsz, u32 argc, ...) {
    static int page_size = 0;
    int     alloc_size;
    void    *ret;
    va_list ap;

    if(!page_size) {
        #ifdef WIN32
        SYSTEM_INFO sSysInfo;
        GetSystemInfo(&sSysInfo);
        page_size = sSysInfo.dwPageSize;
        #else
        page_size = getpagesize();
        #endif
        if(!page_size) page_size = 4096;
    }

    if(!dump) return(NULL);
    if(dumpsz < 0) return(NULL);
    alloc_size = (dumpsz + (page_size - 1)) & (~(page_size - 1));
    if(alloc_size < dumpsz) return(NULL);

    #ifdef WIN32
    ret = VirtualAlloc(
        NULL,
        alloc_size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);
    if(!ret) return(NULL);
    #else
    ret = valloc(alloc_size);
    if(!ret) return(NULL);
    mprotect(
        ret,
        alloc_size,
        PROT_READ | PROT_WRITE);
    #endif

    memset(ret, 0xcc, alloc_size);
    memcpy(ret, dump, dumpsz);

    u32 i, offset;
    void *func;
    va_start(ap, argc);
    for(i = 0; i < argc; i++) {
        offset = va_arg(ap, long);
        func   = va_arg(ap, void *);
        ((u8 *)ret)[offset] = 0xe9;
        ((u32 *)(((u8 *)ret) + offset + 1))[0] = (u32)(func - (void *)(((u8 *)ret) + offset + 5));
    }
    va_end(ap);

    #ifdef WIN32
    DWORD tmp;
    VirtualProtect(ret, alloc_size, PAGE_EXECUTE_READ, &tmp);
    #else
    mprotect(ret, alloc_size, PROT_EXEC | PROT_READ);
    #endif
    return(ret);
}



int CMD_CallDLL_quick_check_memory(void *addr) {
    int     ret = 0;
    if(!addr) return(-1);
#ifdef WIN32
    DWORD   len;
    u8      buff[1];
    if(!ReadProcessMemory(
        GetCurrentProcess(),
        addr,
        buff,
        1,
        &len)) ret = -1;
    if(len != 1) ret = -1;
#endif
    return(ret);
}



#define MAX_DLLS        32
#define MAX_DLL_FUNCS   64
typedef struct {
    u8      *name;
    int     off;
    void    *addr;
} calldllfunc_t;
typedef struct {
    u8      *name;
    HMODULE hlib;
    u8      is_exe;
    u8      is_lib;
    u8      is_mem;
    u32     crc;    // crc for memory files, in this case a 32bit crc is enough
    calldllfunc_t   func[MAX_DLL_FUNCS];
} calldll_t;

int CMD_CallDLL_func(int cmd, u8 *input, u8 *output) {
    static  calldll_t   dll[MAX_DLLS] = {{NULL,NULL,0,0,0,0,{{NULL,0,NULL}}}};  // cache for multiple dlls/funcs

    //static u8   fulldlldir[PATHSZ + 1]; // used only here so don't waste the stack
    static u8   *fulldlldir = NULL;

    HMODULE hlib = NULL;
    void    *args[MAX_ARGS];
    void    *funcaddr   = NULL;
    int     funcoff     = 0,
            argc,
            di,
            dj,
            i,
            n,
            ret;
    u32     crc         = 0;
    u8      *dllname,
            *callconv,
            *funcname   = NULL,
            *p,
            *mypath     = NULL,
            is_exe      = 0,
            is_lib      = 0,    // alternative of is_dat
            is_mem      = 0;    // remember to replicate in calldll_t!

    if(cmd < 0) {   // useless
        memset(dll, 0, sizeof(dll));
        return(-1);
    }

    dllname     = STR(0);
    funcname    = STR(1);
    if(myisdigitstr(funcname)) {
        funcoff  = myatoi(funcname);
        funcname = NULL;
    }
    callconv    = STR(2);

    if(!strnicmp(dllname, MEMORY_FNAME, MEMORY_FNAMESZ)) {
        is_mem = 1;
    } else {
        dllname = get_filename(dllname);
        p = strrchr(dllname, '.');
        if(p && !stricmp(p, ".exe")) is_exe = 1;    // compiling with "-Wl,--image-base=0x8000000" is not much useful
        if(p && (!stricmp(p, ".exe") || !stricmp(p, ".dll") || !stricmp(p, ".so"))) is_lib = 1; // the others are handled as raw functions
    }

    if(is_mem) {
        crc = mycrc(memory_file[-get_memory_file(dllname)].data, memory_file[-get_memory_file(dllname)].size);
    }
    for(di = 0; di < MAX_DLLS; di++) {
        if(!dll[di].name) continue;
        if(stricmp(dllname, dll[di].name)) continue;
        if(is_mem) {
            // do not free the dll, maybe it's used in another part of the script
            if(dll[di].crc != crc) continue;
        }
        hlib   = dll[di].hlib;
        is_exe = dll[di].is_exe;
        is_lib = dll[di].is_lib;
        is_mem = dll[di].is_mem;
        for(dj = 0; dj < MAX_DLL_FUNCS; dj++) {
            if(!dll[di].func[dj].addr) continue;
            if(funcname) {
                if(!dll[di].func[dj].name) continue;
                if(stricmp(funcname, dll[di].func[dj].name)) continue;
            } else {
                if(funcoff != dll[di].func[dj].off) continue;
            }
            funcaddr = dll[di].func[dj].addr;
            break;
        }
        break;
    }

    if(!hlib) {
        if(!enable_calldll) {
            fprintf(stderr, "\n"
                "- the script has requested to load a function from the dll\n"
                "  %s\n"
                "%s"
                "  do you want to continue (y/N)? ",
                dllname, is_exe ? "- also note that it's an executable so its working is not guarantee\n" : "");
            if(get_yesno(NULL) != 'y') myexit(QUICKBMS_ERROR_USER);
        }

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
            if(!mypath) break;
            spr(&fulldlldir, "%s%c%s", mypath, PATHSLASH, dllname);
            if(is_lib) {
                hlib = LOADDLL(fulldlldir);
                /* not, because this is not is_lib since it's related to the extension
                if(!hlib && !strchr(dllname, '.')) {
                    spr(&fulldlldir, "%s%c%s.%s", mypath, PATHSLASH, dllname,
                    #ifdef WIN32
                    "dll"
                    #else
                    "so"
                    #endif
                    );
                    hlib = LOADDLL(fulldlldir);
                }
                */
            } else if(is_mem) {
                hlib = calldll_alloc(   // needed for DEP!
                    memory_file[-get_memory_file(dllname)].data,
                    memory_file[-get_memory_file(dllname)].size,
                    0);
#ifdef WIN32
                if(hlib && !memcmp(hlib, "MZ", 2)) {
                    hlib = (void *)MemoryLoadLibrary((void *)hlib);

                    // try the classical way in case of problems
                    if(!hlib) {
                        fprintf(stderr, "\n- MemoryLoadLibrary failed, dump the memory dll on the disk\n");
                        p = NULL;
                        quickbms_tmpname(&p, NULL, "dll");
                        mystrcpy(fulldlldir, p, PATHSZ);
                        FREEZ(p);
                        mydump(fulldlldir, memory_file[-get_memory_file(dllname)].data, memory_file[-get_memory_file(dllname)].size);
                        hlib = LOADDLL(fulldlldir);
                        is_mem = 0;
                    }
                    is_lib = 1;
                }
#endif

            } else {
                p = (void *)fdload(fulldlldir, &ret);
                if(p) {
                    hlib = calldll_alloc(p, ret, 0);   // needed for DEP!
                    free(p);
                }
            }
            if(hlib) break;
        }
        if(!hlib) {
            fprintf(stderr, "\nError: file %s has not been found or cannot be loaded\n", dllname);
            myexit(QUICKBMS_ERROR_DLL);
        }
        fprintf(stderr, "- library %s loaded at address %p\n", dllname, hlib);

        for(di = 0; di < MAX_DLLS; di++) {
            if(!dll[di].hlib) break;
        }
        if(di >= MAX_DLLS) {
            fprintf(stderr, "\nError: is not possible to use additional dlls or functions\n");
            myexit(QUICKBMS_ERROR_DLL);
        }
        mystrdup(&dll[di].name, dllname);
        dll[di].hlib   = hlib;
        dll[di].is_exe = is_exe;
        dll[di].is_lib = is_lib;
        dll[di].is_mem = is_mem;
        if(is_mem) dll[di].crc = crc;
        else       dll[di].crc = 0;
    }
    if(!funcaddr) {
        if(funcname) {
            if(is_lib) {
#ifdef WIN32
                if(is_mem) {
                    funcaddr = (void *)MemoryGetProcAddress(hlib, funcname);
                } else
#endif
                    funcaddr = GETFUNC(funcname);

                if(!funcaddr) {

                    if(strrchr(funcname, '@')) {
                        // stdcall without @
                        p = mystrdup_simple(funcname);
                        strrchr(p, '@')[0] = 0;
                    } else {
                        // stdcall
                        p = NULL;
                        spr(&p, "%s@%d", funcname, NUM(0) * sizeof(void *));
                    }
#ifdef WIN32
                    if(is_mem) {
                        funcaddr = (void *)MemoryGetProcAddress(hlib, p);
                    } else
#endif
                        funcaddr = GETFUNC(p);
                    free(p);

                    if(!funcaddr) {
                        quick_var_from_name_check(&funcname, NULL);
                        funcaddr = (void *)myatoi(funcname);
                        funcname = NULL;
                        funcoff  = (void *)funcaddr - (void *)hlib;
                    }
                }
            } else {
                fprintf(stderr, "\nError: the input library is handled as raw data so can't have a function name\n");
                myexit(QUICKBMS_ERROR_DLL);
            }
        } else {
            funcaddr = (void *)((u8 *)(hlib) + funcoff);
        }
        /* demangling? maybe in future
#ifdef WIN32
        if(!funcaddr && funcname && is_mem) {
            p = mymangle(funcname);
            if(p) {
                funcaddr = (void *)MemoryGetProcAddress(hlib, p);
                free(p);
            }
        }
#endif
        */
        if(!funcaddr) {
            fprintf(stderr, "\nError: function not found\n");
            myexit(QUICKBMS_ERROR_DLL);
        }
        fprintf(stderr, "- function found at offset %p\n", funcaddr);

        for(dj = 0; dj < MAX_DLLS; dj++) {
            if(!dll[di].func[dj].addr) break;
        }
        if(dj >= MAX_DLL_FUNCS) {
            fprintf(stderr, "\nError: is not possible to use additional dlls or functions\n");
            myexit(QUICKBMS_ERROR_DLL);
        }
        if(funcname) {
            mystrdup(&dll[di].func[dj].name, funcname);
            dll[di].func[dj].off  = 0;
        } else {
            dll[di].func[dj].name = NULL;
            dll[di].func[dj].off  = funcoff;
        }
        dll[di].func[dj].addr = funcaddr;
    }

    argc = NUM(0);
    if(argc < 0) argc = 0;
    if(argc >= MAX_ARGS) argc = MAX_ARGS;
    memset(&args, 0, sizeof(args));

    // COPY THIS TO run_exe IF YOU MODIFY IT!
    for(i = 0; i < argc; i++) { // wow, looks chaotic?
        n = CMD.var[4 + i];
        if(n < 0) {    // MEMORY_FILE
            n = -n;
            args[i] = (void *)memory_file[n].data + memory_file[n].pos;
        } else {
            if(var_is_a_string(n)) {
                args[i] = (void *)get_var(n);
            } else {
                if(CMD.num[4 + i]) {    // &var
                    args[i] = (void *)&variable[n].value32;
                } else {
                    args[i] = (void *)get_var32(n);
                }
            }
        }
    }

    // #INPUT# #OUTPUT#
    if(input || output) {
        for(i = 0; i < argc; i++) { // wow, looks chaotic?
            n = CMD.var[4 + i];
            if(!stricmp(get_varname(n), "#INPUT#"))  {
                args[i] = input;
            } else if(!stricmp(get_varname(n), "#OUTPUT#")) {
                args[i] = output;
            }
        }
    }

#define CALLDLL_DO_INT3 \
        set_int3(dllname, hlib, funcname, funcaddr, argc);

    // horrible? yes, but avoids asm work-arounds and works perfectly
    // note that the arguments of the function prototypes don't seem necessary
    // but I have decided to leave them for maximum compatibility
#define CALLDLL_FUNC(X) \
        switch(argc) { \
            case 0: { \
                __##X int (*function0_##X)(void) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function0_##X(); \
                break; } \
            case 1: { \
                __##X int (*function1_##X)(void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function1_##X(args[0]); \
                break; } \
            case 2: { \
                __##X int (*function2_##X)(void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function2_##X(args[0], args[1]); \
                break; } \
            case 3: { \
                __##X int (*function3_##X)(void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function3_##X(args[0], args[1], args[2]); \
                break; } \
            case 4: { \
                __##X int (*function4_##X)(void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function4_##X(args[0], args[1], args[2], args[3]); \
                break; } \
            case 5: { \
                __##X int (*function5_##X)(void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function5_##X(args[0], args[1], args[2], args[3], args[4]); \
                break; } \
            case 6: { \
                __##X int (*function6_##X)(void*,void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function6_##X(args[0], args[1], args[2], args[3], args[4], args[5]); \
                break; } \
            case 7: { \
                __##X int (*function7_##X)(void*,void*,void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function7_##X(args[0], args[1], args[2], args[3], args[4], args[5], args[6]); \
                break; } \
            case 8: { \
                __##X int (*function8_##X)(void*,void*,void*,void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function8_##X(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); \
                break; } \
            case 9: { \
                __##X int (*function9_##X)(void*,void*,void*,void*,void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function9_##X(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]); \
                break; } \
            case 10: { \
                __##X int (*function10_##X)(void*,void*,void*,void*,void*,void*,void*,void*,void*,void*) = (void *)funcaddr; \
                CALLDLL_DO_INT3 \
                ret = function10_##X(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]); \
                break; } \
            default: { \
                fprintf(stderr, "\nError: this tool doesn't support all these arguments for the dll functions ("#X")\n"); \
                myexit(QUICKBMS_ERROR_DLL); \
            } \
        }
#define CALLDLL_FUNC2(X) \
        switch(argc) { \
            case 0:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc); break; \
            case 1:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0]); break; \
            case 2:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1]); break; \
            case 3:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2]); break; \
            case 4:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3]); break; \
            case 5:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4]); break; \
            case 6:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4], args[5]); break; \
            case 7:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4], args[5], args[6]); break; \
            case 8:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]); break; \
            case 9:  CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]); break; \
            case 10: CALLDLL_DO_INT3    ret = X##_call(funcaddr, argc, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]); break; \
            default: { \
                fprintf(stderr, "\nError: this tool doesn't support all these arguments for the dll functions ("#X")\n"); \
                myexit(QUICKBMS_ERROR_DLL); \
            } \
        }

    ret = 0;
    if(!callconv[0] || stristr(callconv, "stdcall") || stristr(callconv, "winapi")) {
        CALLDLL_FUNC(stdcall)
    } else if(stristr(callconv, "cdecl")) { // thiscall on gcc
        CALLDLL_FUNC(cdecl)
#if defined(i386) || defined(IA64)
    } else if(stristr(callconv, "thiscall")) {
        CALLDLL_FUNC2(thiscall)
    } else if(stristr(callconv, "fastcall") || stristr(callconv, "msfastcall")) {
#ifdef __fastcall
        CALLDLL_FUNC(fastcall)
#else
        CALLDLL_FUNC2(msfastcall)
#endif
    } else if(stristr(callconv, "borland") || stristr(callconv, "delphi") || stristr(callconv, "register")) {
        CALLDLL_FUNC2(borland)
    } else if(stristr(callconv, "watcom")) {
        CALLDLL_FUNC2(watcom)
    } else if(stristr(callconv, "pascal")) {
        CALLDLL_FUNC2(pascal)
    } else if(stristr(callconv, "safecall")) {
        CALLDLL_FUNC2(safecall)
    } else if(stristr(callconv, "syscall") || stristr(callconv, "OS/2")) {
        CALLDLL_FUNC2(syscall)
    } else if(stristr(callconv, "optlink") || stristr(callconv, "VisualAge")) {
        CALLDLL_FUNC2(optlink)
    } else if(stristr(callconv, "clarion") || stristr(callconv, "TopSpeed") || stristr(callconv, "JPI")) {
        CALLDLL_FUNC2(clarion)
    } else if(stristr(callconv, "usercall")) {
        CALLDLL_FUNC2(usercall)
#endif
    /*
    not implemented at the moment because it's a work-in-progress
    } else if(stristr(callconv, "exe")) {
        ret = run_exe(cmd, argc, args);
    */
    } else {
        fprintf(stderr, "\nError: calling convention %s not supported\n", callconv);
        myexit(QUICKBMS_ERROR_DLL);
    }

    if(!variable[CMD.var[3]].constant) {
        if(CMD.num[3] && !CMD_CallDLL_quick_check_memory((void *)ret)) {
            add_var(CMD.var[3], NULL, (void *)ret, 0, -1);  // string only!
        } else {
            add_var(CMD.var[3], NULL, NULL, ret, sizeof(int));
        }
    }

    //CLOSEDLL; // never call it!
    return(0);
}



int CMD_ScanDir_func(int cmd) {
    static u8   filedir[PATHSZ + 1] = "";
    static int  total_files         = -1;
    static int  curr_file           = 0;
    static files_t *files           = NULL;
    int     i;
    u8      *path;

    path    = VAR(0);
    if(!path) return(-1);
    if(!filedir[0]) {
        //if(strcmp(path, ".")) {
            //printf("\nError: at the moment the ScanDir function accepts only the \".\" as scan path\n");
            //myexit(QUICKBMS_ERROR_BMS);
        //}
        mystrcpy(filedir, path, PATHSZ);
        recursive_dir(filedir, PATHSZ);
        files = add_files(NULL, 0, &total_files);
        curr_file = 0;
    }
    if(curr_file < total_files) {
        add_var(CMD.var[1], NULL, files[curr_file].name, 0, -1);
        add_var(CMD.var[2], NULL, NULL, files[curr_file].size, sizeof(int));
        curr_file++;
    } else {
        add_var(CMD.var[1], NULL, "", 0, -1);
        add_var(CMD.var[2], NULL, NULL, -1, sizeof(int));
        if(files) {
            for(i = 0; i < total_files; i++) {
                FREEZ(files[i].name)
            }
            FREEZ(files)
        }
        filedir[0] = 0;
        total_files = -1;
    }
    return(0);
}



int CMD_Put_func(int cmd) {
    int     fd,
            type;

    fd   = FILEZ(2);
    type = NUM(1);
    if(verbose < 0) {   // show only the numbers for the moment
        if(variable[CMD.var[0]].isnum) {
            verbose_print(myftell(fd), "put", CMD.var[0], NULL, 0, VAR32(0), type);
        } else {
            verbose_print(myftell(fd), "put", CMD.var[0], VAR(0), -1, VAR32(0), type);  // VARSZ(0)
        }
    }
    if(myfwx(fd, CMD.var[0], type) < 0) return(-1);
    return(0);
}



int CMD_PutDString_func(int cmd) {
    static int  buffsz  = 0;
    static u8   *buff   = NULL;
    int     fd,
            size,
            datasz;
    u8      *data;

    fd   = FILEZ(2);
    size = VAR32(1);
    if(size == -1) ALLOC_ERR;
    data   = VAR(0);
    //datasz = 0;
    //if(data) datasz = strlen(data);
    //if(datasz > size) datasz = size;
    datasz = VARSZ(0);
    if(size < datasz) datasz = size;

    // alternative method (simpler but uses full allocated buff)
    //myalloc(&buff, size, &buffsz);
    //memcpy(buff, data, datasz);
    //if(size > datasz) memset(buff + datasz, 0, size - datasz);
    //if(myfw(fd, buff, size) < 0) return(-1);

    if(verbose < 0) verbose_print(myftell(fd), "putdstr", CMD.var[0], data, datasz, 0, datasz);
    if(myfw(fd, data, datasz) < 0) return(-1);
    if(size > datasz) { // fill with zeroes, I avoided to use myfputc(0x00, fd);
        size -= datasz;
        myalloc(&buff, size, &buffsz);
        memset(buff, 0, size);
        if(myfw(fd, buff, size) < 0) return(-1);
    }
    return(0);
}



int CMD_PutCT_func(int cmd) {
    int     fd;

    fd = FILEZ(3);
    //if(NUM(1) < 0) {
        // ok
    //} else {
        //printf("\nError: PutCT is supported only with String type\n");
        //myexit(QUICKBMS_ERROR_BMS);
    //}
    if(verbose < 0) verbose_print(myftell(fd), "putct", CMD.var[0], VAR(0), VAR32(2), 0, VAR32(2));
    if(fputss(fd, VAR(0), VAR32(2), (NUM(1) == TYPE_UNICODE) ? 1 : 0, 0, VARSZ(0)) < 0) return(-1);
    return(0);
}

