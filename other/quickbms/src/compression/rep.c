// modified by Luigi Auriemma

/*
    REP is an LZ77-family algorithm, i.e. it founds matches and outputs them as
    (len,offset) pairs. It is oriented toward very fast compression and small
    memory overhead (1/4 of buffer size), but limited to rather large values of
    mimimal match length (say, 32), and don't search for optimum match. It's
    intended to preprocess data before using full-fledged compressors. and in
    this area it beats RZIP and, to some degree, LZP preprocessors. Small
    memory overhead means that RZIP/LZP/REP are capable to find matches at very
    long distances and this algorithm does it much better than RZIP and LZP.
    The algorithm implemented in functions REPEncode() and REPDecode().

    Main differences comparing to RZIP:
    1) Sliding window which slides at 1/16 of buffer size each time
    2) Almost ideal hash function (see update_hash)
    3) Direct hashing without hash chains which 1.5x cuts memory requirements
    4) Tags are not saved in hashtable, which again halves memory requirements.
         Instead, a few lower bits of hash table entry are used to save a few
         bits of tag (see chksum)
    5) Hash size is proportional to buffer size (which is equal to the maximum
         search distance) and by default limited to 1/4 of buffer size
    6) In order to find strings of length >=MinLen, blocks of length L=MinLen/2
         are indexed via hash. Of all those possible blocks, only 1/sqrt(L) are
         indexed and only 1/sqrt(L) are searched. It is alternative to solution
         described in RZIP paper where 1/L of blocks are indexed and each block
         searched. This means that logb(sqrt(L)) lower bits of hash entry are
         zeroes which allows to use trick 4.


References for RZIP algorithm explanation and implementations:
http://samba.org/~tridge/phd_thesis.pdf
http://rzip.samba.org/ftp/rzip/rzip-2.1.tar.gz
http://ck.kolivas.org/apps/lrzip/lrzip-0.18.tar.bz2
http://www.edcassa-ict.nl/lrzip.zip
http://www.edcassa-ict.nl/rzip21.zip

TAYLOR, R., JANA, R., AND GRIGG, M. 1997. Checksum testing of remote
synchronisation tool. Technical Report 0627 (November), Defence Science and
Technology Organisation, Canberra, Australia. (p.72)


References for LZP algorithm implementations:
http://magicssoft.ru/content/download/GRZipII/GRZipIISRC.zip
http://www.compression.ru/ds/lzp.rar


** Detailed algorithm description in Russian **********************************************

    ���� �������� �������� �������������� LZ77, �.�. �� ������� �������������
    ������ �� ������� ������, � �������� �� ��� (len,offset). ��� ������������
    �������� ���������� �� ����� ���������� ���������� ������� ����� �� �������
    ����������. ������� �� ������ ���������� ���������� ������ - ��� �������,
    ��� �������� ������ ��������� �� ����� 25% �� ������� ���� ������. ��� ����
    �� ������� ����������� ��� ���������� ���� ����������� ����� (MinLen)
    ������� ����� - 512 ����, � ������� 98% - � ����� ��� ������������ ��
    ����� ���������� � ������ �� 32 ����. �� �������� ���� �������� ������� ��
    ������������� � �������� �������������, ������������ ������������ �����
    �/��� ���������� ��������� �� ����� ����������, ������� ����������
    ��������� ��������� ��������, � � ���� �������� �� ����������� � ������
    �����������, ��� LZP by Ilya Grebnev � RZIP. ��� ����, ��� ����������
    ������������, ��� ������������� ����������� �������� ����������� �������
    ������ ��������� ������ � ���� �������� - 32-512 ����. ���� ��������
    ������� ���� ������ ����������, ��� LZP/RZIP, � ����� ����, ���
    �������� ������ ������������� ��� ���������� MinLen.

    �������� ����������� ��������� REPEncode() � REPDecode(), � ����������
    ��������� ���� �� LZP, RZIP � ���� �����������. ����� ���������� ������ �
    ���������� ���� - ������� ������ ����������� ������� �� 1/16 �� �������
    ������, � ��� �������� ��� � ����� ������ ������� ��� ������� 15/16 ������
    �������� ���������� ������, ������� ����������� � ������� ����������. ���
    ��������� ��������� �� ������� �����, �� ���������� �� ����� ����������
    ������� ������.

    ��� ������, ��� ������ ����� � ������ �� MinLen � ������� ����� �����
    ������ MinLen ����������� ����� ����������� ����� (��), ������� ��������� �
    ���-�������. ��������� �������� ������������ �� ������� �������� MinLen,
    ������� ���������� �� �� ������ ����� ����� �������� ���������. ���
    �������� �������� �������������� "���������� ��", �� ���� �����, �������
    ����� ������ ����������� ��� ���������� ������ ����� � ����� ����� �
    �������� ������ ����� � ������ (��. update_hash).

    ������ ��������� ������� ��� ����������� ����������� ��� ���������
    ������������. � ����� ������ ������� ������� hash = p[-1] + PRIME*p[-2] +
    PRIME*PRIME*p[-3] + ..., ��� PRIME - ������� �����, ��������� ����� �������
    � ������ ������ ����������� �������������. ����������, ��� ���������� ����
    �� ������ 1<<32, ������� ���������������� ��� ����������� :)

    �����, ���� ������������ �������������� ���� ��� ���������� ���������� �
    ������ � ���������� ��������. ���������� � ������� ������ ��������� ���
    MinLen=512. ��������� ����� 512-������� ���� �������� � ���� 256-�������
    ����, ������������ � �������, ������� 256, �� ��� ���������� ��������� �
    ���-������� ������ ������ �� ��� ����� � ������ ���������� ������ � ����.
    ����������, ��� �������� ���������� �� �� �������������� � �������� 256
    �������, � �������� ���������� ��� ��� ����� ������ � ��� �������. ������
    ��� � ��������� ����������� ��������� ������ ������ ��� ���������������
    ���������� ����� ���� ���������� - �� ������ ������, ����� MinLen
    ���������� ������.

    ������ ����� ����� ��� ������ - ������ ����, ����� ��������� � ���-�������
    ������ 256-� ����, �� ������ ������-������, �� ����� �������� ���������
    ������ 32-�, � ������ ������ 8-�, ��� ��������� ������ 2-�, � ������ ������
    128-�. ����������, ��������� ����� ��������� � ������ ������ 16-� ����.
    ������ ������, ����� ��������� ���� ���� ����� ������ 16 ����, � ������
    ������ 16 ������ �� ������ 256, �� ���� ��������� �����, ������������ �
    ������� 0, 16, 32..., � ���� �����, ������������ � ������� 0, 1, 2..., 15,
    256. 257... ����� �������, ��� MinLen=512 ����������� 8-������� ���������
    ������ (�� ���� 8-�������� ���������� ���������� ��������� � ������) ��
    ��������� � ������������� ����������� - ������, �� ���� ����������
    ���������� � ������ (� 1/64 ������� ������ �� 1/4, ��� �� ��� ������ ������
    ���������).

    �������, ��������� ������ �������� ������������� ������� ����� ������ �
    ���-������� ��� �������� ���������� ��� �� �������� ���-������� (chksum) -
    ����������, ���, ������� �� �������� ������ ������� � ���-�������. ���
    ��������� ������� ������� ����� ������ ����������, �� ��������� ����������
    ������, � ��� ����� ��������� ���������� ��������� � ������ � ��� ������
    �������� ������ ���������.

    � ��������� ������������ ����������� � ������ ����������, ��� ����������
    �����������, ��� ������ ���������� ����� �������. �������� ���-�������
    �� 256-�������� ����� (� ����� ������ ������ ����� ����� - L=MinLen/2)
    ������������ ��� ������ � ���-������� (hasharr[hash&HashMask]), ���
    ��������� ����� ���� ������ �������� ����� ������. �� �������� ���
    (�����������) �� ���� � ���������� ����������. ��� ��� ���������, ���
    ���� ��������, � ������� �� ����������� LZ77 ����������, ���� ��
    ����������� (����� �������) ����������, � ��������� ���� ���� ������ - ��
    ��������� ����, ������� ����� ���� ���-����, � ��� ��, �������������,
    ���������������� ��������� � �� �������� �����.

    ������ ���� (HashSize): ��� ���������� ��������� � �����������, ��� ��
    ������ ���� � 2-4 ���� ������ ���������� ���������, ������� � ���� �������
    ���������. ������ �� �������� ���������, ��� ������ ���������� ����� �� ��
    ����� ���-�� ������, � ��� MinLen=32 - ���� �������� (!) �������. �� ����,
    ��������, ��� 32 �� ����� ��� MinLen=512 � ��� ����������� ������ 16-�
    256-������� ���� � ����� ���������� ����������� ��������� - 32���/16=2���,
    �.�. 8 ��, � ��� �������� ������ ������ �������. ��� MinLen=32 �����
    ���������� ��������� 32���/4=8���, �� �� ������ ���-������� ��������
    ������, �� ���� ���������� �� �� ����� 8 ��. ����� �������, �����������
    ���������� ������������� ������ ���-������� ������� �� ����������� 1/4
    ������� �������� ������. ���� �� ������ ���������� ������ �������� - ��
    ����������� �������� HashBits (����� -h). ���������� HashSize ��� ���������
    MinLen �������� ������� ��������� ������� ������.

    Amplifier: ��� ���� ������� ����, ��� ������ ����������� ������ �����
    ������, ������� �� � ��������� ������� ��� ���������� ���� ����� � ������
    >=MinLen - ���� � ��� ��������� �����������. ������ ���� �����������
    ����������, � ����� ������������� ����� ��-�� ����� ��������. ��������
    Amplifier (����� -a) ��������� ����������� ������������ �������� �����
    ������ (� ��� ����� Amplifier ���). ����� �������, ��� �����������
    ����������� ������ ����� ������ ���������� Amplifier � ���������� �������
    ��������, ������ 99. ����������, ��� ��������� �������� � ���� ���������
    ����������� ������.

    Barrier � SmallestLen: ��������� ���������, � ��������� ppmd, ����������,
    ���� ������������ ���������� ������� �������� MinLen ��� �������
    ���������. ��� ��� ��������� ��������� ���������� ��������������� �������
    ������ ����������, �������� "� ������ ��������� - MinLen=128, �����
    MinLen=32" ������� ����� MinLen=128, Barrier=1<<20, SmallestLen=32
    (����� -l128 -d1048576 -s32). ��� ���� ����� ����� �������������, ��-��,
    �� ���������� ����� � ������ �� SmallestLen ������ MinLen.


** Benchmarks using 1GHz processor ****************************************************************

Test results for 26mb:
        Compression time   Compressed size
-l8192  0.5 seconds
 -l512  1.1
 -l128  1.4
  -l32  2.5                12.7 mb
lrzip   2.6                14.1
lzp:h20 6.5                13.1
lzp:h13 3.0                20.6

Compression speed on uncompressible data:
-l8192  52 mb/sec
 -l512  25 mb/sec
 -l128  17 mb/sec
  -l32   8 mb/sec
lrzip    8 mb/sec

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>



typedef int unrep_io_func (/*void* param,*/ void* buf, int size);



