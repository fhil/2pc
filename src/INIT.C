#include "glob.h"

extern int lastentry;

struct entry keywords[]= {
        "digit sequence", DIGITS,
        "identifier", ID,

        "AF",_AF,
        "AF'",_AF_,
        "BC",_BC,
        "DE",_DE,
        "HL",_HL,
        "SP",_SP,
        "IX",_IX,
        "IY",_IY,

        "A",_A,
        "B",_B,
        "C",_C,
        "D",_D,
        "E",_E,
        "H",_H,
        "L",_L,
        "I",_I,
        "R",_R,

        "LD",  _LD,
        "PUSH",_PUSH,
        "POP", _POP,
        "EXX", _EXX,
        "EX",  _EX,

        "LDIR",_LDIR,
        "LDI", _LDI,
        "LDDR",_LDDR,
        "LDD", _LDD,
        "CPIR",_CPIR,
        "CPI", _CPI,
        "CPDR",_CPDR,
        "CPD", _CPD,

        "ADD",_ADD,
        "ADC",_ADC,
        "SUB",_SUB,
        "SBC",_SBC,
        "AND",_AND,
        "OR", _OR,
        "XOR",_XOR,
        "CP", _CP,
        "INC",_INC,
        "DEC",_DEC,

        "DAA", _DAA,
        "CPL", _CPL,
        "NEG", _NEG,
        "CCF", _CCF,
        "SCF", _SCF,
        "NOP", _NOP,
        "HALT",_HALT,
        "DI",  _DI,
        "EI",  _EI,
        "IM",  _IM,

        "RLCA",_RLCA,
        "RLA", _RLA,
        "RRCA",_RRCA,
        "RRA", _RRA,

        "RLC", _RLC,
        "RL",  _RL,
        "RRC", _RRC,
        "RR",  _RR,
        "SLA", _SLA,
        "SRA", _SRA,
        "SRL", _SRL,

        "RLD", _RLD,
        "RRD", _RRD,

        "BIT", _BIT,
        "SET", _SET,
        "RES", _RES,

        "JP",  _JP,
        "JR",  _JR,
        "DJNZ",_DJNZ,

        "CALL",_CALL,
        "RET", _RET,

        "RETI",_RETI,
        "RETN",_RETN,

        "IN",   _IN,
        "OUT",  _OUT,
        "INI",  _INI,
        "INIR", _INIR,
        "IND",  _IND,
        "INDR", _INDR,
        "OUTI", _OUTI,
        "OTIR", _OTIR,
        "OUTD", _OUTD,
        "OTDR", _OTDR,

        "DB",  _DB,
        "DC",  _DC,
        "DW",  _DW,
        "DS",  _DS,
        "Z80", _Z80,
        "ASEG",_ASEG,
        "ORG", _ORG,
        "EQU", _EQU,
        "END", _END,
        "IFDEF", _IFDEF,
        "IFNDEF",_IFNDEF,
        "ENDIF", _ENDIF,
        "ELSE",  _ELSE,
        "INCLUDE",_INCLUDE,
        "PUBLIC", _PUBLIC,
        "EXTRN", _EXTRN,
        "GLOBAL", _GLOBAL,
        "OFF_HL", _OFF_HL,
        "GLOBALS", _GLOBALS,


        0,0
};

void init()		/* loads keywords in to symtable */
{
	struct entry *p;
	int i;

	for (p = keywords; p -> token; p++) {
		insert(SearchString(p->lexptr),p -> lexptr, p -> token);
	}

	for (i = 0; i < SYMMAX; i++) {
		CountTab[i] = 0;
	}

	for (ItemIndex = 0; ItemIndex < 50; ItemIndex++) {
		Item[ItemIndex]=NULL;
	}

	ItemIndex=0;
}
