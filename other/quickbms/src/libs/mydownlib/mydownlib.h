/*
mydownlib
by Luigi Auriemma
e-mail: aluigi@autistici.org
web:    aluigi.org

Note that this library has been written for being used in my tools, so there is no manual except some comments in the code

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



#define MYDOWN_VER      "0.2.7b"
#define MYDOWN_ERROR    (-1)



typedef struct {
    uint32_t    from;           // download from byte, use -1 for the latest tot bytes
    uint32_t    tot;            // download tot bytes
    int         showhead;       // show the http header and stop    (1 = yes, 0 = no, 2 get all the remote file info (solution for backward compatibility!))
    int         resume;         // resume a download                (1 = yes, 0 = no, 2 or 3 considers the filename as FILE * (solution for backward compatibility!))
    int         onlyifdiff;     // download only if differs from local file (1 = yes, 0 = no)
    uint8_t     *user;          // username for authentication
    uint8_t     *pass;          // password for authentication
    uint8_t     *referer;       // referer string
    uint8_t     *useragent;     // user-agent string
    uint8_t     *cookie;        // cookie string
    uint8_t     *more_http;     // additional http parameters
    int         verbose;        // verbosity (-1 = quiet, 0 = normal, 1 = verbose)
    uint8_t     **filedata;     // use it if you want to store the downloaded file in memory (if showhead is 2 then filedata will contain the name ofthe remote file)
    int         *keep_alive;    // keep-alive socket
    int         timeout;        // seconds of timeout
    int         *ret_code;      // HTTP code from the server (if resume is equal to 3 ret_code returns the number of bytes received (solution for backward compatibility!))
    int         onflyunzip;     // unpack the file on the fly if possible (usually gzipped)
    uint8_t     *content;       // data to post
    int         contentsize;    // optional size of content (default is auto)
    uint8_t     *get;           // the type of request, like GET or POST or HEAD or anything else
    uint8_t     *proxy;         // proxy (HTTP and CONNECT method)
    uint16_t    proxy_port;     // proxy port
} mydown_options;



uint32_t mydown(                // ret: file size
    uint8_t     *myurl,         // the URL
                                // can be like http://aluigi.org/mytoolz/mydown.zip
                                // or http://user:pass@host:port/blabla/blabla.php?file=1
    uint8_t     *filename,      // NULL for automatic filename or forced like "test.txt" (if showhead is 2 filename will be considered a uint8_t ** containing the 
    mydown_options *opt         // the above structure for your options
);



uint32_t mydown_http2file(      // ret: file size
    int         *sock,          // socket for keep-alive
    int         timeout,        // seconds of timeout
    uint8_t     *host,          // hostname or IP
    uint16_t    port,           // port
    uint8_t     *user,          // username
    uint8_t     *pass,          // password
    uint8_t     *referer,       // Referer
    uint8_t     *useragent,     // User-Agent
    uint8_t     *cookie,        // Cookie
    uint8_t     *more_http,     // additional http parameters (ex: mycookie: blabla\r\npar: val\r\n)
    int         verbose,        // verbose
    uint8_t     *getstr,        // URI
    FILE        *fd,            // file descriptor
    uint8_t     *filename,      // force filename
    int         showhead,       // show headers
    int         onlyifdiff,     // download only if differs from local file
    int         resume,         // resume
    uint32_t    from,           // download from byte
    uint32_t    tot,            // download tot bytes
    uint32_t    *filesize,      // for storing file size
    uint8_t     **filedata,     // use it if you want to store the downloaded file in memory
    int         *ret_code,      // HTTP code from the server
    int         onflyunzip,     // on fly decompression
    uint8_t     *content,       // data to post
    int         contentsize,    // optional size of content (default is auto)
    uint8_t     *get,           // request
    uint8_t     *proxy,
    uint16_t    proxy_port,
    mydown_options *opt         // set it to NULL, it's an experimental idea
);


