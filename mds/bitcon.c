/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* The bit concentrator library for Meme.
 * Portions copyright (c) Immersive Systems, Inc.  All rights reserved.
 * This code originally appeared as lzari.c -- a data compression program,
 * written by Haruhiko Okumura    CompuServe	74050,1022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include "bitcon.h"

/****** Globals *********/
unsigned long bitcon_textsize = 0;
unsigned long bitcon_codesize = 0;

/********* Error routine *********/
static	jmp_buf	errExit;
void
Error(long errnum)
{
	longjmp(errExit, (int)errnum);
}

/**********************************************************/
/* Data streams for the compress and decompress functions */

/* The input data stream and its length are passed to Compress()
 * and Expand() by their callers.  It won't change size.
 *
 * The output stream is alloc'd by InitializeIO() and may
 * need to be realloc'd, so two pointers are used for the output.
 * The outputRemaining variable holds a count of the room
 * remaining in the output buffer.
 */
static unsigned char *currentInputChar;	/* Ptr to next char in stream	*/
static unsigned long inputRemaining;	/* # of chars left to read		*/

static unsigned char *outputData;		/* Ptr to start of output array	*/
static unsigned char *currentOutputChar;	/* Ptr to next space for output */
static unsigned long outputRemaining;	/* # of bytes left in alloc'd array */
static unsigned long outputDataAccumulated;

#define	MALLOC_CHUNK	(10000)			/* Increase the output buffer by
										 * this length when necessary
										 */

int
InitializeIO(unsigned char *inpDataPtr, unsigned long len)
{
	inputRemaining = len;
	currentInputChar = inpDataPtr;

	/* Start with MALLOC_CHUNK bytes for the output */
	outputData = (unsigned char *)malloc(MALLOC_CHUNK);
	if (outputData == NULL) return 1;
	currentOutputChar = outputData;
	outputRemaining = MALLOC_CHUNK;
	outputDataAccumulated = 0;
	return 0;
}
	
/* Get the next byte from the input stream.
 * The input argument is the addr that will
 * receive the next byte of data.
 * Return value is non-zero if no more data.
 *
 * It turns out that this routine can be called multiple
 * times after it has returned a non-zero value.
 * GetBit can call this routine after there is
 * no more input.
 */
int
GetInputByte(unsigned char *theByte)
{
	if (inputRemaining--) {
		*theByte = *currentInputChar++;
		return 0;
	} else {
		inputRemaining++;	/* Make sure it's zero in case this
							 * routine gets called again.
							 */
		*theByte = 0xff;	/* Emulate returning EOF, as is done in lzari.c */
		return 1;
	}
}

int
PutOutputByte(unsigned char obyte)
{
	unsigned char *p;

	if (outputRemaining--) {
		*currentOutputChar++ = obyte;
		outputDataAccumulated++;
		return 0;
	} else {
		p = (unsigned char *)realloc(outputData, outputDataAccumulated + MALLOC_CHUNK);
		if (p == NULL) return 1;
		currentOutputChar += (long)(p - outputData);
		outputData = p;
		outputRemaining = MALLOC_CHUNK - 1;

		*currentOutputChar++ = obyte;
		outputDataAccumulated++;
		return 0;
	}
}

/********** Bit I/O **********/

static void
PutBit(int bit)  			/* Output one bit (bit = 0,1) */
{
	static unsigned int  buffer = 0, mask = 128;
	
	if (bit) buffer |= mask;
	if ((mask >>= 1) == 0) {
		if (PutOutputByte((unsigned char)buffer)) {
			Error(ERR_WRITE);
		}
		buffer = 0;  mask = 128;  bitcon_codesize++;
	}
}

static	void FlushBitBuffer(void)  /* Send remaining bits */
{
	int  i;
	
	for (i = 0; i < 7; i++) PutBit(0);
}

