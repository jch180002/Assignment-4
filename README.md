# Assignment-4
A concurrent server that waits for any client to connect and send a series of transactions. 
When a client is connected to the server, the server will create a worker thread to serve the connected client. 
The client will take user inputs for shell commands and then send them to the server to then be run through 
the worker thread and have the result sent back to the client for display. 
The client takes 3 arguments (IP address, port number, and log file name). 
The server takes 2 arguments (the port number and log file name). 
The server and client will take these log file names, create them and log all transactions in them.
(These log files should be .txt files)
