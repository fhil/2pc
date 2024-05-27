#include "glob.h"

int lookahead=0,lookahead2=0;
char lookbuf[4*BSIZE],lookbuf2[4*BSIZE];
char combuf[4*BSIZE],combuf2[4*BSIZE];

int labnumber = 0;

int NumberOfIDs = 0;
int LabelInExpression;	/* boolean */
int ExpressionParsed;	/* boolean */

int lexan();

void expitem(int,char *);
void chgitem(int,char *);
void out();
void emit();
void error();
int newlabel();

void parse();
void expr();
void expr1();
void term();
void factor();
void match();
void hexdigitsequence();

extern char lexbuf[];
extern char commentbuf[];

int newlabel()
{
    return (++labnumber);
}

void UnsignedInteger()
{
    if (lookahead == HEXDIGITS)
    {
        match(HEXDIGITS);
    }
    else
    {
        match(DIGITS);
    }
}

void Sign()
{
    if (lookahead == '-')
    {
        match('-');
    }
    else
    if (lookahead=='+' )
    {
        match('+');
    }
}

void SignedInteger()
{
    Sign();
    UnsignedInteger();
}

void indexedoperand()
{
    if (GenPC)
    {
        emit(lookahead);
    }
    if (GenMSX)
    {
        switch (lookahead)
        {
        case _IX: expitem(1,"0ddh,"); break;
        case _IY: expitem(1,"0fdh,"); break;
        }
    }

    match(lookahead);
    switch (lookahead)
    {
    case ')': break;
    case '+': case '-':
    {
        emit(lookahead);

        match(lookahead);
        expr();
        break;
    }
    default:
        error("'+' or '-' expected before index",0);
        break;
    }
}

int indirectoperand() /* ok */
{
    int i = 0;

    if (GenPC)
    {
        emit('['); match('(');
        expitem(ItemIndex-2,"byte ptr ");
    }
    if (GenMSX)
    {
        match('(');
    }

    switch (lookahead)
    {
    case _HL:
    {
        if (GenPC)
        {
            emit(lookahead);
        }

        match(lookahead);
        i = 1;
        break;
    }
    case _IX: case _IY:
        indexedoperand();
        break;
    default:
        error("illegal instruction",0);
        break;
    }

    if (GenPC)
    {
        emit(']');
    }

    match(')');

    return i;
}

void load8()
{
    int reg1;
    int reg2;
    int i;
    char text[10];

    reg1 = lookahead;
    if (GenPC)
    {
        emit(lookahead); match(lookahead);
        emit(','); match(',');
    }
    if (GenMSX)
    {
        match(lookahead);
        match(',');
    }

    switch (reg1)
    {
    case _A:
        switch (lookahead)
        {
        case _I:
        {
            emit(_I);
            if (GenPC)
            {
                expitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                chgitem(ItemIndex-1, " 0edh,57h");
            }
            match(_I);
            return;
        }
        case _R:
        {
            emit(_R);
            if (GenPC)
            {
                expitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                chgitem(ItemIndex-1, " 0edh,5fh");
            }
            match(_R);
            return;
        }
        default:
            break;
        }

        if (lookahead=='(')
        {
            switch (lookahead2)
            {
            case _BC:
            {
                if (GenPC)
                {
                    expitem(0, "xchg cx,bx\n\t");
                    emit('['); match('(');
                    emit(_HL);
                    match(lookahead);
                    emit(']'); match(')');
                    expitem(ItemIndex-1, "\n\txchg cx,bx");
                }
                if (GenMSX)
                {
                    match('(');
                    match(lookahead);
                    match(')');
                    expitem(1, " 0ah");
                }
                return;
            }
            case _DE:
            {
                if (GenPC)
                {
                    expitem(0, "xchg dx,bx\n\t");
                    emit('['); match('(');
                    emit(_HL);
                    match(lookahead);
                    emit(']'); match(')');
                    expitem(ItemIndex-1, "\n\txchg dx,bx");
                }
                if (GenMSX)
                {
                    match('(');
                    match(lookahead);
                    match(')');
                    expitem(1, " 1ah");
                }
                return;
            }
            }
        }
        break;
    case _I:
    {
        if (lookahead==_A)
        {
            emit(lookahead);
            if (GenPC)
            {
                expitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                chgitem(ItemIndex-1, " 0edh,47h");
            }
            match(lookahead);

            return;
        }
        else
        {
            error("illegal instruction", 0);
        }
    }
    case _R:
    {
        if (lookahead==_A)
        {
            emit(lookahead);
            if (GenPC)
            {
                expitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                chgitem(ItemIndex-1, " 0edh,4fh");
            }
            match(lookahead);

            return;
        }
        else
        {
            error("illegal instruction", 0);
        }
    }
    }

    reg2 = lookahead;
    switch (lookahead)
    {
        case _A: case _B: case _C: case _D: case _E:
        case _H: case _L:
        {
            if (GenPC)
            {
                emit(lookahead);
            }
            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), " %xh", 0x40 |
                    ((reg1-_B) << 3) |
                    (reg2-_B));
                expitem(1, text);
            }

            match(lookahead);
            break;
        }
        case '(':
        {
            if (reg1 == _A && (lookahead2!=_HL &&
                lookahead2!=_IX && lookahead2!=_IY))
            {
                if (GenPC)
                {
                    emit('['); match('(');
                    chgitem(ItemIndex-1, "");
                }
                if (GenMSX)
                {
                    expitem(1, " 3ah\n\tDW ");
                    match('(');
                }

                expr();

                if (GenPC)
                {
                    emit(']'); match(')');
                    chgitem(ItemIndex-1, "");
                    expitem(5, "byte ptr ");
                }
                if (GenMSX)
                {
                    match(')');
                }

                return;
            }
            if (GenMSX)
            {
                expitem(1, " ");
            }

            i = indirectoperand(); /* ok */
            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), "%2xh",
                    ((reg1-_B) << 3) | 0x46);
                expitem(1, text);
                if (i == 0)
                {
                    expitem(1, ",");
                }
            }

            break;
        }
        default:
        {
            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), " %02xh,", ((reg1-_B) << 3) | 0x6);
                expitem(1, text);
            }
            expr();
            break;
        }
    }
}