unsigned char   *unrep_in,
                *unrep_inl,
                *unrep_out,
                *unrep_outl;

int unrep_fread(void *buf, int size) {
    if(size > (unrep_inl - unrep_in)) size = unrep_inl - unrep_in;
    memcpy(buf, unrep_in, size);
    unrep_in += size;
    return(size);
}
int unrep_fwrite(void *buf, int size) {
    if(size > (unrep_outl - unrep_out)) size = unrep_outl - unrep_out;
    memcpy(unrep_out, buf, size);
    unrep_out += size;
    return(size);
}



static inline void unrep_copymem (byte* p, byte* q, unsigned len)
{
    if (len)
    do *p++ = *q++;
    while (--len);
}


// Classical LZ77 decoder with sliding window
int rep_decompress (int BlockSize, int MinCompression, int MinMatchLen, int Barrier, int SmallestLen, int HashBits, int Amplifier, unrep_io_func* In, void* InParam, unrep_io_func* Out, void* OutParam)
{
    byte *last_data;
    int i;
    // ����������� ������ ������ ������� �� ������� ������
    if (In (/*InParam,*/ &BlockSize, sizeof(int32)) != sizeof(int32))   return -1;  // Error: can't read input data
    byte *data = (byte*) malloc (BlockSize), *data0=data;
    if (data==NULL)  return -1;                             // Error: not enough memory
    //stat ("Decompression",0);

    // ����, ������ �������� �������� ������������ ���� ���� ������ ������
    for (last_data=data; ; last_data=data) {

        // ��������� ���� ���� ������ ������
        int ComprSize;
        if (In (/*InParam,*/ &ComprSize, sizeof(int32)) != sizeof(int32))   return -1;  // Error: can't read input data
        if (ComprSize == 0)  break;    // EOF flag (see above)

        byte *buf = (byte*) malloc(ComprSize), *buf0=buf;   // �����, ���� ����� �������� ������� ������
        if (buf==NULL)           return(-1);       // Error: not enough memory
        int Size = In (/*InParam,*/ buf, ComprSize);
        if (Size != ComprSize)   return(-1);       // Error: can't read input data

        // ��������� ����� �������� ������ ������ lens/offsets/datalens; ����� ���� ���� ��� ������� � ������� ����������� ������
        int         num = *(int32*)buf;  buf += sizeof(int32);           // ���������� ���������� (= ���������� ������� � �������� lens/offsets/datalens)
        int32*     lens =  (int32*)buf;  buf += num*sizeof(int32);
        int32*  offsets =  (int32*)buf;  buf += num*sizeof(int32);
        int32* datalens =  (int32*)buf;  buf += (num+1)*sizeof(int32);   // ������, datalens �������� num+1 �������

        // ������ �������� ����� ����� �������� ���� ���� �������� ������ � ���� match, ������� interleaved � ����� ���������� ������� ��������
        for (i=0; i<num; i++) {
            memcpy (data, buf, datalens[i]);  buf += datalens[i];  data += datalens[i];
            //debug (verbose>1 && printf ("Match %d %d %d\n", -offsets[i], data-data0, lens[i]));
            // ���� �������� �������� �� ������ ������, �� ������� �� ���� BlockSize, ����� "����������" ������ ������� ������
            int offset = offsets[i] <= data-data0 ?  offsets[i] : offsets[i]-BlockSize;
            unrep_copymem (data, data-offset, lens[i]);  data += lens[i];
        }
        // ���� ��� ���� ���� ����������� ������ � ����� ����� (��������, ������� �����)
        memcpy (data, buf, datalens[num]);  buf += datalens[num];  data += datalens[num];

        // ����� ������������� ������, ������ ���������� ���������� � ���������� � ��������� �������� �����
        Out (/*OutParam,*/ last_data, data-last_data);
        //debug (verbose>0 && printf( " Decompressed: %u => %u bytes\n", ComprSize+sizeof(int32), data-last_data) );
        //stat ("Decompression", data-last_data);
        if (data==data0+BlockSize)  data=data0;
        free(buf0);
        // NB! check that buf==buf0+Size, data==data0+UncomprSize, and add buffer overflowing checks inside cycle
    }
    free(data0);
    return 0;
}



int unrep(unsigned char *in, int insz, unsigned char *out, int outsz) {
    int bufsize=1<<27, mincompr=100, minlen=512, small_len=0, barrier=8<<20, hashbits=0, amplifier=1;

    unrep_in    = in;
    unrep_inl   = in + insz;
    unrep_out   = out;
    unrep_outl  = out + outsz;
    rep_decompress (bufsize, mincompr, minlen, barrier, small_len, hashbits, amplifier, unrep_fread, in, unrep_fwrite, out);
    return(unrep_out - out);
}

