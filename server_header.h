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
#include <sstream>


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
void update_onlineusers();
void update_conferences();
int get_next_conference_id();



// USER

char buf[100];
char temp[100];
char *ptr;
string str="";
string line;
string user, pass;
string recv_str;
string message;
string str_1;

int my_port = 0;
int nb=0; //NUMBYTES
int conflict=0;
int found=0;
int userfree;
int next_conference_id = 1;
int invite_flag = 0;

FILE *file;

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

online_user onlineuser;
online_user onlineuser_temp; // used for all file READ operations



typedef struct{
  int id;
  char admin[10];
  char topic[100];
}conference_room;

conference_room conference;
conference_room conference_temp; // used for all file READ operations


typedef struct{
  int id;
}next_Conf_Id;

next_Conf_Id nextConfId;


