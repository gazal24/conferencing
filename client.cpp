#include "client_header.h"

int main(int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf(stderr,"usage: <hostname> <PORT>\n");
      exit(1);
    } else {
    HOSTNAME = argv[1];
    PORT = atoi(argv[2]);
  }

  if ((he=gethostbyname(argv[1])) == NULL) /* get the host info */
    {
      herror("gethostbyname");
      exit(1);
    }
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }
  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(PORT);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  bzero(&(their_addr.sin_zero), 8);
  if (connect(sockfd,(struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
      perror("connect");
      exit(1);
    }
  
  fout.open("sockfds", ios::app);
  fout << (int *)sockfd;
  fout.close();

  if ((nb=recv(sockfd, buf, 100, 0)) == -1){
    perror("Recieve 1"); exit(1);
  }

  buf[nb] = '\0';
  str = ""; str.append(buf);
  if(str != "+OK")
    {
      printf("Connection cannot be established");
      exit(0);
    }
  printf("\n\tConnected to Server : \n");
    
  while(1) {
    printf("\n\tEnter Choice : \n");
    printf("\t\t1. Login\n");
    printf("\t\t2. Register\n");
    printf("\t\t3. Change Password\n");
    printf("\t\t_________________________________________\nChoice : ");
    scanf("%d", &choice);
    
    switch(choice) {

    case 1: 
      str = "LOGI";
      if (send (sockfd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	perror("send:"); exit(0);
      }
      if((nb=recv(sockfd, buf, 100, 0)) == -1){
	perror("Recieve 2"); exit(1);
      } 
      buf[nb] = '\0'; str = ""; str.append(buf);
      if(str != "+OK"){
	printf("<LOGI> Login request cannot be completed");
	continue;
      }
	
      printf("Username : ");
      scanf("%s", user);
      printf("Password : ");
      scanf("%s", pass);
      if (send (sockfd, user, strlen(user), 0) == -1){
	perror("send:"); exit(0);
      }
      if ((nb=recv(sockfd, buf, 100, 0)) == -1){
	perror("Recieve 3"); exit(1);
      }
      buf[nb] = '\0'; str = ""; str.append(buf);
      if(str != "PASS") {
	cout << "The login failed" <<endl ;
	continue;
      }
      if (send (sockfd, pass, strlen(pass), 0) == -1){
	perror("send:"); exit(0);
      }
      if ((nb=recv(sockfd, buf, 100, 0)) == -1){
	perror("Recieve 3"); exit(1);
      }
      buf[nb] = '\0'; str = ""; str.append(buf);
      if(str != "AUTH"){
	cout<< "Login failed" << endl;
	continue;
      }
      
      pthread_create(&pth,NULL,recvthread,(void *)sockfd);
      cout <<  "Login Successful" << endl;
      getline(cin, str);

      while(1){

	usleep(usec);
	if(prompt_flag == 1)
	  prompt();
	prompt_flag=0;
	//	cin >> str;
	getline(cin, str);
	//str = ""; str.append(text);
	if(str[0] == '/') {
	  command = str.substr(1, str.find(' ')-1);
	  message = str.substr(str.find(' ')+1);
	}
	else if(str == "") {
	  continue;
	}
	else {
	  command = "data";
	  message = str;
	}
	if(belong_to(command) == 0) {
	  cout << "invalid command : " << command;
	  prompt_flag = 1;
	  continue;
	}

	//{"start", "topic", "list", "invite", "data", "kick", "leave", "end"};
	if(command == "start") {
	  str = "STAT";
	  str.append(" ");
	  str.append(message);
	}

	if(command == "topic") {
	  str = "TOPI";
	  str.append(" ");
	  str.append(message);
	}

	if(command == "users") {
	  str = "USER";
	}

	if(command == "info") {
	  str = "INFO";
	}

	if(command == "invite") {
	  str = "INVI";
	  str.append(" ");
	  str.append(message);
	}

	if(command == "accept") {
	  str = "ACPT";
	  str.append(" ");
	  str.append(message);
	}

	if(command == "data") {
	  str = "DATA";
	  str.append(" ");
	  str.append(message);
	  prompt_flag = 1;
	}

	if(command == "kick") {
	  str = "KICK";
	  str.append(" ");
	  str.append(message);
	}

	if(command == "leave") {
	  str = "LEAV";
	}

	if(command == "end") {
	  str = "ENDC";
	}

	if (send (sockfd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	  perror("send:"); exit(0);
	}
	
      }//END OF Inner while(1) <in case 1:>


      break;
	
    case 2: 
      str = "REGR";
      if (send (sockfd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	perror("send:"); exit(0);
      }
      if ((nb=recv(sockfd, buf, 100, 0)) == -1){
	perror("Recieve 2"); exit(1);
      } 
      buf[nb] = '\0'; str = ""; str.append(buf);
      if(str != "+OK") {
	  printf("<REGR> Register request cannot be completed");
	  continue;
      }
      cout << endl;
      cout << "<Username and Password should contain only alphabetical characters not more than 10>" << endl;
      printf("Username : ");
      scanf("%s", user);
      printf("Password : ");
      scanf("%s", pass);
      if (send (sockfd, user, strlen(user), 0) == -1){
	perror("send:"); exit(0);
      }
      if ((nb=recv(sockfd, buf, 100, 0)) == -1){
	perror("Recieve 3"); exit(1);
      } 
      buf[nb] = '\0'; str = ""; str.append(buf);
      if(str == "ERR") {
	cout << endl << user <<" : already exists. Registration failed\n";
	continue;
      }
      if(str == "PASS") {
	if (send (sockfd, pass, strlen(pass), 0) == -1) {
	  perror("send:"); exit(0);
	}
      }
      break;
      
    case 3: 
      str = "CHPS";
      if (send (sockfd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	perror("send:"); exit(0);
      }
      break;
      
    default: printf("Wrong Choice again");
      exit(0);
    }//END OF switch-case
  }//END OF while(1)
}//END OF main()


void *recvthread(void* arg)
{
  //  int sockf = *((int *)arg);
  //  int sockf = (long)arg;
  while(1){
    if ((nb=recv(sockfd, buf, 100, 0)) == -1){
      perror("Recieve 3"); exit(1);
    } 
    buf[nb] = '\0'; str = ""; str.append(buf);
    cout << endl << str;
    str = "";
    prompt();
  }
}

void prompt()
{
  cout << endl << user << ">";
  fflush(0);
  return;
}

int belong_to(string var)
{
  //  cout << "received:" << var << ":" << endl;
  for(i=0; i<LIST_SIZE; i++)
      if(command_list[i].compare(var) == 0)
	return 1;
  return 0;
}

