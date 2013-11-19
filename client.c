#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

char login[20], password[20];

int main()
{
   // f = fopen("input.txt","r");
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char command[1024];

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 9734;
    len = sizeof(address);
    result = connect(sockfd,(struct sockaddr *)&address,len);
    if (result == -1)
    {
        perror("oops:client");
        exit(1);
    }

    printf("Login: \n");
    scanf("%s",login);
    printf("Password: \n");
    scanf("%s",password);

    char* message = malloc(sizeof(char)*(strlen(login)+strlen(password)));
    int i;
    for (i = 0; i < strlen(login); i++)
    {
        message[i] = login[i];
    }
    i = 0;
    int j;
    for (j = strlen(login); j< len; j++)
    {
        message[j] = password[i];
        i++;
    }
    int len1 = strlen(message);
    send(sockfd, &len1, sizeof(int),0);
    send(sockfd,message,sizeof(char)*len1,0);
    int response;
    recv(sockfd,&response,sizeof(int),0);

    if (response == 1)
    {
        printf("Success\n");
        printf(">>");
        getchar();
        while(1)
        {
            int index = 0;
            int letter;

            while((letter = getchar()) != '\n')
            {
                //letter = getchar();
                command[index] = letter;
               // printf("%s",command[index]);
                index++;
            }
            command[index] = NULL;

            printf("\n");

            int length = strlen(command);
            send(sockfd,&length,sizeof(int),0);
            send(sockfd,command,sizeof(char)*length,0);

            int n = 0;
            memset(command,0,sizeof(command));
            recv(sockfd,&n,sizeof(int),0);
            if (n == -1)
                printf("Error in command \n");
            else
            while(n > 0)
            {
                recv(sockfd,&length,sizeof(int),0);
                recv(sockfd,command,sizeof(char)*length,0);
                printf("%s \n",command);
                memset(command,0,sizeof(command));
                n--;
             }
             memset(command,0,sizeof(command));
             printf("\n\n");
             printf(">>");
        }
    }
    else
    {
        printf("Incorrect login or password\n");
    }
    close(sockfd);
    return 0;
}
