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
      if ((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,(socklen_t*)&sin_size)) ==-1)
	{
	  perror("accept");
	  continue;
	}
      printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
      
      if (!fork()) /* this is the child process */
	{
	  //	  fout.open("sockinfo.txt", ios::app);
	  //	  fout << new_fd << endl;
	  //	  fout.close();			
	  
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
	    cout << endl << "RECV : " << recv_str;
	    
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

	      /*	      fin.close();
	      fin.open("userinfo.txt");

	      if(fin.is_open()) {
		conflict = 0;
		while (fin.good() && !fin.eof()) {
		  getline(fin,line);
		  if(line.substr(0, line.find(' ')).compare(user) == 0)
		    conflict = 1;
		}
		}*/


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
		
		//STRUCT WRITE
		file = fopen("userinfo.txt", "a+");
		sprintf(userinfo.name, "%s", user.c_str());
		sprintf(userinfo.pass, "%s", pass.c_str());
		userinfo.designated = 0;
		fwrite(&userinfo,sizeof(user_info),1,file);
		fclose(file);
	      }	
	      fflush(0);
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
	      fflush(0);
	    
	      /*fin.close();
	      fin.open("userinfo.txt");
	      
	      	      if(fin.is_open()) {
		found = 0; str = "NOAUTH";
		while (fin.good() && !fin.eof()) {
		  getline(fin,line);
		  found=line.find(' ');
		  if(line.substr(0, found).compare(user) == 0 && line.substr(found+1, line.find(' ', found+1)).compare(pass) == 0) {
		    //MATCH
		    str = "AUTH";
		    break;
		  }
		}//END of while(fin)
	      }
	      */

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
	      fflush(0);
	    }//END OF if(str = "RELAY")



	    if(str == "STAT") {
	      //CHECK THE GUY IS DESIGNATED OR NOT. refer userinfo.txt OR chk flag in process itself.
	      //CHECK IF HE IS FREE OR NOT. refer online_user.txt for this.
	      
	      get_info();
	      if(userinfo.designated != 1)
		str = "server> You are not designated to create conference";
	      else if(onlineuser.conf_id != 0)
		str = "server> You are already in a conference";
	      else
		{
		  file = fopen("conference.txt", "a+");
		  conference.id = next_conference_id++;
		  sprintf(conference.admin, "%s", userinfo.name);
		  sprintf(conference.topic, "%s", recv_str.substr(recv_str.find(' ')+1, recv_str.length()-1).c_str());
		  fwrite(&conference,sizeof(conference_room),1,file);
		  fclose(file);
		  
		  onlineuser.conf_id = conference.id;
		  update();
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
	      file = fopen("conference.txt", "a+");
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
	      //cout << endl << "onlien_confid " << onlineuser.conf_id;
	      //fflush(0);
	      if(found) {
		str = "server>Topic : "; 
		str.append(conference_temp.topic);
	      }
	      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
		perror("send");
	      //DISPLAY THE TOPIC. go to online_user.txt and fetch conference_id. go to this conference_id.txt and display the topic.
	    }
	    
	    if(str == "USER") {
	      
	      //LIST ALL USER WHO ARE FREE. go to online_user.txt and check if conference_id is NULL... display him. 
	    }
	    if(str == "INFO") {
	      //GO TO online_user.txt and fetch conference_id. go to conference_id.txt and display all the details.
	    }
	    if(str == "INVI") {
	      //FOR each username, go to online_user.txt. check if he is free and send him a invitation.
	    }
	    if(str == "ACPT") {
	      //go to online_user.txt and check if he is free, update his conference_id. go to conference_id.txt and add him at the bottom. if he is not free, notify him to /leave first and then accept other request.
	    }
	    if(str == "DATA") {
	      //go to online_user.txt, fetch the conference_id. go to conference_id.txt.. fetch all other members and relay message to them.
	    }
	    if(str == "KICK") {
	      //go to conference_id.txt. check if he is the Owner then check if he is in conference list then remove his name from conference_id.txt and notify him. Then go to online_user.txt and change his conference_id to NULL.
	    }
	    if(str == "LEAV") {
	      cout << "yeah i ma here at leave \n";
	      fflush(0);
	      if(onlineuser.conf_id == 0)
		str = "server> You are not in a room :P.";
	      else {
	      onlineuser.conf_id = 0;
	      update();
	      str = "server> You exited the room.";
	      }
	      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
		perror("send");
	      //TODO : NOTIFY ALL OTHER USERS.
	      //go to conference_id.txt and remove him from the list. go to online_user.txt and change his conference_id to NULL. If this person is designated user... inform all other members and end the room.
	    }
	    if(str == "ENDC") {
	      //go to conference_id.txt... notify each member in the list and remove that file.
	    }
	    if(str == "LOGO") {
	      //go to online_user.txt get his conference_id... remove him from conference. and then remove him from online_user.txt also.
	    }
	    fflush(0);
	  }//END OF while(1)	  
	}//END OF fork()
    }//END OF while()
}//END OF main()


void get_info()
{
  cout << "\n in is_free : " << userinfo.name;
  cout << onlineuser.name << endl;
  //UPDATE userinfo, onlineuser with what is present in file;
}


void update()
{
  file = fopen("onlineuser.txt", "r+");
  while(!feof(file)) {
    if(!fread(&onlineuser_temp,sizeof(online_user),1,file))
      break;
    if(strcmp(userinfo.name, onlineuser_temp.name) == 0)
      break;
  }
  fseek(file, (-1*sizeof(online_user)),SEEK_CUR);
  fwrite(&onlineuser_temp,sizeof(online_user),1,file);
  fclose(file);
  //update online_user.conf_id in file onlineuser.txt. use online_user.name to check which record.
}
