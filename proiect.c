#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include <time.h>

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016

#define BUFFERSIZE 4096

int main( int argc, char *argv[])
{
    int fd1,fd2;
    char s[120];

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
    int l,i;
    lseek(fd1,18,SEEK_SET);
    read(fd1,&l,4);
    sprintf(s, "lungime: %d\n", l);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    lseek(fd1,22,SEEK_SET);
    read(fd1,&i,4);
    sprintf(s, "inaltime: %d\n", i);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }

    //dimensiune: <dimensiune in octeti>
    struct stat info;
    fstat(fd1,&info);
    sprintf(s, "dimensiune: %ld\n", info.st_size);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    //varianta 2
    /*int dim;
    lseek(fd1,2,SEEK_SET);
    read(fd1,&dim,4);
    sprintf(s, "dimensiune2: %d\n", dim);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    */

    //identificatorul utilizatorului: <user id>
    sprintf(s, "identificatorul utilizatorului: %d\n", info.st_uid);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    //timpul ultimei modificari: 28.10.2023
   struct tm *timp;
   struct stat attr;
   stat(argv[1],&attr);
   timp=gmtime(&(attr.st_mtime));
    sprintf(s, "timpul ultimei modificari: %s",asctime(timp));
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    
    //contorul de legaturi: <numar legaturi>
    sprintf(s, "contorul de legaturi: %d\n", info.st_nlink);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }

    
    //drepturi de acces user: RWX
    char drept[4];
    if(info.st_mode&S_IRUSR)
            drept[0]='r';
    else
            drept[0]='-';
    if(info.st_mode&S_IWUSR)
            drept[1]='w';
    else
            drept[1]='-';
    if(info.st_mode&S_IXUSR)
            drept[2]='x';
    else
            drept[2]='-';
    sprintf(s, "drepturi de acces user: %s\n", drept);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
    //drepturi de acces grup: R–-
    if(info.st_mode&S_IRGRP)
            drept[0]='r';
    else
            drept[0]='-';
    if(info.st_mode&S_IWGRP)
            drept[1]='w';
    else
            drept[1]='-';
    if(info.st_mode&S_IXGRP)
            drept[2]='x';
    else
            drept[2]='-';
    sprintf(s, "drepturi de acces grup: %s\n", drept);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }


    //drepturi de acces altii: ---
    if(info.st_mode&S_IROTH)
            drept[0]='r';
    else
            drept[0]='-';
    if(info.st_mode&S_IWOTH)
            drept[1]='w';
    else
            drept[1]='-';
    if(info.st_mode&S_IXOTH)
            drept[2]='x';
    else
            drept[2]='-';
    sprintf(s, "drepturi de acces altii: %s\n", drept);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
        return 0;
}