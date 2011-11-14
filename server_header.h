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
#include <pthread.h>


using namespace std;

// DEFINE
#define MAXDATASIZE 1000
#define BACKLOG 10 /* number of pending connections */

// SYSTEM
int sockfd; /* listen on sock_fd,
		       new connection on new_fd */
struct sockaddr_in my_addr; /* my address information */
struct sockaddr_in their_addr; /* client's address info */
int sin_size;
int my_port = 0;



//USER

FILE *file_1;
pthread_t pth;

typedef struct{
  char name[10];
  char pass[10];
  bool designated;
}user_info;

typedef struct{
  char name[10];
  int new_fd;
  int conf_id;
}online_user;

typedef struct{
  int id;
  char admin[10];
  char topic[100];
}conference_room;

typedef struct{
  int id;
}next_Conf_Id;

next_Conf_Id nextConfId;

struct socket_info{
  int fd;
};

struct socket_info socketinfo;


//PROTOTYPES
void update_onlineusers(user_info, online_user);
void remove_conference(online_user);
int get_next_conference_id();
void *main_code(void*);
