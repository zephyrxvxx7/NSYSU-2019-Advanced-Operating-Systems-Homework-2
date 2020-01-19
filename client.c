#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STRING_MAX 50

int recv_result(int sockfd){
    char result[STRING_MAX];
    char result_string[STRING_MAX];

    recv(sockfd, result, STRING_MAX, 0);
    recv(sockfd, result_string, STRING_MAX, 0);

    printf("%s\n", result_string);

    if(strcmp(result, "1") == 0)
        return 0;
    else
        return -1;
}

int main(){
    int sockfd = 0;
    struct sockaddr_in info;

    bzero(&info,sizeof(info));
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8787);

    FILE *fp;

    char operation[STRING_MAX];
    char filename[STRING_MAX];
    char auth[STRING_MAX];
    char buffer[STRING_MAX];

    char username[STRING_MAX];
    char password[STRING_MAX];

    while(1){
        if((sockfd = socket(PF_INET , SOCK_STREAM , 0)) == -1)
            printf("Fail to create a socket.");
        
        if(connect(sockfd,(struct sockaddr *)&info,sizeof(info)) == -1)
            printf("Error\n");

        printf("Please Enter your username: ");
        scanf("%s", username);
        printf("Please Enter your password: ");
        scanf("%s", password);

        send(sockfd, username, STRING_MAX, 0);
        send(sockfd, password, STRING_MAX, 0);

        if(recv_result(sockfd) == 0)
            break;
    }
    

    while(1){
        printf("Please Enter operation: ");
        scanf("%s %s", operation, filename);
        
        if(strcmp(operation, "exit") == 0)
            exit(0);
        
        send(sockfd, operation, STRING_MAX, 0);
        send(sockfd, filename, STRING_MAX, 0);

        if(strcmp(operation, "create") == 0){
            scanf("%s", auth);
            send(sockfd, auth, STRING_MAX, 0);
            recv_result(sockfd);
        }
        else if(strcmp(operation, "read") == 0){

            if(recv_result(sockfd) == 0){
                fp = fopen(filename, "wb");

                recv(sockfd, buffer, STRING_MAX, 0);
                int file_size = atoi(buffer);
                
                size_t string_size = 0;
                size_t count_size = 0;
                while(count_size != file_size){
                    string_size = recv(sockfd, buffer, STRING_MAX, 0);
                    fwrite(buffer, 1, string_size, fp);

                    count_size += string_size;
                    bzero(buffer, sizeof(buffer));
                }

                fclose(fp);
                printf("Transfer completed.\n");
            }
        }
        else if(strcmp(operation, "write") == 0){
            scanf("%s", auth);
            send(sockfd, auth, STRING_MAX, 0);

            if(recv_result(sockfd) == 0){
                fp = fopen(filename, "rb");

                int file_size = 0;

                fseek(fp, 0, SEEK_END);
                file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                sprintf(buffer, "%d", file_size);

                send(sockfd, buffer, STRING_MAX, 0);
                
                size_t string_size = 0;
                while((string_size = fread(buffer, 1, sizeof(buffer), fp)) > 0){ 
                    send(sockfd, buffer, string_size, 0);

                    bzero(buffer, sizeof(buffer));
                }

                fclose(fp);
                printf("Transfer completed.\n");
            }
            
        }
        else if(strcmp(operation, "changemode") == 0){
            scanf("%s", auth);
            send(sockfd, auth, STRING_MAX, 0);
            recv_result(sockfd);
        }
        else{
            char ch;
            while ((ch = getchar()) != EOF && ch != '\n');
            // fflush(stdin);
        }
    }


}