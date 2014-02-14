/*
mydownlib
by Luigi Auriemma
e-mail: aluigi@autistici.org
web:    aluigi.org

    Copyright 2006-2014 Luigi Auriemma

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

// note that these functions contain some experimental code that
// I consider a "work-around" like SSL and global cookies support

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include <zlib.h>
#include "mydownlib.h"

#ifdef WIN32
    #include <winsock.h>

    #define close       closesocket
    #define in_addr_t   u32
    #define TEMPOZ1
    #define TEMPOZ2     GetTickCount()
    #define ONESEC      1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <sys/times.h>
    #include <sys/timeb.h>

    #define stricmp     strcasecmp
    #define strnicmp    strncasecmp
    #define TEMPOZ1     ftime(&timex)
    #define TEMPOZ2     ((timex.time * 1000) + timex.millitm)
    #define ONESEC      1
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;



#define VISDELAY    500
#define BUFFSZ      8192
#define MAXTIMEOUT  60          // one minute
#define MAXARGS     16          // modify it if you need more args in mydown_scanhead
#define MAXDNS      32
#define VERPRINTF   if(verbose >= 0) fprintf(stderr,
#define VERPRINTF2  if(verbose > 0)  fprintf(stderr,
#define TEMPOZ(x)   TEMPOZ1;    \
                    x = TEMPOZ2

#ifdef MYDOWN_SSL
    #include <openssl/ssl.h>    // link with libssl.a libcrypto.a -lgdi32
#else                           // on linux: gcc -o code code.c -lssl -lcrypto -lpthread
    #define SSL     char
    #define SSL_CTX char
    #define SSL_read(A,B,C)     0
    #define SSL_write(A,B,C)    0
    #define SSL_shutdown(X)     mydown_donothing()
    #define SSL_free(X)         mydown_donothing()
    #define SSL_CTX_free(X)     mydown_donothing()
#endif
#define SSL_COMPAT(X)   SSL_CTX_set_cipher_list(X, "ALL"); \
                        SSL_CTX_set_options(X, SSL_OP_ALL);



typedef struct {                // lame DNS caching implementation
    u8          *host;
    in_addr_t   ip;
} dns_db_t;

int         dns_db_max      = 0,
            dns_db_add      = 0;
dns_db_t    dns_db[MAXDNS];
u8          *global_cookie  = NULL; // used only with -DMYDOWN_GLOBAL_COOKIE



void mydown_donothing(void) {}
int mydown_get_host(u8 *url, u8 **hostx, u16 *portx, u8 **urix, u8 **userx, u8 **passx, int verbose);
u8 *mydown_uri2hex(u8 *uri);
u8 *mydown_hex2uri(u8 *uri);
void mydown_scanhead(u8 *data, int datalen, ...);
u8 *mydown_tmpnam(void);
void mydown_free_sock(SSL *ssl_sd, SSL_CTX *ctx_sd, int *sock);
u8 *mydown_http_skip(u8 *buff, int len, int *needed, int *remain);
void mydown_free(u8 **buff);
int mydown_chunked_skip(u8 *buff, int chunkedsize);
int mydown_unzip(z_stream z, u8 *in, int inlen, u8 **outx, int *outxlen);
int mydown_sscanf_hex(u8 *data, int datalen);
int mydown_timeout(int sock, int secs);
int mydown_block_recv(SSL *ssl_sd, int sd, u8 *data, int len, int timeout);
u8 *mydown_showhttp80(u16 port);
void mydown_showstatus(u32 fsize, u32 ret, u32 oldret, int timediff);
u8 *mydown_base64_encode(u8 *data, int *length);
in_addr_t mydown_resolv(char *host);
char *mydown_stristr(const char *String, const char *Pattern);



int mydown_send(SSL *ssl_sd, int sd, u8 *data, int datasz) {
    if(ssl_sd) return(SSL_write(ssl_sd, data, datasz));
    return(send(sd, data, datasz, 0));
}



int mydown_recv(SSL *ssl_sd, int sd, u8 *data, int datasz) {
    int     ret;
    if(ssl_sd) ret = SSL_read(ssl_sd, data, datasz);
    else       ret = recv(sd, data, datasz, 0);
    //if(ret > 0) show_dump(2, data, ret, stdout);
    //if(ret > 0) fwrite(data, 1, ret, stdout);
    return ret;
}



#define mydown_opt_pop \
    if(opt) { \
        if(!from)       from        = opt->from; \
        if(!tot)        tot         = opt->tot; \
        if(!showhead)   showhead    = opt->showhead; \
        if(!resume)     resume      = opt->resume; \
        if(!onlyifdiff) onlyifdiff  = opt->onlyifdiff; \
        if(!user)       user        = opt->user; \
        if(!pass)       pass        = opt->pass; \
        if(!referer)    referer     = opt->referer; \
        if(!useragent)  useragent   = opt->useragent; \
        if(!cookie)     cookie      = opt->cookie; \
        if(!more_http)  more_http   = opt->more_http; \
        if(!verbose)    verbose     = opt->verbose; \
        if(!filedata)   filedata    = opt->filedata; \
        if(!keep_alive) keep_alive  = opt->keep_alive; \
        if(!timeout)    timeout     = opt->timeout; \
        if(!ret_code)   ret_code    = opt->ret_code; \
        if(!onflyunzip) onflyunzip  = opt->onflyunzip; \
        if(!content)    content     = opt->content; \
        if(!contentsize)contentsize = opt->contentsize; \
        if(!get)        get         = opt->get; \
        if(!proxy)      proxy       = opt->proxy; \
        if(!proxy_port) proxy_port  = opt->proxy_port; \
    }

#define mydown_opt_push \
    if(opt) { \
        opt->from        = from; \
        opt->tot         = tot; \
        opt->showhead    = showhead; \
        opt->resume      = resume; \
        opt->onlyifdiff  = onlyifdiff; \
        opt->user        = user; \
        opt->pass        = pass; \
        opt->referer     = referer; \
        opt->useragent   = useragent; \
        opt->cookie      = cookie; \
        opt->more_http   = more_http; \
        opt->verbose     = verbose; \
        opt->filedata    = filedata; \
        opt->keep_alive  = keep_alive; \
        opt->timeout     = timeout; \
        opt->ret_code    = ret_code; \
        opt->onflyunzip  = onflyunzip; \
        opt->content     = content; \
        opt->contentsize = contentsize; \
        opt->get         = get; \
        opt->proxy       = proxy; \
        opt->proxy_port  = proxy_port; \
        if(global_cookie) global_cookie = cookie; \
    }



u32 mydown(u8 *myurl, u8 *filename, mydown_options *opt) {
    FILE    *fd         = NULL;
    u32     from        = 0,
            tot         = 0,
            filesize    = MYDOWN_ERROR;
    int     showhead    = 0,
            resume      = 0,
            onlyifdiff  = 0,
            verbose     = 0,
            *keep_alive = 0,
            *ret_code   = 0,
            timeout     = 0,
            onflyunzip  = 0,
            contentsize = 0,
            dossl       = 0;
    u16     port        = 0,
            proxy_port  = 0;
    u8      *url        = NULL,
            *uri        = NULL,
            *host       = NULL,
            *user       = NULL,
            *pass       = NULL,
            *referer    = NULL,
            *useragent  = NULL,
            *cookie     = NULL,
            *more_http  = NULL,
            **filedata  = NULL,
            *content    = NULL,
            *get        = NULL,
            *proxy      = NULL;

#ifdef WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    mydown_opt_pop
    if(global_cookie) cookie = global_cookie;
    if(user) {
        VERPRINTF2
            "  user   %s\n"
            "  pass   %s\n",
            user,
            pass);
    }

    if(!myurl) return(MYDOWN_ERROR);
    url = strdup(myurl);
    if(!url) return(MYDOWN_ERROR);
    dossl = mydown_get_host(url, &host, &port, &uri, user ? NULL : &user, pass ? NULL : &pass, verbose);

    VERPRINTF2"  start download\n");

    filesize = mydown_http2file(
        keep_alive,         // int *sock
        timeout,            // int timeout
        host,               // u8 *host
        port,               // u16 port
        user,               // u8 *user
        pass,               // u8 *pass
        referer,            // u8 *referer
        useragent,          // u8 *useragent
        cookie,             // u8 *cookie
        more_http,          // u8 *more_http
        verbose,            // int verbose
        uri,                // u8 *getstr
        fd,                 // FILE *fd
        filename,           // u8 *filename
        showhead,           // int showhead
        (dossl << 1) | onlyifdiff,         // int onlyifdiff
        resume,             // int resume
        from,               // u32 from
        tot,                // u32 tot
        NULL,               // u32 *filesize
        filedata,           // u8 **filedata
        ret_code,           // int *ret_code
        onflyunzip,         // int onflyunzip
        content,            // u8 *content
        contentsize,        // int contentsize
        get,                // u8 *get
        proxy,
        proxy_port,
        NULL
    );

    if(fd && (fd != stdout)) fclose(fd);
    mydown_free(&url);
    mydown_free(&uri);
    return(filesize);
}



int mydown_get_host(u8 *url, u8 **hostx, u16 *portx, u8 **urix, u8 **userx, u8 **passx, int verbose) {
    int     dossl = 0;
    u16     port  = 80;
    u8      *host = NULL,
            *uri  = NULL,
            *user = NULL,
            *pass = NULL,
            *p;

    if(!url) goto quit;
    host = url;

    p = strstr(host, "://");    // handle http://
    if(!p) p = strstr(host, ":\\\\");
    if(p) {
        if(!strnicmp(host, "https", 5)) {
            dossl = 1;
            port  = 443;
        }
        for(p += 3; *p; p++) {  // in case of http:////
            if((*p != '/') && (*p != '\\')) break;
        }
        host = p;
    }

    for(p = host; *p; p++) {    // search the uri
        if((*p == '/') || (*p == '\\')) {
            uri = p;
            break;
        }
    }
    if(uri) {
        *uri++ = 0;
        uri = mydown_uri2hex(uri);
    }
    if(!uri) uri = strdup("");  // in case of no uri or mydown_uri2hex fails

    p = strrchr(host, '@');
    if(p) {
        *p = 0;

        user = host;

        pass = strchr(host, ':');
        if(pass) {
            *pass++ = 0;
        } else {
            pass = "";
        }

        host = p + 1;
    }

    p = strchr(host, ':');
    if(p) {
        *p = 0;
        port = atoi(p + 1);
    }

    VERPRINTF"  %s\n", url);
    VERPRINTF2
        "  host   %s : %u\n"
        "  uri    %s\n",
        host, port,
        uri);

    if(user) {
        VERPRINTF2
            "  user   %s\n"
            "  pass   %s\n",
            user,
            pass);
    }

quit:
    if(hostx) *hostx = host;
    if(portx) *portx = port;
    if(urix)  *urix  = uri;
    if(userx) *userx = user;
    if(passx) *passx = pass;
    return(dossl);
}



u8 *mydown_http_delimit(u8 *data, u8 *limit, int *mod) {
    u8      *p;

    if(mod) *mod = -1;
    if(!data || !data[0]) return(NULL);

    for(p = data;; p++) {
        if(p >= limit) return(NULL);
        if(!*p) return(NULL);
        if((*p == '\r') || (*p == '\n')) break;
    }
    *p = 0;
    if(mod) *mod = p - data;
    for(p++;; p++) {
        if(p >= limit) return(NULL);
        if(!*p) return(NULL);
        if((*p != '\r') && (*p != '\n')) break;
    }
    return(p);
}



u8 *mydown_uri2hex(u8 *uri) {
    static const u8 hex[] = "0123456789abcdef";
    u8      *ret,
            *p,
            c;

    if(!uri) return(NULL);
    ret = calloc((strlen(uri) * 3) + 1, 1);
    if(!ret) return(NULL);

    for(p = ret; *uri; uri++) {
        c = *uri;
        if(isprint(c) && !strchr(" \"<>#" /*"%\\"*/ "{}|^~[]`", c)) {   // I have left the backslash and the percentage out
            *p++ = c;
        } else {
            *p++ = '%';
            *p++ = hex[c >> 4];
            *p++ = hex[c & 15];
        }
    }
    *p = 0;

    return(ret);
}



