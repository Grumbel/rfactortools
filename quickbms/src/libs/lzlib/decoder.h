/*  Lzlib - Compression library for lzip files
    Copyright (C) 2009, 2010, 2011, 2012, 2013 Antonio Diaz Diaz.

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.

    As a special exception, you may use this file as part of a free
    software library without restriction.  Specifically, if other files
    instantiate templates or use macros or inline functions from this
    file, or you compile this file and link it with other files to
    produce an executable, this file does not by itself cause the
    resulting executable to be covered by the GNU General Public
    License.  This exception does not however invalidate any other
    reasons why the executable file might be covered by the GNU General
    Public License.
*/

enum { rd_min_available_bytes = 8 };

struct Range_decoder
  {
  struct Circular_buffer cb;		/* input buffer */
  unsigned long long member_position;
  uint32_t code;
  uint32_t range;
  bool at_stream_end;
  bool reload_pending;
  };

static inline bool Rd_init( struct Range_decoder * const rdec )
  {
  if( !Cb_init( &rdec->cb, 65536 + rd_min_available_bytes ) ) return false;
  rdec->member_position = 0;
  rdec->code = 0;
  rdec->range = 0xFFFFFFFFU;
  rdec->at_stream_end = false;
  rdec->reload_pending = false;
  return true;
  }

static inline void Rd_free( struct Range_decoder * const rdec )
  { Cb_free( &rdec->cb ); }

static inline bool Rd_finished( const struct Range_decoder * const rdec )
  { return rdec->at_stream_end && !Cb_used_bytes( &rdec->cb ); }

static inline void Rd_finish( struct Range_decoder * const rdec )
  { rdec->at_stream_end = true; }

static inline bool Rd_enough_available_bytes( const struct Range_decoder * const rdec )
  {
  return ( Cb_used_bytes( &rdec->cb ) >= rd_min_available_bytes ||
           ( rdec->at_stream_end && Cb_used_bytes( &rdec->cb ) > 0 ) );
  }

static inline int Rd_available_bytes( const struct Range_decoder * const rdec )
  { return Cb_used_bytes( &rdec->cb ); }

static inline int Rd_free_bytes( const struct Range_decoder * const rdec )
  { if( rdec->at_stream_end ) return 0; return Cb_free_bytes( &rdec->cb ); }

static inline void Rd_purge( struct Range_decoder * const rdec )
  { rdec->at_stream_end = true; Cb_reset( &rdec->cb ); }

static inline void Rd_reset( struct Range_decoder * const rdec )
  { rdec->at_stream_end = false; Cb_reset( &rdec->cb ); }


/* Seeks a member header and updates 'get'.
   Returns true if it finds a valid header.
*/
static bool Rd_find_header( struct Range_decoder * const rdec )
  {
  while( rdec->cb.get != rdec->cb.put )
    {
    if( rdec->cb.buffer[rdec->cb.get] == magic_string[0] )
      {
      int get = rdec->cb.get;
      int i;
      File_header header;
      for( i = 0; i < Fh_size; ++i )
        {
        if( get == rdec->cb.put ) return false;	/* not enough data */
        header[i] = rdec->cb.buffer[get];
        if( ++get >= rdec->cb.buffer_size ) get = 0;
        }
      if( Fh_verify( header ) ) return true;
      }
    if( ++rdec->cb.get >= rdec->cb.buffer_size ) rdec->cb.get = 0;
    }
  return false;
  }


/* Returns true, fills 'header', and updates 'get' if 'get' points to a
   valid header.
   Else returns false and leaves 'get' unmodified.
*/
static bool Rd_read_header( struct Range_decoder * const rdec,
                            File_header header )
  {
  int get = rdec->cb.get;
  int i;
  for( i = 0; i < Fh_size; ++i )
    {
    if( get == rdec->cb.put ) return false;	/* not enough data */
    header[i] = rdec->cb.buffer[get];
    if( ++get >= rdec->cb.buffer_size ) get = 0;
    }
  if( Fh_verify( header ) )
    {
    rdec->cb.get = get;
    rdec->member_position = Fh_size;
    rdec->reload_pending = true;
    return true;
    }
  return false;
  }

static inline int Rd_write_data( struct Range_decoder * const rdec,
                                 const uint8_t * const inbuf, const int size )
  {
  if( rdec->at_stream_end || size <= 0 ) return 0;
  return Cb_write_data( &rdec->cb, inbuf, size );
  }

static inline uint8_t Rd_get_byte( struct Range_decoder * const rdec )
  {
  ++rdec->member_position;
  return Cb_get_byte( &rdec->cb );
  }

static inline int Rd_read_data( struct Range_decoder * const rdec,
                                uint8_t * const outbuf, const int size )
  {
  const int sz = Cb_read_data( &rdec->cb, outbuf, size );
  if( sz > 0 ) rdec->member_position += sz;
  return sz;
  }

