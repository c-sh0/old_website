/****************************************************************************

    MIMECODE - encode/decode binary data using MIME's base64 method

    Definition: ``radix encoding'' is the process of encoding data
    by treating the input data as a number or sequence of numbers
    in a particular base. The most common example is base-16 
(hexadecimal)
    encoding, although other bases are possible.

    This program encodes and decodes data using the base 64 encoding
    used by MIME. Output is broken into lines every 72 characters.
    Decoding ignores control characters.  Base 64 encoding adds 33% to
    the size of the input file.

    Usage: mimecode <options>
    Description: reads from stdin and writes encoded/decoded data to 
stdout.
    Options: -e  Encode
             -d  Decode

****************************************************************************/


#include <stdio.h>

/* This digit string is used by MIME's base-64 encoding */
/* MIME also deliberately ignores `=' characters */
#define BASE64DIGITS \
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"


static unsigned long bitStorage = 0;
static int numBits = 0;

/* Masks for 0-8 bits */
static int mask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

/****************************************************************************
    ReadBits: reads a fixed number of bits from stdin

    If insufficient bits are available, the remaining bits are
    returned left-justified in the desired width.
*/

unsigned int ReadBits(int n, int *pBitsRead)
{
    static int eof = 0;
    unsigned long scratch;
    while ((numBits < n) && (!eof)) {
        int c = getchar();
        if (c == EOF) eof = 1;
        else {
            bitStorage <<= 8;
            bitStorage |= (c & 0xff);
            numBits += 8;
        }
    }
    if (numBits < n) {
        scratch = bitStorage << (n - numBits);
        *pBitsRead = numBits;
        numBits = 0;
    } else {
        scratch = bitStorage >> (numBits - n);
        *pBitsRead = n;
        numBits -= n;
    } return scratch & mask[n];
}

/****************************************************************************


    WriteChar: output character to stdout, breaking lines at 72 
characters.
*/
static count = 0;
void WriteChar(char c)
{
    putchar(c);
    count++;
    if (count >= 72) { /* Chop after 72 chars */
        putchar('\n');
        count = 0;
    }
}

/************************************************************************
    PadMimeOutput: pad output for MIME base64 encoding
*/
void PadMimeOutput(void)
{
  while ((count % 4) != 0) {
    putchar('=');
    count++;
  }
}

/************************************************************************
    ReadChar: Get next non-control character from stdin, return EOF at
        end-of-file
*/
int ReadChar(void)
{
    int c;
    do {
        c = getchar();
        if (c==EOF) return c;
    } while ( (((c+1) & 0x7f) < 33) ); /* Skip any control character */ return c;
}

/************************************************************************
    WriteBits: Write bits to stdout

    Note: assumes `bits' is already properly masked.
*/
void WriteBits(unsigned bits, int n)
{
    bitStorage = (bitStorage << n) | bits;
    numBits += n;
    while (numBits > 7) {
        unsigned scratch = bitStorage >> (numBits - 8);
        putchar(scratch & 0xff);
        numBits -= 8;
    }
}

/************************************************************************
    Base64Encode: encode stdin to stdout in base64

    The encoding vector used here is the one used by MIME.
*/
void Base64Encode(void)
{
    int numBits = 6; /* Encode 6 bits at a time */
    int digit;
    const char *digits = BASE64DIGITS;

    digit = ReadBits(numBits,&numBits);
    while (numBits > 0) { /* Encode extra bits at the end */
        WriteChar(digits[digit]);
        digit = ReadBits(numBits,&numBits);
    }    
    PadMimeOutput(); /* Pad to multiple of four characters */
    putchar('\n');
}

/************************************************************************
    Base64Decode: decode stdin to stdout in base64

    The `decode' array specifies the value of each digit character.
    -2 indicates an illegal value, -1 for a value that should be
    ignored.  ReadChar() already ignores control characters.

    Ignores parity.
*/
void Base64Decode(void)
{
    int c, digit;
    int decode[256];

    { /* Build decode table */
        int i;
        const char *digits = BASE64DIGITS;
        for (i=0;i<256;i++) decode[i] = -2; /* Illegal digit */
        for (i=0;i<64;i++) {
            decode[digits[i]] = i;
        decode[digits[i]|0x80] = i; /* Ignore parity when decoding */
    } decode['='] = -1; decode['='|0x80] = -1; /* Ignore '=' for MIME */
    }    

    c = ReadChar();
    while (c != EOF) {
        digit = decode[c & 0x7f];
        if (digit < -1) {
        fprintf(stderr,"Illegal base 64 digit: %c\n",c);
        exit(1);
    } else if (digit >= 0) 
        WriteBits(digit & 0x3f,6);
    c = ReadChar();
    }
}

/************************************************************************
    Usage: print usage message to stderr
*/
void Usage(char * progname)
{
    fprintf(stderr,"Usage: %s <options>\n",progname);
    fprintf(stderr,"Options:  -e   Encode\n");
    fprintf(stderr,"          -d   Decode\n");
}

/************************************************************************
    main: parse arguments, call appropriate encode/decode function
*/
int main(int argc, char **argv)
{
    int encode = 1;

    if (argc < 2) { Usage(argv[0]); exit(1);}
    while (argc > 1) {
        char *p=argv[--argc];
        switch(*p) {
            case '-':
                {
                    switch(*++p) {
                        case 'e': case 'E': encode = 1; break;
                        case 'd': case 'D': encode = 0; break;
                        default:
                        fprintf(stderr,"Unrecognized option: %s\n",p);
                            Usage(argv[0]);
                            exit(1);
                    }
                }
                break;
            default:
                fprintf(stderr,"Unrecognized option: %s\n",p);
                Usage(argv[0]);
                exit(1);
        }
    }

    if (encode) Base64Encode();
    else Base64Decode();
    exit(0);
} 


