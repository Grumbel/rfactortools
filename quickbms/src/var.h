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

// QuickBMS variables-related operations



/*
TODO and notes
var.size referred to var.name must be ever >= VAR_NAMESZ, this is an old
work-around introduced when I opted for strings allocated with a minimum
size of STRINGSZ.

check script:
    get VAR long
    print "%VAR%"
    set VAR2 string VAR
    print "%VAR2%"
*/



void check_variable_errors(int idx, variable_t *myvar) {
    variable_t  *var;

    if(myvar) var = myvar;
    else      var = &variable[idx];

    if(var->name) {
        if((var->name  != var->name_static)  && (var->name  != var->name_alloc)) {
            fprintf(stderr, "\nError: %svariable %"PRId" contains an invalid name pointer, contact me!\n", (var == myvar) ? "array " : "", idx);
            myexit(QUICKBMS_ERROR_BMS);
        }
    }
    if(var->value) {
        if((var->value != var->value_static) && (var->value != var->value_alloc)) {
            fprintf(stderr, "\nError: %svariable %"PRId" contains an invalid value pointer, contact me!\n", (var == myvar) ? "array " : "", idx);
            myexit(QUICKBMS_ERROR_BMS);
        }
    }
}



int get_memory_file(u8 *str) {
    int     ret = 0;    // because -1 is returned for MEMORY_FILE

    // MEMORY_FILE  = -1
    // MEMORY_FILE1 = -1
    // MEMORY_FILE2 = -2

    if(str) {
        ret = myatoi(str + MEMORY_FNAMESZ);
        if(!ret) ret++;
        if((ret < 0) || (ret > MAX_FILES)) {
            fprintf(stderr, "\nError: too big MEMORY_FILE number\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        ret = -ret;
    }
    if(ret >= 0) {
        fprintf(stderr, "\nError: the memory file has a positive number\n");
        myexit(QUICKBMS_ERROR_BMS);
    }
    return(ret);
}



int add_datatype(u8 *str) {
    if(str) {
        if(!stricmp(str, "Long"))       return(TYPE_LONG);
        if(!stricmp(str, "Int"))        return(TYPE_SHORT);
        if(!stricmp(str, "Byte"))       return(TYPE_BYTE);
        if(!stricmp(str, "ThreeByte"))  return(TYPE_THREEBYTE);
        if(!stricmp(str, "String"))     return(TYPE_STRING);
        if(!stricmp(str, "ASize"))      return(TYPE_ASIZE);
        // added by me
        if(!stricmp(str, "sLong"))              return(TYPE_SIGNED_LONG);
        if(!stricmp(str, "sInt"))               return(TYPE_SIGNED_SHORT);
        if(!stricmp(str, "sShort"))             return(TYPE_SIGNED_SHORT);
        if(!stricmp(str, "sByte"))              return(TYPE_SIGNED_BYTE);
        if(!stricmp(str, "sThreeByte"))         return(TYPE_SIGNED_THREEBYTE);
        if(!stricmp(str, "signed_Long"))        return(TYPE_SIGNED_LONG);
        if(!stricmp(str, "signed_Int"))         return(TYPE_SIGNED_SHORT);
        if(!stricmp(str, "signed_Short"))       return(TYPE_SIGNED_SHORT);
        if(!stricmp(str, "signed_Byte"))        return(TYPE_SIGNED_BYTE);
        if(!stricmp(str, "signed_ThreeByte"))   return(TYPE_SIGNED_THREEBYTE);
        if(stristr(str,  "bits"))       return(TYPE_BITS);
        if(!stricmp(str, "Longlong"))   return(TYPE_LONGLONG);
        //if(!stricmp(str, "Llong"))      return(TYPE_LONGLONG);
        if(!stricmp(str, "Short"))      return(TYPE_SHORT);
        if(!stricmp(str, "Char"))       return(TYPE_BYTE);
        if(!stricmp(str, "dword"))      return(TYPE_LONG);
        if(!stricmp(str, "word"))       return(TYPE_SHORT);
        if(!stricmp(str, "char16"))     return(TYPE_SHORT);
        if(!stricmp(str, "FileName"))   return(TYPE_FILENAME);
        if(!stricmp(str, "BaseName"))   return(TYPE_BASENAME);
        if(!stricmp(str, "FullName"))   return(TYPE_FULLNAME);
        if(!stricmp(str, "Extension"))  return(TYPE_EXTENSION);
        if(!stricmp(str, "FileExt"))    return(TYPE_EXTENSION);
        if(!stricmp(str, "current_folder")) return(TYPE_CURRENT_FOLDER);
        if(!stricmp(str, "file_folder")) return(TYPE_FILE_FOLDER);
        if(!stricmp(str, "input_folder")) return(TYPE_INOUT_FOLDER);
        if(!stricmp(str, "output_folder")) return(TYPE_INOUT_FOLDER);
        if(!stricmp(str, "bms_folder")) return(TYPE_BMS_FOLDER);
        if(!stricmp(str, "Unicode"))    return(TYPE_UNICODE);
        if(!stricmp(str, "UTF-16"))     return(TYPE_UNICODE);
        if(!stricmp(str, "UTF16"))      return(TYPE_UNICODE);
        if(!stricmp(str, "Binary"))     return(TYPE_BINARY);
        if(!stricmp(str, "Line"))       return(TYPE_LINE);
        if(!stricmp(str, "UTF-8"))      return(TYPE_STRING);
        if(!stricmp(str, "UTF8"))       return(TYPE_STRING);
        if(!stricmp(str, "Alloc"))      return(TYPE_ALLOC);
        if(!stricmp(str, "Compressed")) return(TYPE_COMPRESSED);
        if(!stricmp(str, "float"))      return(TYPE_FLOAT);
        if(!stricmp(str, "float32"))    return(TYPE_FLOAT);
        if(!stricmp(str, "double"))     return(TYPE_DOUBLE);
        if(!stricmp(str, "float64"))    return(TYPE_DOUBLE);
        if(!stricmp(str, "double64"))   return(TYPE_DOUBLE);
        if(!stricmp(str, "longdouble")) return(TYPE_LONGDOUBLE);
        if(!stricmp(str, "double96"))   return(TYPE_LONGDOUBLE);
        if(!stricmp(str, "bool"))       return(TYPE_LONG);
        if(!stricmp(str, "void"))       return(TYPE_LONG);
        if(!stricmp(str, "variable"))   return(TYPE_VARIABLE);
        if(!stricmp(str, "variable1"))  return(TYPE_VARIABLE);
        if(!stricmp(str, "variable2"))  return(TYPE_VARIABLE2);
        if(!stricmp(str, "variable3"))  return(TYPE_VARIABLE3);
        if(!stricmp(str, "variable4"))  return(TYPE_VARIABLE4);
        if(!stricmp(str, "unreal"))     return(TYPE_VARIABLE2);
        if(!stricmp(str, "variant"))    return(TYPE_VARIANT);
        if(!stricmp(str, "date"))       return(TYPE_TIME);
        if(!stricmp(str, "time"))       return(TYPE_TIME);
        if(!stricmp(str, "time_t"))     return(TYPE_TIME);
        if(!stricmp(str, "time32"))     return(TYPE_TIME);
        if(!stricmp(str, "timestamp"))  return(TYPE_TIME);
        if(!stricmp(str, "date64"))     return(TYPE_TIME64);
        if(!stricmp(str, "time64"))     return(TYPE_TIME64);
        if(!stricmp(str, "timestamp64"))return(TYPE_TIME);
        if(!stricmp(str, "FILETIME"))   return(TYPE_TIME64);
        if(!stricmp(str, "clsid"))      return(TYPE_CLSID);
        if(!stricmp(str, "classid"))    return(TYPE_CLSID);
        if(!stricmp(str, "uuid"))       return(TYPE_CLSID);
        if(!stricmp(str, "ip"))         return(TYPE_IPV4);
        if(!stricmp(str, "ipv4"))       return(TYPE_IPV4);
        if(!stricmp(str, "inet4"))      return(TYPE_IPV4);
        if(!stricmp(str, "ipv6"))       return(TYPE_IPV6);
        if(!stricmp(str, "inet6"))      return(TYPE_IPV6);
        if(!stricmp(str, "asm"))        return(TYPE_ASM);
        if(!stricmp(str, "assembly"))   return(TYPE_ASM);
        if(!stricmp(str, "assembler"))  return(TYPE_ASM);
        if(!stricmp(str, "x86"))        return(TYPE_ASM);

        // ever at the end!
        if(!stricmp(str, "unknown") || !stricmp(str, "?"))  return(TYPE_UNKNOWN);
        //if(!stricmp(str, "8"))          return(TYPE_LONGLONG);
        if(!stricmp(str, "4"))          return(TYPE_LONG);
        if(!stricmp(str, "3"))          return(TYPE_THREEBYTE);
        if(!stricmp(str, "2"))          return(TYPE_SHORT);
        if(!stricmp(str, "1"))          return(TYPE_BYTE);

        if(strstr(str,   "64"))         return(TYPE_LONGLONG);
        if(strstr(str,   "32"))         return(TYPE_LONG);
        if(strstr(str,   "24"))         return(TYPE_THREEBYTE);
        if(strstr(str,   "16"))         return(TYPE_SHORT);
        if(strstr(str,   "8"))          return(TYPE_BYTE);
        // nothing must be added here
    }
    fprintf(stderr, "\nError: invalid datatype %s at line %d\n", str, (i32)bms_line_number);
    myexit(QUICKBMS_ERROR_BMS);
    return(-1);
}



int get_var_from_name(u8 *name, int namelen) {  // a memory_file IS NOT a variable!
    int     i;

    if(!name) return(-1);
    if(namelen < 0) namelen = strlen(name);
    for(i = 0; variable[i].name; i++) {
        check_variable_errors(i, NULL);

        if(strlen(variable[i].name) != namelen) continue;

        // the following check is necessary because name may be longer than namelen
        if(!strnicmp(variable[i].name, name, namelen) && !variable[i].name[namelen]) return(i);
        //if(!strnicmp(variable[i].name, name, namelen) && (strlen(variable[i].name) == namelen)) return(i);
    }
    return(-1);
}



void check_idx(int idx) {
    if((idx < 0) || (idx >= MAX_VARS)) {
        fprintf(stderr, "\nError: the variable index is invalid (%"PRId"), there is an error in this tool\n", idx);
        myexit(QUICKBMS_ERROR_BMS);
    }
    check_variable_errors(idx, NULL);
}



int check_sub_vars(int idx, int create_if_unexistent);



/*
X1  value32
X2  value
X3  name
X4  memory_file
X5  array
*/

// do NOT enable X4 and memory files here or will be visualized an error!
#define GET_VAR_COMMON(X1,X2,X3,X4,X5) \
    int     sub_idx; \
    check_idx(idx); \
    if(variable[idx].sub_var && variable[idx].sub_var->active) { \
        sub_idx = check_sub_vars(idx, 0); \
        if(sub_idx < 0) { \
            printf("\nError: the specified coordinates of the multidimensional array don't exist\n"); \
            myexit(QUICKBMS_ERROR_BMS); \
        } \
        return(X5); \
    } \
    if(variable[idx].isnum) { \
        if(verbose > 0) printf("             <get %s (%d) 0x%"PRIx"\n", variable[idx].name, (i32)idx, variable[idx].value32); \
        /* else if(verbose < 0) printf("               %-10s 0x%08x\n", variable[idx].name, (i32)variable[idx].value32); */ \
        return(X1); \
    } \
    if(variable[idx].value) { \
        if(verbose > 0) printf("             <get %s (%d) \"%s\"\n", variable[idx].name, (i32)idx, variable[idx].value); \
        /* else if(verbose < 0) printf("               %-10s \"%s\"\n", variable[idx].name, variable[idx].value); */ \
        return(X2); \
    } \
    if(variable[idx].name[0] && strnicmp(variable[idx].name, MEMORY_FNAME, MEMORY_FNAMESZ)) { /* "" is for sequential file names */ \
        if(verbose > 0) printf("- variable \"%s\" seems uninitialized, I use its name\n", variable[idx].name); \
        /* else if(verbose < 0) printf("               %-10s \"%s\"\n", variable[idx].name, variable[idx].name); */ \
        /* myexit(QUICKBMS_ERROR_BMS); */ \
    } \
    if(verbose > 0) printf("             <get %s (%d) \"%s\"\n", variable[idx].name, (i32)idx, variable[idx].name); \
    /* else if(verbose < 0) printf("               %-10s \"%s\"\n", variable[idx].name, variable[idx].name); */ \
    return(X3);



u8 *get_varname(int idx) {
    if((idx < 0) || (idx >= MAX_VARS)) {
        //fprintf(stderr, "\nError: the variable index is invalid (%"PRId"), there is an error in this tool\n", idx);
        //myexit(QUICKBMS_ERROR_BMS);
        return("");
    }
    check_variable_errors(idx, NULL);
    return(variable[idx].name);
}



int get_var32(int idx) {
    GET_VAR_COMMON(
        variable[idx].value32,
        myatoi(variable[idx].value),
        myatoi(variable[idx].name),
        myatoi(memory_file[-get_memory_file(variable[idx].name)].data),
        myatoi(variable[idx].sub_var->array[sub_idx].data)
    )
}



u8 *get_var(int idx) {
    GET_VAR_COMMON(
        myitoa(variable[idx].value32),
        variable[idx].value,
        variable[idx].name,
        memory_file[-get_memory_file(variable[idx].name)].data,
        variable[idx].sub_var->array[sub_idx].data
    )
}



// return strlen of the above ones except for the memory file.
// deprecated: get_varsz will be abandoned
int get_varsz(int idx) {
    GET_VAR_COMMON(
        strlen(myitoa(variable[idx].value32)), //sizeof(int),
        strlen(variable[idx].value),    // do NOT use variable[idx].size or things like AES encryption will no longer work!
        strlen(variable[idx].name),
        memory_file[-get_memory_file(variable[idx].name)].size,
        strlen(variable[idx].sub_var->array[sub_idx].data)  //variable[idx].sub_var->array[sub_idx].size
    )
}



int var_is_a_string(int idx) {
    GET_VAR_COMMON(
        0,
        1,
        1,
        1,
        1
    )
}



int var_is_a_number(int idx) {
    GET_VAR_COMMON(
        1,
        0,
        0,
        0,
        0
    )
}



int var_is_a_memory_file(int idx) {
    GET_VAR_COMMON(
        0,
        0,
        1,  // uhmmm correct?
        1,
        0
    )
}



int var_is_a_constant(int idx) {
    check_idx(idx);
    if(variable[idx].constant) return(1);
    return(0);
}



int check_sub_vars(int idx, int create_if_unexistent) {
    static int  tmp32sz = 0;
    static int  *tmp32  = NULL;
    int     i,
            sz,
            sub_idx;

    check_idx(idx);
    if(!variable[idx].sub_var) return(-1);
    if(!variable[idx].sub_var->vars) return(-1);
    sz = variable[idx].sub_var->vars * sizeof(int);
    if(sz > tmp32sz) {
        tmp32 = realloc(tmp32, sz);
        if(!tmp32) STD_ERR(QUICKBMS_ERROR_MEMORY);
        tmp32sz = sz;
    }
    for(i = 0; i < variable[idx].sub_var->vars; i++) {
        sub_idx = get_var32(variable[idx].sub_var->var[i]);
        if(sub_idx < 0) {
            fprintf(stderr, "\nError: the sub_variable index for the array is invalid (%"PRId"->%"PRId")\n", i, sub_idx);
            myexit(QUICKBMS_ERROR_BMS);
        }
        tmp32[i] = sub_idx;
    }
    for(i = 0; i < variable[idx].sub_var->arrays; i++) {
        if(!memcmp(tmp32, variable[idx].sub_var->array[i].info, sz)) break;
    }
    if(i >= variable[idx].sub_var->arrays) {
        if(!create_if_unexistent) return(-1);
        if(variable[idx].sub_var->arrays == -1) ALLOC_ERR;
        variable[idx].sub_var->array = realloc(variable[idx].sub_var->array, (variable[idx].sub_var->arrays + 1) * sizeof(data_t));
        if(!variable[idx].sub_var->array) STD_ERR(QUICKBMS_ERROR_MEMORY);
        i = variable[idx].sub_var->arrays;
        variable[idx].sub_var->arrays++;
        memset(&variable[idx].sub_var->array[i], 0, sizeof(data_t));
        variable[idx].sub_var->array[i].info = malloc_copy(NULL, tmp32, sz);
    }
    return(i);
}



// I have chosen -2 because it's negative and is different than -1, a fast solution
int add_varval(int idx, /*u8 *name,*/ u8 *val, int val32, int valsz) {
    int     sub_idx;

    if(valsz != -2) {
        if(variable[idx].constant) return(-1); //goto quit_error;

        // experimental multi-dimensional arrays
        if(variable[idx].sub_var && variable[idx].sub_var->vars) {
            sub_idx = check_sub_vars(idx, 1);
            if(sub_idx < 0) {
                printf("\nError: multi dimensional add_varval error\n");
                myexit(QUICKBMS_ERROR_BMS);
            }
            if(val) {
                strdup_replace(&variable[idx].sub_var->array[sub_idx].data, val, valsz, &variable[idx].sub_var->array[sub_idx].size);
                //variable[idx].sub_var->array[sub_idx].isnum = 0;
            } else {
                strdup_replace(&variable[idx].sub_var->array[sub_idx].data, myitoa(val32), -1, &variable[idx].sub_var->array[sub_idx].size);
                //variable[idx].sub_var->array[sub_idx].isnum = 1;
            }
            if(!variable[idx].sub_var->active) variable[idx].sub_var->active = 1;
            return(0);
        }

        if(val) {
            if(valsz < 0) {
                valsz = strlen(val);
                if(valsz < 0) ALLOC_ERR;
                variable[idx].binary = 0;
            } else {
                variable[idx].binary = 1;
            }
            /*
            if(valsz < strlen(val)) {
                fprintf(stderr, "\nError: there is an error in this tool: valsz < strlen(val). Contact me!\n");
                myexit(QUICKBMS_ERROR_BMS);
            }
            */
            if(valsz <= VAR_VALUESZ) {
                memcpy(variable[idx].value_static, val, valsz);
                variable[idx].value_static[valsz] = 0;
                variable[idx].size = VAR_VALUESZ;   // valsz
                variable[idx].value = variable[idx].value_static;
            } else {
                strdup_replace(&variable[idx].value_alloc, val, valsz, &variable[idx].size);
                variable[idx].value = variable[idx].value_alloc;
            }
            variable[idx].isnum   = 0;
        } else {
            variable[idx].value32 = val32;
            variable[idx].isnum   = 1;
        }
    //} else {
        // avoids problems with commands like putvarchr
        // in short if the variable has no value (uninitialized) then it uses the
        // name but if this value gets reallocated then the name will continue
        // to point to the old value causing tons of problems

        // do not enable the following so the user can notice his errors,
        // for example a full script containing only:
        // putvarchr MYVAR 0x1000000 0
        //strdup_replace(&variable[idx].value, name, -1, &variable[idx].size);
    }
    return(0);
}



int *add_multi_dimensional(u8 *name, int *sub_vars);



int add_var(int idx, u8 *name, u8 *val, int val32, int valsz) {
    int     sub_vars    = 0,
            *sub_var    = NULL,
            t;

    // do NOT touch valsz, it's a job of strdup_replace
    check_idx(idx);
    //if((valsz == -2) && !name) name = ""; // specific for the ARGs, only in case of errors in my programming
    // if(valsz < 0) valsz = STRINGSZ;  do NOT do this, valsz is calculated on the length of val
    if(!name) {  // && (idx >= 0)) {
        //name = variable[idx].name; // unused
        if(add_varval(idx, val, val32, valsz) < 0) goto quit_error;
        //goto quit;
    } else {    // used only when the bms file is parsed at the beginning

        sub_var = add_multi_dimensional(name, &sub_vars);

        if(!stricmp(name, "EXTRCNT") || !stricmp(name, "BytesRead") || !stricmp(name, "NotEOF") || !stricmp(name, "SOF") || !stricmp(name, "EOF")) {
            if(!mex_default) {
                mex_default = 1;    // this avoids to waste cpu for these boring and useless variables
                mex_default_init(0);
            }
        }
        for(idx = 0; variable[idx].name; idx++) {
            if(!stricmp(variable[idx].name, name)) {
                if(add_varval(idx, val, val32, valsz) < 0) goto quit_error;
                goto quit;
            }
        }
        if(idx >= MAX_VARS) {
            fprintf(stderr, "\nError: the script uses more variables (%"PRId") than supported\n", idx);
            myexit(QUICKBMS_ERROR_BMS);
        }


        t = strlen(name);
        if(t < 0) ALLOC_ERR;
        if(t <= VAR_NAMESZ) {
            memcpy(variable[idx].name_static, name, t + 1);
            variable[idx].name = variable[idx].name_static;
            variable[idx].size = VAR_NAMESZ;    // t
        } else {
            strdup_replace(&variable[idx].name_alloc, name, t, &variable[idx].size);
            variable[idx].name = variable[idx].name_alloc;
        }


        if(add_varval(idx, val, val32, valsz) < 0) goto quit_error;

        if(!variable[idx].name[0]) {        // ""
            variable[idx].constant = 1;     // it's like read-only
        }

        // if this "if" is removed the tool will be a bit slower but will be able to handle completely the script in the example below
        if(myisdigitstr(variable[idx].name)) {  // removes the problem of Log "123.txt" 0 0
        //if(myisdigit(variable[idx].name[0])) {  // number: why only the first byte? because decimal and hex (0x) start all with a decimal number or a '-'
            //strdup_replace(&variable[idx].value, variable[idx].name, -1, &variable[idx].size);
            variable[idx].value32  = myatoi(variable[idx].name);
            variable[idx].isnum    = 1;
            variable[idx].constant = 1;     // it's like read-only

            // there is only one incompatibility with the string-only variables, but it's acceptable for the moment:
            //   set NAME string "mytest"
            //   set NUM long 0x1234
            //   string NAME += NUM
            //   print "%NAME%"
            //   set NUM string "0x12349999999999"
            //   string NAME += NUM
            //   print "%NAME%"
        }

        if(sub_var) {
            variable[idx].sub_var = calloc(sizeof(sub_variable_t), 1);
            if(!variable[idx].sub_var) STD_ERR(QUICKBMS_ERROR_MEMORY);
            variable[idx].sub_var->var  = sub_var;
            variable[idx].sub_var->vars = sub_vars;
        }
    }
quit:
    if(verbose > 0) {
        if(variable[idx].isnum) {
            printf("             >set %s (%"PRId") to 0x%"PRIx"\n", variable[idx].name, idx, variable[idx].value32);
        } else if(variable[idx].value) {
            printf("             >set %s (%"PRId") to \"%s\"\n", variable[idx].name, idx, variable[idx].value);
        } else {
            printf("             >set %s (%"PRId") to \"%s\"\n", variable[idx].name, idx, variable[idx].name);
        }
    /*} else if(verbose < 0) {
        if(variable[idx].isnum) {
            printf("             >%-10s 0x%"PRIx"\n", variable[idx].name, variable[idx].value32);
        } else if(variable[idx].value) {
            printf("             >%-10s \"%s\"\n", variable[idx].name, variable[idx].value);
        } else {
            printf("             >%-10s \"%s\"\n", variable[idx].name, variable[idx].name);
        } */
    }
    return(idx);
quit_error:
    fprintf(stderr, "\nError: there is something wrong in the BMS, var %d is a constant number\n", (i32)idx);
    myexit(QUICKBMS_ERROR_BMS);
    return(-1);
}



int *add_multi_dimensional(u8 *name, int *sub_vars) {
    int     ret = 0,
            op  = 0,
            cl  = 0,
            *sub_var    = NULL;
    u8      *s,
            *p,
            *l;

    if(sub_vars) *sub_vars = 0;
    if(!name) return(NULL);

    for(s = name; *s; s++) {
        if(*s == '[') op++;
        if(*s == ']') cl++;
    }
    if(op != cl) {
        printf("\nError: add_multi_dimensional with different number of [ and ]\n");
        myexit(QUICKBMS_ERROR_BMS);
    }
    if(!op) return(NULL);

    sub_var = calloc(op, sizeof(int));
    if(!sub_var) STD_ERR(QUICKBMS_ERROR_MEMORY);

    p = NULL;
    l = NULL;
    for(s = name; *s; s++) {
        if(*s == '[') {
            p = s;
            l = NULL;
            continue;
        } else if(*s == ']') {
            l = s;
        } else {
            continue;
        }
        if(!p || (l < p)) {
            printf("\nError: add_multi_dimensional error\n");
            myexit(QUICKBMS_ERROR_BMS);
        }
        *p = 0;
        *l = 0;
        sub_var[ret] = add_var(0, p + 1, NULL, 0, -2);
        *p = '[';
        *l = ']';
        p = NULL;
        l = NULL;
        ret++;
    }
    *sub_vars = ret;
    return(sub_var);
}



// this function is a partial work-around
void quick_var_from_name_check(u8 **ret_key, int *ret_keysz) {
    int     keysz = -1,
            tmp;
    u8      *key,
            *p;

    if(!ret_key || !*ret_key) return;
    if(ret_keysz) keysz = *ret_keysz;
    if(keysz >= (NUMBERSZ + 1)) return; // it's useless to make the check for keys over this size
    key = *ret_key;

    tmp = get_var_from_name(key, keysz);
    if(tmp >= 0) {  // variable
        p = get_var(tmp);
        if(p) {
            keysz = get_varsz(tmp);
            key   = p;
        }
    } else if(!strnicmp(key, MEMORY_FNAME, MEMORY_FNAMESZ)) {   // memory_file
        keysz = memory_file[-get_memory_file(key)].size;
        key   = memory_file[-get_memory_file(key)].data;
    }

    if(ret_keysz) *ret_keysz = keysz;
    *ret_key = key;
}



u8 *bad_chars_filter(u8 *s, int len) {
    int     i;

    for(i = 0; i < len; i++) {
        if(!s[i]) break;
        //if(!isprint(s[i])) return("");    // doesn't work on linux
        //if(s[i] == '\r') continue;    // one line only
        //if(s[i] == '\n') continue;    // one line only
        if((s[i] >= ' ') && (s[i] < 0x7f)) continue;
        return("");
    }
    if(s[i]) return("");
    return(s);
}



int verbose_print(int offset, u8 *cmd, int idx, u8 *str, i32 strsz, int num, i32 more) {
    i32     len;
    u8      *name;

    name = get_varname(idx);
    if(!strcmp(name, QUICKBMS_DUMMY)) return(-1);   // no need of stricmp
    if(str) {
        len = strsz;
        if(strsz < 0) len = strlen(str);
        printf(". %"PRIx" %-7s %-10s \"%.*s\" %d\n", offset, cmd, name, len, bad_chars_filter(str, len), more);
        if(strsz > 0) show_dump(4, str, strsz, stdout);
    } else {
        printf(". %"PRIx" %-7s %-10s 0x%"PRIx" %d\n", offset, cmd, name, num, more);
    }
    return(0);
}



void variable_copy(variable_t *output, variable_t *input, int keep_content) {
    int     i;
    u8      *name,
            *name_alloc,
            name_static[VAR_NAMESZ + 1],
            *value,
            *value_alloc,
            value_static[VAR_VALUESZ + 1];

    if(!output || !input) return;

    if(keep_content) {
        name_alloc = output->name_alloc;
        if(output->name == output->name_static) {
            strncpy(name_static, output->name, VAR_NAMESZ + 1);
            name = name_static;
        } else {
            name = name_alloc;
        }

        value_alloc = output->value_alloc;
        if(output->value == output->value_static) {
            if(output->size > VAR_VALUESZ) STD_ERR(QUICKBMS_ERROR_BMS);
            memcpy(value_static, output->value_static, output->size + 1); //VAR_VALUESZ + 1);
            value = value_static;
        } else {
            value = value_alloc;
        }
    }

    memcpy(output, input, sizeof(variable_t));

    if(keep_content) {
        output->name_alloc = name_alloc;
        if(name == name_static) {
            strncpy(output->name_static, name_static, VAR_NAMESZ + 1);
            output->name = output->name_static;
        } else {
            output->name = output->name_alloc;
        }

        output->value_alloc = value_alloc;
        if(value == value_static) {
            if(output->size > VAR_VALUESZ) STD_ERR(QUICKBMS_ERROR_BMS);
            memcpy(output->value_static, value_static, output->size + 1); //VAR_VALUESZ + 1);
            output->value = output->value_static;
        } else {
            output->value = output->value_alloc;
        }

    } else {
        output->name = NULL;
        output->name_alloc = NULL;
        output->name_static[0] = 0;     //memset(output->name_static, 0, VAR_NAMESZ + 1);

        if(!input->name) {
        } else if(input->name == input->name_static) {  // don't use strlen, consider possible binary names
            strncpy(output->name_static, input->name, VAR_NAMESZ + 1);
            output->name = output->name_static;
        } else {
            re_strdup(&output->name_alloc, input->name, NULL); // not needed
            output->name = output->name_alloc;
        }

        output->value = NULL;
        output->value_alloc = NULL;
        output->value_static[0] = 0;    //memset(output->value_static, 0, VAR_VALUESZ + 1);
    }

    if(input->value) {
        if(output->size == -1) ALLOC_ERR;
        if(output->size <= VAR_VALUESZ) {
            output->value = output->value_static;
        } else {
            myalloc(&output->value_alloc, output->size + 1, NULL); /* lame, there is a heap corruption mah... */
            output->value = output->value_alloc;
        }
        memcpy(output->value, input->value, output->size);
        output->value[output->size] = 0; /* final NULL byte or just memcpy output->size + 1 */
        if(output->size <= VAR_VALUESZ) output->size = VAR_VALUESZ;
    } else {
        // output->value = input->name

        if(keep_content) {
            if(!input->name) {
                output->value = NULL;
                output->size = 0;
            } else if(input->name == input->name_static) {  // consider names with binary data so avoid strlen
                strncpy(output->value_static, input->name, VAR_VALUESZ + 1);    // never use memcpy
                output->value = output->value_static;
                output->size = VAR_NAMESZ; //strlen(output->value);   // output->size = input->size ???
            } else {
                strdup_replace(&output->value_alloc, input->name, -1, &output->size);   // old method
                output->value = output->value_alloc;
            }
        }
    }

    if(input->sub_var) {
        output->sub_var        = malloc_copy(keep_content ? output->sub_var        : NULL, input->sub_var,        sizeof(sub_variable_t));
        output->sub_var->var   = malloc_copy(keep_content ? output->sub_var->var   : NULL, input->sub_var->var,   input->sub_var->vars * sizeof(int));
        output->sub_var->array = malloc_copy(keep_content ? output->sub_var->array : NULL, input->sub_var->array, input->sub_var->arrays * sizeof(data_t));
        for(i = 0; i < input->sub_var->arrays; i++) {
            output->sub_var->array[i].info = malloc_copy(keep_content ? output->sub_var->array[i].info : NULL, input->sub_var->array[i].info, input->sub_var->vars * sizeof(int));
            output->sub_var->array[i].data = malloc_copy(keep_content ? output->sub_var->array[i].data : NULL, input->sub_var->array[i].data, input->sub_var->array[i].size);
        }
    }

    output->constant = 0;
}



void FREEZ_VAR(variable_t *X) {
    int     i;

    if(!X) return;
    X->name  = NULL;
    X->value = NULL;
    FREEZ(X->name_alloc)
    FREEZ(X->value_alloc)
    if(X->sub_var) {
        FREEZ(X->sub_var->var)
        for(i = 0; i < X->sub_var->arrays; i++) {
            FREEZ(X->sub_var->array[i].info)
            FREEZ(X->sub_var->array[i].data)
        }
        FREEZ(X->sub_var->array)
        FREEZ(X->sub_var)
    }
    //memset(X, 0, sizeof(variable_t)); // not needed
}

