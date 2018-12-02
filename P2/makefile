all: a4p2Server a4p2Client
	
a4p2Server: a4p2Server.c 
	gcc a4p2Server.c -o a4p2Server -lnsl -lpthread

a4p2Client: a4p2Client.c
	gcc a4p2Client.c -o a4p2Client

clean: 
	rm a4p2Client a4p2Server

