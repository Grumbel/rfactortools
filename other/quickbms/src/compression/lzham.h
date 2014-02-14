#ifdef WIN32

#include "lzham_dll.h"

static void *(__cdecl *lzham_decompress_init)(void *pParams) = NULL;
static int (__cdecl *lzham_decompress)(void *p, unsigned char *pIn_buf, u32 *pIn_buf_size, unsigned char *pOut_buf, u32 *pOut_buf_size, u32 no_more_input_bytes_flag) = NULL;
static int (__cdecl *lzham_decompress_deinit)(void *p) = NULL;

int unlzham(unsigned char *in, int insz, unsigned char *out, int outsz) {
   struct lzham_decompress_params
   {
      u32 m_struct_size;
      u32 m_dict_size_log2;
      u32 m_output_unbuffered;
      u32 m_compute_adler32;
      u32 m_num_seed_bytes;
      void *m_pSeed_bytes;
   };
    static HMODULE hlib = NULL;
    int     ret;

    if(!hlib) {
        hlib = MemoryLoadLibrary((void *)lzham_dll);
        if(!hlib) return(-1);
        lzham_decompress_init   = (void *)MemoryGetProcAddress(hlib, "lzham_decompress_init");
        lzham_decompress        = (void *)MemoryGetProcAddress(hlib, "lzham_decompress");
        lzham_decompress_deinit = (void *)MemoryGetProcAddress(hlib, "lzham_decompress_deinit");
    }

    void    *p;
    struct lzham_decompress_params par;
    memset(&par, 0, sizeof(par));
    par.m_struct_size = sizeof(par);
    par.m_dict_size_log2 = 26;
    par.m_output_unbuffered = 1;
    p = lzham_decompress_init(&par);
    if(!p) return(-1);
    u32     itmp = insz, otmp = outsz;
    ret = lzham_decompress(p, in, &itmp, out, &otmp, 1);
    lzham_decompress_deinit(p);
    if(ret < 0) return(-1);
    return(otmp);
}

#else

int unlzham(unsigned char *in, int insz, unsigned char *out, int outsz) {
    return(-1);
}

#endif
