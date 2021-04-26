#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>

#include "strBST.c"
#include "strbuf.c"

#define BACKLOG 5

// the argument we will pass to the connection-handler threads
struct connection {
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

typedef struct args_t{
    struct connection* con;
    BST* tree;
}args_t;

int server(char *port);
void *echo(void *arg);

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    (void) server(argv[1]);
    return EXIT_SUCCESS;
}


int server(char *port)
{
    struct addrinfo hint, *info_list, *info;
    struct connection *con;
    int error, sfd;
    pthread_t tid;
    
    
    args_t* args = malloc(sizeof(args_t));
    args->tree = newBST();
    args->con = con;
    

    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    	// setting AI_PASSIVE means that we want to create a listening socket

    // get socket and address info for listening port
    // - for a listening socket, give NULL as the host name (because the socket is on
    //   the local host)
    error = getaddrinfo(NULL, port, &hint, &info_list);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (info = info_list; info != NULL; info = info->ai_next) {
        sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        
        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        // 
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, info->ai_addr, info->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (info == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(info_list);

    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
    for (;;) {
    	// create argument struct for child thread
		con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
        	// addr_len is a read/write parameter to accept
        	// we set the initial value, saying how much space is available
        	// after the call to accept, this field will contain the actual address length
        
        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
        	// we provide
        	// sfd - the listening socket
        	// &con->addr - a location to write the address of the remote host
        	// &con->addr_len - a location to write the length of the address
        	//
        	// accept will block until a remote host tries to connect
        	// it returns a new socket that can be used to communicate with the remote
        	// host, and writes the address of the remote hist into the provided location
        
        // if we got back -1, it means something went wrong
        if (con->fd == -1) {
            perror("accept");
            continue;
        }

		// spin off a worker thread to handle the remote connection
        args->con = con;
        //error = pthread_create(&tid, NULL, echo, con);
        error = pthread_create(&tid, NULL, echo, args);


		// if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0) {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

		// otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);
    }

    // never reach here
    return 0;
}

#define BUFSIZE 8

enum request{GET, SET, DEL};

void *echo(void *arg)
{
    char host[100], port[10], buf[BUFSIZE + 1];
    //struct connection *c = (struct connection *) arg;
    args_t* args = (args_t*) arg;
    struct connection *c = args->con;

    int error, nread;
    enum request req;
    int field = 0;
    int byteSize;
    int currByteCount = 0;
    bool isFailed = false;
    strbuf_t* readBuf = malloc(sizeof(strbuf_t));
    strbuf_t* field1 = malloc(sizeof(strbuf_t));
    strbuf_t* field2 = malloc(sizeof(strbuf_t));
    strbuf_t* field3 = malloc(sizeof(strbuf_t));


    sb_init(readBuf, 10);
    sb_init(field1, 10);
    sb_init(field2, 10);
    sb_init(field3, 10);
    
	// find out the name and port of the remote host
    error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
    	// we provide:
    	// the address and its length
    	// a buffer to write the host name, and its length
    	// a buffer to write the port (as a string), and its length
    	// flags, in this case saying that we want the port as a number, not a service name
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
        close(c->fd);
        return NULL;
    }

    printf("[%s:%s] connection\n", host, port);

    nread = read(c->fd, buf, 3);
    buf[nread] = '\0';

    if(strcmp(buf, "GET") == 0){
        req = GET;      //0
    }
    else if(strcmp(buf, "SET") == 0){
        req = SET;      //1
    }
    else if(strcmp(buf, "DEL") == 0){
        req = DEL;      //2
    }
    else{
        req = -1;
    }

    printf("Request type: %d\n", req);

    if(req != -1){      //If a valid request was made
        while ((nread = read(c->fd, buf, BUFSIZE)) > 0) {
            buf[nread] = '\0';

            for(int i = 0; i < nread; i++){
                if(buf[i] != '\n'){
                    sb_append(readBuf, buf[i]);
                    currByteCount++;
                }
                else{
                    if(field == 0){     //Before the bytesize
                        field++;
                        printf("Found first newline\n");
                        sb_destroy(readBuf);
                        sb_init(readBuf, 10);
                        continue;
                    }
                    else if(field == 1){    //Finished reading the bytesize
                        printf("Found second newline, data is %s\n", readBuf->data);
                        
                        byteSize = atoi(readBuf->data);
                        if(byteSize == 0){
                            printf("Error: [BAD]; Invalid byteSize entered\n");
                            break;
                        }
                        else{
                            printf("ByteSize set to %d\n", byteSize);
                        }
                        sb_destroy(readBuf);
                        sb_init(readBuf, 10);
                        currByteCount = 0;
                        field++;
                    }
                    else if(field == 2){    //Read first field
                        printf("Found third newline, data is %s\n", readBuf->data);

                        //Do stuff with first field, depends on request
                        if(currByteCount > byteSize){
                            printf("Error [LEN]; Invalid length\n");
                            //sb_destroy(readBuf);
                            isFailed = true;
                            break;
                        }
                        if(req == GET){
                            node* val = findValue(args->tree, readBuf->data);
                            if(val != NULL)
                                printf("GET value associated with key '%s': '%s'\n", readBuf->data, val->value);
                            else
                                printf("GET value associated with key '%s': key not found\n", readBuf->data);

                        }
                        else if(req == DEL){
                            printf("DELETE value associated with key '%s'\n", readBuf->data);
                        }
                        else{   //req == SET
                            sb_concat(field1, readBuf->data);
                        }
                        sb_destroy(readBuf);
                        sb_init(readBuf, 10);

                        field++;
                    }
                    else if(field == 3){
                        printf("Found fourth newline, data is %s\n", readBuf->data);

                        //Do stuff with second field, depends on request
                        if(currByteCount > byteSize){
                            printf("Error [LEN]; Invalid length\n");
                            isFailed = true;
                            break;
                        }
                        if(req == SET){
                            printf("SET value associated with key '%s'\n", field1->data);
                            insert(args->tree, field1->data, readBuf->data);
                        }
                    }
                }
            }

            printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);
            if(isFailed)
                break;
        }

        /*
        else{
            while ((nread = read(c->fd, buf, BUFSIZE)) > 0) {
                buf[nread] = '\0';
                printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);
            }
        }
        */
    }
    else{
        printf("Error [BAD]; Malformed message.\n");
    }
    printf("[%s:%s] got EOF\n", host, port);

    if(isFailed)
        printf("Failure\n");
    close(c->fd);
    free(c);
    return NULL;
}