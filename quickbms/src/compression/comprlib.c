// modified by Luigi Auriemma

// http://sourceforge.net/projects/comprlib/
/* By Jeff Connelly */



#define comprlib_function(X) \
    int X(unsigned char *in, int insz, unsigned char *out, int outsz) { \
        comprlib_in     = in; \
        comprlib_inl    = in + insz; \
        comprlib_out    = out; \
        comprlib_outl   = out + outsz; \
        comprlib_inx    = in;



#define comprlib_BufSize     16384       /* size of input and output buffers */
#define comprlib_Sig         0xff02aa55L /* arbitrary signature denotes compressed file */
#define comprlib_MaxChar     256         /* ordinal of highest character */
#define comprlib_EofChar     256         /* used to mark end of compressed file */
#define comprlib_PredMax     255         /* MaxChar - 1 */
#define comprlib_TwiceMax    512         /* 2 * MaxChar */
#define comprlib_Root        0           /* index of root node */

/* Used to unpack bits and bytes */
byte comprlib_BitMask[8]={1, 2, 4, 8, 16, 32, 64, 128};

#define comprlib_block_copy(source,destination,source_size,nb_of_time)  { register unsigned int i, index=0;\
                                                                 for (i=1;i<=(nb_of_time);i++)\
                                                                     { (void)memcpy(&((destination)[index]),(source),(source_size));\
                                                                       index += (source_size);\
                                                                     }\
                                                               }
#define comprlib_fill_array(array,nb_to_fill,value)  ((void)memset((array),(value),(nb_to_fill)))



unsigned char   *comprlib_in,
                *comprlib_inl,
                *comprlib_out,
                *comprlib_outl,
                *comprlib_inx;



typedef word comprlib_CodeType;  /* 0..MaxChar */
typedef byte comprlib_UpIndex;   /* 0..PredMax */
typedef word comprlib_DownIndex; /* 0..TwiceMax */
typedef comprlib_DownIndex comprlib_TreeDownArray[comprlib_PredMax + 1];
typedef comprlib_UpIndex comprlib_TreeUpArray[comprlib_TwiceMax + 1];     /* DownIndex */
comprlib_TreeDownArray comprlib_Left, comprlib_Right;
comprlib_TreeUpArray comprlib_Up;
int comprlib_Index,
    comprlib_BitPos;



int comprlib_read_byte(void) {
    unsigned char   c;

    if(comprlib_in >= comprlib_inl) return(-1);
    c = comprlib_in[0];
    comprlib_in++;
    return(c);
}
int comprlib_read_array(unsigned char *buff, int size) {
    int     i;
    for(i = 0; i < size; i++) {
        buff[i] = comprlib_read_byte();
    }
    return(i);
}
int comprlib_write_byte(int c) {
    if(comprlib_out >= comprlib_outl) return(-1);
    comprlib_out[0] = c;
    comprlib_out++;
    return(c);
}
int comprlib_write_array(unsigned char *buff, int size) {
    int     i;
    for(i = 0; i < size; i++) {
        comprlib_write_byte(buff[i]);
    }
    return(i);
}
int comprlib_write_block(int c, int size) {
    int     i;
    for(i = 0; i < size; i++) comprlib_write_byte(c);
    return(i);
}
int comprlib_beginning_of_data(void) {
    comprlib_in = comprlib_inx;
    return(0);
}
int comprlib_end_of_data(void) {
    if(comprlib_in >= comprlib_inl) return(1);
    return(0);
}
static byte comprlib_GetByte(void)
{
    comprlib_Index++;
    return comprlib_read_byte();

    /* return InBuffer[comprlib_Index]; */
}
static void comprlib_Splay(comprlib_CodeType Plain)
{
    comprlib_DownIndex A, B;
    comprlib_UpIndex C, D;
    
    A = Plain + comprlib_MaxChar;
    
    do
    {
        /* walk up the tree semi-rotating pairs */
        C = comprlib_Up[A];
        if (C != comprlib_Root)
        {
            /* a pair remains */
            D = comprlib_Up[C];
            
            /* exchange children of pair */
            B = comprlib_Left[D];
            if (C == B)
            {
                B = comprlib_Right[D];
                comprlib_Right[D] = A;
            }
            else
                comprlib_Left[D] = A;
            
            if (A == comprlib_Left[C])
                comprlib_Left[C] = B;
            else
                comprlib_Right[C] = B;
            
            comprlib_Up[A] = D;
            comprlib_Up[B] = C;
            A = D;
        }
        else
            A = C;
    } while (A != comprlib_Root);
}
static int comprlib_Expand(void)
{
    comprlib_DownIndex A;
    int InByte = 0;
    
    /* scan the tree to a leaf, which determines the character */
    A = comprlib_Root;
    do
    {
        if (comprlib_BitPos == 7)
        {
            /* used up bits in current byte, get another */
            InByte = comprlib_GetByte();
            comprlib_BitPos = 0;
        }
        else
            comprlib_BitPos++;
        
        if ((InByte & comprlib_BitMask[comprlib_BitPos]) == 0)
            A = comprlib_Left[A];
        else
            A = comprlib_Right[A];
    } while (A <= comprlib_PredMax);
    
    /* Update the code tree */
    A -= comprlib_MaxChar;
    comprlib_Splay(A);
    
    /* return the character */
    return A;
}



