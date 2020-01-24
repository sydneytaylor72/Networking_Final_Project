// Author: Sydney Taylor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n, m;
 
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char n_buffer[256], m_buffer[256];
    // If the user did not provide the host name or port, print an error message
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // If the socket cannot be opened, print an error message
    // Else, print a message that the socket was created
    if (sockfd < 0) { 
        error("ERROR opening socket");
    } else {
	printf("Socket created\n");
    }
    printf("Connecting to server...\n");
    server = gethostbyname(argv[1]);
    // If the server does not exist, print an error message
    // Else, print that the client is connected to the server
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    } else {
	printf("Connected to server...\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // If the connection did not work, print an error message
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    } else {
	// While the client is connected
	while (strncmp(n_buffer,"quit",4) != 0) {
	  printf("Waiting for server...\n");
	  bzero(m_buffer,256);
    	  m = read(sockfd,m_buffer,255);
	  // If the client cannot read from the socket, print an error message
    	  if (m < 0)
             error("ERROR reading from socket");
	  printf("Server: ");
	  printf("%s",m_buffer);
	  printf("Your message: ");
   	  bzero(n_buffer,256);
    	  fgets(n_buffer,255,stdin);
    	  n = write(sockfd,n_buffer,strlen(n_buffer));
	  // If the client cannot write to the socket, print an error message
    	  if (n < 0) {
            error("ERROR writing to socket");
	  }
	  // If the client is connected, the message is sent to the server
	  if (strncmp(n_buffer,"quit",4) != 0) {
	    printf("Message sent to server\n");
	  }
	}
    }
    // Close the socket
    close(sockfd);
    printf("Closing connection...\n");
    return 0;
}
