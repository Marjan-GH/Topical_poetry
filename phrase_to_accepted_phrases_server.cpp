/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <math.h>
#include <set>
#include <signal.h>
using namespace std;

set<string> words_and_phrases;
ifstream words("data/accepted_words_and_phrases");


void dostuff(int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void init_function(){
  string s;
  while(words>>s)
    words_and_phrases.insert(s);
}

int main(int argc, char *argv[])
{
  signal(SIGCHLD,SIG_IGN);
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

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
     clilen = sizeof(cli_addr);
     init_function();
     cout<<"ready to operate ...."<<endl;
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer[256];
   string topic_words[10];
   bool used_words[10];
   int num_words=0;
      
   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   string s;
   string str(buffer);
   stringstream message(str);
   stringstream result("");
  while(message>>s && num_words<10){
    if(words_and_phrases.find(s)!=words_and_phrases.end()){
      topic_words[num_words]=s;
      used_words[num_words]=false;
      num_words++;
    }   
  }
  for(int i=0;i<num_words-1;i++){
    string s=topic_words[i]+"_"+topic_words[i+1];
    if(words_and_phrases.find(s)!=words_and_phrases.end()){
      result<<s<<" ";
      used_words[i]=true;
      used_words[i+1]=true;
      
    }
    if(!used_words[i])
      result<<topic_words[i]<<" ";
    
  }
  
  if(!used_words[num_words-1])
    result<<topic_words[num_words-1]<<" ";
  //cout<<endl;
  
  s=result.str();
  string output=s.substr(0,s.length()-1);

  cout<<output<<endl;
  //n = write(sock,"I got your message",18);
  n = send(sock, output.data(), output.size(), 0);
   if (n < 0) error("ERROR writing to socket");
}