u8 *mydown_hex2uri(u8 *uri) {
    int     t,
            n;
    u8      *ret,
            *p;

    if(!uri) return(NULL);
    ret = strdup(uri);
    if(!ret) return(NULL);

    for(p = ret; *uri; uri++, p++) {
        t = *uri;
        if((*uri == '%') && (*(uri + 1) == 'u')) {
            if(sscanf(uri + 1, "u%04x", &t) == 1) uri += 5;
        } else if(*uri == '%') {
            if(sscanf(uri + 1, "%02x",  &t) == 1) uri += 2;
        } else if(*uri == '&') {
            if(*(uri + 1) == '#') {
                if((sscanf(uri + 1, "#%d;%n", &t, &n) == 1) && (n > 2)) {
                    uri += n;
                } else {
                    t = *uri;
                }
            } else if(!strnicmp(uri, "&quot;",   6)) {
                t = '\"';
                uri += 5;
            } else if(!strnicmp(uri, "&amp;",    5)) {
                t = '&';
                uri += 4;
            } else if(!strnicmp(uri, "&frasl;",  7)) {
                t = '/';
                uri += 6;
            } else if(!strnicmp(uri, "&lt;",     4)) {
                t = '<';
                uri += 3;
            } else if(!strnicmp(uri, "&gt;",     4)) {
                t = '>';
                uri += 3;
            } else if(!strnicmp(uri, "&nbsp;",   6)) {
                t = 160;
                uri += 5;
            } else if(!strnicmp(uri, "&middot;", 8)) {
                t = 183;
                uri += 7;
            }
        }
        *p = t;
    }
    *p = 0;

    return(ret);
}



