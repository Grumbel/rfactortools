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

struct Circular_buffer
  {
  uint8_t * buffer;
  int buffer_size;		/* capacity == buffer_size - 1 */
  int get;			/* buffer is empty when get == put */
  int put;
  };

static inline void Cb_reset( struct Circular_buffer * const cb )
  { cb->get = 0; cb->put = 0; }

static inline bool Cb_init( struct Circular_buffer * const cb,
                            const int buf_size )
  {
  cb->buffer = (uint8_t *)malloc( buf_size + 1 );
  cb->buffer_size = buf_size + 1;
  cb->get = 0;
  cb->put = 0;
  return ( cb->buffer != 0 );
  }

static inline void Cb_free( struct Circular_buffer * const cb )
  { free( cb->buffer ); cb->buffer = 0; }

static inline int Cb_used_bytes( const struct Circular_buffer * const cb )
    { return ( (cb->get <= cb->put) ? 0 : cb->buffer_size ) + cb->put - cb->get; }

static inline int Cb_free_bytes( const struct Circular_buffer * const cb )
    { return ( (cb->get <= cb->put) ? cb->buffer_size : 0 ) - cb->put + cb->get - 1; }

static inline uint8_t Cb_get_byte( struct Circular_buffer * const cb )
    {
    const uint8_t b = cb->buffer[cb->get];
    if( ++cb->get >= cb->buffer_size ) cb->get = 0;
    return b;
    }

static inline void Cb_put_byte( struct Circular_buffer * const cb,
                                const uint8_t b )
    {
    cb->buffer[cb->put] = b;
    if( ++cb->put >= cb->buffer_size ) cb->put = 0;
    }


/* Copies up to 'out_size' bytes to 'out_buffer' and updates 'get'.
   Returns the number of bytes copied.
*/
static int Cb_read_data( struct Circular_buffer * const cb,
                         uint8_t * const out_buffer, const int out_size )
  {
  int size = 0;
  if( out_size < 0 ) return 0;
  if( cb->get > cb->put )
    {
    size = min( cb->buffer_size - cb->get, out_size );
    if( size > 0 )
      {
      memcpy( out_buffer, cb->buffer + cb->get, size );
      cb->get += size;
      if( cb->get >= cb->buffer_size ) cb->get = 0;
      }
    }
  if( cb->get < cb->put )
    {
    const int size2 = min( cb->put - cb->get, out_size - size );
    if( size2 > 0 )
      {
      memcpy( out_buffer + size, cb->buffer + cb->get, size2 );
      cb->get += size2;
      size += size2;
      }
    }
  return size;
  }


/* Copies up to 'in_size' bytes from 'in_buffer' and updates 'put'.
   Returns the number of bytes copied.
*/
static int Cb_write_data( struct Circular_buffer * const cb,
                          const uint8_t * const in_buffer, const int in_size )
  {
  int size = 0;
  if( in_size < 0 ) return 0;
  if( cb->put >= cb->get )
    {
    size = min( cb->buffer_size - cb->put - (cb->get == 0), in_size );
    if( size > 0 )
      {
      memcpy( cb->buffer + cb->put, in_buffer, size );
      cb->put += size;
      if( cb->put >= cb->buffer_size ) cb->put = 0;
      }
    }
  if( cb->put < cb->get )
    {
    const int size2 = min( cb->get - cb->put - 1, in_size - size );
    if( size2 > 0 )
      {
      memcpy( cb->buffer + cb->put, in_buffer + size, size2 );
      cb->put += size2;
      size += size2;
      }
    }
  return size;
  }
