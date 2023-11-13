#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include <time.h>
#include<dirent.h>
#include<sys/wait.h>

#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016

#define BUFFERSIZE 4096
char s[250];
int fd2;
struct stat info;

void nume_fis(char dir[]){
    //scriere denumire fisier
    sprintf(s, "nume fisier: %s\n", dir);
    if(write(fd2,s,strlen(s))!=strlen(s))
    {
        printf("Error writing to file\n");
        exit(4);
    }
}

void lungime_inaltime(int fd1){
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
}

void dimensiune(){
     //dimensiune: <dimensiune in octeti>
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
}

void identificatorul(){
    //identificatorul utilizatorului: <user id>
    sprintf(s, "identificatorul utilizatorului: %d\n", info.st_uid);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
}

void timpul_ult_modif(char dir[]){
     //timpul ultimei modificari: 28.10.2023
   struct tm *timp;
   struct stat attr;
   stat(dir,&attr);
   timp=gmtime(&(attr.st_mtime));
    sprintf(s, "timpul ultimei modificari: %s",asctime(timp));
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
}

void nr_leg(){
    //contorul de legaturi: <numar legaturi>
    sprintf(s, "contorul de legaturi: %d\n", info.st_nlink);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
}

void drepturi(char *sir){
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
    sprintf(s, "drepturi de acces user%s: %s\n",sir,drept);
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
    sprintf(s, "drepturi de acces grup%s: %s\n",sir, drept);
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
    sprintf(s, "drepturi de acces altii%s: %s\n",sir, drept);
    if(write(fd2,s,strlen(s))!=strlen(s))
        {
            printf("Error writing to file\n");
            exit(4);
        }
}

void parcurgere(char *nume_dir,char *nume_dir2,int nivel){
    DIR *dir;
    struct dirent *in;
    char *nume;
    char cale[PATH_MAX],cale_link[PATH_MAX+1],spatii[PATH_MAX];
    int n;

    pid_t pid,wpid;
    int status;

    memset(spatii,' ',2*nivel);
    spatii[2*nivel]='\0';
    if(!(dir=opendir(nume_dir))){
        perror("Nu s-a deschis directorul");
        exit(1);
    }

    

    while((in=readdir(dir))>0){
        
        
        if((pid=fork())<0){
            printf("eroare creare proces fiu\n");
        }
        nume=in->d_name;
        if(strcmp(nume,".")==0||strcmp(nume,"..")==0)
                continue;
        snprintf(cale,sizeof(cale),"%s/%s",nume_dir,nume);
        if(lstat(cale,&info)<0){
                perror("eroare la lstat");
                exit(1);
        }
        if(pid==0){
            
            char s_fis[250];
            sprintf(s_fis,"%s/%s_statistica.txt",nume_dir2,nume);
            if((fd2=open(s_fis, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0)
            {
                printf("Error creating destination file\n");
                exit(3);
            }
            
            //caz director
            if(S_ISDIR(info.st_mode)){
                sprintf(s, "nume director: %s\n", nume_dir);
                if(write(fd2,s,strlen(s))!=strlen(s))
                {
                    printf("Error writing to file\n");
                    exit(4);
                }
                identificatorul();
                drepturi("");
            }
            else{
                //caz legatura
                if(S_ISLNK(info.st_mode)){
                    n=readlink(cale,cale_link,sizeof(cale_link));
                    cale_link[n]='\0';
                    char cale2[PATH_MAX+1];
                    //printf("%s %s -> %s\n",spatii,cale,cale_link);
                        snprintf(cale2,sizeof(cale2),"%s/%s",nume_dir,cale_link);
                        //printf("%s \n",cale2);
                        struct stat info2;
                        if(lstat(cale2,&info2)<0){
                            perror("eroare la lstat");
                            exit(1);
                        }
                        if(S_ISREG(info2.st_mode)){
                            sprintf(s, "nume legatura: %s\n", nume);
                        if(write(fd2,s,strlen(s))!=strlen(s))
                        {
                            printf("Error writing to file\n");
                            exit(4);
                        }
                        sprintf(s, "dimensiune legatura: %ld\n", info.st_size);
                        if(write(fd2,s,strlen(s))!=strlen(s))
                        {
                            printf("Error writing to file\n");
                            exit(4);
                        }
                    
                        sprintf(s, "dimensiune fisier: %ld\n", info2.st_size);
                        if(write(fd2,s,strlen(s))!=strlen(s))
                        {
                            printf("Error writing to file\n");
                            exit(4);
                        }
                        drepturi(" legatura");
                    }
                }
                else{
                    //caz fisier(bmp/normal)
                    if(S_ISREG(info.st_mode)){
                        //verificare fisier bmp
                        int fd1;
                        if((fd1=open(cale, O_RDONLY))<0)
                        {
                            printf("Error opening input file\n");
                            exit(2);
                        }
                        char *ext;
                        ext=strchr(nume,'.');
                        if(strcmp(ext,".bmp")!=0){
                            nume_fis(nume);
                            dimensiune();
                            identificatorul();
                            timpul_ult_modif(nume);
                            nr_leg();
                            drepturi("");
                        }
                        else{
                            nume_fis(nume);
                            lungime_inaltime(fd1);
                            dimensiune();
                            identificatorul();
                            timpul_ult_modif(nume);
                            nr_leg();
                            drepturi("");
                        }
                    }
                }
            }
             char fis[250];
             sprintf(fis,"%s_statistica.txt",nume);
             //printf("%s\n",fis);
             char *arg[]={"./","wc","-l",fis,NULL};
             execvp("./",arg);
             printf("Eroare la executie\n");
             exit(2);
        }
        else{
            wpid=wait(&status);
            if(WIFEXITED(status))
                printf("\nChild %d ended with code %d\n",wpid,WEXITSTATUS(status));
            else
                printf("\nChild %d ended abnormally\n",wpid);
            }
       
    }
    closedir(dir);
}

int main( int argc, char *argv[])
{
   

    if(argc!=3)
    {
        exit(2);
    }
   
   parcurgere(argv[1],argv[2],0);
    
    
    return 0;
}