void load16()
{
    int t;
    t = lookahead;
    if (GenPC)
    {
        emit(lookahead); match(lookahead);
        emit(','); match(',');
    }
    if (GenMSX)
    {
        match(lookahead);
        match(',');
    }

    switch (lookahead)
    {
    case '(':
    {
        if (GenPC)
        {
            chgitem(ItemIndex-1, ",word ptr ");
            emit('(');
            chgitem(ItemIndex-1, "");
        }

        match('(');

        expr();

        if (GenPC)
        {
            emit(')');
            chgitem(ItemIndex-1, "");
        }

        match(')');

        if (GenMSX)
        {
            chgitem(1, "DB ");

            switch (t)
            {
            case _IX: expitem(1, "0ddh,"); break;
            case _IY: expitem(1, "0fdh,"); break;
            }

            switch (t)
            {
            case _BC: expitem(1, "0edh,4bh"); break;
            case _DE: expitem(1, "0edh,5bh"); break;
            case _IX: case _IY: case _HL:
                expitem(1, "02ah"); break;
            case _SP: expitem(1, "0edh,7bh"); break;
            }
            expitem(1, "\n\tDW ");
        }
        break;
    }
    case _HL: case _IX: case _IY:
    {
        if (GenPC)
        {
            emit(lookahead);
        }
        if (GenMSX)
        {
            chgitem(1, "DB ");

            switch (lookahead)
            {
            case _IX: expitem(1, "0ddh,"); break;
            case _IY: expitem(1, "0fdh,"); break;
            }

            expitem(1, "0f9h");
        }

        match(lookahead);

        if (t != _SP) error("illegal instruction", 0);

        break;
    }
    default:
    {
        expr();

        if (LabelInExpression)
        {
            if (GenPC)
            {
                expitem(5, "offset ");
            }
        }

        if (GenMSX)
        {
            chgitem(1, "DB ");

            switch (t)
            {
            case _IX: expitem(1, "0ddh,"); break;
            case _IY: expitem(1, "0fdh,"); break;
            }
        }

        switch (t)
        {
        case _BC:
            if (GenMSX) expitem(1, "01h");
            break;
        case _DE:
            if (GenMSX) expitem(1, "11h");
            break;
        case _IX: case _IY: case _HL:
            if (GenMSX) expitem(1, "21h");
            break;
        case _SP:
            if (GenMSX) expitem(1, "31h");
            break;
        }

        if (GenMSX)
        {
            expitem(1, "\n\tDW ");
        }
        break;
    }
    }
}

int reg8_or_expr()
{
    int i;

    ExpressionParsed = 0;
    switch (lookahead)
    {
    case _B: case _C: case _D: case _E:
    case _H: case _L: case _A:
        i = lookahead - _B;
        if (GenPC)
        {
            emit(lookahead);
        }

        match(lookahead);
        break;

    default:
        expr();
        ExpressionParsed = 1;
        i = _LA;
        break;
    }
    return i;
}

