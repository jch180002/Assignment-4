all: a4p1Server a4p1Client1
	
a4p1Server: a4p1Server.c 
	gcc a4p1Server.c -o a4p1Server -lnsl -lpthread

a4p1Client1: a4p1Client1.c
	gcc a4p1Client1.c -o a4p1Client1

clean: 
	rm a4p1Client1 a4p1Server

