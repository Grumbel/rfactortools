/*
by Luigi Auriemma
*/



#define LC_LZ1 0
#define LC_LZ2 1
#define LC_LZ3 2
#define LC_LZ4 3
#define LC_LZ5 4
#define LC_LZ6 5
#define LC_LZ7 6
#define LC_LZ8 7
#define LC_LZ9 8
#define LC_LZ10 9
#define LC_LZ11 10
#define LC_LZ12 11
#define LC_LZ13 12
#define LC_LZ14 13
#define LC_LZ15 14
#define LC_LZ16 15
#define LC_LZ17 16
#define LC_LZ18 17

#define LC_RLE1 100
#define LC_RLE2 101
#define LC_RLE3 102
#define LC_RLE4 103



#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include "lunar_dll.h"



void *mymemmem(const void *b1, const void *b2, size_t len1, size_t len2);



static unsigned char* (__stdcall *LunarOpenRAMFile)(void* data, u32 FileMode, u32 size) = NULL;
static u32 (__stdcall *LunarDecompress)(void* Destination,u32 AddressToStart,u32 MaxDataSize, u32 Format, u32 Format2, u32* LastROMPosition) = NULL;
static u32 (__stdcall *LunarCloseFile)() = NULL;
#endif



int lunar_uncompress(unsigned char *in, int insz, unsigned char *out, int outsz, int format, int format2) {
#ifdef WIN32
    static HMODULE hlib = NULL;
    int     ret;
    unsigned char   *p;

    if(!hlib) {

        // "The DLL file is corrupt..."
        p = mymemmem(lunar_dll, "\x8B\x45\x0C\x83\xE8\x01\x72", sizeof(lunar_dll), 7);
        if(p) memcpy(p, "\x33\xc0\x90", 3);

        hlib = (void *)MemoryLoadLibrary((void *)lunar_dll);
        if(!hlib) return(-1);
        LunarOpenRAMFile = (void *)MemoryGetProcAddress(hlib, "LunarOpenRAMFile");
        if(!LunarOpenRAMFile) return(-2);
        LunarDecompress  = (void *)MemoryGetProcAddress(hlib, "LunarDecompress");
        if(!LunarDecompress) return(-3);
        LunarCloseFile   = (void *)MemoryGetProcAddress(hlib, "LunarCloseFile");
        if(!LunarCloseFile) return(-4);
    }

    if(!LunarOpenRAMFile(in, 0 /*LC_READONLY*/, insz)) return(-5);
    ret = LunarDecompress(out, 0, outsz, format, format2, NULL);
    LunarCloseFile();
    return(ret);
#else
    return -1;
#endif
}

