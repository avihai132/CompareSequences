#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define VALID_ARG_NUM 5 // 4 + 1 for program name
#define SEQUENCES_FILE_IDX 1
#define MATCHW_IDX 2
#define MISMATCHW_IDX 3
#define GAPW_IDX 4

#define MAX_SEQUENCES 100
#define MAX_LINE_LENGTH 101 // + 1 for null-terminator char

#define HEADER_LINE_PREFIX '>'


typedef struct Sequence {
    char *seqName;
    char *seq;
} Sequence;


int strToInt(char *str);

Sequence* newSequence();

int loadSequences(const char *filePath, Sequence **seqArr);

void copySeqName(Sequence **seq, char *str);

void replaceNl(char *str);

void resizeSequence(char **curSeq, unsigned int curSeqLen, unsigned int *curSeqMaxLen,
                    unsigned int curLineLen);

void printLoadedSequences(Sequence **seqArr, int seqNum); // todo - remove, only for debug

void compareSequences(Sequence *seq1, Sequence *seq2, int matchW, int mismatchW, int gapW);

void initFirstRowCol(int *tbl, int rows, int cols, int gapW);

void populateTable(const char *seq1, const char *seq2, int *tbl, int rows, int cols, int matchW,
                   int mismatchW, int gapW);

void printTable(char *seq1, char *seq2, int *tbl, int rows, int cols);

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

    int matchW = strToInt(argv[MATCHW_IDX]);
    int mismatchW = strToInt(argv[MISMATCHW_IDX]);
    int gapW = strToInt(argv[GAPW_IDX]);

    seqNum = loadSequences(argv[SEQUENCES_FILE_IDX], seqArr);
    printLoadedSequences(seqArr, seqNum);
    for (int i = 0; i < seqNum; i++)
    {
        for (int j = i + 1; j < seqNum; j++)
        {
            compareSequences(seqArr[i], seqArr[j], matchW, mismatchW, gapW);
        }
    }
    // todo: make a function to free all the allocated memory

    return 0;
}

int strToInt(char *str)
{
    int intVal = 0;
    char *end;
    errno = 0;
    intVal = (int) strtod(str, &end);
    if (intVal == 0 && (errno != 0 || end == str))
    {
        // todo - check the error message and check what string inputs i can get
        fprintf(stderr, "Error - cannot convert string to int.");
        exit(EXIT_FAILURE);
    }

    return intVal;
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
            copySeqName(&curSeq, buffer);
            curSeqLen = 0;
            curSeqMaxLen = MAX_LINE_LENGTH;
            continue; // nothing else to do with header line
        }

        // handle sequence line
        replaceNl(buffer);
        curLineLen = strlen(buffer);

        if (curSeqLen + curLineLen + 1 > curSeqMaxLen) // +1 for \0
        {
            //printf("%d + %d + 1 = %d > %d\n", curSeqLen, curLineLen, curSeqLen + curLineLen + 1 ,curSeqMaxLen); todo - for debug remove
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

void copySeqName(Sequence **seq, char *str)
{
    // todo check if the name has char limit - if not, need to make dynamic alloc
    strcpy((*seq)->seqName, str + 1); // +1 to skip the first '>' char
    // replace \n with \0 if exists
    if ((*seq)->seqName[strlen((*seq)->seqName) - 1] == '\n')
    {
        (*seq)->seqName[strlen((*seq)->seqName) - 1] = '\0';
    }
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

void compareSequences(Sequence *seq1, Sequence *seq2, int matchW, int mismatchW, int gapW)
{
    int cols = strlen(seq1->seq) + 1; // +1 for first empty cols
    int rows = strlen(seq2->seq) + 1; // +1 for first empty rows

    printf("Comparing %s to %s with M=%d, S=%d, G=%d\n", seq1->seqName, seq2->seqName, matchW, mismatchW, gapW);
    int *compTbl = (int*) calloc((size_t) cols * rows, sizeof(int));
    if (compTbl == NULL)
    {
        // todo print error msg, exit, (free memory, close file)
        exit(EXIT_FAILURE);
    }

    initFirstRowCol(compTbl, rows, cols, gapW);
    populateTable(seq1->seq, seq2->seq, compTbl, rows, cols, matchW, mismatchW, gapW);
    printTable(seq1->seq, seq2->seq, compTbl, rows, cols);
}

void initFirstRowCol(int *tbl, int rows, int cols, int gapW)
{
    tbl[0] = 0; // first cell is 0
    for (int i = 1; i < rows; i++) // start from 1 to not overwrite tbl[0][0]
    {
        tbl[i * cols] = gapW;
    }

    for (int i = 1; i < cols; i++) // start from 1 to not overwrite tbl[0][0]
    {
        tbl[i] = gapW;
    }
}

void populateTable(const char *seq1, const char *seq2, int *tbl, int rows, int cols, int matchW,
                   int mismatchW, int gapW)
{
    int cur = 0;
    int curPlusTop = 0;
    int curPlusLeft = 0;
    int curPlusDiag = 0;
    int max = 0;

    for (int i = 1; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            cur = (seq1[j - 1] == seq2[i - 1]) ? matchW : mismatchW; // -1 because of empty col/row
            curPlusTop = gapW + tbl[((i - 1) * cols) + j];
            curPlusLeft = gapW + tbl[(i * cols) + j - 1];
            curPlusDiag = cur + tbl[((i - 1) * cols) + j - 1];

            max = (curPlusTop > curPlusLeft) ? curPlusTop : curPlusLeft;
            max = (max > curPlusDiag) ? max : curPlusDiag;
            tbl[(i * cols) + j] = max;
        }
    }
}

void printTable(char *seq1, char *seq2, int *tbl, int rows, int cols)
{
    printf("       ");
    for (int i = 0; i < strlen(seq1); i++)
    {
        printf(" %c  ", seq1[i]);
    }
    printf("\n");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (j == 0)
            {
                if (i == 0)
                {
                    printf("  ");
                }
                else
                {
                    printf("%c ", seq2[i - 1]);
                }
            }
            printf(" %02d ", tbl[i * cols + j]);
        }
        printf("\n");
    }
}