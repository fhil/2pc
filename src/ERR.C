#include "glob.h"

/* This routine prints all error messages */
void error(char *m, int t)
{

        printf("\nline %d\n",lineno);

        if (t) {

                if (t >= 256)
                {
                        printf("'%s' expected ",
                                symtable[looktoken(t)].lexptr);
                }
                else
                {
                        printf("'%c' expected ",(char)t);
                }

                if (lookahead >= 256)
                {
                        printf(" '%s' found\n",symtable[looktoken(lookahead)].lexptr);
                }
                else
                {
                        printf(" '%c' found\n",lookahead);
                }

                exit(1);

        }
        else
        {

                printf("%s\n",m);

                exit(1);

        }
}

