//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>


#include <time.h>   
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>

using namespace std;

ifstream words_LM("data/LM-word");
ifstream curse_file("data/cursed_word-interactive.txt");
ifstream not_good_words_for_rhyme("data/impossible_for_rhyme");
ifstream words_CMU("data/rhythm_words");
ifstream distance_file("data/distance_all");
ofstream good_words_file("data/goodwords");


//set<string> good_words_set;
map<string,string> goodword2pattern;


const long long max_size = 2000;         // max length of strings
const long long N = 500;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

const int max_all_words=1000000;
const int max_good_words=100000;
const int max_num_hidden=250;

int num_words, num_hidden;



map<string,int> all_words;
float vector_all_words[max_all_words][max_num_hidden];

int num_good_words=1;
string good_words[max_all_words];
map<string,int> num2goodwords;
float vector_good_words[max_all_words][max_num_hidden];

float distance_all[max_good_words][N];

void fill_good_words(){
	map<string,string> temp_good_words_pattern;

	string word,pattern;
	while(words_CMU>>pattern>>word){
		temp_good_words_pattern[word]=pattern;
	}
	while(words_LM>>word){
		if(temp_good_words_pattern.find(word)!=temp_good_words_pattern.end()){
			//good_words_set.insert(word);
			goodword2pattern[word]=temp_good_words_pattern[word];
		}
	}
	while(curse_file>>word){
		map<string,string>::iterator it=goodword2pattern.find(word);
		if (it!=goodword2pattern.end()){
			goodword2pattern.erase(it);
		}
	}
	while(not_good_words_for_rhyme>>word){
		map<string,string>::iterator it=goodword2pattern.find(word);
		if (it!=goodword2pattern.end()){
			goodword2pattern.erase(it);
		}
	}

	//cout<<good_words_set.size()<<endl;
}
char start_pattern(string pattern){
	return pattern[0];
}
char end_pattern(string pattern){
	return pattern[pattern.length()-1];
}

bool is_good_word_phrase(string s){

	char c;
	string current="";
	char last='_';
	for(int i=0;i<s.length();i++){
		c=s[i];

		if(c!='_'){
			current+=c;
		}
		else{
			if(goodword2pattern.find(current)==goodword2pattern.end())
				return false;
			else{
				string pattern=goodword2pattern[current];
				if(start_pattern(pattern)==last)
					return false;
				last=end_pattern(pattern);

			}
			current="";
		}
	}
	if(goodword2pattern.find(current)==goodword2pattern.end())
		return false;
	string pattern=goodword2pattern[current];
	if(start_pattern(pattern)==last)
					return false;
	return true;

}

void read_vectors(FILE* f){
  	fscanf(f, "%lld", &num_words);
  	fscanf(f, "%lld", &num_hidden);

  	//cout<<num_words<<" "<<num_hidden<<endl;
  	//num_words=100;
  	for(int num=1;num<num_words;num++){
  		string current_word="";
  		char temp=fgetc(f);
  		while(1){
  			char c=fgetc(f);
  			if (feof(f) || (c == ' ')) break;
  			current_word+=c;
  		}
  		all_words[current_word]=num;
  		float M;
  		float len = 0;
   		for (int a = 0; a < num_hidden; a++){
    		fread(&M , sizeof(float), 1, f);
    		vector_all_words[num][a]=M;
    		len+=(M*M);
		}
		len = sqrt(len);
    	for (int a = 0; a < num_hidden; a++) 
    		vector_all_words[num][a] /= len;
    	if(is_good_word_phrase(current_word)){
  			good_words[num_good_words]=current_word;
  			num2goodwords[current_word]=num_good_words;
  			for (int a = 0; a < num_hidden; a++) 
    			vector_good_words[num_good_words][a] =vector_all_words[num][a];
    		good_words_file<<num_good_words<< " "<< current_word<<endl;
    		num_good_words++;
  		}

  	}

  	cout<<num_good_words<<endl;


 
    fclose(f);
    return;
}

