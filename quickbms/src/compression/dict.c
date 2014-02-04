// modified by Luigi Auriemma

// http://freearc.org/download/research/dict.zip
/*

DICT - ������ ᫮��୮� ������. ����஥��� ᫮���� �����⢫����
       �� �६� ࠡ��� �ணࠬ��, �ᯮ�짮����� ᫮���� �뢮�����
       ��। ������஢���묨 ����묨
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define USE_DICT2                    1

#define dict_put_byte(c)      (*outptr++ = (c))

#define dict2(i,j)       (dict2_var [i*(UCHAR_MAX+1) + j])
#define dict_len(i)      (dict[i]? dict[i]->len : 0)
#define dict_ptr(i)      (dict[i]->ptr)
#define dict2_len(i,j)   (dict2(i,j)? dict2(i,j)->len : 0)
#define dict2_ptr(i,j)   (dict2(i,j)->ptr)

// ���������� *****************************************************************************

// �������-�����, �ᯮ��㥬� ��� ������஢����
typedef struct
{
    unsigned len;     // ����� ᫮��
    byte *ptr;        // ��砫� ᫮��
} dict_entry;
dict_entry dict[UCHAR_MAX+1];

#define dict_get_byte()       (*ptr++)
#define dict_put_byte(c)      (*outptr++ = (c))
#define dict_put_word(p,len)  (memcpy (outptr, (p), (len)), outptr += (len))

// ��ᯠ������ �室�� ����� buf[bufsize] � outbuf � �������� ࠧ��� �ᯠ�������� ������
int DictDecode (byte *buf, unsigned bufsize, byte *outbuf, unsigned *outsize)
{
    int retcode = 0;
    byte *ptr = buf,
         *end = buf+bufsize,
         *outptr = outbuf;     // ⥪�騩 㪠��⥫� � ��室��� ����

    // �������-�����, �ᯮ��㥬� ��� ������஢���� 2-���⮢�� ᫮�
    dict_entry *dict2_var = (dict_entry*) calloc ((UCHAR_MAX+1)*(UCHAR_MAX+1), sizeof (dict_entry));

    //stat1 ("������ �������");
    // ���⮨� �� 5 横���:
    //   1. ������ 256 ���� - �� ����� ��� ᫮�, �����㥬�� ����� ���⮬
    //        (0 ����砥�, �� ��� ���� ᫮� �� �������, 1 ����砥�, �� � �⮣� ���� ��稭����� ���� 256 ᫮�)
    //   2. ������ ����� ��� ᫮�, �����㥬�� ���� ���⠬�
    //        (256*n ����, ��� n - ������⢮ ������, ���⠭��� �� �।��饬 �⠯�)
    //   3. ������ ⥪�� ��� �������⮢�� ᫮�
    //   4. ������ ⥪�� ��� ���塠�⮢�� ᫮�
    //   5. ������� �ᥢ��-᫮�� ��� ������஢���� �� ᨬ�����, ����� �⤠�� ᢮� ���� ᫮���
    int dictsize = 0, words2 = 0;
    int i, j, k;
    dict_entry *d;

    for( i=0; i<=UCHAR_MAX; i++ ) {
        dictsize += dict[i].len = dict_get_byte();
    }
    for( i=0; i<=UCHAR_MAX; i++ ) {
        if( dict[i].len==USE_DICT2 ) {
            for( j=0; j<=UCHAR_MAX; j++ ) {
                dictsize += dict2(i,j).len = dict_get_byte();
                words2++;
            }
        }
    }
    // ���� ��� �࠭���� ⥪�� ᫮� (������ ��� ���� ᥩ�� �뤥����� �� �����, �� � ����訬 ����ᮬ :)
    byte *words = (byte*) malloc (dictsize+UCHAR_MAX+1+words2*20+100000), *wordptr = words;
    for( i=0; i<=UCHAR_MAX; i++ ) {
        if (dict[i].len == USE_DICT2)  continue;
        dict[i].ptr = wordptr;
        for( k=0; k<dict[i].len; k++ ) {
            *wordptr++ = dict_get_byte();
        }
    }
    {
    byte word_sep = dict_get_byte();
    byte *prevptr = NULL;
    for( i=0; i<=UCHAR_MAX; i++ ) {
        if( dict[i].len==USE_DICT2 ) {
            for( j=0; j<=UCHAR_MAX; j++ ) {
                dict2(i,j).ptr = wordptr;
                // ������㥬 ��砫� ᫮�� �� �।��饣�
                for( k=0; k<dict2(i,j).len; k++ ) {
                    if (prevptr==NULL)  {retcode = -1; goto done;}  // �訡�� �� �室��� ������ - ����஢���� ������ �� �।��饣� ᫮��, ���ண� ��� :)
                    *wordptr++ = *prevptr++;
                }
                // � ���⠥� ���⮪ ᫮�� �� �室���� ��⮪�
                for(;;) {
                    byte c = dict_get_byte();
                    if (c==word_sep) break;
                    *wordptr++ = c;
                }
                dict2(i,j).len = wordptr - dict2(i,j).ptr;
                prevptr = dict2(i,j).ptr;
            }
        }
    }
    // ��䨪�, �ᯮ��㥬� ��� ����஢���� �ࠤ����� ᨬ�����
    byte prefix = dict_get_byte();
    dict[prefix].len = USE_DICT2;
    // �������� �ᥢ��-᫮��, �������騥 �ࠤ���� ᨬ����
    for (j=0; j<=UCHAR_MAX; j++) {
        dict2(prefix,j).len = 1;
        dict2(prefix,j).ptr = wordptr;
        *wordptr++ = (byte)j;
    }


    //stat1 ("������������ �����, ��������� ����������� ���� �������");
    while( ptr<end ) {
        byte c = dict_get_byte();
        d = &dict[c];

        // �᫨ ��� ���� �� ������� ������� ᫮�, � �뢥�� ��� ᠬ �� ᥡ�
        if (d->len == 0) {
            dict_put_byte(c);

        // �᫨ ��� ���� - ��砫� ���� ���塠�⮢��� ᫮��, � �뢥�� �� ᫮��
        } else if( d->len == USE_DICT2 ) {
            byte c2 = dict_get_byte();
            d = &dict2(c,c2);
            dict_put_word (d->ptr, d->len);

        // ���� ��� ���� - ��砫� ���� �������⮢��� ᫮��
        } else {
            dict_put_word (d->ptr, d->len);
        }
    }
    }
done:
    free(words);
    free(dict2_var);

    // ������� ����� ������஢������ ⥪�� � ������ ��� (���)�ᯥ譮�� �����襭��
    *outsize = outptr-outbuf;
    return retcode;
}

