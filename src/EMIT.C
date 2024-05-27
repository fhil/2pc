#include "glob.h"

extern char lookbuf[];
extern char lookbuf2[];
extern char combuf[];

int ItemIndex;
char *Item[100];

char DataTable[50000];
char *NextData;


void fast_malloc_init()
{
        NextData = DataTable;
}

void *fast_malloc(int size)
{
    char *Data = NextData;

    NextData += size;

    return Data;
}

char *fast_stralloc(char *str)
{
    char *p_str = NULL;
    int len;
    len = (int)strlen(str) + 1;
    if (len > 0)
    {
        p_str = fast_malloc(len);
        if (p_str)
        {
            strcpy_s(p_str, len, str);
        }
    }
    return p_str;
}

char *stralloc(char *str)
{
    char *p_str = NULL;
    int len;
    len = (int)strlen(str) + 1;
    if (len > 0)
    {
        p_str = malloc(len);
        if (p_str)
        {
            strcpy_s(p_str, len, str);
        }
    }
    return p_str;
}

void out()
{
    int i = 0;

    fast_malloc_init();

    for (i = 0; i < ItemIndex; i++)
    {
        fprintf(output,"%s",Item[i]);

        /* free(Item[i]); */
        Item[i] = NULL;
    }
    ItemIndex = 0;
}


void addstr(char *str)
{
    Item[ItemIndex++] = fast_stralloc(str);
}

void expitem(int Index, char *str)
{
    char *temp;
    int len;
    len = (int)strlen(Item[Index]) + (int)strlen(str) + 1;

    temp = fast_malloc(len);
    strcpy_s(temp, len, Item[Index]);
    strcat_s(temp, len, str);

    /* free(Item[Index]); */
    Item[Index] = temp;
}

void chgitem(int Index, char *str)
{
    /* if (Item[Index]) free(Item[Index]); */
    Item[Index] = fast_stralloc(str);
}

void Genstrcpy(char *eb, rsize_t eb_size, char *Z80, char *PC)
{
    if (GenMSX)
    {
        strcpy_s(eb, eb_size, Z80);
    }
    else
    {
        strcpy_s(eb, eb_size, PC);
    }
}

