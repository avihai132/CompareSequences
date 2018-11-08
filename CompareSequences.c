#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define VALID_ARG_NUM 5 // 4 + 1 for program name
#define SEQUENCES_FILE_IDX 1

#define MAX_SEQUENCES 100
#define MAX_LINE_LENGTH 101 // + 1 for null-terminator char

#define HEADER_LINE_PREFIX '>'


typedef struct Sequence {
    char *seqName;
    char *seq;
} Sequence;


Sequence* newSequence();

int loadSequences(const char *filePath, Sequence **seqArr);

void replaceNl(char *str);

void resizeSequence(char **curSeq, unsigned int curSeqLen, unsigned int *curSeqMaxLen,
                    unsigned int curLineLen);

void printLoadedSequences(Sequence **seqArr, int seqNum); // todo - remove, only for debug

/*
 * todo list
 * todo - check that i freed all allocated memory
 * todo - check that i checked all allocations are not NULL - (int*) a = malloc(...) if (a == NULL)
 * todo - check const in func signature
 * todo - check documentation
 * todo - check coding style
 * todo - check if else coding style
 */

/**
 * The entry point of the program.
 * @param argc The command line argument count.
 * @param argv The command line argument values.
 * @return 0 on success, exits on failure.
 */
int main(unsigned int argc, char *argv[])
{
    // holds the pointers to the loaded sequences
    Sequence *seqArr[MAX_SEQUENCES] = {{0}};
    int seqNum = 0;

    if (argc != VALID_ARG_NUM)
    {
        // todo print usage message. check if to stdout or stderr
        exit(EXIT_FAILURE);
    }

    seqNum = loadSequences(argv[SEQUENCES_FILE_IDX], seqArr);
    printLoadedSequences(seqArr, seqNum);
    // todo: make a function to free all the allocated memory

    return 0;
}

Sequence* newSequence()
{
    Sequence* seq = (Sequence*) malloc(sizeof(Sequence));
    seq->seqName = (char*) malloc(MAX_LINE_LENGTH);
    seq->seq = (char*) malloc(MAX_LINE_LENGTH);
    if (seq->seqName == NULL || seq->seq == NULL)
    {
        // todo print error msg, exit, (free memory, close file)
        exit(EXIT_FAILURE);
    }

    return seq;
}

int loadSequences(const char *filePath, Sequence **seqArr)
{
    FILE *fp;
    char buffer[MAX_LINE_LENGTH] = {0};
    unsigned int curSeqIdx = 0;
    Sequence *curSeq = NULL;
    unsigned int curSeqMaxLen = 0; // the max length of curSeq (its size)
    unsigned int curSeqLen = 0; // the actual used length of curSeq up until the point we
                                // read it (if it spans over multiple lines)
    unsigned int curLineLen = 0;
    int isFirstHeader = 1;

    fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        // todo print file open error
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL)
    {
        // handle header line
        if (buffer[0] == HEADER_LINE_PREFIX)
        {
            if (isFirstHeader == 1)
            {
                isFirstHeader = 0;
            }
            else
            {
                // if this is not the first header - store prev seq in arr and update seq index
                curSeq->seq[curSeqLen] = '\0'; // add \0 at the end of the sequence
                seqArr[curSeqIdx] = curSeq;
                curSeqIdx++;
            }

            // start a new sequence
            curSeq = newSequence();
            strcpy(curSeq->seqName, buffer + 1); // +1 to start copying after '>' // todo: make copySeqName func and check if there is \n
            curSeqLen = 0;
            curSeqMaxLen = MAX_LINE_LENGTH;
            continue; // nothing else to do with header line
        }

        // handle sequence line
        replaceNl(buffer);
        curLineLen = strlen(buffer);

        if (curSeqLen + curLineLen + 1 > curSeqMaxLen) // +1 for \0
        {
            printf("%d + %d + 1 = %d > %d\n", curSeqLen, curLineLen, curSeqLen + curLineLen + 1 ,curSeqMaxLen);
            // MAX_LINE_LENGTH >= curLineLen so it's safe to realloc with old-size + MAX_LINE_LENGTH
            curSeq->seq = (char*) realloc(curSeq->seq, curSeqMaxLen + MAX_LINE_LENGTH);
            if (curSeq->seq == NULL)
            {
                // todo print error msg, exit, (free memory, close file)
                exit(EXIT_FAILURE);
            }
            curSeqMaxLen += MAX_LINE_LENGTH;
        }

        // append the current line to the sequence without the \0
        memcpy(curSeq->seq + curSeqLen, buffer, strlen(buffer));
        curSeqLen += strlen(buffer);
    }

    // handle left overs
    curSeq->seq[curSeqLen] = '\0'; // add \0 at the end of the sequence
    seqArr[curSeqIdx] = curSeq;
    curSeqIdx++;

    fclose(fp);

    /*
     * todo - check if the file contains no header lines (empty file), check if the file
     * todo - contains header line and no sequence after it - it can affect curSeqIdx correctness
     * todo - because it starts from -1, and because if there is only header line and no sequence
     * todo - it will inc curSeqIdx without actually putting anything in seqArr[curSeqIdx]
     */

    // todo - free the allocated sequence
    return curSeqIdx;
}

void replaceNl(char *str)
{
    if (str[strlen(str) - 1] == '\n')
    {
        str[strlen(str) - 1] = '\0';
    }
}

void resizeSequence(char **curSeq, unsigned int curSeqLen, unsigned int *curSeqMaxLen,
                     unsigned int curLineLen)
{
    if (curSeqLen + curLineLen + 1 > *curSeqMaxLen) // +1 for \0
    {
        // MAX_LINE_LENGTH >= curLineLen so it's safe to realloc with old-size + MAX_LINE_LENGTH
        *curSeq = (char*) realloc(*curSeq, *curSeqMaxLen + MAX_LINE_LENGTH);
        if (*curSeq == NULL)
        {
            // todo print error msg, exit, (free memory, close file)
        }
        *curSeqMaxLen += MAX_LINE_LENGTH;
    }
}

void printLoadedSequences(Sequence **seqArr, int seqNum)
{
    for (int i = 0; i < seqNum; i++)
    {
        printf("Sequence %s: %s\n", seqArr[i]->seqName, seqArr[i]->seq);
    }
}