#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define VALID_ARG_NUM 4
#define SEQUENCES_FILE_IDX 1

#define MAX_SEQUENCES 100
#define MAX_LINE_LENGTH 100 // + 1 for null-terminator char

#define HEADER_LINE_PREFIX '>'

void loadSequences(const char *filePath, char **seqArr);



/**
 * The entry point of the program.
 * @param argc The command line argument count.
 * @param argv The command line argument values.
 * @return 0 on success, exits on failure.
 */
int main(unsigned int argc, char *argv[])
{
    // holds the pointers to the loaded sequences
    char *seqArr[MAX_SEQUENCES] = {{0}};

    if (argc != VALID_ARG_NUM)
    {
        // todo print usage message. check if to stdout or stderr
        exit(EXIT_FAILURE);
    }

    loadSequences(argv[SEQUENCES_FILE_IDX], seqArr);


    return 0;
}

void loadSequences(const char *filePath, char **seqArr)
{
    FILE *fp;
    char buffer[MAX_LINE_LENGTH] = {0};
    unsigned int curSeqIdx = 0;
    char *curSequence = (char*) malloc(MAX_LINE_LENGTH);
    unsigned int maxSeqLen = MAX_LINE_LENGTH;
    unsigned int curSeqLen = 0;

    fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        // todo print file open error
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL)
    {
        
    }

}