void fill_distance_all(){
	string s;
	int num;
	float distance;
	while(getline(distance_file,s)){
		stringstream ss(s);
		ss>>s;
		num=num2goodwords[s];
		for(int i=0;i<N;i++){
			ss>>distance>>s;
			distance_all[num][i]=distance;
		}
	}
	
}

bool is_reverse_rank(float cur_dist,int word){

	if(cur_dist<=distance_all[word][N-1])
		return false;
	else
		return true;
	/*float distance[N];
	int num_closest[N];
	float vec[max_num_hidden];
	for(int h=0;h<num_hidden;h++){
		vec[h]=vector_good_words[word][h];
	}

	
	int count=0;
	for(int w2=0;w2<num_good_words;w2++){

		float dist=0;
		for(int h=0;h<num_hidden;h++)
			dist+=vec[h]*vector_good_words[w2][h];
		
		if(cur_dist<dist)
			count++;
		if(count>threshold_num){
			cout<<count<<endl;
			return false;	
		}
	}
	cout<<count<<endl;
	return true;


	
	/*for(int n=N-2;n>=0;n--)
		if(dist<distance_all[word][n])
			return n+1;
	return 0;*/
}
void find_related_words(string message, string outputname){
	string s;
	stringstream ss(message);
	float vec[max_num_hidden];
	float len=0;
	for(int i=0;i<num_hidden;i++)
		vec[i]=0;
	while(ss>>s){
		int num=all_words[s];
		if(num==0){
			cout<<"Out of dictionary word!"<<endl;
			continue;
		}else{
			for(int i=0;i<num_hidden;i++){
				vec[i]+=vector_all_words[num][i];
				len+=(vec[i]*vec[i]);
			}
		}

	}
	len = sqrt(len);
	for(int i=0;i<num_hidden;i++)
		vec[i]/=len;
	float distance[N];
	int num_closest[N];

	for(int i=0;i<N;i++){
		num_closest[i]=-1;
		distance[i]=-10000;
	}

	for(int i=0;i<num_good_words;i++){

		float dist=0;
		for(int h=0;h<num_hidden;h++)
			dist+=vec[h]*vector_good_words[i][h];
		int n;
		if(dist<=distance[N-1])
			continue;
		for(n=N-2;n>=0;n--){
			if(dist<=distance[n])
				break;
			else{
				distance[n+1]=distance[n];
				num_closest[n+1]=num_closest[n];
			}
		}

		distance[n+1]=dist;
		num_closest[n+1]=i;
	}

	ofstream out(outputname.c_str());
	
	for(int i=0;i<N;i++){
		if(is_reverse_rank(distance[i],num_closest[i]))
			out<<num_closest[i]<<" "<<distance[i]<<endl;
	}
}

void dostuff(int, int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}




int main(int argc, char **argv) {
	signal(SIGCHLD,SIG_IGN);
	int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     srand (time(NULL));

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

     //init_function();
    FILE *f;
  	char file_name[100];
  	strcpy(file_name, argv[2]);
  	f = fopen(file_name, "rb");
  	if (f == NULL) {
    	printf("Input file not found\n");
    	return -1;
  	}
  	fill_good_words();
  	read_vectors(f);
  	fill_distance_all();
     
    cout<<"ready to operate ...."<<endl;
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
	 int rand_number = rand() % 10000;
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd,rand_number);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);


  //ifstream in(argv[1]);
  
  
  //find_related_words();

  

  return 0;
}

void dostuff (int sock, int rand_number)
{
   int n;
   char buffer[256];
   string topic_words[10];
   bool used_words[10];
   int num_words=0;

   stringstream ss("");
   ss<<"data/related_words_"<<rand_number;


   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   
   string str(buffer);
   string output=ss.str();
   find_related_words(str,output);
   
     
	//n = write(sock,"I got your message",18);
	n = send(sock, output.data(), output.size(), 0);
   if (n < 0) error("ERROR writing to socket");
}
