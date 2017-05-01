#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <math.h>
#include <set>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <algorithm>
#include <vector>
#include <math.h>



using namespace std;

ifstream wordnumfile("data/goodwords");
ifstream rhythm_rhyme_inf("data/rhythm_and_rhyme_words");
ifstream general_words("data/general_words");
ifstream more_general_words("data/general_words_for_each_rhyme");
ifstream similar_pronouncation("data/similar_pronouncation");
ifstream slant_inf("data/process_for_slant");
ifstream proscorefile("data/similarity_score");




const int max_words=50000;
const int max_candidates=5000;
const int max_rhymes=120;
//bool can_rhyme[max_words][max_words]; //0: not at all 1: slant 2:strict

map<string,string> word2word_similarpro;
string num2word[max_words];
int num2last[max_words];
map<string,int> word2num;
int countword=0;
double slant_rhyme_penalty=0.7;
double general_words_penalty=0.1;
double slant_rhyme_unsimilarity_penalty=0.5;
double slant_rhyme_not_equal_vowel_penalty=0.3;
set<int> can_rhyme[max_words];   // I am not sure if I need it or not. 
map<int,double> rhyme_score[max_words];  

vector<string> num2slantallinf[max_words];
string num2srhyme[max_words];
string num2rhyme[max_words];
string num2rhythm[max_words];

map < pair<string,string> , double> proscore;

set<int> generalwords;
set<int> very_general;
map<int,double> generalwords_score;
set<int> relatedwords;
map<int,double> relatedwords_score;
map<string, set<int> > srhyme2word_g;


vector< pair<int,int> > candidate2(max_candidates);
vector<double> score_candidate2(max_candidates,0.0);
vector<double> score_candidate2_acc(max_candidates,0.0);
int currentnum_candidate2=0;

int final_rhyme[max_rhymes];
int last_final_rhyme[max_rhymes];
int num_chosen_rhyme=0;