comprlib_function(decode_spread)
    int i, c, lucky = 0, mask = 1;

    while(!comprlib_end_of_data())
    {
        for (i = 0; i < 7; ++i) /* Read unlucky with lucky bytes spread over*/
        {
            c = comprlib_read_byte();
            if (c & 0x80)
            {
                lucky |= mask;  /* Construct lucky byte */
                c -= 0x80;      /* Remove MSB */
            }
            comprlib_write_byte(c);
            mask *= 2;
        }
        mask = 0;
        comprlib_write_byte(lucky);
    }
    return(comprlib_out - out);
    return 0;
}



comprlib_function(decode_rle1)
  unsigned char header;
  register unsigned char i;

  while (!comprlib_end_of_data())
        { header=comprlib_read_byte();
          switch (header & 128)
          { case 0:for (i=0;i<=header;i++)
                       comprlib_write_byte(comprlib_read_byte());
                   break;
            case 128:comprlib_write_block(comprlib_read_byte(),(header & 127)+2);
          }
        }
        return(comprlib_out - out);
        return 0;
}



comprlib_function(decode_rle2)
  unsigned char header_byte,byte_read,byte_to_repeat;
  register unsigned int i;

  if (!comprlib_end_of_data())
     { header_byte=comprlib_read_byte();
       do {                  /* Being that header byte is present, then there are bytes to decompress */
            if ((byte_read=comprlib_read_byte())==header_byte)
               { byte_read=comprlib_read_byte();
                 if (byte_read<3)
                    byte_to_repeat=header_byte;
                 else byte_to_repeat=comprlib_read_byte();
                 for (i=0;i<=byte_read;i++)
                     comprlib_write_byte(byte_to_repeat);
               }
            else comprlib_write_byte(byte_read);
          }
       while (!comprlib_end_of_data());
     }
     return(comprlib_out - out);
     return 0;
}



comprlib_function(decode_rle3)
  unsigned char header_byte,byte_read,nb_repetitions,
                raster[256];
  register unsigned int frame_length,
                        i;

  if (!comprlib_end_of_data())
     { header_byte=comprlib_read_byte();
       do {                  /* Being that header byte is present, then there are bytes to decompress */
            byte_read=comprlib_read_byte();
            if (byte_read==header_byte)
                             /* Encoding of a repetition of the header byte */
               { nb_repetitions=comprlib_read_byte();
                 frame_length=((unsigned int)comprlib_read_byte())+1;
                 if (!nb_repetitions)
                    for (i=1;i<=frame_length;i++)
                        comprlib_write_byte(header_byte);
                 else { comprlib_read_array(raster,frame_length);
                        for (i=0;i<=nb_repetitions;i++)
                            comprlib_write_array(raster,frame_length);
                      }
               }
            else comprlib_write_byte(byte_read);
          }
       while (!comprlib_end_of_data());
     }
    return(comprlib_out - out);
}



comprlib_function(rle4decoding)
  unsigned char byte_code;
  unsigned int frame_size=0,frame_nb;
  unsigned char frame[16705];

/*
  if (comprlib_read_byte() != 'R' || comprlib_read_byte() != 'L' || comprlib_read_byte() != '4')
  {
    fprintf (stderr, "Not encoded with CODRLE4.EXE");
    exit (BAD_FILE);
  }
*/
  while (!comprlib_end_of_data())
        { byte_code=comprlib_read_byte();
          switch (byte_code & 192)
          { case 0:          /* Frames repetition of 1 byte
                                Encoding [00xxxxxx|1 byte] */
                   frame_size=(byte_code & 63)+2;
                   comprlib_fill_array(frame,frame_size,comprlib_read_byte());
                   break;
            case 64:         /* Frames repetition of less 66 bytes
                                Encoding [01xxxxxx|xxxxxxxx|1 byte] */
                    frame_size=(((unsigned int)(byte_code & 63)) << 8)+comprlib_read_byte()+66;
                    comprlib_fill_array(frame,frame_size,comprlib_read_byte());
                    break;
            case 128:        /* Frame with several bytes
                                Encoding [10xxxxxx|yyyyyyyy|n bytes] */
                     frame_size=(byte_code & 63)+2;
                     frame_nb=((unsigned int)comprlib_read_byte())+2;
                     comprlib_read_array(frame,frame_size);
                     comprlib_block_copy(frame,frame+frame_size,frame_size,frame_nb);
                     frame_size *= frame_nb;
                     break;
            case 192:        /* No repetition
                                Encoding [110xxxxxx|n octets] or [111xxxxxx|yyyyyyyy|n octets] */
                     frame_size=byte_code & 31;
                     if (!(byte_code & 32))
                             /* Non repetition of less 33 bytes [110xxxxxx|n bytes] ? */
                        frame_size++;
                     else frame_size=(frame_size << 8)+comprlib_read_byte()+33;
                     comprlib_read_array(frame,frame_size);
                     break;
          }
          comprlib_write_array(frame,frame_size);
        }
    return(comprlib_out - out);
}



