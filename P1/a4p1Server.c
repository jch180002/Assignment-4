#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

FILE *log;
pthread_mutex_t logLock;
struct sockaddr_in myAddr; 
char *fileName; 

struct serverParm 
{ // begin serverParm
	int connectionDesc;
}; // end serverParm

char* concatenate(char *command)
{ // begin concatenate
	const char *locator = "which ";
	char *result = malloc(strlen(locator) + strlen(command) + 1); // Find length required for the result

	memset(result,'\0',strlen(result)); // Set contents to null
	strcpy(result,locator); // Copy contents of locator to result
	strcat(result,command); // Concatenate command name to the end of result
	return result; // Return the result
} // end concatenate

void *serverThread(void *parmPtr)
{ // begin serverThread

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];
    int fd;

    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n", PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) 
    { // begin if
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    } // end if

    /* Receive messages from sender... */
    while ((recievedMsgLen = read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0) 
    { // begin while
        recievedMsgLen[messageBuf] = '\0';

		char *line = malloc(strlen(messageBuf) + 1); // Find length required for copy
		strcpy(line,messageBuf); // Make copy of messageBuf to check for valid command
		char *command = strtok(line," "); // Get command from line 
		// Concatenate desired command to use with which to verify validity of input (basically check if a real command)
		char *which = concatenate(command); 

		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); // Readability
		printf("Thread ID: %d Port Number: %d\n",pthread_self(),&myAddr.sin_port); // Display thread ID and server information
		printf("----------------------------------------------------------------------------------------------------\n"); // Readability
		system("hostname;date"); // Display hostname and date
		printf("----------------------------------------------------------------------------------------------------\n"); // Readability
		printf("Message: %s\n",messageBuf); // Display commandline given to server by client
		system(messageBuf); // Show run of command in server
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); // Readability
		
		
		// Verify existence of command
		if (system(which) != 0)
		{ // begin if
			fd = dup(1); // File descriptor stdout
			dup2(PARMPTR->connectionDesc,1); // Set stdout to socket descriptor
			printf("Please input a valid command.\n");
			dup2(fd,1); // Set file descriptor back to stdout
		} // end if

		fd = dup(1); // File descriptor stdout
		dup2(PARMPTR->connectionDesc,1); // Set stdout to socket descriptor
		system(messageBuf); // Run command 
		dup2(fd,1); // Set file descriptor back to stdout

		// Log transactions
		pthread_mutex_lock(&logLock); // Lock before editing the log file
		int tfD = dup(1); // File descriptor stdout
		dup2(fileno(log),1); // Set standard out to file descriptor
		// Place in log file to increase readability of transactions
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"); 
		printf("Thread ID: %d Port Number: %d\n",pthread_self(),&myAddr.sin_port); // Display thread ID and server information
		printf("----------------------------------------------------------------------------------------------------\n"); // Readability
		system("hostname;date"); // Display hostname and date
		printf("----------------------------------------------------------------------------------------------------\n"); // Readability
		printf("Message: %s\n",messageBuf); // Display commandline given to server by client
		system(messageBuf); // Show run of command in log
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"); // Readability
		dup2(tfD,1); // Set file descriptor back to standard out
		pthread_mutex_unlock(&logLock); // Unlock so other threads can edit the log file

		memset(messageBuf,'\0',sizeof(messageBuf));
    } // end while

    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);                   /* And memory leaks */
    return(0);                       /* Exit thread */
} // end serverThread

int main (int argc, char** argv) 
{ // begin main
    int listenDesc;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;
    fileName = argv[2]; // Get filename of log
    pthread_mutex_init(&logLock,NULL); // Initialize mutex logLock

    /* For testing purposes, make sure process will terminate eventually */
    alarm(120);  /* Terminate in 120 seconds */

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { // begin if
        perror("open error on socket");
        exit(1);
    } // end if

    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons((int) strtol(argv[1],(char**)NULL,10));
        
    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) 
    { // begin if
        perror("bind error");
        exit(1);
    } // end if

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc,5);

    log = fopen(fileName,"w+"); // Open the log file

    while (1) /* Do forever */ 
    { // begin while
        /* Wait for a client connection */
        connectionDesc = accept(listenDesc, NULL, NULL);

        /* Create a thread to actually handle this client */
        parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
        parmPtr->connectionDesc = connectionDesc;
        if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr) != 0) 
		{ // begin if
            perror("Thread create error");
            close(connectionDesc);
            close(listenDesc);
            exit(1);
        } // end if

        printf("Parent ready for another connection\n");
    } // end while
    
    fclose(log); // Close log file
} // end main