// can be used only one time because modifies the input!
void mydown_scanhead(u8 *data, int datalen, ...) {
    va_list ap;
    int     i,
            mod,
            vals,
            cookie_len  = 0;
    u8      *par[MAXARGS],
            **val[MAXARGS],
            *l,
            *p,
            *t,
            *datal,
            *limit;

    if(!data) return;
    if(datalen <= 0) return;

    va_start(ap, datalen);
    for(i = 0; i < MAXARGS; i++) {
        par[i] = va_arg(ap, u8 *);
        if(!par[i]) break;
        val[i] = va_arg(ap, u8 **);
        if(!val[i]) break;
        *val[i] = NULL;
    }
    vals = i;
    va_end(ap);

    for(limit = data + datalen; data; data = l) {   // in case data becomes NULL
        l = mydown_http_delimit(data, limit, &mod);
        datal = strchr(data, ':');
        if(datal) {
            *datal = 0; // restore later
            for(i = 0; i < vals; i++) {
                if(stricmp(data, par[i])) continue;
                for(p = datal + 1; *p && ((*p == ' ') || (*p == '\t')); p++);
                if(!stricmp(data, "Set-Cookie")) {
                    t = strchr(p, ';');
                    if(t) *t = 0;   // restore later
                    *val[i] = realloc(*val[i], cookie_len + 2 + strlen(p) + 1);
                    if(*val[i]) cookie_len += sprintf(*val[i] + cookie_len, "%s%s", cookie_len ? "; " : "", p);
                    if(t) *t = ';';
                } else {
                    *val[i] = p;
                }
                break;
            }
            *datal = ':';
        }
        //if(mod >= 0) data[mod] = '\r';  // never enable this, the data must remain modified
        if(!l) break;
    }
}



// fname is provided by the user or from the network, the cleaning here is intended only
// about the bad chars not supported by the file-system, it's not a security function
void mydown_filename_clean(u8 *fname) {
    u8      *p;

    if(!fname) return;
    p = strchr(fname, ':'); // the first ':' is allowed
    if(p) fname = p + 1;
    for(p = fname; *p && (*p != '\r') && (*p != '\n'); p++) {
        if(strchr("?%*:|\"<>", *p)) {   // invalid filename chars not supported by the most used file systems
            *p = '_';
        }
    }
    if(*p) *p = 0;
    for(p--; (p >= fname) && ((*p == ' ') || (*p == '.')); p--) *p = 0;   // remove final spaces and dots
}



u32 mydown_http2file(int *keep_alive, int timeout, u8 *host, u16 port, u8 *user, u8 *pass, u8 *referer, u8 *useragent, u8 *cookie, u8 *more_http, int verbose, u8 *getstr, FILE *fd, u8 *filename, int showhead, int onlyifdiff, int resume, u32 from, u32 tot, u32 *filesize, u8 **filedata, int *ret_code, int onflyunzip, u8 *content, int contentsize, u8 *get, u8 *proxy, u16 proxy_port, mydown_options *opt) {
#ifndef WIN32
    struct  timeb   timex;
#endif
    static struct linger ling = {1,1};
    z_stream    z;
    FILE    *oldfd          = fd;
    struct  sockaddr_in peer;
    struct  stat    xstat;
    time_t  oldtime         = 0,
            newtime;
    u32     ret             = 0,
            httpret         = 0,
            vishttpret      = 0,
            fsize           = 0;
    int     sd              = 0,
            t,
            err,
            len,
            code            = 0,
            b64len,
            filedatasz      = 0,
            httpcompress    = 0,
            httpgzip        = 0,
            httpdeflate     = 0,
            httpz           = 0,
            chunked         = 0,
            chunkedsize     = 0,
            chunkedlen      = 0,
            wbits,
            zbufflen        = 0,
            httpskipbytes   = 0;
    u8      tmp[32],
            *buff           = NULL,
            *query          = NULL,
            *data           = NULL,
            *header_limit   = NULL,
            *s              = NULL,
            *userpass       = NULL,
            *b64            = NULL,
            *conttype       = NULL,
            *contlen        = NULL,
            *contdisp       = NULL,
            *icyname        = NULL,
            *transenc       = NULL,
            *contenc        = NULL,
            *connection     = NULL,
            *location       = NULL,
            *filedatatmp    = NULL,
            *zbuff          = NULL,
            *ztmp           = NULL,
            *chunkedbuff    = NULL,
            *chunkedtmp     = NULL,
            *filenamemalloc = NULL,
            *filename_tmp1  = NULL,
            *filename_tmp2  = NULL,
            *remote_filename= NULL,
            httpgzip_flags  = 0;

    int     dossl           = 0;
    SSL     *ssl_sd         = NULL;
    SSL_CTX *ctx_sd         = NULL;
#ifdef MYDOWN_SSL
    static int  ssl_loaded  = 0;
    SSL_METHOD  *ssl_method = NULL;
#endif

#define GOTOQUIT    { ret = MYDOWN_ERROR; goto quit; }

    mydown_opt_pop

    if(onlyifdiff) {
        // lame work-around to avoid to use additional fields
        // ******** ******** ******** ***22221
        dossl = (onlyifdiff >> 1) & 15; // 4 bits
        onlyifdiff = onlyifdiff & 1;    // 1 bit
    }
    if(dossl) keep_alive = NULL;  // ssl_sd and ctx_sd don't get saved at the moment

    if(!keep_alive || !*keep_alive) {
        if(proxy) {
            peer.sin_port        = htons(proxy_port);
            peer.sin_addr.s_addr = mydown_resolv(proxy);
        } else {
            peer.sin_port        = htons(port);
            peer.sin_addr.s_addr = mydown_resolv(host);
        }
        if(peer.sin_addr.s_addr == INADDR_NONE) GOTOQUIT;
        peer.sin_family      = AF_INET;

        sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sd < 0) GOTOQUIT;
        setsockopt(sd, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
        if(keep_alive) *keep_alive = sd;

        VERPRINTF2"  connect to %s:%u...", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
        if(connect(sd, (struct sockaddr *)&peer, sizeof(struct sockaddr_in)) < 0) {
            fprintf(stderr, "\nError: connection refused\n");
            GOTOQUIT;
        }
        VERPRINTF2"done\n");

        if(proxy && dossl) {    // requires CONNECT
            len = 64 + strlen(host) + 1 + 6 + 64 + 1;
            query = malloc(len);
            len = sprintf(
                query,
                "CONNECT %s:%u HTTP/1.0\r\n"
                "\r\n",
                host, port);
            if(mydown_send(NULL, sd, query, len) != len) GOTOQUIT;
            mydown_free(&query);

            len = 0;
            while((t = mydown_block_recv(NULL, sd, tmp, 1, timeout)) > 0) {
                if((tmp[0] == '\r') || (tmp[0] == '\n')) len++;
                else                                     len = 0;
                if(len >= 4) break;
            }
        }

#ifdef MYDOWN_SSL
        if(dossl) {
            if(!ssl_loaded) {
                SSL_library_init();
                //SSL_load_error_strings();
                ssl_loaded = 1;
            }
            switch(dossl) {
                case 1:  ssl_method = (SSL_METHOD *)SSLv23_method();    break;
                case 2:  ssl_method = (SSL_METHOD *)SSLv2_method();     break;
                case 3:  ssl_method = (SSL_METHOD *)SSLv3_method();     break;
                case 4:  ssl_method = (SSL_METHOD *)DTLSv1_method();    break;
                case 5:  ssl_method = (SSL_METHOD *)TLSv1_method();     break;
                default: ssl_method = NULL;                             break;
            }
            if(!ssl_method) GOTOQUIT
            t = 0;
            ctx_sd = SSL_CTX_new(ssl_method);
            if(!ctx_sd) {
                t = -1;
            } else {
                SSL_COMPAT(ctx_sd)
                ssl_sd = SSL_new(ctx_sd);
                SSL_set_fd(ssl_sd, sd);
                if(SSL_connect(ssl_sd) < 0) t = -1;
            }
            if(t < 0) {
                dossl++;
                VERPRINTF2"  try ssl method %u\n", dossl);
                if(sd && !keep_alive) mydown_free_sock(ssl_sd, ctx_sd, &sd);
                mydown_free_sock(ssl_sd, ctx_sd, keep_alive);
                ret = mydown_http2file(keep_alive, timeout, host, port, user, pass, referer, useragent, cookie, more_http, verbose, getstr, fd, filename, showhead, (dossl << 1) | onlyifdiff, resume, from, tot, filesize, filedata, ret_code, onflyunzip, content, contentsize, get, proxy, proxy_port, opt);
                goto quit;
            }
        }
