//
// unstargun.cpp
//
// Stargunner decompression utility.
//
// Written by Adam Nielsen <malvineous@shikadi.net>.  Based on The_coder's work
// for tombexcavator.
//
// Use: unstargun < infile > outfile
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
/// Decompress a data chunk.
/**
 * @param in
 *   Input data.  First byte is the one immediately following the chunk length.
 *
 * @param expanded_size
 *   The size of the input chunk after decompression.  The output buffer must
 *   be able to hold this many bytes.
 *
 * @param out
 *   Output buffer.
 */
unsigned int unstargun(const uint8_t* in, size_t expanded_size, uint8_t* out)
{
	uint8_t tableA[256], tableB[256];
	unsigned int inpos = 0;
	unsigned int outpos = 0;
    int i;
 
	while (outpos < expanded_size) {
		// Initialise the dictionary so that no bytes are codewords (or if you
		// prefer, each byte expands to itself only.)
		for (i = 0; i < 256; i++) tableA[i] = i;
 
		//
		// Read in the dictionary
		//
 
		uint8_t code;
		unsigned int tablepos = 0;
		do {
			code = in[inpos++];
 
			// If the code has the high bit set, the lower 7 bits plus one is the
			// number of codewords that will be skipped from the dictionary.  (Those
			// codewords were initialised to expand to themselves in the loop above.)
			if (code > 127) {
				tablepos += code - 127;
				code = 0;
			}
			if (tablepos == 256) break;
 
			// Read in the indicated number of codewords.
			for (i = 0; i <= code; i++) {
				//assert(tablepos < 256);
                if(tablepos >= 256) return(-1);
				uint8_t data = in[inpos++];
				tableA[tablepos] = data;
				if (tablepos != data) {
					// If this codeword didn't expand to itself, store the second byte
					// of the expansion pair.
					tableB[tablepos] = in[inpos++];
				}
				tablepos++;
			}
		} while (tablepos < 256);
 
		// Read the length of the data encoded with this dictionary
		int len = in[inpos++];
		len |= in[inpos++] << 8;
 
		//
		// Decompress the data
		//
 
		int expbufpos = 0;
		// This is the maximum number of bytes a single codeword can expand to.
		uint8_t expbuf[32];
		while (1) {
			if (expbufpos) {
				// There is data in the expansion buffer, use that
				code = expbuf[--expbufpos];
			} else {
				// There is no data in the expansion buffer, use the input data
				if (--len == -1) break; // no more input data
				code = in[inpos++];
			}
 
			if (code == tableA[code]) {
				// This byte is itself, write this to the output
				out[outpos++] = code;
			} else {
				// This byte is actually a codeword, expand it into the expansion buffer
				//assert(expbufpos < (signed)sizeof(expbuf) - 2);
                if(expbufpos >= (signed)sizeof(expbuf) - 2) return(-1);
				expbuf[expbufpos++] = tableB[code];
				expbuf[expbufpos++] = tableA[code];
			}
		}
	}
	return outpos - expanded_size;
}

