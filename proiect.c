#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016

#define BUFFERSIZE 4096

int main( int argc, char *argv[])
{
    int fd1,fd2;
    char s[120];
    char buff[BUFFERSIZE];
    int n;

    if(argc!=2)
    {
        exit(2);
    }
    char *ext;
        ext=strchr(argv[1],'.');
        if(strcmp(ext,".bmp")!=0)
             printf("Usage %s %s",argv[0], argv[1]);
    /*** Deschiderea fisierelor */
    if((fd1=open(argv[1], O_RDONLY))<0)
    {
        printf("Error opening input file\n");
        exit(2);
    }
    if((fd2=open("statistica.txt", O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0)
    {
        printf("Error creating destination file\n");
        exit(3);
    }

    //scriere denumire fisier
    sprintf(s, "nume fisier: %s\n", argv[1]);
    if(write(fd2,s,strlen(s))!=strlen(s))
    {
        printf("Error writing to file\n");
        exit(4);
    }
    //citire lungime si inaltime fisier bmp
    int32_t l,i;
    fseek(argv[1],18*4,SEEK_SET);
    fread(l,4,1,argv[1]);
    sprintf(s, "lungime: %d\n", l);
    if(write(fd2,s,strlen(s))!=strlen(s))
    {
        printf("Error writing to file\n");
        exit(4);
    }
    fseek(argv[1],4,SEEK_CUR);
    fread(i,4,1,argv[1]);
    sprintf(s, "inaltime: %d\n", i);
    if(write(fd2,s,strlen(s))!=strlen(s))
    {
        printf("Error writing to file\n");
        exit(4);
    }
    //scriere in statistica.txt
    
}