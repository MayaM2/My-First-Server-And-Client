# My First Server And Client
Includes two programs:
1. pcc_server: The server accepts TCP connections from clients. A client that connects sends the server a finite stream of bytes. The server counts the number of printable characters in the stream (a printable character is a byte whose value is in the range [32,126]). Once the stream ends, the server sends the count back to the client over the same connection.
In addition, the server maintains a data structure in which it counts the number of times each printable character was observed in all the connections. When the server receives a SIGINT, it prints these counts and exits.

2. pcc_client: The client creates a TCP connection to the server and sends it N bytes read from /dev/urandom, where N is a user-supplied argument. The client then reads back the count of printable characters from the server, prints it, and exits.


## Client Specification: 
Command line arguments:
1. Server ip: Assumes it is a valid IP address.
2. Server port: Assume it is a 16-bit unsigned integer.
3. Length: The number of bytes to read from /dev/urandom and send to the server. Assume it is an unsigned integer.


## Server Specification:
command line arguments:
1. Server port: Assume this is a 16-bit unsigned integer.
