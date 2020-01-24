// Author: Sydney Taylor

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void chat(int);
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, pid;
     struct sockaddr_in serv_addr, cli_addr;
     // If no port is provided, print an error message
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     // If the socket did not open, print an error message
     // Else, print a message that it was created
     if (sockfd < 0) {
        error("ERROR opening socket");
     } else {
	printf("Socket created\n");
     } 
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     // If the bind did not occur, print an error message
     // Else, print a message that the bind was completed
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
     } else {
	printf("Bind complete\n");
     }
     printf("Waiting for incoming connection\n");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     // While the socket is running, fork for each new process
     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
	 // If the new socket was not accepted, print an error message
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
	 // If the fork did not work, print an error message
         if (pid < 0)
             error("ERROR on fork");
	 // If the fork did work, call chat on the new socket
         if (pid == 0)  {
	     printf("Connection created with client\n");
             close(sockfd);
             chat(newsockfd);
	     printf("Waiting for incoming connection\n");
             exit(0);
         }
	 // Else, close the new socket
         else {
		 close(newsockfd);
	 }
     }
     return 0;
}

// Chat with a client
void chat (int sock)
{
   int n, m;
   char n_buffer[256], m_buffer[256];
   // While the client is connected
   while (strncmp(n_buffer,"quit",4) != 0) {
     printf("Your message: ");
     bzero(m_buffer,256);
     fgets(m_buffer,255,stdin);
     m = write(sock,m_buffer,strlen(m_buffer));
     // If the server cannot write to the socket, print an error message
     // Else, print a message that the server wrote to the client
     if (m < 0) {
        error("ERROR writing to socket");
     } else {
        printf("Mesage sent to client\n");
     }
     printf("Waiting for client...\n");
     bzero(n_buffer,256);
     n = read(sock,n_buffer,255);
     // If the server cannot read from the socket, print an error message
     if (n < 0) error("ERROR reading from socket");
     // If the client ended the connection, print a connection ended message
     // Else, print the message from the client
     if (strncmp(n_buffer,"quit",4) == 0) {
	printf("Client ended connection\n");
     	break;
     } else {
	printf("Client: %s",n_buffer);
     }
   }
}