static bool Rd_try_reload( struct Range_decoder * const rdec, const bool force )
  {
  if( force ) rdec->reload_pending = true;
  if( rdec->reload_pending && Rd_available_bytes( rdec ) >= 5 )
    {
    int i;
    rdec->reload_pending = false;
    rdec->code = 0;
    for( i = 0; i < 5; ++i )
      rdec->code = (rdec->code << 8) | Rd_get_byte( rdec );
    rdec->range = 0xFFFFFFFFU;
    }
  return !rdec->reload_pending;
  }

static inline void Rd_normalize( struct Range_decoder * const rdec )
  {
  if( rdec->range <= 0x00FFFFFFU )
    {
    rdec->range <<= 8;
    rdec->code = (rdec->code << 8) | Rd_get_byte( rdec );
    }
  }

static inline int Rd_decode( struct Range_decoder * const rdec,
                             const int num_bits )
  {
  int symbol = 0;
  int i;
  for( i = num_bits; i > 0; --i )
    {
    uint32_t mask;
    Rd_normalize( rdec );
    rdec->range >>= 1;
/*    symbol <<= 1; */
/*    if( rdec->code >= rdec->range ) { rdec->code -= rdec->range; symbol |= 1; } */
    mask = 0U - (rdec->code < rdec->range);
    rdec->code -= rdec->range;
    rdec->code += rdec->range & mask;
    symbol = (symbol << 1) + (mask + 1);
    }
  return symbol;
  }

static inline int Rd_decode_bit( struct Range_decoder * const rdec,
                                 Bit_model * const probability )
  {
  uint32_t bound;
  Rd_normalize( rdec );
  bound = ( rdec->range >> bit_model_total_bits ) * *probability;
  if( rdec->code < bound )
    {
    rdec->range = bound;
    *probability += (bit_model_total - *probability) >> bit_model_move_bits;
    return 0;
    }
  else
    {
    rdec->range -= bound;
    rdec->code -= bound;
    *probability -= *probability >> bit_model_move_bits;
    return 1;
    }
  }

static inline int Rd_decode_tree( struct Range_decoder * const rdec,
                                  Bit_model bm[], const int num_bits )
  {
  int symbol = 1;
  int i;
  for( i = num_bits; i > 0; --i )
    symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  return symbol - (1 << num_bits);
  }

static inline int Rd_decode_tree6( struct Range_decoder * const rdec,
                                   Bit_model bm[] )
  {
  int symbol = 1;
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
  return symbol - (1 << 6);
  }

static inline int Rd_decode_tree_reversed( struct Range_decoder * const rdec,
                                           Bit_model bm[], const int num_bits )
  {
  int model = 1;
  int symbol = 0;
  int i;
  for( i = 0; i < num_bits; ++i )
    {
    const bool bit = Rd_decode_bit( rdec, &bm[model] );
    model <<= 1;
    if( bit ) { ++model; symbol |= (1 << i); }
    }
  return symbol;
  }

static inline int Rd_decode_tree_reversed4( struct Range_decoder * const rdec,
                                            Bit_model bm[] )
  {
  int model = 1;
  int symbol = 0;
  int bit = Rd_decode_bit( rdec, &bm[model] );
  model = (model << 1) + bit; symbol |= bit;
  bit = Rd_decode_bit( rdec, &bm[model] );
  model = (model << 1) + bit; symbol |= (bit << 1);
  bit = Rd_decode_bit( rdec, &bm[model] );
  model = (model << 1) + bit; symbol |= (bit << 2);
  if( Rd_decode_bit( rdec, &bm[model] ) ) symbol |= 8;
  return symbol;
  }

static inline int Rd_decode_matched( struct Range_decoder * const rdec,
                                     Bit_model bm[], int match_byte )
  {
  Bit_model * const bm1 = bm + 0x100;
  int symbol = 1;
  int i;
  for( i = 7; i >= 0; --i )
    {
    int match_bit, bit;
    match_byte <<= 1;
    match_bit = match_byte & 0x100;
    bit = Rd_decode_bit( rdec, &bm1[match_bit+symbol] );
    symbol = ( symbol << 1 ) | bit;
    if( match_bit != bit << 8 )
      {
      while( symbol < 0x100 )
        symbol = ( symbol << 1 ) | Rd_decode_bit( rdec, &bm[symbol] );
      break;
      }
    }
  return symbol - 0x100;
  }

static inline int Rd_decode_len( struct Range_decoder * const rdec,
                                 struct Len_model * const lm,
                                 const int pos_state )
  {
  if( Rd_decode_bit( rdec, &lm->choice1 ) == 0 )
    return Rd_decode_tree( rdec, lm->bm_low[pos_state], len_low_bits );
  if( Rd_decode_bit( rdec, &lm->choice2 ) == 0 )
    return len_low_symbols +
           Rd_decode_tree( rdec, lm->bm_mid[pos_state], len_mid_bits );
  return len_low_symbols + len_mid_symbols +
         Rd_decode_tree( rdec, lm->bm_high, len_high_bits );
  }


enum { lzd_min_free_bytes = max_match_len };