void store()
{
    int i, t;
    char text[5];

    if (GenPC)
    {
        emit('[');
    }

    match('(');
    switch (lookahead)
    {
    case _BC:
        if (GenPC)
        {
            expitem(0, "xchg cx,bx\n\t");
            emit(_HL);
        }
        match(lookahead);

        if (GenPC)
        {
            emit(']'); match(')');
            emit(','); match(',');
            emit(_A);  match(_A);
            expitem(ItemIndex-1, "\n\txchg cx,bx");
        }
        if (GenMSX)
        {
            match(')'); match(','); match(_A);
            expitem(ItemIndex-1, " 02h");
        }

        break;

    case _DE:
        if (GenPC)
        {
            expitem(0, "xchg dx,bx\n\t");
            emit(_HL);
        }

        match(lookahead);

        if (GenPC)
        {
            emit(']'); match(')');
            emit(','); match(',');
            emit(_A);  match(_A);
            expitem(ItemIndex-1, "\n\txchg dx,bx");
        }
        if (GenMSX)
        {
            match(')'); match(','); match(_A);
            expitem(ItemIndex-1, " 12h");
        }

        break;

    case _HL:
        if (GenPC)
        {
            emit(_HL);
        }

        match(lookahead);

        if (GenPC)
        {
            emit(']'); match(')');
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match(')'); match(',');
        }
        i = reg8_or_expr();
        if (GenPC)
        {
            expitem(1, " byte ptr");
        }
        if (GenMSX)
        {
            if (i == _LA)
            {
                expitem(1, " 36h,");
            }
            else
            {
                sprintf_s(text, sizeof(text), " %2xh",
                    0x70 | i);
                expitem(ItemIndex-1, text);
            }
        }
        break;

    case _IX: case _IY:

        if (GenMSX)
        {
            expitem(1, " ");
        }
        if (GenPC)
        {
            expitem(1, " byte ptr");
        }

        indexedoperand();

        if (GenPC)
        {
            emit(']'); match(')');
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match(')');
            emit(','); match(',');
            t = ItemIndex-1;
        }

        i = reg8_or_expr();

        if (GenMSX)
        {
            if (i == _LA)
            {
                expitem(1, "36h,");
            }
            else
            {
                chgitem(t, "");
                sprintf_s(text, sizeof(text), "%2xh,",
                    0x70 | i);
                expitem(1, text);
            }
        }
        break;
    default:
        if (GenPC)
        {
            chgitem(3, "");
        }

        expr();

        if (GenPC)
        {
            emit(']'); match(')');
            chgitem(ItemIndex-1, "");
            emit(',');
        }
        if (GenMSX)
        {
            match(')');
        }
        match(',');

        if (GenMSX)
        {
            switch (lookahead)
            {
            case _BC: chgitem(1, "DW 43edh,"); break;
            case _DE: chgitem(1, "DW 53edh,"); break;
            case _HL: expitem(1, " 22h\n\tDW "); break;
            case _SP: chgitem(1, "DW 73edh,"); break;
            case _IX: chgitem(1, "DW 22ddh,"); break;
            case _IY: chgitem(1, "DW 22fdh,"); break;
            }
        }

        switch (lookahead)
        {
        case _BC: case _DE: case _HL:
        case _SP: case _IX: case _IY:
            if (GenPC)
            {
                expitem(1, " word ptr");
                emit(lookahead);
            }
            match(lookahead);
            break;
        case _A:
            if (GenPC)
            {
                expitem(1, " byte ptr");
            }

            if (GenPC)
            {
                emit(lookahead);
            }
            if (GenMSX)
            {
                expitem(1, " 32h\n\tDW ");
            }

            match(lookahead);
            break;
        default:
            emit(lookahead);
            error("illegal instruction", 0);
            break;
        }
        break;
    }
}

void loadinstruction()
{
    if (GenPC)
        emit(_LD);
    if (GenMSX)
        emit(_DB);

    match(_LD);

    switch (lookahead)
    {
    case _A: case _B: case _C: case _D: case _E:
    case _H: case _L: case _I: case _R:
        load8(); /* ok */
        break;

    case _BC: case _DE: case _HL: case _SP:
    case _IX: case _IY:
        load16();
        break;

    case '(':
        store(); /* ok */
        break;

    default:
        error("syntax error in load instruction", 0);
        break;
    }
}

void pushpop_operand(int i)
{
    if (GenMSX)
    {
        switch (lookahead)
        {
        case _IX: expitem(1, "0ddh,"); break;
        case _IY: expitem(1, "0fdh,"); break;
        }
    }

    switch (lookahead)
    {
        /* i = 0, push, i = 1, pop */
    case _AF: if (GenMSX) expitem(1, "0f5h"); break;
    case _BC: if (GenMSX) expitem(1, "0c5h"); break;
    case _DE: if (GenMSX) expitem(1, "0d5h"); break;
    case _HL: case _IX: case _IY:
        if (GenMSX) expitem(1, "0e5h");
        break;

    default:
        error("illegal instruction", 0);
        break;
    }
    if (GenPC)
    {
        emit(lookahead);
    }
    if (GenMSX)
    {
        if (i == 1)
        {
            Item[1][strlen(Item[1])-2] = '1';
        }
    }

    match(lookahead);
}

void pushinstruction()
{
    emit(_PUSH);

    if (GenMSX)
    {
        chgitem(ItemIndex-1, "DB ");
    }

    match(_PUSH);

    if (lookahead == _AF)
    {

        if (GenPC)
        {
            expitem(0, "lahf\n\txchg ah,al\n\t");
        }

        pushpop_operand(0);

        if (GenPC)
        {
            expitem(3, "\n\txchg ah,al");
        }

        return;
    }
    pushpop_operand(0);
}

void popinstruction()
{
    emit(_POP);

    if (GenMSX)
    {
        chgitem(ItemIndex-1, "DB ");
    }

    match(_POP);

    if (lookahead == _AF)
    {
        pushpop_operand(1);
        if (GenPC)
        {
            expitem(3, "\n\txchg ah,al\n\tsahf");
        }
        return;
    }

    pushpop_operand(1);
}

void exchangeinstruction()
{
    emit(_EX);

    if (GenMSX)
    {
        chgitem(ItemIndex-1, "DB ");
    }

    match(_EX);

    switch (lookahead)
    {
    case '(':
        if (GenPC)
        {
            expitem(0, "mov bp,sp\n\t");
            emit('['); match('(');
            emit(_SP); match(_SP);
            chgitem(ItemIndex-1, "bp");
            emit(']'); match(')');
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match('(');
            match(_SP);
            match(')');
            match(',');
            switch (lookahead)
            {
            case _IX: expitem(1, "0ddh,"); break;
            case _IY: expitem(1, "0fdh,"); break;
            }
        }

        switch (lookahead)
        {
        case _HL: case _IX: case _IY:
            if (GenPC)
            {
                emit(lookahead);
            }
            if (GenMSX)
            {
                expitem(1, "0e3h");
            }
            match(lookahead);
            break;
        default:
            error("illegal instruction", 0);
            break;
        }
        break;

    case _DE:
        if (GenPC)
        {
            emit(_DE); match(_DE);
            emit(','); match(',');
            emit(_HL); match(_HL);
        }
        if (GenMSX)
        {
            match(_DE);
            match(',');
            match(_HL);
            expitem(1, "0ebh");
        }

        break;

    case _AF:
        if (GenPC)
        {
            expitem(0, "lahf\n\txchg ah,al\n\t");

            emit(_AF); match(_AF);
            emit(','); match(',');
            emit(_AF_); match(_AF_);

            expitem(ItemIndex-2, "word ptr ");
            expitem(ItemIndex-1, "\n\txchg ah,al\n\tsahf");
        }
        if (GenMSX)
        {
            match(_AF);
            match(',');
            match(_AF_);
            expitem(1, "08h");
        }

        break;

    default: error("illegal instruction", 0);
    }
}