#endif
    } else {
        sd = *keep_alive;
    }

    if(user && pass) {
        userpass = malloc(strlen(user) + 1 + strlen(pass) + 1);
        if(!userpass) GOTOQUIT;
        b64len = sprintf(userpass, "%s:%s", user, pass);
        b64 = mydown_base64_encode(userpass, &b64len);
    }

    if(!get) get = "GET";
    if(content && !contentsize) contentsize = strlen(content);

    len =
        400                                       + // my format strings and HTTP parameters
        strlen(host)                              + // http proxy
        6                                         +
        strlen(get)                               +
        strlen(getstr)                            +
        strlen(host)                              +
        6                                         +
        ((from || tot)   ? 20 + 20           : 0) +
        (b64             ? strlen(b64)       : 0) +
        (referer         ? strlen(referer)   : 0) +
        (useragent       ? strlen(useragent) : 0) +
        (cookie          ? strlen(cookie)    : 0) +
        (more_http       ? strlen(more_http) : 0) +
        1;

    query = calloc(len, 1);
    if(!query) GOTOQUIT;

    if(port == 80) tmp[0] = 0;
    else           sprintf(tmp, ":%u", port);

#define HTTP_APPEND  len += sprintf(query + len,

    len = 0;
    HTTP_APPEND "%s ", get);
    if(proxy && !dossl) HTTP_APPEND "http://%s%s", host, tmp);
    HTTP_APPEND "/%s HTTP/1.1\r\n", getstr);
    HTTP_APPEND "Host: %s%s\r\n", host, tmp);
    HTTP_APPEND "Connection: %s\r\n", keep_alive ? "keep-alive" : "close");
    HTTP_APPEND "Pragma: no-cache\r\n");        // useful?
    HTTP_APPEND "Cache-Control: no-cache\r\n"); // useful?

    if(from || tot) {
        HTTP_APPEND "Range: bytes=");
        if(from != -1) HTTP_APPEND "%u", from);
        HTTP_APPEND "-");
        if(tot >= 1)   HTTP_APPEND "%u", (tot - 1) + ((from == -1) ? 0 : from));
        HTTP_APPEND "\r\n");
    } else {
        // unfortunately doesn't seem possible to use chunks with Range, example:
        // mydown -m 8 http://ftp.hp.com/pub/softpaq/sp50501-51000/sp50693.exe
        if(!onlyifdiff && (showhead != 2)) {   // x-compress needs unlzw???
            HTTP_APPEND "Accept-Encoding: deflate,gzip,x-gzip,compress,x-compress\r\n");
        }
    }
    if(b64) {
        HTTP_APPEND "Authorization: Basic %s\r\n", b64);
    }
    if(referer) {
        HTTP_APPEND "Referer: %s\r\n", referer);
    }
    if(useragent) {
        HTTP_APPEND "User-Agent: %s\r\n", useragent);
    }
    if(cookie) {
        HTTP_APPEND "Cookie: %s\r\n", cookie);
    }
    if(more_http) {
        HTTP_APPEND "%s", more_http);
        if(query[len - 1] == '\r') {
            HTTP_APPEND "\n");
        } else if(query[len - 1] != '\n') {
            HTTP_APPEND "\r\n");
        }
    }
    if(content) {
        HTTP_APPEND "Content-length: %d\r\n", contentsize);
    }
    HTTP_APPEND "\r\n");

#undef HTTP_APPEND

    if(verbose >= 3) fputs(query, stdout);
    if(mydown_send(ssl_sd, sd, query, len) != len) GOTOQUIT;
    mydown_free(&query);
    if(content) {
        if(mydown_send(ssl_sd, sd, content, contentsize) != contentsize) GOTOQUIT;
    }

    buff = calloc(BUFFSZ + 1, 1);
    if(!buff) GOTOQUIT;
    buff[0] = 0;
    data = buff;
    len  = BUFFSZ;
    while((t = mydown_block_recv(ssl_sd, sd, data, len, timeout)) > 0) {
        data += t;
        len  -= t;
        *data = 0;

        header_limit = strstr(buff, "\r\n\r\n");
        if(header_limit) {
            *header_limit = 0;
            header_limit += 4;
            break;
        }
        header_limit = strstr(buff, "\n\n");
        if(header_limit) {
            *header_limit = 0;
            header_limit += 2;
            break;
        }
    }
    if(t < 0) GOTOQUIT;

    if(!header_limit) {    // no header received
        header_limit = buff;
    } else {
        if(showhead == 1) {
            if(filedata) *filedata = strdup(buff);
            VERPRINTF"\n%s", buff);
            goto quit;
        }
        if(verbose == 2) fprintf(stderr, "\n%s", buff);

        mydown_scanhead(buff, header_limit - buff,
            "content-length",       &contlen,
            "content-type",         &conttype,
            "content-disposition",  &contdisp,
            "icy-name",             &icyname,
            "transfer-encoding",    &transenc,
            "content-encoding",     &contenc,
            "Set-Cookie",           &cookie,
            "Connection",           &connection,
            "location",             &location,
            NULL,                   NULL);

