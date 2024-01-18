#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#define PORT 12345

int main() {
    int server_socket, client_socket, addr_len;
    struct sockaddr_in server_addr, client_addr;
    int num_ints = 0;
    int fpga_address = 0;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 1) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connections on port %d...\n", PORT);

    while (1) {
        // Accept connection
        addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("Client connected\n");

        // accept array length in int32
        int received1 = recv(client_socket, &num_ints, sizeof(int), 0);
		if (received1 == -1) {
			perror("Error receiving array length");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		signed int *myNumbers = malloc(num_ints * sizeof(int));

//		signed int myNumbers[num_ints];
		int buffer_size = num_ints*sizeof(int); // and calculate what that would mean in bytes

        // accept fpga base address
		int received2 = recv(client_socket, &fpga_address, sizeof(int), 0);
		if (received2 == -1) {
			perror("Error receiving fpga base address");
			close(client_socket);
			exit(EXIT_FAILURE);
		}

		off_t fpga_address_format = (off_t)(unsigned int)fpga_address; //change to correct format

		// Receive data and save to int array
		int received = recv(client_socket, myNumbers, buffer_size, MSG_WAITALL);
		if (received == -1) {
			perror("Error receiving data");
		}
		else {
			printf("received %d bytes of data\n", received);
		}
//		// print what was accepted
//		for (int i = 0; i < num_ints; i++) {
//			printf("%x\n", myNumbers[i]);
//		}

		// open mmap
		int fd = open("/dev/mem", O_RDWR | O_SYNC);
		if (fd == -1) {
			perror("Error opening /dev/mem");
		}

		// find the base address
		signed int *axi_base = mmap(NULL, 16384, PROT_READ | PROT_WRITE, MAP_SHARED, fd, fpga_address_format);

	   if (axi_base == MAP_FAILED) {
		   perror("Error mapping /dev/mem");
		   close(fd);
	   }


	 //   write to memory
	   for (int i = 0; i < num_ints; i++) {
		   axi_base[i]=myNumbers[i];
			}

	   //   check memory

	   int counter = 0;
	   for (int i = 0; i < num_ints; i++) {
		   if (axi_base[i]!=myNumbers[i]) {
			   counter+=1;
			   printf("idx wrong: %d\n",i);
			   printf("saved : %d ",axi_base[i]);
			   printf("and not : %d\n",myNumbers[i]);
		   }
			}
	   printf("\ncounter wrong: %d\n",counter);

	  //  close mmap
	   int err = munmap(axi_base, 16384);
		 if (err != 0) {
		   printf("Un-mapping Failed\n");
		 }

	   close(fd);
		printf("\n");

		// close allocated memory
		free(myNumbers);


        // Close client socket
        close(client_socket);
        printf("Client disconnected\n");
    }

    // Close server socket (never reached in this example)
    close(server_socket);

    return 0;
}