void add_adc_sbc_instruction()
{
    int i, t;
    int op8 = 0;
    int op16 = 0;
    char text[10];

    t = lookahead;

    switch (t)
    {
    case _ADD: op8 = 0x80; op16 = 0x09; break;
    case _ADC: op8 = 0x88; op16 = 0x4a; break;
    case _SBC: op8 = 0x98; op16 = 0x42; break;
    }

    emit(lookahead);

    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);

    switch (lookahead)
    {
    case _A:
        if (GenPC)
        {
            emit(_A); match(_A);
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match(_A);
            match(',');
        }

        if (lookahead=='(')
        {
            i = indirectoperand();

            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), "0%2xh", op8 | 0x6);
                expitem(1, text);
                if (i == 0)
                    expitem(1, ",");
            }
        }
        else
        {
            i = reg8_or_expr();

            t = i;
            if (i == _LA)
            {
                t = 0;
                op8 |= 0x46;
            }

            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), "0%2xh", op8 | t);
                expitem(1, text);

                if (i == _LA)
                {
                    expitem(1, ",");
                }
            }
        }
        return;

    case _HL: case _IX: case _IY:
        i = lookahead;

        if (GenPC)
        {
            emit(lookahead); match(lookahead);
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match(lookahead);
            match(',');
        }

        if ((i == _IX) || (i == _IY))
        {
            if ((t==_ADC) || (t==_SBC))
            {
                error("illegal instruction", 0);
            }
        }

        if (GenMSX)
        {
            switch (i)
            {
            case _IX: expitem(1, "0ddh,"); break;
            case _IY: expitem(1, "0fdh,"); break;
            }
            switch (t)
            {
            case _ADC: case _SBC:
                expitem(1, "0EDH,"); break;
            }
        }

        switch (lookahead)
        {
        case _HL: case _IX: case _IY:
            if (i != lookahead)
                error("illegal instruction", 0);

            i = _HL;
            break;

        default:
            i = lookahead;
            break;
        }

        if (GenMSX)
        {
            sprintf_s(text, sizeof(text), "0%xh", op16 |
                ((i - _BC) << 4));
            expitem(1, text);
        }
        if (GenPC)
        {
            emit(lookahead);
        }

        match(lookahead);

        break;

    default:
        error("illegal instruction", 0);
        break;
    }
}

void math8_16bit()
{
    int t;
    int op8 = 0;
    int op16 = 0;
    char text[10];

    t = lookahead;
    emit(lookahead);

    if (GenMSX)
    {
        switch (t)
        {
        case _SUB: op8 = 0x90; break;
        case _AND: op8 = 0xa0; break;
        case _OR:  op8 = 0xb0; break;
        case _XOR: op8 = 0xa8; break;
        case _CP:  op8 = 0xb8; break;
        case _INC: op8 = 0x04; op16 = 0x03; break;
        case _DEC: op8 = 0x05; op16 = 0x0b; break;
        }
    }

    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);
    if (lookahead=='(')
    {
        op16 = indirectoperand();

        if (GenMSX)
        {
            if ((t != _INC) && (t != _DEC))
                op8 |= 0x86;
            else
                op8 |= 0x30;

            sprintf_s(text, sizeof(text), "0%xh", op8);
            expitem(1, text);

            if (op16 == 0)
            {
                expitem(1, ",");
            }
        }
    }
    else
    {
        if (t == _INC || t == _DEC)
        {

            if (GenMSX)
            {
                t = lookahead;
                switch (lookahead)
                {
                case _IX:
                    expitem(1, "0ddh,");
                    t = _HL;
                    break;
                case _IY:
                    expitem(1, "0fdh,");
                    t = _HL;
                    break;
                }
            }

            switch (lookahead)
            {

            case _BC: case _DE: case _HL: case _SP:
            case _IX: case _IY:
                if (GenPC)
                {
                    emit(lookahead);
                }
                if (GenMSX)
                {
                    sprintf_s(text, sizeof(text), "%02xh",
                        op16 | (t - _BC) << 4);
                    expitem(1, text);
                }
                match(lookahead);

                return;

            case _B: case _C: case _D: case _E:
            case _H: case _L: case _A:

                break;

            default: emit(lookahead);
                match(lookahead);
                error("illegal instruction", 0);
                break;
            }
            t = reg8_or_expr();
            if (GenMSX)
            {
                sprintf_s(text, sizeof(text), "%02xh", op8 | (t << 3));
                expitem(1, text);
            }
            return;
        }
        t = reg8_or_expr();

        if (GenMSX)
        {
            if (t == _LA)
            {
                sprintf_s(text, sizeof(text), "0%xh,", op8 | 0x46);
            }
            else
            {
                sprintf_s(text, sizeof(text), "0%xh", op8 | 0x80 | t);
            }
            expitem(1, text);
        }
    }
}

