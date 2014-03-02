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



int     overwrite       = 0;
u8      *filter_in_files    = NULL;



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



u8 *fd_read(u8 *name, int *fdlen) {
    struct stat xstat;
    FILE    *fd;
    u8      *buff;

    printf("- open file %s\n", name);
    fd = fopen(name, "rb");
    if(!fd) std_err(NULL);
    fstat(fileno(fd), &xstat);
    buff = malloc(xstat.st_size);
    fread(buff, xstat.st_size, 1, fd);
    fclose(fd);
    *fdlen = xstat.st_size;
    return(buff);
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
            printf("- create folder %s\n", name);
            MAKEDIR(name);
        } else if(!S_ISDIR(xstat.st_mode)) {
            printf("\nError: %s is not a folder\n", name);
            std_err("");
        }

        *p = PATHSLASH;
    }
    return(name);
}



void fd_write(u8 *name, u8 *data, int datasz) {
    FILE    *fd;
    u8      ans[16];

    name = create_dir(name);

    printf("- create file %s\n", name);
    if(!overwrite) {
        fd = fopen(name, "rb");
        if(fd) {
            fclose(fd);
            printf("- file already exists, do you want to overwrite it (y/N/all)?\n  ");
            fflush(stdin);
            fgets(ans, sizeof(ans), stdin);
            if(ans[0] == 'a') overwrite = 1;
            else if(ans[0] != 'y') exit(1); // terminate immediately
        }
    }
    fd = fopen(name, "wb");
    if(!fd) std_err(NULL);
    fwrite(data, datasz, 1, fd);
    fclose(fd);
}