static int 
GetBit(void)		/* Get one bit (0 or 1) */
{
	unsigned char inpChar;

	static unsigned int  buffer, mask = 0;
	
	if ((mask >>= 1) == 0) {

		/* Original code didn't check for error on return from
		 * getc(), which was called on the line below (and is
		 * now replaced by GetInputByte).  When I added a check
		 * for EOF it broke the program.
		 */
		(void)GetInputByte(&inpChar);
		buffer = (unsigned int)inpChar;
		mask = 128;
	}
	return ((buffer & mask) != 0);
}

/********** LZSS with multiple binary trees **********/

#define N		 4096	/* size of ring buffer */

#define F		   60	/* upper limit for match_length */

#define THRESHOLD	2   /* encode string into position and length
						 * if match_length is greater than this
						 */

#define NIL			N	/* index for root of binary search trees */


/* Ring buffer of size N, with an extra F - 1 bytes to facilitate
 * string comparison.
 */
static unsigned char text_buf[N + F - 1];


/* Pos and len of longest match.  These are set by the 
 * InsertNode() procedure. 
 */
static int match_position; 
static int match_length;  


/* left & right children & parents -- These constitute binary search trees. */
static int lson[N + 1]; 
static int rson[N + 257]; 
static int dad[N + 1];  


static void
InitTree(void)  /* Initialize trees */
{
	int  i;

	/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	 * left children of node i.  These nodes need not be initialized.
	 * Also, dad[i] is the parent of node i.  These are initialized to
	 * NIL (= N), which stands for 'not used.'
	 *
	 * For i = 0 to 255, rson[N + i + 1] is the root of the tree
	 * for strings that begin with character i.  These are initialized
	 * to NIL.  Note there are 256 trees. 
	 */


	for (i = N + 1; i <= N + 256; i++) {
		rson[i] = NIL;	/* root */
	}

	for (i = 0; i < N; i++) {
		dad[i] = NIL;	/* node */
	}
}

/* Inserts string of length F, text_buf[r..r+F-1], into one of the
 * trees (text_buf[r]'th tree) and returns the longest-match position
 * and length via the global variables match_position and match_length.
 *
 * If match_length = F, then removes the old node in favor of the new
 * one, because the old one will be deleted sooner.
 * Note r plays double role, as tree node and position in buffer.
 */
static void
InsertNode(int r)
{
	int  i, p, cmp, temp;
	unsigned char *key;

	cmp = 1;  
	key = &text_buf[r];  
	p = N + 1 + key[0];
	rson[r] = lson[r] = NIL;  
	match_length = 0;

	for (;;) {

		if (cmp >= 0) {

			if (rson[p] != NIL) {
				p = rson[p];
			} else {  
				rson[p] = r;  
				dad[r] = p;  
				return;  
			}

		} else {

			if (lson[p] != NIL) {
				p = lson[p];
			} else {  
				lson[p] = r;  
				dad[r] = p;  
				return;  
			}

		}

		for (i = 1; i < F; i++) {
			if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
		}

		if (i > THRESHOLD) {

			if (i > match_length) {
				match_position = (r - p) & (N - 1);
				if ((match_length = i) >= F) {
					break;
				}
			} else if (i == match_length) {
				if ((temp = (r - p) & (N - 1)) < match_position) {
					match_position = temp;
				}
			}
		}
	}

	dad[r] = dad[p];  
	lson[r] = lson[p];  
	rson[r] = rson[p];

	dad[lson[p]] = r;  
	dad[rson[p]] = r;

	if (rson[dad[p]] == p) {
		rson[dad[p]] = r;
	} else {
		lson[dad[p]] = r;
	}

	dad[p] = NIL;  /* remove p */
}

static	void DeleteNode(int p)  /* Delete node p from tree */
{
	int  q;
	
	if (dad[p] == NIL) return;  /* not in tree */
	if (rson[p] == NIL) q = lson[p];
	else if (lson[p] == NIL) q = rson[p];
	else {
		q = lson[p];
		if (rson[q] != NIL) {
			do {  q = rson[q];  } while (rson[q] != NIL);
			rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
			lson[q] = lson[p];  dad[lson[p]] = q;
		}
		rson[q] = rson[p];  dad[rson[p]] = q;
	}
	dad[q] = dad[p];
	if (rson[dad[p]] == p) rson[dad[p]] = q;
	else                   lson[dad[p]] = q;
	dad[p] = NIL;
}