void emit(t)
int t;
{
    int i;
    char eb[500];

    addstr(combuf);
    combuf[0] = '\0';
    i=0;

    /*
    while ( (i < strlen(lookbuf)) &&
          (isspace(lookbuf[i]) || (lookbuf[i]=='\n')) )
    {
    i++;
    }
    if (isspace(lookbuf[0])) strcpy(&lookbuf[0],&lookbuf[i]);
    */

    switch (t)
    {
    case '+': case '-': case '*': case '/':  case ':':
    case ',': case '(': case ')': case '\'': case '.':
    case '[': case ']': case ' ': case 'C':
        sprintf_s(eb, sizeof(eb), "%c", t); break;

    case _LD:   Genstrcpy(eb, sizeof(eb), "LD", "MOV");  break;
    case _PUSH: Genstrcpy(eb, sizeof(eb), "PUSH", "PUSH"); break;
    case _POP:  Genstrcpy(eb, sizeof(eb), "POP", "POP");  break;
    case _EX:   Genstrcpy(eb, sizeof(eb), "EX", "XCHG"); break;
    case _EXX:  Genstrcpy(eb, sizeof(eb), "EXX", "CALL __EXX");  break;

    case _LDIR: Genstrcpy(eb, sizeof(eb), "LDIR", "CALL __LDIR"); break;
    case _LDI:  Genstrcpy(eb, sizeof(eb), "LDI", "CALL __LDI");  break;
    case _LDDR: Genstrcpy(eb, sizeof(eb), "LDDR", "CALL __LDDR"); break;
    case _LDD:  Genstrcpy(eb, sizeof(eb), "LDD", "CALL __LDD");  break;

    case _CPIR: Genstrcpy(eb, sizeof(eb), "CPIR", "CALL __CPIR"); break;
    case _CPI:  Genstrcpy(eb, sizeof(eb), "CPI", "CALL __CPI");  break;
    case _CPDR: Genstrcpy(eb, sizeof(eb), "CPDR", "CALL __CPDR"); break;
    case _CPD:  Genstrcpy(eb, sizeof(eb), "CPD", "CALL __CPD");  break;

    case _ADD:  Genstrcpy(eb, sizeof(eb), "ADD", "ADD");     break;
    case _ADC:  Genstrcpy(eb, sizeof(eb), "ADC", "ADC");     break;
    case _SBC:  Genstrcpy(eb, sizeof(eb), "SBC", "SBB");     break;
    case _SUB:  Genstrcpy(eb, sizeof(eb), "SUB", "SUB al,"); break;
    case _AND:  Genstrcpy(eb, sizeof(eb), "AND", "AND al,"); break;
    case _OR:   Genstrcpy(eb, sizeof(eb), "OR", "OR al,");  break;
    case _XOR:  Genstrcpy(eb, sizeof(eb), "XOR", "XOR al,"); break;
    case _CP:   Genstrcpy(eb, sizeof(eb), "CP", "CMP al,"); break;
    case _INC:  Genstrcpy(eb, sizeof(eb), "INC", "INC");     break;
    case _DEC:  Genstrcpy(eb, sizeof(eb), "DEC", "DEC");     break;

    case _DAA:  Genstrcpy(eb, sizeof(eb), "DAA", "; ---- DAA or DAS");  break;
    case _CPL:  Genstrcpy(eb, sizeof(eb), "CPL", "NOT al"); break;
    case _NEG:  Genstrcpy(eb, sizeof(eb), "NEG", "NEG al"); break;
    case _CCF:  Genstrcpy(eb, sizeof(eb), "CCF", "CMC");    break;
    case _SCF:  Genstrcpy(eb, sizeof(eb), "SCF", "STC");    break;
    case _NOP:  Genstrcpy(eb, sizeof(eb), "NOP", "NOP");    break;
    case _HALT: Genstrcpy(eb, sizeof(eb), "HALT", "HLT");   break;
    case _DI:   Genstrcpy(eb, sizeof(eb), "DI", "CLI");    break;
    case _EI:   Genstrcpy(eb, sizeof(eb), "EI", "STI");    break;
    case _IM:   Genstrcpy(eb, sizeof(eb), "IM", "; ---- IM"); break;

    case _RLCA: Genstrcpy(eb, sizeof(eb), "RLCA", "ROL al,1"); break;
    case _RLA:  Genstrcpy(eb, sizeof(eb), "RLA", "RCL al,1"); break;
    case _RRCA: Genstrcpy(eb, sizeof(eb), "RRCA", "ROR al,1"); break;
    case _RRA:  Genstrcpy(eb, sizeof(eb), "RRA", "RCR al,1"); break;

    case _RLC:  Genstrcpy(eb, sizeof(eb), "RLC", "ROL"); break;
    case _RL:   Genstrcpy(eb, sizeof(eb), "RL", "RCL"); break;
    case _RRC:  Genstrcpy(eb, sizeof(eb), "RRC", "ROR"); break;
    case _RR:   Genstrcpy(eb, sizeof(eb), "RR", "RCR"); break;
    case _SLA:  Genstrcpy(eb, sizeof(eb), "SLA", "SAL"); break;
    case _SRA:  Genstrcpy(eb, sizeof(eb), "SRA", "SAR"); break;
    case _SRL:  Genstrcpy(eb, sizeof(eb), "SRL", "SHR"); break;
    case _RLD:  Genstrcpy(eb, sizeof(eb), "RLD", "; ---- RLD"); break;
    case _RRD:  Genstrcpy(eb, sizeof(eb), "RRD", "; ---- RRD"); break;

    case _BIT:  Genstrcpy(eb, sizeof(eb), "BIT", "TEST"); break;
    case _SET:  Genstrcpy(eb, sizeof(eb), "SET", "OR");   break;
    case _RES:  Genstrcpy(eb, sizeof(eb), "RES", "AND");  break;

    case _JP:   Genstrcpy(eb, sizeof(eb), "JP", "J");   break;
    case _JR:   Genstrcpy(eb, sizeof(eb), "JR", "J");   break;
    case _DJNZ: Genstrcpy(eb, sizeof(eb), "DJNZ", "DEC ch\n\tJNZ"); break;

    case _NZ:   Genstrcpy(eb, sizeof(eb), "NZ", "NZ"); break;
    case _Z:    Genstrcpy(eb, sizeof(eb), "Z", "Z");  break;
    case _NC:   Genstrcpy(eb, sizeof(eb), "NC", "NC"); break;
    case _PO:   Genstrcpy(eb, sizeof(eb), "PO", "PO"); break;
    case _PE:   Genstrcpy(eb, sizeof(eb), "PE", "PE"); break;
    case _P:    Genstrcpy(eb, sizeof(eb), "P", "P");  break;
    case _M:    Genstrcpy(eb, sizeof(eb), "M", "M");  break;

    case _CALL: Genstrcpy(eb, sizeof(eb), "CALL", "CALL"); break;
    case _RET:  Genstrcpy(eb, sizeof(eb), "RET", "RET");  break;
    case _RETI: Genstrcpy(eb, sizeof(eb), "RETI", "; ---- RETI"); break;
    case _RETN: Genstrcpy(eb, sizeof(eb), "RETN", "; ---- RETN"); break;

    case _IN:   Genstrcpy(eb, sizeof(eb), "IN", "; ---- IN");   break;
    case _OUT:  Genstrcpy(eb, sizeof(eb), "OUT", "; ---- OUT");  break;

    case _INI:  strcpy_s(eb, sizeof(eb), "INI");  break;
    case _INIR: strcpy_s(eb, sizeof(eb), "INIR"); break;
    case _IND:  strcpy_s(eb, sizeof(eb), "IND");  break;
    case _INDR: strcpy_s(eb, sizeof(eb), "INDR"); break;
    case _OUTI: strcpy_s(eb, sizeof(eb), "OUTI"); break;
    case _OTIR: strcpy_s(eb, sizeof(eb), "OTIR"); break;
    case _OUTD: strcpy_s(eb, sizeof(eb), "OUTD"); break;
    case _OTDR: strcpy_s(eb, sizeof(eb), "OTDR"); break;

    case _DB:   strcpy_s(eb, sizeof(eb), "DB");   break;
    case _DC:   strcpy_s(eb, sizeof(eb), "DC");   break;
    case _DW:   strcpy_s(eb, sizeof(eb), "DW");   break;
    case _DS:   strcpy_s(eb, sizeof(eb), "DS");   break;
    case _ORG:  strcpy_s(eb, sizeof(eb), "ORG");  break;
    case _ASEG: strcpy_s(eb, sizeof(eb), "ASEG"); break;
    case _Z80:  strcpy_s(eb, sizeof(eb), "Z80");  break;
    case _EQU:  strcpy_s(eb, sizeof(eb), "EQU");  break;
    case _END:  strcpy_s(eb, sizeof(eb), "END");  break;

    case _A:    Genstrcpy(eb, sizeof(eb), "A", "al"); break;
    case _B:    Genstrcpy(eb, sizeof(eb), "B", "ch"); break;
    case _C:    Genstrcpy(eb, sizeof(eb), "C", "cl"); break;
    case _D:    Genstrcpy(eb, sizeof(eb), "D", "dh"); break;
    case _E:    Genstrcpy(eb, sizeof(eb), "E", "dl"); break;
    case _H:    Genstrcpy(eb, sizeof(eb), "H", "bh"); break;
    case _L:    Genstrcpy(eb, sizeof(eb), "L", "bl"); break;
    case _I:    Genstrcpy(eb, sizeof(eb), "I", "I");  break;
    case _R:    Genstrcpy(eb, sizeof(eb), "R", "R");  break;

    case _AF:   Genstrcpy(eb, sizeof(eb), "AF", "ax"); break;
    case _AF_:  Genstrcpy(eb, sizeof(eb), "AF'", "[AF_]"); break;
    case _BC:   Genstrcpy(eb, sizeof(eb), "BC", "cx"); break;
    case _DE:   Genstrcpy(eb, sizeof(eb), "DE", "dx"); break;
    case _HL:   Genstrcpy(eb, sizeof(eb), "HL", "bx"); break;
    case _SP:   Genstrcpy(eb, sizeof(eb), "SP", "sp"); break;
    case _IX:   Genstrcpy(eb, sizeof(eb), "IX", "si"); break;
    case _IY:   Genstrcpy(eb, sizeof(eb), "IY", "di"); break;

    case _PUBLIC:   strcpy_s(eb, sizeof(eb), "PUBLIC"); break;
    case _EXTRN:    strcpy_s(eb, sizeof(eb), "EXTRN");  break;
    case _GLOBAL:	strcpy_s(eb, sizeof(eb), "GLOBAL"); break;
    case _GLOBALS:  strcpy_s(eb, sizeof(eb), "GLOBALS"); break;
    case _IFDEF:    strcpy_s(eb, sizeof(eb), "IFDEF");  break;
    case _ENDIF:	strcpy_s(eb, sizeof(eb), "ENDIF");  break;
    case _ELSE:	    strcpy_s(eb, sizeof(eb), "ELSE");   break;
    case _IFNDEF:	strcpy_s(eb, sizeof(eb), "IFNDEF"); break;
    case _INCLUDE:	strcpy_s(eb, sizeof(eb), "INCLUDE"); break;
    case _OFF_HL:   strcpy_s(eb, sizeof(eb), "OFF_HL"); break;

    case ID:		strcpy_s(eb, sizeof(eb), lookbuf); break;
    case NUM:		strcpy_s(eb, sizeof(eb), lookbuf); break;
    case STRING:	strcpy_s(eb, sizeof(eb), lookbuf); break;
    case COMMENT:	strcpy_s(eb, sizeof(eb), ""); break;
    default:        sprintf_s(eb, sizeof(eb), "\ttoken %d\n", t);
    }
    addstr(eb);
    return;
}
