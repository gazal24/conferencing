#include "server_header.h"

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
      fprintf(stderr, "usage: <port number>\n");
      exit(1);
    }
  else my_port = atoi(argv[1]);

  fflush(0);
  remove("onlineuser.txt");
  remove("conference.txt");
  remove("nextConfId.txt");

  file_1 = fopen("nextConfId.txt", "a+");
  nextConfId.id = 0;
  fwrite(&nextConfId,sizeof(next_Conf_Id),1,file_1);
  fclose(file_1);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("socket");
      exit(1);
    }
  my_addr.sin_family = AF_INET;  /*   my_addr.sin_port = htons(MYPORT); */
  my_addr.sin_port = htons(my_port);
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  bzero(&(my_addr.sin_zero), 8); /* zero the rest */
  if (bind(sockfd,(struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
      perror("bind");
      exit(1);
    }
  if (listen(sockfd, BACKLOG) == -1)
    {
      perror("listen");
      exit(1);
    }
  while(1) /* main accept() loop */
    {
      sin_size = sizeof(struct sockaddr_in);
      if ((socketinfo.fd = accept(sockfd,(struct sockaddr *)&their_addr,(socklen_t*)&sin_size)) ==-1)
	{
	  perror("accept");
	  continue;
	}
      printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
      
      pthread_create(&pth,NULL,main_code,(void *)&socketinfo);
      
    }//END OF main()
}

 
void *main_code(void* socketinfo) {

  //============================================
  //============================================
  int new_fd;

  char buf[100];
  char temp[100];
  char *ptr;
  string str="";
  string line;
  string user, pass;
  string recv_str;
  string message;
  string str_1;

  int nb=0; //NUMBYTES
  int conflict=0;
  int found=0;
  int invite_flag = 0;

  FILE *file;
  FILE *file_temp;

  user_info userinfo;

  online_user onlineuser;
  online_user onlineuser_temp; // used for all file READ operations

  conference_room conference;
  conference_room conference_temp; // used for all file READ operations

  //============================================
  //============================================



  struct socket_info *socketinfo_temp;
  socketinfo_temp = (struct socket_info*)socketinfo;

  new_fd = socketinfo_temp->fd;

  str = "+OK";
  if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1){
    perror("send"); exit(1);
  }
  while(1) {
    if ((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
      perror  ("Recieve 1"); exit(0);
    }

    buf[nb] = '\0'; recv_str = ""; recv_str.append(buf);
    str = recv_str.substr(0,4);
	    
    if(str == "REGR") {
      str = "+OK";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1){
	perror("send"); exit(0);
      }
      if((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
	perror  ("Recieve 2"); exit(0);
      }
      buf[nb] = '\0'; str = ""; str.append(buf);
      user = str;

      file = fopen("userinfo.txt", "a+");
      if(!file) continue;

      conflict = 0;
      while(!feof(file)) {
	if(!fread(&userinfo,sizeof(user_info),1,file)) break;
	if(strcmp(userinfo.name, user.c_str()) == 0)
	  conflict = 1;
      }
      fclose(file);


      if(conflict == 1) {
	str = "ERR";
	if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1){
	  perror("send"); exit(0);
	}
	continue;
      }

      else {
	str = "PASS";
	if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1){
	  perror("send"); exit(0);
	}
	if((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
	  perror  ("Recieve 2"); exit(0);
	}
	buf[nb] = '\0'; str = ""; str.append(buf);
	pass = str;
	cout << "Pass : " << pass;

	if(pass[pass.length()-1] == 'y')
	  userinfo.designated = 1;
	else
	  userinfo.designated = 0;
	
	pass = pass.substr(0, pass.length()-1);
	//	cout << "\nnew_pass"  << pass;
	//	cout << "designated : " << userinfo.designated;
	//STRUCT WRITE
	file = fopen("userinfo.txt", "a+");
	sprintf(userinfo.name, "%s", user.c_str());
	sprintf(userinfo.pass, "%s", pass.c_str());
	fwrite(&userinfo,sizeof(user_info),1,file);
	fclose(file);
      }	
      fflush(stdout);
    }//END OF if(str = "REGR")

	  
    if(str == "LOGI") {
	    
      str = "+OK";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	perror("send"); exit(1);
      }
      if((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
	perror  ("Recieve 2"); exit(0);
      }
      buf[nb] = '\0'; str= ""; str.append(buf);
      user = str;
      str = "PASS";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1) {
	perror("send"); exit(1);
      }
      if((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
	perror  ("Recieve 2"); exit(0);
      }
      buf[nb] = '\0'; str= ""; str.append(buf);
      pass = str;

      cout << user << pass;
      fflush(stdout);
	    
      //STRUCT READ
      str = "NOAUTH";

      file = fopen("userinfo.txt", "r");

      while(!feof(file)) {
	if(!fread(&userinfo,sizeof(user_info),1,file))
	  break;
	if(strcmp(userinfo.name, user.c_str()) == 0 
	   && strcmp(userinfo.pass, pass.c_str()) == 0) {
	  str = "AUTH"; break;
	}
      }

      fclose(file);

      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
	      
      //MAKE HIS ENTRY IN ONLINE_USER.TXT.
      if(str == "AUTH") {
	file = fopen("onlineuser.txt", "a+");
	sprintf(onlineuser.name, "%s", user.c_str());
	onlineuser.new_fd = new_fd;
	onlineuser.conf_id = 0;
	fwrite(&onlineuser,sizeof(online_user),1,file);
	fclose(file);
      }
    }//END OF if(str = "LOGI")

    if(str ==  "RELA") {
      str = "and i got this reply";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      cout << "yea i got relay from thread" << endl;
      fflush(stdout);
    }//END OF if(str = "RELAY")



    if(str == "STAT") {
      //CHECK THE GUY IS DESIGNATED OR NOT. refer userinfo.txt OR chk flag in process itself.
      //CHECK IF HE IS FREE OR NOT. refer online_user.txt for this.
	      
      if(userinfo.designated != 1)
	str = "server> You are not designated to create conference";
      else if(onlineuser.conf_id != 0)
	str = "server> You are already in a conference";
      else
	{
	  conference.id = get_next_conference_id();
	  sprintf(conference.admin, "%s", userinfo.name);
	  sprintf(conference.topic, "%s", recv_str.substr(recv_str.find(' ')+1, recv_str.length()-1).c_str());
	  file = fopen("conference.txt", "a+");
	  fwrite(&conference,sizeof(conference_room),1,file);
	  fclose(file);
	  onlineuser.conf_id = conference.id;
	  update_onlineusers(userinfo, onlineuser);
	  str = "server> Room Created Successfully";
	}

      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
	      
      //CREATE A ROOM. create a file <conference_id.txt>
      //ADD THE TOPIC OF MESSAGE TO THE TOP OF <conference_id.txt>
      //ADD HIM TO THE SECOND LINE OF <conference_id.txt>.
      //UPDATE HIS CONFERENCE ID IN online_user.txt.
    }

    if(str == "TOPI") {
      str = "server> You are not in any conference. No Topic Found";
      file = fopen("conference.txt", "r");
      found = 0;
      while(!feof(file)) {
	if(!fread(&conference_temp,sizeof(conference_room),1,file))
	  break;
	if(onlineuser.conf_id == conference_temp.id) {
	  found = 1; break;
	}
      }
      fclose(file);
      //	      cout << endl << "confid " << conference_temp.id;
      //	      cout << endl << "onlien_confid " << onlineuser.conf_id;
      fflush(stdout);
      if(found) {
	str = "server> Topic : "; 
	str.append(conference_temp.topic);
      }
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      //DISPLAY THE TOPIC. go to online_user.txt and fetch conference_id. go to this conference_id.txt and display the topic.
    }


    if(str == "USER") {
      str = "Available Users : ";
      file = fopen("onlineuser.txt", "r+");
      found = 0;
      while(!feof(file)) {
	if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	  break;
	cout << " id : " << onlineuser_temp.conf_id;
	if(onlineuser_temp.conf_id == 0)
	  {
	    str.append(onlineuser_temp.name);
	    str.append(", ");
	    found = 1;
	  }
      }
      fclose(file);
      if(found) {
	str[str.length()-2] = '.';
      }
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      //LIST ALL USER WHO ARE FREE. go to online_user.txt and check if conference_id is NULL... display him. 
    }
    if(str == "INFO") {
      //GO TO online_user.txt and fetch conference_id. go to conference_id.txt and display all the details.
    }
    if(str == "INVI") {
      message = recv_str.substr(recv_str.find(' ')+1, recv_str.length()-1);
      invite_flag = 0;
      str = "Some error";
      if(userinfo.designated == 0)
	str = "You are not designated to invite.";
      else if(onlineuser.conf_id == 0)
	str = "You are not in any chat.";
      else { //espp
	file = fopen("conference.txt", "a+");
	while(!feof(file)) {
	  if(!fread(&conference_temp,sizeof(conference_room),1,file))
	    break;
	  if(onlineuser.conf_id == conference_temp.id && strcmp(onlineuser.name, conference_temp.admin) == 0) {
	    found = 1; break;
	  }
	}
	fclose(file);
	//	cout << "str" << str;
	//	cout << "\nfound : " << found << endl;
	fflush(stdout);
		
	if(found == 0)
	  str = "server> You are not the admin of this conference.";
	else {
	  file = fopen("onlineuser.txt", "r+");
	  while(!feof(file)) {
	    if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	      break;
	    if(strcmp(message.c_str(), onlineuser_temp.name) == 0) {
	      if(onlineuser_temp.conf_id == 0) {
		str = "sever>User has been sent an invite."; invite_flag = 1;
		break;
	      }
	      else {
		str = "server> User already in a conference"; break;
	      }
	    }
	    else {
	      str = "server> User not found";
	    }
	  }
	}
	fclose(file);
	//	cout << "\nstr_1 : " << str_1;
	//	cout << "\nstr : " << str;
	//	cout << "\ninvite_flag : " << invite_flag << endl;
	//	cout << "\nNew_fd : " << onlineuser_temp.new_fd << endl;
	fflush(stdout);
      }
	      
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
	      
      if(invite_flag) {
	str_1 = "REQU You have been invited to a conference.(Hit y to accept) CONF";
		
	sprintf(temp, " %d ", onlineuser.conf_id);
	str_1.append(temp);
	cout << "str_1" << str_1 << endl;
	if (send(onlineuser_temp.new_fd, str_1.c_str(), strlen(str_1.c_str()), 0) == -1)
	  perror("send");
      }	      
      //FOR each username, go to online_user.txt. check if he is free and send him a invitation.
    }
    if(str == "ACPT") {
      str = "server> Invite Rejected.";
      if(recv_str.find("YES") != string::npos)
	{
	  if(onlineuser.conf_id != 0)
	    str = "You are already in a conference you cannot accept this request.";
	  else {
	    sprintf(temp, "%s", recv_str.c_str());
	    ptr = strtok(temp," ");
	    ptr = strtok (NULL, " ");
	    onlineuser.conf_id = atoi(ptr);
	    update_onlineusers(userinfo, onlineuser);
	    str = "server> You joined this room. <Welcome>";
	  }
	}
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      //go to online_user.txt and check if he is free, update his conference_id. go to conference_id.txt and add him at the bottom. if he is not free, notify him to /leave first and then accept other request.
    }
    if(str == "DATA") {
      message = recv_str.substr(recv_str.find(' ')+1, recv_str.length()-1);
      //      cout << "\nMessage is : " << message <<endl;
      str = "";
      str.append(onlineuser.name);
      str.append("> ");
      str.append(message);
      if(onlineuser.conf_id == 0)
	str = "You are not in any conference. Msg not sent.";
      else {
	file = fopen("onlineuser.txt", "r+");
	while(!feof(file)) {
	  if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	    break;
	  if(onlineuser_temp.conf_id == onlineuser.conf_id && strcmp(onlineuser_temp.name, onlineuser.name) !=  0) {
	    //	    cout << onlineuser_temp.name;
	    if (send(onlineuser_temp.new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	      perror("send");
	  }
	}
	fclose(file);
      }
      //go to online_user.txt, fetch the conference_id. go to conference_id.txt.. fetch all other members and relay message to them.
    }
	    
    if(str == "KICK") {
      str = "Some error";
      if(userinfo.designated == 0)
	str = "You are not designated to invite.";
      else if(onlineuser.conf_id == 0)
	str = "You are not in any chat.";
      else { 
      }
      //go to conference_id.txt. check if he is the Owner then check if he is in conference list then remove his name from conference_id.txt and notify him. Then go to online_user.txt and change his conference_id to NULL.
    }


    if(str == "LEAV") {
      //TODO : NOTIFY ALL OTHER USERS.
      //go to conference_id.txt and remove him from the list. go to online_user.txt and change his conference_id to NULL. If this person is designated user... inform all other members and end the room.
      if(onlineuser.conf_id == 0)
	str = "server> You are not in a room :P.";
      
      file = fopen("conference.txt", "r");
      found = 0;
      while(!feof(file)) {
	if(!fread(&conference_temp,sizeof(conference_room),1,file))
	  break;
	if(strcmp(onlineuser.name, conference_temp.admin) == 0) {
	  found = 1; break;
	}
      }
      fclose(file);
      
      if(found) {
	str = "server> You are the admin of this conference. You cannot leave.";
	str.append("\nserver> Issue /end to end conference.");
      }
      else {
	onlineuser.conf_id = 0;
	update_onlineusers(userinfo, onlineuser);
	str = "server> You exited the room.";
      }
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
    }


    if(str == "ENDC") {
      //go to conference_id.txt... notify each member in the list and remove that file.
      if(onlineuser.conf_id == 0)
	str = "server> You are not in a conference. Nothing to End";
      else {
	file = fopen("conference.txt", "r");
	found = 0;
	while(!feof(file)) {
	  if(!fread(&conference_temp,sizeof(conference_room),1,file))
	    break;
	  if(strcmp(onlineuser.name, conference_temp.admin) == 0) {
	    found = 1;
	    remove_conference(onlineuser);
	    break;
	  }
	}
	fclose(file);

	str = "ENDD Conference ended by Admin.";
	if(found) {
	  cout << "yes conference ends now";
	  file = fopen("onlineuser.txt", "r+");
	  while(!feof(file)) {
	    if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	      break;
	    if(onlineuser_temp.conf_id == onlineuser.conf_id && strcmp(onlineuser.name, onlineuser_temp.name) != 0)
	      if (send(onlineuser_temp.new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
		perror("send");
	  }
	  fclose(file);
	  str = "ENDD You ended this conference.";
	}
	else 
	  str = "server> You are not the admin of this conference. You cannot End it.";
      }
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
    }
    
    
    if(str == "LOGO") {
      //go to online_user.txt get his conference_id... remove him from conference. and then remove him from online_user.txt also.
      file = fopen("onlineuser.txt", "r");
      file_temp = fopen("onlineuser_temp.txt", "w");

      while(!feof(file)) {
	if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	  break;
	if(strcmp(onlineuser_temp.name, onlineuser.name) != 0)
	  fwrite(&onlineuser_temp,sizeof(online_user),1,file_temp);
      }
      fclose(file);
      fclose(file_temp);
      remove("onlineuser.txt");
      rename("onlineuser_temp.txt", "onlineuser.txt");
      str = "You logged out successfully";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      return 0;
    }


    if(str == "PRIN") {
      cout << "\nONLINE USERS DETAILS";
      fflush(stdout);
      file = fopen("onlineuser.txt", "r+");
      while(!feof(file)) {
	if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
	  break;
	cout << "\nName : " << onlineuser_temp.name << " New_fd : " << onlineuser_temp.new_fd << " conf_id : " << onlineuser_temp.conf_id;
      }
      fclose(file);
      cout << "\nCONFERENCE DETAILS";
      fflush(stdout);
      file = fopen("conference.txt", "a+");
      while(!feof(file)) {
	if(!fread(&conference_temp,sizeof(conference_room),1,file))
	  break;
	cout << "\nAdmin : " << conference_temp.admin << " id : " << conference_temp.id << " topic : " << conference_temp.topic;
      }
      fclose(file);
      cout << "\nEND OF DETAILS";
      fflush(stdout);
      str = "+OK";
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
    }

    if(str == "HELP") {
      str = "";
      str.append("COMMAND \t- DESCRIPTION");
      str.append("\n start  \t- begin a conference.");
      str.append("\n users  \t- see online free users.");
      str.append("\n invite \t- send join request to online free user.");
      str.append("\n topic  \t- view topic of conference.");
      str.append("\n leave  \t- leave the conference.");
      str.append("\n logout \t- logout.");
      str.append("\n end    \t- End a conference.");
      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
	perror("send");
      //go to online_user.txt get his conference_id... remove him from conference. and then remove him from online_user.txt also.
    }

    fflush(stdout);
  }//END OF while(1)	  
	  
}




void update_onlineusers(user_info userinfo, online_user onlineuser)
{
  FILE *file;
  online_user onlineuser_temp;
  file = fopen("onlineuser.txt", "r+");
  while(!feof(file)) {
    if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
      break;
    if(strcmp(userinfo.name, onlineuser_temp.name) == 0)
      break;
  }
  fseek(file, (-1*sizeof(online_user)),SEEK_CUR);
  fwrite(&onlineuser,sizeof(online_user),1,file);
  fclose(file);
  //update online_user.conf_id in file onlineuser.txt. use online_user.name to check which record.
}


void remove_conference(online_user onlineuser)
{
  FILE *file, *file_temp;
  conference_room conference_temp;
  
  file = fopen("conference.txt", "r");
  file_temp = fopen("conference_temp.txt", "w");
  
  while(!feof(file)) {
    if(!fread(&conference_temp,sizeof(conference_room),1,file))
      break;
    if(strcmp(conference_temp.admin, onlineuser.name) != 0)
      fwrite(&conference_temp,sizeof(conference_room),1,file_temp);
  }
  fclose(file);
  fclose(file_temp);
  remove("conference.txt");
  rename("conference_temp.txt", "conference.txt");
  cout << "\nIn update_conferences\n";
}


int get_next_conference_id()
{
  int val = 0;
  FILE *file;
  file = fopen("nextConfId.txt", "r+");
  fread(&nextConfId,sizeof(next_Conf_Id),1,file);
  val = ++nextConfId.id;
  printf("\nVAl %d\n", val);
  fflush(stdout);
  fseek(file, (-1*sizeof(next_Conf_Id)),SEEK_CUR);
  fwrite(&nextConfId,sizeof(next_Conf_Id),1,file);
  fclose(file);

  return val;
}
 
 
