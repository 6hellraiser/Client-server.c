#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>
#define SEM_NAME "mysem"

int filedes[2];
char buf[1024];
char buf2[1024];
sem_t mutex;
int cnt = 0;

int CheckLogin(char* str)
{
    FILE* file;
    char correct_data[30];
    file = fopen("input.txt", "rt");
    fscanf(file,"%s", correct_data);
    int i = 0;
    if (strlen(str) != strlen(correct_data))
        return 0;
    else
    {
        int flag = 1;
        for (;i< strlen(correct_data);i++)
        {
            if (str[i] != correct_data[i])
                flag = 0;
        }
        if (flag)
            return 1;
        else
            return 0;
    }
    fclose(file);
}


int main()
{
    printf("Hello world!\n");
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    //??<
    int auth = 0;
    int client_counts;
    cnt = (int)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    cnt = 0;
    client_counts = 0;
    //>
    server_sockfd = socket(AF_INET,SOCK_STREAM,0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr =  inet_addr("127.0.0.1");
    server_address.sin_port = 9734;
    server_len = sizeof(server_address);

    bind(server_sockfd,(struct sockaddr *)&server_address,server_len);

    listen(server_sockfd,5);//second argument-?
    signal(SIGCHLD,SIG_IGN);
    pipe(filedes);
   // mutex = sem_open(Px_ipc_name(SEM_NAME), O_CREAT | O_EXCL);
    sem_init(&mutex,1,1);
    //sem_open(&mutex, O_CREAT | O_EXCL);
   // sem_unlink(Px_ipc_name(SEM_NAME));
     setbuf(stdout, NULL);
   // signal(SIGCHLD, SIG_IGN); //-??


    while(1){
        printf("server waiting...\n");
        client_sockfd = accept(server_sockfd, NULL, NULL);
        int flag = 1;

        if (client_counts < 64)
        {
            int pid = fork();
            if (pid == -1)
            {
                perror("fork");
                flag = 0;
            }

            if (pid == 0)
            {
               // close(server_sockfd);
                sem_wait(&mutex);
              //  client_counts++;
                cnt++;
                printf("Clients count: %d \n",cnt);
                printf("sth has changed\n");
                sem_post(&mutex);
                int len;
                recv(client_sockfd, &len, sizeof(int), 0);
                recv(client_sockfd,buf,sizeof(char)*len,0);

                if (CheckLogin(buf))
                    auth = 1;
                send(client_sockfd, &auth,sizeof(int),0);

                if (auth == 1)
                {
                    while(1)
                    {
                        memset(buf,0,sizeof(buf));
                        recv(client_sockfd,&len,sizeof(int),0);
                        recv(client_sockfd,buf,sizeof(char)*len,0);

                      /*  if (CompareStr(buf,"close_client"))
                        {
                            printf("Client disconnected \n");
                            break;
                        }*/

                        FILE* result;
                        result = popen(buf,"r");
                        if (!result)
                        {
                            int n = -1;
                            send(client_sockfd,&n,sizeof(int),0);
                            printf("can't execute command \n");
                        }
                        else
                        {
                            int n = 0;
                            while (fgets(buf2,sizeof(buf2),result) != NULL)
                                n++;
                            send(client_sockfd,&n,sizeof(int),0);
                            //printf("num: %d \n",n);
                            if (n != 0)
                            {
                                result = popen(buf,"r");
                                while (fgets(buf2,sizeof(buf2),result) != NULL)
                                {
                                    len = strlen(buf2);
                                    send(client_sockfd,&len,sizeof(int),0);
                                    send(client_sockfd,buf2,sizeof(char)*len,0);
                                   // printf("%s \n",buf2);
                                }
                            }
                        }
                    }
                }
                close(filedes[0]);
                pid_t pid = getpid();
                write(filedes[1], &pid, sizeof(pid_t));
                close(filedes[1]);
                close(client_sockfd);
                _exit(0);
            }
            else
            {
                if (fork() == 0)
                {
                    close(filedes[1]);
                    pid_t pid;
                    read(filedes[0], &pid, sizeof(pid));
                    client_counts--;
                   close(client_sockfd);
                   _exit(1);
                }
            }
        }

       /* char ch;
        printf("server waiting\n");
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address,
                               &client_len);
        if (fork() == 0)
        {
            read(client_sockfd,&ch,1);
            sleep(5);
            ch++;
            write(client_sockfd,&ch,1);
            close(client_sockfd);
            exit(0);
        }
        else
        {
            close(client_sockfd);
        }*/
    }
    close(server_sockfd);
}
