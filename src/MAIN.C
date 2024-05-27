#include "glob.h"

FILE *input;
FILE *output;
FILE *outputcode;

char *VERSION="2PC: Version 1.6 by F. Hilderink\n";

void init();
void parse();

extern char lexbuf[];
extern int lastentry;

int GenPC;
int GenMSX;
int GenCODE;

void main(int argc, char *argv[])
{
    errno_t error;
    int i;

    if (argc < 3)
    {
        printf("%s", VERSION);
        printf("No input file given\n");
        exit(1);
    }

    error = fopen_s(&input, argv[1], "r");
    if (error != 0)
    {
        printf("%s", VERSION);
        printf("error opening input file '%s'\n", argv[1]);
        exit(1);
    }
    i = setvbuf(input, NULL, _IOFBF, 512*1024);


    error = fopen_s(&output, argv[2], "w");
    if (error != 0)
    {
        printf("%s", VERSION);
        printf("error creating output file '%s'\n", argv[2]);
        exit(1);
    }

    GenPC = 0;
    GenMSX = 0;
    GenCODE = 0;

    if (argc >= 4)
    {
        if (strcmp(argv[3], "pc") == 0) GenPC = 1;
        if (strcmp(argv[3], "msx") == 0) GenMSX = 1;
        if (strcmp(argv[3], "code") == 0) GenCODE = 1;
    }

    i = setvbuf(output, NULL, _IOFBF, 512*1024);

    fprintf(stderr, "%s", VERSION);

    if (GenMSX)
    {
        fprintf(stderr, "Converting Z80 mnemonics to DB/DW statements\n");
    }

    if (GenPC)
    {
        fprintf(stderr, "Converting Z80 mnemonics to 80x86 assembler mnemonics\n");
    }

    initpostab();
    init();
    parse();

    fprintf(stderr, "\n");

    fclose(input);
    fclose(output);

    exit(0);
}
