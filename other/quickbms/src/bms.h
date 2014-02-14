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

// QuickBMS script reading and parsing operations



// the rule is simple: start_bms is executed for EACH recursive command like do, for, if
int start_bms(int startcmd, int nop, int *ret_break) {
#define NEW_START_BMS(B,X,Y) \
    cmd = B(X, Y, ret_break); \
    if(cmd < 0) goto quit_error; \
    if(*ret_break) { \
        *ret_break = 0; \
        nop = 1; \
    }

    int     cmd,
            tmp;
    u8      *error  = NULL;

    if(startcmd < 0) {
        cmd = 0;    // needed because it's the beginning
    } else {
        cmd = startcmd;
    }
    if(verbose > 0) printf("             .start_bms start: %d %d %d\n", (i32)startcmd, (i32)nop, (i32)*ret_break);
    for(; CMD.type != CMD_NONE; cmd++) {
        //if(verbose && CMD.debug_line) printf("\n%"PRIx" %s%s\n", myftell(filenumber[0]), CMD.debug_line, nop ? " (SKIP)" : "");
        if((verbose > 0) && CMD.debug_line && !nop) {
            printf("\n%"PRIx" %02x  %s\n",
                filenumber[0].fd ? myftell(0) : 0,
                CMD.type,
                CMD.debug_line);
        }

        if(enable_hexhtml) {
            hexhtml_idx  = CMD.var[0];
            hexhtml_name = NULL;
        }

        switch(CMD.type) {
            case CMD_For: {
                //if(nop) break;
                //if(verbose < 0) printf(".\n");  // useful
                NEW_START_BMS(start_bms, cmd + 1, nop)
                break;
            }
            case CMD_Next: {
                if(nop) goto quit;
                //if(verbose < 0) printf(".\n");  // useful done in CMD_Next_func
                if(CMD_Next_func(cmd) < 0) goto quit_error;
                if(startcmd >= 0) cmd = startcmd - 1;   // due to "cmd++"
                break;
            }
            case CMD_Prev: {
                if(nop) goto quit;
                //if(verbose < 0) printf(".\n");  // useful done in CMD_Prev_func
                if(CMD_Prev_func(cmd) < 0) goto quit_error;
                if(startcmd >= 0) cmd = startcmd - 1;   // due to "cmd++"
                break;
            }
            case CMD_ForTo: {
                if(nop) break;
                //if(verbose < 0) printf(".\n");  // useful
                if(check_condition(cmd, -1, NULL, -1) == FALSE) nop = 1;
                break;
            }
            case CMD_Get: {
                if(nop) break;
                if(CMD_Get_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_GetDString: {
                if(nop) break;
                if(CMD_GetDString_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_GoTo: {
                if(nop) break;
                if(CMD_GoTo_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_IDString: {
                if(nop) break;
                if(CMD_IDString_func(cmd) < 0) {
                    error = "the signature doesn't match";        
                    goto quit_error;
                }
                break;
            }
            case CMD_Log: {
                if(nop) break;
                if(CMD_Log_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_CLog: {
                if(nop) break;
                if(CMD_CLog_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Math: {
                if(nop) break;
                if(CMD_Math_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_XMath: {
                if(nop) break;
                if(CMD_XMath_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_SavePos: {
                if(nop) break;
                if(CMD_SavePos_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Set: {
                if(nop) break;
                if(CMD_Set_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_String: {
                if(nop) break;
                if(CMD_String_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_If: {
                //if(nop) break;
                tmp = 0;
                do {
                    if(!tmp && (check_condition(cmd, -1, NULL, -1) == TRUE)) {
                        NEW_START_BMS(start_bms, cmd + 1, nop)
                        tmp = 1;
                    } else {
                        NEW_START_BMS(start_bms, cmd + 1, 1)
                    }
                } while(CMD.type != CMD_EndIf);
                break;
            }
            case CMD_Elif: {
                if(nop) goto quit;
                goto quit;
                break;
            }
            case CMD_Else: {
                if(nop) goto quit;
                goto quit;
                break;
            }
            case CMD_EndIf: {
                if(nop) goto quit;
                goto quit;
                break;
            }
            case CMD_GetCT: {
                if(nop) break;
                if(CMD_GetCT_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ComType: {
                if(nop) break;
                if(CMD_ComType_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Open: {
                if(nop) break;
                if(CMD_Open_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ReverseShort: {
                if(nop) break;
                if(CMD_ReverseShort_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ReverseLong: {
                if(nop) break;
                if(CMD_ReverseLong_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ReverseLongLong: {
                if(nop) break;
                if(CMD_ReverseLongLong_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Endian: {
                if(nop) break;
                switch(NUM(0)) {
                    case MYLITTLE_ENDIAN:   endian = MYLITTLE_ENDIAN;   break;
                    case MYBIG_ENDIAN:      endian = MYBIG_ENDIAN;      break;
                    default: {
                             if(endian == MYLITTLE_ENDIAN) endian = MYBIG_ENDIAN;
                        else if(endian == MYBIG_ENDIAN)    endian = MYLITTLE_ENDIAN;
                        // in case of other endianess in future... who knows
                        break;
                    }
                }
                break;
            }
            case CMD_FileXOR: {
                if(nop) break;
                if(CMD_FileXOR_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_FileRot13: {
                if(nop) break;
                if(CMD_FileRot13_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_FileCrypt: {
                if(nop) break;
                if(CMD_FileCrypt_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Break: {
                if(nop) break;  // like cleanexit, don't touch
                nop = 1;
                *ret_break = 1;
                break;
            }
            case CMD_GetVarChr: {
                if(nop) break;
                if(CMD_GetVarChr_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_PutVarChr: {
                if(nop) break;
                if(CMD_PutVarChr_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Append: {
                if(nop) break;
                append_mode = !append_mode;
                break;
            }
            case CMD_Encryption: {
                if(nop) break;
                if(CMD_Encryption_func(cmd, 0) < 0) goto quit_error;
                break;
            }
            case CMD_GetArray: {
                if(nop) break;
                if(CMD_GetArray_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_PutArray: {
                if(nop) break;
                if(CMD_PutArray_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_SortArray: {
                if(nop) break;
                if(CMD_SortArray_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_StartFunction: {
                //if(nop) break;
                NEW_START_BMS(CMD_Function_func, cmd, 1)
                break;
            }
            case CMD_CallFunction: {
                if(nop) break;
                if(verbose < 0) printf(".\n");  // useful
                NEW_START_BMS(CMD_Function_func, cmd, nop)
                break;
            }
            case CMD_EndFunction: {
                if(nop) goto quit;
                goto quit;
                break;
            }
            case CMD_Debug: {
                if(nop) break;
                //verbose = !verbose;
                if(CMD_Debug_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Padding: {
                if(nop) break;
                if(CMD_Padding_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ScanDir: {
                if(nop) break;
                if(CMD_ScanDir_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_CallDLL: {
                if(nop) break;
                if(CMD_CallDLL_func(cmd, NULL, NULL) < 0) goto quit_error;
                break;
            }
            case CMD_Put: {
                if(nop) break;
                if(CMD_Put_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_PutDString: {
                if(nop) break;
                if(CMD_PutDString_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_PutCT: {
                if(nop) break;
                if(CMD_PutCT_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Strlen: {
                if(nop) break;
                if(CMD_Strlen_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_Do: {
                //if(nop) break;
                if(verbose < 0) printf(".\n");  // useful
                NEW_START_BMS(start_bms, cmd + 1, nop)
                break;
            }
            case CMD_While: {
                if(nop) goto quit;
                if(check_condition(cmd, -1, NULL, -1) == FALSE) goto quit;
                if(startcmd >= 0) cmd = startcmd - 1;     // due to "cmd++"
                break;
            }
            case CMD_Print: {
                if(nop) break;
                if(CMD_Print_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_FindLoc: {
                if(nop) break;
                if(CMD_FindLoc_func(cmd) < 0) {
                    error = "the searched string has not been found";
                    goto quit_error;
                }
                break;
            }
            case CMD_GetBits: {
                if(nop) break;
                if(CMD_GetBits_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_PutBits: {
                if(nop) break;
                if(CMD_PutBits_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_ImpType: {
                if(nop) break;
                if(CMD_ImpType_func(cmd) < 0) goto quit_error;
                break;
            }
            case CMD_CleanExit: {
                if(nop) break;  // don't touch
                error = "invoked the termination of the extraction (CleanExit)";
                goto quit_error;
                break;
            }
            case CMD_NOP: {
                if(nop) break;
                // no operation, do nothing
                break;
            }
            default: {
                fprintf(stderr, "\nError: invalid command %d\n", (i32)CMD.type);
                myexit(QUICKBMS_ERROR_BMS);
                break;
            }
        }
    }
    return(-1); // CMD_NONE
quit_error:
    if(verbose > 0) printf("\nError: %s\n", error ? error : (u8 *)"something wrong during the extraction");
    //myexit(QUICKBMS_ERROR_BMS);
    return(-1);
quit:
    if(verbose > 0) printf("             .start_bms end: %d %d %d (ret %d)\n", (i32)startcmd, (i32)nop, (i32)*ret_break, (i32)cmd);
    return(cmd);
}



int bms_line(FILE *fd, u8 *input_line, u8 **argument, u8 **debug_line, u8 *ret_is_const) {
#define ARGS_DELIMITER  " \t" ",()"
    static  int wide_comment = 0;
    static  u8  crlf[2] = {0,0};    // used only for bms_line_number!
    static  int buffsz  = 0;
    static  u8  *buff   = NULL;
    static  u8  tmpchars[MAX_ARGS][NUMBERSZ + 1] = {""};
    static int  do_multiline = 0;
    static  u8  *multiline   = NULL;
    int     i,
            argi    = 0,
            j,
            c;
    u8      tmp[1 + 1],
            *line,
            *p,
            *s;

    if(!argument) {
        FREEZ(buff)
        buffsz = 0;
        memset(&crlf, 0, sizeof(crlf));
        return(-1);
    }

    if(!bms_line_number) wide_comment = 0;
    if(!bms_line_number) do_multiline = 0;

redo:
    //if(!input_line) return(0); NEVER
    do {
        bms_line_number++;
        for(i = 0;;) {
            if(fd) {
                c = fgetc(fd);
            } else {
                c = *input_line;
                input_line++;
                if(!c) c = -1;  // a buffer ends with 0
            }
            if(!c) continue;    // unicode blah, !i is used to handle only the first bytes 
            if((bms_line_number <= 1) && !i && ((c == 0xef) || (c == 0xbb) || (c == 0xbf) || (c == 0xfe) || (c == 0xff))) continue;
            if(c < 0) {
                if(!i) {    // end of file
                    bms_line_number = 0;
                    return(-1);
                }
                break;
            }
            if((c == '\n') || (c == '\r')) {
                if(!i) {    // used only for bms_line_number!
                    if(
                        (!crlf[0])
                     || ((crlf[0] == '\n') && (c == '\n'))
                     || ((crlf[0] == '\r') && (c == '\r'))
                    // || ((crlf[1] == '\r') && (crlf[0] == '\n') && (c == '\r'))
                     || ((crlf[1] == '\n') && (crlf[0] == '\r') && (c == '\n'))
                    ) {
                        bms_line_number++;
                    }
                    crlf[1] = crlf[0];
                    crlf[0] = c;
                    continue;
                }
                crlf[1] = 0;
                crlf[0] = c;
                break;
            }
            if(i >= buffsz) {
                //if((buffsz + STRINGSZ + 1) < buffsz) ALLOC_ERR;
                buffsz += STRINGSZ;
                buff = realloc(buff, buffsz + 1);
                if(!buff) STD_ERR(QUICKBMS_ERROR_MEMORY);
            }
            buff[i] = c;
            i++;
        }
        if(!buff) buff = malloc(1);
        buff[i] = 0;

        for(p = buff; *p && (*p != '\n') && (*p != '\r'); p++);
        *p = 0;

        if(do_multiline) {
            line = buff;
            goto goto_multiline;
        }

        if(fd) {
            for(p--; (p >= buff) && strchr(ARGS_DELIMITER ";", *p); p--);
            if(p >= buff) p[1] = 0;

            for(p = buff; *p && strchr(ARGS_DELIMITER "}", *p); p++);   // '}' is for C maniacs like me
            line = p;
            if((line[0] == '/') && (line[1] == '*')) {
                wide_comment = 1;
                break;
            }
            if((line[0] == '*') && (line[1] == '/')) {
                if(wide_comment) break;
            }
            if(!myisalnum(line[0])) line[0] = 0;  // so we avoids both invalid chars and comments like # ; // and so on
        } else {
            line = buff;
        }
    } while(!line[0]);

    if(debug_line) {
        *debug_line = realloc(*debug_line, 32 + strlen(line) + 1);
        if(!*debug_line) STD_ERR(QUICKBMS_ERROR_MEMORY);
        sprintf(*debug_line, "%-3d %s", (i32)bms_line_number, line);
        if(verbose > 0) printf("READLINE %s\n", *debug_line);
    }

    for(i = 0; i < MAX_ARGS; i++) { // reset all
        argument[i] = NULL;
        if(ret_is_const) ret_is_const[i] = 0;
    }

    if(wide_comment) {
        p = strstr(line, "*/");
        if(!p) {
            line[0] = 0;
        } else {
            p += 2;
            for(j = 0;; j++) {
                line[j] = p[j];
                if(!p[j]) break;
            }
            wide_comment = 0;
        }
    }

    for(argi = 0;;) {
        if(argi >= MAX_ARGS) {
            fprintf(stderr, "\nError: the BMS script uses more arguments than how much supported by this tool\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        for(p = line; *p && strchr(ARGS_DELIMITER, *p); p++);
        if(!*p) break;
        line = p;

        if((line[0] == '/') && (line[1] == '/')) break;
        if((line[0] == '/') && (line[1] == '*')) {
            wide_comment = 1;
            p = strstr(line + 2, "*/");
            if(!p) {
                break;
            } else {
                p += 2;
                for(j = 0;; j++) {
                    line[j] = p[j];
                    if(!p[j]) break;
                }
                wide_comment = 0;
            }
        }
        if(line[0] == '#') break;
        if(line[0] == ';') break;
        if(line[0] == '\'') {     // C char like 'A' or '\x41'
            line++;
            cstring(line, tmp, 1, &c);
            for(p = line + c; *p; p++) {
                if((p[0] == '\\') && (p[1] == '\'')) {
                    p++;
                    continue;
                }
                if(*p == '\'') break;
            }
            sprintf(tmpchars[argi], "0x%02x", tmp[0]);
            argument[argi] = tmpchars[argi];

        } else if(line[0] == '\"') {  // string
            line++;
            if(multiline) multiline[0] = 0;
goto_multiline:
            s = line;
            for(p = line;; p++) {
                c = *p; // needed only for this "if"
                if(!c || (c == '\"')) {
                    if(!c) do_multiline++;
                    if(do_multiline) {
                        *s = 0;
                        j = 0;
                        if(multiline) j = strlen(multiline);
                        multiline = realloc(multiline, j + 2 + strlen(line) + 1);
                        if(!multiline) STD_ERR(QUICKBMS_ERROR_MEMORY);
                        if(j) {
                            multiline[j++] = '\r';
                            multiline[j++] = '\n';
                        }
                        strcpy(multiline + j, line);
                    }
                    if(!c) goto redo;
                    break;
                }
                if((p[0] == '\\') && (p[1] == '\"')) {
                    p++;
                    *s++ = *p;
                    continue;
                }
                *s++ = *p;
            }
            if(s != p) *s = 0;
            if(do_multiline) argument[argi] = multiline;
            else             argument[argi] = line;
            do_multiline = 0;
            if(ret_is_const) ret_is_const[argi] = 1;

        } else {
            for(p = line; *p; p++) {
                if(strchr(ARGS_DELIMITER, *p)) break;
            }
            argument[argi] = line;
        }
        //if(p == line) break;  // this must be ignored otherwise "" is not handled

        c = *p;
        *p = 0;
        argument[argi] = mystrdup_simple(argument[argi]);   // create a new copy, introduced with multiline
        argi++;

        if(!c) break;
        line = p + 1;
    }
    argument[argi] = NULL;
    return(argi);
}



void set_quickbms_arg(u8 *quickbms_arg) {
    int     i,
            argc;
    u8      tmp[64],
            *argument[MAX_ARGS + 1] = { NULL };

    if(!quickbms_arg) return;
    if(verbose > 0) printf("- quickbms_arg:   %s\n", quickbms_arg);
    argc = bms_line(NULL, quickbms_arg, argument, NULL, NULL);
    for(i = 0; i < argc; i++) {
        sprintf(tmp, "quickbms_arg%d", (i32)i + 1);
        if(verbose > 0) printf("- %s:  %s\n", tmp, ARG[i]);
        add_var(0, tmp, ARG[i], 0, -1);
    }
}



// zlib + base64
u8 *type_decompress(u8 *str, int *ret_len) {
    int     len,
            tmp;
    i32     t32;
    u8      *ret;

    if(ret_len) *ret_len = 0;
    if(!str) goto quit;
    len = unbase64(str, -1, str, -1);   // use the same buffer
    if(len < 0) goto quit;  //return(str)
    tmp = 0;
    ret = NULL;
    t32 = tmp;
    len = unzip_dynamic(str, len, &ret, &t32);
    tmp = t32;
    if(len < 0) goto quit;  //return(str)
    if(ret_len) *ret_len = len;
    return(ret);
quit:
    fprintf(stderr, "\nError: failed Set type decompression, recheck your script\n");
    myexit(QUICKBMS_ERROR_BMS);
    return(NULL);
}



void c_struct_clean(u8 *str) {
    u8      *s;

    for(s = str + strlen(str) - 1; s >= str; s--) {
        if(!strchr(" \t;", *s)) break;  // example: int magic3; // asdf
        *s = 0;
    }
}



int c_structs(u8 *argument[MAX_ARGS + 1], int argc, int *ret_cmd) {
typedef struct {
    u8  *old;
    u8  *new;
} define_t;
    static u8       tmp[128] = "";
    static int      defines = 0;
    static define_t *define = NULL;

    int     i,
            cmd,
            type    = TYPE_NONE,
            array   = 0;
    u8      *arrayp = NULL,
            *put    = NULL,
            *p;

    printf("- c_structs:");
    for(i = 0; (i < MAX_ARGS) && argument[i]; i++) {
        c_struct_clean(argument[i]);
        printf(" \"%s\"", argument[i]);
    }
    printf("\n");

    if(argc <= 0) {
        ARG[0] = "NOP";
        goto quit;
    }

    while(
      !stricmp(ARG[0], "unsigned") ||
      !stricmp(ARG[0], "signed") ||
      !stricmp(ARG[0], "const") ||
      !stricmp(ARG[0], "static") ||
      !stricmp(ARG[0], "local") ||
      !stricmp(ARG[0], "global") ||
      !stricmp(ARG[0], "volatile") ||
        // ???
      !stricmp(ARG[0], "hexadecimal") ||
      !stricmp(ARG[0], "decimal") ||
      !stricmp(ARG[0], "octal") ||
      !stricmp(ARG[0], "read-only")
    ) {
        for(i = 1; i <= argc; i++) {
            ARG[i - 1] = ARG[i];
        }
        argc--;
    }

    if(!stricmp(ARG[1], "int")) {
        for(i = 2; i <= argc; i++) {
            ARG[i - 1] = ARG[i];
        }
        argc--;
    }

    if(!stricmp(ARG[1], "*")) {
        for(i = 2; i <= argc; i++) {
            ARG[i - 1] = ARG[i];
        }
        argc--;
        array = -1;
    }

    if(!stricmp(ARG[0], "struct") || !stricmp(ARG[1], "struct")) {
        ARG[0] = "NOP";
        argc = 0;
        goto quit;
    }

    if(!stricmp(ARG[0], "#define") || !stricmp(ARG[0], "define")) {
        i = (defines + 1) * sizeof(define_t);
        if(i < sizeof(define_t)) ALLOC_ERR;
        define = realloc(define, i);
        if(!define) STD_ERR(QUICKBMS_ERROR_MEMORY);
        mystrdup(&define[defines].new, ARG[1]);
        mystrdup(&define[defines].old, ARG[2]);
        defines++;
        ARG[0] = "NOP";
        argc = 0;
        goto quit;
    }

    if(!stricmp(ARG[0], "typedef")) {
        i = (defines + 1) * sizeof(define_t);
        if(i < sizeof(define_t)) ALLOC_ERR;
        define = realloc(define, i);
        if(!define) STD_ERR(QUICKBMS_ERROR_MEMORY);
        mystrdup(&define[defines].old, ARG[1]);
        mystrdup(&define[defines].new, ARG[2]);
        defines++;
        ARG[0] = "NOP";
        argc = 0;
        goto quit;
    }

    for(i = 0; i < defines; i++) {
        if(!stricmp(ARG[0], define[i].new)) {
            ARG[0] = define[i].old;
            break;
        }
    }

    p = strchr(ARG[0], '*');
    if(p) {
        *p = 0;
        array = -1;
    }

    p = strchr(ARG[1], '*');
    if(!p) p = strchr(ARG[0], '*');
    if(p) {
        p++;
        for(i = 0;; i++) {
            ARG[1][i] = p[i];
            if(!p[i]) break;
        }
        array = -1;
    }

    p = strchr(ARG[1], ':');
    if(p && myisdigit(p[1])) {
        *p++ = 0;
        array  = 1;
        arrayp = p; // bits
        ARG[0] = "bits";
    }

    p = strchr(ARG[1], '=');
    if(p && !strchr(ARG[1], '(')) {
        *p++ = 0;
        while(*p && (*p <= ' ')) p++;
        put = p;
        /*
        for(i = 0;; i++) {
            ARG[1][i] = p[i];
            if(!p[i]) break;
        }
        */
    }

    while(ARG[0][0] == '_') {
        p = ARG[0] + 1;
        for(i = 0;; i++) {
            ARG[0][i] = p[i];
            if(!p[i]) break;
        }
    }

    if(!strncmp(ARG[0], "LP", 2)) {          // LPVOID
        p = ARG[0] + 2;
        for(i = 0;; i++) {
            ARG[0][i] = p[i];
            if(!p[i]) break;
        }
        array = -1;
    } else if(!strncmp(ARG[0], "P", 1)) {    // PCHAR, PLONG
        p = ARG[0] + 1;
        for(i = 0;; i++) {
            ARG[0][i] = p[i];
            if(!p[i]) break;
        }
        array = -1;
    }

    i = 0;
    p = NULL;
    if((argc >= 2) && (ARG[2][0] == '[')) {
        p = ARG[2];
        i = 1;
    }
    if(!p) p = strchr(ARG[0], '[');
    if(!p) p = strchr(ARG[1], '[');
    if(p) {
        *p++ = 0;
        array  = 1;
        arrayp = p;
        while(*p) {
            if(*p == ']') {
                *p = 0;
                break;
            }
            p++;
        }
    }
    if(i) {
        for(i = 3; i <= argc; i++) {
            ARG[i - 1] = ARG[i];
        }
        argc--;
    }

    if((argc >= 2) && !stricmp(ARG[2], "=")) {
        put = ARG[3];
        /*
        for(i = 3; i <= argc; i++) {
            ARG[i - 2] = ARG[i];
        }
        */
        argc -= 2;
    }

    if(
        !stricmp(ARG[0], "8") ||
        !stricmp(ARG[0], "8bit") ||
        !stricmp(ARG[0], "byte") ||
        !stricmp(ARG[0], "ubyte") ||
        !stricmp(ARG[0], "char") ||
        !stricmp(ARG[0], "cchar") ||
        !stricmp(ARG[0], "tchar") ||
        !stricmp(ARG[0], "uchar") ||
        !stricmp(ARG[0], "u_char") ||
        !stricmp(ARG[0], "uint8_t") ||
        !stricmp(ARG[0], "uint8") ||
        !stricmp(ARG[0], "int8_t") ||
        !stricmp(ARG[0], "int8") ||
        !stricmp(ARG[0], "u8") ||
        !stricmp(ARG[0], "i8") ||
        !stricmp(ARG[0], "si8") ||
        !stricmp(ARG[0], "ui8") ||
        !stricmp(ARG[0], "ch") ||
        !stricmp(ARG[0], "tch") ||
        !stricmp(ARG[0], "str") ||
        !stricmp(ARG[0], "sz") ||
        !stricmp(ARG[0], "ctstr") ||
        !stricmp(ARG[0], "tstr") ||
        !stricmp(ARG[0], "fchar") ||
        !stricmp(ARG[0], "boole8") ||
        !stricmp(ARG[0], "string") ||
        !stricmp(ARG[0], "zstring") ||
        !stricmp(ARG[0], "binary")
    ) {
        type = TYPE_BYTE;
    } else if(
        !stricmp(ARG[0], "16") ||
        !stricmp(ARG[0], "16bit") ||
        !stricmp(ARG[0], "word") ||
        !stricmp(ARG[0], "short") ||
        !stricmp(ARG[0], "ushort") ||
        !stricmp(ARG[0], "u_short") ||
        !stricmp(ARG[0], "uint16_t") ||
        !stricmp(ARG[0], "uint16") ||
        !stricmp(ARG[0], "int16_t") ||
        !stricmp(ARG[0], "int16") ||
        !stricmp(ARG[0], "u16") ||
        !stricmp(ARG[0], "i16") ||
        !stricmp(ARG[0], "si16") ||
        !stricmp(ARG[0], "ui16") ||
        !stricmp(ARG[0], "fixed8") ||   // 8.8 = 16
        !stricmp(ARG[0], "wchar") ||
        !stricmp(ARG[0], "wchar_t") ||
        !stricmp(ARG[0], "wch") ||
        !stricmp(ARG[0], "wstr") ||
        !stricmp(ARG[0], "fshort") ||
        !stricmp(ARG[0], "char16") ||
        !stricmp(ARG[0], "string16") ||
        !stricmp(ARG[0], "boole16") ||
        !stricmp(ARG[0], "zstring16")
    ) {
        type = TYPE_SHORT;
    } else if(
        !stricmp(ARG[0], "32") ||
        !stricmp(ARG[0], "32bit") ||
        !stricmp(ARG[0], "dword") ||
        !stricmp(ARG[0], "unsigned") ||
        !stricmp(ARG[0], "int") ||
        !stricmp(ARG[0], "uint") ||
        !stricmp(ARG[0], "u_int") ||
        !stricmp(ARG[0], "long") ||
        !stricmp(ARG[0], "ulong") ||
        !stricmp(ARG[0], "u_long") ||
        !stricmp(ARG[0], "uint32_t") ||
        !stricmp(ARG[0], "uint32") ||
        !stricmp(ARG[0], "int32_t") ||
        !stricmp(ARG[0], "int32") ||
        !stricmp(ARG[0], "u32") ||
        !stricmp(ARG[0], "i32") ||
        !stricmp(ARG[0], "si32") ||
        !stricmp(ARG[0], "ui32") ||
        !stricmp(ARG[0], "fixed") ||
        !stricmp(ARG[0], "float16") ||  // 16.16 = 32
        !stricmp(ARG[0], "bool") ||
        !stricmp(ARG[0], "boolean") ||
        !stricmp(ARG[0], "boole32") ||
        !stricmp(ARG[0], "void") ||
        !stricmp(ARG[0], "handle") ||
        !stricmp(ARG[0], "flong") ||
        !stricmp(ARG[0], "DOSDateTime") ||
        !stricmp(ARG[0], "UNIXDateTime") ||
        !stricmp(ARG[0], "time_t")
    ) {
        type = TYPE_LONG;
    } else if(
        !stricmp(ARG[0], "64") ||
        !stricmp(ARG[0], "64bit") ||
        !stricmp(ARG[0], "longlong") ||
        !stricmp(ARG[0], "ulonglong") ||
        !stricmp(ARG[0], "u_longlong") ||
        !stricmp(ARG[0], "uint64_t") ||
        !stricmp(ARG[0], "uint64") ||
        !stricmp(ARG[0], "int64_t") ||
        !stricmp(ARG[0], "int64") ||
        !stricmp(ARG[0], "u64") ||
        !stricmp(ARG[0], "i64") ||
        !stricmp(ARG[0], "si64") ||
        !stricmp(ARG[0], "ui64") ||
        !stricmp(ARG[0], "void64") ||
        !stricmp(ARG[0], "FileTime") ||
        !stricmp(ARG[0], "OLEDateTime") ||
        !stricmp(ARG[0], "SQLDateTime") ||
        !stricmp(ARG[0], "JavaDateTime")
    ) {
        type = TYPE_LONGLONG;
    } else if(
        !stricmp(ARG[0], "float")
    ) {
        type = TYPE_FLOAT;
    } else if(
        !stricmp(ARG[0], "double")
    ) {
        type = TYPE_DOUBLE;
    } else if(
        !stricmp(ARG[0], "encodedu32") ||
        !stricmp(ARG[0], "encoded")
    ) {
        type = TYPE_VARIABLE;
    } else if(
        !stricmp(ARG[0], "bits") ||
        !stricmp(ARG[0], "sb") ||
        !stricmp(ARG[0], "ub") ||
        !stricmp(ARG[0], "fb")
    ) {
        type = TYPE_BITS;
    } else {
        type = TYPE_LONG;
        return(-1); // give an error
    }

    if(array < 0) {
        ARG[0] = put ? "put" : "get";
        // ARG[1] is ok
        ARG[2] = "string";
        argc = 2;
        goto quit;
    }

    if(arrayp) {
        for(p = arrayp; *p; p++) {
            if(*p == ']') break;
            if(*p == ',') break;
            if(*p == ')') break;
        }
        *p = 0;
    } else {
        arrayp = "0";
    }

    if(array > 0) {
        if(type == TYPE_BITS) {
            ARG[0] = put ? "putbits" : "getbits";
            // ARG[1] is ok
            sprintf(tmp, "%.*s", sizeof(tmp) - 4, arrayp);
            ARG[2] = tmp;
            argc = 2;
            goto quit;
        }
        ARG[0] = put ? "putdstring" : "getdstring";
        // ARG[1] is ok
        switch(type) {
            case TYPE_BYTE:     sprintf(tmp, "%.*s",   sizeof(tmp) - 4, arrayp);    break;
            case TYPE_SHORT:    sprintf(tmp, "%.*s*2", sizeof(tmp) - 4, arrayp);    break;
            case TYPE_LONG:     sprintf(tmp, "%.*s*4", sizeof(tmp) - 4, arrayp);    break;
            case TYPE_LONGLONG: sprintf(tmp, "%.*s*8", sizeof(tmp) - 4, arrayp);    break;
            case TYPE_FLOAT:    sprintf(tmp, "%.*s*4", sizeof(tmp) - 4, arrayp);    break;
            case TYPE_DOUBLE:   sprintf(tmp, "%.*s*8", sizeof(tmp) - 4, arrayp);    break;
            default:            sprintf(tmp, "%.*s*4", sizeof(tmp) - 4, arrayp);    break;
        }
        ARG[2] = tmp;
        argc = 2;
        goto quit;
    }

        ARG[0] = put ? "put" : "get";
        // ARG[1] is ok
        switch(type) {
            case TYPE_BYTE:     sprintf(tmp, "byte");       break;
            case TYPE_SHORT:    sprintf(tmp, "short");      break;
            case TYPE_LONG:     sprintf(tmp, "long");       break;
            case TYPE_LONGLONG: sprintf(tmp, "longlong");   break;
            case TYPE_FLOAT:    sprintf(tmp, "float");      break;
            case TYPE_DOUBLE:   sprintf(tmp, "double");     break;
            case TYPE_VARIABLE: sprintf(tmp, "variable");   break;
            default:            sprintf(tmp, "long");       break;
        }
        ARG[2] = tmp;
        argc = 2;
        goto quit;

quit:
    ARG[argc + 1] = "";
    if(put) {
        cmd = *ret_cmd;
        CMD.type   = CMD_Set;
        CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName
        CMD.num[1] = add_datatype(ARG[2]);                  // datatype
        CMD.var[2] = add_var(0, put, NULL, 0, -2);          // Var/Number
        (*ret_cmd)++;
    }
    return(argc);
}



int calc_quickbms_version(u8 *version) {
    int     n,
            len,
            ret,
            seq;
    u8      *p;

    if(!version) return(0);
    version = skip_delimit(version);
    ret = 0;
    seq = 24;
    for(p = version; *p; p += len) {
        if(*p == '.') {
            seq -= 8;
            if(seq < 0) break;
            len = 1;
        } else if(((*p >= 'a') && (*p <= 'z')) || ((*p >= 'A') && (*p <= 'Z'))) {
            ret += *p;
            len = 1;
        } else if(strchr(" \t\r\n,;|", *p)) {
            len = 1;
            if(!stricmp(p + len, "-9") && XDBG_ALLOC_ACTIVE) {
                fprintf(stderr,
                    "- the script requires the disabling of the secure allocation XDBG_ALLOC_ACTIVE\n"
                    "  this operation may have negative security effects so consider it only if the\n"
                    "  script comes from a trusted source.\n"
                    "  do you want to disable XDBG_ALLOC_ACTIVE? (y/N)\n"
                    "  ");
                if(get_yesno(NULL) == 'y') XDBG_ALLOC_ACTIVE = 0;   // xdbg_toggle() is not ready yet
                p += 2;
            }
        } else {
            n = readbase(p, 10, &len);
            if(len <= 0) break;
            ret += n << seq;
        }
    }
    return(ret);
}



int set_uint_flex(int cmd, u8 *name, u8 *arg) {
    int     i,
            len;
    u8      max = 0,
            min = 0xff,
            mini[32],
            *tmp;

    tmp = numbers_to_bytes(arg, &len, 0);
    for(i = 0; i < len; i++) {
        if(tmp[i] < min) min = tmp[i];
        if(tmp[i] > max) max = tmp[i];
    }
    max++;

    if(min) {
        sprintf(mini, "%u", min);
        CMD.type   = CMD_GetBits;
        CMD.var[0] = add_var(0, QUICKBMS_DUMMY, NULL, 0, -2); // varname
        CMD.var[1] = add_var(0, mini, NULL, 0, -2);         // bits
        CMD.num[2] = 0;                                     // filenumber
        cmd++;
    }

    sprintf(mini, "%u", max - min);
    CMD.type   = CMD_GetBits;
    CMD.var[0] = add_var(0, name, NULL, 0, -2);         // varname
    CMD.var[1] = add_var(0, mini, NULL, 0, -2);         // bits
    CMD.num[2] = 0;                                     // filenumber

    if(max < 32) {
        cmd++;
        sprintf(mini, "%u", 32 - max);
        CMD.type   = CMD_GetBits;
        CMD.var[0] = add_var(0, QUICKBMS_DUMMY, NULL, 0, -2); // varname
        CMD.var[1] = add_var(0, mini, NULL, 0, -2);         // bits
        CMD.num[2] = 0;                                     // filenumber
    }

    return(cmd);
}



void set_cmd_args_ptr(int cmd, int idx, u8 *arg) {
    if((arg[0] == '&') || (arg[0] == '*')) {
        CMD.num[idx] = 1;                     // &var enabled
        CMD.var[idx] = add_var(0, arg + 1, NULL, 0, -2);
    } else {
        CMD.num[idx] = 0;
        CMD.var[idx] = add_var(0, arg, NULL, 0, -2);
    }
}



int set_cmd_args(int cmd, int argc, u8 *arg[MAX_ARGS + 1], int argx) {
    int     i;

    if(argc > argx) {
        CMD.num[0] = argc - argx;                           // number of arguments
        for(i = argx + 1; i <= argc; i++) {
            CMD.num[i - 1] = 0;                             // &var disabled
            if(!strnicmp(arg[i], MEMORY_FNAME, MEMORY_FNAMESZ)) {
                CMD.var[i - 1] = get_memory_file(arg[i]);
            } else {
                set_cmd_args_ptr(cmd, i - 1, arg[i]);
            }
        }
    }
    return(cmd);
}



u8 set_cmd_string_op(u8 *str) {
    u8      op = str[0];

         if(!stricmp(str, "equal"))     op = '=';
    else if(!stricmp(str, "append"))    op = '+';
    else if(!stricmp(str, "truncate"))  op = '-';
    else if(!stricmp(str, "remove"))    op = '-';
    else if(!stricmp(str, "xor"))       op = '^';
    else if(!stricmp(str, "shl"))       op = '<';
    else if(!stricmp(str, "shr"))       op = '>';
    else if(!stricmp(str, "hex"))       op = 'b';     // to hex
    else if(!stricmp(str, "byte2hex"))  op = 'b';
    else if(!stricmp(str, "byte"))      op = 'h';    // to byte
    else if(!stricmp(str, "hex2byte"))  op = 'h';
    else if(!stricmp(str, "encrypt"))   op = 'e';
    else if(!stricmp(str, "encryption"))op = 'e';
    else if(!stricmp(str, "compress"))  op = 'c';
    else if(!stricmp(str, "compression")) op = 'c';
    else if(!stricmp(str, "upper"))     op = 'u';
    else if(!stricmp(str, "toupper"))   op = 'u';
    else if(!stricmp(str, "lower"))     op = 'l';
    else if(!stricmp(str, "tolower"))   op = 'l';
    else if(!stricmp(str, "reverse"))   op = 'r';
    else if(!stricmp(str, "replace"))   op = 'R';

    else if(!stricmp(str, "printf"))    op = 'p';
    else if(!stricmp(str, "sprintf"))   op = 'p';
    else if(!stricmp(str, "scanf"))     op = 's';
    else if(!stricmp(str, "sscanf"))    op = 's';

    else if(!stricmp(str, "strchr"))    op = '&';
    else if(!stricmp(str, "strstr"))    op = '&';
    else if(!stricmp(str, "strichr"))   op = '&';
    else if(!stricmp(str, "stristr"))   op = '&';
    else if(!stricmp(str, "strchrx"))   op = '|';
    else if(!stricmp(str, "strstrx"))   op = '|';
    else if(!stricmp(str, "strichrx"))  op = '|';
    else if(!stricmp(str, "stristrx"))  op = '|';

    else if(!stricmp(str, "strrchr"))   op = '$';
    else if(!stricmp(str, "strrstr"))   op = '$';
    else if(!stricmp(str, "strrichr"))  op = '$';
    else if(!stricmp(str, "strristr"))  op = '$';
    else if(!stricmp(str, "strrchrx"))  op = '!';
    else if(!stricmp(str, "strrstrx"))  op = '!';
    else if(!stricmp(str, "strrichrx")) op = '!';
    else if(!stricmp(str, "strristrx")) op = '!';
    return(op);
}



int parse_bms(FILE *fds, u8 *inputs, int cmd) {
    FILE    *include_fd;
    int     i,
            argc,
            c_structs_do;
    u8      //*debug_line = NULL,
            *argument[MAX_ARGS + 1] = { NULL },
            *tmp,
            is_const[MAX_ARGS + 1]; // currently this is not used, the idea is only to alert users if
                                    // they specify a constant string like "test" and a variable named TEST
                                    // ... maybe in future

    for(;;) {       // do NOT use "continue;"!
        if(cmd >= MAX_CMDS) {
            fprintf(stderr, "\nError: the BMS script uses more commands than how much supported by this tool\n");
            myexit(QUICKBMS_ERROR_BMS);
        }

        // never do memset because commands are initialized in a particular way (check bms_init)
        //memset(&command[cmd], 0, sizeof(command[cmd]));

        argc = bms_line(fds, inputs, argument, &CMD.debug_line, is_const); //&debug_line);
        if(argc < 0) break; // means "end of file"
        if(!argc) continue; // means "no command", here is possible to use "continue"

        argc--; // remove command argument
        // remember that myatoi is used only for the file number, all the rest must be add_var

        c_structs_do = 1;
redo:
               if(!stricmp(ARG[0], "QuickBMSver")   && (argc >= 1)) {
            CMD.type   = CMD_NOP;
            if(calc_quickbms_version(ARG[1]) > quickbms_version) {
                fprintf(stderr, "\n"
                    "Error: this script has been created for a newer version of QuickBMS (%s),\n"
                    "       you can download it from:\n"
                    "\n"
                    "         http://aluigi.org/quickbms\n"
                    "\n", ARG[1]);
                myexit(QUICKBMS_ERROR_BMS);
            }

        } else if(!stricmp(ARG[0], "CLog")          && (argc >= 4)) {
            CMD.type   = CMD_CLog;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // name
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // offset
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // compressed size
            CMD.var[3] = -1;                                    // offsetoffset
            CMD.var[4] = -1;                                    // resourcesizeoffset
            CMD.var[6] = -1;                                    // uncompressedsizeoffset
            if(argc >= 6) {
                CMD.var[5] = add_var(0, ARG[6], NULL, 0, -2);   // uncompressedsize
                CMD.num[7] = myatoifile(ARG[8]);                // filenumber
            } else {
                CMD.var[5] = add_var(0, ARG[4], NULL, 0, -2);   // uncompressedsize
                CMD.num[7] = myatoifile(ARG[5]);                // filenumber
            }

        } else if(
                 (!stricmp(ARG[0], "Do")            && (argc >= 0))
              || (!stricmp(ARG[0], "Loop")          && (argc >= 0))) {  // mex inifile (not BMS)
            CMD.type   = CMD_Do;

        } else if(!stricmp(ARG[0], "FindLoc")       && (argc >= 3)) {
            CMD.type   = CMD_FindLoc;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var
            CMD.num[1] = add_datatype(ARG[2]);                  // datatype
            CSTRING(2, ARG[3])                                  // text/number
            if(argc >= 4) {
                if(!ARG[4][0]) {    // a typical mistake that I do too!
                    CMD.num[3] = 0;
                    mystrdup(&CMD.str[4], ARG[4]);
                } else {
                    CMD.num[3] = myatoifile(ARG[4]);            // filenumber
                    mystrdup(&CMD.str[4], ARG[5]);              // optional/experimental: the value you want to return in case the string is not found
                }
            } else {
                CMD.num[3] = 0;                                 // filenumber
                FREEZ(CMD.str[4])                               // optional/experimental: the value you want to return in case the string is not found
            }

        } else if(!stricmp(ARG[0], "FindFileID")    && (argc >= 2)) {   // mex inifile (not BMS)
            CMD.type   = CMD_FindLoc;
            CMD.var[0] = add_var(0, ARG[2], NULL, 0, -2);       // var
            CMD.num[1] = add_datatype("String");                // datatype
            mystrdup(&CMD.str[2], ARG[1]);                      // text/number
            CMD.num[3] = myatoifile(ARG[3]);                    // filenumber

        } else if(!stricmp(ARG[0], "For")           && (argc >= 0)) {
            if(argc >= 3) {
                CMD.type   = CMD_Math;
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // VarName
                CMD.num[1] = ARG[2][0];                         // operation
                CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);   // Var/Number
                cmd++;
            }

            CMD.type   = CMD_For;   // yes, no arguments, this is the new way

            if(argc >= 5) {
                cmd++;
                CMD.type   = CMD_ForTo;
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // T
                                                                // = T_value (check later, it must be check_condition compatible)
                if(!stricmp(ARG[4], "To")) {                    // To
                    mystrdup(&CMD.str[1], "<=");
                } else {
                    mystrdup(&CMD.str[1], ARG[4]);
                }
                CMD.var[2] = add_var(0, ARG[5], NULL, 0, -2);   // To_value
                //CMD.var[3] = add_var(0, ARG[3], NULL, 0, -2);  // T_value (not used)
            }

        } else if(!stricmp(ARG[0], "Get")           && (argc >= 2)) {
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype(ARG[2]);                  // type
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber
            if(CMD.num[1] == TYPE_BITS) {
                CMD.type   = CMD_GetBits;
                //CMD.var[0] is ok
                CMD.var[1] = myatoi(ARG[2]);
            }

        } else if(!stricmp(ARG[0], "GetBits")       && (argc >= 2)) {
            CMD.type   = CMD_GetBits;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // bits
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber

        } else if(!stricmp(ARG[0], "PutBits")       && (argc >= 2)) {
            CMD.type   = CMD_PutBits;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // bits
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber

        } else if(!stricmp(ARG[0], "Put")           && (argc >= 2)) {   // write mode
            CMD.type   = CMD_Put;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype(ARG[2]);                  // type
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber
            if(CMD.num[1] == TYPE_BITS) {
                CMD.type   = CMD_PutBits;
                //CMD.var[0] is ok
                CMD.var[1] = myatoi(ARG[2]);
            }

        } else if(!stricmp(ARG[0], "GetLong")       && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype("Long");                  // type
            CMD.num[2] = myatoifile(ARG[2]);                    // filenumber

        } else if(!stricmp(ARG[0], "GetInt")        && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype("Int");                   // type
            CMD.num[2] = myatoifile(ARG[2]);                    // filenumber

        } else if(!stricmp(ARG[0], "GetByte")       && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype("Byte");                  // type
            CMD.num[2] = myatoifile(ARG[2]);                    // filenumber

        } else if(!stricmp(ARG[0], "GetString")     && (argc >= 2)) {   // mex inifile (not BMS)
            CMD.type   = CMD_GetDString;
            CMD.var[0] = add_var(0, ARG[2], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[1], NULL, 0, -2);       // NumberOfCharacters
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber

        } else if(!stricmp(ARG[0], "GetNullString") && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype("String");                // type
            CMD.num[2] = myatoifile(ARG[2]);                    // filenumber

        } else if(
                  (!stricmp(ARG[0], "GetDString")    && (argc >= 2))
               || (!stricmp(ARG[0], "fread")         && (argc >= 2))) {
            CMD.type   = CMD_GetDString;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // NumberOfCharacters
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber

        } else if(
                  (!stricmp(ARG[0], "PutDString")    && (argc >= 2))
               || (!stricmp(ARG[0], "fwrite")        && (argc >= 2))) {   // write mode
            CMD.type   = CMD_PutDString;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // NumberOfCharacters
            CMD.num[2] = myatoifile(ARG[3]);                    // filenumber

        } else if(
                  (!stricmp(ARG[0], "GoTo")          && (argc >= 1))
               || (!stricmp(ARG[0], "fseek")         && (argc >= 1))) {
            CMD.type   = CMD_GoTo;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // pos
            CMD.num[1] = myatoifile(ARG[2]);                    // file
            CMD.num[2] = SEEK_SET;
            if(argc >= 3) {
                     if(stristr(ARG[3], "SET")) CMD.num[2] = SEEK_SET;
                else if(stristr(ARG[3], "CUR")) CMD.num[2] = SEEK_CUR;
                else if(stristr(ARG[3], "END")) CMD.num[2] = SEEK_END;
            }

        } else if(
                  (!stricmp(ARG[0], "IDString")     && (argc >= 1))
               || (!stricmp(ARG[0], "ID")           && (argc >= 1))     // mex inifile (not BMS)
               || (!stricmp(ARG[0], "memcmp")       && (argc >= 1))
               || (!stricmp(ARG[0], "strcmp")       && (argc >= 1))) {
            CMD.type   = CMD_IDString;
            if(argc == 1) {
                CMD.num[0] = 0;
                CSTRING(1, ARG[1])                              // string
            } else {
                CMD.num[0] = myatoifile(ARG[1]);                // filenumber
                if(CMD.num[0] == MAX_FILES) {                   // simple work-around to avoid the different syntax of idstring
                    CSTRING(1, ARG[1])                          // string
                    CMD.num[0] = myatoifile(ARG[2]);
                } else {
                    CSTRING(1, ARG[2])                          // string
                    // CMD.num[0] = myatoifile(ARG[1]); // already set
                }
            }

        } else if(!strnicmp(ARG[0], "ID=", 3)       && (argc >= 0)) {   // mex inifile (not BMS)
            CMD.type   = CMD_IDString;
            CMD.num[0] = 0;
            mystrdup(&CMD.str[1], ARG[0] + 3);                  // bytes

        } else if(!stricmp(ARG[0], "ImpType")       && (argc >= 1)) {
            CMD.type   = CMD_ImpType;
            mystrdup(&CMD.str[0], ARG[1]);                      // type

        } else if(!stricmp(ARG[0], "Log")           && (argc >= 3)) {
            CMD.type   = CMD_Log;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // name
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // offset
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // size
            CMD.var[3] = -1;                                    // offsetoffset
            CMD.var[4] = -1;                                    // resourcesizeoffset
            if(argc >= 5) {
                CMD.num[5] = myatoifile(ARG[6]);                // filenumber
            } else {
                CMD.num[5] = myatoifile(ARG[4]);                // filenumber
            }

        } else if(!stricmp(ARG[0], "ExtractFile")   && (argc >= 0)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Log;
            CMD.var[0] = add_var(0, "FILENAME", NULL, 0, -2);   //  name
            CMD.var[1] = add_var(0, "FILEOFF",  NULL, 0,  0);   // offset
            CMD.var[2] = add_var(0, "FILESIZE", NULL, 0, -2);   // size
            CMD.var[3] = -1;                                    // offsetoffset
            CMD.var[4] = -1;                                    // resourcesizeoffset
            CMD.num[5] = myatoifile(ARG[6]);                    // filenumber

        } else if(!stricmp(ARG[0], "Math")          && (argc >= 3)) {
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = set_math_operator(ARG[2], &CMD.num[2], NULL);  // op
            if(!stricmp(ARG[2], "long")) CMD.num[1] = '=';      // a stupid error that can happen
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // var2

        } else if(!stricmp(ARG[0], "XMath")         && (argc >= 2)) {
            CMD.type   = CMD_XMath;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // output
            mystrdup(&CMD.str[1], ARG[2]);                      // input instructions, "1 + 2"

        } else if(!stricmp(ARG[0], "Add")           && (argc >= 3)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '+';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // var2

        } else if(!stricmp(ARG[0], "Inc")           && (argc >= 1)) {
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '+';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, "1", NULL, 0, -2);          // var2

        } else if(!stricmp(ARG[0], "Subst")         && (argc >= 3)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '-';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // var2

        } else if(!stricmp(ARG[0], "Dec")           && (argc >= 1)) {
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '-';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, "1", NULL, 0, -2);          // var2

        } else if(!stricmp(ARG[0], "Multiply")      && (argc >= 5)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Set;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName
            CMD.num[1] = add_datatype("String");                // datatype
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // Var/Number
            cmd++;
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '*';                                   // op
            CMD.var[2] = add_var(0, ARG[5], NULL, 0, -2);       // var2

        } else if(!stricmp(ARG[0], "Divide")        && (argc >= 5)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Set;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName
            CMD.num[1] = add_datatype("String");                // datatype
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // Var/Number
            cmd++;
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '/';                                   // op
            CMD.var[2] = add_var(0, ARG[5], NULL, 0, -2);       // var2

        } else if(!stricmp(ARG[0], "Up")            && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '+';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, "1", NULL, 0, -2);          // var2

        } else if(!stricmp(ARG[0], "Down")          && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Math;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var1
            CMD.num[1] = '-';                                   // op (skip specifier!)
            CMD.var[2] = add_var(0, "1", NULL, 0, -2);          // var2

        } else if(!stricmp(ARG[0], "Next")          && (argc >= 0)) {
            CMD.type   = CMD_Next;
            if(!argc) {
                CMD.var[0] = -1;
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // VarName
                if(argc >= 2) {
                    CMD.num[1] = set_math_operator(ARG[2], &CMD.num[2], NULL);
                    CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);
                }
            }

        } else if(!stricmp(ARG[0], "Prev")          && (argc >= 0)) {
            CMD.type   = CMD_Prev;
            if(!argc) {
                CMD.var[0] = -1;
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // VarName
            }

        //} else if(!stricmp(ARG[0], "Continue")      && (argc >= 0)) {
            //CMD.type   = CMD_Continue;

        } else if(!stricmp(ARG[0], "Open")          && (argc >= 2)) {
            CMD.type   = CMD_Open;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // Folder/Specifier
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // Filename/Extension
            CMD.num[2] = myatoifile(ARG[3]);                    // File (default is 0, the same file)
            CMD.var[3] = add_var(0, ARG[4], NULL, 0, -2);       // optional/experimental: this var will be 1 if exists otherwise 0

        } else if(
                  (!stricmp(ARG[0], "SavePos")       && (argc >= 1))
               || (!stricmp(ARG[0], "ftell")         && (argc >= 1))) {
            CMD.type   = CMD_SavePos;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName
            CMD.num[1] = myatoifile(ARG[2]);                    // File

        } else if(!stricmp(ARG[0], "Set")           && (argc >= 2)) {
            CMD.type   = CMD_Set;
            //CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);      // VarName
            if(!strnicmp(ARG[1], MEMORY_FNAME, MEMORY_FNAMESZ)) {
                CMD.var[0] = get_memory_file(ARG[1]);
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);
            }
            if(argc == 2) {
                CMD.num[1] = add_datatype("String");            // datatype
                //CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);  // Var/Number
                tmp = ARG[2];
            } else {
                if(ARG[2][0] == '=') {
                    CMD.num[1] = add_datatype("String");
                } else if(!stricmp(ARG[2], "strlen")) { // I'm crazy
                    CMD.type   = CMD_Strlen;
                    CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // dest var
                    CMD.var[1] = add_var(0, ARG[3], NULL, 0, -2);   // string
                } else {
                    CMD.num[1] = add_datatype(ARG[2]);           // datatype
                }
                //CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);  // Var/Number
                tmp = ARG[3];
            }
            if(CMD.num[1] == TYPE_BINARY) {
                CSTRING(2, tmp)
            } else if(CMD.num[1] == TYPE_COMPRESSED) {
                CMD.num[1] = TYPE_BINARY;
                FREEZ(CMD.str[2])
                CMD.str[2] = type_decompress(tmp, &CMD.num[2]);
            } else {
                CMD.var[2] = add_var(0, tmp, NULL, 0, -2);
            }

        } else if(!stricmp(ARG[0], "SETFILECNT")    && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Set;
            CMD.var[0] = add_var(0, "FILECNT", NULL, 0, -2);    // VarName
            CMD.num[1] = add_datatype("String");                // datatype
            CMD.var[2] = add_var(0, ARG[1], NULL, 0, -2);       // Var/Number

        } else if(!stricmp(ARG[0], "SETBYTESREAD")  && (argc >= 1)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Set;
            CMD.var[0] = add_var(0, "BYTESREAD", NULL, 0, -2);  // VarName
            CMD.num[1] = add_datatype("String");                // datatype
            CMD.var[2] = add_var(0, ARG[1], NULL, 0, -2);       // Var/Number

        } else if(!stricmp(ARG[0], "While")         && (argc >= 3)) {
            CMD.type   = CMD_While;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // Varname
            mystrdup(&CMD.str[1], ARG[2]);                      // Criterium
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // VarName2

        } else if(!stricmp(ARG[0], "EndLoop")       && (argc >= 2)) {   // mex inifile (not BMS)
            CMD.type   = CMD_While;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // Varname
            mystrdup(&CMD.str[1], "!=");                        // Criterium
            CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);       // VarName2

        } else if(!stricmp(ARG[0], "String")        && (argc >= 3)) {
            CMD.type   = CMD_String;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            CMD.num[1] = set_cmd_string_op(ARG[2]); /* NO tolower! */ // op
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // VarName2
            CMD.num[0] = argc - 3;
            for(i = 4; i <= argc; i++) {
                //if(!strnicmp(ARG[i], MEMORY_FNAME, MEMORY_FNAMESZ)) {
                    //CMD.var[i - 1] = get_memory_file(ARG[i]);
                //} else {
                    CMD.var[i - 1] = add_var(0, ARG[i], NULL, 0, -2);
                //}
            }

        } else if(!stricmp(ARG[0], "CleanExit")     && (argc >= 0)) {
            CMD.type   = CMD_CleanExit;

        } else if(!stricmp(ARG[0], "Exit")          && (argc >= 0)) {
            CMD.type   = CMD_CleanExit;

        } else if(!stricmp(ARG[0], "ExitIfNoFilesOpen") && (argc >= 0)) {
            CMD.type   = CMD_CleanExit;

        } else if(!stricmp(ARG[0], "Case")          && (argc >= 2)) {   // mex inifile (not BMS)
            CMD.type   = CMD_If;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], "=");                         // Criterium
            CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);       // VarName2
            cmd++;
            CMD.type   = CMD_EndIf;

        } else if(!stricmp(ARG[0], "If")            && (argc >= 3)) {
            CMD.type   = CMD_If;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], ARG[2]);                      // Criterium
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // VarName2
            for(i = 4; i <= argc; i += 4) {
                mystrdup(&CMD.str[(i-1)], ARG[i]);                      // and/or
                CMD.var[(i-1)+1] = add_var(0, ARG[i+1], NULL, 0, -2);   // VarName1
                mystrdup(&CMD.str[(i-1)+2], ARG[i+2]);                  // Criterium
                CMD.var[(i-1)+3] = add_var(0, ARG[i+3], NULL, 0, -2);   // VarName2
            }

        } else if((!stricmp(ARG[0], "Elif") || !stricmp(ARG[0], "ElseIf")) && (argc >= 3)) {   // copy as above!
            CMD.type   = CMD_Elif;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], ARG[2]);                      // Criterium
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // VarName2
            for(i = 4; i <= argc; i += 4) {
                mystrdup(&CMD.str[(i-1)], ARG[i]);                      // and/or
                CMD.var[(i-1)+1] = add_var(0, ARG[i+1], NULL, 0, -2);   // VarName1
                mystrdup(&CMD.str[(i-1)+2], ARG[i+2]);                  // Criterium
                CMD.var[(i-1)+3] = add_var(0, ARG[i+3], NULL, 0, -2);   // VarName2
            }

        } else if(!stricmp(ARG[0], "Else")          && (argc >= 0)) {
            CMD.type   = CMD_Else;
            if((argc >= 4) && !stricmp(ARG[1], "If")) {         // copy as above!
                CMD.type   = CMD_Elif;
                CMD.var[0] = add_var(0, ARG[2], NULL, 0, -2);   // VarName1
                mystrdup(&CMD.str[1], ARG[3]);                  // Criterium
                CMD.var[2] = add_var(0, ARG[4], NULL, 0, -2);   // VarName2
                for(i = 5; i <= argc; i += 4) {
                    mystrdup(&CMD.str[(i-2)], ARG[i]);                      // and/or
                    CMD.var[(i-2)+1] = add_var(0, ARG[i+1], NULL, 0, -2);   // VarName1
                    mystrdup(&CMD.str[(i-2)+2], ARG[i+2]);                  // Criterium
                    CMD.var[(i-2)+3] = add_var(0, ARG[i+3], NULL, 0, -2);   // VarName2
                }
            }

        } else if(!stricmp(ARG[0], "IfEqual")       && (argc >= 2)) {
            CMD.type   = CMD_If;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], "==");                        // Criterium
            CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);       // VarName2

        } else if(!stricmp(ARG[0], "IfGreater")     && (argc >= 2)) {
            CMD.type   = CMD_If;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], ">");                         // Criterium
            CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);       // VarName2

        } else if(!stricmp(ARG[0], "IfLower")     && (argc >= 2)) {
            CMD.type   = CMD_If;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // VarName1
            mystrdup(&CMD.str[1], "<");                         // Criterium
            CMD.var[2] = add_var(0, ARG[2], NULL, 0, -2);       // VarName2

        } else if(!stricmp(ARG[0], "EndIf")         && (argc >= 0)) {
            CMD.type   = CMD_EndIf;

        } else if(!stricmp(ARG[0], "GetCT")         && (argc >= 3)) {
            CMD.type   = CMD_GetCT;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // variable
            CMD.num[1] = add_datatype(ARG[2]);                  // datatype
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // character
            CMD.num[3] = myatoifile(ARG[4]);                    // filenumber

        } else if(!stricmp(ARG[0], "PutCT")         && (argc >= 3)) {   // write mode
            CMD.type   = CMD_PutCT;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // variable
            CMD.num[1] = add_datatype(ARG[2]);                  // datatype
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // character
            CMD.num[3] = myatoifile(ARG[4]);                    // filenumber

        } else if(!stricmp(ARG[0], "ComType")       && (argc >= 1)) {
            CMD.type   = CMD_ComType;
            mystrdup(&CMD.str[0], ARG[1]);                      // ComType
            if(argc <= 2) {
                CSTRING(1, ARG[2])                              // optional static binary dictionary
            } else {
                CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);   // optional variable dictionary
                CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);
            }

        } else if(
                 (!stricmp(ARG[0], "ReverseShort")  && (argc >= 1))
              || (!stricmp(ARG[0], "FlipShort")     && (argc >= 1))) {  // mex inifile (not BMS)
            CMD.type   = CMD_ReverseShort;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // variable

        } else if(
                 (!stricmp(ARG[0], "ReverseLong")   && (argc >= 1))
              || (!stricmp(ARG[0], "FlipLong")      && (argc >= 1))) {  // mex inifile (not BMS)
            CMD.type   = CMD_ReverseLong;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // variable

        } else if(
                 (!stricmp(ARG[0], "ReverseLongLong")   && (argc >= 1))
              || (!stricmp(ARG[0], "FlipLongLong")      && (argc >= 1))) {  // mex inifile (not BMS)
            CMD.type   = CMD_ReverseLongLong;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // variable

        } else if(!stricmp(ARG[0], "PROMPTUSER")    && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "EVENTS")        && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "SEPPATH")       && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "NOFILENAMES")   && (argc >= 0)) {   // mex inifile (not BMS)
            CMD.type   = CMD_Set;
            CMD.var[0] = add_var(0, "FILENAME", NULL, 0, -2);   // VarName
            CMD.num[1] = add_datatype("String");                // datatype
            CMD.var[2] = add_var(0, "", NULL, 0, -2);           // Var/Number

        } else if(!stricmp(ARG[0], "WriteLong")     && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "StrCReplace")   && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "StrEResizeC")   && (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "SeperateHeader")&& (argc >= 0)) {   // mex inifile (not BMS)
            // do nothing, this command is useless
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "Endian")        && (argc >= 1)) {
            CMD.type   = CMD_Endian;
            if(!stricmp(ARG[1], "little") || !stricmp(ARG[1], "intel") || !stricmp(ARG[1], "1234")) {
                CMD.num[0] = MYLITTLE_ENDIAN;
            } else if(!stricmp(ARG[1], "big") || !stricmp(ARG[1], "network") || !stricmp(ARG[1], "4321")) {
                CMD.num[0] = MYBIG_ENDIAN;
            } else if(!stricmp(ARG[1], "swap") || !stricmp(ARG[1], "invert") || !stricmp(ARG[1], "change")) {
                CMD.num[0] = -1;
            } else {
                fprintf(stderr, "\nError: invalid endian value %s\n", ARG[1]);
                myexit(QUICKBMS_ERROR_BMS);
            }

        } else if(!stricmp(ARG[0], "FileXOR")       && (argc >= 1)) {
            CMD.type   = CMD_FileXOR;
            CMD.num[0] = 0; // used to contain the size of str[0], improves the performances
            if(myisdigit(ARG[1][0]) || (ARG[1][0] == '\\')) {
                NUMS2BYTES(ARG[1], CMD.num[1], CMD.str[0], CMD.num[0])  // acts like a realloc
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // string
            }
            CMD.num[2] = 0;                                     // reset pos
            if(argc == 1) {
                CMD.var[3] = add_var(0, "-1", NULL, 0, -2);     // current offset
                CMD.num[4] = 0;
            } else {
                CMD.var[3] = add_var(0, ARG[2], NULL, 0, -2);   // first position offset (used only for Log and multiple bytes in rare occasions)
                CMD.num[4] = myatoifile(ARG[3]);                // filenumber (not implemented)
            }

        } else if(!strnicmp(ARG[0], "FileRot", 7)   && (argc >= 1)) {
            CMD.type   = CMD_FileRot13;
            CMD.num[0] = 0; // used to contain the size of str[0], improves the performances
            if(myisdigit(ARG[1][0]) || (ARG[1][0] == '\\')) {
                NUMS2BYTES(ARG[1], CMD.num[1], CMD.str[0], CMD.num[0])  // acts like a realloc
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // string
            }
            CMD.num[2] = 0;                                     // reset pos
            if(argc == 1) {
                CMD.var[3] = add_var(0, "-1", NULL, 0, -2);     // current offset
                CMD.num[4] = 0;
            } else {
                CMD.var[3] = add_var(0, ARG[2], NULL, 0, -2);   // first position offset (used only for Log and multiple bytes in rare occasions)
                CMD.num[4] = myatoifile(ARG[3]);                // filenumber (not implemented)
            }

        } else if(!stricmp(ARG[0], "FileCrypt")     && (argc >= 1)) {
            CMD.type   = CMD_FileCrypt;
            CMD.num[0] = 0; // used to contain the size of str[0], improves the performances
            if(myisdigit(ARG[1][0]) || (ARG[1][0] == '\\')) {
                NUMS2BYTES(ARG[1], CMD.num[1], CMD.str[0], CMD.num[0])  // acts like a realloc
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // string
            }
            CMD.num[2] = 0;                                     // reset pos
            if(argc == 1) {
                CMD.var[3] = add_var(0, "-1", NULL, 0, -2);     // current offset
                CMD.num[4] = 0;
            } else {
                CMD.var[3] = add_var(0, ARG[2], NULL, 0, -2);   // first position offset (used only for Log and multiple bytes in rare occasions)
                CMD.num[4] = myatoifile(ARG[3]);                // filenumber (not implemented)
            }

        } else if(!stricmp(ARG[0], "Break")         && (argc >= 0)) {
            CMD.type   = CMD_Break;

        } else if(!stricmp(ARG[0], "Strlen")        && (argc >= 2)) {
            CMD.type   = CMD_Strlen;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // dest var
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // string

        } else if(!stricmp(ARG[0], "GetVarChr")     && (argc >= 3)) {
            CMD.type   = CMD_GetVarChr;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // dst byte
            if(!strnicmp(ARG[2], MEMORY_FNAME, MEMORY_FNAMESZ)) {
                CMD.var[1] = get_memory_file(ARG[2]);
            } else {
                CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);   // src var
            }
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // offset
            if(argc == 3) {
                CMD.num[3] = add_datatype("byte");
            } else {
                CMD.num[3] = add_datatype(ARG[4]);
            }

        } else if(!stricmp(ARG[0], "PutVarChr")     && (argc >= 3)) {
            CMD.type   = CMD_PutVarChr;
            if(!strnicmp(ARG[1], MEMORY_FNAME, MEMORY_FNAMESZ)) {
                CMD.var[0] = get_memory_file(ARG[1]);
            } else {
                CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);   // dst var
            }
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // offset
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // src byte
            if(argc == 3) {
                CMD.num[3] = add_datatype("byte");
            } else {
                CMD.num[3] = add_datatype(ARG[4]);
            }

        } else if(!stricmp(ARG[0], "Debug")         && (argc >= 0)) {
            CMD.type   = CMD_Debug;
            CMD.num[0] = myatoi(ARG[1]);                        // type of verbosity

        } else if(!stricmp(ARG[0], "Padding")       && (argc >= 1)) {
            CMD.type   = CMD_Padding;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // padding size
            CMD.num[1] = myatoifile(ARG[2]);                    // filenumber

        } else if(!stricmp(ARG[0], "Append")        && (argc >= 0)) {
            CMD.type   = CMD_Append;

        } else if(!stricmp(ARG[0], "Encryption")    && (argc >= 2)) {
            CMD.type   = CMD_Encryption;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // type
            if(!stricmp(ARG[2], "?")) {
                fprintf(stderr, "\n"
                    "Error: seems that the script you are using needs that you specify a fixed\n"
                    "       %s key at line %d for using it, so edit the script source code\n"
                    "       adding this needed value, examples:\n"
                    "         encryption %s \"mykey\"\n"
                    "         encryption %s \"\\x6d\\x79\\x6b\\x65\\x79\"\n"
                    "\n", ARG[1], (i32)bms_line_number, ARG[1], ARG[1]);
                myexit(QUICKBMS_ERROR_BMS);
            }
            CSTRING(1, ARG[2])                                  // key
            CSTRING(2, ARG[3])                                  // ivec
            if(argc >= 4) {
                CMD.var[3] = add_var(0, ARG[4], NULL, 0, -2);   // decrypt/encrypt
            }
            if(argc >= 5) {
                CMD.var[4] = add_var(0, ARG[5], NULL, 0, -2);   // keylen
            }

        } else if(!stricmp(ARG[0], "Print")         && (argc >= 1)) {
            CMD.type   = CMD_Print;
            CSTRING(0, ARG[1])                                  // message

        } else if(!stricmp(ARG[0], "MessageBox")    && (argc >= 1)) {
            CMD.type   = CMD_Print;
            CSTRING(0, ARG[1])                                  // message

        } else if(!stricmp(ARG[0], "GetArray")      && (argc >= 3)) {
            CMD.type   = CMD_GetArray;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // var
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // array number
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // number/string

        } else if(!stricmp(ARG[0], "PutArray")      && (argc >= 3)) {
            CMD.type   = CMD_PutArray;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // array number
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // number/string
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // var

        } else if(!stricmp(ARG[0], "SortArray")      && (argc >= 1)) {
            CMD.type   = CMD_SortArray;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // array number
            if(argc >= 2) CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2); // all

        } else if(!stricmp(ARG[0], "StartFunction") && (argc >= 1)) {
            CMD.type   = CMD_StartFunction;
            mystrdup(&CMD.str[0], ARG[1]);

        } else if(!stricmp(ARG[0], "CallFunction")  && (argc >= 1)) {
            CMD.type   = CMD_CallFunction;
            mystrdup(&CMD.str[0], ARG[1]);
            CMD.num[1] = myatoi(ARG[2]);
            cmd = set_cmd_args(cmd, argc, ARG, 2);              // number of arguments

        } else if(!stricmp(ARG[0], "EndFunction")   && (argc >= 0)) {
            CMD.type   = CMD_EndFunction;
            //mystrdup(&CMD.str[0], ARG[1]);

        } else if(!stricmp(ARG[0], "ScanDir")       && (argc >= 3)) {
            CMD.type   = CMD_ScanDir;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // path to scan
            CMD.var[1] = add_var(0, ARG[2], NULL, 0, -2);       // filename
            CMD.var[2] = add_var(0, ARG[3], NULL, 0, -2);       // filesize
            if(ARG[4] && !filter_files) {
                mystrdup(&tmp, ARG[4]);
                filter_files = build_filter(tmp);
                FREEZ(tmp)
            }

        } else if(!stricmp(ARG[0], "CallDLL")       && (argc >= 3)) {
            CMD.type   = CMD_CallDLL;
            mystrdup(&CMD.str[0], ARG[1]);                      // name of the dll
            mystrdup(&CMD.str[1], ARG[2]);                      // name of the function or relative offset
            mystrdup(&CMD.str[2], ARG[3]);                      // stdcall/cdecl
            //CMD.var[3] = add_var(0, ARG[4], NULL, 0, -2);       // return value
                set_cmd_args_ptr(cmd, 3, ARG[4]);
            cmd = set_cmd_args(cmd, argc, ARG, 4);              // number of arguments

        } else if(!stricmp(ARG[0], "include")       && (argc >= 1)) {
            tmp = quickbms_fopen(ARG[1]);
            if(!tmp) tmp = ARG[1];
            include_fd = fopen(tmp, "rb");
            if(!include_fd) {
                fprintf(stderr, "- requested script \"%s\" not found\n", ARG[1]);
                myexit(QUICKBMS_ERROR_BMS);
            }
            cmd = parse_bms(include_fd, NULL, cmd);
            cmd--;  // needed!
            fclose(include_fd);

        } else if(!stricmp(ARG[0], "Game") || !stricmp(ARG[0], "Archive")
               || !strnicmp(ARG[0], "Game ", 5)
               || !strnicmp(ARG[0], "Game:", 5)
               || !strnicmp(ARG[0], "Archive", 7)
               || !strnicmp(ARG[0], "Archive:", 8)
               || strstr(ARG[0], "-------")
               || strstr(ARG[0], "=-=-=-=")
               || stristr(ARG[0], "<bms")
               || stristr(ARG[0], "<bms>")
               || stristr(ARG[0], "</bms>")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "NOP")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "template")) {
            CMD.type   = CMD_Debug;
            CMD.num[0] = 1;

        } else if(!stricmp(ARG[0], "description")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "applies_to")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "fixed_start") && (argc >= 1)) {
            CMD.type   = CMD_GoTo;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // pos
            CMD.num[1] = 0;                                     // file
            CMD.num[2] = SEEK_SET;

        } else if(!stricmp(ARG[0], "sector-aligned")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "big-endian")) {
            CMD.type   = CMD_Endian;
            CMD.num[0] = MYBIG_ENDIAN;

        } else if(!stricmp(ARG[0], "little-endian")) {
            CMD.type   = CMD_Endian;
            CMD.num[0] = MYLITTLE_ENDIAN;

        } else if(!stricmp(ARG[0], "requires") && argc >= 2) {
            CMD.type   = CMD_SavePos;
            CMD.var[0] = add_var(0, QUICKBMS_DUMMY, NULL, 0, -2);   // VarName
            CMD.num[1] = 0;                                     // File

            cmd++;
            CMD.type   = CMD_GoTo;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // pos
            CMD.num[1] = 0;                                     // file
            CMD.num[2] = SEEK_SET;

            cmd++;
            CMD.type   = CMD_IDString;
            CMD.num[0] = 0;
            NUMS2BYTES_HEX(ARG[2], CMD.num[1], CMD.str[1], CMD.num[2])  // string (acts like a realloc)

            cmd++;
            CMD.type   = CMD_GoTo;
            CMD.var[0] = add_var(0, QUICKBMS_DUMMY, NULL, 0, -2);   // pos
            CMD.num[1] = 0;                                     // file
            CMD.num[2] = SEEK_SET;

        } else if(!stricmp(ARG[0], "begin")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "end")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "hex") && argc >= 2) {
            CMD.type   = CMD_GetDString;
            CMD.var[0] = add_var(0, ARG[2], NULL, 0, -2);       // varname
            CMD.var[1] = add_var(0, ARG[1], NULL, 0, -2);       // NumberOfCharacters
            CMD.num[2] = 0;                                     // filenumber

        } else if(!stricmp(ARG[0], "move") && argc >= 1) {
            CMD.type   = CMD_GoTo;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // pos
            CMD.num[1] = 0;                                     // file
            CMD.num[2] = SEEK_CUR;

        } else if(!stricmp(ARG[0], "uint_flex") && (argc >= 2)) {
            cmd = set_uint_flex(cmd, ARG[2], ARG[1]);

        } else if(!stricmp(ARG[0], "section")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "endsection")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "numbering")) {
            CMD.type   = CMD_NOP;

        } else if(!stricmp(ARG[0], "zstring") && (argc >= 1)) {
            CMD.type   = CMD_Get;
            CMD.var[0] = add_var(0, ARG[1], NULL, 0, -2);       // varname
            CMD.num[1] = add_datatype("string");                // type
            CMD.num[2] = 0;                                     // filenumber

        } else {
            if(c_structs_do) {
                argc = c_structs(argument, argc, &cmd);
                if(argc >= 0) {
                    c_structs_do = 0;
                    goto redo;
                }
            }

            fprintf(stderr, "\nError: invalid command \"%s\" or arguments %d at line %d\n", ARG[0], (i32)argc, (i32)bms_line_number);
            myexit(QUICKBMS_ERROR_BMS);
        }

        if(CMD.type == CMD_NONE) {
            fprintf(stderr, "\nError: there is an error in this tool because there is no command type\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        //CMD.debug_line = debug_line;

        // not much useful
        if(verbose > 0) {
            for(i = 0; i <= argc; i++) {
                printf("  ARG%-2d \"%s\"\n", (i32)i, ARG[i]);
            }
            printf("\n");
        }

        // free, introduced with multiline
        for(i = 0; i <= argc; i++) {
            FREEZ(ARG[i])
        }

        cmd++;
        if(inputs) break;
    }
    if(!cmd) {
        fprintf(stderr, "\nError: the input BMS script is empty\n");
        myexit(QUICKBMS_ERROR_BMS);
    }
    return(cmd);
}



void mex_default_init(int file_only) {
    if(!file_only) EXTRCNT_idx   = add_var(0, "EXTRCNT", NULL, 0, sizeof(int)); // used by MultiEx as fixed variable
    BytesRead_idx = add_var(0, "BytesRead", NULL, 0, sizeof(int));              // used by MultiEx as fixed variable
    NotEOF_idx    = add_var(0, "NotEOF",    NULL, 1, sizeof(int));              // used by MultiEx as fixed variable
    EOF_idx       = add_var(0, "EOF",       NULL, myfilesize(0), sizeof(int));  // used by MultiEx as fixed variable
    SOF_idx       = add_var(0, "SOF",       NULL, 0, sizeof(int));              // used by MultiEx as fixed variable
}



void bms_init(int reinit) {
    int     i,
            j;

        bms_line_number     = 0;
        extracted_files     = 0;
        reimported_files    = 0;
        endian              = MYLITTLE_ENDIAN;
        //force_overwrite     = 0;
        //force_rename        = 0;
        compression_type    = COMP_ZLIB;
        file_xor_pos        = NULL;
        file_xor_size       = 0;
        file_rot13_pos      = NULL;
        file_rot13_size     = 0;
        file_crypt_pos      = NULL;
        file_crypt_size     = 0;
        comtype_dictionary_len = 0;
        comtype_scan        = 0;
        encrypt_mode        = 0;
        append_mode         = 0;
        temporary_file_used = 0;
        mex_default         = 0;
        file_xor            = NULL;
        file_rot13          = NULL;
        file_crypt          = NULL;
        comtype_dictionary  = NULL;
        //EXTRCNT_idx         = 0;
        //BytesRead_idx       = 0;
        //NotEOF_idx          = 0;

    if(mex_default) {
        mex_default_init(0);
    }
    CMD_Encryption_func(-1, 0);

    // input folder only: in case someone writes bad scripts
    //do NOT enable//
    /*for(i = 0; i < MAX_VARS; i++) {
        if(variable[i].name)  variable[i].name[0]  = 0;
        if(variable[i].value) variable[i].value[0] = 0;
        variable[i].value32 = 0;
    }*/
    // input folder only: enough useful
    for(i = 0; i < MAX_FILES; i++) {
        memory_file[i].pos  = 0;
        memory_file[i].size = 0;
    }

    if(reinit) return;

    // not done in reinit because they contain allocated stuff
    memset(filenumber,  0, sizeof(filenumber));
    variable = variable_main;
    memset(variable,    0, sizeof(variable_main));
    memset(command,     0, sizeof(command));
    memset(memory_file, 0, sizeof(memory_file));
    memset(array,       0, sizeof(array));
    for(i = 0; i < MAX_CMDS; i++) {
        for(j = 0; j < MAX_ARGS; j++) {
            command[i].var[j] = -0x7fffff;  // helps a bit to identify errors in this tool, DO NOT MODIFY IT! NEVER! (it's used in places like check_condition)
            command[i].num[j] = -0x7fffff;  // helps a bit to identify errors in this tool
            // do NOT touch command[i].str[j]
        }
    }
    CMD_CallDLL_func(-1, NULL, NULL);

    getcwd(current_folder, PATHSZ);
    quickbms_version = calc_quickbms_version(VER);
}



int bms_finish(void) { // totally useless function, except in write mode for closing the files
    int     i;

    for(i = 0; i < MAX_FILES; i++) {
        myfclose(i);
    }
    for(i = 0; i < MAX_FILES; i++) {
        myfclose(-i);
    }
    if(temporary_file_used) {
        fprintf(stderr, "\n- a temporary file was created, do you want to delete it (y/N): ");
        if(get_yesno(NULL) == 'y') {
            unlink(TEMPORARY_FILE);
        }
    }

    /*
    freeing memory is a problem if XDBG_ALLOC_ACTIVE has been disabled
    at runtime so... who cares, skip it!

    int     j;

    for(i = 0; i < MAX_FILES; i++) {
        FREEZ(filenumber[i].fullname)
        FREEZ(filenumber[i].filename)
        FREEZ(filenumber[i].basename)
        FREEZ(filenumber[i].fileext)
    }
    memset(filenumber, 0, sizeof(filenumber));

    variable = variable_main;
    for(i = 0; i < MAX_VARS; i++) {
        FREEZ_VAR(&variable[i]);
    }
    memset(variable, 0, sizeof(variable_main));

    for(i = 0; i < MAX_CMDS; i++) {
        FREEZ(command[i].debug_line)
        for(j = 0; j < MAX_ARGS; j++) {
            FREEZ(command[i].str[j])
        }
    }
    memset(command, 0, sizeof(command));

    for(i = 0; i < MAX_FILES; i++) {
        FREEZ(memory_file[i].data)
    }
    memset(memory_file, 0, sizeof(memory_file));

    for(i = 0; i < MAX_ARRAYS; i++) {
        for(j = 0; j < array[i].elements; j++) {
            FREEZ_VAR(&array[i].var[j]);
        }
        FREEZ(array[i].var)
    }
    memset(array, 0, sizeof(array));
    dumpa(0, NULL, NULL, -1, -1, -1);
    //unzip(0, NULL, 0, NULL, 0);
    bms_line(NULL, NULL, NULL, NULL, NULL);
    */

    // xdbg_toggle() is not ready yet
    //xdbg_freeall();

    return(0);
}