#ifdef MYDOWN_GLOBAL_COOKIE
        if(cookie) global_cookie = cookie;
#endif

        s = strchr(buff, ' ');
        if(s) {
            code = atoi(s + 1);

            if((code / 100) == 3) {
                if(!location) {
                    fprintf(stderr, "\nError: remote file is temporary unavailable (%d)\n", code);
                    GOTOQUIT;
                }
                //s = mydown_hex2uri(location); // not needed
                //if(s) {
                    //free(location);
                    //location = s;
                //}
                // the problem is that the not all the webservers follow a standard
                // so happens that you find something Location: example.com/path
                // or Location: /path or Location: path and so on...
                // that's why I need to guess it
                VERPRINTF2"\n- redirect: %s\n", location);
                t = 0;
                if(!location[0]) goto quit;
                else if(location[0] == '/') t = 0;
                else if(strstr(location, "://")) t = 1;
                else if(strstr(location, ":\\\\")) t = 1;
                if(!t) {
                    if(!location[1]) goto quit; // "/" returns to the index
                    getstr = location;
                    getstr = mydown_uri2hex(getstr);
                    if(!getstr) getstr = strdup("");
                    fprintf(stderr, "\n- redirect to URI on the same host: %s\n", location); // better to show it
                } else {
                    dossl = mydown_get_host(location, &host, &port, &getstr, &user, &pass, verbose);
                }
                if(!host || !host[0] || !port) {
                    fprintf(stderr, "\nError: the Location field is invalid (error code %d)\n", code);
                    GOTOQUIT;
                }
                if(sd && !keep_alive) mydown_free_sock(ssl_sd, ctx_sd, &sd);
                mydown_free_sock(ssl_sd, ctx_sd, keep_alive);
                ret = mydown_http2file(keep_alive, timeout, host, port, user, pass, referer, useragent, cookie, more_http, verbose, getstr, fd, filename, showhead, (dossl << 1) | onlyifdiff, resume, from, tot, filesize, filedata, ret_code, onflyunzip, content, contentsize, get, proxy, proxy_port, opt);
                goto quit;
            }

                // if((code != 200) && (code != 206)) {
            if((code / 100) != 2) {
                fprintf(stderr, "\nError: remote file is temporary unavailable (%d)\n", code);
                GOTOQUIT;
            }
        }

        if(connection) {
            if(!stricmp(connection, "close") && keep_alive) *keep_alive = 0;
        }

        if(contlen) {
            s = strchr(contlen, '/');
            if(s) contlen = s + 1;
            //if(_atoi64(contlen) > (u64)0x7fffffff) {
            //    fprintf(stderr, "\nError: large files are not yet supported by mydownlib\n");
            //    GOTOQUIT;
            //}
            if(sscanf(contlen, "%u", &fsize) != 1) fsize = 0;   //break;
        }

        if(conttype && onflyunzip) {    // needed or better
            if(mydown_stristr(conttype, "compress"))   httpcompress  = 1;
            if(mydown_stristr(conttype, "gzip"))       httpgzip      = 1;
            if(!onflyunzip) if(mydown_stristr(conttype, "x-gzip")) httpgzip = 0;   // work-around
            if(mydown_stristr(conttype, "deflate"))    httpdeflate   = 1;
        }
        if(contenc) {
            if(mydown_stristr(contenc,  "compress"))   httpcompress  = 1;
            if(mydown_stristr(contenc,  "gzip"))       httpgzip      = 1;
            if(!onflyunzip) if(mydown_stristr(contenc,  "x-gzip")) httpgzip = 0;   // work-around
            if(mydown_stristr(contenc,  "deflate"))    httpdeflate   = 1;
        }

        if(!contdisp && icyname) contdisp = icyname;

        if(transenc && mydown_stristr(transenc, "chunked")) chunked = 1;

        if(filesize) *filesize = fsize;
    }

        if(filename && ((resume == 2) || (resume == 3))) {
            fd = (void *)filename;
            if(!fd) {
                fprintf(stderr, "\nError: no FILE used with resume %d\n", resume);
                GOTOQUIT;
            }
            filename = "."; // this instruction is useless, I have added it only to skip the checks below
        }

            if(contdisp) {
                s = (u8 *)mydown_stristr(contdisp, "filename=");
                if(!s) s = (u8 *)mydown_stristr(contdisp, "filename*=");
                if(!s) s = (u8 *)mydown_stristr(contdisp, "file=");
                if(!s) s = (u8 *)mydown_stristr(contdisp, "file*=");
                if(s) {
                    s = strchr(s, '=') + 1;
                } else {
                    s = contdisp;
                }
                while(*s && ((*s == '\"') || (*s == ' ') || (*s == '\t'))) s++;
                remote_filename = mydown_hex2uri(s);
                if(remote_filename) filenamemalloc = remote_filename; // needed for freeing it later!
                if(remote_filename && remote_filename[0]) {
                    for(s = remote_filename; *s; s++) {    // \r \n are don't exist, it's a line
                        if((*s == '\\') || (*s == '/') || (*s == ';') || (*s == ':') || (*s == '\"') || (*s == '&') || (*s == '?')) break;
                    }
                    for(s--; (s >= remote_filename) && *s && ((*s == ' ') || (*s == '\t')); s--);
                    *(s + 1) = 0;
                }
            } else {
                remote_filename = mydown_hex2uri(getstr);
                if(remote_filename) filenamemalloc = remote_filename; // needed for freeing it later!
                if(remote_filename && remote_filename[0]) {
                    for(
                      s = remote_filename + strlen(remote_filename) - 1;
                      (s >= remote_filename) && (*s != '/') && (*s != '\\') && (*s != ':') && (*s != '&') && (*s != '?') && (*s != '=');
                      s--);
                    remote_filename = s + 1;
                }
            }

            if(remote_filename) {                          // last useless check to avoid directory traversal
                s = strrchr(remote_filename, ':');     if(s) remote_filename = s + 1;
                s = strrchr(remote_filename, '\\');    if(s) remote_filename = s + 1;
                s = strrchr(remote_filename, '/');     if(s) remote_filename = s + 1;
            }

        if(!filename || !filename[0]) filename = remote_filename;

        if(!filename || !filename[0]) {
            filename = mydown_tmpnam();
            if(filename) filenamemalloc = filename; // needed for freeing it later!
            //VERPRINTF"\nError: no filename retrieved, you must specify an output filename\n\n");
            //GOTOQUIT;
        }

        // automatic gzip decompression on the fly
        if(onflyunzip) {
            s = remote_filename ? strrchr(remote_filename, '.') : NULL;
            if(!(s && !stricmp(s, ".gz"))) s = filename ? strrchr(filename, '.') : NULL;
            if(s && !stricmp(s, ".gz")) {
                httpgzip = 1;
                *s = 0;
            }
        }

        if(showhead == 2) {
            if(filedata) *filedata = strdup(filename);
            ret = fsize;
            goto quit;
        }

        if(!filedata && !fd) {
            if(!strcmp(filename, "-")) {
                fd = stdout;
                VERPRINTF"  file   %s\n", "stdout");
            } else {
                // lame but needed to avoid problems with mydown_filename_clean
                filename_tmp1 = strdup(filename);
                filename_tmp2 = filename;
                filename = filename_tmp1;

                mydown_filename_clean(filename);
                VERPRINTF"  file   %s\n", filename);
                err = stat(filename, &xstat);
                if(onlyifdiff && !err && (xstat.st_size == fsize)) {
                    VERPRINTF"  the remote file has the same size of the local one, skip\n");
                    GOTOQUIT;
                }
                /* can't be implemented or will make the library useless (think to updates)
                fd = fopen(filename, "rb");
                if(fd) {
                    fclose(fd);
                    fprintf(stderr, 
                        "- the output file (%s) already exists\n"
                        "  do you want to overwrite it (y/N)? ", filename);
                    fgets(tmp, sizeof(tmp), stdin);
                    if(tolower(tmp[0]) != 'y') GOTOQUIT;
                }
                */
                if((err < 0) || !resume) {      // file doesn't exist or must not resume
                    fd = fopen(filename, "wb");
                } else {
                    fd = fopen(filename, "ab");
                    from = xstat.st_size;
                    VERPRINTF2"  resume %u\n", from);
                    if(sd && !keep_alive) mydown_free_sock(ssl_sd, ctx_sd, &sd);
                    mydown_free_sock(ssl_sd, ctx_sd, keep_alive);
                    ret = mydown_http2file(keep_alive, timeout, host, port, user, pass, referer, useragent, cookie, more_http, verbose, getstr, fd, filename, showhead, (dossl << 1) | onlyifdiff, resume, from, tot, filesize, filedata, ret_code, onflyunzip, content, contentsize, get, proxy, proxy_port, opt);
                    mydown_free(&filename_tmp1);
                    filename = filename_tmp2;
                    goto quit;
                }
                mydown_free(&filename_tmp1);
                filename = filename_tmp2;
                if(!fd) GOTOQUIT;
            }
        }

    len = data - header_limit;
    memmove(buff, header_limit, len);

    httpz = 1;
    if(onflyunzip < 0) httpz = 0;   // disabled forcely!
    if(httpcompress) {
        VERPRINTF2"  compression: compress\n");
        wbits =  15;
    } else if(httpgzip) {
        VERPRINTF2"  compression: gzip\n");
        wbits = -15;
    } else if(httpdeflate) {
        VERPRINTF2"  compression: deflate\n");
        wbits = -15;
    } else {
        httpz = 0;
    }
    if(httpz) {
        z.zalloc = (alloc_func)0;
        z.zfree  = (free_func)0;
        z.opaque = (voidpf)0;
        if(inflateInit2(&z, wbits)) GOTOQUIT;

        zbufflen = BUFFSZ * 4;
        zbuff    = calloc(zbufflen, 1);
        if(!zbuff) GOTOQUIT;
    }

    if(verbose > 0) {
        fprintf(stderr, "\n");
        if(fsize) fprintf(stderr, "    ");
        fprintf(stderr, " | downloaded   | kilobytes/second\n");
        if(fsize) fprintf(stderr, "----");
        fprintf(stderr, "-/--------------/-----------------\n");
    }

    if(verbose >= 0) {
        TEMPOZ(oldtime);
        oldtime -= VISDELAY;
    }

    if(filedata) {
        filedatasz  = fsize;
        if(!filedatasz == -1) GOTOQUIT;
        filedatatmp = calloc(filedatasz + 1, 1);
        if(!filedatatmp) GOTOQUIT;
        filedatatmp[filedatasz] = 0;
    }

    if(chunked) chunkedsize = len;

    do {
redo:
        httpret += len;

        if(chunked) {
            for(;;) {
                chunkedsize = mydown_chunked_skip(buff, chunkedsize);

                err = mydown_sscanf_hex(buff, chunkedsize);
                if(err > 0) break;
                if(!err) {
                    chunkedsize = mydown_chunked_skip(buff, chunkedsize);
                    break;
                }

                t = mydown_block_recv(ssl_sd, sd, buff + chunkedsize, BUFFSZ - chunkedsize, timeout);
                if(t <= 0) GOTOQUIT;
                chunkedsize += t;
                if(chunkedsize > BUFFSZ) GOTOQUIT;
            }

            chunkedlen = err;
            if(!chunkedlen) break;

            //if(chunkedbuff) free(chunkedbuff);
            //chunkedbuff = calloc(chunkedlen, 1);
            chunkedbuff = realloc(chunkedbuff, chunkedlen);
            if(!chunkedbuff) GOTOQUIT;
            memset(chunkedbuff, 0, chunkedlen);

            s = (u8 *)strchr(buff, '\n');
            if(!s) GOTOQUIT;
            err = (s + 1) - buff;
            chunkedsize -= err;
            memmove(buff, buff + err, chunkedsize);

            if(chunkedlen < chunkedsize) {      // we have more data than how much we need
                memcpy(chunkedbuff, buff, chunkedlen);
                chunkedsize -= chunkedlen;
                memmove(buff, buff + chunkedlen, chunkedsize);
            } else {                            // we have only part of the needed data
                memcpy(chunkedbuff, buff, chunkedsize);
                for(len = chunkedsize; len < chunkedlen; len += t) {
                    t = mydown_block_recv(ssl_sd, sd, chunkedbuff + len, chunkedlen - len, timeout);
                    if(t <= 0) GOTOQUIT;
                }
                chunkedsize = 0;
            }

            chunkedtmp  = buff;
            buff        = chunkedbuff;
            len         = chunkedlen;
        }

            /* DECOMPRESSION */

        if(httpz) {
            if(httpgzip && !ret) {  // gzip is really bad to handle in my code... blah
                if(len < 2) {
                    t = mydown_block_recv(ssl_sd, sd, buff + len, 2 - len, timeout);
                    if(t <= 0) GOTOQUIT;
                    len += t;
                }
                t = len;
                s = buff;
                if((httpgzip == 1) && !httpskipbytes && (len >= 2)) {
                    if((buff[0] == 0x1f) && (buff[1] == 0x8b)) {
                        httpskipbytes = 3;  // CM is usually 8, no need to check it
                    } else {                // in this case it's a raw deflate stream
                        httpgzip = -1;      // -1 means that it's no longer needed to check the header
                        t = 0;              // handle all the buffer
                    }
                }

                if(httpgzip > 0) {
                    for(;;) {
                        s = mydown_http_skip(s, t, &httpskipbytes, &t);
                        if(!s) {
                            t = len;
                            break;
                        }
                        if(httpgzip == 1) {
                            httpgzip_flags = *s++;      // flags
                            httpskipbytes  = 4 + 1 + 1; // mtime + xfl + os
                            httpgzip++;
                        } else if(httpgzip == 2) {      // xfl flag
                            httpskipbytes = 0;
                            if(httpgzip_flags & 4) {
                                if(t >= 2) {            // uff boring, not 100% correct
                                    httpskipbytes = s[0] | (s[1] << 8);
                                    s += 2;
                                }
                            }
                            httpgzip++;
                        } else if(httpgzip == 3) {
                            httpskipbytes = 0;
                            if(httpgzip_flags & 8) {    // name
                                while((s - buff) < len) {
                                    if(!*s++) {
                                        httpgzip++;
                                        break;
                                    }
                                }
                            } else {
                                httpgzip++;
                            }
                        } else if(httpgzip == 4) {
                            httpskipbytes = 0;
                            if(httpgzip_flags & 16) {   // comment
                                while((s - buff) < len) {
                                    if(!*s++) {
                                        httpgzip++;
                                        break;
                                    }
                                }
                            } else {
                                httpgzip++;
                            }
                        } else if(httpgzip == 5) {
                            httpskipbytes = 0;
                            if(httpgzip_flags & 2) {    // crc
                                httpskipbytes = 2;
                            }
                            httpgzip++;
                        } else {
                            t = s - buff;
                            break;
                        }
                    }
                }
                len = mydown_unzip(z, buff + t, len - t, &zbuff, &zbufflen);
            } else {
                len = mydown_unzip(z, buff, len, &zbuff, &zbufflen);
            }
            if(len < 0) GOTOQUIT;
            ztmp = buff;
            buff = zbuff;
        }

            /* UPDATE THE AMOUNT OF UNCOMPRESSED BYTES DOWNLOADED */
            // ret is the total size of the data we have downloaded (uncompressed)
            // httpret is the total size of the data we have downloaded from the server
            // len is the size of the current block of data we have downloaded (uncompressed)

        ret += len;

            /* WRITE THE DATA INTO FILE OR MEMORY */

        if(tot && (ret > tot)) {
            len = tot - (ret - len);
            ret = tot;
        }

        if(filedata) {
            if(filedatasz < ret) {
                filedatasz  = ret;
                if(filedatasz == -1) GOTOQUIT;
                filedatatmp = realloc(filedatatmp, filedatasz);
                if(!filedatatmp) GOTOQUIT;
                filedatatmp[filedatasz] = 0;
            }
            memcpy(filedatatmp + ret - len, buff, len);
        } else if(fd) {
            if(fwrite(buff, 1, len, fd) != len) {
                fprintf(stderr, "\nError: I/O error. Probably your disk is full or the file is write protected\n");
                GOTOQUIT;
            }
            //fflush(fd);
        }
        if(ret_code && (resume == 3)) *ret_code = ret;

            /* VISUALIZATION */

        if(verbose >= 0) {
            TEMPOZ(newtime);
            if((newtime - oldtime) >= VISDELAY) {
                mydown_showstatus(fsize, httpret, vishttpret, (int)(newtime - oldtime));
                oldtime = newtime;
                vishttpret = httpret;
            }
        }

            /* FREE, EXCHANGE OR OTHER STUFF */

        if(httpz) {
            zbuff = buff;
            buff  = ztmp;
        }
        if(chunked) {
            chunkedbuff = buff;
            buff        = chunkedtmp;
            httpret += len; // lame work-around, sorry
            len         = 0;
            goto redo;
        }

            /* FSIZE CHECK */

        if(tot && (ret == tot)) break;
        if(fsize) {
            if(httpret >= fsize) break;
        }

            /* READ NEW DATA FROM THE STREAM */

    } while((len = mydown_block_recv(ssl_sd, sd, buff, BUFFSZ, timeout)) > 0);

    if(verbose >= 0) {
        TEMPOZ(newtime);
        mydown_showstatus(fsize, httpret, vishttpret, (int)(newtime - oldtime));
    }

    if(fsize && (len < 0)) GOTOQUIT;

    if(filedata) {
        *filedata = filedatatmp;
    }

