// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
int main(int argc, char const * argv[]) {

  printf("Number of arguments : %d\n", argc);

  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  int child_uid;
  int valread;
  char * hello = "Hello from server\n";

  switch (argc) {
  case 1:
    {
      // Creating socket file descriptor 
      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed\n");
        exit(EXIT_FAILURE);
      }

      // Forcefully attaching socket to the port 8080 
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt\n");
        exit(EXIT_FAILURE);
      }
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(PORT);

      // Forcefully attaching socket to the port 8080 
      if (bind(server_fd, (struct sockaddr * ) & address,
          sizeof(address)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);
      }
      if (listen(server_fd, 3) < 0) {
        perror("listen\n");
        exit(EXIT_FAILURE);
      }
      if ((new_socket = accept(server_fd, (struct sockaddr * ) & address,
          (socklen_t * ) & addrlen)) < 0) {
        perror("accept\n");
        exit(EXIT_FAILURE);
      }

     //Forking the child process
      child_uid = fork();
      printf("child fork id is %d\n", child_uid);

      if (child_uid == 0) {
            printf(" ******* Entered Child Process ***** \n");
          /*Making the child process as a nobody user to drop privileges*/
            printf("Child before dropping privileges is : %d\n", getuid());
           int new_uid = setuid(65534);

        /*Check if there is error in dropping privileges*/
        if(new_uid == -1) {
            perror("Privilege dropping failed");
        } else if (setuid(65534) == 0) {
           //Sending socket file descriptor as args
          char socket_file_desc[12];
          //Format the socket file descriptor to store in array buffer
          snprintf(socket_file_desc, 2, "%d", new_socket);
          char * args[] = {
            "child exec process",
            socket_file_desc,
            NULL
          };

          if (execvp(argv[0], args) < 0) {

            printf("exec error\n");
            exit(0);
          }

        }

        
        
        printf(" ******* Exited Child Process ***** \n");
      }
      //if child_uid > 0 that means the control has taken by parent process 
      else if (child_uid > 0) {
        printf(" ******* Entered Parent Process ***** \n");
         printf(" wait for child process termination \n");
        wait(NULL);
        printf(" ******* Exit Parent Process ***** \n");

      } 
      // Handling fork failure 
      else {
        perror("Fork failure\n");
      }
      return 0;

    }
    break;
  case 2:
    {
      //Using the socket file descriptor from the forked child  
      printf("Re-execing the child\n");
      int reader;

      //Reading the formatted data
      sscanf(argv[1], "%d", & reader);

      printf("Socket obtained %d\n", reader);
      valread = read(reader, buffer, 1024);
      printf("%s\n", buffer);
      send(reader, hello, strlen(hello), 0);
      printf("Hello message sent\n");
      exit(1);


     

    }
    break;

  default:
    break;

  }
}