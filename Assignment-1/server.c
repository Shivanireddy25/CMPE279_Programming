// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
int main(int argc, char const *argv[]) 
{ 

    //Set up socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    pid_t child_uid;
    int  valread;
    char *hello = "Hello from server"; 

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    
    /* Fork a child process */
    child_uid = fork();
    
    /* if child_uid is 0 then the current running process is child process*/
    if(child_uid == 0){
        printf(" ******* Entered Child Process ***** \n");
        
        /*Making the child process as a nobody user to drop privileges*/
        printf("Child before dropping privileges is : %d\n", getuid());
       int new_uid = setuid(65534);

        /*Check if there is error in dropping privileges*/
        if(new_uid == -1) {
            perror("Privilege dropping failed");
        } else {
            printf("Child after dropping privileges is : %d\n", getuid());
        }

        valread = read( new_socket , buffer, 1024); 
        printf("%s\n",buffer ); 
        send(new_socket , hello , strlen(hello) , 0 ); 
        printf("Hello message sent\n"); 
        printf(" ******* Exit Child Process ***** \n");

    } 
    /* if child_uid > 0 that means the control has taken by parent process */
    else if (child_uid > 0) {
        printf(" ******* Entered Parent Process ***** \n");
        /*Block Parent process untill all child processes complete */
        wait(NULL);
        printf(" ******* Exit Parent Process ***** \n");
        

    } else {
        perror("Fork failure");
    }

    return 0; 
} 
