#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<math.h>
#include<set>

using namespace std;
ifstream words_LM("data/LM-word");
ifstream curse_file("data/cursed_word-auto.txt");
ifstream words_CMU("data/rhythm_words");
ofstream goodwords("data/goodwords-auto");
ofstream fsa;

//ofstream fsa("data/complete_fsa_without_rhyme.fsa");
//int number_of_words=20000;

int poemline;

map<string,pair<bool,int> > cmu_patterns;
set<string> lm;
set<string> curse;


pair<bool,int> convert_pattern(string pattern,string word){
	if(pattern.find("0#0")!=string::npos || pattern.find("1#1")!=string::npos){
		cout<<"error!!!!!!!!!!!!!!!!!!!!!! "<<pattern<<endl; 
		return make_pair(0,-1);
	}
	if(pattern[0]=='0')
		return make_pair(0,(pattern.length()/2)+1);
	if(pattern[0]=='1')
		return make_pair(1,(pattern.length()/2)+1);	
	cout<<"errror  None of them............"<<pattern <<"   "<<word<<endl;
	return make_pair(0,-1);
}

void read_cmu(){
	string pattern,word;
	while(words_CMU>>pattern>>word){
		cmu_patterns[word]=convert_pattern(pattern,word);
	}
	
}

int check_state_pattern(int start_state,bool spattern, int lpattern){
	//poem pattern : 0 1 0 1   so even ->(0) odd  odd ->(1) even
	if(spattern==0 && start_state%2==1)
		return -1;
	if(spattern==1 && start_state%2==0)
		return -1;
	int end_state=start_state+lpattern;
	/*if(end_state>140)
		return -1;*/  //That is not necessary.

	if(end_state%10==0 ){
		if(lpattern<10)
			return -1;  // no rhyme words for preprocessing part
		else{
			cout<<"errrror ......strange!!!!!!!!!!!  "<<lpattern<<endl;
			return -1;
		}
	}
	if(start_state/10 != end_state/10){
		//Two different lines of peom
		return -1;
	}
	else{
		return end_state;
	}	
	
}
void write_fsa_reverse_edge(int start,int end,string word){
	//(3 (7 "suicide"))
	if(end%10!=0)
		fsa<<"("<<end<<" ("<<start<<" \""<<word<<"\"))"<<endl;
	else
		fsa<<"("<<end<<"-t ("<<start<<" \""<<word<<"\"))"<<endl;
}

int main(int argc, char** argv){
	string poemline_s=argv[1];
	string s;
	/*while(words_LM>>s)
		lm.insert(s);*/
		
	stringstream ss("");
	ss<<poemline_s;
	ss>>poemline;
	fsa.open(("data/complete_fsa_without_rhyme-"+poemline_s+"-auto.fsa").c_str());
	while(curse_file>>s)
		curse.insert(s);
		
	read_cmu();
	fsa<<"0"<<endl;
	for(int i=poemline*10;i>=10;i-=10){
		fsa<<"("<<i<<" ("<<i<<"-t \""<<"."<<"\"))"<<endl;
		if(poemline!=2){
			if(!(i==40 || i==80 || i==120 || i==140 )){
				fsa<<"("<<i<<" ("<<i<<"-t \""<<","<<"\"))"<<endl;
				fsa<<"("<<i<<" ("<<i<<"-t \""<<"!"<<"\"))"<<endl;
				//fsa<<"("<<i<<" ("<<i<<"-t \""<<"?"<<"\"))"<<endl;
			}
		}else{
			if(!(i==20)){
				fsa<<"("<<i<<" ("<<i<<"-t \""<<","<<"\"))"<<endl;
				fsa<<"("<<i<<" ("<<i<<"-t \""<<"!"<<"\"))"<<endl;
				//fsa<<"("<<i<<" ("<<i<<"-t \""<<"?"<<"\"))"<<endl;
			}
		}
		//cout<<"("<<i<<" ("<<i<<"-t \""<<"?"<<"\"))"<<endl;
		//cout<<"("<<i<<" ("<<i<<"-t \""<<"!"<<"\"))"<<endl;
	}

	string word;
	while( words_LM>>word){
		//number_of_words--;
		//cout<<word<<endl;
		if(cmu_patterns.find(word)!=cmu_patterns.end() && curse.find(word)==curse.end()){
			goodwords<<word<<endl;
			
			for(int i=0;i<poemline*10;i++){
				int r=check_state_pattern(i,cmu_patterns[word].first,cmu_patterns[word].second);
				if(r!=-1){
					write_fsa_reverse_edge(i,r,word);
				}
			}
		}
		
		
		
	}
	
}
