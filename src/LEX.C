#include "glob.h"
#include <string.h>

extern char *skip();
extern char combuf[];

char lexbuf[BSIZE];
char commentbuf[3*BSIZE];
int lineno=1;
int tokenval=NONE;

void digitsequence(char t)
{
    char *ptr;
    int b=0;

    ptr = lexbuf + strlen(lexbuf);
    *ptr++ = t;
    t = getc(input);

    while (isdigit(t))
    {
        *ptr++ = t;
        t = getc(input);

        if (ptr - lexbuf >= BSIZE)
        {
            error("lexical buffer overflow",0);
        }
    }

    ungetc(t,input);

    *ptr = '\0';
}

void hexdigitsequence(char t)
{
    char *ptr;
    int b=0;

    ptr = lexbuf + strlen(lexbuf);
    *ptr++ = t;
    t = getc(input);

    while (ishex(t))
    {
        *ptr++ = t;
        t = getc(input);

        if (ptr - lexbuf >= BSIZE)
        {
                error("lexical buffer overflow",0);
        }
    }

    if ((t == 'H') || (t == 'h'))
    {
        *ptr++ = t;
    }
    else
    {
        ungetc(t,input);
    }

    *ptr = '\0';
}

void identifier(char t)
{
    char *ptr;
    int b=0;

    ptr = lexbuf + strlen(lexbuf);
    *ptr++ = toupper(t);
    t = getc(input);

    while (isdigit(t) || isalpha(t) || (t == '_') ||
            (t == '\'') || (t == '.'))
    {
        *ptr++ = toupper(t);
        t = getc(input);

        if (ptr - lexbuf >= BSIZE)
        {
                error("lexical buffer overflow",0);
        }
    }

    ungetc(t,input);

    *ptr = '\0';
}

void string(int t, char limiter)
{
    char *ptr;
    //size_t b;
    char c;

    //b = strlen(lexbuf);
    ptr = lexbuf + strlen(lexbuf);

    if (limiter != '"') *ptr++ = t;
    t = getc(input);

    while ((t != EOF) && (t != '\n') && (t != 255))
    {
        if (t == limiter)
        {
            c = getc(input);
            if (c != limiter)
            {
                ungetc(c,input);
                break;
            }
            else *ptr++ = c;
        }

        if (t != '"')
        {
            *ptr++ = t;
        }

        t = getc(input);

        if (ptr - lexbuf >= BSIZE)
        {
            error("lexical buffer overflow",0);
        }
    }

    if (t == limiter)
    {
        if (t != '"')
        {
            *ptr++ = t;
        }
    }
    else
    {
        error("invalid termination of character string",0);
    }

    *ptr = '\0';
}

void comment()
{
    char *ptr;
    //int b;
    int t;

    //b = strlen(commentbuf);
    ptr = commentbuf + strlen(commentbuf);
    t = getc(input);
    while ((t != EOF) && (t != '\n') && (t != 255))
    {
        *ptr++ = t;

        t = getc(input);

        if (ptr - commentbuf >= 3*BSIZE)
        {
                error("lexical buffer overflow",0);
        }
    }

    ungetc(t,input);

    *ptr = '\0';
}

char *skip(s)
char *s;
{
    while (isspace(*s)) s++;

    return s;
}

void printinfo()
{
    static int once = 0;

    if (once == 0)
    {
        once++;

        fprintf(stderr,"\rlines: %u symbols: %u\r",
                lineno,lastentry);
    }
}

int lexan()			/* lexical analyzer */
{
    int t, t2;
    int i;
    char s[BSIZE];
    while (1)
    {
        t = getc(input);
        if (t == 255)
        {
            t = -1;
        }

        if (t == '\'')
        {
            string(t,'\'');
            return STRING;
        }
        else
        if (t == '"')
        {
            string(t,'"');
            return STRING;
        }
        else
        if (t == ';')
        {
            t2 = getc(input);
            if ( (t2 == '!') && (GenPC) )
            {
                /* ";!" parsed */

                /*
                if (strlen(commentbuf)>2*BSIZE) {
                fprintf(output,"%s",commentbuf);
                commentbuf[0] = '\0';
                }
                */

                comment();

                return COMMENT;
            }
            else
            if ( (t2 == 'M') && (GenMSX) )
            {
                /* ";M" parsed */

                /*
                if (strlen(commentbuf)>2*BSIZE)
                {
                    fprintf(output,"%s",commentbuf);
                    commentbuf[0] = '\0';
                }
                */

                comment();

                return COMMENT;
            }
            else
            {
                ungetc(t2,input);

                /*
                if (strlen(commentbuf)>2*BSIZE)
                {
                    fprintf(output,"%s",commentbuf);
                    commentbuf[0] = '\0';
                }
                */

                int len = (int)strlen(commentbuf);

                commentbuf[len] = t;
                commentbuf[len+1] = '\0';

                comment();

                return COMMENT;
            }
        }
        else
        if (t=='\n')
        {
            lineno++;
            if ((lineno & 1023) == 0) fprintf(stderr,".");
            s[0] = '\n';
            s[1] = '\0';
            strcat_s(commentbuf, sizeof(commentbuf), s);
        }
        else
        if (isspace(t))
        {
            s[0] = t;
            s[1] = '\0';
            strcat_s(commentbuf, sizeof(commentbuf), s);
        }
        else
        if (isdigit(t))             /* t is a digit */
        {
            hexdigitsequence(t);    /* digit seq. to lexbuf */
            return DIGITS;
        }
        else if (t=='$')
        {
            t2=getc(input);
            ungetc(t2,input);
            if (ishex(t2))          /* followed by a hex. digit */
            {   
               hexdigitsequence(t);
               return HEXDIGITS;
            }
            else
            {
                lexbuf[0]='$';
                lexbuf[1]='\0';
                return ID;
            }
        }
        else
        if (isalpha(t) || (t=='_')) /* t is a letter */
        {
            identifier(t);
            tokenval = lookup(lexbuf);

            if (tokenval<=0)
            {
                return ID;
            }

            return symtable[PosTab[tokenval]].token;
        }
        else
        if (t == EOF)
        {
            printinfo();
            return DONE;
        }
        else
        {
            tokenval = NONE;
            lexbuf[0] = t;
            lexbuf[1] = '\0';
            return t;
        }
    }
}