quit:
    if(httpz) {
        if(zbuff) inflateEnd(&z);
        if(zbuff != buff) mydown_free(&zbuff);
    }
    if(chunkedbuff != buff) mydown_free(&chunkedbuff);
    mydown_free(&userpass);
    mydown_free(&b64);
    mydown_free(&buff);
    mydown_free(&filenamemalloc);
    mydown_free(&query);
    if(ret_code) {
        *ret_code = code;
        if(resume == 3) *ret_code = ret;
    }
    if(sd && !keep_alive) mydown_free_sock(ssl_sd, ctx_sd, &sd);
    VERPRINTF"\n");
    if(ret == MYDOWN_ERROR) mydown_free_sock(ssl_sd, ctx_sd, keep_alive);
    if(filename && ((resume == 2) || (resume == 3))) {
        // do nothing
    } else {
        if(!oldfd && fd && (fd != stdout)) fclose(fd);
    }
    mydown_opt_push
    return(ret);

#undef GOTOQUIT
}



u8 *mydown_tmpnam(void) {
    FILE    *fd;
    int     i;
    u8      *ret;

    ret = malloc(32);
    for(i = 1; ; i++) {
        sprintf(ret, "%u.myd", i);  // enough small for any OS (8.3 too)
        fd = fopen(ret, "rb");      // check real existence of the file
        if(!fd) break;
        fclose(fd);
    }
    return(ret);
}



