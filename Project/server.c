// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#define PORT 8080
#define MAX 3

char *hello = "Hello from server";
int thread_counter = 0;
sem_t mutex;


void *message(int ns){
	int valread;
	char buffer[1024] = {0};
	char b[] = "-1";
	while(1){
		valread = read( ns , buffer, 1024);
		if(strcmp(buffer, b) == 0){
			sem_post(&mutex);
			thread_counter--;
			pthread_exit(NULL);
		}else{
			printf("%s\n",buffer );
			send(ns , hello , strlen(hello) , 0 );
			printf("Hello message sent\n");
		}
	}
}

int main(int argc, char const *argv[])
{
	int server_fd;
	int new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	pthread_t thread_array[MAX];
	//int thread_int_array[MAX];
	

	//semaphore
	if ((sem_init(&mutex, 0, MAX) == -1)){
		perror("semaphore failed");
		exit(EXIT_FAILURE);
	}

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while(thread_counter<=MAX){
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		if (new_socket < 0){
			perror("accept");
			exit(EXIT_FAILURE);
		}
	
		pthread_create(&(thread_array[thread_counter]), NULL,  message, new_socket);

		sem_wait(&mutex);

		thread_counter++;
	}



	return 0;
}

