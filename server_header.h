#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <ctype.h>

using namespace std;

// DEFINE
#define MAXDATASIZE 1000
#define BACKLOG 10 /* number of pending connections */


// SYSTEM
int sockfd, new_fd; /* listen on sock_fd,
		       new connection on new_fd */
struct sockaddr_in my_addr; /* my address information */
struct sockaddr_in their_addr; /* client's address info */
int sin_size;


// USER
ifstream fin;
ofstream fout;

char buf[100];
string str="";
string line;
string user, pass;

int my_port = 0;
int nb=0; //NUMBYTES
int conflict=0;


typedef struct{
  char user[10];
  char pass[10];
}data;

typedef struct{
  int id;
  int *members;
  char *topic;
}conference;

