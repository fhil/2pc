#include "glob.h"

extern int tokenval;		/* value of token attribute */
extern int lineno;

extern int lookahead;
extern FILE *input;

int PosTab[SYMMAX];
int CountTab[SYMMAX];

int lastchar = -1;		/* last used position in lexemes */
struct entry symtable[SYMMAX];
int lastentry = 0;		/* last used position in symtable */

int looktoken(int t)
{
	int p;
	for (p = 0; p < lastentry; p++)
	{
		if (symtable[p].token == t)
		{
			return p;
		}
	}
	return 0;
}

void initpostab()
{
}

int SearchString(char *String)
{
	int res;
	int m, left, right;

	left = 0L;
	right = (int)lastentry+1;

	while (1)
	{
		m = (left+right) >> 1L;
		if (m != left)
		{
			res = strcmp(String, symtable[PosTab[m]].lexptr);
			if (res==0)
			{
				return m;
			}
			else
			if (res < 0)
			{
				right = m;
			}
			else
			{
				left = m;
			}
		}
		else
		{
			return -right;
		}
	}
}

int Add(int pos)
{
	int i;

	if (pos < 0)
	{
		pos = -pos;
		for (i = lastentry; i >= pos; i--)
		{
			PosTab[i+1] = PosTab[i];
		}
		PosTab[pos] = lastentry+1;
		pos = -pos;
		return 1;
	}
	return 0;
}

int insert(int pos, char *String, int token)
{
	int flag;

	if (lastentry == 0)
	{
		PosTab[1] = 1;
		symtable[1].token = token;
		symtable[1].lexptr = (char *)stralloc(String);

		lastentry++;
		return 1;
	}
	else
	{
		flag = Add(pos);
		if (flag)
		{
			pos=-pos;
			lastentry++;
			symtable[lastentry].token = token;
			symtable[lastentry].lexptr = (char *)stralloc(String);
		}
		return pos;
	}
}

int lookup(char *s)             /* returns position of entry for s */
{
	int p;
	p = SearchString(s);

	return p;
}