void special_control_instruction()
{
    int t;

    t = lookahead;
    emit(lookahead);

    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);

    if (t == _IM)
    {

        if (lookahead!=NUM) error("illegal interrupt mode", 0);

        t = atoi(lookbuf);

        if (t<0 || t>2) error("illegal interrupt mode", 0);

        if (GenPC)
        {
            emit(NUM);
        }
        if (GenMSX)
        {
            switch (t) {
            case 0: expitem(1, "0edh,46h"); break;
            case 1: expitem(1, "0edh,56h"); break;
            case 2: expitem(1, "0edh,5eh"); break;
            }
        }

        match(NUM);
    }
}

void bitsetresinstruction()
{
    char *bitset[8] ={ ",1",",2",",4",",8",",16",",32",",64",",128" };
    char *res[8] ={ ",11111110b",
            ",11111101b",
            ",11111011b",
            ",11110111b",
            ",11101111b",
            ",11011111b",
            ",10111111b",
            ",01111111b" };
    int value, token;
    int i;
    int op8 = 0;
    char text[10];

    token = lookahead;

    /* emit BIT, SET or RES token */
    emit(lookahead);
    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);

    if (lookahead != NUM)
    {
        error("illegal bit number", 0);
    }

    value = atoi(lookbuf);

    if (value < 0 || value > 7)
    {
        error("illegal bit number", 0);
        return;
    }

    if (GenPC)
    {
        emit(NUM);
        chgitem(ItemIndex-1, "");
    }

    match(NUM);

    match(',');

    switch (token)
    {
    case _BIT: op8 = 0x40; break;
    case _SET: op8 = 0xc0; break;
    case _RES: op8 = 0x80; break;
    }

    if (lookahead=='(')
    {
        i = indirectoperand();
        if (GenPC) {
            expitem(ItemIndex-1, "");
        }
        if (GenMSX) {
            expitem(1, "0cbh");
            if (i == 0)
                expitem(1, ",");

            sprintf_s(text, sizeof(text), ",0%xh",
                op8 | (value << 3) | 6);

            expitem(ItemIndex-1, text);
        }
    }
    else
        switch (lookahead)
        {
        case _B: case _C: case _D: case _E:
        case _H: case _L: case _A:

            if (GenPC)
            {
                emit(lookahead);
            }
            if (GenMSX)
            {
                expitem(1, "0cbh");

                sprintf_s(text, sizeof(text), ",0%xh",
                    op8 | (value << 3) | lookahead - _B);

                expitem(1, text);
            }

            match(lookahead);

            break;
        default:
            error("illegal instruction", 0);
            break;
        }

    if (GenPC)
    {
        switch (token)
        {
        case _BIT:
            expitem(ItemIndex-1, bitset[value]);
            break;

        case _SET:
            expitem(0, "pushf\n\t");
            expitem(ItemIndex-1, bitset[value]);
            expitem(ItemIndex-1, "\n\tpopf");
            break;

        case _RES:
            expitem(0, "pushf\n\t");
            expitem(ItemIndex-1, res[value]);
            expitem(ItemIndex-1, "\n\tpopf");
            break;
        }
    }
}

int getcondition()
{
    int cc;

    if (strcmp(lookbuf,"NZ")==0) cc = 0x00;
    else
    if (strcmp(lookbuf,"Z")==0)  cc = 0x01;
    else
    if (strcmp(lookbuf,"NC")==0) cc = 0x02;
    else
    if (strcmp(lookbuf,"C")==0)  cc = 0x03;
    else
    if (strcmp(lookbuf,"PO")==0) cc = 0x04;
    else
    if (strcmp(lookbuf,"PE")==0) cc = 0x05;
    else
    if (strcmp(lookbuf,"P")==0)  cc = 0x06;
    else
    if (strcmp(lookbuf,"M")==0)  cc = 0x07;
    else
    {
        cc = 0x08;
    }

    return cc;
}

