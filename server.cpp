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
	  fout.open("sockinfo.txt", ios::app);
	  fout << new_fd << endl;
	  fout.close();			
	  
	  str = "+OK";
	  if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1){
	    perror("send"); exit(1);
	  }

	  while(1) {
	    if ((nb=recv(new_fd, buf, MAXDATASIZE, 0)) <= 0) {
	      perror  ("Recieve 1"); exit(0);
	    }

	    buf[nb] = '\0'; str = ""; str.append(buf);
	  
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
	      fin.close();
	      fin.open("userinfo.txt");

	      if(fin.is_open()) {
		conflict = 0;
		while (fin.good() && !fin.eof()) {
		  getline(fin,line);
		  if(line.substr(0, line.find(' ')).compare(user) == 0)
		    conflict = 1;
		}
	      }

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
		fin.close();
		fout.open("userinfo.txt", ios::app);
		fout << user << " " << pass << endl;
		fout.close();			
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
	    
	      fin.close();
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

	      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
		perror("send");
	    }//END OF if(str = "LOGI")

	    if(str ==  "RELAY") {
	      str = "and i got this reply";
	      if (send(new_fd, str.c_str(), strlen(str.c_str()), 0) == -1)
		perror("send");
	      cout << "yea i got relay from thread" << endl;
	      fflush(0);
	      //fin.open("sockfds");
	    }//END OF if(str = "RELAY")

	  }//END OF while(1)	  
	}//END OF fork()
    }//END OF while()
}//END OF main()
