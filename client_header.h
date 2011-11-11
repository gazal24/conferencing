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
#include <time.h>
#include <fstream>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <sstream>

using namespace std;

#define MAXDATASIZE 1000 /* max number of bytes we can get at once */
//PROTOTYPE
void *recvthread(void* arg);
void prompt();
int belong_to(string);


//SYSTEM
int sockfd;
int PORT;
struct hostent *he;
struct sockaddr_in their_addr; /* client's address information */
char *HOSTNAME =(char*)malloc(sizeof(char)*50);

//USER
string str="";
string command,message;

char buf[100];
char user[10],pass[10];
char temp[10];
char *ptr;

int i;
int nb=0;
int prompt_flag = 1;
int request_flag = 0;
int conf_id = 0;
int choice=0;
int found=0;

pthread_t pth;
unsigned int usec = 100000;

#define LIST_SIZE 11
std::string command_list[LIST_SIZE] = {"start", "topic", "users", "invite", "data", "kick", "leave", "end", "info", "accept", "print"};