/********** Arithmetic Compression **********/

/* If you are not familiar with arithmetic compression, you should read
 * I. E. Witten, R. M. Neal, and J. G. Cleary,
 * Communications of the ACM, Vol. 30, pp. 520-540 (1987),
 * from which much have been borrowed.  
 */

#define M   15

/* Q1 (= 2 to the M) must be sufficiently large, but not so
 * large as the unsigned long 4 * Q1 * (Q1 - 1) overflows.  
 */

#define Q1  	(1UL << M)
#define Q2  	(2 * Q1)
#define Q3  	(3 * Q1)
#define Q4  	(4 * Q1)
#define MAX_CUM (Q1 - 1)

/* character code = 0, 1, ..., N_CHAR - 1 */
#define N_CHAR  (256 - THRESHOLD + F)

unsigned long int low = 0; 
unsigned long int high = Q4; 
unsigned long int value = 0;

int  shifts = 0;  /* counts for magnifying low and high around Q2 */

int char_to_sym[N_CHAR]; 
int sym_to_char[N_CHAR + 1];

unsigned int	sym_freq[N_CHAR + 1];  /* frequency for symbols */
unsigned int	sym_cum[N_CHAR + 1];   /* cumulative freq for symbols */
unsigned int	position_cum[N + 1];   /* cumulative freq for positions */

static void
StartModel(void)  /* Initialize model */
{
	int ch, sym, i;
	
	sym_cum[N_CHAR] = 0;
	for (sym = N_CHAR; sym >= 1; sym--) {
		ch = sym - 1;
		char_to_sym[ch] = sym;  sym_to_char[sym] = ch;
		sym_freq[sym] = 1;
		sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
	}
	sym_freq[0] = 0;  /* sentinel (!= sym_freq[1]) */
	position_cum[N] = 0;

	/* empirical distribution function (quite tentative) */
	/* Please devise a better mechanism! */
	for (i = N; i >= 1; i--) {
		position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);
	}
}

static void 
UpdateModel(int sym)
{
	int i, c, ch_i, ch_sym;
	
	if (sym_cum[0] >= MAX_CUM) {
		c = 0;

		for (i = N_CHAR; i > 0; i--) {
			sym_cum[i] = c;
			c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
		}

		sym_cum[0] = c;
	}

	for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--) 
		;

	if (i < sym) {
		ch_i = sym_to_char[i];    ch_sym = sym_to_char[sym];
		sym_to_char[i] = ch_sym;  sym_to_char[sym] = ch_i;
		char_to_sym[ch_i] = sym;  char_to_sym[ch_sym] = i;
	}

	sym_freq[i]++;

	while (--i >= 0) sym_cum[i]++;
}

static void 
Output(int bit)  /* Output 1 bit, followed by its complements */
{
	PutBit(bit);

	for ( ; shifts > 0; shifts--) {
		PutBit(! bit);
	}
}

static	void EncodeChar(int ch)
{
	int sym;
	unsigned long int range;

	sym = char_to_sym[ch];

	range = high - low;
	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
	low +=       (range * sym_cum[sym    ]) / sym_cum[0];

	for (;;) {
		if (high <= Q2) {
			Output(0);
		} else if (low >= Q2) {
			Output(1);  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			shifts++;  low -= Q1;  high -= Q1;
		} else {
			break;
		}
		low += low;  high += high;
	}
	UpdateModel(sym);
}

static void 
EncodePosition(int position)
{
	unsigned long int  range;

	range = high - low;
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) {
		if (high <= Q2) Output(0);
		else if (low >= Q2) {
			Output(1);  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			shifts++;  low -= Q1;  high -= Q1;
		} else break;
		low += low;  high += high;
	}
}

static void 
EncodeEnd(void)
{
	shifts++;

	if (low < Q1) {
		Output(0);  
	} else {
		Output(1);
	}

	FlushBitBuffer();  /* flush bits remaining in buffer */
}