int comprlib_diff[0x10000];
int comprlib_inc0tab [0x10000];
int comprlib_inc1tab [0x10000];
int comprlib_cnt[0x100];


#define comprlib_INC0(x) comprlib_inc0tab[(unsigned int)(x)]
#define comprlib_INC1(x) comprlib_inc1tab[(unsigned int)(x)]
#define comprlib_DIFF(x) comprlib_diff[(unsigned int)(x)]
#define comprlib_CNT1(x) ((x&0xff)+1)
#define comprlib_CNT0(x) ((x>>8)+1)

int comprlib_init() {
   int i,j;
   for (i=0;i<256;i++) for (j=0;j<256;j++) {
     if (i != 255)
        comprlib_inc0tab[(i<<8)+j] = ((i+1)<<8) + j;
     else
        comprlib_inc0tab[(i<<8)+j] = (127<<8) + (j>>1);
     if (j != 255)
        comprlib_inc1tab[(i<<8)+j] = (i<<8) + j + 1;
     else
        comprlib_inc1tab[(i<<8)+j] = ((i>>1)<<8) + 127;
     if (i < j) {
        comprlib_diff[(i<<8)+j] = 175 * (i+1) / (i+j+2);
        if (!comprlib_diff[(i<<8)+j]) comprlib_diff[(i<<8)+j]++;
     }else{
        comprlib_diff[(i<<8)+j] = -175 * (j+1) / (i+j+2);
        if (!comprlib_diff[(i<<8)+j]) comprlib_diff[(i<<8)+j]--;
     }
   }
   return 0;
}



comprlib_function(decode_arith)
register int count = -8;
unsigned register int mask;
register int space = 0xff,
    min = 0;
unsigned register char c=0,gc = 0;
unsigned register int last = 0;
unsigned register int val;

comprlib_init();

val = comprlib_read_byte();
gc = comprlib_read_byte();

do{
   c = 0;
   comprlib_Index = 1;
   for(mask=0x80;mask;mask>>=1){
         register int l = last  + comprlib_Index;
         register int a = comprlib_cnt[l];
         register int x = comprlib_DIFF(a);
         if (x>0) 
            if ((unsigned)val < (unsigned)min+space-x) {
               c |= mask;
               space -=x;
               comprlib_cnt[l] = comprlib_INC1(a);
               comprlib_Index += comprlib_Index + 1;
            }else { 
               min += space-x;
               space = x;
               comprlib_cnt[l] = comprlib_INC0(a);
               comprlib_Index += comprlib_Index;
            }
         else 
            if ((unsigned)val < (unsigned)min+space+x) {
               space +=x; 
               comprlib_cnt[l] = comprlib_INC0(a);
               comprlib_Index += comprlib_Index;
            } else { 
               c |= mask;
              min += space+x; 
              space = -x;
              comprlib_cnt[l] = comprlib_INC1(a);
              comprlib_Index += comprlib_Index+1;
            }
      while ((space) < 128) {
         space <<= 1;
         min <<= 1;
         val = (val<<1) | (gc>>(7-(count&7)));
         if (!++count) {
            count = -8;
            gc = comprlib_read_byte();
            min &= 0xffffff;
            val &= 0xffffff;
            if (0xffffff - min < space){
                space = 0xffffff - min;
                }
            }
         }
      }
   if (c == (unsigned char)EOF) break;
   comprlib_write_byte(c);
   }
while (1);
   return(comprlib_out - out);
   return 0;
}



comprlib_function(decode_splay)
    /* force buffer load first time */
    comprlib_Index = 0;
    //InSize = 0;
    /* nothing in output buffer */
    //OutSize = 0;
    /* force bit buffer load first time */
    comprlib_BitPos = 7;
    
    /* read and comprlib_Expand the compressed input */
    int OutByte = comprlib_Expand();
    while (OutByte != comprlib_EofChar)
    {
        comprlib_write_byte(OutByte);
        OutByte = comprlib_Expand();
    }
    return(comprlib_out - out);
    return 0;
}


