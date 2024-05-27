#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#ifdef __MEDIUM__

#else
/*        asdf  asd f asdf */
#endif


#define SYMMAX  2700	/* size of symbol table */
#define BSIZE  256  /* buffer size */
#define NONE	-1
#define EOS	'\0'

#define NUM	  256
#define DIGITS    256
#define HEXDIGITS 256
#define SPACE	  257
#define ID        258
#define DONE      259
#define STRING	  260
#define COMMENT   261

#define _LD	264
#define _PUSH	265
#define _POP	266
#define _EXX	267
#define _EX	268
#define _LDIR	269
#define _LDI	270
#define _LDDR	271
#define _LDD	272
#define _CPIR	273
#define _CPI	274
#define _CPDR	275
#define _CPD	276

#define _ADD	277
#define _ADC	278
#define _SUB	279
#define _SBC	280
#define _AND	281
#define _OR	282
#define _XOR	283
#define _CP	284
#define _INC	285
#define _DEC	286

#define _DAA	287
#define _CPL	288
#define _NEG	289
#define _CCF	290
#define _SCF	291
#define _NOP	292
#define _HALT	293
#define _DI	294
#define _EI	295
#define _IM	296

#define _RLCA	297
#define _RLA	298
#define _RRCA	299
#define _RRA	300

#define _RLC	301
#define _RL	302
#define _RRC	303
#define _RR	304
#define _SLA	305
#define _SRA	306
#define _SRL	307
#define _RLD	308
#define _RRD	309

#define _BIT	310
#define _SET	311
#define _RES	312

#define _JP	313
#define _JR	314
#define _DJNZ	315

#define _NZ	316
#define _Z	317
#define _NC	318
/* #define _C	319 */
#define _PO	320
#define _PE	321
#define _P	322
#define _M	323

#define _CALL	324
#define _RET	325
#define _RETI	326
#define _RETN	327

#define _IN	328
#define _OUT	329
#define _INI	330
#define _INIR	331
#define _IND	332
#define _INDR	333
#define _OUTI	334
#define _OTIR	335
#define _OUTD	336
#define _OTDR	337

#define _DB	600
#define _DS	601
#define _Z80	602
#define _ASEG	603
#define _ORG	604
#define _EQU	605
#define _DW	606
#define _DC	607
#define _END	608

#define _IFDEF	609
#define _IFNDEF 610
#define _ENDIF	611
#define _ELSE	612
#define _INCLUDE 613
#define _PUBLIC 614
#define _EXTRN  615
#define _GLOBAL 616
#define _OFF_HL	617
#define _GLOBALS 618

#define _AF	500
#define _AF_	501

#define _BC	502		/* 0 */
#define _DE	503		/* 1 */
#define _HL	504		/* 2 */
#define _SP	505		/* 3 */

#define _IX	506
#define _IY	507

#define _B	980
#define _C	981
#define _D	982
#define _E	983
#define _H	984
#define _L	985
#define _LA	986	/* 986 = special case, not a register */
#define _A	987

#define _I	900
#define _R	901

#define isalpha(c)     (isupper(c) || islower(c))
#define isupper(c)     ('A' <= (c) && (c) <= 'Z')
#define islower(c)     ('a' <= (c) && (c) <= 'z')
#define isdigit(c)     ('0' <= (c) && (c) <= '9')
#define ishexalpha(c)  ( ('A' <= (c) && (c) <= 'F') || \
			 ('a' <= (c) && (c) <= 'f') )
#define ishex(c)       (isdigit(c) || ishexalpha(c))
#define isblank        (0 <= (c) || (c) <= ' ')

/* only one of the following 2 variables can be true */
extern int GenPC;
extern int GenMSX;

extern int tokenval;		/* value of token attribute */
extern int lineno;
extern int lastentry;

extern int lookahead;
extern FILE *input;
extern FILE *output;
extern FILE *outputcode;

struct entry {			/* form of symbol table entry */
	char *lexptr;
	int token;
};

extern int PosTab[];
extern int CountTab[];
extern struct entry symtable[];	/* symbol table itself */

extern ItemIndex;
extern char *Item[];



/*
        Prototypes
*/



/*
        File: err.c
*/

void error(char *m, int t);



/*
        File: init.c
*/

void init();



/*
        File: lex.c
*/

void digitsequence(char t);
void hexdigitseqeuence(char t);
void identifier(char t);
void string(int t, char limiter);
void comment();
char *skip(char *s);
void printinfo();
int lexan();



/*
        File: symb.c
*/

int looktoken(int t);
int SearchString(char *String);
int Add(int pos);
int insert(int pos, char *String, int token);
int lookup(char *s);



/*
        File: parse.c
*/

int newlabel();
void UnsignedInteger();
void Sign();
void SignedInteger();
void indexedoperand();
int indirectoperand();
void load8();
void load16();
int reg8_or_expr();
void store();
void loadinstruction();
void pushpop_operand(int i);
void pushinstruction();
void popinstruction();
void exchangeinstruction();
void add_adc_sbc_instruction();
void math8_16bit();
void special_control_instruction();
void bitsetresinstruction();
int getcondition();
void jump_jumprelative_instruction();
void callretinstruction();
void in_out_instruction();
void DefineByteWord();
void DotOperation();
void oneword(int token, char *text);
void parse();
void expr();
void expr1();
void term();
void factor();
void match();



/*
        File: emit.c
*/

char *stralloc(char *str);
void out();
void addstr(char *str);
void expitem(int Index, char *str);
void chgitem(int Index, char *str);
void Genstrcpy(char *eb, rsize_t eb_size, char *Z80, char *PC);
void emit(int t);
