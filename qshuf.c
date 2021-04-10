#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NTRY 5

long long int fileSizeInBytes(FILE *fp)
{
    long long int size = 0;

    fseek(fp,SEEK_SET,SEEK_END);
    size=ftell(fp);
    rewind(fp);
    return size;
}

int printHeader(FILE *fp, char *delim, int blen)
{
    char *header;
    header = malloc(blen);

    char iterhead[blen];
    char *token;
    int cnt = 0;

    fgets(header,blen,fp);
    rewind(fp);
    fgets(iterhead,blen,fp);
    token = strtok(iterhead,delim);
    while (token != NULL)
    {
        cnt++;
        token = strtok(NULL,delim);
    }
    rewind(fp);
    printf("%s", header);
    free(header);

    return cnt;
}

int gatherNewLine(char **result, char *buf, int s,  char *delim, int blen)
{
    int bool=-1, tcnt=0;
    for(int i=s+1;i<blen;i++)
    {
        if (buf[i]=='\"')
        {
            bool = -1*bool;
        }
        if (bool == -1 && buf[i] == delim[0])
        {
            tcnt++;
        }
        if (bool == -1 && buf[i] == '\n')
        {
            *result = (char*)malloc((i-s+1)*sizeof(char));
            strncpy(*result, buf+s+1, i-s);
            (*result)[i-s] = '\0';
            return tcnt;
        }
    }
    return -1;
}

int seekFirstBreakLine(int instring, int blen, char* buf)
{
    for(int i=0;i<blen;i++)
    {
        if(buf[i] == '\"')
        {
            instring = -1*instring;
        }
        if (instring < 0 && buf[i] == '\n')
        {
            return i;
        }
        if(buf[i] == EOF)
        {
            return -1;
        }
    }
    return -1;
}

int printRandomLine(char *buf, int ntok, char *delim, int blen)
{
    char *instr=NULL, *outstr=NULL;
    int tcnt_in=0, tcnt_out=0;
    int posbl = -1;
    int boolinstr = 1;

    posbl = seekFirstBreakLine(boolinstr,blen,buf);
    tcnt_in = gatherNewLine(&instr, buf, posbl, delim, blen);
    
    int booloutstr = -1;
    posbl = seekFirstBreakLine(booloutstr,blen,buf);
    tcnt_out = gatherNewLine(&outstr, buf, posbl, delim, blen);

    int result = -1;
    if (tcnt_in == ntok-1 && tcnt_out != ntok-1)
    {
        printf("%s",instr);
        result=tcnt_in;
    }
    else if (tcnt_out == ntok-1 && tcnt_in != ntok-1)
    {
        printf("%s",outstr);
        result=tcnt_out;
    }
    
    if(instr!=NULL)
    {
        free(instr);
    }        
    if(outstr!=NULL)
    {
        free(outstr);
    }
        
    return result;
}

void randomBuffers(FILE *fp, long long int size,
                   int ntok, char *delim, int nrows, int blen)
{
    int val=0;
    long long int r;
    srand(time(NULL));
    for (int i=0;i<nrows;i++)
    {
        char * buffers = malloc(blen);

        for(int j=0;j<NTRY;j++)
        {
            r = rand()%size;
            fseek(fp,r,0); 
            for (int k=0;k<blen;k++)
            {
                buffers[k] = fgetc(fp);
            }
            val = printRandomLine(buffers, ntok, delim, blen);
            if(val != -1)
            {
                break;
            }
        }
        free(buffers);        
    }
    return;
}

int main (int argc, char **argv)
{   // 1 filename, 2 delim, 3 nrows, 4 buffer_lengt
    FILE *fp;
    char *delim = argv[2];
    int nrows = atoi(argv[3]), blen = atoi(argv[4]);
    int ntok;
    long long int fsize;

    fp = fopen(argv[1],"r");

    fsize = fileSizeInBytes(fp);
    ntok = printHeader(fp, delim, blen);
    randomBuffers(fp, fsize,ntok, delim, nrows, blen);
    fclose(fp);
    return 0;
}