void jump_jumprelative_instruction()
{
    int t;
    int cc;
    char text[12];

    t = lookahead;
    emit(lookahead);

    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);

    if (lookahead=='(')
    {
        if (GenPC)
        {
            expitem(ItemIndex-1, "MP");

            emit('[');
            chgitem(ItemIndex-1, "");
        }

        match('(');

        if (GenPC)
        {
            emit(lookahead);
        }

        switch (lookahead)
        {
        case _HL:
            if (GenMSX) expitem(1, "0e9h");
            match(lookahead);
            break;
        case _IX:
            if (GenMSX) expitem(1, "0ddh,0e9h");
            match(lookahead);
            break;
        case _IY:
            if (GenMSX) expitem(1, "0fdh,0e9h");
            match(lookahead);
            break;

        default: error("illegal instruction", 0);
        }

        match(')');
    }
    else
    {
        cc = getcondition();

        switch (cc) {
        case 0x00:
            if (GenPC)
            {
                emit(_NZ); match(ID);
                chgitem(ItemIndex-2, "");
                expitem(ItemIndex-1, " near ptr ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x01:
            if (GenPC)
            {
                emit(_Z); match(ID);
                chgitem(ItemIndex-2, "");
                expitem(ItemIndex-1, " near ptr ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x02:
            if (GenPC)
            {
                emit(_NC); match(ID);
                chgitem(ItemIndex-2, "");
                expitem(ItemIndex-1, " near ptr ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x03:
            if (GenPC)
            {
                emit('C'); match(_C);
                chgitem(ItemIndex-2, "");
                expitem(ItemIndex-1, " near ptr ");
            }
            if (GenMSX)
            {
                match(_C);
            }
            break;

        case 0x04:
            if (t == _JR) error("illegal instruction", 0);

            if (GenPC)
            {
                emit(_PO); match(ID);
                chgitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x05:
            if (t == _JR) error("illegal instruction", 0);
            if (GenPC)
            {
                emit(_PE); match(ID);
                expitem(0, "; ---- ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x06:
            if (t == _JR)
            {
                error("illegal instruction", 0);
            }
            if (GenPC)
            {
                emit(_P); match(ID);
                chgitem(ItemIndex-2, "");
                chgitem(ItemIndex-1, "NS near ptr ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        case 0x07:
            if (t == _JR)
            {
                error("illegal instruction", 0);
            }
            if (GenPC)
            {
                emit(_M); match(ID);
                chgitem(ItemIndex-2, "");
                chgitem(ItemIndex-1, "S near ptr ");
            }
            if (GenMSX)
            {
                match(ID);
            }
            break;

        default:
            if (GenPC)
            {
                expitem(ItemIndex-1, "MP near ptr ");
            }
            if (GenMSX)
            {
                if (t == _JR)
                {
                    expitem(1, "18h,(");
                }
                else
                {
                    expitem(1, "0c3h\n\tDW ");
                }
            }
            expr();

            if (GenMSX)
            {
                if (t == _JR)
                {
                    expitem(ItemIndex-1, ")-$-1");
                }
            }
            return;
        }

        if (GenMSX)
        {
            if (t == _JR)
            {
                switch (cc)
                {
                case 0x00: expitem(1, "20h,("); break;
                case 0x01: expitem(1, "28h,("); break;
                case 0x02: expitem(1, "30h,("); break;
                case 0x03: expitem(1, "38h,("); break;
                }
            }
            else
            {
                sprintf_s(text, sizeof(text), "0%xh\n\tDW ", 0xc2 | (cc << 3));
                expitem(1, text);
            }
        }

        match(',');

        expr();

        if (GenMSX)
        {
            if (t == _JR)
            {
                expitem(ItemIndex-1, ")-$-1");
            }
        }

        return;
    }
}

void callretinstruction()
{
    int t;
    int cc;
    char text[12];

    t = lookahead;
    emit(lookahead);

    if (GenMSX)
    {
        chgitem(1, "DB ");
    }

    match(lookahead);

    cc = getcondition();

    switch (cc)
    {
    case 0x00:
        if (GenPC)
        {
            emit(_NZ);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x01:
        if (GenPC)
        {
            emit(_Z);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x02:
        if (GenPC)
        {
            emit(_NC);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x03:
        if (GenPC)
        {
            emit('C');
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(_C);
        break;
    case 0x04:
        if (GenPC)
        {
            emit(_PO);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x05:
        if (GenPC)
        {
            emit(_PE);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x06:
        if (GenPC)
        {
            emit(_P);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    case 0x07:
        if (GenPC)
        {
            emit(_M);
            chgitem(ItemIndex-2, "");
            emit(' ');
        }
        match(ID);
        break;
    default:
        if (t == _CALL)
        {
            if (GenPC)
            {
                expitem(ItemIndex-1, " near ptr");
            }
            if (GenMSX)
            {
                expitem(1, "0cdh\n\tDW ");
            }
            expr();
        }
        else
        {
            if (GenMSX)
                expitem(1, "0c9h");
        }

        return;

    }

    if (t == _CALL)
    {
        match(',');
        if (GenMSX)
        {
            sprintf_s(text, sizeof(text), "0%xh\n\tDW ", 0xc4 | (cc << 3));
            expitem(1, text);
        }
        expr();
    }
    else
    {
        if (GenMSX)
        {
            sprintf_s(text, sizeof(text), "0%xh", 0xc0 | (cc << 3));
            expitem(1, text);
        }
    }
}

void in_out_instruction()
{
    int flag,t;
    char text[15];

    t = lookahead;

    emit(lookahead);

    if (GenMSX) {
        chgitem(1,"DB ");
    }

    match(lookahead);

    flag = (strcmp(lookbuf,"F") == 0);

    switch (t)
    {
    case _IN:
        t = lookahead;

        if (GenPC)
        {
            emit(lookahead); match(lookahead);
            emit(','); match(',');
            emit('('); match('(');
        }
        if (GenMSX)
        {
            match(lookahead);
            match(',');
            match('(');
        }

        if (lookahead == _C)
        {
            if (flag) t = _LA;
            switch (t)
            {
            case _A: case _B: case _C: case _D:
            case _E: case _H: case _L: case _LA:
                if (GenPC)
                {
                    emit(_C); match(_C);
                    emit(')'); match(')');
                }
                if (GenMSX)
                {
                    match(_C);
                    match(')');
                }
                if (GenMSX)
                {
                    t -= _B;
                    sprintf_s(text, sizeof(text), "0edh,0%xh", 0x40 | (t << 3));
                    expitem(1, text);
                }
                return;

            default:
                error("illegal instruction", 0);
                break;
            }
        }

        if (t != _A)
        {
            error("illegal instruction", 0);
        }

        expr();

        expitem(1, "0dbh,");
        match(')');
        break;

    case _OUT:
        if (GenPC)
        {
            emit('(');  match('(');
        }
        if (GenMSX)
        {
            match('(');
        }

        if (lookahead == _C)
        {
            match(lookahead);
            if (GenPC)
            {
                emit(')'); match(')');
                emit(','); match(',');
                emit(lookahead);
            }
            if (GenMSX)
            {
                match(')');
                match(',');
            }

            switch (lookahead)
            {
            case _A: case _B: case _C: case _D:
            case _E: case _H: case _L:
                if (GenMSX)
                {
                    lookahead -= _B;
                    sprintf_s(text, sizeof(text), "0edh,0%xh", 0x41 | (lookahead << 3));
                    expitem(1, text);
                }
                match(lookahead);
                return;

            default:
                error("illegal instruction", 0);
                break;
            }
        }

        expr();

        if (GenPC)
        {
            emit(')'); match(')');
            emit(','); match(',');
        }
        if (GenMSX)
        {
            match(')');
            match(',');
        }
        if (lookahead != _A)
        {
            error("illegal instruction", 0);
        }
        if (GenPC)
        {
            emit(lookahead);
        }
        expitem(1, "0d3h,");
        match(lookahead);
        break;
    }
}

void DefineByteWord()
{
    emit(lookahead); match(lookahead);
    expr();

    while (lookahead==',')
    {
        emit(','); match(',');
        expr();
    }
}

void DotOperation()
{
    emit('.');
    match('.');
    switch (lookahead)
    {
        case _Z80:
            emit(_Z80);
            match(_Z80);

            if (GenPC)
            {
                expitem(0, "; ");
            }

            break;
        default:
            error("illegal dot operation",0);
            break;
    }
}

void oneword(int token, char *text)
{
    emit(token);
    match(token);

    if (GenMSX)
    {
        chgitem(1, "DB ");
        expitem(1, text);
    }
}

void parse()			/* parses and translates one statement */
{
    int op8,i;
    char text[10];

    lookahead = lookahead2 = 0;
    lexbuf[0] = '\0';
    commentbuf[0] = '\0';
    match(lookahead);
    match(lookahead);

    while (lookahead != DONE)
    {

        out();

        switch (lookahead)
        {
        case _LD:   loadinstruction(); 	   break;
        case _PUSH: pushinstruction();     break;
        case _POP:  popinstruction();      break;
        case _EX:   exchangeinstruction(); break;

        case _EXX:  oneword(_EXX, "0d9h");  break;

        case _LDI:  oneword(_LDI, "0edh,0a0h"); break;
        case _LDIR: oneword(_LDIR, "0edh,0b0h"); break;
        case _LDD:  oneword(_LDD, "0edh,0a8h"); break;
        case _LDDR: oneword(_LDDR, "0edh,0b8h"); break;
        case _CPI:  oneword(_CPI, "0edh,0a1h"); break;
        case _CPIR: oneword(_CPIR, "0edh,0b1h"); break;
        case _CPD:  oneword(_CPD, "0edh,0a9h"); break;
        case _CPDR: oneword(_CPDR, "0edh,0b9h"); break;

        case _ADD: case _ADC: case _SBC:
            add_adc_sbc_instruction();
            break;

        case _SUB: case _AND: case _OR: case _XOR:
        case _CP:  case _INC: case _DEC:
            math8_16bit();
            break;

        case _DAA:  oneword(_DAA, "27h"); break;
        case _CPL:  oneword(_CPL, "2fh"); break;
        case _NEG:  oneword(_NEG, "0edh,44h"); break;
        case _CCF:  oneword(_CCF, "3fh"); break;
        case _SCF:  oneword(_SCF, "37h"); break;
        case _NOP:  oneword(_NOP, "00h"); break;
        case _HALT: oneword(_HALT, "76h"); break;
        case _DI:   oneword(_DI, "0f3h"); break;
        case _EI:   oneword(_EI, "0fbh"); break;

        case _IM:
            special_control_instruction();
            break;

        case _RLCA: oneword(_RLCA, "07h"); break;
        case _RLA:  oneword(_RLA, "17h"); break;
        case _RRCA: oneword(_RRCA, "0fh"); break;
        case _RRA:  oneword(_RRA, "1fh"); break;
        case _RLD:  oneword(_RLD, "0edh,6fh"); break;
        case _RRD:  oneword(_RRD, "0edh,67h"); break;

        case _RLC: case _RL: case _RRC: case _RR:
        case _SLA: case _SRA: case _SRL:

            emit(lookahead);
            if (GenMSX)
            {
                chgitem(1, "DB ");

                switch (lookahead)
                {
                case _RLC: op8 = 0x00; break;
                case _RL:  op8 = 0x10; break;
                case _RRC: op8 = 0x08; break;
                case _RR:  op8 = 0x18; break;
                case _SLA: op8 = 0x20; break;
                case _SRA: op8 = 0x28; break;
                case _SRL: op8 = 0x38; break;
                }
            }

            match(lookahead);

            if (lookahead=='(')
            {
                i = indirectoperand();
                if (GenPC)
                {
                    expitem(ItemIndex-1, ",1");
                }
                if (GenMSX)
                {
                    expitem(1, "0cbh");
                    if (i == 0)
                        expitem(1, ",");

                    sprintf_s(text, sizeof(text), ",%02xh", op8 | 6);
                    expitem(ItemIndex-1, text);
                }
                break;
            }
            switch (lookahead)
            {
            case _B: case _C: case _D: case _E:
            case _H: case _L: case _A:

                if (GenPC)
                {
                    emit(lookahead);
                    expitem(ItemIndex-1, ",1");
                }
                if (GenMSX)
                {
                    sprintf_s(text, sizeof(text), "0cbh,%02xh", op8 |
                        (lookahead - _B));
                    expitem(1, text);
                }

                match(lookahead);

                break;

            default:
                error("illegal instruction", 0);
                break;
            }
            break;

        case _BIT: case _SET: case _RES:
            bitsetresinstruction();
            break;

        case _JP: case _JR:
            jump_jumprelative_instruction();
            break;

        case _DJNZ:
            emit(lookahead);

            if (GenMSX)
            {
                chgitem(1, "DB 10h,(");
            }
            if (GenPC)
            {
                expitem(ItemIndex-1, " near ptr");
            }

            match(lookahead);

            expr();

            if (GenMSX)
            {
                expitem(ItemIndex-1, ")-$-1");
            }
            break;

        case _CALL: case _RET:
            callretinstruction();
            break;

        case _RETI: oneword(_RETI, "0edh,4dh"); break;
        case _RETN: oneword(_RETN, "0edh,45h"); break;

        case _IN: case _OUT:
            in_out_instruction();
            break;

        case _INI:  oneword(_INI, "0edh,0a2h"); break;
        case _INIR: oneword(_INIR, "0edh,0b2h"); break;
        case _IND:  oneword(_IND, "0edh,0aah"); break;
        case _INDR: oneword(_INDR, "0edh,0bah"); break;
        case _OUTI: oneword(_OUTI, "0edh,0a3h"); break;
        case _OTIR: oneword(_OTIR, "0edh,0b3h"); break;
        case _OUTD: oneword(_OUTD, "0edh,0abh"); break;
        case _OTDR: oneword(_OTDR, "0edh,0bbh"); break;

        case _ORG:
            emit(lookahead); match(lookahead);
            expr();

            if (GenPC)
            {
                expitem(0, "; ");
            }
            break;

        case _DS:
            emit(lookahead);

            chgitem(ItemIndex-1, "db");

            match(lookahead);
            expr();

            expitem(ItemIndex-1, " dup (0)");
            break;

        case _DB: case _DW: case _DC:
            DefineByteWord();
            break;

        case '.':
            DotOperation();
            break;

        case _END:
            if (GenMSX)
            {
                emit(_END);
                addstr("\n\n");
                out();
            }
            return;

        case _IFDEF:
            emit(_IFDEF); match(_IFDEF);
            emit(ID); match(ID);
            break;

        case _IFNDEF:
            emit(_IFNDEF); match(_IFNDEF);
            emit(ID); match(ID);
            break;

        case _ENDIF:
            emit(_ENDIF); match(_ENDIF);
            break;

        case _ELSE:
            emit(_ELSE); match(_ELSE);
            break;

        case _INCLUDE:
            emit(_INCLUDE); match(_INCLUDE);
            expr();
            break;

        case _GLOBALS:
            emit(_GLOBALS); match(_GLOBALS);
            emit(ID); match(ID);
            break;

        case _PUBLIC:
            emit(_PUBLIC); match(_PUBLIC);
            emit(ID); match(ID);
            while (lookahead==',')
            {
                emit(','); match(',');
                emit(ID); match(ID);
            }
            break;

        case _EXTRN: case _GLOBAL:
            emit(lookahead); match(lookahead);
            emit(ID); match(ID);
            while (lookahead==',')
            {
                emit(','); match(',');
                emit(ID); match(ID);
            }
            break;

        case _OFF_HL:
            emit(_OFF_HL); match(_OFF_HL);
            expr();
            expr();
            break;

        case _ASEG:
            emit(lookahead);
            match(lookahead);
            if (GenPC)
            {
                expitem(0, "; ");
            }
            break;

        case ID:
            if (lookahead2 == ':')
            {
                emit(ID); match(ID);
                emit(':'); match(':');
                break;
            }
            if (lookahead2 == _EQU)
            {
                emit(ID); match(ID);
                emit(_EQU); match(_EQU);
                expr();
                break;
            }
        case COMMENT:
            emit(COMMENT); match(COMMENT);
            break;

        default:
            emit(lookahead);
            error("illegal instruction start", 0);
            break;
        }
    }
    out();
}

void expr()
{
    NumberOfIDs = 0;
    expr1();
    LabelInExpression = (NumberOfIDs > 0);
}

void expr1()
{
    if ((lookahead == '+') || (lookahead == '-'))
    {
        emit(lookahead); match(lookahead);
    }
    term();
    while (1)
    {
        switch (lookahead)
        {
        case '+': case '-':
            emit(lookahead);
            match(lookahead);
            term();
            continue;
        default:
            return;
        }
    }
}

void term()
{
    factor();
    while (1)
    {
        switch (lookahead)
        {
        case '*':
        case '/':
            emit(lookahead);
            match(lookahead);
            factor();
            continue;
        default:
            return;
        }
    }
}

void factor()
{
    switch (lookahead)
    {
    case '(':
        emit('('); match('(');
        expr1();
        emit(')'); match(')');
        break;
    case NUM:
        emit(NUM); match(NUM);
        break;
    case ID:
        emit(ID); match(ID);
        NumberOfIDs++;
        break;
    case STRING:
        emit(STRING); match(STRING);
        break;
    default:
        error("syntax error", 0);
        break;
    }
}

void match(t)
int t;
{
//    int i;

    if (lookahead == t)
    {

       lookahead = lookahead2;
       strcpy_s(lookbuf, sizeof(lookbuf), lookbuf2);
       strcpy_s(combuf, sizeof(combuf), combuf2);

       lookahead2 = lexan();
       strcpy_s(lookbuf2, sizeof(lookbuf2), lexbuf);
       strcpy_s(combuf2, sizeof(combuf2), commentbuf);

       lexbuf[0]     = '\0';
       commentbuf[0] = '\0';

    }
    else
    {
        error("", t); /* t(oken) expected */
    }

}