/* 1      if x >= sym_cum[1],
 * N_CHAR if sym_cum[N_CHAR] > x,
 * i such that sym_cum[i - 1] > x >= sym_cum[i] otherwise 
 */
static int 
BinarySearchSym(unsigned int x)
{
	int i, j, k;
	
	i = 1;  j = N_CHAR;
	while (i < j) {
		k = (i + j) / 2;
		if (sym_cum[k] > x) i = k + 1;  else j = k;
	}
	return i;
}

/* 0 if x >= position_cum[1],
 * N - 1 if position_cum[N] > x,
 * i such that position_cum[i] > x >= position_cum[i + 1] otherwise 
 */
static int 
BinarySearchPos(unsigned int x)
{
	int i, j, k;
	
	i = 1;
	j = N;

	while (i < j) {

		k = (i + j) / 2;

		if (position_cum[k] > x) {
			i = k + 1;  
		} else {
			j = k;
		}

	}
	return i - 1;
}

static void
StartDecode(void)
{
	int i;

	for (i = 0; i < M + 2; i++) {
		value = 2 * value + GetBit();
	}
}

static int
DecodeChar(void)
{
	int	 sym, ch;
	unsigned long int range;
	
	range = high - low;

	sym = BinarySearchSym((unsigned int)
		(((value - low + 1) * sym_cum[0] - 1) / range));

	high = low + (range * sym_cum[sym - 1]) / sym_cum[0];

	low +=       (range * sym_cum[sym    ]) / sym_cum[0];

	for (;;) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) {
			break;
		}

		low += low;  high += high;

		value = 2 * value + GetBit();
	}
	ch = sym_to_char[sym];
	UpdateModel(sym);
	return ch;
}

static int 
DecodePosition(void)
{
	int position;
	unsigned long int range;
	
	range = high - low;
	position = BinarySearchPos((unsigned int)
		(((value - low + 1) * position_cum[0] - 1) / range));
	high = low + (range * position_cum[position    ]) / position_cum[0];
	low +=       (range * position_cum[position + 1]) / position_cum[0];
	for ( ; ; ) {
		if (low >= Q2) {
			value -= Q2;  low -= Q2;  high -= Q2;
		} else if (low >= Q1 && high <= Q3) {
			value -= Q1;  low -= Q1;  high -= Q1;
		} else if (high > Q2) break;
		low += low;  high += high;
		value = 2 * value + GetBit();
	}
	return position;
}

/* Read doubleword from the input stream,  big-endian style. */
void
GetInput32(unsigned long *doubleword)
{
	unsigned char inpChar;

	*doubleword = 0;

	if (GetInputByte( &inpChar )) Error(ERR_READ);
	*doubleword |= (unsigned long)inpChar << 24;
	if (GetInputByte( &inpChar )) Error(ERR_READ);
	*doubleword |= (unsigned long)inpChar << 16;
	if (GetInputByte( &inpChar )) Error(ERR_READ);
	*doubleword |= (unsigned long)inpChar << 8;
	if (GetInputByte( &inpChar )) Error(ERR_READ);
	*doubleword |= (unsigned long)inpChar;
}

/* Write doubleword to the output stream, big-endian style.  */
void
PutOutput32(unsigned long doubleword)
{
	if (PutOutputByte((unsigned char)((doubleword & 0xff000000)>>24))) {
		Error(ERR_WRITE);
	}
	if (PutOutputByte((unsigned char)((doubleword & 0xff0000  )>>16))) {
		Error(ERR_WRITE);
	}
	if (PutOutputByte((unsigned char)((doubleword & 0xff00    )>> 8))) {
		Error(ERR_WRITE);
	}
	if (PutOutputByte((unsigned char)( doubleword & 0xff      )     )) {
		Error(ERR_WRITE);
	}
}

/* Perform compression.
 * inputdata is the array of uncompressed data.
 * inputlen is the length in bytes.
 * outputdata is a ptr to the array of compressed data.
 * outputlen is the length in bytes.
 *
 * The return value is zero if successful.
 *
 * The calling routine must call free() on the pointer returned in
 * outputdata.
 */
