// modified by Luigi Auriemma
/***********************************************************
*
*    bpe2.c
*
*      Byte Pair Encoding �t�@�C���������̃e�X�g
*      �u���b�N�̃w�b�_���ȗ��ł���ꍇ�ȗ�����
*
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

static
unsigned char   *infile   = NULL,
                *outfile  = NULL,
                *infilel  = NULL,
                *outfilel = NULL;
static int xgetc(void *skip) {
    if(infile >= infilel) return(-1);
    return(*infile++);
}
//static int xputc(int chr, void *skip) {
    //if(outfile >= outfilel) return(-1);
    //*outfile++ = chr;
    //return(chr);
//}
static int xread(void *buff, int a, int b, void *X) {
    int     len,
            inlen;

    len = a * b;
    inlen = (infilel - infile);
    if(len > inlen) len = inlen;
    memcpy(buff, infile, len);
    infile += len;
    return(len);
}
static int xwrite(void *buff, int a, int b, void *X) {
    int     len,
            outlen;

    len = a * b;
    outlen = (outfilel - outfile);
    if(len > outlen) len = outlen;
    memcpy(outfile, buff, len);
    outfile += len;
    return(len);
}

typedef unsigned char  Uchar;
typedef unsigned short Ushort;
typedef unsigned int   Uint;

/* �O���[�o���ϐ��̒�` */
#define  HEADER_SIZE 4
char file_header[HEADER_SIZE] = "BPE2";
char file_ext[] = ".bpe2";
char input_fname[FILENAME_MAX];
char output_fname[FILENAME_MAX];
int  mode;      /* 'e':encode 'd':decode */

FILE *infp, *outfp;      /* ���̓t�@�C��, �o�̓t�@�C�� */

#define TRUE    1
#define FALSE   0
#define BUFMIN  128
#define BUFMAX  32511         /* 0x7eff */
int bufsize = 4096;           /* �f�[�^��Ɨ̈�̃T�C�Y */

Uchar  ptsize;                /* �y�A�\�̑傫�� */
Uchar  ptbuf[256*3];          /* �������p�y�A�\�o�b�t�@ */

Uchar  pairtable1[256];       /* �ꎞ�I�y�A�\(1�Ԗڂ̕���) ���������[�h�ł͕������g���Ă��邩�ǂ����̔���p */
Uchar  pairtable2[256];       /* �ꎞ�I�y�A�\(2�Ԗڂ̕���) */

Uchar  srcbuf[BUFMAX + 16];   /* ���̃f�[�^�̃o�b�t�@ */
Uchar  workbuf[BUFMAX + 16];  /* ���k�f�[�^�̃o�b�t�@ */
Ushort *paircount;            /* �y�A�o���p�J�E���^(malloc�ŗ̈�m�ۂ���, ���������[�h�̂ݎg�p) */
int    ob_count = 0;          /* *outbuf[] ���g���Ă��鐔 */
Uchar  *outbuf[256];          /* �f�[�^�o�͗p�o�b�t�@������(malloc�ŗ̈�m�ۂ���, ���������[�h�̂ݎg�p) */

/*--------------------------------------------------------*/
/*    ��������                                            */
/*--------------------------------------------------------*/

/* workbuf[]��W�J��srcbuf[]�֏��� */
static int decode_buf(int bfs, int isize)
{
	int wpos = 0, spos = 0;
	Uchar stackbuf[256], stackhead = 0;  /* �f�R�[�h�p�X�^�b�N */
	
	while (wpos < isize || stackhead > 0)
	{
		Uchar ch;
		if (!stackhead) {
			/* �X�^�b�N����̎��A�f�[�^����1Byte�Ǎ� */
			ch = workbuf[wpos++];
		} else {
			/* �X�^�b�N����1Byte�Ǎ� */
			ch = stackbuf[--stackhead];
		}

		while (TRUE) {
			/* �y�A�\���當���𓾂� */
			if (ch == pairtable1[ch]) {
				/* �f�[�^�����̂܂�1Byte���� */
				if (spos >= bfs)  return(-1);
				srcbuf[spos++] = ch;
				break;
			}
			/* �f�[�^���X�^�b�N�֓���� */
			stackbuf[stackhead++] = pairtable2[ch];
			ch = pairtable1[ch];
		}
	}
	return spos;
}

/* ���� */
static int decode(void)
{
	int osize = 0;
	
	/* �o�b�t�@�T�C�Y�̓Ǎ� */
	bufsize = xgetc(infp);
	bufsize = (bufsize << 8) | xgetc(infp);
	if (bufsize < BUFMIN || BUFMAX < bufsize)  return(-1);
	//printf("��Ɨ̈�p�������o�b�t�@: %dByte\n", bufsize);
	while (TRUE)
	{
		int i, typ;
		int pts, ch, c2;
		int isize, srcsize;
		/* �u���b�N�w�b�_�Ǎ� */
		if (ob_count > 0) {
			if (xread(workbuf, 1, bufsize, infp) != (size_t)bufsize)  return(-1);
			xwrite(workbuf, 1, bufsize, outfp);
			srcsize = bufsize;
			ob_count--;
		} else {
			int headcode = xgetc(infp);
			if (headcode == EOF)  break;
			if (headcode == 0x7f) {
				ob_count = xgetc(infp);
				headcode = bufsize;
			} else {
				headcode = (headcode << 8) | xgetc(infp);
			}
			
			typ = headcode >> 15;
			isize = headcode & 0x7fff;
			/* �u���b�N�f�[�^�̓Ǎ� */
			if (xread(workbuf, 1, isize, infp) != (size_t)isize)  return(-1);

			/* �y�A�\�������� */
			for (i = 0; i < 256; i++) {
				pairtable1[i] = i;
			}
			if (typ) {
				/* �y�A�\�̓Ǎ� */
				if ((pts = xgetc(infp)) < 0)  return(-1);
				for (i = 0; i < pts; i++) {
					if ((ch = xgetc(infp)) < 0)  return(-1);
					if ((c2 = xgetc(infp)) < 0)  return(-1);
					pairtable1[ch] = c2;
					if ((c2 = xgetc(infp)) < 0)  return(-1);
					pairtable2[ch] = c2;
				}
			}
			srcsize = decode_buf(bufsize, isize);  /* �������� */
            if(srcsize < 0) return(-1);
			xwrite(srcbuf, 1, srcsize, outfp);
		}
		
		/* �r���o�ߕ\�� */
		osize += srcsize;
		//printf("             %d\r", osize);
	}
    return(0);
}



int unbpe2(unsigned char *in, int insz, unsigned char *out, int outsz) {
    infile   = in;
    infilel  = in + insz;
    outfile  = out;
    outfilel = out + outsz;
    if(decode() < 0) return(-1);
    return(outfile - out);
}

