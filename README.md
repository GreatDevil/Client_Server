# Client_Server

#### A console application (server and client side) that runs in a LINUX environment at the user level.

#  
  
## Installation  

make  

~~~html
run client    ./client
~~~  
~~~html
run server    ./server
~~~ 

## Description

The server part generates a sequence of numbers consisting of subsequences (the subsequence is set by the client). Each of the subsequences is an integer non-negative 64-bit counter, which is specified by the initial value and step.  The generated sequence is sent to the client.  
The server can serve multiple clients

#### commands accepted by the client to send a request to the server


*seq* 1 xxxx yyyy (set initial value = xxxx and step = yyyy for the first subsequence);

*seq2* xxxx yyyy (set initial value = xxxx and step = yyyy for the second subsequence);

*seq* 3 xxxx yyyy (set initial value = xxxx and step = yyyy for the third subsequence);

*exportseq* - issue the generated sequence to the socket for each client.  
  
  
#  

Client:
- Exit by 'q' or ctrl + c
- Limit on the number of clients in the queue entering commands-5 clients
- Limit on the number of clients receiving sequences-the program is not limited
- Delay of incoming numbers (for better visibility of what is happening) - 25000 MS, starting with " num: "
- Input errors are displayed in stdout  
######  
######  

Server:
- Exit by ctrl +c
- Joins, detaches, and waits are displayed on the screen