struct LZ_decoder
  {
  struct Circular_buffer cb;
  unsigned long long partial_data_pos;
  int dictionary_size;
  uint32_t crc;
  bool member_finished;
  bool verify_trailer_pending;
  unsigned rep0;		/* rep[0-3] latest four distances */
  unsigned rep1;		/* used for efficient coding of */
  unsigned rep2;		/* repeated distances */
  unsigned rep3;
  State state;

  Bit_model bm_literal[1<<literal_context_bits][0x300];
  Bit_model bm_match[states][pos_states];
  Bit_model bm_rep[states];
  Bit_model bm_rep0[states];
  Bit_model bm_rep1[states];
  Bit_model bm_rep2[states];
  Bit_model bm_len[states][pos_states];
  Bit_model bm_dis_slot[len_states][1<<dis_slot_bits];
  Bit_model bm_dis[modeled_distances-end_dis_model];
  Bit_model bm_align[dis_align_size];

  struct Range_decoder * rdec;
  struct Len_model match_len_model;
  struct Len_model rep_len_model;
  };

static inline bool LZd_enough_free_bytes( const struct LZ_decoder * const decoder )
  { return Cb_free_bytes( &decoder->cb ) >= lzd_min_free_bytes; }

static inline uint8_t LZd_get_prev_byte( const struct LZ_decoder * const decoder )
  {
  const int i =
    ( ( decoder->cb.put > 0 ) ? decoder->cb.put : decoder->cb.buffer_size ) - 1;
  return decoder->cb.buffer[i];
  }

static inline uint8_t LZd_get_byte( const struct LZ_decoder * const decoder,
                                    const int distance )
  {
  int i = decoder->cb.put - distance - 1;
  if( i < 0 ) i += decoder->cb.buffer_size;
  return decoder->cb.buffer[i];
  }

static inline void LZd_put_byte( struct LZ_decoder * const decoder,
                                 const uint8_t b )
  {
  CRC32_update_byte( &decoder->crc, b );
  decoder->cb.buffer[decoder->cb.put] = b;
  if( ++decoder->cb.put >= decoder->cb.buffer_size )
    { decoder->partial_data_pos += decoder->cb.put; decoder->cb.put = 0; }
  }

static inline void LZd_copy_block( struct LZ_decoder * const decoder,
                                   const int distance, int len )
  {
  int i = decoder->cb.put - distance - 1;
  if( i < 0 ) i += decoder->cb.buffer_size;
  if( len < decoder->cb.buffer_size - max( decoder->cb.put, i ) &&
      len <= abs( decoder->cb.put - i ) )	/* no wrap, no overlap */
    {
    CRC32_update_buf( &decoder->crc, decoder->cb.buffer + i, len );
    memcpy( decoder->cb.buffer + decoder->cb.put, decoder->cb.buffer + i, len );
    decoder->cb.put += len;
    }
  else for( ; len > 0; --len )
    {
    LZd_put_byte( decoder, decoder->cb.buffer[i] );
    if( ++i >= decoder->cb.buffer_size ) i = 0;
    }
  }

static inline bool LZd_init( struct LZ_decoder * const decoder,
                             const File_header header,
                             struct Range_decoder * const rde )
  {
  decoder->dictionary_size = Fh_get_dictionary_size( header );
  if( !Cb_init( &decoder->cb, max( 65536, decoder->dictionary_size ) + lzd_min_free_bytes ) )
    return false;
  decoder->partial_data_pos = 0;
  decoder->crc = 0xFFFFFFFFU;
  decoder->member_finished = false;
  decoder->verify_trailer_pending = false;
  decoder->rep0 = 0;
  decoder->rep1 = 0;
  decoder->rep2 = 0;
  decoder->rep3 = 0;
  decoder->state = 0;

  Bm_array_init( decoder->bm_literal[0], (1 << literal_context_bits) * 0x300 );
  Bm_array_init( decoder->bm_match[0], states * pos_states );
  Bm_array_init( decoder->bm_rep, states );
  Bm_array_init( decoder->bm_rep0, states );
  Bm_array_init( decoder->bm_rep1, states );
  Bm_array_init( decoder->bm_rep2, states );
  Bm_array_init( decoder->bm_len[0], states * pos_states );
  Bm_array_init( decoder->bm_dis_slot[0], len_states * (1 << dis_slot_bits) );
  Bm_array_init( decoder->bm_dis, modeled_distances - end_dis_model );
  Bm_array_init( decoder->bm_align, dis_align_size );

  decoder->rdec = rde;
  Lm_init( &decoder->match_len_model );
  Lm_init( &decoder->rep_len_model );
  decoder->cb.buffer[decoder->cb.buffer_size-1] = 0; /* prev_byte of first_byte */
  return true;
  }

static inline void LZd_free( struct LZ_decoder * const decoder )
  { Cb_free( &decoder->cb ); }

static inline bool LZd_member_finished( const struct LZ_decoder * const decoder )
  { return ( decoder->member_finished && !Cb_used_bytes( &decoder->cb ) ); }

static inline unsigned LZd_crc( const struct LZ_decoder * const decoder )
  { return decoder->crc ^ 0xFFFFFFFFU; }

static inline unsigned long long
LZd_data_position( const struct LZ_decoder * const decoder )
  { return decoder->partial_data_pos + decoder->cb.put; }
