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
#include <errno.h>


#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22
#define HEADER_SIZE 54

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
    sprintf(s, "contorul de legaturi: %ld\n", info.st_nlink);
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

//, char *output_path
void process_image(char *input_path) {
    int input_fd;
    // output_fd;

    if ((input_fd = open(input_path, O_RDWR)) < 0) {
        perror("Error opening input file");
        exit(2);
    }

    // if ((output_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
    //     perror("Error opening output file");
    //     exit(3);
    // }

    // unsigned char header[HEADER_SIZE];
    // if (read(input_fd, header, HEADER_SIZE) != HEADER_SIZE) {
    //     perror("Error reading header");
    //     exit(4);
    // }

    // if (write(input_fd, header, HEADER_SIZE) != HEADER_SIZE) {
    //     perror("Error writing header");
    //     exit(5);
    // }

    int width, height;
    lseek(input_fd, WIDTH_OFFSET, SEEK_SET);
    read(input_fd, &width, sizeof(int));
    lseek(input_fd, HEIGHT_OFFSET, SEEK_SET);
    read(input_fd, &height, sizeof(int));

    unsigned char pixel[3];
   //lseek(input_fd, HEADER_SIZE, SEEK_SET);
    int data_offset;
    lseek(input_fd,10,SEEK_SET);
    read(input_fd,&data_offset,sizeof(int));
    lseek(input_fd,data_offset,SEEK_SET);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Citirea culorilor pixelului
            if (read(input_fd, pixel, sizeof(pixel)) != sizeof(pixel)) {
                perror("Error reading pixel");
                exit(6);
            }
            
            // Calculul culorii gri
           lseek(input_fd,-3,SEEK_CUR);

            unsigned char gray = 0.30 * pixel[0] + 0.59 * pixel[1] + 0.11 * pixel[2];

            
            // Setarea culorii gri pentru toate cele trei canale de culoare
            pixel[0] = pixel[1] = pixel[2] = gray;

            // Scrierea pixelului modificat în fișierul de destinație
            if (write(input_fd, pixel, sizeof(pixel)) != sizeof(pixel)) {
                perror("Error writing pixel");
                exit(7);
            }
            lseek(input_fd,3-sizeof(unsigned char)*3,SEEK_CUR);
        }
        //Dacă lățimea imaginii nu este multiplu de 4, adăugați octeți de umplere
        // int padding = 4 - (width * 4) % 4;
        // for (int k = 0; k < padding; k++) {
        //     char dummy;
        //     read(input_fd, &dummy, sizeof(char));
        //     write(input_fd, &dummy, sizeof(char));
        // }
    }

    // Închideți fișierele
   // close(input_fd);
    //close(output_fd);
}

void parcurgere(char *nume_dir,char *nume_dir2,int nivel){
    DIR *dir;
    struct dirent *in;
    char *nume;
    char cale[PATH_MAX],cale_link[PATH_MAX+1],spatii[PATH_MAX];
    int n;

    //int pfd[2];
    pid_t pid,wpid,pid2,pid3;
    int status;

    memset(spatii,' ',2*nivel);
    spatii[2*nivel]='\0';
    if(!(dir=opendir(nume_dir))){
        perror("Nu s-a deschis directorul");
        exit(1);
    }
     char s_executie[250];
     sprintf(s_executie,"statistica.txt");
             int fd3;
            if((fd3=open(s_executie, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0)
            {
                printf("Error creating destination file\n");
                exit(3);
            }

    while((in=readdir(dir))>0){
        
        // if(pipe(pfd)<0){
        //     printf("eroare la creare pipe-ului\n");
        // }
        //creare primul fiu
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
        //primul fiu
        if(pid==0){
            
            //close(pfd[0]);
            char s_fis[250];
            sprintf(s_fis,"%s/%s_statistica.txt",nume_dir2,nume);
            if((fd2=open(s_fis, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) < 0){
                printf("Error creating destination file\n");
                exit(3);
            }
           
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
                        //caz fisier normal
                        if(strcmp(ext,".bmp")!=0){
                            nume_fis(nume);
                            dimensiune();
                            identificatorul();
                            timpul_ult_modif(nume);
                            nr_leg();
                            drepturi("");

                            


                        }
                        //caz ffisier bmp
                        else{
                            nume_fis(nume);
                            lungime_inaltime(fd1);
                            dimensiune();
                            identificatorul();
                            timpul_ult_modif(nume);
                            nr_leg();
                            drepturi("");
                            if((pid2=fork())<0){
                                    printf("eroare creare proces fiu\n");
                            }
                            
                            if(pid2==0){
                               process_image(cale);

                                exit(0);
                            }
                        }
                        close(fd1);
                    }
                }
             char fis[250];
             sprintf(fis,"%s/%s_statistica.txt",nume_dir2,nume);
             //printf("%s\n",fis);
        }
            
              char *arg[]={"-l",s_fis,NULL};
             dup2(fd3,1);
             execvp("wc",arg);
             printf("Eroare la executie\n");
             exit(2);
            //caz director
            
             
        }
        else{

            //close(pfd[1]);
            //FILE *stream=fdopen(pfd[0],"r");
            //char *string;
            //fscanf(stream,"%s",string);
            wpid=wait(&status);
            if(WIFEXITED(status)){
                printf("S-a incheiat procesul cu pid-ul %d si codul %d\n",wpid,WEXITSTATUS(status));
            }
            else
                printf("\nChild %d ended abnormally\n",wpid);
            close(pid);
            close(pid2);
            //close(pfd[0]);
            }
       
    }
    closedir(dir);
    close(fd3);
    //close(fd1);
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