void mydown_free_sock(SSL *ssl_sd, SSL_CTX *ctx_sd, int *sock) {
    if(sock && *sock) {
        // note that ssl_sd and ctx_sd exist only in http2file so if the socket is saved/reused
        // they get lost... yeah it's lame but it's only a work-around
        if(ssl_sd) {
            SSL_shutdown(ssl_sd);
            SSL_free(ssl_sd);
        }
        if(ctx_sd) SSL_CTX_free(ctx_sd);
        close(*sock);
        *sock = 0;
    }
}



u8 *mydown_http_skip(u8 *buff, int len, int *needed, int *remain) {
    int     rest;

    if(!buff) return(NULL);
    rest = *needed;
    if(len < rest) {
        *needed = rest - len;
        *remain = 0;
        return(NULL);
    }
    *needed = 0;
    *remain = len - rest;
    return(buff + rest);
}



void mydown_free(u8 **buff) {
    if(!buff || !*buff) return;
    free(*buff);
    *buff = NULL;
}



int mydown_chunked_skip(u8 *buff, int chunkedsize) {
    int     t;

    if(!buff) return(0);
    for(t = 0; t < chunkedsize; t++) {
        if((buff[t] != '\r') && (buff[t] != '\n')) break;
    }
    if(t) {
        chunkedsize -= t;
        memmove(buff, buff + t, chunkedsize);
    }
    return(chunkedsize);
}



