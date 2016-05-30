/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define NUM_THREADS 4

void *Ask(void *t);
void error(const char *msg);
int t = 0;


int main(int argc, char *argv[])
{
     pthread_t thread[NUM_THREADS];
     pthread_attr_t attr;
     int rc;
     long newsockfd;
     void *status;
     int sockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     pthread_attr_init(&attr);
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);

     printf("Main: creating thread %d\n", t);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
     	error("ERROR on accept");
	 if(t>-1 && t<5){
		printf("%ld \n",newsockfd);
     	rc = pthread_create(&thread[t], &attr, Ask,
			(void *)newsockfd);
		t++;
	 }  
     if (rc) {
     	printf("ERROR; return code from pthread_create() is %d\n", rc);
     	exit(-1);
     }
     pthread_join(rc,NULL);
     return 0; 
}

void *Ask(void *t)
{
    int i;
    int n;
    long clientfd = (long) t;
	printf("%ld \n",clientfd);
    char buffer[256];
    long tid;
    tid = (long)t;
    printf("Thread %ld starting...\n",tid);
	bzero(buffer,256);
    n = read(clientfd,buffer,255);
	if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    for (i=0; i<10; i++)
    {
       bzero(buffer,256);
       n = write(clientfd,"Question %d, waiting for your answer :",i);
	   printf("asdf\n");
       if (n < 0) error("ERROR writing to socket");
       n = read(clientfd,buffer,255);
       if (n < 0) error("ERROR reading from socket");
       printf("You answered: %s\n",buffer);
    }
    printf("Thread %ld done.",tid);
    pthread_exit((void*) t);
	close(clientfd);
	t--;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
