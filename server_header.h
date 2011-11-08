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



//PROTOTYPES
void get_info();
void update();


// USER

char buf[100];
string str="";
string line;
string user, pass;
string recv_str;

int my_port = 0;
int nb=0; //NUMBYTES
int conflict=0;
int found=0;
int userfree;
int next_conference_id = 1;


FILE *file;

typedef struct{
  int id;
  char admin[10];
  char topic[100];
}conference_room;

conference_room conference, conference_temp;


typedef struct{
  char name[10];
  char pass[10];
  bool designated;
}user_info;

user_info userinfo;


typedef struct{
  char name[10];
  int new_fd;
  int conf_id;
}online_user;

online_user onlineuser, onlineuser_temp;