int mydown_unzip(z_stream z, u8 *in, int inlen, u8 **outx, int *outxlen) {
    int     zerr,
            outsz;
    u8      *out;

    if(!in) return(0);
    if(inlen <= 0) return(0);

    out     = *outx;
    outsz   = *outxlen;

    z.next_in   = in;
    z.avail_in  = inlen;

    for(;;) {
        z.next_out  = out   + z.total_out;
        z.avail_out = outsz - z.total_out;

        zerr = inflate(&z, Z_NO_FLUSH);

        if(zerr == Z_STREAM_END) break;
        if((zerr != Z_OK) && (zerr != Z_BUF_ERROR)) {
            fprintf(stderr, "\nError: zlib error %d\n", zerr);
            z.total_out = MYDOWN_ERROR;
            break;
        }

        if(!z.avail_in) break;

        outsz += (inlen << 1);      // inlen * 2 should be enough each time
        out = realloc(out, outsz);
        if(!out) {
            outsz       = 0;
            z.total_out = MYDOWN_ERROR;
            break;
        }
    }

    *outx    = out;
    *outxlen = outsz;
    return(z.total_out);
}



int mydown_sscanf_hex(u8 *data, int datalen) {
    int     i,
            ret;

    if(!data) return(MYDOWN_ERROR);
    for(i = 0; i < datalen; i++) {
        if((data[i] == '\r') || (data[i] == '\n')) break;
    }
    if(i == datalen) return(MYDOWN_ERROR);

    if(sscanf(data, "%x", &ret) != 1) return(MYDOWN_ERROR);
    return(ret);
}



int mydown_timeout(int sock, int secs) {
    struct  timeval tout;
    fd_set  fdr;
    int     err;

    tout.tv_sec  = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fdr);
    FD_SET(sock, &fdr);
    err = select(sock + 1, &fdr, NULL, NULL, &tout);
    if(err < 0) return(MYDOWN_ERROR); //std_err();
    if(!err) return(MYDOWN_ERROR);
    return(0);
}



int mydown_block_recv(SSL *ssl_sd, int sd, u8 *data, int len, int timeout) {
    if(!timeout) timeout = MAXTIMEOUT;
    if(mydown_timeout(sd, timeout) < 0) return(MYDOWN_ERROR);
    return(mydown_recv(ssl_sd, sd, data, len));
}



u8 *mydown_showhttp80(u16 port) {
    static u8  mini[16];    // static but used only for some milliseconds

    if(port == 80) return("");
    sprintf(mini, ":%u", port);
    return(mini);
}



void mydown_showstatus(u32 fsize, u32 ret, u32 oldret, int timediff) {
    u32     vis;

    if(fsize) {
        vis = ((u64)ret * (u64)100) / (u64)fsize;
        fprintf(stderr, "%3u%%", (vis < 100) ? vis : 100);
    }
    fprintf(stderr, "   %12u", ret);
    if(ret > 0) {
        if(timediff) fprintf(stderr, "   %-10u", (u32)(((((u64)ret - (u64)oldret) * (u64)1000) / (u64)timediff) / 1024));
    }
    fprintf(stderr, "\r");
}



u8 *mydown_base64_encode(u8 *data, int *size) {
    int     len;
    u8      *buff,
            *p,
            a,
            b,
            c;
    static const u8 base[64] = {
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
        'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
        'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
        'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
    };

    if(!data) return(NULL);
    if(!size || (*size < 0)) {      // use size -1 for auto text size!
        len = strlen(data);
    } else {
        len = *size;
    }
    buff = calloc(((len / 3) << 2) + 6, 1);
    if(!buff) return(NULL);

    p = buff;
    do {
        a     = data[0];
        b     = data[1];
        c     = data[2];
        *p++  = base[(a >> 2) & 63];
        *p++  = base[(((a &  3) << 4) | ((b >> 4) & 15)) & 63];
        *p++  = base[(((b & 15) << 2) | ((c >> 6) &  3)) & 63];
        *p++  = base[c & 63];
        data += 3;
        len  -= 3;
    } while(len > 0);
    *p = 0;

    for(; len < 0; len++) *(p + len) = '=';

    if(size) *size = p - buff;
    return(buff);
}



in_addr_t mydown_resolv(char *host) {
    struct      hostent *hp;
    in_addr_t   host_ip;
    int         i;
    dns_db_t    *dns;

    host_ip = inet_addr(host);
    if(host_ip == htonl(INADDR_NONE)) {

        for(i = 0; i < dns_db_max; i++) {           // search
            if(!stricmp(host, dns_db[i].host)) return(dns_db[i].ip);
        }

        hp = gethostbyname(host);
        if(!hp) {
            fprintf(stderr, "\nError: Unable to resolve hostname (%s)\n\n", host);
            return(INADDR_NONE);
        }
        host_ip = *(in_addr_t *)(hp->h_addr);

        if(!dns_db_max) memset(&dns_db, 0, sizeof(dns_db));
        if(dns_db_add == MAXDNS) dns_db_add = 0;    // add
        dns = &dns_db[dns_db_add];
        mydown_free(&dns->host);
        dns->host = strdup(host);
        dns->ip   = host_ip;
        dns_db_add++;
        if(dns_db_max < MAXDNS) dns_db_max++;
    }
    return(host_ip);
}



char *mydown_stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;

      for (start = (char *)String; *start; start++)
      {
            /* find start of pattern in string */
            for ( ; (*start && (toupper(*start) != toupper(*Pattern))); start++)
                  ;
            if (!*start)
                  return 0;

            pptr = (char *)Pattern;
            sptr = (char *)start;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if (!*pptr)
                        return (start);
            }
      }
      return 0;
}