void dostuff(int,int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

bool is_rhythm_pattern(string pattern){
	if (pattern[pattern.length()-1]=='*')
		return false;
	return true;
}
string find_last_of(string phrase,char ch){
	string cur_word="";
	
	for(int i=phrase.length()-1;i>=0;i--){
		char c=phrase[i];
		if(c==ch)
			return cur_word;
		else
			cur_word=(c+cur_word);
	}
	return cur_word;
}
string last_word(string phrase){
	string cur_word="";
	
	for(int i=phrase.length()-1;i>=0;i--){
		char c=phrase[i];
		if(c=='_')
			return cur_word;
		else
			cur_word=(c+cur_word);
	}
	return cur_word;	
}
string first_word(string phrase){
	string cur_word;
	
	for(int i=0;i<phrase.length();i++){
		char c=phrase[i];
		if(c=='_')
			return cur_word;
		else
			cur_word+=c;
	}
	return cur_word;	
}
void fill_similarity_score(){
	string s1,s2;
	double d;
	while(proscorefile>>s1>>s2>>d){
		pair<string,string> p1=make_pair(s1,s2);
		pair<string,string> p2=make_pair(s2,s1);
		proscore[p1]=d;
		proscore[p2]=d;
	}	
}
double similar_phoneme(string s1,string s2){
	pair<string,string> p=make_pair(s1,s2);
	if(proscore.find(p)!=proscore.end())
		return proscore[p];
	else
		return -100;	
}
int count_vowels(string s){
	string token="";
	int count =0;
	for(int i=0;i<s.length();i++){
		char c=s[i];
		if(c=='_'){
			if(token[0]=='A' || token[0]=='E' || token[0]=='I' || token[0]=='O' || token[0]=='U' )
				count++;
			token="";
		}
		else
			token+=c;
	}
	if(token[0]=='A' || token[0]=='E' || token[0]=='I' || token[0]=='O' || token[0]=='U' )
				count++;
	
	return count;	
}
double is_slant_rhyme(vector<string> s1, vector<string> s2){
	double threshold=-0.6;
	
	
	string a1=s1[0];
	string v1=s1[1];
	string x1=s1[2];
	string w1=s1[3];
	string c1=s1[4];
	
	string a2=s2[0];
	string v2=s2[1];
	string x2=s2[2];
	string w2=s2[3];
	string c2=s2[4];
	
	if(v1!=v2)
		return 0;
	if(w1!=w2)
		return 0;
	if(a1!="_" && a1==a2)
		return 0;
	if(c1!=c2)
		return 0;
	if(x1=="_" || x2=="_")
			return 0;
	if(first_word(x1)==x1 && first_word(x2)==x2)

			if(similar_phoneme(x1,x2)>threshold)
				return 1;
			else
				return slant_rhyme_unsimilarity_penalty;
	if(count_vowels(x1)!=count_vowels(x2))
				return slant_rhyme_not_equal_vowel_penalty;
	
	string p1=first_word(x1);
	string q1=last_word(x1);
	
	string p2=first_word(x2);
	string q2=last_word(x2);
	
	string pq1=q1;
	string pq2=q2;
	string qp1=p1;
	string qp2=p2;
	
	if(q1==x1){
		pq1="*_";
		qp1="_*";
	}
	if(q2==x2){
		pq2="*_";
		qp2="_*";
	}
	
	if(p1==p2 && similar_phoneme(pq1,pq2)> threshold)
		return 1;
	if(q1==q2 && similar_phoneme(qp1,qp2)> threshold)
		return 1;
	return slant_rhyme_unsimilarity_penalty;
}

void fill_slantpronouncation(){
	string word;
	string a,v,x,w,c;

	map<string,vector<string> >slantallinf_temp; 
	while(slant_inf>>word>>a>>v>>x>>w>>c){
		//cout<<word<<" "<<a<<" "<<v<<endl;
		vector<string> vv;
		
		vv.push_back(a);
		vv.push_back(v);
		vv.push_back(x);
		vv.push_back(w);
		vv.push_back(c);
		slantallinf_temp[word]=vv;
	}

	for(int i=1;i<countword;i++){
		string word=num2word[i];
		string last=last_word(word);
		if(slantallinf_temp.find(last)==slantallinf_temp.end())
			continue;
		vector<string> v=slantallinf_temp[last];
		num2slantallinf[i]=v;
		string srhyme=v[1]+"#"+v[3]+"#"+v[4];
		num2srhyme[i]=srhyme;
	}	
}
void fill_exact_pronouncation(){
	string line;
	while(getline(similar_pronouncation,line)){
		stringstream ss(line);
		string pro,word;
		ss>>pro;
		while(ss>>word){
			word2word_similarpro[word]=pro;
		}
	}
}

void fill_rhyme(){
	map<string,string> rhyme_temp;
	map<string,string> rhythm_temp;
	string pattern,word;
	while(rhythm_rhyme_inf>>pattern>>word){
		if(!is_rhythm_pattern(pattern)){
			string rhyme=find_last_of(pattern,'#');
			rhyme_temp[word]=rhyme;
		}
		else{
			rhythm_temp[word]=pattern;
		}
	}

	for(int i=1;i<countword;i++){
		string word=num2word[i];
		string last=last_word(word);
		if(rhyme_temp.find(last)!=rhyme_temp.end()){
			string rhyme=rhyme_temp[last];
			num2rhyme[i]=rhyme;
			if(num2srhyme[i]=="")
				num2srhyme[i]=rhyme;
		}
		string rhythm="";
		string current_word="";
		for(int j=0;j<word.length();j++){
			char c=word[j];
			if(c!='_')
				current_word+=c;
			else{
				if(rhythm_temp.find(current_word)==rhythm_temp.end()){
					cout<<"error .... I can't find rhythm pattern for "<<current_word<<" in "<<word<<endl;
					continue;
				}
				rhythm+=(rhythm_temp[current_word]+"#");
				current_word="";
			}
		}

		if(rhythm_temp.find(current_word)==rhythm_temp.end()){
			cout<<"error .... I can't find rhythm pattern for "<<current_word<<" in "<<word<<endl;
			continue;
		}
		rhythm+=(rhythm_temp[current_word]);

		num2rhythm[i]=rhythm;
		
		
	}
}

void readwordnum(){
	int num;
	string word;
	
	string temp_last[max_words];

	while(wordnumfile>>num>>word){
		num2word[num]=word;
		word2num[word]=num;
		string last=last_word(word);
		temp_last[num]=last;
		//temp_last.insert(last);
	}
	int num_sofar=num;
	for(int i=1;i<=num_sofar;i++){
		string last=temp_last[i];
		int id=word2num[last];
		if(id==0){
			id=num+1;
			num++;
			word2num[last]=id;
			num2word[id]=last;
		}
		num2last[i]=id;
	}
	countword=num; //I think num+1
	


	cout<<countword<<endl;
}

void fill_can_rhyme(){
	map<string, set<int> > temp_srhyme2words;

	for(int i=1;i<countword;i++){
		string s=num2srhyme[i];
		if(s=="")
			continue;
		temp_srhyme2words[s].insert(i);
	}
	int count=0;
	for(auto it=temp_srhyme2words.begin();it!=temp_srhyme2words.end();++it){
		string srhyme=it->first;
		set<int> candidates=it->second;
		for(auto it1=candidates.begin();it1!=candidates.end();++it1){
			int word1=*it1;
			string last1=last_word(num2word[word1]);
			string pro1=word2word_similarpro[last1];
			for(auto it2=next(it1,1);it2!=candidates.end();++it2){
				int word2=*it2;
				string last2=last_word(num2word[word2]);
				if(last1==last2)
					continue;
			    string pro2=word2word_similarpro[last2];	
				if(pro1!="" && pro1==pro2)
					continue;

				can_rhyme[word1].insert(word2);
				can_rhyme[word2].insert(word1);
				if(num2rhyme[word1]==num2rhyme[word2]){
					rhyme_score[word1][word2]=1;
					rhyme_score[word2][word1]=1;
				}

				else{
					double sim=is_slant_rhyme(num2slantallinf[word1],num2slantallinf[word2]);
					if(sim>0){
						rhyme_score[word1][word2]=sim*slant_rhyme_penalty;
						rhyme_score[word2][word1]=sim*slant_rhyme_penalty;
					}
				}
				count++;
			}
		}
	}
	cout<<count<<endl;

}
void fill_general_words(){
	string word;
	double p;
	while(general_words>>word){
		int n=word2num[word];
		if(n!=0){
			generalwords.insert(n);
			very_general.insert(n);
			generalwords_score[n]=0.00001;
			string srhyme=num2srhyme[n];
			if(srhyme!="")
				srhyme2word_g[srhyme].insert(n);

		}
	}

	//int counter=500;
	while(more_general_words>>word>>p){
		//if(counter<0)
		//	break;
		int n=word2num[word];
		if(n!=0){
			generalwords.insert(n);
			generalwords_score[n]=p*0.001;
			string srhyme=num2srhyme[n];
			if(srhyme!="")
				srhyme2word_g[srhyme].insert(n);
			//counter--;
		}
	}
}

void initial_server(){
	readwordnum();
	fill_exact_pronouncation();
	fill_slantpronouncation();
	fill_rhyme();
	fill_can_rhyme();
	fill_general_words();
	srand(time(NULL));
}
void initial_client(){
	relatedwords=generalwords;
	relatedwords_score=generalwords_score;
	currentnum_candidate2=0;
	num_chosen_rhyme=0;



}

int find_index(vector<double> acc,double p,int max_index){
	for(int i=0;i<max_index;i++){
			if(p<=acc[i])
				return i;
	}
	return -1;
}
pair<int,int> choose_candidate2(){

	double p = ((double) rand() / (RAND_MAX))*score_candidate2_acc[currentnum_candidate2-1];
	int r=find_index(score_candidate2_acc,p,currentnum_candidate2);

	return candidate2[r];
}

bool check_consistency(pair<int,int>  a){
	int a1=a.first;
	int a2=a.second;



	for(int i=0;i<num_chosen_rhyme;i++){
		if(final_rhyme[i]==a1 || final_rhyme[i]==a2)
			return false;
	}
	int l1=num2last[a1];
	int l2=num2last[a2];


	for(int i=0;i<num_chosen_rhyme;i++){
		if(last_final_rhyme[i]==l1 || last_final_rhyme[i]==l2)
			return false;
	}

	double b = rand()%2;
	if(b==0){
		final_rhyme[num_chosen_rhyme]=a1;
		last_final_rhyme[num_chosen_rhyme]=l1;
		num_chosen_rhyme++;
		final_rhyme[num_chosen_rhyme]=a2;
		last_final_rhyme[num_chosen_rhyme]=l2;
		num_chosen_rhyme++;
	}
	else{
		final_rhyme[num_chosen_rhyme]=a2;
		last_final_rhyme[num_chosen_rhyme]=l2;
		num_chosen_rhyme++;
		final_rhyme[num_chosen_rhyme]=a1;
		last_final_rhyme[num_chosen_rhyme]=l1;
		num_chosen_rhyme++;
	}
	return true;

}

void update_probabilities(pair<int,int> a){
	int a1=a.first;
	int a2=a.second;
	int la1=num2last[a1];
	int la2=num2last[a2];

	for(int i=0;i<currentnum_candidate2;i++){
		int c1=candidate2[i].first;
		int c2=candidate2[i].second;
		int lc1=num2last[c1];
		int lc2=num2last[c2];


		double score=0;
		if(lc1==la1 || lc1==la2 || lc2==la1 || lc2==la2){
			score=0;
		}
		else
			score=score_candidate2[i];
		score_candidate2[i]=score;
		if(i==0)
			score_candidate2_acc[i]=score;
		else
			score_candidate2_acc[i]=score+score_candidate2_acc[i-1];
	}
}

void remove_the_pair(pair<int,int> a){
	int a1=a.first;
	int a2=a.second;
	
	for(int i=0;i<currentnum_candidate2;i++){
		int c1=candidate2[i].first;
		int c2=candidate2[i].second;
		double score=0;
		if( (c1==a1 && c2==a2) || (c1==a2 && c2==a1)){
			score=0;
		}
		else
			score=score_candidate2[i];
		score_candidate2[i]=score;
		if(i==0)
			score_candidate2_acc[i]=score;
		else
			score_candidate2_acc[i]=score+score_candidate2_acc[i-1];
	}
}

void write_rhymes(string infile,string outfile,string encourage, string inf_file, int num2){
	ifstream in(infile.c_str());
	ofstream out(outfile.c_str());
	ofstream enc(encourage.c_str());
	ofstream log(inf_file.c_str());

	int wordnum;
	double score;
	while(in>>wordnum>>score){
		relatedwords.insert(wordnum);
		relatedwords_score[wordnum]=score;
		enc<<num2word[wordnum]<<endl;
	}
	//cout<<"done reading words"<<endl;

	map<string, set<int> > srhyme2word;
	for(auto it=relatedwords.begin();it!=relatedwords.end();++it){
		int n=*it;
		string srhyme=num2srhyme[n];
		if(srhyme!="")
			srhyme2word[srhyme].insert(n);
	}
	//cout<<"done filing rhymes"<<endl;

	//int log_total_words=0;
	for(auto it=srhyme2word.begin();it!=srhyme2word.end();++it){
		string rhyme=it->first;
		stringstream logline("");
		int log_last=-1;
		bool write_log_line=false;
		logline<<rhyme<<":  "; 
		set<int> words=it->second;
		for(auto it1=words.begin();it1!=words.end();++it1){
			int w1=*it1;
			double s1=relatedwords_score[*it1];
			bool general1=0;
			if(generalwords.find(w1)!=generalwords.end()){
				//cout<<"a general wordsss..."<<endl;
				s1=s1*general_words_penalty;
				general1=1;
			}
			if(general1!=1){
				logline<<num2word[w1]<<", ";
				//log_total_words++;
				int l=num2last[w1];
				if(log_last==-1){
					log_last=l;
				}
				else if(log_last!=l)
					write_log_line=true;
			}
			for(auto it2=next(it1,1);it2!=words.end();++it2){
				int w2=*it2;
				//cout<<w1<<" "<<num2word[w1]<<" "<<w2<<" "<<num2word[w2]<<endl;
				if(can_rhyme[w1].find(w2)==can_rhyme[w1].end())
					continue;
				double s2=relatedwords_score[*it2];
				bool general2=0;
				if(generalwords.find(w2)!=generalwords.end()){
					s2=s2*general_words_penalty;
					general2=1;
				}
				if(general1 && general2 && !(very_general.find(w1)!=very_general.end() && very_general.find(w2)!=very_general.end() ))
					continue;
				candidate2[currentnum_candidate2]=make_pair(w1,w2);
				//double score=pow(s1*s2,2)*rhyme_score[w1][w2];
				double score=pow(s1*s2,2);
				score_candidate2[currentnum_candidate2]=score;
				if(currentnum_candidate2==0)
					score_candidate2_acc[currentnum_candidate2]=score;
				else
					score_candidate2_acc[currentnum_candidate2]=score_candidate2_acc[currentnum_candidate2-1]+score;
				//cout<<currentnum_candidate2<<endl;
				currentnum_candidate2++;

				if(currentnum_candidate2>=(max_candidates-10))
					break;


			}
		}

		if(write_log_line)
		//if(log_last!=-1)
			log<<logline.str()<<endl;
	}
	//log<<endl<<log_total_words<<endl;

	cout<<currentnum_candidate2<<endl;
	/*double sum=score_candidate2_acc[currentnum_candidate2-1];
	for(int i=0;i<currentnum_candidate2;i++){
		score_candidate2[i]/=sum;
		score_candidate2_acc[i]/=sum;
	}*/
	
	int bad_counter=0;
	while(num2>0){
		//cout<<"num candidate2: "<<currentnum_candidate2<<endl;
		pair<int,int> pair2=choose_candidate2();
		bool result=check_consistency(pair2);
		cout<<pair2.first<<" "<<pair2.second<<" "<<result<<endl;
		if(result){
			bad_counter--;
			num2--;
			if(num2!=0)
				update_probabilities(pair2);
		}
		/*else{

		}*/

		if(!result){
			bad_counter++;
			if(bad_counter>10)
				remove_the_pair(pair2);
			cout<<"not applicable   "<< num2word[pair2.first]<<" "<<num2word[pair2.second]<<endl;
			cout<<currentnum_candidate2<<"      "<<score_candidate2_acc[currentnum_candidate2-1]<<endl;
		}

		if(bad_counter>1000){
			break;
		}
		//cout<<num2<<endl;

	}

	for(int i=0;i<num_chosen_rhyme;i++){
			out<<num2rhythm[final_rhyme[i]]<<" "<<num2word[final_rhyme[i]]<<endl;
	}

}



int main(int argc, char **argv) {
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

     //init_function();
     initial_server();
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
	   cout<<"pid"<<pid<<"\n";
             close(sockfd);
             dostuff(newsockfd,rand_number);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
  return 0;
}

void dostuff (int sock,int rand_number)
{

	initial_client();
	
   int n;
   char buffer[256];
   stringstream ss_out("");
   ss_out<<"data/rhyme_file_"<<rand_number;


   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   
   string str(buffer);
   stringstream ss_input(str);
   
   string input_address;
   string encourage_add;
   string log_address;
   int num_pair2;
   ss_input>>input_address>>encourage_add>>log_address>>num_pair2;

   string output_address=ss_out.str();
   write_rhymes(input_address,output_address,encourage_add,log_address,num_pair2);
   
     
	//n = write(sock,"I got your message",18);
	n = send(sock, output_address.data(), output_address.size(), 0);
   if (n < 0) error("ERROR writing to socket");
}