int
Concentrate(char *inputdata, long inputlen, char **outputdata, long *outputlen)
{
	int  i, c, len, r, s, last_match_length;
	unsigned char inpChar;
	int setjmprtn;
	
	/* Set up the err-return vector */
	if (setjmprtn = setjmp(errExit)) return setjmprtn;
	
	/* Allocate IO buffers etc */
	if ( InitializeIO(inputdata, inputlen) ) Error(ERR_INITIO);

	PutOutput32((unsigned long)BITCON_MAGIC);	/* Write magic number */
	PutOutput32((unsigned long)inputlen);		/* Write text length */

	StartModel();  
	InitTree();

	s = 0;  
	r = N - F;

	for (i = s; i < r; i++) {
		text_buf[i] = ' ';
	}

	for (len = 0; len < F && (GetInputByte(&inpChar) == 0); len++) {
		c = (int)inpChar & 0xff;
		text_buf[r + len] = c;
	}

	bitcon_textsize = len;

	for (i = 1; i <= F; i++) {
		InsertNode(r - i);
	}

	InsertNode(r);

	do {
		if (match_length > len) match_length = len;

		if (match_length <= THRESHOLD) {
			match_length = 1;  
			EncodeChar(text_buf[r]);
		} else {
			EncodeChar(255 - THRESHOLD + match_length);
			EncodePosition(match_position - 1);
		}

		last_match_length = match_length;

		for (i = 0; i < last_match_length&&(GetInputByte(&inpChar)==0); i++) {
			c = (int)inpChar & 0xff;
			DeleteNode(s);  
			text_buf[s] = c;
			if (s < F - 1) text_buf[s + N] = c;
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			InsertNode(r);
		}

		bitcon_textsize += i;

		while (i++ < last_match_length) {
			DeleteNode(s);
			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			if (--len) InsertNode(r);
		}

	} while (len > 0);

	EncodeEnd();

	*outputdata = outputData;			/* Save ptr to data */
	*outputlen = outputDataAccumulated;	/* Save length of data */
	return 0;
}

/* Perform decompression.
 * inputdata is the array of compressed data.
 * inputlen is the length in bytes.
 * outputdata is a ptr to the array of uncompressed data.
 * outputlen is the length in bytes.
 *
 * The return value is zero if successful.
 *
 * The calling routine must call free() on the pointer returned in
 * outputdata.
 */
int
Dilute(char *inputdata, long inputlen, char **outputdata, long *outputlen)
{
	int  i, j, k, r, c;
	unsigned long int count;
	int setjmprtn;
	unsigned long magic;
	
	/* Set up the err-return vector */
	if (setjmprtn = setjmp(errExit)) return setjmprtn;
	
	if ( InitializeIO(inputdata, inputlen) ) Error(ERR_INITIO);

	GetInput32(&magic);		/* Get magic number from input file */
	if (magic != BITCON_MAGIC) Error(ERR_BADJUJU);

	GetInput32(&bitcon_textsize);	/* Get size of uncompressed text */
	if (bitcon_textsize == 0) Error(ERR_BADTEXTSIZE);

	StartDecode();  
	StartModel();

	for (i = 0; i < N - F; i++) {
		text_buf[i] = ' ';
	}

	r = N - F;
	for (count = 0; count < bitcon_textsize; ) {
		c = DecodeChar();
		if (c < 256) {
			if (PutOutputByte((unsigned char)c)) {
				Error(ERR_WRITE);
			}
			text_buf[r++] = c;
			r &= (N - 1);  
			count++;
		} else {
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESHOLD;
			for (k = 0; k < j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				if (PutOutputByte((unsigned char)c)) {
					Error(ERR_WRITE);
				}
				text_buf[r++] = c;
				r &= (N - 1);  
				count++;
			}
		}
	}
	*outputdata = outputData;			/* Save ptr to data */
	*outputlen = outputDataAccumulated;	/* Save length of data */
	return 0;
}

