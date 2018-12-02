#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>


#define MAXLINE 4096 /*max text line length*/

void info(char *str)
{ // begin info 
	printf("------------------------------------------------------------------------------------------\n");
	printf("%s",str);
	system("hostname;date");
	printf("------------------------------------------------------------------------------------------\n");
} // end info

int main(int argc, char **argv) 
{ // begin main
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];

	alarm(300);  // to terminate after 300 seconds
	
	//basic check of the arguments
	//additional checks can be inserted
	if (argc != 4)  
	{ // begin if
		perror("Usage: TCPClient <Server IP> <Server Port> <log-file name>\n"); 
		exit(1);
	} // end if

	//Create a socket for the client
	//If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
	{ // begin if
		perror("Problem in creating the socket\n"); // Failed to create the socket
		exit(2);
	} // end if
	
	//Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr= inet_addr(argv[1]);
	servaddr.sin_port =  htons((int) strtol(argv[2],(char**) NULL,10)); // convert to big-endian order 
	
	//Connection of the client to the socket 
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) 
	{ // begin if
		perror("Problem in connecting to the server\n"); // Failure to connect to server
		exit(3);
	} // end if

 	// Display hostname and date of the client upon verification of arguments and successful connection to the server
 	info("Successful connection.\n");
 	printf("\n"); // Newline for readability

	// Begin logging of run of client
	FILE *log;
	char *fileName = argv[3]; // Get filename of log file form argument passed
	
	log = fopen(fileName,"w+"); // Open the file

 	// Loop through as long as the user supplies input
 	while (fgets(sendline, MAXLINE, stdin) != NULL) 
 	{ // begin while
   
		// Disconnect from the server and exit the client upon reception of the string "exit"
		if (strncmp(sendline,"exit",4) == 0 || strncmp(sendline,"Exit",4) == 0)
		{ // begin if
			info("Disconnecting from server.\n"); // Display hostname and date of the client
			printf("\n"); // Newline for readability of console
			exit(4);
		} // end if
		
		info("^String to be sent.\n"); // Display hostname and date of the client
		printf("\n"); // Newline for readability
  		send(sockfd, sendline, strlen(sendline), 0); // Send line to the server
  		info("Message sent to the server.\n"); // Display hostname and date of the client
  		printf("\n"); // Newline for readability of console

  		// Verify reception of line
  		if (recv(sockfd, recvline, MAXLINE,0) == 0)
  		{ // begin if
			//error: server terminated prematurely
   			info("The server terminated prematurely\n"); // Display hostname and date of the client
   			exit(5);
  		} // end if 
 
  		info("A response from the server has arrived.\n"); // Display hostname and date of the client
  		printf("%s", "Response received from the server: \n"); // Display the string that the server received
  		fputs(recvline, stdout);
  		printf("\n"); // Newline for readability of console
  		info("hostname and date info at receipt/display of string from the server.\n");
  		printf("\n"); // Newline for readability of console
  		info("Please provide new input, or type exit to quit.\n"); // Display hostname and date of the client
		printf("\n"); // Newline for readability of console
   
		// Log transactions through I/O redirection
		int fd = dup(1); // File descriptor stdout
		dup2(fileno(log),1); // Set stdout to file descriptor
		// Place in log file separate transactions for readability
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		info("Info on transaction\n"); // Print hostname and date in the file (^_^)	
		printf("Input from user: %s\n", sendline); // Place user input sent to server in log file
  		printf("Result from server:\n%s\n", recvline); // Put result from transaction into log file
		// Place in log file to separate transactions for readability
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    	dup2(fd,1); // Set file descriptor back to stdout	
    
		memset(recvline,'\0',sizeof(recvline)); // Set contents to null
 	} // end while
 
 	fclose(log); // Close the file
 	exit(0);
} // end main
